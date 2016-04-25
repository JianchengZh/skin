// RoundSliderCtrl.h : Header file
//
// Copyright (c) 1999 Daniel Frey
// See RoundSliderCtrl.cpp for details.

#if !defined(_ROUNDSLIDERCTRL_H_)
#define _ROUNDSLIDERCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_RSLIDER_MOVING	WM_USER + 1303
#define WM_RSLIDER_MOVED	WM_USER + 1304

class CRoundSliderCtrl : public CSliderCtrl
{
protected:
	CPoint		m_ptCenter;
	CPoint		m_ptKnobCenter;
	int			m_nRadius;
	CRgn		m_rgn;

	int			m_nZero;
	bool		m_bInverted;

	CString		m_strText;
	int			m_nKnobRadius;

	bool		m_bDragging;
	bool		m_bDragByKnobOnly;
	bool		m_bDragChanged;
	bool		m_bDrawRadioButton;

	LOGFONT		m_lf;
	CFont		m_font;
	COLORREF	m_crText;
	HBRUSH		m_hKnobBrush;
	HBRUSH		m_hDialBrush;

public:
	DECLARE_DYNAMIC(CRoundSliderCtrl)

	CRoundSliderCtrl();
	virtual ~CRoundSliderCtrl();

	// You may use '%ld' to show the slider value. Default: "%ld?
	virtual void SetText(const CString& strNewText);
	virtual CString GetText() const;

	// Default: 7, Minimum: 5
	virtual void SetKnobRadius(const int nNewKnobRadius);
	virtual int GetKnobRadius() const;

	virtual void SetDragByKnobOnly(const bool bDragByKnobOnly);
	virtual bool GetDragByKnobOnly() const;

	// 0-359, 0 = Top, 90 = Right, 180 = Bottom, 270 = Left
	virtual void SetZero(const int nZero);
	virtual int GetZero() const;

	// CW/CCW
	virtual void SetInverted(const bool bNewInverted = true);
	virtual bool GetInverted() const;

	virtual void SetRadioButtonStyle(const bool bNewRadioButtonStyle = true);
	virtual bool GetRadioButtonStyle() const;

	virtual void SetDialColor(const COLORREF crBkgnd);
	virtual void SetKnobColor(const COLORREF crBkgnd);
	virtual void SetTextColor(const COLORREF crText);

	virtual void SetFontBold(const bool bSet = true);
	virtual void SetFontUnderline(const bool bSet = true);
	virtual void SetFontItalic(const bool bSet = true);
	virtual void SetFontSize(const int nSize);
	virtual void SetFontName(const CString& strFont);

	void SetCaption(CString val){SetWindowText(val);};
	int GetId(){return m_nId;};
	void SetId(int id){m_nId = id;};
	void CopyFromBk(CBitmap& bmSkinImage, CRect Rect_Bk);
	BOOL CopyFromThumb(CBitmap& bmSkinImage, CRect Rect_Thumb, CRect Rect_ThumbActive, CRect Rect_ThumbMask, 
		BOOL bTransparent=TRUE, COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );
	void SetThumbLock(bool bLock){m_bLock = bLock;};
	void SetCenter(int x, int y)
	{
		m_nCenterX = x;
		m_nCenterY = y;
	}
	void SetDiameter(int val){m_nDiameter = val;};
	void SetShowText(bool val){m_bShowText = val;};

	virtual void Init();
protected:
	int m_nId;
	bool m_bShowText;
	int m_nCenterX, m_nCenterY;
	int m_nDiameter;
	CBitmap	m_bmpBk, m_bmpThumb, m_bmpThumbMask, m_bmpThumbActive, m_bmpThumbBk;
	BITMAP	m_bmBk, m_bmThumb, m_bmThumbMask, m_bmThumbActive;
	bool m_bLock;
	bool m_bThumb;
	int m_nThumbWidth, m_nThumbHeight;
	void DrawBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,	CDC* pTmpDC, int xSource, int ySource,	CBitmap *bmMask = NULL, BOOL bTransparent = FALSE );
	void DrawTransparentBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,CDC* pTmpDC, int xSource, int ySource, CBitmap *bmMask );
	void PrepareMask(CBitmap* pBmpSource, CBitmap* pBmpMask,COLORREF clrpTransColor=0xFF000000, 
		int iTransPixelX=0, int iTransPixelY=0 );
	

	// Override this function for customized output
	virtual CString OnFormatText() const;

	//{{AFX_VIRTUAL(CRoundSliderCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	virtual void ReconstructFont();

	
	virtual bool SetKnob(const CPoint& pt);
	virtual void PostMessageToParent(const int nTBCode) const;

	//{{AFX_MSG(CRoundSliderCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif // _DEBUG
};

//{{AFX_INSERT_LOCATION}}

#endif // _ROUNDSLIDERCTRL_H_
