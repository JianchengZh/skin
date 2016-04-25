// DFSkinComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinComboBox.h"

#include "Define.h"
#include "SkinUtil.h"

int nListBoxTextLeft = 0;
static WNDPROC m_pWndProc = 0;

extern "C" LRESULT FAR PASCAL xSkinComboListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//TRACE("xSkinComboListBoxProc 0x%.4X\n",nMsg);
	return CallWindowProc(m_pWndProc, hWnd, nMsg, wParam, lParam);
}

// CDFSkinComboBox

IMPLEMENT_DYNAMIC(CxSkinComboBox, CComboBox)
	CxSkinComboBox::CxSkinComboBox()
	:m_clrNormalBG(COLOR_WHITE)
	,m_clrNormalText(COLOR_BLACK)
	,m_clrFocusBG(COLOR_WHITE)
	,m_clrFocusText(COLOR_BLACK)
	,m_nMarginLeft(2)
	,m_nMarginTop(2)
	,m_nMarginBottom(2)
	,m_nMarginRight(14)
	,m_nEditHeight(18)
	,m_nDrawMode(DRAWMODE_IMAGE)
	,m_bCustomList(FALSE)
{
}

CxSkinComboBox::~CxSkinComboBox()
{

}

BEGIN_MESSAGE_MAP(CxSkinComboBox, CComboBox)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CDFSkinComboBox message handlers

LRESULT CxSkinComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam)
{
	// Here we need to get a reference to the listbox of the combobox (the dropdown part).
	//We can do it using TRACE("OnCtlColorListBox()\n");

	if (this->m_skinListBox.m_hWnd == 0)
	{
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) 
		{
			// Save the handle
			m_skinListBox.m_hWnd = hWnd;
			CRect rect;
			m_skinListBox.GetClientRect(&rect);

			if(m_bCustomList)
			{
				// Subclass ListBox
				m_pWndProc = (WNDPROC)GetWindowLong(m_skinListBox.m_hWnd, GWL_WNDPROC);
				SetWindowLong(m_skinListBox.m_hWnd, GWL_WNDPROC, (LONG)xSkinComboListBoxProc);
			}
		}
	}

	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

BOOL CxSkinComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// Remove the CBS_SIMPLE and CBS_DROPDOWN styles and add the one I'm designed for
	dwStyle &= ~0xF;
	dwStyle |= CBS_DROPDOWNLIST;

	// Use default strings. We need the itemdata to store the state of the lamps
	//dwStyle |= CBS_HASSTRINGS;
	return CComboBox::Create(dwStyle, rect, pParentWnd, nID);
}

void CxSkinComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
	if( lpDrawItemStruct->itemID == -1 )
		return;

	if(!IsWindow(m_skinListBox.m_hWnd))
		return;

	CDC pDC;	
	pDC.Attach(lpDrawItemStruct->hDC);
	CBrush pBrush;
	CString strItem;

	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		pBrush.CreateSolidBrush(m_clrFocusBG);
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(m_clrFocusText);
	}
	else
	{
		pBrush.CreateSolidBrush(m_clrNormalBG);
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(m_clrNormalText);
	}


	if(m_bCustomList)
	{

		DrawComboListLeft();

		if( lpDrawItemStruct->itemID == 0 )
			DrawComboListTop();

		if( lpDrawItemStruct->itemID == m_skinListBox.GetCount()-1 )
			DrawComboListBottom();

		DrawComboListRight();
	}

	// Copy the text of the item to a string
	this->GetLBText(lpDrawItemStruct->itemID, strItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text after the images left postion
	lpDrawItemStruct->rcItem.left = nListBoxTextLeft;

	pDC.DrawText(strItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);

	if(lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
		Invalidate();
}

void CxSkinComboBox::OnDestroy()
{
	CComboBox::OnDestroy();

	FreeSkin();

	if(m_Font.m_hObject)
		m_Font.DeleteObject();
}

void CxSkinComboBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CComboBox::OnPaint() for painting messages
	CRect rcClient;//rect;
	CDC memDC, objDC;

	GetClientRect(&rcClient);
	memDC.CreateCompatibleDC(&dc);
	objDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());

	BITMAP pBmLeft, pBmCenter, pBmRight;
	memset((void *)&pBmLeft,0,sizeof(pBmLeft));
	memset((void *)&pBmCenter,0,sizeof(pBmCenter));
	memset((void *)&pBmRight,0,sizeof(pBmRight));

	m_bmComboBoxLeft.GetBitmap(&pBmLeft);
	m_bmComboBoxCenter.GetBitmap(&pBmCenter);
	m_bmComboBoxRight.GetBitmap(&pBmRight);

	CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&bitmap);

	objDC.SelectObject(m_bmComboBoxLeft);
	memDC.BitBlt(rcClient.left, rcClient.top, pBmLeft.bmWidth, pBmLeft.bmHeight, &objDC, 0, 0, SRCCOPY);

	objDC.SelectObject(m_bmComboBoxCenter);
	memDC.StretchBlt(rcClient.left + pBmLeft.bmWidth, rcClient.top, rcClient.right - pBmLeft.bmWidth - pBmRight.bmWidth, 
		pBmCenter.bmHeight, &objDC, 0, 0, pBmCenter.bmWidth, pBmCenter.bmHeight, SRCCOPY);

	objDC.SelectObject(m_bmComboBoxRight);
	memDC.BitBlt(rcClient.right - pBmRight.bmWidth, rcClient.top, pBmRight.bmWidth, pBmRight.bmHeight, &objDC, 0, 0, SRCCOPY);

	CString strTxt;
	GetWindowText(strTxt);

	//rect.left += nListBoxTextLeft;
	CRect rcText = rcClient;
	rcText.top += m_nMarginTop;
	rcText.left += m_nMarginLeft;
	rcText.right -= m_nMarginRight;
	rcText.bottom -= m_nMarginBottom;

	//Select the font only if list box is created
	//if( m_skinListBox )
	//	dc.SelectObject(m_skinListBox.GetFont());

	//memdc에 텍스트 출력
	CFont *pOldFont = memDC.SelectObject(&m_Font);
	memDC.SetBkMode(TRANSPARENT);

	RECT rcRepaint;
	GetClientRect(&rcRepaint);
	rcRepaint.left = rcRepaint.left + pBmLeft.bmWidth;
	rcRepaint.right = rcRepaint.right - pBmRight.bmWidth;
	rcRepaint.top += m_nMarginTop;
	rcRepaint.bottom -= m_nMarginBottom;

	//Set the normal/highlight color when its repainted
	if( (GetFocus()) && (GetFocus()->m_hWnd == m_hWnd) )
	{
		if(m_nDrawMode == DRAWMODE_RGB)
		{
			memDC.FillSolidRect(&rcRepaint, m_clrFocusBG);
		}
		memDC.SetTextColor(m_clrFocusText);
	}
	else
	{
		if(m_nDrawMode == DRAWMODE_RGB)
		{
			memDC.FillSolidRect(&rcRepaint, m_clrNormalBG);
		}

		memDC.SetTextColor(m_clrNormalText);

	}

	if(strTxt.Find(_T("&")) >= 0)
		strTxt.Replace(_T("&"), _T("&&"));

	memDC.DrawText(strTxt, &rcText, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );	

	dc.BitBlt(0,0,rcClient.Width(), rcClient.Height(), &memDC, 0,0,SRCCOPY);

	memDC.SelectObject(pOldFont);
	memDC.SelectObject(pOldBitmap);

	memDC.DeleteDC();

	if(m_skinListBox)
	{
		CRect rcList;
		CDC *pDC = m_skinListBox.GetDC();
		m_skinListBox.GetClientRect(&rcList);

		if(m_bCustomList)
		{			
			CBrush pBrush;
			pBrush.CreateSolidBrush(m_clrNormalBG);
			m_skinListBox.GetDC()->FillRect(&rcList, &pBrush);

			DrawComboListLeft();
			DrawComboListRight();
			DrawComboListTop();
			DrawComboListBottom();
		}

		RECT rcItemData;
		CString strItemData;		
		for(int i =0;i<m_skinListBox.GetCount();i++)
		{
			m_skinListBox.GetText(i, strItemData);
			m_skinListBox.GetItemRect(i, &rcItemData);

			////아이스테이션용 아이템 높이가 변경을 하면 정상적으로 출력이 안됨(까지의 라인을 주석처리하고 함)
			//if (i == 0 && 0 == strItemData.CompareNoCase(_T(" ")))
			//{
			//	pDC->FillSolidRect(&rcItemData, RGB(245,245,245));
			//}

			rcItemData.left += pBmLeft.bmWidth;
			pDC->SetBkMode(TRANSPARENT);
			nListBoxTextLeft = rcItemData.left;
			m_skinListBox.SetFont(&m_Font);
			m_skinListBox.SetBackGroundColor(m_clrNormalBG);
			pDC->SelectObject(m_skinListBox.GetFont());
			pDC->DrawText(strItemData, &rcItemData, DT_VCENTER | DT_SINGLELINE);

		}
	}
}

void CxSkinComboBox::SetNormalColor(COLORREF clrBG, COLORREF clrText)
{
	m_clrNormalBG	= clrBG;
	m_clrNormalText	= clrText;
}

void CxSkinComboBox::SetFocusColor(COLORREF clrBG, COLORREF clrText)
{
	m_clrFocusBG	= clrBG;
	m_clrFocusText	= clrText;
}

void CxSkinComboBox::DrawComboListLeft(int nHeight)
{
	CRect rect;
	CDC memDC;
	CDC *pDC = m_skinListBox.GetDC();
	memDC.CreateCompatibleDC(pDC);
	m_skinListBox.GetClientRect(&rect);

	BITMAP bmLine;

	m_bmComboListLeft.GetBitmap(&bmLine);
	memDC.SelectObject(m_bmComboListLeft);

	pDC->StretchBlt(rect.left,rect.top,bmLine.bmWidth,rect.bottom,&memDC,0,0,bmLine.bmWidth,bmLine.bmHeight,SRCCOPY);

}

void CxSkinComboBox::DrawComboListTop(int nWidth)
{
	CRect rect;
	CDC memDC;
	CDC *pDC = m_skinListBox.GetDC();
	memDC.CreateCompatibleDC(pDC);
	m_skinListBox.GetClientRect(&rect);

	BITMAP bmLine;

	m_bmComboListTop.GetBitmap(&bmLine);
	memDC.SelectObject(m_bmComboListTop);

	pDC->StretchBlt(rect.left,rect.top,rect.right,bmLine.bmHeight,&memDC,0,0,bmLine.bmWidth,bmLine.bmHeight,SRCCOPY);
}

void CxSkinComboBox::DrawComboListBottom(int nWidth)
{
	CRect rect;
	CDC memDC;
	CDC *pDC = m_skinListBox.GetDC();
	memDC.CreateCompatibleDC(pDC);
	m_skinListBox.GetClientRect(&rect);

	BITMAP bmLine;

	m_bmComboListBottom.GetBitmap(&bmLine);
	memDC.SelectObject(m_bmComboListBottom);

	pDC->StretchBlt(rect.left,rect.bottom-bmLine.bmHeight,rect.right,bmLine.bmHeight,&memDC,0,0,bmLine.bmWidth,bmLine.bmHeight,SRCCOPY);
}

void CxSkinComboBox::DrawComboListRight(int nHeight)
{
	CRect rect;
	CDC memDC;
	CDC *pDC = m_skinListBox.GetDC();
	memDC.CreateCompatibleDC(pDC);
	m_skinListBox.GetClientRect(&rect);

	BITMAP bmLine;

	m_bmComboListRight.GetBitmap(&bmLine);
	memDC.SelectObject(m_bmComboListRight);

	pDC->StretchBlt(rect.right-bmLine.bmWidth,rect.top,bmLine.bmWidth,rect.bottom,&memDC,0,0,bmLine.bmWidth,bmLine.bmHeight,SRCCOPY);
}

void CxSkinComboBox::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	RECT rcRepaint;
	GetClientRect(&rcRepaint);
	rcRepaint.left += m_nMarginLeft;
	rcRepaint.right -= m_nMarginRight;
	rcRepaint.top += m_nMarginTop;
	rcRepaint.bottom -= m_nMarginBottom;

	CDC *pDC = GetDC();
	if(m_nDrawMode == DRAWMODE_RGB)
	{
		pDC->FillSolidRect(&rcRepaint, m_clrNormalBG);
	}
	else
	{
		RECT rect;
		CDC memDC;

		GetClientRect(&rect);
		memDC.CreateCompatibleDC(pDC);

		BITMAP pBmLeft, pBmCenter, pBmRight;
		memset((void *)&pBmLeft,0,sizeof(pBmLeft));
		memset((void *)&pBmCenter,0,sizeof(pBmCenter));
		memset((void *)&pBmRight,0,sizeof(pBmRight));

		memDC.SelectObject(m_bmComboBoxLeft);
		m_bmComboBoxLeft.GetBitmap(&pBmLeft);
		pDC->BitBlt(rect.left, rect.top, pBmLeft.bmWidth, pBmLeft.bmHeight, &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(m_bmComboBoxCenter);
		m_bmComboBoxCenter.GetBitmap(&pBmCenter);
		pDC->StretchBlt(rect.left + pBmLeft.bmWidth, rect.top, rect.right - pBmLeft.bmWidth - pBmRight.bmWidth, pBmCenter.bmHeight, &memDC, 0, 0, pBmCenter.bmWidth, pBmCenter.bmHeight, SRCCOPY);

		memDC.SelectObject(m_bmComboBoxRight);
		m_bmComboBoxRight.GetBitmap(&pBmRight);
		pDC->BitBlt(rect.right - pBmRight.bmWidth, rect.top, pBmRight.bmWidth, pBmRight.bmHeight, &memDC, 0, 0, SRCCOPY);

		memDC.DeleteDC();
	}

	pDC->SetTextColor(m_clrNormalText);

	//Select the font only if list box is created
	pDC->SelectObject(&m_Font);
	pDC->SetBkMode(TRANSPARENT);

	CString strText;
	GetWindowText(strText);
	CRect rect;
	GetClientRect(&rect);

	rect.top += m_nMarginTop;
	rect.left += m_nMarginLeft;
	rect.right -= m_nMarginRight;
	rect.bottom -= m_nMarginBottom;

	pDC->DrawText(strText, &rect, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
}

void CxSkinComboBox::SetMargin(int left, int top, int bottom, int right)
{
	m_nMarginLeft	= left;
	m_nMarginTop	= top;
	m_nMarginBottom	= bottom;
	m_nMarginRight	= right;
}

void CxSkinComboBox::SetDrawMode(int nMode, BOOL bCustomList)
{
	m_nDrawMode = nMode;
	m_bCustomList = bCustomList;
}

void CxSkinComboBox::CopyFrom(CBitmap& bmSkinImage, CRect Rect_BoxL, CRect Rect_BoxC, CRect Rect_BoxRNM, CRect Rect_BoxROV, CRect Rect_BoxRDN, CRect Rect_BoxRDS, CRect Rect_ListL, CRect Rect_ListT, CRect Rect_ListB, CRect Rect_ListR)
{
	FreeSkin();

	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmComboBoxLeft, bmSkinImage, Rect_BoxL);
	skinUtil.CopyBitmap(pDC, m_bmComboBoxCenter, bmSkinImage, Rect_BoxC);
	skinUtil.CopyBitmap(pDC, m_bmComboBoxRight, bmSkinImage, Rect_BoxRNM);
	skinUtil.CopyBitmap(pDC, m_bmComboListLeft, bmSkinImage, Rect_ListL);
	skinUtil.CopyBitmap(pDC, m_bmComboListTop, bmSkinImage, Rect_ListT);
	skinUtil.CopyBitmap(pDC, m_bmComboListBottom, bmSkinImage, Rect_ListB);
	skinUtil.CopyBitmap(pDC, m_bmComboListRight, bmSkinImage, Rect_ListR);

	ReleaseDC(pDC);
}

//비트맵 해제
void CxSkinComboBox::FreeSkin(void)
{
	if(m_bmComboBoxLeft.m_hObject)		m_bmComboBoxLeft.DeleteObject();
	if(m_bmComboBoxCenter.m_hObject)	m_bmComboBoxCenter.DeleteObject();
	if(m_bmComboBoxRight.m_hObject)		m_bmComboBoxRight.DeleteObject();
	if(m_bmComboListLeft.m_hObject)		m_bmComboListLeft.DeleteObject();
	if(m_bmComboListTop.m_hObject)		m_bmComboListTop.DeleteObject();
	if(m_bmComboListBottom.m_hObject)	m_bmComboListBottom.DeleteObject();
	if(m_bmComboListRight.m_hObject)	m_bmComboListRight.DeleteObject();

}

void CxSkinComboBox::OnSetFocus(CWnd* pOldWnd)
{
	CComboBox::OnSetFocus(pOldWnd);
}

void CxSkinComboBox::Init(void)
{
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
	int nH = m_nEditHeight - 6;
	SetItemHeight(-1, nH);
}

void CxSkinComboBox::SetValue(CString val)
{
	if(val.IsEmpty() || val.Find(_T("|")) <= 0) return;
	SetRedraw(FALSE);
	CString tmp;
	int i = 0;
	while(TRUE == ::AfxExtractSubString(tmp, val, i, _T('|')))
	{
		AddString(tmp);
		i++;
	}
	SetRedraw(TRUE);
}
