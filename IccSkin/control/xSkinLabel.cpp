// Label.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CxSkinLabel, CStatic)
	//{{AFX_MSG_MAP(CxSkinLabel)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CxSkinLabel Version 1.2
//
// From now on I'll try to keep a log of fixes and enhancements...
// 
// The new feature were added due to the response of people.
// All I ask is to all you programmers out there, is if you add, fix or
// enhance this code, sent me a copy and I'll send the copy on to www.codeproject.com
//
// Happy Software Engineer :)
// 
// New features include:
//
// A. Support for 3D Fonts
// B. Support for background transparency
// C. More comments provided
// D. If alignment is 'centered' and the window text is seperated by '\r\n'
//	  the will be centered accordingly - requested by someone @ nasa ;)
// E. Support for font rotation.
// F. Respond to System Color Change
// G. OnPaint improved performance - using Double Buffering Technique
//
// Thanks to:
// Mark McDowell	- For suggestion on 'Increasing the flexibility of "hypertext" setting...'
// Erich Ruth		- For suggestion on 'Font Rotation'
//

/////////////////////////////////////////////////////////////////////////////
// CxSkinLabel Version 1.3
//
// A. Added SS_LEFTNOWORDWRAP to include wordwrap
// B. Fix repainting problem 
// C. Fix SetBkColor
// D. Added SS_CENTER

// Thanks to:
// Marius						- Added styling problem.
// Azing Vondeling & Broker		- Spotting painting Problem.
// Mel Stober					- Back Color & SS_CENTER
// 
/////////////////////////////////////////////////////////////////////////////
// CxSkinLabel Version 1.4
//
// A. Fix to transparency mode
// B. Added new SetText3DHiliteColor to change the 3D Font face color - default is white.
// 
// Thanks to:
// michael.groeger				- Spotting Transparency with other controls bug.
//
//
/////////////////////////////////////////////////////////////////////////////
// CxSkinLabel Version 1.5
//
// A. Sanity handle check
// B. Support Interface Charset
// C. Check compilition with _UNICODE
// D. Fix hyperlink feature
// E. Support default Dialog Font
// F. Inclusion of SS_OWNERDRAW via control creation and subclassing
// G. Modification to Text aligmnent code
// H. New background gradient fill function
// 
// Thanks to:
// Steve Kowald				- Using null handles 
// Alan Chan				- Supporting International Windows
// Dieter Fauth				- Request for default Dialog font
// Herb Illfelder			- Text Alignment code
// 
/////////////////////////////////////////////////////////////////////////////
// CxSkinLabel Version 1.6
// Jeroen Roosendaal		- SetFont suggestion
// Laurent					- Spotting SelectObject bugs
// Bernie					- Fix PreCreateWindow bug
// Jignesh I. Patel			- Added expanded tabs feature
// Jim Farmelant 			- Fix SetText crash


//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::CxSkinLabel
//
// Description:		Default contructor
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////

#include "Define.h"

CxSkinLabel::CxSkinLabel()
	:m_crText(COLOR_BLACK)
	,m_hBackBrush(NULL)
	,m_crHiColor(0)
	,m_crLoColor(0)
	,m_bTimer(false)
	,m_bState(false)
	,m_bTransparent(false)
	,m_Link(LinkNone)
	,m_hCursor(NULL)
	,m_Type(None)
	,m_bFont3d(false)
	,m_bNotifyParent(false)
	,m_bToolTips(false)
	,m_bRotation(false)
	,m_fillmode(Normal)
	,m_cr3DHiliteColor(COLOR_WHITE)
	,m_hwndBrush(::CreateSolidBrush(GetSysColor(COLOR_3DFACE)))
	,m_strText(_T(""))
	,m_bVertCenter(false)
	,m_bEllipsis(false)
	,m_nId(0)
	,m_nLineSpace(0)
	,m_clrTextUnlink(COLOR_DISABLE)
	,m_bHighLight(false)
	,m_bWordWrap(false)
{

}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::~CxSkinLabel
//
// Description:		
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				15092000	1.5		Handle Check
//////////////////////////////////////////////////////////////////////////
CxSkinLabel::~CxSkinLabel()
{
	// Clean up
	if(m_font.m_hObject)
		m_font.DeleteObject();
	
	if(m_hwndBrush)
		::DeleteObject(m_hwndBrush);

	if (m_hBackBrush)
		::DeleteObject(m_hBackBrush);

}

void CxSkinLabel::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	
	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::ReconstructFont
//
// Description:		Helper function to build font after it was changed
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			PROTECTED
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
void CxSkinLabel::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);

	ASSERT(bCreated);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::OnPaint
//
// Description:		Handles all the drawing code for the label
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			Called by Windows... not by USER
//					Probably needs tiding up a some point.
//					Different states will require this code to be reworked.
//
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
// NT ALMOND				15092000	1.5		Handle Check
// NT ALMOND				15092000	1.5		Alignment mods
// NT ALMOND				15092000	1.5		Gradient Fill Mode
// NT ALMOND				02072002	1.6		Fill SelectObject bugs
// NT ALMOND				02072002	1.6		Added to expand tabs
//////////////////////////////////////////////////////////////////////////

void CxSkinLabel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DWORD dwFlags = 0;

	CRect rc;
	GetClientRect(rc);
	CString strText;
	
	strText = m_strText;
	
	//GetWindowText(strText);

	CBitmap bmp;
		///////////////////////////////////////////////////////
	//
	// Set up for double buffering...
	//
	CDC* pDCMem;
	CBitmap*	pOldBitmap = NULL;

	if (!m_bTransparent)
	{
		pDCMem = new CDC;
		pDCMem->CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
		pOldBitmap = pDCMem->SelectObject(&bmp);
	}
	else
	{
		pDCMem = &dc;
	}

	UINT nMode = pDCMem->SetBkMode(TRANSPARENT);
	
	COLORREF crText = pDCMem->SetTextColor(m_crText);	

	CFont *pOldFont = pDCMem->SelectObject(&m_font);

	// Fill in backgound if not transparent
	if (!m_bTransparent)
	{
		if (m_fillmode == Normal)
		{
			CBrush br;
			if (m_hBackBrush != NULL)
				br.Attach(m_hBackBrush);
			else
				br.Attach(m_hwndBrush);

			pDCMem->FillRect(rc,&br);
			br.Detach();
		}
		else // Gradient Fill
		{
			DrawGradientFill(pDCMem, &rc, m_crLoColor, m_crHiColor, 100);
		}

	}


	// If the text is flashing turn the text color on
	// then to the color of the window background.

	LOGBRUSH lb;
	ZeroMemory(&lb,sizeof(lb));

	// Stop Checking complaining
	if (m_hBackBrush)
		::GetObject(m_hBackBrush,sizeof(lb),&lb);


	// Something to do with flashing
	if (!m_bState && m_Type == Text)
		pDCMem->SetTextColor(lb.lbColor);

	DWORD style = GetStyle();

	switch (style & SS_TYPEMASK)
	{
	case SS_RIGHT: 
		dwFlags = DT_RIGHT | DT_WORDBREAK; 
		break; 

	case SS_CENTER: 
		dwFlags = SS_CENTER | DT_WORDBREAK;
		break;

	case SS_LEFTNOWORDWRAP: 
		dwFlags = DT_LEFT; 
		break;

	default: // treat other types as left
	case SS_LEFT: 
		dwFlags = DT_LEFT | DT_WORDBREAK; 
		break;
	}	

	// Added to expand tabs...
	if(strText.Find(_T('\t')) != -1)
		dwFlags |= DT_EXPANDTABS;

	// If the text centered make an assumtion that
	// the will want to center verticly as well
	if (style & SS_CENTERIMAGE)
	{
		dwFlags = DT_CENTER;

		// Apply 
		if (strText.Find(_T("\r\n")) == -1)
		{
			dwFlags |= DT_VCENTER;

			// And because DT_VCENTER only works with single lines
			dwFlags |= DT_SINGLELINE; 
		}

	}

	//
	// 3333   DDDDD
	//     3  D    D
	//   33   D     D    E F X 
	//     3  D    D
	// 3333   DDDDD
	//
	//
	if (m_bRotation)
	{
		int nAlign = pDCMem->SetTextAlign (TA_BASELINE);

		CPoint pt;
		GetViewportOrgEx (pDCMem->m_hDC,&pt) ;
		SetViewportOrgEx (pDCMem->m_hDC,rc.Width() / 2, rc.Height() / 2, NULL) ;
		pDCMem->TextOut (0, 0, strText) ;
		SetViewportOrgEx (pDCMem->m_hDC,pt.x / 2, pt.y / 2, NULL) ;
		pDCMem->SetTextAlign (nAlign);
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 2008.7.26 멀티라인일때 줄간격을 설정
		CRect rect;
		rect = rc;			//출력좌표
		CString strLine;	//한줄 텍스트
		int nLineCnt = 0;	//라인개수
		CSize cs;			//한줄크기
		CString strAllText = strText;
		strAllText.Replace(_T("\\r\\n"), _T("\r\n"));//ini 스트링 예외처리

		BOOL bBreak = FALSE;

		int nTLineCnt = 0;
		int nStartY = 0;
		//수직중앙정렬
		if(m_bVertCenter)
		{
			int spos = 0;
			while(!bBreak)
			{
				int nPos = strAllText.Find(_T("\r\n"), spos);

				if (nPos >= 0)//줄바꿈 문자가 있다면
				{
					nTLineCnt ++;
					spos = nPos+2;
				}
				else//없으면 나간다.
				{
					nTLineCnt ++;
					break;
				}
			}
			//수직 중앙 정렬이 안되는 오류 수정
			cs = pDCMem->GetTextExtent(strAllText);
			int nTextH = rect.Height();
			if(cs.cy != 0)
				nTextH = nTLineCnt * (cs.cy + m_nLineSpace);
			int cy = rect.Height();
			if( cy > nTextH)
				nStartY = (cy - nTextH) / 2;

		}

		bBreak = FALSE;
		//20160308 줄바꿈 문자 추가
		if(m_bWordWrap)
		{
			TEXTMETRIC tm;
			HDC hdc = ::GetDC(this->m_hWnd);
			GetTextMetrics(hdc, &tm);
			int xsize = tm.tmAveCharWidth;
			int ysize = tm.tmHeight;
			int wrap_pos = rect.Width()/xsize;
			
			int cnt = strAllText.GetLength() / wrap_pos;
			for(int i = 0; i < cnt; i++)
			{
				strAllText.Insert(wrap_pos * (i+1), _T("\r\n"));
			}
		}
		while(!bBreak)
		{
			strLine = _T("");
			int nPos = strAllText.Find(_T("\r\n"));

			if (nPos >= 0)//줄바꿈 문자가 있다면
			{
				strLine = strAllText.Left(nPos);

				cs = pDCMem->GetTextExtent(strLine);
				rect.top = nStartY + nLineCnt* (cs.cy + m_nLineSpace);//출력위치를 계산(라인 개수와 라인높이를
				rect.bottom = rect.top + cs.cy;

				pDCMem->DrawText(strLine, rect, dwFlags);

				nLineCnt++;//라인 개수 증가
				strLine += _T("\r\n");
				strAllText.Replace(strLine, _T(""));//출력한 라인제거
			}
			else//싱글라인 또는 마지막라인
			{
				if (nLineCnt > 0)//마지막 라인
				{
					rect.top = nStartY + (nLineCnt) * (cs.cy + m_nLineSpace);//출력위치를 계산(라인 개수와 라인높이를
					//TRACE("LineCount>0:: top=%d\n",rect.top);
					rect.bottom = rect.top + cs.cy;
				}
				else//싱글라인이 영역을 벗어나면 줄간격 조절을 설정해줘야함
				{
					rect.top = nStartY;
					//TRACE("LineCount<=0:: top=%d\n",rect.top);
					if(m_bEllipsis)
					{
						dwFlags |= DT_WORD_ELLIPSIS;
						dwFlags |= DT_SINGLELINE;
					}
				}
				
				pDCMem->DrawText(strAllText, rect, dwFlags);

				bBreak = TRUE;
			}		
		}


		//pDCMem->DrawText(strText,rc,dwFlags); 원본 소스
		//////////////////////////////////////////////////////////////////////////
		if (m_bFont3d)
		{
			pDCMem->SetTextColor(m_cr3DHiliteColor);

			if (m_3dType == Raised)
				rc.OffsetRect(-1,-1);
			else
				rc.OffsetRect(1,1);

			pDCMem->DrawText(strText,rc,dwFlags);
			m_3dType;

		}
	}

	// Restore DC's State
	pDCMem->SetBkMode(nMode);
	pDCMem->SelectObject(pOldFont);
	pDCMem->SetTextColor(crText);

	if (!m_bTransparent)
	{
		dc.BitBlt(0,0,rc.Width(),rc.Height(),pDCMem,0,0,SRCCOPY);
		// continue DC restore 
		pDCMem->SelectObject ( pOldBitmap ) ;
		delete pDCMem;
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::OnTimer
//
// Description:		Used in conjunction with 'FLASH' functions
//
// INPUTS:          Windows API
// 
// RETURNS:         Windows API
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
void CxSkinLabel::OnTimer(UINT nIDEvent) 
{

	m_bState = !m_bState;

	UpdateSurface();

	CStatic::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::OnSetCursor
//
// Description:		Used in conjunction with 'LINK' function
//
// INPUTS:          Windows API
// 
// RETURNS:         Windows API
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
BOOL CxSkinLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{

	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::OnLButtonDown
//
// Description:		Called when a link is click on
//
// INPUTS:          Windows API
// 
// RETURNS:         Windows API
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				02072002    1.6     Added Mail support
//////////////////////////////////////////////////////////////////////////
void CxSkinLabel::OnLButtonDown(UINT nFlags, CPoint point) 
{

	if (!m_bNotifyParent) // Fix
	{
		CString strLink;
		strLink = m_strText;

		if (m_Link == HyperLink)
		{
			ShellExecute(NULL,_T("open"),m_sLink.IsEmpty() ? strLink : m_sLink,NULL,NULL,SW_SHOWNORMAL);
		}
		if (m_Link == MailLink)
		{
			strLink = _T("mailto:") + strLink;
			ShellExecute( NULL, NULL,  strLink,  NULL, NULL, SW_SHOWNORMAL );
		}
	}
	else
	{
		// To use notification in parent window
		// Respond to a OnNotify in parent and disassemble the message
		//
		NMHDR nm;

		nm.hwndFrom = GetSafeHwnd();
		nm.idFrom  = GetDlgCtrlID();
		nm.code = NM_LINKCLICK;
		GetParent()->SendMessage(WM_NOTIFY,nm.idFrom,(LPARAM) &nm);
	}

	CStatic::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// THE FUNCTIONS START HERE :----
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetText
//
// Description:		Short cut to set window text - caption - label
//
// INPUTS:          Text to use
// 
// RETURNS:         Reference to this
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26081998    1.0     Origin
// NT ALMOND				02072002    1.6     Crash Fix
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetText(const CString& strText)
{
	if(IsWindow(this->GetSafeHwnd())) 
	{
		if (m_strText != strText)
		{
			m_strText = strText;
			UpdateSurface();
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetTextColor
//
// Description:		Sets the text color 
//
// INPUTS:          True or false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetTextColor(COLORREF clrText, COLORREF clrTextFocus, COLORREF clrTextUnlink)
{

	m_crText = clrText;
	m_clrTextFocus = clrTextFocus;
	m_clrTextUnlink = clrTextUnlink;
	UpdateSurface();
	return *this;
}

CxSkinLabel& CxSkinLabel::SetHighlight(bool highlight)
{
	m_bHighLight = highlight;
	UpdateSurface();
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFontBold
//
// Description:		Sets the font ot bold 
//
// INPUTS:          True or false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFontBold(BOOL bBold)
{	

	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	UpdateSurface();
	return *this;
}



//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFontUnderline
//
// Description:		Sets font underline attribue
//
// INPUTS:          True of false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFontUnderline(BOOL bSet)
{	
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFontItalic
//
// Description:		Sets font italic attribute
//
// INPUTS:          True of false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFontItalic(BOOL bSet)
{

	m_lf.lfItalic = bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;	
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetSunken
//
// Description:		Sets sunken effect on border
//
// INPUTS:          True of false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetSunken(BOOL bSet)
{

	if (!bSet)
		ModifyStyleEx(WS_EX_STATICEDGE,0,SWP_DRAWFRAME);
	else
		ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_DRAWFRAME);

	return *this;	
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetBorder
//
// Description:		Toggles the border on/off
//
// INPUTS:          True of false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetBorder(BOOL bSet)
{

	if (!bSet)
		ModifyStyle(WS_BORDER,0,SWP_DRAWFRAME);
	else
		ModifyStyle(0,WS_BORDER,SWP_DRAWFRAME);

	return *this;	
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFontSize
//
// Description:		Sets the font size
//
// INPUTS:          True of false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFontSize(int nSize)
{
	CFont cf;
	LOGFONT lf;

	int nPointSize = nSize * 10;
	cf.CreatePointFont(nPointSize, m_lf.lfFaceName);
	cf.GetLogFont(&lf);

	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;

	nSize *= -1;
	m_lf.lfHeight = nSize;

	ReconstructFont();
	UpdateSurface();

	return *this;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetBkColor
//
// Description:		Sets background color
//
// INPUTS:          Colorref of background color
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh , BackFillMode mode)
{

	m_crLoColor = crBkgnd;
	m_crHiColor = crBkgndHigh;

	m_fillmode = mode;

	if (m_hBackBrush)
		::DeleteObject(m_hBackBrush);


	if (m_fillmode == Normal)
		m_hBackBrush = ::CreateSolidBrush(crBkgnd);

	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFontName
//
// Description:		Sets the fonts face name
//
// INPUTS:          String containing font name
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				15092000	1.5		Support internation windows
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFontName(const CString& strFont, BYTE byCharSet /* Default = ANSI_CHARSET */)
{	

	m_lf.lfCharSet = byCharSet;

	_tcscpy_s(m_lf.lfFaceName, 32, strFont);
	ReconstructFont();
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::FlashText
//
// Description:		As the function states
//
// INPUTS:          True or false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::FlashText(BOOL bActivate)
{

	if (m_bTimer)
		KillTimer(1);

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;

		SetTimer(1,500,NULL);

		m_Type = Text;
	}
	else
		m_Type = None; // Fix

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::FlashBackground
//
// Description:		As the function states
//
// INPUTS:          True or false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::FlashBackground(BOOL bActivate)
{

	if (m_bTimer)
		KillTimer(1);

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;
		SetTimer(1,500,NULL);

		m_Type = Background;
	}

	return *this;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetLink
//
// Description:		Indicates the string is a link
//
// INPUTS:          True or false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				26/08/99	1.2		Added flexbility of
//												Sending Click meessage to parent
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetLink(BOOL bLink,BOOL bNotifyParent)
{

	if (bLink)
		m_Link = HyperLink;
	else
		m_Link = LinkNone;

	m_bNotifyParent = bNotifyParent;

	if (m_Link != LinkNone)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);

	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetLinkCursor
//
// Description:		Sets the internet browers link
//
// INPUTS:          Handle of cursor
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetLinkCursor(HCURSOR hCursor)
{

	m_hCursor = hCursor;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetTransparent
//
// Description:		Sets the Label window to be transpaent
//
// INPUTS:          True or false
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetTransparent(BOOL bSet)
{

	m_bTransparent = bSet;
	ModifyStyleEx(0,WS_EX_TRANSPARENT); // Fix for transparency
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFont3D
//
// Description:		Sets the 3D attribute of the font.
//
// INPUTS:          True or false, Raised or Sunken
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFont3D(BOOL bSet,Type3D type)
{

	m_bFont3d = bSet;
	m_3dType = type;
	UpdateSurface();


	return *this;
}

void CxSkinLabel::OnSysColorChange() 
{

	if (m_hwndBrush)
		::DeleteObject(m_hwndBrush);

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	UpdateSurface();


}



//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetRotationAngle
//
// Description:		Sets the rotation angle for the current font.
//
// INPUTS:          Angle in Degress
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetRotationAngle(UINT nAngle,BOOL bRotation)
{
	// Arrrrh...
	// Your looking in here why the font is rotating, aren't you?
	// Well try setting the font name to 'Arial' or 'Times New Roman'
	// Make the Angle 180 and set bRotation to true.
	//
	// Font rotation _ONLY_ works with TrueType fonts...
	//
	// 
	m_lf.lfEscapement = m_lf.lfOrientation = (nAngle * 10);
	m_bRotation = bRotation;

	ReconstructFont();

	UpdateSurface();


	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetText3DHiliteColor
//
// Description:		Sets the 3D font hilite color
//
// INPUTS:          Color 
// 
// RETURNS:         Reference to 'this' object
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				17/07/00    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetText3DHiliteColor(COLORREF cr3DHiliteColor)
{
	m_cr3DHiliteColor = cr3DHiliteColor;
	UpdateSurface();


	return *this;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::PreSubclassWindow
//
// Description:		Assigns default dialog font
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				15092000    1.5     Origin
// NT ALMOND				02072002    1.6     Fix crash when GetFont returns NULL
//////////////////////////////////////////////////////////////////////////
void CxSkinLabel::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

	CFont* cf = GetFont();
	if(cf !=NULL)
	{
		cf->GetObject(sizeof(m_lf),&m_lf);
	}
	else
	{
		GetObject(GetStockObject(SYSTEM_FONT),sizeof(m_lf),&m_lf);
	}

	ReconstructFont();



}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::PreCreateWindow
//
// Description:		
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				15092000    1.5     Origin
//////////////////////////////////////////////////////////////////////////
BOOL CxSkinLabel::PreCreateWindow(CREATESTRUCT& cs) 
{	
	return CStatic::PreCreateWindow(cs);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::DrawGradientFill
//
// Description:		Internal help function to gradient fill background
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				15092000    1.5     Origin
//////////////////////////////////////////////////////////////////////////
void CxSkinLabel::DrawGradientFill(CDC* pDC, CRect* pRect, COLORREF crStart, COLORREF crEnd, int nSegments)
{
	// Get the starting RGB values and calculate the incremental
	// changes to be applied.

	COLORREF cr;
	int nR = GetRValue(crStart);
	int nG = GetGValue(crStart);
	int nB = GetBValue(crStart);

	int neB = GetBValue(crEnd);
	int neG = GetGValue(crEnd);
	int neR = GetRValue(crEnd);

	if(nSegments > pRect->Width())
		nSegments = pRect->Width();

	int nDiffR = (neR - nR);
	int nDiffG = (neG - nG);
	int nDiffB = (neB - nB);

	int ndR = 256 * (nDiffR) / (max(nSegments,1));
	int ndG = 256 * (nDiffG) / (max(nSegments,1));
	int ndB = 256 * (nDiffB) / (max(nSegments,1));

	nR *= 256;
	nG *= 256;
	nB *= 256;

	neR *= 256;
	neG *= 256;
	neB *= 256;

	int nCX = pRect->Width() / max(nSegments,1), nLeft = pRect->left, nRight;
	pDC->SelectStockObject(NULL_PEN);

	for (int i = 0; i < nSegments; i++, nR += ndR, nG += ndG, nB += ndB)
	{
		// Use special code for the last segment to avoid any problems
		// with integer division.

		if (i == (nSegments - 1))
			nRight = pRect->right;
		else
			nRight = nLeft + nCX;

		cr = RGB(nR / 256, nG / 256, nB / 256);

		{
			CBrush br(cr);
			CBrush* pbrOld = pDC->SelectObject(&br);
			pDC->Rectangle(nLeft, pRect->top, nRight + 1, pRect->bottom);
			pDC->SelectObject(pbrOld);
		}

		// Reset the left side of the drawing rectangle.

		nLeft = nRight;
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetFont
//
// Description:		Sets font with LOGFONT structure
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				02072002    1.6     Origin
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetFont(LOGFONT lf)
{
	CopyMemory(&m_lf, &lf, sizeof(m_lf));
	ReconstructFont();
	UpdateSurface();
	return *this;

}

BOOL CxSkinLabel::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetMailLink
//
// Description:		Sets the label so it becomes Mail enabled
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				02072002    1.6     Origin
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetMailLink(BOOL bEnable, BOOL bNotifyParent)
{
	if (bEnable)
		m_Link = MailLink;
	else
		m_Link = LinkNone;


	m_bNotifyParent = bNotifyParent;

	if (m_Link != LinkNone)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CxSkinLabel::SetHyperLink
//
// Description:		Sets the label so it becomes hyperlink enabled
//
// INPUTS:          
// 
// RETURNS:         
//
// NOTES:			
// 
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				02072002    1.6     Origin
//////////////////////////////////////////////////////////////////////////
CxSkinLabel& CxSkinLabel::SetHyperLink(const CString& sLink)
{
	m_sLink = sLink;

	return *this;
}

CString	CxSkinLabel::GetText(void)
{
	if(IsWindow(this->GetSafeHwnd())) 
	{
		return m_strText;
	}
	else
		return _T("");
}

void CxSkinLabel::SetToolTipText(CString s)
{
	if(m_tooltip.m_hWnd==NULL){
		ModifyStyle(0,SS_NOTIFY);
		if(m_tooltip.Create(this, TTS_ALWAYSTIP)){	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(TRUE);


			m_tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
			//m_tooltip.SetDelayTime(300);
		}
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
		//m_tooltip.SetDelayTime(300);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CxSkinLabel::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// This function will create a MSG structure, fill it in a pass it to
	// the ToolTip control, m_tooltip.  Note that we ensure the point is in window
	// coordinates (relative to the control's window).
	if(NULL != m_tooltip.m_hWnd){
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.RelayEvent(&msg);
	}
}

void CxSkinLabel::OnMouseMove(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: Mouse\n", ::GetTickCount());
	if(NULL != m_tooltip.m_hWnd){
		m_tooltip.Activate(TRUE);//클릭후, 툴팁이 사라지는 문제 수정

		//Pass this message to the ToolTip control
		RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	}
		//Forward this event to superclass
	CStatic::OnMouseMove(nFlags, point);
}

LRESULT CxSkinLabel::OnMouseLeave(WPARAM, LPARAM)
{
	if(NULL != m_tooltip.m_hWnd){
		m_tooltip.Activate(FALSE);//클릭후, 툴팁이 사라지는 문제
	}

	return S_OK;
}