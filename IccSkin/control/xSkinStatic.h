#ifndef XSKINSTATIC_H
#define XSKINSTATIC_H

#if _MSC_VER > 1000
#pragma once
#endif
// xSkinStatic.h : header file
//

#include "Define.h"
#include "ImageMap_Static.h"
/////////////////////////////////////////////////////////////////////////////
// CxSkinStatic window


#define		NOSIZE		0
#define		RESIZEV		1
#define		RESIZEH		2
#define		RESIZEF		3

#define ID_TXTSHIFT_TIMER 100

typedef BOOL ( CALLBACK* LPFN_DrawStatic)(CDC* pDC, void* pParam);

#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinStatic : public CStatic
#else
class CxSkinStatic : public CStatic
#endif
{
// Construction
public:
	CxSkinStatic();

// Attributes
public:
	LPFN_DrawStatic    _lpCallback;
// Operations
public:
	
	void SetShiftTime(int nMSec);
	void SetCaption(CString val){SetWindowText(val);};
	int GetId(){return m_nId;};
	void SetId(int id){m_nId = id;};
	void SetText(CString strTxt);	
	CString GetText();
	void Pause(int nPause);
	void SetAutoShift(bool bShift);
	void SetDrawPattern(BOOL bDraw){m_bDrawPattern = bDraw;};
	void SetLinkCursor(HCURSOR hCursor){m_hCursor = hCursor;};
private:
	CString				m_strSentence;
	HCURSOR		m_hCursor;
protected:
	
	CBitmap				m_BitBG;
	CRect				m_Rect;
	UINT				m_nOption;
	CFont				m_Font;
	short				m_sResizeType;
	int m_nMSec;
	int		m_nId;
	CImageMap_Static	m_Map_Static;
	COLORREF			m_crTextColor;
	int					m_nXPos;
	BOOL				m_bAutoShift;
	bool				m_bRev;
	bool				m_bText;
	BOOL				m_bDrawPattern;
	

// Implementation
public:
	int GetValidTxt(CDC *pDC, int nPixelWidth, CString strString);
	void CopyFromBG(CBitmap& bmSkinImage, CRect RTopLeft, CRect RTopCenter, CRect RTopRight, CRect RMiddleLeft, 
								CRect RMiddleCenter, CRect RMiddleRight, CRect RBottomLeft, CRect RBottomCenter, CRect RBottomRight);
	void SetTextColor(COLORREF crTextColor);
	void SetResizeType(short type){m_sResizeType = type;};

	// Generated message map functions
protected:
	//{{AFX_MSG(CxSkinStatic)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
public:
	BOOL AttachCallback(LPFN_DrawStatic _CallFunc);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();	
	void SetFontType(CString strFont, COLORREF corFont = COLOR_BLACK,int nFontSize = FONT_SIZE, BOOL bBold = FALSE, BOOL bUnderLine = FALSE);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual ~CxSkinStatic();
	
};

#endif 
