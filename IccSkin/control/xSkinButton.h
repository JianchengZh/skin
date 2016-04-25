#ifndef XSKINBUTTON_INCLUDED
#define XSKINBUTTON_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif 


#define WM_CXSHADE_RADIO		WM_USER + 1101
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton window
#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinButton : public CButton
#else
class CxSkinButton : public CButton
#endif
{
	// Construction
public:
	CxSkinButton();

	// Attributes
private:

	// Operations
public:
	virtual ~CxSkinButton();
	//{{AFX_MSG(CxSkinButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnClicked();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	// Implementation
public:
	void Init(void);
	bool GetTracking(void);	
	void SetCaption(CString val){};
	void SetId(int id){m_nId = id;};	
	int	GetId(){return m_nId;};
	void ChangeNormalImg(UINT chimg_NOR, UINT chimg_CLK, UINT chimg_OVR);
	void ChangeSkin(UINT normal,UINT down,UINT over,UINT disabled, UINT focus);
	void SetToolTipText(CString s);
	void SetToolTipText(CString s, CString sDown);
	void SetFontName(CString strFontName){m_strFontName = strFontName;};
	void SetFontSize(int nFontSize){m_nFontSize = nFontSize;};
	void SetFontBold(BOOL bFontBold){m_bFontBold = bFontBold;};
	void SetTextColor(COLORREF clrText, COLORREF clrFocusText);	
	void SetUseHandCursor(BOOL bUse){m_bUseHandCursor = bUse;};
	void CopyFrom(CBitmap& bmSkinImage, CRect NormalRect, CRect OverRect, CRect DownRect, CRect DisabledRect, CRect MaskRect, 
		short drawmode, short border, short margin);
	void CopyFrom(CRect rect, 
		CBitmap& bmNormal, CBitmap& bmOver, CBitmap& bmClick, CBitmap& bmDisable, CBitmap& bmMask, 
		short drawmode, short border, short margin);
	void SetSkin(UINT normal,UINT down, UINT over=0, UINT disabled=0, UINT focus=0,UINT mask=0,
		short drawmode=1,short border=1,short margin=4);
	
	// Generated message map functions
protected:
	CFont	m_Font;
	CString m_strIDName;
	int		m_nId;
	bool	m_Checked;					//radio & check buttons
	DWORD	m_Style;					//radio & check buttons
	bool m_tracking;
	bool m_button_down;
	CToolTipCtrl m_tooltip;	
	CString m_ToolTipUp,m_ToolTipDw;
	CString m_strFontName;
	int m_nFontSize;
	BOOL m_bFontBold;	
	BOOL	m_bUseHandCursor;//핸드커서
	CBitmap m_bNormal,m_bDown,m_bDisabled,m_bMask,m_bOver,m_bFocus; //skin bitmaps
	short	m_FocusRectMargin;		//dotted margin offset
	COLORREF m_clrText, m_clrFocusText;			//button text color
	HRGN	hClipRgn;				//clipping region
	BOOL	m_Border;				//0=flat; 1=3D;
	short	m_DrawMode;				//0=normal; 1=stretch; 2=tiled;

	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);	
	HRGN	CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);
	void	FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r);
	void	DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode);
	int		GetBitmapWidth (HBITMAP hBitmap);
	int		GetBitmapHeight (HBITMAP hBitmap);

	
};

#endif 
