// ResizableState.h: interface for the CResizableState class.
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

#ifndef RESIZABLESTATE_H
#define RESIZABLESTATE_H

#if _MSC_VER > 1000
#pragma once
#endif 

#ifdef _AFXEXT
class AFX_EXT_CLASS CResizableState  
#else
class CResizableState  
#endif
{
protected:
	// non-zero if successful
	BOOL LoadWindowRect(LPCTSTR pszSection, BOOL bRectOnly);
	BOOL SaveWindowRect(LPCTSTR pszSection, BOOL bRectOnly);

	virtual CWnd* GetResizableWnd() = 0;

public:
	CResizableState();
	virtual ~CResizableState();
};

#endif 
