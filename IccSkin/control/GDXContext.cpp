// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "GDXContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGDXContext

CGDXContext::CGDXContext(CWnd* pWin)
	:m_pDC(NULL)
{
	ASSERT(pWin != NULL);
	m_pWinObj = pWin;
	m_DefSize.SetSize(0,0);
}

CGDXContext::~CGDXContext()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGDXContext Drag Operations

void CGDXContext::_AdjustRectangle(CRect& rect, CPoint pt)
{
	int nXOffset = (pt.x < rect.left) ? (pt.x - rect.left) :
		(pt.x > rect.right) ? (pt.x - rect.right) : 0;
	int nYOffset = (pt.y < rect.top) ? (pt.y - rect.top) :
		(pt.y > rect.bottom) ? (pt.y - rect.bottom) : 0;
	rect.OffsetRect(nXOffset, nYOffset);
}

void CGDXContext::StartDrag(CPoint pt)
{
	ASSERT_VALID(m_pWinObj);
	m_bDragging = TRUE;

	InitLoop();

	if (m_pDC->GetLayout() & 0x00000001)
		m_pDC->SetLayout(0x00000000);

	CRect rect;
	m_pWinObj->GetWindowRect(rect);
	m_ptLast = pt;

	m_rectTrack = rect;
	_AdjustRectangle(m_rectTrack, pt);// adjust rectangles so that point is inside

	Move(pt);   // call it here to handle special keys
	Track();
}

void CGDXContext::Move(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;

	m_rectTrack.OffsetRect(ptOffset);	// offset all drag rects to new position
	m_ptLast = pt;

	DrawFocusRect();// update feedback
}

void CGDXContext::EndDrag()
{
	CancelLoop();
}

void CGDXContext::StartResize(int nHitTest, CPoint pt)
{
	ASSERT_VALID(m_pWinObj);
	m_bDragging = FALSE;

	InitLoop();

	if (m_pDC->GetLayout() & 0x00000001)
		m_pDC->SetLayout(0x00000000);

	// get true bar size (including borders)
	CRect rect;
	m_pWinObj->GetWindowRect(rect);
	m_ptLast = pt;
	m_nHitTest = nHitTest;

	m_rectTrack = rect;
	m_rectTrack.InflateRect(1, 1);

	Stretch(pt);   // call it here to handle special keys
	Track();
}

void CGDXContext::Stretch(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;

	// offset all drag rects to new position
	if (m_nHitTest == HTLEFT || m_nHitTest == HTRIGHT){
		if (m_nHitTest == HTLEFT){
			m_rectTrack.left += ptOffset.x;
			if(pt.x >= (m_rectTrack.right-m_DefSize.cx)){
				m_rectTrack.left = m_rectTrack.right-m_DefSize.cx;
			}
		}else{
			m_rectTrack.right += ptOffset.x;
			if(pt.x <= (m_rectTrack.left+m_DefSize.cx)){
				m_rectTrack.right = m_rectTrack.left + m_DefSize.cx;
			}
		}
	}else if(m_nHitTest == HTTOP || m_nHitTest == HTBOTTOM){
		if (m_nHitTest == HTTOP){
			m_rectTrack.top += ptOffset.y;
			if(pt.y >= (m_rectTrack.bottom-m_DefSize.cy)){
				m_rectTrack.top = m_rectTrack.bottom-m_DefSize.cy;
			}
		}else{
			m_rectTrack.bottom += ptOffset.y;
			if(pt.y <= (m_rectTrack.top+m_DefSize.cy)){
				m_rectTrack.bottom = m_rectTrack.top + m_DefSize.cy;
			}
		}
	}else if(m_nHitTest == HTTOPLEFT){
		m_rectTrack.left += ptOffset.x;
		m_rectTrack.top += ptOffset.y;
		if(pt.x >= (m_rectTrack.right-m_DefSize.cx)){
			m_rectTrack.left = m_rectTrack.right-m_DefSize.cx;
		}
		if(pt.y >= (m_rectTrack.bottom-m_DefSize.cy)){
			m_rectTrack.top = m_rectTrack.bottom-m_DefSize.cy;
		}
	}else if(m_nHitTest == HTTOPRIGHT){
		m_rectTrack.right += ptOffset.x;
		m_rectTrack.top += ptOffset.y;
		if(pt.x <= (m_rectTrack.left+m_DefSize.cx)){
			m_rectTrack.right = m_rectTrack.left + m_DefSize.cx;
		}
		if(pt.y >= (m_rectTrack.bottom-m_DefSize.cy)){
			m_rectTrack.top = m_rectTrack.bottom-m_DefSize.cy;
		}
	}else if(m_nHitTest == HTBOTTOMLEFT){
		m_rectTrack.left += ptOffset.x;
		m_rectTrack.bottom += ptOffset.y;
		if(pt.x >= (m_rectTrack.right-m_DefSize.cx)){
			m_rectTrack.left = m_rectTrack.right-m_DefSize.cx;
		}
		if(pt.y <= (m_rectTrack.top+m_DefSize.cy)){
			m_rectTrack.bottom = m_rectTrack.top + m_DefSize.cy;
		}
	}else if(m_nHitTest == HTBOTTOMRIGHT){
		m_rectTrack.right += ptOffset.x;
		m_rectTrack.bottom += ptOffset.y;
		if(pt.x <= (m_rectTrack.left+m_DefSize.cx)){
			m_rectTrack.right = m_rectTrack.left + m_DefSize.cx;
		}
		if(pt.y <= (m_rectTrack.top+m_DefSize.cy)){
			m_rectTrack.bottom = m_rectTrack.top + m_DefSize.cy;
		}
	}

	m_ptLast = pt;

	DrawFocusRect(); // update feedback
}

void CGDXContext::EndResize()
{
	CancelLoop();
}

/////////////////////////////////////////////////////////////////////////////
// CGDXContext Operations

void CGDXContext::InitLoop()
{
	// handle pending WM_PAINT messages
	MSG msg;
	while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
	{
		if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
			return;
		DispatchMessage(&msg);
	}

	// initialize state
	m_rectLast.SetRectEmpty();
	m_sizeLast.cx = m_sizeLast.cy = 0;
	m_bDitherLast = FALSE;

	// lock window update while dragging
	ASSERT(m_pDC == NULL);
	CWnd* pWnd = CWnd::GetDesktopWindow();
	if (pWnd->LockWindowUpdate())
		m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW|DCX_CACHE|DCX_LOCKWINDOWUPDATE);
	else
		m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW|DCX_CACHE);
	ASSERT(m_pDC != NULL);
}

void CGDXContext::CancelLoop()
{
	DrawFocusRect(TRUE);    // gets rid of focus rect
	ReleaseCapture();

	CWnd* pWnd = CWnd::GetDesktopWindow();
	pWnd->UnlockWindowUpdate();
	if (m_pDC != NULL)
	{
		pWnd->ReleaseDC(m_pDC);
		m_pDC = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

void CGDXContext::DrawFocusRect(BOOL bRemoveRect)
{
	ASSERT(m_pDC != NULL);
	if(m_pDC == NULL) return;
	//#define CX_BORDER   1
	//#define CY_BORDER   1
	CSize size(1, 1);// default to thin frame

	// determine new rect and size
	CRect rect;
	CBrush* pWhiteBrush = CBrush::FromHandle((HBRUSH)::GetStockObject(WHITE_BRUSH));
	CBrush* pDitherBrush = CDC::GetHalftoneBrush();
	CBrush* pBrush = pWhiteBrush;

	// use thick frame instead
	size.cx = GetSystemMetrics(SM_CXFRAME) - 1;
	size.cy = GetSystemMetrics(SM_CYFRAME) - 1;

	rect = m_rectTrack;
	pBrush = pDitherBrush;

	if (bRemoveRect)
		size.cx = size.cy = 0;

	rect.InflateRect(-1, -1);

	// draw it and remember last size
	m_pDC->DrawDragRect(&rect, size, &m_rectLast, m_sizeLast,
		pBrush, m_bDitherLast ? pDitherBrush : pWhiteBrush);
	m_rectLast = rect;
	m_sizeLast = size;
	m_bDitherLast = (pBrush == pDitherBrush);
}

BOOL CGDXContext::Track()
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// set capture to the window which received this message
	m_pWinObj->SetCapture();
	ASSERT(m_pWinObj == CWnd::GetCapture());

	// get messages until capture lost or cancelled/accepted
	while (CWnd::GetCapture() == m_pWinObj)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage(msg.wParam);
			break;
		}

		switch (msg.message)
		{
		case WM_LBUTTONUP:
			if (m_bDragging)
				EndDrag();
			else
				EndResize();

			m_pWinObj->SendMessage(WM_LBUTTONUP, msg.wParam, msg.lParam);
			return TRUE;
		case WM_MOUSEMOVE:
			if (m_bDragging)
				Move(msg.pt);
			else
				Stretch(msg.pt);
			break;

		case WM_KEYDOWN:
			if (msg.wParam == VK_ESCAPE){
				CancelLoop();
				return FALSE;
			}
			break;
		case WM_RBUTTONDOWN:
			CancelLoop();
			return FALSE;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	CancelLoop();

	return FALSE;
}

CRect CGDXContext::GetDragRect(void)
{
	return m_rectTrack;
}

void CGDXContext::SetDefSize(int nCx, int nCy)
{
	m_DefSize.cx = nCx;
	m_DefSize.cy = nCy;
}
/////////////////////////////////////////////////////////////////////////////