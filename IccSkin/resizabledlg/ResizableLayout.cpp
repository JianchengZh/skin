// ResizableLayout.cpp: implementation of the CResizableLayout class.
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResizableLayout.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define BS_TYPEMASK SS_TYPEMASK

void CResizableLayout::AddAnchor(HWND hWnd, CSize sizeTypeTL, CSize sizeTypeBR)
{
	CWnd* pParent = GetResizableWnd();

	// child window must be valid
	ASSERT(::IsWindow(hWnd));
	// must be child of parent window
	ASSERT(::IsChild(pParent->GetSafeHwnd(), hWnd));
	// top-left anchor must be valid
	ASSERT(sizeTypeTL != NOANCHOR);

	// get control's window class
	CString st;
	GetClassName(hWnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();

	// add the style 'clipsiblings' to a GroupBox
	// to avoid unnecessary repainting of controls inside
	if (0 == st.CompareNoCase(_T("Button")))
	{
		DWORD style = GetWindowLong(hWnd, GWL_STYLE);
		if ((style & BS_TYPEMASK) == BS_GROUPBOX)
			SetWindowLong(hWnd, GWL_STYLE, style | WS_CLIPSIBLINGS);
	}

	// window classes that don't redraw client area correctly
	// when the hor scroll pos changes due to a resizing
	BOOL bHScroll = FALSE;
	if (0 == st.CompareNoCase(_T("ListBox"))) bHScroll = TRUE;

	// window classes that need refresh when resized
	BOOL bRefresh = NeedsRefresh(hWnd);

	// get parent window's rect
	CRect rectParent;
	GetTotalClientRect(&rectParent);
	// and child control's rect
	CRect rectChild;
	::GetWindowRect(hWnd, &rectChild);
	pParent->ScreenToClient(&rectChild);

	// go calculate margins
	CSize sizeMarginTL, sizeMarginBR;

	if (sizeTypeBR == NOANCHOR)
		sizeTypeBR = sizeTypeTL;

	// calculate margin for the top-left corner

	sizeMarginTL.cx = rectChild.left - rectParent.Width() * sizeTypeTL.cx / 100;
	sizeMarginTL.cy = rectChild.top - rectParent.Height() * sizeTypeTL.cy / 100;

	// calculate margin for the bottom-right corner

	sizeMarginBR.cx = rectChild.right - rectParent.Width() * sizeTypeBR.cx / 100;
	sizeMarginBR.cy = rectChild.bottom - rectParent.Height() * sizeTypeBR.cy / 100;

	// add to the list
	LayoutInfo layout(hWnd, sizeTypeTL, sizeMarginTL,
		sizeTypeBR, sizeMarginBR, bHScroll, bRefresh);
	// always add before callbacks
	m_arrLayout.InsertAt(m_iFirstCallback++, layout);
}

// one callback control cannot rely upon another callback control's
// size and/or position (they're updated all together at the end)
void CResizableLayout::AddAnchorCallback(UINT nCallbackID)
{
	// add to the list
	LayoutInfo layout;
	layout.nCallbackID = nCallbackID;
	// callbacks added to tail (for efficiency)
	m_arrLayout.Add(layout);
}

BOOL CResizableLayout::ArrangeLayoutCallback(LayoutInfo& /*layout*/)
{
	ASSERT(FALSE);
	// must be overridden, if callback is used

	return FALSE;	// no output data
}

void CResizableLayout::ArrangeLayout()
{
	CWnd* pParent = GetResizableWnd();

	// get parent window's rect
	CRect rectParent;
	GetTotalClientRect(&rectParent);

	// init some vars
	int i, count = m_arrLayout.GetSize();
	HDWP hdwp = BeginDeferWindowPos(count);

	for (i=0; i<count; ++i)
	{
		LayoutInfo layout = m_arrLayout[i];

		if (layout.hWnd == NULL)	// callback
		{
			if (i == m_iFirstCallback)	// first time only
			{
				// update previous controls
				EndDeferWindowPos(hdwp);
				// start again for callback controls
				hdwp = BeginDeferWindowPos(count-i);
			}
			// callbacks are added at the end, so that
			// you don't have multiple screen updates

			if (!ArrangeLayoutCallback(layout))	// request data
				continue;
		}

		CRect rectChild, newrc;
		CWnd* pWnd = CWnd::FromHandle(layout.hWnd); // temporary solution

		pWnd->GetWindowRect(&rectChild);
		pParent->ScreenToClient(&rectChild);

		// calculate new top-left corner

		newrc.left = layout.sizeMarginTL.cx + rectParent.Width() * layout.sizeTypeTL.cx / 100;
		newrc.top = layout.sizeMarginTL.cy + rectParent.Height() * layout.sizeTypeTL.cy / 100;

		// calculate new bottom-right corner

		newrc.right = layout.sizeMarginBR.cx + rectParent.Width() * layout.sizeTypeBR.cx / 100;
		newrc.bottom = layout.sizeMarginBR.cy + rectParent.Height() * layout.sizeTypeBR.cy / 100;

		if (!newrc.EqualRect(&rectChild))
		{
			if (layout.bAdjHScroll)
			{
				// needs repainting, due to horiz scrolling
				int diff = newrc.Width() - rectChild.Width();
				int max = pWnd->GetScrollLimit(SB_HORZ);

				layout.bNeedRefresh = FALSE;
				if (max > 0 && pWnd->GetScrollPos(SB_HORZ) > max - diff)
				{
					layout.bNeedRefresh = TRUE;
				}
			}

			// set flags
			DWORD flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION;
			if (layout.bNeedRefresh)
				flags |= SWP_NOCOPYBITS;
			if (newrc.TopLeft() == rectChild.TopLeft())
				flags |= SWP_NOMOVE;
			if (newrc.Size() == rectChild.Size())
				flags |= SWP_NOSIZE;

			hdwp = DeferWindowPos(hdwp, layout.hWnd, NULL, newrc.left, newrc.top,
				newrc.Width(), newrc.Height(), flags);
		}
	}
	// go re-arrange child windows
	EndDeferWindowPos(hdwp);
}

void CResizableLayout::EnumAndClipChildWindow(HWND hWnd, CDC* pDC)
{
	// obtain window position
	CRect rect;
	::GetWindowRect(hWnd, &rect);
	GetResizableWnd()->ScreenToClient(&rect);
	pDC->DPtoLP(&rect);
	//rect.right += 10;
	//rect.bottom += 10;

	// use window region if any
	CRgn rgn;
	rgn.CreateRectRgn(0,0,0,0);
	if (COMPLEXREGION == ::GetWindowRgn(hWnd, rgn))
	{
		rgn.OffsetRgn(rect.TopLeft());
	}
	else
	{
		rgn.SetRectRgn(&rect);
	}

	// go clipping?
	if (LikesClipping(hWnd))
		pDC->SelectClipRgn(&rgn, RGN_DIFF);
	else
		pDC->SelectClipRgn(&rgn, RGN_OR);
}

void CResizableLayout::ClipChildren(CDC *pDC)
{
	// only clips anchored controls
	for (int i=0; i<m_arrLayout.GetSize(); ++i)
	{
		HWND hWnd = m_arrLayout[i].hWnd;
		if (hWnd != NULL && ::IsWindowVisible(hWnd))
			EnumAndClipChildWindow(m_arrLayout[i].hWnd, pDC);
	}
}

void CResizableLayout::GetTotalClientRect(LPRECT lpRect)
{
	GetResizableWnd()->GetClientRect(lpRect);
}

BOOL CResizableLayout::NeedsRefresh(HWND hWnd)
{
	// get window class
	CString st;
	GetClassName(hWnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();

	// optimistic, no need to refresh
	BOOL bRefresh = FALSE;

	// window classes that need refresh when resized
	if (0 == st.CompareNoCase(_T("Static")))
	{
		DWORD style = GetWindowLong(hWnd, GWL_STYLE);

		switch (style & SS_TYPEMASK)
		{
		case SS_LEFT:
		case SS_CENTER:
		case SS_RIGHT:
		case SS_ENHMETAFILE:
		case SS_BITMAP:
			// word-wrapped text needs bRefresh
			bRefresh = TRUE;
			break;

		case SS_ICON:
		case SS_SIMPLE:
		case SS_ETCHEDHORZ:
		case SS_ETCHEDVERT:
			break;

		default:
			// centered images or text need bRefresh
			if (style & SS_CENTERIMAGE)
				bRefresh = TRUE;
		}
	}

	return bRefresh;
}

BOOL CResizableLayout::LikesClipping(HWND hWnd)
{
	// check child type
	CString st;
	GetClassName(hWnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();

	DWORD style = GetWindowLong(hWnd, GWL_STYLE);

	// skip windows that wants background repainted
	if (st == TOOLBARCLASSNAME && (style & TBSTYLE_TRANSPARENT))
		return FALSE;
	if (0 == st.CompareNoCase(_T("Button")) && (style & BS_TYPEMASK) == BS_GROUPBOX)
		return FALSE;
	if (0 == st.CompareNoCase(_T("Static")))
	{
		switch (style & SS_TYPEMASK)
		{
		case SS_BLACKRECT:
		case SS_GRAYRECT:
		case SS_WHITERECT:
		case SS_ETCHEDHORZ:
		case SS_ETCHEDVERT:
		case SS_BITMAP:
			break;
		case SS_ICON:
		case SS_ENHMETAFILE:
			if (style & SS_CENTERIMAGE)
				return FALSE;
			break;
		default:
			//return FALSE;//del by GUDXO
			return TRUE;
		}
	}

	// assume the others like clipping
	return TRUE;
}