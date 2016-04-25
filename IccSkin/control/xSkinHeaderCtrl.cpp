// xSkinHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinHeaderCtrl.h"

#include "Define.h"
#include "XMemdc.h"

// CxSkinHeaderCtrl

IMPLEMENT_DYNAMIC(CxSkinHeaderCtrl, CHeaderCtrl)

	CxSkinHeaderCtrl::CxSkinHeaderCtrl()
	:m_bDragging(false)
	,m_bShowCheckbox(false)
	,m_bShowSort(false)
	,m_bSortAsc(true)
	,m_bAllChecked(false)
	,m_bMouseDown(false)
	,m_bMouseOver(false)
	,m_bSelected(false)
	,m_clrText(COLOR_BLACK)
	,m_clrBG(COLOR_WHITE)
	,m_nCheckBoxWidth(0)
	,m_nCheckBoxHeight(0)
	,m_nCenterWidth(0)
	,m_nCenterHeight(0)
	,m_nDividerWidth(0)
	,m_nDividerHeight(0)
	,m_nSortCol(-1)
	,m_nDragCol(-1)
	,m_strFont(FONT_NAME)
	,m_nFontSize(FONT_SIZE)
	,m_bFontBold(false)
	,m_ListWnd(NULL)
	,m_clrSortMask(COLOR_MAGENTA)
	,m_bUseDrag(false)
	,m_nCheckBoxIdx(0)
	,m_nCheckBoxMargin(5)
	,m_nHeight(20)
{
}

CxSkinHeaderCtrl::~CxSkinHeaderCtrl()
{
	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	FreeSkin();
}

BEGIN_MESSAGE_MAP(CxSkinHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)//헤더 높이 설정
END_MESSAGE_MAP()

// CxSkinHeaderCtrl message handlers

void CxSkinHeaderCtrl::FreeSkin(void)
{
	if(m_bmChecker.m_hObject)
		m_bmChecker.DeleteObject();
	if(m_bmUnChecker.m_hObject)
		m_bmUnChecker.DeleteObject();

	if( m_bmLeft.m_hObject)
		m_bmLeft.DeleteObject();

	if(m_bmCenter.m_hObject)
		m_bmCenter.DeleteObject();
	if(m_bmCenterOver.m_hObject)
		m_bmCenterOver.DeleteObject();
	if(m_bmCenterDown.m_hObject)
		m_bmCenterDown.DeleteObject();
	if(m_bmCenterSelect.m_hObject)
		m_bmCenterSelect.DeleteObject();

	if(m_bmDivider.m_hObject)
		m_bmDivider.DeleteObject();
	if(m_bmDividerDown.m_hObject)
		m_bmDividerDown.DeleteObject();

	if(m_bmRight.m_hObject)
		m_bmRight.DeleteObject();

	if(m_bmSortAsc.m_hObject)
		m_bmSortAsc.DeleteObject();
	if(m_bmSortDesc.m_hObject)
		m_bmSortDesc.DeleteObject();
}

void CxSkinHeaderCtrl::SetCheckWH(int nWidth, int nHeigth)
{
	m_nCheckBoxWidth	= nWidth;
	m_nCheckBoxHeight	= nHeigth;
}

void CxSkinHeaderCtrl::SetBkWH(int nLeftWidth, int nCenterWidth, int nRightWidth, int nDividerWidth, int nHeight)
{
	m_nLeftWidth	= nLeftWidth;
	m_nCenterWidth	= nCenterWidth;
	m_nCenterHeight	= nHeight;
	m_nRightWidth	= nRightWidth;
	m_nDividerWidth	= nDividerWidth;
	m_nDividerHeight= nHeight;
}

void CxSkinHeaderCtrl::SetSortWH(int nWidth, int nHeigth)
{
	m_nSortWidth		= nWidth;
	m_nSortHeight		= nHeigth;
}

void CxSkinHeaderCtrl::SetSortImage(int nColumn, BOOL bAscend)
{
	int nPreCol = m_nSortCol;

	m_nSortCol = nColumn;
	m_bSortAsc = bAscend;

	m_bSelected = TRUE;

	Invalidate();
}

void CxSkinHeaderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CSkinListCtrlHeader::OnPaint() for painting messages
	CRect rcClient;
	GetClientRect(&rcClient);

	CXMemDC memDC(&dc, &rcClient, TRUE);
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);

	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(m_clrText);
	memDC.FillSolidRect(rcClient, m_clrBG);

	CBitmap* pOldbmp = bitmapDC.SelectObject(&m_bmCenter);

	//Draw Center
	memDC.StretchBlt(0, 0, rcClient.Width(), m_nCenterHeight, &bitmapDC, 0, 0, m_nCenterWidth, m_nCenterHeight, SRCCOPY);

	int nItems = GetItemCount();

	int nDividerIdx = -1;
	for(int i = 0; i < nItems; i++)
	{
		CRect rcItem;
		GetItemRect(i, &rcItem);

		CRect rcChkbox;;
		rcChkbox = rcItem;

		//Draw Center
		if(m_bMouseOver)
		{
			bitmapDC.SelectObject(&m_bmCenterOver);
			memDC.StretchBlt(rcItem.left+m_nDividerWidth, rcItem.top, rcItem.Width(), m_nCenterHeight, &bitmapDC, 0, 0, m_nCenterWidth, m_nCenterHeight, SRCCOPY);
		}
		else if(m_bMouseDown)
		{
			bitmapDC.SelectObject(&m_bmCenterDown);
			memDC.StretchBlt(rcItem.left+m_nDividerWidth, rcItem.top, rcItem.Width(), m_nCenterHeight, &bitmapDC, 0, 0, m_nCenterWidth, m_nCenterHeight, SRCCOPY);
		}
		else if(m_bSelected && m_nSortCol == i && m_bShowSort)
		{
			bitmapDC.SelectObject(&m_bmCenterSelect);
			memDC.StretchBlt(rcItem.left+m_nDividerWidth, rcItem.top, rcItem.Width(), m_nCenterHeight, &bitmapDC, 0, 0, m_nCenterWidth, m_nCenterHeight, SRCCOPY);
		}
		else
		{
			bitmapDC.SelectObject(&m_bmCenter);
			memDC.StretchBlt(rcItem.left+m_nDividerWidth, rcItem.top, rcItem.Width(), m_nCenterHeight, &bitmapDC, 0, 0, m_nCenterWidth, m_nCenterHeight, SRCCOPY);
		}

		//체크박스 위치
		if(i == m_nCheckBoxIdx && m_bShowCheckbox)
		{
			rcChkbox.left	= rcItem.left + m_nCheckBoxMargin;
			rcChkbox.right	= rcChkbox.left + m_nCheckBoxWidth;
			rcChkbox.top	= (rcItem.Height() - m_nCheckBoxHeight)/2;
			rcChkbox.bottom	= rcChkbox.top + m_nCheckBoxHeight;

			//check status
			if(m_bAllChecked)
			{
				bitmapDC.SelectObject(&m_bmChecker);
			}else{
				bitmapDC.SelectObject(&m_bmUnChecker);
			}

			//Draw Checkbox
			memDC.BitBlt(rcChkbox.left, rcChkbox.top, m_nCheckBoxWidth, m_nCheckBoxHeight, &bitmapDC, 0, 0, SRCCOPY);
		}

		//Draw Divider
		bitmapDC.SelectObject(&m_bmDivider);
		memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);

		//구현중 : 컬럼이동시 Divider Down 이미지 그리기
		//if(i != nItems)
		//{
		//	if(m_bDragging && m_bMouseDown)
		//	{
		//		CPoint pt;
		//		GetCursorPos(&pt);

		//		ScreenToClient(&pt);

		//		GetItemRect(i, &rcItem);

		//		if(pt.y >= rcItem.top && pt.y <= rcItem.bottom)
		//		{
		//			if(pt.x >= rcItem.left && pt.x <= rcItem.right)
		//			{
		//				nDividerIdx = i;
		//			}
		//			//int nHalf = rcItem.left + (rcItem.Width() /2);

		//			//CString strTmp;
		//			//strTmp.Format(_T("i: %d, half : %d, mouse : %d\n"), i, nHalf, pt.x);
		//			//::OutputDebugString(strTmp);

		//			//if(pt.x >= nHalf)//반보다 큰경우
		//			//{
		//			//	bitmapDC.SelectObject(&m_bmDivider);
		//			//	memDC.BitBlt(rcItem.left, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
		//			//	CString strTmp;
		//			//	strTmp.Format(_T("big half %d : left: %d normal draw\n"), i, rcItem.left);
		//			//	::OutputDebugString(strTmp);

		//			//	bitmapDC.SelectObject(&m_bmDividerDown);
		//			//	memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
		//			//	strTmp.Format(_T("big half %d : right: %d move draw\n"), i, rcItem.right);
		//			//	::OutputDebugString(strTmp);
		//			//}
		//			//else
		//			//{
		//			//	//bitmapDC.SelectObject(&m_bmDividerDown);
		//			//	//memDC.BitBlt(rcItem.left, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
		//			//	bitmapDC.SelectObject(&m_bmDivider);
		//			//	memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
		//			//}
		//		}
		//		//else
		//		//{
		//		//	bitmapDC.SelectObject(&m_bmDivider);
		//		//	memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
		//		//	//CString strTmp;
		//		//	//strTmp.Format(_T("small half %d : right: %d normal draw\n"), i, rcItem.right);
		//		//	//::OutputDebugString(strTmp);
		//		//}

		//	}
		//	else
		//	{
		//		//bitmapDC.SelectObject(&m_bmDivider);
		//		//memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);

		//		//CString strTmp;
		//		//strTmp.Format(_T("other %d : right: %d normal draw\n"), i, rcItem.right);
		//		//::OutputDebugString(strTmp);
		//	}
		//}

		if(m_bShowSort && m_bSelected)
		{
			if( !(m_bShowCheckbox && i <= 0) )
			{
				if(i == m_nSortCol)
				{
					CRect rect;
					GetItemRect(m_nSortCol, &rect);
					rect.left = rect.right - (m_nSortWidth + 2);
					//rect.top	= (rect.Height() - m_nSortHeight)/2;

					int nTop = (rect.Height() - m_nSortHeight) /2;

					if(m_bSortAsc)
					{
						//bitmapDC.SelectObject(&m_bmSortAsc);

						HBITMAP hBitmap = (HBITMAP)m_bmSortAsc;
						DrawTransparentBitmap(memDC.m_hDC, hBitmap, rect.left, nTop, m_clrSortMask);
					}
					else
					{
						//bitmapDC.SelectObject(&m_bmSortDesc);

						HBITMAP hBitmap = (HBITMAP)m_bmSortDesc;
						DrawTransparentBitmap(memDC.m_hDC, hBitmap, rect.left, nTop, m_clrSortMask);
					}
				}

				//memDC.BitBlt(rect.left, rect.top, m_nSortWidth, m_nSortHeight, &bitmapDC, 0,0,SRCCOPY);
			}
		}

		DRAWITEMSTRUCT	DrawItemStruct;
		GetItemRect(i, &rcItem);

		DrawItemStruct.CtlType		= ODT_HEADER;
		DrawItemStruct.hDC			= dc.GetSafeHdc();
		DrawItemStruct.itemAction	= ODA_DRAWENTIRE;
		DrawItemStruct.hwndItem 	= GetSafeHwnd();
		DrawItemStruct.rcItem		= rcItem;
		DrawItemStruct.itemID		= i;

		DrawItem(&DrawItemStruct);// 오버라이딩하지 않았음

		CFont font;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = m_nFontSize;

		lf.lfWeight = FW_NORMAL;
		if(m_bFontBold)
			lf.lfWeight = FW_SEMIBOLD;

		_tcscpy_s(lf.lfFaceName, m_strFont);

		font.CreateFontIndirect(&lf);

		CFont* def_font = memDC.SelectObject(&font);

		TCHAR buf[256];
		HD_ITEM hd_item;

		hd_item.mask = HDI_TEXT | HDI_HEIGHT | HDI_FORMAT | HDI_ORDER;
		hd_item.pszText = buf;
		hd_item.cchTextMax = 255;
		GetItem( DrawItemStruct.itemID, &hd_item );

		UINT uFormat;

		if(i == m_nCheckBoxIdx && m_bShowCheckbox)
		{
			if(rcItem.Width() > m_nCheckBoxWidth)
			{
				uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS;
			}
			else
			{
				uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_RIGHT | DT_END_ELLIPSIS;
			}
		}
		else
		{
			int nFmt = DT_CENTER;
			if(hd_item.fmt == HDF_LEFT)
				nFmt = DT_LEFT;
			else if(hd_item.fmt == HDF_RIGHT)
				nFmt = DT_RIGHT;

			uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS | nFmt;
		}

		if(i == m_nCheckBoxIdx)
		{
			if(m_bShowCheckbox)
			{
				rcItem.left = rcChkbox.right + 5;
				memDC.DrawText(buf,&rcItem, uFormat);
				memDC.SelectObject(def_font);
				font.DeleteObject();
			}
			else
			{
				memDC.DrawText(buf,&rcItem, uFormat);
				memDC.SelectObject(def_font);
				font.DeleteObject();
			}
		}
		else
		{
			if(i == m_nSortCol)
				rcItem.right -= m_nSortWidth + 2;
			memDC.DrawText(buf,&rcItem, uFormat);
			memDC.SelectObject(def_font);

			font.DeleteObject();
		}
	}

	if(nDividerIdx != -1)
	{
		CPoint pt;
		GetCursorPos(&pt);

		ScreenToClient(&pt);

		CRect rcItem;
		GetItemRect(nDividerIdx, rcItem);

		int nHalf = rcItem.left + (rcItem.Width() /2);

		//CString strTmp;
		//strTmp.Format(_T("i: %d, half : %d, mouse : %d\n"), nDividerIdx, nHalf, pt.x);
		//::OutputDebugString(strTmp);

		if(pt.x >= nHalf)//반보다 큰경우
		{
			bitmapDC.SelectObject(&m_bmDivider);
			memDC.BitBlt(rcItem.left, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
			//CString strTmp;
			//strTmp.Format(_T("big half %d : left: %d normal draw\n"), nDividerIdx, rcItem.left);
			//::OutputDebugString(strTmp);

			bitmapDC.SelectObject(&m_bmDividerDown);
			memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
			//strTmp.Format(_T("big half %d : right: %d move draw\n"), nDividerIdx, rcItem.right);
			//::OutputDebugString(strTmp);
		}
		else
		{
			bitmapDC.SelectObject(&m_bmDividerDown);
			memDC.BitBlt(rcItem.left, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
			bitmapDC.SelectObject(&m_bmDivider);
			memDC.BitBlt(rcItem.right, rcItem.top, m_nDividerWidth, m_nDividerHeight, &bitmapDC, 0, 0, SRCCOPY);
		}
	}
	//텍스트에 잔상생겨 임시 주석
	//bitmapDC.SelectObject(&m_bmLeft);
	//memDC.BitBlt(rcClient.left, rcClient.top, m_nLeftWidth, m_nCenterHeight, &bitmapDC, 0,0,SRCCOPY);
	//bitmapDC.SelectObject(&m_bmRight);
	//memDC.BitBlt(rcClient.right-m_nRightWidth, rcClient.top, m_nRightWidth, m_nCenterHeight, &bitmapDC, 0,0,SRCCOPY);

	bitmapDC.SelectObject(pOldbmp);
}

void CxSkinHeaderCtrl::PreSubclassWindow()
{
	int fnWeight = FW_NORMAL;

	if(m_bFontBold)
		fnWeight = FW_SEMIBOLD;

	if(m_Font.m_hObject)
		m_Font.DeleteObject();
	VERIFY(m_Font.CreateFont(
		m_nFontSize,
		0,
		0,
		0,
		fnWeight,
		FALSE,
		FALSE,
		0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,
		m_strFont));

	SetFont(&m_Font, TRUE);

	CHeaderCtrl::PreSubclassWindow();
}

void CxSkinHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	m_bMouseDown = TRUE;

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CxSkinHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	
	m_bMouseDown = FALSE;

	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

void CxSkinHeaderCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CHeaderCtrl::OnSetFocus(pOldWnd);

	
	m_bMouseOver = TRUE;
}

void CxSkinHeaderCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CHeaderCtrl::OnKillFocus(pNewWnd);

	
	m_bMouseOver = FALSE;
}

void CxSkinHeaderCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	
	if(m_ListWnd != NULL)
	{
		m_ListWnd->SendMessage(UM_HEADER_RBTN_DOWN, 0, (LPARAM)&point);
	}

	CHeaderCtrl::OnRButtonDown(nFlags, point);
}

void CxSkinHeaderCtrl::DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap,LONG xStart, LONG yStart, COLORREF cTransparentColor)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hBitmap);   // Select the bitmap

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Get width of bitmap
	ptSize.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device
	// to logical points

	// Create some DCs to hold temporary data.
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);

	// Create a bitmap for each DC. DCs are required for a number of
	// GDI functions.

	// Monochrome DC
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	// Monochrome DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	// Each DC must select a bitmap object to store pixel data.
	bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);

	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));

	// Save the bitmap sent here, because it will be overwritten.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Set the background color of the source DC to the color.
	// contained in the parts of the bitmap that should be transparent
	cColor = SetBkColor(hdcTemp, cTransparentColor);

	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Set the background color of the source DC back to the original
	// color.
	SetBkColor(hdcTemp, cColor);

	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);

	// Copy the background of the main DC to the destination.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart, SRCCOPY);

	// Mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	// Mask out the transparent colored pixels on the bitmap.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	// XOR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

	// Copy the destination to the screen.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);

	// Place the original bitmap back into the bitmap sent here.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	// Delete the memory bitmaps.
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	// Delete the memory DCs.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}
BOOL CxSkinHeaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	
	return FALSE;

	return CHeaderCtrl::OnEraseBkgnd(pDC);
}

void CxSkinHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	
}

LRESULT CxSkinHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);

	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam;

	RECT *prc = hdl.prc;
	WINDOWPOS *pwpos = hdl.pwpos;

	int nHeight = m_nHeight;//pwpos->cy * 2.30;높이
	
	pwpos->cy = nHeight;//m_nCenterHeight;//nHeight;
	prc->top = nHeight;//m_nCenterHeight;//nHeight;

	return lr;
}
