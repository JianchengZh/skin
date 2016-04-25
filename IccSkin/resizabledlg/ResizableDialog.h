#ifndef RESIZABLEDIALOG_H
#define RESIZABLEDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ResizableLayout.h"
#include "ResizableGrip.h"
#include "ResizableMinMax.h"
#include "ResizableState.h"
#include "BaseSkin.h"
#include "GDXContext.h"
#include "WndShadow.h"

enum RESIZE_TYPE{
	RESIZENO = 0,
	RESIZENWSELT,
	RESIZENWSERB,
	RESIZENESWRT,
	RESIZENESWLB,
	RESIZEWEL,
	RESIZEWER,
	RESIZENST,
	RESIZENSB,
};
/////////////////////////////////////////////////////////////////////////////
// CResizableDialog window

#ifdef _AFXEXT
class AFX_EXT_CLASS CResizableDialog : public CDialog, public CResizableLayout
				, public CBaseSkin, public CResizableGrip
				, public CResizableMinMax, public CResizableState
#else
class CResizableDialog : public CDialog, public CResizableLayout
	, public CBaseSkin, public CResizableGrip
	, public CResizableMinMax, public CResizableState
#endif
{

// Construction
public:

	CResizableDialog();
	CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);

	BOOL	m_bResizing;
	BOOL	m_bUseResizeTracker;
	BOOL	m_bUseMoveTracker;
	CPoint	m_ptResizeFrom;
	unsigned short	m_usCursor;
	bool	m_bIsMaxmize;
	bool	m_bTracking;
	CSize	m_nSize;
	CGDXContext*	m_pGDXContext;
// Attributes
private:
	// flags
	BOOL m_bEnableSaveRestore;
	BOOL m_bRectOnly;

	// internal status
	LPCTSTR m_sSection;			// section name (identifies a parent window)
	BOOL m_bInitDone;			// if all internal vars initialized
	BOOL	m_bDraggable;

protected:
	CWndShadow m_shadow;
	
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizableDialog)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResizableDialog();

// used internally
private:
	void Construct();

// callable from derived classes
protected:
	// section to use in app's profile
	void EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly = FALSE);
	void UpdateShadow();

	/*virtual*/ CWnd* GetResizableWnd()
	{
		// make the layout know its parent window
		return this;
	};

// Generated message map functions
protected:
	//{{AFX_MSG(CResizableDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void Draggable(BOOL drag = TRUE){m_bDraggable = drag;};
private:
	bool SetStyle();

public:
	virtual BOOL OnChooseCursor(CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	bool SetSkin(CString strSkinFile);
	bool SetSkin(CString strSkinFile,CBitmap *bmBaseImage);
	void SetTracker_Resize(bool bUseTracker);
	void SetTracker_Move(bool bUseTracker);
	bool ReLoadSkins();
	void SetContextSize(int cx, int cy);
	void MoveCenter();
	//void SetShadow(HINSTANCE hInstance);
	
	void GetImageFromSkin(CBitmap &bmImage, CRect rect);
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif 
