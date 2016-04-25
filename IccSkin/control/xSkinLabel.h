#ifndef SKINLABEL_INCLUDED
#define SKINLABEL_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif 
// Label.h : header file
//

#define TOOLTIP_ID		1

#define	NM_LINKCLICK	WM_APP + 0x200

/////////////////////////////////////////////////////////////////////////////
// CxSkinLabel window

#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinLabel : public CStatic
#else
class CxSkinLabel : public CStatic
#endif
{
// Construction
public:


	static enum LinkStyle { LinkNone, HyperLink, MailLink };
	static enum FlashType {None, Text, Background };
	static enum Type3D { Raised, Sunken};
	static enum BackFillMode { Normal, Gradient };

	CxSkinLabel();
	virtual CxSkinLabel& SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh = 0, BackFillMode mode = Normal);
	virtual CxSkinLabel& SetTextColor(COLORREF clrText, COLORREF clrTextFocus = 0, COLORREF clrTextUnlink=0);
	virtual CxSkinLabel& SetHighlight(bool highlight);
	virtual CxSkinLabel& SetText(const CString& strText);
	virtual CxSkinLabel& SetFontBold(BOOL bBold);
	virtual CxSkinLabel& SetFontName(const CString& strFont, BYTE byCharSet = ANSI_CHARSET);
	virtual CxSkinLabel& SetFontUnderline(BOOL bSet);
	virtual CxSkinLabel& SetFontItalic(BOOL bSet);
	virtual CxSkinLabel& SetFontSize(int nSize);
	virtual CxSkinLabel& SetSunken(BOOL bSet);
	virtual CxSkinLabel& SetBorder(BOOL bSet);
	virtual CxSkinLabel& SetTransparent(BOOL bSet);
	virtual CxSkinLabel& FlashText(BOOL bActivate);
	virtual CxSkinLabel& FlashBackground(BOOL bActivate);
	virtual CxSkinLabel& SetLink(BOOL bLink,BOOL bNotifyParent);
	virtual CxSkinLabel& SetLinkCursor(HCURSOR hCursor);
	virtual CxSkinLabel& SetFont3D(BOOL bSet,Type3D type=Raised);
	virtual CxSkinLabel& SetRotationAngle(UINT nAngle,BOOL bRotation);
	virtual CxSkinLabel& SetText3DHiliteColor(COLORREF cr3DHiliteColor);
	virtual CxSkinLabel& SetFont(LOGFONT lf);
	virtual CxSkinLabel& SetMailLink(BOOL bEnable, BOOL bNotifyParent);
	virtual CxSkinLabel& SetHyperLink(const CString& sLink);
	virtual LOGFONT	GetLogFont(void){return m_lf;};
	virtual CString	GetText(void);

	void SetVertCenter(bool val){m_bVertCenter = val;}
	void SetEllipse(bool val){m_bEllipsis = val;}
	
	virtual void SetCaption(CString val){SetWindowText(val);};
	virtual void SetId(int id){m_nId = id;}
	virtual int GetId(){return m_nId;}
	virtual void SetLineSpace(int val){m_nLineSpace = val;};
	virtual void SetWordWrap(bool wordwrap){m_bWordWrap = wordwrap;}

	void SetToolTipText(CString s);
	
private:
	CString m_strText;
	int m_nId;

protected:
	void UpdateSurface();
	void ReconstructFont();
	void DrawGradientFill(CDC* pDC, CRect* pRect, COLORREF crStart, COLORREF crEnd, int nSegments);
	COLORREF		m_crText;
	COLORREF		m_clrTextFocus;
	COLORREF		m_clrTextUnlink;
	COLORREF		m_cr3DHiliteColor;
	HBRUSH			m_hwndBrush;
	HBRUSH			m_hBackBrush;
	LOGFONT			m_lf;
	CFont			m_font;
	BOOL			m_bState;
	BOOL			m_bTimer;
	LinkStyle		m_Link;
	BOOL			m_bTransparent;
	BOOL			m_bFont3d;
	BOOL			m_bToolTips;
	BOOL			m_bNotifyParent;
	BOOL			m_bRotation;
	FlashType		m_Type;
	HCURSOR			m_hCursor;
	Type3D			m_3dType;
	BackFillMode	m_fillmode;
	COLORREF		m_crHiColor;
	COLORREF		m_crLoColor;
	CString			m_sLink;
	int				m_nLineSpace;
	BOOL			m_bVertCenter;
	BOOL			m_bEllipsis;
	bool			m_bHighLight;

	CToolTipCtrl		m_tooltip;
	bool			m_bWordWrap;
	
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinLabel)
	protected:
	virtual void PreSubclassWindow();
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CxSkinLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CxSkinLabel)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	//}}AFX_MSG
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif 
