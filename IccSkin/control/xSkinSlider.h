#ifndef XSKINBITMAPSLIDER_INCLUDED
#define XSKINBITMAPSLIDER_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif 
// xSkinSlider.h : header file
//


#define WM_SLIDER_MOVING	WM_USER + 1301
#define WM_SLIDER_MOVED		WM_USER + 1302



#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinSlider : public CStatic
#else
class CxSkinSlider : public CStatic
#endif
{
// Construction
public:
	CxSkinSlider();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinSlider)
	//}}AFX_VIRTUAL

// Implementation
public:		
	
	void SetCaption(CString val){SetWindowText(val);};
	int GetId(){return m_nId;};
	void SetId(int id){m_nId = id;};
	void CopyFromBG(CBitmap& bmSkinImage, CRect Rect_Left, CRect Rect_Right, CRect Rect_CenterLeft, CRect Rect_CenterRight, CRect Rect_ThumbMask);
	void CopyFromBG(CBitmap& bmSkinImage, CRect Rect_Left, CRect Rect_Right, CRect Rect_CenterLeft, CRect Rect_CenterRight, CRect Rect_ThumbMask, CRect Rect_LeftU, CRect Rect_RightU);
	void CopyFromBk(CBitmap& bmSkinImage, CRect Rect_Bk);
	BOOL CopyFromThumb(CBitmap& bmSkinImage, CRect Rect_Thumb, CRect Rect_Active, BOOL bTransparent=FALSE, COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );
	void SetBitmapThumbMask(UINT nThumbMask);
	void ChangeSkinImage(UINT nChannelID, UINT nActiveID, UINT nMASKID);
	void UpdatePos();
	BOOL m_bLButtonDown;
	void SetBGBitmap(UINT nIDLeft, UINT nIDCenterL, UINT nIDCenterR, UINT nIDRight);
	void GetRange( int &nMin, int &nMax ) { nMin = m_nMin; nMax = m_nMax; };
	int GetRangeMax() { return m_nMax; };
	int GetRangeMin() { return m_nMin; };
	int GetPos() { return m_nPos; };
	void SetRange( int nMin, int nMax, BOOL bRedraw=FALSE );
	void SetRangeMin(int nMin, BOOL bRedraw = FALSE);
	void SetRangeMax( int nMax, BOOL bRedraw = FALSE );
	void SetPos( int nPos );
	int SetPageSize( int nSize );
	BOOL SetBitmapChannel(UINT nChannelID, UINT nActiveID=NULL, BOOL bTransparent=FALSE,COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );
	BOOL SetBitmapThumb(UINT nThumbID, UINT nActiveID=NULL, BOOL bTransparent=FALSE,COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );
	
	void SetBkMargin(int nLeft, int nTop, int nRight, int nBottom);
	void SetBkMarginTop( int nMargin ) { m_nBkMarginTop = nMargin; };
	void SetBkMarginLeft( int nMargin ) { m_nBkMarginLeft = nMargin; };
	void SetBkMarginRight( int nMargin ) { m_nBkMarginRight = nMargin; };
	void SetBkMarginBottom( int nMargin ) { m_nBkMarginBottom = nMargin; };

	void SetMargin( int nLeft, int nTop, int nRight, int nBottom );
	void SetMarginTop( int nMargin ) { m_nMarginTop = nMargin; };
	void SetMarginLeft( int nMargin ) { m_nMarginLeft = nMargin; };
	void SetMarginRight( int nMargin ) { m_nMarginRight = nMargin; };
	void SetMarginBottom( int nMargin ) { m_nMarginBottom = nMargin; };
	void SetVertical( BOOL bVertical=TRUE ) { m_bVertical = bVertical; };
	void Enable( BOOL bEnable = TRUE );
	void DrawFocusRect( BOOL bDraw = TRUE, BOOL bRedraw = FALSE );
	virtual ~CxSkinSlider();

	// Generated message map functions
protected:
	void RestoreBackground(	CDC *pDC, int nXDst, int nYDst, int nWidth, int nHeight, CBitmap *pBmSrc);
	void CopyBackground(CDC *pDC, int nXSrc, int nYSrc, int nWidth, int nHeight, CBitmap *pBmDst );
	void DrawBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,	CDC* pTmpDC, int xSource, int ySource,	CBitmap *bmMask = NULL, BOOL bTransparent = FALSE );
	void DrawTransparentBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,CDC* pTmpDC, int xSource, int ySource, CBitmap *bmMask );
	void PrepareMask(CBitmap* pBmpSource, CBitmap* pBmpMask,COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );
	int Pixel2Pos( int nPixel );
	int Pos2Pixel( int nPos );

	int m_nMax, m_nMin, m_nPos, m_nPage;
	int		m_nId;
	CRect m_rect;
	int m_nWidth, m_nHeight;
	int m_nThumbWidth, m_nThumbHeight;
	int m_nMarginLeft, m_nMarginRight, m_nMarginTop, m_nMarginBottom;
	int m_nBkMarginLeft, m_nBkMarginRight, m_nBkMarginTop, m_nBkMarginBottom;
	int m_nThumbBgX, m_nThumbBgY;
	int m_nMouseOffset;

	BOOL m_bVertical;
	BOOL m_bChannelActive, m_bThumbActive;
	BOOL m_bTransparentChannel, m_bTransparentThumb, m_bThumb, m_bChannel;
	BOOL m_bFocus, m_bFocusRect, m_bDrawFocusRect;
	BOOL m_bEnable;

	CBitmap m_bmChannel, m_bmChannelMask, m_bmChannelActive, m_bmChannelActiveMask;
	CBitmap m_bmThumb, m_bmThumbMask, m_bmThumbActive, m_bmThumbActiveMask, m_bmThumbBg;
	//CBitmap m_bmThumbMask2;

	CBitmap	m_bmpBk;
	BITMAP	m_bmBk;
	CBitmap  m_bmpLeft, m_bmpCenterL, m_bmpCenterR,  m_bmpRight,  m_bmpRightU,  m_bmpLeftU;
	BITMAP	 m_bmLeft, m_bmCenterL, m_bmCenterR, m_bmRight, m_bmRightU, m_bmLeftU;
	BOOL     m_bSetBGBitmap;
	bool	m_bThumbLock;
	//{{AFX_MSG(CxSkinSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	void SetThumbLock(bool bLock);
	CRect GetThumbClipBox(void);
};

#endif 
