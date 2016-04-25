// ResizableMinMax.h: interface for the CResizableMinMax class.
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

#ifndef RESIZABLEMINMAX_H
#define RESIZABLEMINMAX_H

#if _MSC_VER > 1000
#pragma once
#endif 

#ifdef _AFXEXT
class AFX_EXT_CLASS CResizableMinMax  
#else
class CResizableMinMax  
#endif
{
// Attributes
private:
	// flags
	BOOL m_bUseMaxTrack;
	BOOL m_bUseMinTrack;
	BOOL m_bUseMaxRect;

	POINT m_ptMinTrackSize;		// min tracking size
	POINT m_ptMaxTrackSize;		// max tracking size
	POINT m_ptMaxPos;			// maximized position
	POINT m_ptMaxSize;			// maximized size

public:
	CResizableMinMax();
	virtual ~CResizableMinMax();

protected:
	void MinMaxInfo(LPMINMAXINFO lpMMI);

	void SetMaximizedRect(const CRect& rc);		// set window rect when maximized
	void ResetMaximizedRect();					// reset to default maximized rect
	void SetMinTrackSize(const CSize& size);	// set minimum tracking size
	void ResetMinTrackSize();					// reset to default minimum tracking size
	void SetMaxTrackSize(const CSize& size);	// set maximum tracking size
	void ResetMaxTrackSize();					// reset to default maximum tracking size
};

#endif 
