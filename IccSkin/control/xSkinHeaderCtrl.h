#ifndef XSKINHEADERCTRL_INCLUDED
#define XSKINHEADERCTRL_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

// CxSkinHeaderCtrl

#define UM_HEADER_RBTN_DOWN		WM_USER + 1701

#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinHeaderCtrl : public CHeaderCtrl
#else
class CxSkinHeaderCtrl : public CHeaderCtrl
#endif
{
	DECLARE_DYNAMIC(CxSkinHeaderCtrl)

public:
	CxSkinHeaderCtrl();
	virtual ~CxSkinHeaderCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL	m_bDragging;			//dragging column?
	BOOL	m_bShowCheckbox;	//show checkbox?

	BOOL	m_bMouseDown;		//mouse down?
	BOOL	m_bMouseOver;		//mouse over?
	BOOL	m_bSelected;		//selected

	BOOL	m_bShowSort;		//show sort
	BOOL	m_bSortAsc;			//sort asc or desc

	BOOL	m_bAllChecked;		//all checked?

	int		m_nDragCol;			//drag column
	int		m_nSortCol;			//sort column

	COLORREF	m_clrText;		//text color
	COLORREF	m_clrBG;		//back color
	CFont		m_Font;			//font
	CString		m_strFont;
	int			m_nFontSize;
	BOOL		m_bFontBold;
	CWnd*		m_ListWnd;
	COLORREF	m_clrSortMask;

public:
	CBitmap m_bmChecker;		//checked box
	CBitmap m_bmUnChecker;		//unchecked box
	CBitmap m_bmLeft;			//left image
	CBitmap m_bmCenter;			//center image
	CBitmap m_bmCenterOver;
	CBitmap m_bmCenterDown;
	CBitmap m_bmCenterSelect;	//sorted.
	CBitmap m_bmDivider;		//divider image
	CBitmap m_bmDividerDown;	//divider down image...컬럼을 드래그할 경우, 표시
	CBitmap m_bmRight;			//right image
	CBitmap m_bmSortAsc;		//asc sort image
	CBitmap m_bmSortDesc;		//desc sort image

protected:
	int		m_nCheckBoxWidth;		//checkbox width
	int		m_nCheckBoxHeight;		//checkbox height
	int		m_nLeftWidth;
	int		m_nCenterWidth;		//center image width
	int		m_nCenterHeight;	//center image height
	int		m_nDividerWidth;	//divider width
	int		m_nDividerHeight;	//divider height
	int		m_nRightWidth;
	int		m_nSortWidth;		//sort width
	int		m_nSortHeight;		//sort height
	BOOL	m_bUseDrag;
	int		m_nCheckBoxIdx;
	int		m_nCheckBoxMargin;
	int		m_nHeight;

public:
	
	void FreeSkin(void);
	void SetListWnd(CWnd* wnd){m_ListWnd = wnd;};
	

	void SetFontName(CString strFont){m_strFont = strFont;};
	CString GetFontName(void){return m_strFont;};
	
	void SetFontSize(int nSize){m_nFontSize = nSize;};
	int GetFontSize(void){return m_nFontSize;};

	void SetFontBold(BOOL bBold){m_bFontBold = bBold;};
	BOOL GetFontBold(void){return m_bFontBold;};

	void SetDragStatus(BOOL bDrag){m_bDragging = bDrag;};
	BOOL GetDragStatus(void){return m_bDragging;};

	void SetDrag(BOOL bUseDrag){m_bUseDrag = bUseDrag;};
	void SetDragColumn(int nCol){m_nDragCol = nCol;};
	int GetDragColumn(void){return m_nDragCol;};

	void SetCheckbox(BOOL bShowCheckbox){m_bShowCheckbox = bShowCheckbox;};
	BOOL GetCheckbox(void){return m_bShowCheckbox;};
	int GetCheckWidth(void){return m_nCheckBoxWidth;};
	int GetCheckHeight(void){return m_nCheckBoxHeight;};
	void SetCheckIndex(int nIndex){m_nCheckBoxIdx = nIndex;};
	int GetCheckIndex(void){return m_nCheckBoxIdx;};
	void SetCheckboxMargin(int nMargin){m_nCheckBoxMargin = nMargin;};

	void SetSort(BOOL bShowSort){m_bShowSort = bShowSort;};
	BOOL GetSort(void){return m_bShowSort;};
	void SetSortImage(int nColumn, BOOL bAscend);

	void SetAllCheck(BOOL bAllCheck){m_bAllChecked = bAllCheck;};
	BOOL GetAllCheck(void){return m_bAllChecked;};

	void SetBGColor(COLORREF clrBk){m_clrBG = clrBk;};
	void SetTxtColor(COLORREF clrTxt){m_clrText = clrTxt;};

	void SetCheckWH(int nWidth, int nHeigth);
	void SetBkWH(int nLeftWidth, int nCenterWidth, int nRightWidth, int nDividerWidth, int nHeight);
	void SetSortWH(int nWidth, int nHeigth);

	void SetUseDrag(BOOL bDrag){m_bUseDrag = bDrag;};
	void SetHeight(int nHeight){m_nHeight = nHeight;};
	
	afx_msg void OnPaint();

protected:
	virtual void PreSubclassWindow();

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

private:
	void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, LONG xStart, LONG yStart, COLORREF cTransparentColor);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg LRESULT OnLayout(WPARAM wParam, LPARAM lParam);
};

#endif