// ResizableGrip.h: interface for the CResizableGrip class.
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

#ifndef RESIZABLEGRIP_H
#define RESIZABLEGRIP_H

#if _MSC_VER > 1000
#pragma once
#endif 

#define WS_EX_LAYOUT_RTL	0x00400000

#ifdef _AFXEXT
class AFX_EXT_CLASS CResizableGrip  
#else
class CResizableGrip  
#endif
{
private:
	SIZE m_sizeGrip;		// holds grip size

	CScrollBar m_wndGrip;	// grip control

private:
	static BOOL IsRTL(HWND hwnd)
	{
		DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		return (dwExStyle & WS_EX_LAYOUT_RTL);
	}

	static LRESULT CALLBACK GripWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	BOOL InitGrip();
	void UpdateGripPos();
	void ShowSizeGrip(BOOL bShow = TRUE);	// show or hide the size grip

	virtual CWnd* GetResizableWnd() = 0;

public:
	CResizableGrip();
	virtual ~CResizableGrip();

};

#endif 
