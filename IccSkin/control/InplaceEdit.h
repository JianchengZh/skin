#ifndef _INPLACE_EDIT_H_
#define _INPLACE_EDIT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Define.h"

#include "Text.h"

#define ED_DEFAULT_WIDTH			100
#define ED_DEFAULT_HEIGHT			100
#define IDC_INPLACE_EDIT			101

#define UM_HIDE_EDIT				WM_USER + 1401

class CInplaceEdit : public CEdit
{
public:
    CBrush   m_Brush;
	CBitmap	 m_Background;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInplaceEdit)
	//}}AFX_VIRTUAL
public:
	CText* m_pText;
	CFont m_Font;

public:
	// Construction/destruction
	//
	CInplaceEdit();
	virtual ~CInplaceEdit();
	DECLARE_DYNCREATE(CInplaceEdit)


	BOOL CreateEditor(CWnd* pParentWnd);

	// Main operations
	//
	void Deselect();
	void Hide();
	void UpdateEditor( CText* pText );
	void UpdateContents( CText* pText );
	void SetFocus();
	void SelectAll();
	void PrepareBackground( CRect pos );
	void UpdateFont( LONG nHeight );

	CText* GetTextObj() const {
		return m_pText;
	}

	void SetTextObj( CText* t ) {
		m_pText = t;
	}

	void SetFontColor(COLORREF fontRGB);
	void SetBGColor(COLORREF bgRGB);
	void SetFrameColor(COLORREF frRGB);

	void SetFontType(CString strFont,COLORREF corFont = COLOR_BLACK,int nFontSize = 12,BOOL bBold = FALSE);
	
	void SetCaption(CString val){};
	void SetID(CString strName, int nID);
	int GetId();

private:
	CString	m_strFont;
	int		m_nFontSize;
	COLORREF m_corFont;
	COLORREF m_corBG;
	COLORREF m_corFrame;
	CBrush m_brBackGnd;
	CString m_strName;
	int m_nID;

protected:
	//{{AFX_MSG(CInplaceEdit)
	afx_msg void OnKillfocus();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnUpdate();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

};

#endif 