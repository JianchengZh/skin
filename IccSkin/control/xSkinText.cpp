// xText.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinText.h"

#include "Define.h"

// CxSkinText

IMPLEMENT_DYNAMIC(CxSkinText, CStatic)

	CxSkinText::CxSkinText()
	:m_strText(_T(""))
	,m_clrText(COLOR_BLACK)
	,m_strFontName(FONT_NAME)
	,m_nFontSize(FONT_SIZE)
	,m_bFontBold(false)
	,m_bUnderline(false)
	,m_bShowToolTip(FALSE)
{
		m_strText = _T("");

	COLORREF col = 0;

	col = GetSysColor(COLOR_INFOTEXT);
	m_clrTTFont	= col;				// ToolTip Foreground color

	col = GetSysColor(COLOR_INFOBK);
	m_clrTTBckGnd = col;			// ToolTip Background color

	m_bToolTipCreated = FALSE;
}

CxSkinText::~CxSkinText()
{
	if(m_Font.m_hObject)
		m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CxSkinText, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

// CxSkinText message handlers

void CxSkinText::Init(void)
{
	SetFontType(m_strFontName, m_nFontSize, m_bFontBold, m_bUnderline, m_clrText);

	//CRect rect;
	//GetWindowRect(&rect);

	//CWnd *pParent = GetParent();
	//ASSERT(pParent);
	//pParent->ScreenToClient(&rect);  //convert our corrdinates to our parents

	//CDC *pParentDC = pParent->GetDC();

	//CDC memDC;
	//memDC.CreateCompatibleDC(pParentDC);
	//m_bmBG.CreateCompatibleBitmap(pParentDC, rect.Width(), rect.Height());

	//CBitmap *pOldBmp = memDC.SelectObject(&m_bmBG);

	//memDC.BitBlt(0,0,rect.Width(),rect.Height(), pParentDC, rect.left, rect.top,SRCCOPY);

	////pDC->BitBlt(0, 0, rect.Width(), rect.Height(),&memDC, 0, 0,SRCCOPY);

	//memDC.SelectObject(pOldBmp);

	//pParent->ReleaseDC(pParentDC);
}

void CxSkinText::SetText(CString strText)
{
	this->m_strText = strText;
	SetWindowText(m_strText);
	Invalidate();
}
CString CxSkinText::GetText(void)
{
	GetWindowText(m_strText);
	return m_strText;
}

void CxSkinText::SetFontType(CString strFontName, int nFontSize, BOOL bBold, BOOL bUnderline, COLORREF clrText)
{
	m_clrText = clrText;

	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure

	NONCLIENTMETRICS nm;
	nm.cbSize = sizeof (NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0);

	memcpy(&lf, &(nm.lfMenuFont), sizeof(LOGFONT));

	m_strFontName = strFontName;

	if(0 == m_strFontName.CompareNoCase(_T(""))) m_strFontName = _T("MS Sans Serif");
	lstrcpy(lf.lfFaceName, m_strFontName);

	m_nFontSize = nFontSize;
	lf.lfHeight = m_nFontSize;

	m_bFontBold = bBold;
	if(m_bFontBold)		lf.lfWeight = FW_BOLD;
	else				lf.lfWeight = FW_MEDIUM;

	//#define FW_MEDIUM           500
	//#define FW_SEMIBOLD         600
	//#define FW_BOLD             700

	m_bUnderline = bUnderline;
	lf.lfUnderline	= m_bUnderline;

	VERIFY(m_Font.CreateFontIndirect(&lf));
	SetFont(&m_Font, TRUE);
}

HBRUSH CxSkinText::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_clrText);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

BOOL CxSkinText::OnEraseBkgnd(CDC* pDC)
{
	
	//CBitmap bitmap;

	//CRect rect;
	//GetWindowRect(&rect);

	//CWnd *pParent = GetParent();
	//ASSERT(pParent);
	//pParent->ScreenToClient(&rect);  //convert our corrdinates to our parents

	//CDC *pParentDC = pParent->GetDC();

	//CDC memDC;
	//memDC.CreateCompatibleDC(pParentDC);
	//bitmap.CreateCompatibleBitmap(pParentDC, rect.Width(), rect.Height());

	//CBitmap *pOldBmp = memDC.SelectObject(&bitmap);

	//memDC.BitBlt(0,0,rect.Width(),rect.Height(), pParentDC, rect.left, rect.top,SRCCOPY);

	//pDC->BitBlt(0, 0, rect.Width(), rect.Height(),&memDC, 0, 0,SRCCOPY);

	//memDC.SelectObject(pOldBmp);

	//pParent->ReleaseDC(pParentDC);

	//if(m_strText != _T(""))
	//{
	//	CRect rcClient;
	//	GetClientRect(&rcClient);

	//	pDC->SetBkMode(TRANSPARENT);
	//	pDC->SetTextColor(m_clrText);

	//	CFont* def_font = static_cast<CFont*>(pDC->SelectObject(&m_Font));
	//	pDC->SetTextAlign(TA_LEFT);

	//	pDC->ExtTextOut(rcClient.left, rcClient.top, ETO_CLIPPED, rcClient, m_strText, NULL);

	//	pDC->SelectObject(def_font);
	//}

	//return TRUE;

	return CStatic::OnEraseBkgnd(pDC);
}

LRESULT CxSkinText::OnSetText(WPARAM wParam,LPARAM lParam)
{
	LRESULT Result = Default();
	CRect Rect;
	GetWindowRect(&Rect);
	GetParent()->ScreenToClient(&Rect);
	GetParent()->InvalidateRect(&Rect);
	GetParent()->UpdateWindow();
	return Result;

	//use erasebkgnd
	//LRESULT Result = Default();
	//Invalidate();
	//UpdateWindow();
	//return Result;
}


BOOL CxSkinText::SetToolTipFGColor(COLORREF crTTFgColor)
{
	m_clrTTFont = crTTFgColor;

	if(m_bToolTipCreated){
		m_ToolTip.SetTipTextColor(m_clrTTFont);
	}

	return(TRUE);
}

BOOL CxSkinText::SetToolTipBGColor(COLORREF crTTBgColor)
{
	m_clrTTBckGnd = crTTBgColor;

	if(m_bToolTipCreated){
		m_ToolTip.SetTipBkColor(m_clrTTBckGnd);
	}

	return(TRUE);
}

BOOL CxSkinText::SetToolTipText(CString strText)
{
	if(m_bShowToolTip == FALSE) return FALSE;

	if(m_bToolTipCreated){
		//Window is existing
		CDC *pDC = GetDC();
		if(pDC == NULL){
			ASSERT(FALSE);
			return(FALSE);
		}
		//StaticControl client rectangle
		CRect rcStcCtrl(0,0,0,0);		
		GetClientRect(rcStcCtrl);

		//Store size of the formatted text to be drawn in static control.
		CRect rcNew(0, 0 ,0, 0);	
		CFont *pfontOld = NULL;
		//if(m_bFontInit){
			pfontOld = (CFont *)pDC->SelectObject(&m_Font);
		//}
		pDC->DrawText(m_strText, &rcNew, DT_CALCRECT); 
		//if(m_bFontInit){
			pDC->SelectObject(pfontOld);
		//}
		ReleaseDC(pDC);

		//Check if the tool tip is to be shown
		if(rcNew.right > rcStcCtrl.right){
			m_ToolTip.UpdateTipText(strText, this, TOOLTIP_ID);
			m_ToolTip.Activate(TRUE);
		}else{
			m_ToolTip.UpdateTipText(_T(""), this, TOOLTIP_ID);
			m_ToolTip.Activate(FALSE);
		}
	}

	return(TRUE);
}

void CxSkinText::PreSubclassWindow()
{
		//Modify windows style to get mouse notifications.
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	////Set the font
	//CFont *pFont = NULL;
	//pFont = GetFont();
	//ASSERT(pFont);
	//if(pFont){
	//	LOGFONT LogFont = {0};
	//	pFont->GetLogFont(&LogFont);
	//	if(m_font.CreateFontIndirect(&LogFont)){
	//		m_bFontInit = TRUE;
	//	}else{
	//		ASSERT(FALSE);
	//	}
	//}

	// Create the tooltip
    CRect rect; 
    GetClientRect(rect);
    if(m_ToolTip.Create(this)){
		if(m_strTT.IsEmpty()){
			CString strTemp;
			GetWindowText(strTemp);
			m_strTT = strTemp;
		}
		if(m_ToolTip.AddTool(this, (LPCTSTR)m_strTT, rect, TOOLTIP_ID)){
			m_bToolTipCreated = TRUE;
			SetToolTipText(m_strTT);
		}else{
			ASSERT(FALSE);
		}
	}else{
		ASSERT(FALSE);
	}

	CStatic::PreSubclassWindow();
}

BOOL CxSkinText::PreTranslateMessage(MSG* pMsg) 
{
	if(m_bToolTipCreated){
		m_ToolTip.RelayEvent(pMsg);
	}
	return CStatic::PreTranslateMessage(pMsg);
}
