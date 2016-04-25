// xSkinStatic.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinStatic.h"

#include "Define.h"
//#include "SkinUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CxSkinStatic

CxSkinStatic::CxSkinStatic()
	:m_strSentence("")
	,m_nOption(ETO_CLIPPED)
	,m_crTextColor(COLOR_WHITE)
	,m_sResizeType(NOSIZE)
	,_lpCallback(NULL)
	,m_nXPos(0)
	,m_bAutoShift(false)
	,m_bRev(false)
	,m_nMSec(120)
	,m_bText(false)
	,m_bDrawPattern(false)
	,m_hCursor(NULL)
	
{

	
}

CxSkinStatic::~CxSkinStatic()
{
	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	if(m_BitBG.m_hObject)
		m_BitBG.DeleteObject();
}


BEGIN_MESSAGE_MAP(CxSkinStatic, CStatic)
	//{{AFX_MSG_MAP(CxSkinStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CxSkinStatic message handlers

BOOL CxSkinStatic::OnEraseBkgnd(CDC* pDC) 
{
	if(!m_bText )
	{
		if(!m_BitBG.m_hObject)
			return true;

		CDC memDC, objDC;
		memDC.CreateCompatibleDC(pDC);
		objDC.CreateCompatibleDC(pDC);

		CRect cRect;
		GetClientRect(&cRect);

		CBitmap bmpBuffer;
		bmpBuffer.CreateCompatibleBitmap(pDC, cRect.Width(), cRect.Height());

		CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);
		CBitmap *poldtmpbitmap = objDC.SelectObject(&m_BitBG);

		//#define		NOSIZE		0
		//#define		RESIZEV		1
		//#define		RESIZEH		2
		//#define		RESIZEF		3

		if(m_sResizeType == RESIZEF)
		{

			//__Top Part
			int xFillsNeeded = (cRect.Width() - (m_Map_Static.OffSet(M_TOP_LEFT).Width() + m_Map_Static.OffSet(M_TOP_RIGHT).Width()));

			//__TopLeft
			memDC.BitBlt(0, 0, m_Map_Static.OffSet(M_TOP_LEFT).Width(), m_Map_Static.OffSet(M_TOP_LEFT).Height(), &objDC, m_Map_Static.OffSet(M_TOP_LEFT).left, m_Map_Static.OffSet(M_TOP_LEFT).top, SRCCOPY);

			// StretchBlt 방식 사선이나 점선등을 표현 못함
			// BitBlt 방식 StretchBlt 방식에 비해 처리가 느림
			//__TopCenter

			if (m_bDrawPattern)
			{
				for(int i = 0; i <= xFillsNeeded; i++){
					memDC.BitBlt(m_Map_Static.OffSet(M_TOP_LEFT).Width() + (m_Map_Static.OffSet(M_TOP_CENTER).Width() * i), 0, 
						m_Map_Static.OffSet(M_TOP_CENTER).Width(), m_Map_Static.OffSet(M_TOP_CENTER).Height(), &objDC, m_Map_Static.OffSet(M_TOP_CENTER).left, m_Map_Static.OffSet(M_TOP_CENTER).top, SRCCOPY);
				}
			}
			else
			{
				memDC.StretchBlt(m_Map_Static.OffSet(M_TOP_LEFT).Width(), 0, xFillsNeeded, m_Map_Static.OffSet(M_TOP_CENTER).Height(), &objDC, 
					m_Map_Static.OffSet(M_TOP_CENTER).left, m_Map_Static.OffSet(M_TOP_CENTER).top, m_Map_Static.OffSet(M_TOP_CENTER).Width(), m_Map_Static.OffSet(M_TOP_CENTER).Height(), SRCCOPY);
			}

			//__TopRight
			memDC.BitBlt((cRect.Width() - m_Map_Static.OffSet(M_TOP_RIGHT).Width()), 0, m_Map_Static.OffSet(M_TOP_RIGHT).Width(), 
				m_Map_Static.OffSet(M_TOP_RIGHT).Height(), &objDC, m_Map_Static.OffSet(M_TOP_RIGHT).left, m_Map_Static.OffSet(M_TOP_RIGHT).top, SRCCOPY);

			//__Middle Part	
			int yStretch = cRect.Height() - (m_Map_Static.OffSet(M_TOP_LEFT).Height() + m_Map_Static.OffSet(M_BOTTOM_LEFT).Height());
			int xStretch = cRect.Width() - (m_Map_Static.OffSet(M_MIDDLE_LEFT).Width() + m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width());

			if (m_bDrawPattern)
			{
				int nTLH = m_Map_Static.OffSet(M_TOP_LEFT).Height();
				int nBLH = m_Map_Static.OffSet(M_BOTTOM_LEFT).Height();
				int nMLH = m_Map_Static.OffSet(M_MIDDLE_LEFT).Height();

				if(nMLH <= 0)
					nMLH = 1;

				int yFillsNeeded = (cRect.Height() - (nTLH + nBLH)) / nMLH;

				int nMLW = m_Map_Static.OffSet(M_MIDDLE_LEFT).Width();
				int nMRW = m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width();
				int nMCW = m_Map_Static.OffSet(M_MIDDLE_CENTER).Width();

				if(nMCW <= 0)
					nMCW = 1;

				xFillsNeeded = (cRect.Width() - (nMLW + nMRW)) / nMCW;


				int y = 0;
				for(int i = 0; i <= yFillsNeeded; i++)
				{
					y = m_Map_Static.OffSet(M_TOP_LEFT).Height() + (m_Map_Static.OffSet(M_MIDDLE_LEFT).Height() * i);
					//__MiddleLeft						
					memDC.BitBlt(0, y, m_Map_Static.OffSet(M_MIDDLE_LEFT).Width(), m_Map_Static.OffSet(M_MIDDLE_LEFT).Height(), &objDC, 
						m_Map_Static.OffSet(M_MIDDLE_LEFT).left, m_Map_Static.OffSet(M_MIDDLE_LEFT).top, SRCCOPY);

					for(int j = 0; j <= xFillsNeeded; j++)
					{
						//__MiddleCenter
						memDC.BitBlt(m_Map_Static.OffSet(M_MIDDLE_LEFT).Width() + (m_Map_Static.OffSet(M_MIDDLE_CENTER).Width() * j), y, 
							m_Map_Static.OffSet(M_MIDDLE_CENTER).Width(), m_Map_Static.OffSet(M_MIDDLE_CENTER).Height(), &objDC, 
							m_Map_Static.OffSet(M_MIDDLE_CENTER).left, m_Map_Static.OffSet(M_MIDDLE_CENTER).top, SRCCOPY);
					}

					//__MiddleRight
					memDC.BitBlt((cRect.Width() - m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width()), y, m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width(), 
						m_Map_Static.OffSet(M_MIDDLE_RIGHT).Height(), &objDC, m_Map_Static.OffSet(M_MIDDLE_RIGHT).left, m_Map_Static.OffSet(M_MIDDLE_RIGHT).top, SRCCOPY);
				}
			}
			else
			{
				memDC.StretchBlt(0, m_Map_Static.OffSet(M_TOP_LEFT).Height(), m_Map_Static.OffSet(M_MIDDLE_LEFT).Width(), yStretch, &objDC, 
					m_Map_Static.OffSet(M_MIDDLE_LEFT).left, m_Map_Static.OffSet(M_MIDDLE_LEFT).top, m_Map_Static.OffSet(M_MIDDLE_LEFT).Width(), 
					m_Map_Static.OffSet(M_MIDDLE_LEFT).Height(), SRCCOPY);
				memDC.StretchBlt(m_Map_Static.OffSet(M_MIDDLE_LEFT).Width(), m_Map_Static.OffSet(M_TOP_LEFT).Height(), xStretch, yStretch, &objDC, 
					m_Map_Static.OffSet(M_MIDDLE_CENTER).left, m_Map_Static.OffSet(M_MIDDLE_CENTER).top, m_Map_Static.OffSet(M_MIDDLE_CENTER).Width(), m_Map_Static.OffSet(M_MIDDLE_CENTER).Height(), SRCCOPY);
				memDC.StretchBlt(cRect.Width() - m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width(), m_Map_Static.OffSet(M_TOP_RIGHT).Height(), 
					m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width(), yStretch, &objDC, m_Map_Static.OffSet(M_MIDDLE_RIGHT).left, m_Map_Static.OffSet(M_MIDDLE_RIGHT).top, 
					m_Map_Static.OffSet(M_MIDDLE_RIGHT).Width(), m_Map_Static.OffSet(M_MIDDLE_RIGHT).Height(), SRCCOPY);
			}

			//__Bottom Part
			memDC.BitBlt(0, cRect.Height() - m_Map_Static.OffSet(M_BOTTOM_LEFT).Height(), m_Map_Static.OffSet(M_BOTTOM_LEFT).Width(), 
				m_Map_Static.OffSet(M_BOTTOM_LEFT).Height(), &objDC, m_Map_Static.OffSet(M_BOTTOM_LEFT).left, m_Map_Static.OffSet(M_BOTTOM_LEFT).top, SRCCOPY);

			if (m_bDrawPattern)
			{
				for(int i = 0; i <= xFillsNeeded; i++)
				{
					memDC.BitBlt(m_Map_Static.OffSet(M_BOTTOM_LEFT).Width() + (m_Map_Static.OffSet(M_BOTTOM_CENTER).Width() * i), cRect.Height() - 
						m_Map_Static.OffSet(M_BOTTOM_CENTER).Height(), m_Map_Static.OffSet(M_BOTTOM_CENTER).Width(), m_Map_Static.OffSet(M_BOTTOM_CENTER).Height(), 
						&objDC, m_Map_Static.OffSet(M_BOTTOM_CENTER).left, m_Map_Static.OffSet(M_BOTTOM_CENTER).top, SRCCOPY);
				}
			}
			else
			{
				memDC.StretchBlt(m_Map_Static.OffSet(M_BOTTOM_LEFT).Width(), cRect.Height() - m_Map_Static.OffSet(M_BOTTOM_CENTER).Height(), 
					xFillsNeeded, m_Map_Static.OffSet(M_BOTTOM_CENTER).Height(),
					&objDC, m_Map_Static.OffSet(M_BOTTOM_CENTER).left, m_Map_Static.OffSet(M_BOTTOM_CENTER).top, m_Map_Static.OffSet(M_BOTTOM_CENTER).Width(), 
					m_Map_Static.OffSet(M_BOTTOM_CENTER).Height(), SRCCOPY);
			}

			memDC.BitBlt((cRect.Width() - m_Map_Static.OffSet(M_BOTTOM_RIGHT).Width()), cRect.Height() - m_Map_Static.OffSet(M_BOTTOM_RIGHT).Height(), 
				m_Map_Static.OffSet(M_BOTTOM_RIGHT).Width(), m_Map_Static.OffSet(M_BOTTOM_RIGHT).Height(), &objDC, m_Map_Static.OffSet(M_BOTTOM_RIGHT).left, m_Map_Static.OffSet(M_BOTTOM_RIGHT).top, SRCCOPY);
		}
		else if(m_sResizeType == RESIZEV)
		{
			//__Top Part
			memDC.BitBlt(0, 0, m_Map_Static.OffSet(M_TOP_LEFT).Width(), m_Map_Static.OffSet(M_TOP_LEFT).Height(), &objDC, m_Map_Static.OffSet(M_TOP_LEFT).left, m_Map_Static.OffSet(M_TOP_LEFT).top, SRCCOPY);


			if (m_bDrawPattern)
			{
			}
			else
			{
				memDC.StretchBlt(0, m_Map_Static.OffSet(M_TOP_LEFT).Height(), m_Map_Static.OffSet(M_MIDDLE_LEFT).Width(), cRect.Height() - 
					(m_Map_Static.OffSet(M_TOP_LEFT).Height()+m_Map_Static.OffSet(M_BOTTOM_LEFT).Height()), &objDC, m_Map_Static.OffSet(M_MIDDLE_LEFT).left, 
					m_Map_Static.OffSet(M_MIDDLE_LEFT).top, m_Map_Static.OffSet(M_MIDDLE_LEFT).Width(), m_Map_Static.OffSet(M_MIDDLE_LEFT).Height(), SRCCOPY);
			}

			//__Bottom Part
			memDC.BitBlt(0, cRect.Height() - m_Map_Static.OffSet(M_BOTTOM_LEFT).Height(), m_Map_Static.OffSet(M_BOTTOM_LEFT).Width(), 
				m_Map_Static.OffSet(M_BOTTOM_LEFT).Height(), &objDC, m_Map_Static.OffSet(M_BOTTOM_LEFT).left, m_Map_Static.OffSet(M_BOTTOM_LEFT).top, SRCCOPY);

		}
		else if(m_sResizeType == RESIZEH)
		{
			//__Top Part
			int xFillsNeeded = (cRect.Width() - (m_Map_Static.OffSet(M_TOP_LEFT).Width() + m_Map_Static.OffSet(M_TOP_RIGHT).Width()));

			//__TopLeft
			memDC.BitBlt(0, 0, m_Map_Static.OffSet(M_TOP_LEFT).Width(), m_Map_Static.OffSet(M_TOP_LEFT).Height(), &objDC, m_Map_Static.OffSet(M_TOP_LEFT).left, m_Map_Static.OffSet(M_TOP_LEFT).top, SRCCOPY);

			//__TopCenter

			if (m_bDrawPattern)
			{
				for(int i = 0; i <= xFillsNeeded; i++){
					memDC.BitBlt(m_Map_Static.OffSet(M_TOP_LEFT).Width() + (m_Map_Static.OffSet(M_TOP_CENTER).Width() * i), 0, 
						m_Map_Static.OffSet(M_TOP_CENTER).Width(), m_Map_Static.OffSet(M_TOP_CENTER).Height(), &objDC, m_Map_Static.OffSet(M_TOP_CENTER).left, m_Map_Static.OffSet(M_TOP_CENTER).top, SRCCOPY);
				}
			}
			else
			{
				memDC.StretchBlt(m_Map_Static.OffSet(M_TOP_LEFT).Width(), 0, xFillsNeeded, m_Map_Static.OffSet(M_TOP_CENTER).Height(), &objDC, 
					m_Map_Static.OffSet(M_TOP_CENTER).left, m_Map_Static.OffSet(M_TOP_CENTER).top, m_Map_Static.OffSet(M_TOP_CENTER).Width(), m_Map_Static.OffSet(M_TOP_CENTER).Height(), SRCCOPY);
			}

			//__TopRight
			memDC.BitBlt((cRect.Width() - m_Map_Static.OffSet(M_TOP_RIGHT).Width()), 0, m_Map_Static.OffSet(M_TOP_RIGHT).Width(),
				m_Map_Static.OffSet(M_TOP_RIGHT).Height(), &objDC, m_Map_Static.OffSet(M_TOP_RIGHT).left, m_Map_Static.OffSet(M_TOP_RIGHT).top, SRCCOPY);

		}
		else
		{
			memDC.BitBlt(0, 0, m_Map_Static.OffSet(M_TOP_LEFT).Width(), 
				m_Map_Static.OffSet(M_TOP_LEFT).Height(), 
				&objDC, 
				m_Map_Static.OffSet(M_TOP_LEFT).left, 
				m_Map_Static.OffSet(M_TOP_LEFT).top, 
				SRCCOPY);
		}

		pDC->BitBlt(0, 0, cRect.Width(), cRect.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();

		objDC.SelectObject(poldtmpbitmap);
		objDC.DeleteDC();

		bmpBuffer.DeleteObject();
	}

	if(_lpCallback){
		_lpCallback(pDC, this); 

	}else{

		CRect rcClient;
		GetClientRect(&rcClient);

		CRect Rng(rcClient.left + 10, rcClient.top, rcClient.right - 10 , rcClient.bottom);
		int nWidth = 180;
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_crTextColor);

		CFont* def_font = static_cast<CFont*>(pDC->SelectObject(&m_Font));
		pDC->SetTextAlign(TA_LEFT);


		int nLen = GetValidTxt(pDC, nWidth, m_strSentence);
		if(nLen == 0){
			pDC->ExtTextOut(Rng.left, Rng.top, ETO_CLIPPED, Rng, m_strSentence, NULL);
		}else{
			CString strDummy = _T("");
			CString strSrc = m_strSentence;
			for(int i=0; i < 10; i++){
				if(strSrc.GetLength() > 0){
					strDummy = _T("");
					nLen = GetValidTxt(pDC, nWidth, strSrc);
					strDummy = strSrc.Left(nLen);
					strSrc = strSrc.Mid(nLen, strSrc.GetLength() - nLen);
					//pDC->ExtTextOut(Rng.left, Rng.top + (20*i)+20, ETO_CLIPPED, Rng, strDummy, NULL);
				}
			}
		}		
		if(m_bText){
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_crTextColor);

			CString strTmp = m_strSentence;

			for(int i=0;i<m_strSentence.GetLength();i++){
				CString strTmp2;
				if(strTmp.Find(_T("\r\n"))>-1){
					strTmp2 = strTmp.Left(strTmp.Find(_T("\r\n"))+2);
					strTmp = strTmp.Right(strTmp.GetLength() - strTmp2.GetLength());
					//strTmp = strTmp
				}
				else{
					strTmp2 = strTmp;
					strTmp = _T("");
				}								

				strTmp2.TrimRight(_T("\r\n"));
				pDC->ExtTextOut(rcClient.left, rcClient.top+i*15, ETO_CLIPPED, rcClient, strTmp2, NULL);
				if(strTmp.GetLength()<1)break;
			}
			//pDC->ExtTextOut(rcClient.left, rcClient.top, ETO_CLIPPED, rcClient, m_strSentence, NULL);
		}
		pDC->SelectObject(def_font);
	}
	//return CxSkinStatic::OnEraseBkgnd(pDC);
	return true;
}

void CxSkinStatic::OnPaint() 
{
	CPaintDC dc(this); 
	//CDC* pDC = GetDC();
	//ReleaseDC(pDC);
}

int CxSkinStatic::GetValidTxt(CDC *pDC, int nPixelWidth, CString strString)
{
	SIZE size;
	int nRes = GetTextExtentPoint32(pDC->m_hDC, strString, strString.GetLength(), &size);
	if (nRes == 0){
		int nLen;
		nLen = strString.GetLength();
		if(nLen > 10){
			return 10;
		}else{
			return nLen;
		}
	}

	if(size.cx <= nPixelWidth)
		return strString.GetLength();

	TCHAR *chrDummy;
	chrDummy = new TCHAR[241];

	int iback = 0;
	for(int i = 0; (i < 240) && (i < strString.GetLength()) ; i++){
		memset(chrDummy, 0x00, sizeof(TCHAR) * (241));

		if(IsDBCSLeadByte(strString[i])){
			i++;
		}

		//_tcsncpy(chrDummy, strString, i+1);
		_tcsncpy_s(chrDummy, 240, strString, i+1);
		nRes = GetTextExtentPoint32(pDC->m_hDC, chrDummy, i, &size);
		if(size.cx > nPixelWidth){
			delete chrDummy;
			return iback+1;
		}

		iback = i;
	}

	delete chrDummy;
	//return i;
	return iback;
}

void CxSkinStatic::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor = crTextColor;
	Invalidate();// Repaint control
}

void CxSkinStatic::SetText(CString strTxt)
{
	if(0 == strTxt.CompareNoCase(m_strSentence)) return;
	m_nXPos = 0;
	m_bRev = false;
	strTxt.Replace(_T("\\"),_T("~~"));
	strTxt.Replace(_T("~~n"),_T("\r\n"));

	strTxt.Replace(_T("~~"),_T("\\"));

	this->m_strSentence = strTxt;
	m_bText = true;

	this->ShowWindow(SW_HIDE);
	this->ShowWindow(SW_SHOW);
	Invalidate(true);


}

CString CxSkinStatic::GetText()
{
	return m_strSentence;
}


int CxSkinStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure

	NONCLIENTMETRICS nm; 
	nm.cbSize = sizeof (NONCLIENTMETRICS); 
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0); 

	memcpy(&lf, &(nm.lfMenuFont), sizeof(LOGFONT));

	lf.lfHeight = 11;
	lf.lfWeight = FW_MEDIUM;

	//#define FW_MEDIUM           500
	//#define FW_SEMIBOLD         600
	//#define FW_BOLD             700

	VERIFY(m_Font.CreateFontIndirect(&lf));


	return 0;
}


void CxSkinStatic::CopyFromBG(CBitmap& bmSkinImage, CRect RTopLeft, CRect RTopCenter, CRect RTopRight, CRect RMiddleLeft, 
	CRect RMiddleCenter, CRect RMiddleRight, CRect RBottomLeft, CRect RBottomCenter, CRect RBottomRight)
{
	if(m_BitBG.m_hObject)
		m_BitBG.DeleteObject();

	BITMAP bmInfo;
	bmSkinImage.GetBitmap(&bmInfo);
	CRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = bmInfo.bmWidth;
	rect.bottom = bmInfo.bmHeight;

	CDC* pDC = GetDC();
	//CSkinUtil skinUtil;
	//skinUtil.CopyBitmap(pDC, m_BitBG, bmSkinImage, rect);
	m_BitBG.Attach(bmSkinImage.m_hObject);
	//int nRet = ReleaseDC(pDC);
	ReleaseDC(pDC);

	m_Map_Static.SetRect(0, RTopLeft);
	m_Map_Static.SetRect(1, RTopCenter);
	m_Map_Static.SetRect(2, RTopRight);

	m_Map_Static.SetRect(3, RMiddleLeft);
	m_Map_Static.SetRect(4, RMiddleCenter);
	m_Map_Static.SetRect(5, RMiddleRight);

	m_Map_Static.SetRect(6, RBottomLeft);
	m_Map_Static.SetRect(7, RBottomCenter);
	m_Map_Static.SetRect(8, RBottomRight);

	m_Map_Static.m_bInit = true;
}


BOOL CxSkinStatic::AttachCallback(LPFN_DrawStatic _CallFunc)
{
	_lpCallback = _CallFunc;
	return 0;
}

void CxSkinStatic::Pause(int nPause)
{
	Sleep(nPause);
}


void CxSkinStatic::SetShiftTime(int nMSec)
{
	if(nMSec <= 0){
		m_nMSec = 120;
	}else{
		m_nMSec = nMSec;
	}
}

void CxSkinStatic::SetAutoShift(bool bShift)
{
	m_bAutoShift = bShift;
	SetTimer(ID_TXTSHIFT_TIMER, m_nMSec, NULL);
}


void CxSkinStatic::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == ID_TXTSHIFT_TIMER){
		if(m_bRev){
			m_nXPos += 1;
		}else{
			m_nXPos -= 1;
		}
		Invalidate(true);
	}

	CStatic::OnTimer(nIDEvent);
}

void CxSkinStatic::OnClose()
{
	if(m_bAutoShift)
		KillTimer(ID_TXTSHIFT_TIMER);

	CStatic::OnClose();
}

void CxSkinStatic::SetFontType(CString strFont,COLORREF corFont,int nFontSize,BOOL bBold,BOOL bUnderLine)
{
	m_crTextColor = corFont;
	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure

	NONCLIENTMETRICS nm; 
	nm.cbSize = sizeof (NONCLIENTMETRICS); 
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0); 

	memcpy(&lf, &(nm.lfMenuFont), sizeof(LOGFONT));

	if(0 == strFont.CompareNoCase(_T(""))) strFont = _T("MS Sans Serif");
	lstrcpy(lf.lfFaceName, strFont);

	lf.lfHeight = nFontSize;
	if(bBold)	lf.lfWeight = FW_BOLD;
	else		lf.lfWeight = FW_MEDIUM;

	//#define FW_MEDIUM           500
	//#define FW_SEMIBOLD         600
	//#define FW_BOLD             700

	lf.lfUnderline	= bUnderLine;

	VERIFY(m_Font.CreateFontIndirect(&lf));
}

BOOL CxSkinStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{

	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}
