// SkinEdit.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinEdit.h"

#include "SkinUtil.h"
#include "IniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UWM_CHECKTEXT_MSG _T("UWM_CHECKTEXT_MSG-{D53C7065_FD4C_138A_BC72_01A0253651C0}")

//Resize Message
const UINT CxSkinEdit::UWM_CHECKTEXT = ::RegisterWindowMessage(UWM_CHECKTEXT_MSG);
/////////////////////////////////////////////////////////////////////////////
// CxSkinEdit

CxSkinEdit::CxSkinEdit()
	:m_bReadOnly(false)
	,m_strFontName(FONT_NAME)
	,m_nFontSize(FONT_SIZE)
	,m_bFontBold(false)
	,m_bMouseOver(false)
	,m_bIsFocused(false)
	,m_bBorder(true)
	,m_bShowScroll(false)
	,m_bShowHoriz(false)
	,m_bShowVert(false)
	,m_clrOutline(COLOR_WHITE)
	,m_clrOutlineFocus(COLOR_WHITE)
	,m_dwStyle(WS_VISIBLE | WS_CHILD)
	,m_bNumeric(false)
	,m_bValidDigit(false)
	,m_bTransparentThumb(false)
	,m_clrGripBk(COLOR_SB_GRIP)
	,m_clrGripOutline(COLOR_SB_GRIP)
	,m_clrText(COLOR_BLACK)
	,m_clrFocusText(COLOR_WHITE)
	,m_clrBk(COLOR_WHITE)
	,m_clrFocusBk(COLOR_BK_FOCUS)
	,m_nSBWidth(0)
	,m_nSBHeight(0)
	,m_nVThumbTWidth(0)
	,m_nVThumbTHeight(0)
	,m_nVThumbMWidth(0)
	,m_nVThumbMHeight(0)
	,m_nVThumbBWidth(0)
	,m_nVThumbBHeight(0)
#if USE_TRANSPARENTBAR
	,m_nVChanWidth(0)
	,m_nVChanHeight(0)
#else
	,m_nVChanTWidth(0)
	,m_nVChanTHeight(0)
	,m_nVChanMWidth(0)
	,m_nVChanMHeight(0)
	,m_nVChanBWidth(0)
	,m_nVChanBHeight(0)
#endif
	,m_nHThumbTWidth(0)
	,m_nHThumbTHeight(0)
	,m_nHThumbMWidth(0)
	,m_nHThumbMHeight(0)
	,m_nHThumbBWidth(0)
	,m_nHThumbBHeight(0)
#if USE_TRANSPARENTBAR
	,m_nHChanWidth(0)
	,m_nHChanHeight(0)
#else
	,m_nHChanTWidth(0)
	,m_nHChanTHeight(0)
	,m_nHChanMWidth(0)
	,m_nHChanMHeight(0)
	,m_nHChanBWidth(0)
	,m_nHChanBHeight(0)
#endif
{
}

CxSkinEdit::~CxSkinEdit()
{
	DeleteObject(m_brFocusLine);
	DeleteObject(m_brLine);
	DeleteObject(m_brFocusBG);
	DeleteObject(m_brBG);
}

BEGIN_MESSAGE_MAP(CxSkinEdit, CEdit)
	//{{AFX_MSG_MAP(CxSkinEdit)
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()

	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UWM_CHECKTEXT, OnCheckText)
	ON_WM_CREATE()
	ON_WM_DESTROY()

END_MESSAGE_MAP()

//Overloaded Function
void CxSkinEdit::SetWindowText(LPCTSTR lpszString)
{
	CheckScrolling(lpszString);
	CEdit::SetWindowText(lpszString);
}

/////////////////////////////////////////////////////////////////////////////
// CxSkinEdit message handlers
void CxSkinEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEdit::OnMouseMove(nFlags, point);
	if(m_bIsFocused) return;
	if(GetCapture()!=this)		//come in for the first time
	{
		m_bMouseOver=TRUE;
		SetCapture();
		Invalidate(TRUE);
	}
	else
	{
		CRect rect;
		GetClientRect(&rect);
		if(!rect.PtInRect(point))//Mouse move out of Edit control
		{
			m_bMouseOver=FALSE;
			Invalidate(TRUE);
			ReleaseCapture();
		}
	}
}

void CxSkinEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	m_bIsFocused=TRUE;
	m_bMouseOver=TRUE;
	Invalidate(TRUE);
}

void CxSkinEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	m_bIsFocused=FALSE;
	m_bMouseOver=FALSE;
	Invalidate(TRUE);
}

HBRUSH CxSkinEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if(m_bMouseOver)
	{
		pDC->SetBkColor(m_clrFocusBk);
		pDC->SetTextColor(m_clrFocusText);
		return m_brFocusBG;
	}
	else
	{
		pDC->SetBkColor(m_clrBk);
		pDC->SetTextColor(m_clrText);
		return m_brBG;
	}
}

void  CxSkinEdit::OnPaint() 
{
	CEdit::OnPaint();

	if(m_bBorder)
	{
		CRect rect;
		GetClientRect(&rect);
		rect.InflateRect(1,1,1,1);

		CBrush brush;
		brush.CreateSolidBrush(m_clrOutline);

		CDC *pDC=GetDC();
		pDC->FrameRect(rect,&brush);

		//TODO 마우스 오버시 깜빡임 문제 발생
		//if(m_bIsFocused)
		//{
		//	pDC->FrameRect(rect,&m_brFocusLine);
		//}
		//else
		//{
		//	pDC->FrameRect(rect,&m_brLine);
		//}

		DeleteObject(brush);
	}
}

BOOL CxSkinEdit::PreCreateWindow(CREATESTRUCT& cs) 
{
	//ES_AUTOHSCROLL 스타일을 줘야 스크롤되면서 글씨를 입력할 수 있다.
	if(m_bShowScroll)
	{
		if(m_bShowHoriz && m_bShowVert)
			cs.style |= WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN;
		else if(m_bShowHoriz)
			cs.style |= WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL;
		else if(m_bShowVert)
			cs.style |= WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL;
	}
	return CEdit::PreCreateWindow(cs);
}

void CxSkinEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_bReadOnly)
	{
		return;
	}

	if (m_bNumeric)
	{
		TCHAR delimiter = m_strDelim.GetAt(0);
		int nStartChar, nEndChar;

		if( nChar == (UINT) delimiter )
		{

			CString strText;
			GetWindowText( strText );

			if ( strText.IsEmpty() )
			{
				SetWindowText( (_T("0") + m_strDelim) );
				SetSel( 2, 2, FALSE );
				return;
			}
			else
			{
				// If the entire number is selected, replace it with a zero and the delimiter
				BOOL delimSelected;
				delimSelected = false;
				GetSel(nStartChar, nEndChar);
				if ( (nStartChar == 0) && (nEndChar == strText.GetLength()) )
				{
					// full replacement
					SetWindowText( (_T("0") + m_strDelim) );
					SetSel( 2, 2, FALSE );
					return;
				}
				else if (nStartChar < nEndChar) { //some of it was selected
					//look for delim in selection
					int x;
					x = nStartChar;
					while (x <= nEndChar) {
						if ( strText[x] == m_strDelim) {
							delimSelected = true;
							x = nEndChar; //force exit loop
						}
						x++;
					}
				}

				// if the decimal point already entered, not allow enter more points
				if( strText.Find( m_strDelim ) >= 0 && delimSelected == false)
					return;

				// no delimiter yet entered, so insert one //let the bottom take care of this.
				// SetWindowText( (strText + m_strDelim) );
				// int nNewLength = strText.GetLength() + m_strDelim.GetLength(); // delimiter might be more than one character?
				// SetSel( nNewLength, nNewLength, FALSE );
				// return;
			}
		}

		// 8 - back space
		// 46 - .
		// 48 - 1
		// 57 - 0
		GetSel(nStartChar, nEndChar); //so we can check for '-' in first pos. only
		if( (nChar == (UINT) delimiter) || (nChar >= '0' && nChar <= '9') || (nChar == 8)
			|| (nChar == '-' && nStartChar == 0) )
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}

		//TCHAR c = static_cast<TCHAR>(nChar);

		//if (!_istdigit(c) &&  isprint(nChar))
		//{
		//	return;
		//}	
	}
	else
	{
		//Possible Text Change
		PostMessage(UWM_CHECKTEXT);

		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}

}

LRESULT CxSkinEdit::OnCheckText(WPARAM wParam, LPARAM lParam)
{
	if(!m_bShowScroll)
		return 0;
	CString oStr;
	GetWindowText(oStr);

	CheckScrolling(oStr);
	return 0;
}

void CxSkinEdit::CheckScrolling(LPCTSTR lpszString)
{
	if(!m_bShowScroll)
	{
		return;
	}
	CRect oRect;
	GetClientRect(&oRect);
	CClientDC oDC(this);
	int iHeight=0;
	BOOL bHoriz = FALSE;
	CFont* pEdtFont = GetFont();
	if(pEdtFont != NULL)
	{
		//Determine Text Width and Height
		SIZE oSize;
		CFont* pOldFont = oDC.SelectObject(pEdtFont);
		//Determine the line Height
		oSize = oDC.GetTextExtent(CString(_T(" ")));
		iHeight = oSize.cy;
		//Text Width
		int iWidth=0, i =0;
		CString oStr;
		//Parse the string, the lines in a multiline Edit are separated by "\r\n"
		while(TRUE == ::AfxExtractSubString(oStr, lpszString, i, _T('\n')))
		{
			if(FALSE == bHoriz)
			{
				int iLen = oStr.GetLength()-1;
				if(iLen >=0)
				{
					//Eliminate last '\r'
					if(_T('\r') == oStr.GetAt(iLen))
						oStr = oStr.Left(iLen);
					oSize = oDC.GetTextExtent(oStr);
					if(iWidth < oSize.cx)
						iWidth = oSize.cx;
					if(iWidth >= oRect.Width())
						bHoriz = TRUE;
				}
			}
			i++;
		}
		oDC.SelectObject(pOldFont);
		//Text Height
		iHeight *= i;
	}
	ShowHorizScrollBar(bHoriz);
	ShowVertScrollBar(iHeight >= oRect.Height());
}

#if USE_TRANSPARENTBAR
void CxSkinEdit::CopyVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
	CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
	CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
	CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
	CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
	CRect rcVCH)
#else
void CxSkinEdit::CopyVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
	CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
	CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
	CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
	CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
	CRect rcVTCH, CRect rcVMCH, CRect rcVBCH)
#endif
{
	CDC* pDC = GetDC();

	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmVSC_TopBtn_N, bmSkinImage, rcVTBN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_TopBtn_O, bmSkinImage, rcVTBO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_TopBtn_D, bmSkinImage, rcVTBD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_BottomBtn_N, bmSkinImage, rcVBBN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_BottomBtn_O, bmSkinImage, rcVBBO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_BottomBtn_D, bmSkinImage, rcVBBD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_T_N, bmSkinImage, rcVTHTN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_T_O, bmSkinImage, rcVTHTO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_T_D, bmSkinImage, rcVTHTD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_C_N, bmSkinImage, rcVTHMN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_C_O, bmSkinImage, rcVTHMO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_C_D, bmSkinImage, rcVTHMD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_B_N, bmSkinImage, rcVTHBN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_B_O, bmSkinImage, rcVTHBO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_B_D, bmSkinImage, rcVTHBD);

#if USE_TRANSPARENTBAR
	skinUtil.CopyBitmap(pDC, m_bmVSC_Channel, bmSkinImage, rcVCH);
#else
	skinUtil.CopyBitmap(pDC, m_bmVSC_ChannelTop, bmSkinImage, rcVTCH);
	skinUtil.CopyBitmap(pDC, m_bmVSC_ChannelMid, bmSkinImage, rcVMCH);
	skinUtil.CopyBitmap(pDC, m_bmVSC_ChannelBtm, bmSkinImage, rcVBCH);
#endif

	m_nSBWidth		= rcVTHTN.Width();
	m_nSBHeight		= rcVTHTN.Height();

	ReleaseDC(pDC);

	m_nVThumbTWidth		= rcVTHTN.Width();
	m_nVThumbTHeight	= rcVTHTN.Height();
	m_nVThumbMWidth		= rcVTHMN.Width();
	m_nVThumbMHeight	= rcVTHMN.Height();
	m_nVThumbBWidth		= rcVTHBN.Width();
	m_nVThumbBHeight	= rcVTHBN.Height();
#if USE_TRANSPARENTBAR
	m_nVChanWidth		= rcVCH.Width();
	m_nVChanHeight		= rcVCH.Height();
#else
	m_nVChanTWidth		= rcVTCH.Width();
	m_nVChanTHeight		= rcVTCH.Height();
	m_nVChanMWidth		= rcVMCH.Width();
	m_nVChanMHeight		= rcVMCH.Height();
	m_nVChanBWidth		= rcVBCH.Width();
	m_nVChanBHeight		= rcVBCH.Height();
#endif
}

#if USE_TRANSPARENTBAR
void CxSkinEdit::CopyHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
	CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
	CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
	CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
	CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
	CRect rcHCH)
#else
void CxSkinEdit::CopyHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
	CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
	CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
	CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
	CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
	CRect rcHTCH, CRect rcHMCH, CRect rcHBCH)
#endif
{
	CDC* pDC = GetDC();

	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmHSC_LeftBtn_N, bmSkinImage, rcHLBN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_LeftBtn_O, bmSkinImage, rcHLBO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_LeftBtn_D, bmSkinImage, rcHLBD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_RigthBtn_N, bmSkinImage, rcHRBN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_RigthBtn_O, bmSkinImage, rcHRBO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_RigthBtn_D, bmSkinImage, rcHRBD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_T_N, bmSkinImage, rcHTHTN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_T_O, bmSkinImage, rcHTHTO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_T_D, bmSkinImage, rcHTHTD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_C_N, bmSkinImage, rcHTHMN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_C_O, bmSkinImage, rcHTHMO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_C_D, bmSkinImage, rcHTHMD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_B_N, bmSkinImage, rcHTHBN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_B_O, bmSkinImage, rcHTHBO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_B_D, bmSkinImage, rcHTHBD);

#if USE_TRANSPARENTBAR
	skinUtil.CopyBitmap(pDC, m_bmHSC_Channel, bmSkinImage, rcHCH);
#else
	skinUtil.CopyBitmap(pDC, m_bmHSC_ChannelTop, bmSkinImage, rcHTCH);
	skinUtil.CopyBitmap(pDC, m_bmHSC_ChannelMid, bmSkinImage, rcHMCH);
	skinUtil.CopyBitmap(pDC, m_bmHSC_ChannelBtm, bmSkinImage, rcHBCH);
#endif

	m_nSBWidth		= rcHLBN.Width();
	m_nSBHeight		= rcHLBN.Height();

	ReleaseDC(pDC);

	m_nHThumbTWidth		= rcHTHTN.Width();
	m_nHThumbTHeight	= rcHTHTN.Height();
	m_nHThumbMWidth		= rcHTHMN.Width();
	m_nHThumbMHeight	= rcHTHMN.Height();
	m_nHThumbBWidth		= rcHTHBN.Width();
	m_nHThumbBHeight	= rcHTHBN.Height();
#if USE_TRANSPARENTBAR
	m_nHChanWidth		= rcHCH.Width();
	m_nHChanHeight		= rcHCH.Height();
#else
	m_nHChanTWidth		= rcHTCH.Width();
	m_nHChanTHeight		= rcHTCH.Height();
	m_nHChanMWidth		= rcHMCH.Width();
	m_nHChanMHeight		= rcHMCH.Height();
	m_nHChanBWidth		= rcHBCH.Width();
	m_nHChanBHeight		= rcHBCH.Height();
#endif
}

#if USE_COOL_SB
LRESULT CxSkinEdit::_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm)
{ 
	UNREFERENCED_PARAMETER(ctrlid);

#if USE_TRANSPARENTBAR
	int nWidthCC, nHeightCC;
#else
	int nWidthCCT, nHeightCCT, nWidthCCM, nHeightCCM, nWidthCCB, nHeightCCB;
#endif
	int nHeightT, nHeightC, nHeightB, nWidthT, nWidthC, nWidthB, nStartH=0,nStartW=0;;

	CDC *pDC = CDC::FromHandle(nm->hdc);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	if(memDC == NULL)
		return CDRF_DODEFAULT;                        

	CBitmap *pOldBitmap;

	if(nm->nBar == SB_BOTH)
	{//the sizing gripper in the bottom-right corner
		RECT *rc = &nm->rect;

		pDC->FillSolidRect(rc, m_clrGripOutline);
		rc->top += 1;
		rc->left += 1;
		pDC->FillSolidRect(rc, m_clrGripBk);

		return CDRF_SKIPDEFAULT;

	}
	else if(nm->nBar == SB_HORZ)
	{	
		//nWidthCC = HSC_Channel.Width();
		//nHeightCC = HSC_Channel.Height();

		//nWidthT = HSC_Thumb_T_N.Width();
		//nHeightT = HSC_Thumb_T_N.Height();
		//nHeightC = HSC_Thumb_C_N.Height();
		//nWidthC = HSC_Thumb_C_N.Width();
		//nWidthB = HSC_Thumb_B_N.Width();
		//nHeightB = HSC_Thumb_B_N.Height();
#if USE_TRANSPARENTBAR
		nWidthCC	= m_nHChanWidth;
		nHeightCC	= m_nHChanHeight;
#else
		nWidthCCT	= m_nHChanTWidth;
		nHeightCCT	= m_nHChanTHeight;
		nWidthCCM	= m_nHChanMWidth;
		nHeightCCM	= m_nHChanMHeight;
		nWidthCCB	= m_nHChanBWidth;
		nHeightCCB	= m_nHChanBHeight;
#endif

		nWidthT		= m_nHThumbTWidth;
		nHeightT	= m_nHThumbTHeight;
		nWidthC		= m_nHThumbMWidth;
		nHeightC	= m_nHThumbMHeight;
		nWidthB		= m_nHThumbBWidth;
		nHeightB	= m_nHThumbBHeight;

		if(nm->uState == CDIS_HOT)
		{			
			switch(nm->uItem)
			{
			case HTSCROLL_LEFT: //up arrow   ACTIVE
				{					
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_LeftBtn_O);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_RIGHT: //down arrow ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_RigthBtn_O);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);				
					break;
				}
			case HTSCROLL_PAGELEFT: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGERIGHT: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //horz thumb
				{
					CDC objDC;
					CRect ThumbRect;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);					

					int nCenter =(ThumbRect.Width()-nWidthC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}
					//nStartH++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelMid);
					memDC.StretchBlt(nWidthCCT, 0, ThumbRect.Width()-(nWidthCCT+nWidthCCB), nHeightCCM, &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelBtm);
					memDC.BitBlt(ThumbRect.Width()-nWidthCCB, 0, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_O);
						memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_C_O);
						memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
						memDC.TransparentBlt(nCenter, nStartH, nWidthC, nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
						memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_B_O);
						memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_O);
						memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_C_O);
						memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
						memDC.BitBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_B_O);
						memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					
					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					DeleteDC(objDC);

					DeleteObject(bmpBuffer.m_hObject);					
					break;
				}
			}
		}
		else if(nm->uState == CDIS_SELECTED) 
		{
			switch(nm->uItem)
			{
			case HTSCROLL_LEFT: //up arrow   ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_LeftBtn_D);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					break;
				}
			case HTSCROLL_RIGHT: //down arrow ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_RigthBtn_D);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					break;
				}
			case HTSCROLL_PAGELEFT: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);						
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGERIGHT: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);					
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//pDC->TransparentBlt(-1, -1, 0, 0, &memDC,0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //horz thumb
				{
					CRect ThumbRect;
					CDC objDC;						
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter = (ThumbRect.Width()-nWidthC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}

					//nStartH++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					//////////////////////////////////////////////////////////////////////////
					

					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelMid);
					memDC.StretchBlt(nWidthCCT, 0, ThumbRect.Width()-(nWidthCCT+nWidthCCB), nHeightCCM, &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelBtm);
					memDC.BitBlt(ThumbRect.Width()-nWidthCCB, 0, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_D);						
						memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_C_D);
						memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
						memDC.TransparentBlt(nCenter, nStartH, nWidthC, nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
						memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_B_D);						
						memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_D);						
						memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_C_D);						
						memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
						memDC.BitBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_B_D);						
						memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					
					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					DeleteDC(objDC);

					DeleteObject(bmpBuffer.m_hObject);

					break;
				}
			}
		}
		else
		{
			switch(nm->uItem)
			{
			case HTSCROLL_LEFT: //up arrow   NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_LeftBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);				
					break;
				}
			case HTSCROLL_RIGHT: //down arrow NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_RigthBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					//pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, 14, 14, SRCCOPY);
					break;
				}
			case HTSCROLL_PAGELEFT: //page up	 NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGERIGHT: //page down  NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_THUMB: //horz thumb
				{
					CRect ThumbRect;
					CDC objDC;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);


					int nCenter =(ThumbRect.Width()-nWidthC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}
					//nStartH++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelMid);
					memDC.StretchBlt(nWidthCCT, 0, ThumbRect.Width()-(nWidthCCT+nWidthCCB), nHeightCCM, &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelBtm);
					memDC.BitBlt(ThumbRect.Width()-nWidthCCB, 0, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_N);
						memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_C_N);
						memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
						memDC.TransparentBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
						memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_B_N);
						memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_N);
						memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_C_N);
						memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);				
						memDC.BitBlt(nCenter, nStartH, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_B_N);
						memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					
					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					DeleteDC(objDC);
					memDC.SelectObject(pOldBitmap);
					DeleteDC(memDC);
					DeleteObject(bmpBuffer.m_hObject);

					break;
				}
			}//__End Switch
		}//__End if

		return CDRF_SKIPDEFAULT;
	}
	else if(nm->nBar == SB_VERT)
	{
#if USE_TRANSPARENTBAR
		nWidthCC	= m_nVChanWidth;
		nHeightCC	= m_nVChanHeight;
#else
		nWidthCCT	= m_nVChanTWidth;
		nHeightCCT	= m_nVChanTHeight;
		nWidthCCM	= m_nVChanMWidth;
		nHeightCCM	= m_nVChanMHeight;
		nWidthCCB	= m_nVChanBWidth;
		nHeightCCB	= m_nVChanBHeight;
#endif
		nWidthT		= m_nVThumbTWidth;
		nHeightT	= m_nVThumbTHeight;
		nWidthC		= m_nVThumbMWidth;
		nHeightC	= m_nVThumbMHeight;
		nWidthB		= m_nVThumbBWidth;
		nHeightB	= m_nVThumbBHeight;


		if(nm->uState == CDIS_HOT) 
		{
			switch(nm->uItem)
			{
			case HTSCROLL_UP: //up arrow   ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_TopBtn_O);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_DOWN: //down arrow ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_BottomBtn_O);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_PAGEGUP: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGEGDOWN: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(-1, -1, 0, 0, &memDC,0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //vert thumb
				{
					CRect ThumbRect;
					CDC objDC;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter =( ThumbRect.Height()-nHeightC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Height()-nHeightC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Width() > nWidthT){
						nStartW = (ThumbRect.Width() - nWidthT)/2;
					}
					//nStartW++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//memDC.TransparentBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelMid);
					memDC.StretchBlt(0, nHeightCCT, nWidthCCM, ThumbRect.Height()-(nHeightCCT+nHeightCCB), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelBtm);
					memDC.BitBlt(0, ThumbRect.Height() - nHeightB, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////

#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_O);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_C_O);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_O);
						memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_O);
						memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_C_O);
						memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_B_O);
						memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmVSC_Thumb_T_O);
					////memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);
					//
					//objDC.SelectObject(&m_bmVSC_Thumb_C_O);
					////memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
					////memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
					////memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
					//memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_B_O);
					////memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);
					DeleteObject(bmpBuffer.m_hObject);
					break;
				}		
			}
		}
		else if(nm->uState == CDIS_SELECTED) 
		{
			switch(nm->uItem)
			{				
			case HTSCROLL_UP: //up arrow   ACTIVE
				{					
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_TopBtn_D);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_DOWN: //down arrow ACTIVE
				{					
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_BottomBtn_D);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_PAGEGUP: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);					
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGEGDOWN: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);				
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);	

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(-1, -1, 0, 0, &memDC,0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //vert thumb
				{
					CRect ThumbRect;
					CDC  objDC;					
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter =(ThumbRect.Height() - nHeightC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Height()-nHeightC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Width() > nWidthT){
						nStartW = (ThumbRect.Width() - nWidthT)/2;
					}
					//nStartW++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//memDC.TransparentBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelMid);
					memDC.StretchBlt(0, nHeightCCT, nWidthCCM, ThumbRect.Height()-(nHeightCCT+nHeightCCB), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelBtm);
					memDC.BitBlt(0, ThumbRect.Height() - nHeightB, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_D);
						//memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_C_D);
						//memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						//memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
						//memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_D);
						//memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
						memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);

					}
					else
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_D);
						memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_C_D);
						memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_B_D);
						memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmVSC_Thumb_T_D);
					////memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_C_D);
					////memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
					////memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
					////memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
					//memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_B_D);
					////memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					//////////////////////////////////////////////////////////////////////////

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);
					DeleteObject(bmpBuffer.m_hObject);
					break;
				}
			}
		}
		else
		{
			switch(nm->uItem)
			{
			case HTSCROLL_UP: //up arrow   NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_TopBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					//pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, 14, 14, SRCCOPY);
					break;
				}
			case HTSCROLL_DOWN: //down arrow NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_BottomBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					//pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, 14, 14, SRCCOPY);
					break;
				}
			case HTSCROLL_PAGEGUP: //page up	 NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGEGDOWN: //page down  NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right-nm->rect.left, nm->rect.bottom-nm->rect.top, &memDC, -1, -1, -1, -1, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right-nm->rect.left, nm->rect.bottom-nm->rect.top, &memDC, -1, -1, -1, -1, RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //vert thumb
				{
					CRect ThumbRect;
					CDC objDC;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter =( ThumbRect.Height()-nHeightC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Height()-nHeightC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Width() > nWidthT){
						nStartW = (ThumbRect.Width()- nWidthT)/2;
					}
					//nStartW++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//memDC.TransparentBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelMid);
					memDC.StretchBlt(0, nHeightCCT, nWidthCCM, ThumbRect.Height()-(nHeightCCT+nHeightCCB), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelBtm);
					memDC.BitBlt(0, ThumbRect.Height() - nHeightB, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_N);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_C_N);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_N);
						memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_N);
						memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_C_N);
						memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						memDC.BitBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_B_N);
						memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmVSC_Thumb_T_N);
					//memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					////memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_C_N);
					//memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
					//memDC.BitBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
					//memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
					////memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
					////memDC.TransparentBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
					////memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_B_N);
					//memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					////memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					//////////////////////////////////////////////////////////////////////////

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);
					DeleteObject(bmpBuffer.m_hObject);
					break;
				}
			}//__End Switch
		}//__End if
		return CDRF_SKIPDEFAULT;
	}
	else
	{
		return CDRF_DODEFAULT;

	}

	return CDRF_SKIPDEFAULT;
}
#endif
void CxSkinEdit::FreeSkin()
{
	if(m_bmVSC_TopBtn_N.m_hObject)
		m_bmVSC_TopBtn_N.DeleteObject();

	if(m_bmVSC_TopBtn_O.m_hObject)
		m_bmVSC_TopBtn_O.DeleteObject();

	if(m_bmVSC_TopBtn_D.m_hObject)
		m_bmVSC_TopBtn_D.DeleteObject();

	if(m_bmVSC_BottomBtn_N.m_hObject)
		m_bmVSC_BottomBtn_N.DeleteObject();

	if(m_bmVSC_BottomBtn_O.m_hObject)
		m_bmVSC_BottomBtn_O.DeleteObject();

	if(m_bmVSC_BottomBtn_D.m_hObject)
		m_bmVSC_BottomBtn_D.DeleteObject();

	if(m_bmVSC_Thumb_T_N.m_hObject)
		m_bmVSC_Thumb_T_N.DeleteObject();

	if(m_bmVSC_Thumb_T_O.m_hObject)
		m_bmVSC_Thumb_T_O.DeleteObject();

	if(m_bmVSC_Thumb_T_D.m_hObject)
		m_bmVSC_Thumb_T_D.DeleteObject();

	if(m_bmVSC_Thumb_C_N.m_hObject)
		m_bmVSC_Thumb_C_N.DeleteObject();

	if(m_bmVSC_Thumb_C_O.m_hObject)
		m_bmVSC_Thumb_C_O.DeleteObject();

	if(m_bmVSC_Thumb_C_D.m_hObject)
		m_bmVSC_Thumb_C_D.DeleteObject();

	if(m_bmVSC_Thumb_B_N.m_hObject)
		m_bmVSC_Thumb_B_N.DeleteObject();

	if(m_bmVSC_Thumb_B_O.m_hObject)
		m_bmVSC_Thumb_B_O.DeleteObject();

	if(m_bmVSC_Thumb_B_D.m_hObject)
		m_bmVSC_Thumb_B_D.DeleteObject();

#if USE_TRANSPARENTBAR
	if(m_bmVSC_Channel.m_hObject)
		m_bmVSC_Channel.DeleteObject();
#else
	if(m_bmVSC_ChannelTop.m_hObject)
		m_bmVSC_ChannelTop.DeleteObject();

	if(m_bmVSC_ChannelMid.m_hObject)
		m_bmVSC_ChannelMid.DeleteObject();

	if(m_bmVSC_ChannelBtm.m_hObject)
		m_bmVSC_ChannelBtm.DeleteObject();
#endif

	if(m_bmHSC_RigthBtn_D.m_hObject)
		m_bmHSC_RigthBtn_D.DeleteObject();

	if(m_bmHSC_RigthBtn_O.m_hObject)
		m_bmHSC_RigthBtn_O.DeleteObject();

	if(m_bmHSC_RigthBtn_N.m_hObject)
		m_bmHSC_RigthBtn_N.DeleteObject();

	if(m_bmHSC_LeftBtn_D.m_hObject)
		m_bmHSC_LeftBtn_D.DeleteObject();

	if(m_bmHSC_LeftBtn_O.m_hObject)
		m_bmHSC_LeftBtn_O.DeleteObject();

	if(m_bmHSC_LeftBtn_N.m_hObject)
		m_bmHSC_LeftBtn_N.DeleteObject();

#if USE_TRANSPARENTBAR
	if(m_bmHSC_Channel.m_hObject)
		m_bmHSC_Channel.DeleteObject();
#else	
	if(m_bmHSC_ChannelTop.m_hObject)
		m_bmHSC_ChannelTop.DeleteObject();

	if(m_bmHSC_ChannelMid.m_hObject)
		m_bmHSC_ChannelMid.DeleteObject();

	if(m_bmHSC_ChannelBtm.m_hObject)
		m_bmHSC_ChannelBtm.DeleteObject();
#endif

	if(m_bmHSC_Thumb_B_D.m_hObject)
		m_bmHSC_Thumb_B_D.DeleteObject();

	if(m_bmHSC_Thumb_B_O.m_hObject)
		m_bmHSC_Thumb_B_O.DeleteObject();

	if(m_bmHSC_Thumb_B_N.m_hObject)
		m_bmHSC_Thumb_B_N.DeleteObject();

	if(m_bmHSC_Thumb_C_D.m_hObject)
		m_bmHSC_Thumb_C_D.DeleteObject();

	if(m_bmHSC_Thumb_C_O.m_hObject)
		m_bmHSC_Thumb_C_O.DeleteObject();

	if(m_bmHSC_Thumb_C_N.m_hObject)
		m_bmHSC_Thumb_C_N.DeleteObject();

	if(m_bmHSC_Thumb_T_D.m_hObject)
		m_bmHSC_Thumb_T_D.DeleteObject();

	if(m_bmHSC_Thumb_T_O.m_hObject)
		m_bmHSC_Thumb_T_O.DeleteObject();

	if(m_bmHSC_Thumb_T_N.m_hObject)
		m_bmHSC_Thumb_T_N.DeleteObject();
}
int CxSkinEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CxSkinEdit::OnDestroy()
{
	CEdit::OnDestroy();

#if USE_COOL_SB
	UninitializeCoolSB(m_hWnd);
#endif
	if(m_Font.m_hObject)
		m_Font.DeleteObject();
	FreeSkin();
}

void CxSkinEdit::SetTextColor(COLORREF clrNormalText, COLORREF clrFocusText)
{
	m_clrText = clrNormalText;
	m_clrFocusText = clrFocusText;
}

void CxSkinEdit::SetBGColor(COLORREF clrNormalBG, COLORREF clrFocusBG)
{
	m_clrBk = clrNormalBG;
	m_clrFocusBk = clrFocusBG;
}

void CxSkinEdit::SetLineColor(COLORREF clrNomalLine, COLORREF clrFocusLine)
{
	m_clrOutline = clrNomalLine;
	m_clrOutlineFocus = clrFocusLine;
}

void CxSkinEdit::SetScrollGripColor(COLORREF clrBk, COLORREF clrOutline)
{
	m_clrGripBk	= clrBk;
	m_clrGripOutline = clrOutline;
}

void CxSkinEdit::SetShowScrollbar(BOOL bShow)
{
	m_bShowScroll = bShow;

	m_bShowHoriz = bShow;
	m_bShowVert = bShow;
}

void CxSkinEdit::Init(void)
{
	int nfWeight = FW_NORMAL;
	if (m_bFontBold)
	{
		nfWeight = FW_SEMIBOLD;
	}	

	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	VERIFY(m_Font.CreateFont(
		m_nFontSize, 
		0, 
		0, 
		0, 
		FW_NORMAL,
		FALSE, 
		FALSE, 
		0, 
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, 
		m_strFontName));

	SetFont(&m_Font, TRUE);

	m_brFocusLine.CreateSolidBrush(m_clrOutlineFocus);
	m_brLine.CreateSolidBrush(m_clrOutline);
	m_brFocusBG.CreateSolidBrush(m_clrFocusBk);	//focus Bg
	m_brBG.CreateSolidBrush(m_clrBk);			//none focus bg

	if(IsWindow(this->GetSafeHwnd()))
	{
		SetWindowText(_T(""));//if text is empty, Don't display scrollbar
#if USE_COOL_SB
		InitializeSB(this->m_hWnd);
		SB_SetStyle(this->m_hWnd, SB_BOTH, CSBS_HOTTRACKED);
		SB_SetSize(this->m_hWnd, SB_BOTH,m_nSBWidth, m_nSBHeight);
		SB_SetMinThumbSize(this->m_hWnd, SB_BOTH, m_nMinThumbSize);
#endif		
	}

	// determine the decimal delimiter buffer size
	const int nBuffLen = ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, NULL, 0 );
	_ASSERT( nBuffLen > 0 );

	// get the decimal number delimiter
	const int nResult = ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, 
		m_strDelim.GetBuffer(nBuffLen), nBuffLen );
	_ASSERT(nResult != 0);
	m_strDelim.ReleaseBuffer();


	Invalidate();
}


BOOL CxSkinEdit::PreTranslateMessage(MSG* pMsg)
{
	if (m_bReadOnly)
	{
		//읽기전용에서 키 처리
		if(pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == VK_DELETE)
				return TRUE;// DELETE키는 더이상 처리하지 않는다. 이상하게 키를 먹는다.

			return FALSE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CxSkinEdit::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	
#if USE_COOL_SB
	NMHDR *hdr = (NMHDR *)lParam;
	if(hdr->code == NM_COOLSB_CUSTOMDRAW)
	{
		NMCSBCUSTOMDRAW *nm;
		nm = (NMCSBCUSTOMDRAW *)lParam;

		if(nm->dwDrawStage == CDDS_PREPAINT)
		{
			return CDRF_SKIPDEFAULT;
		}

		if(nm->hdr.hwndFrom == m_hWnd)
			return _SBCustomDraw(wParam, nm);

		return CDRF_SKIPDEFAULT;
	}
#endif
	return CEdit::OnNotify(wParam, lParam, pResult);
}

void CxSkinEdit::AppendText(CString strText)
{
	int nLen = GetWindowTextLength();
	SetFocus();
	SetSel(nLen, nLen);
	ReplaceSel(strText);
}