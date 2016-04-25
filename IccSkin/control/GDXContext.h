#ifndef GDXCONTEXT_INCLUDE
#define GDXCONTEXT_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif


class CGDXContext
{
public:
// Construction
	CGDXContext(CWnd* pWin);
	virtual ~CGDXContext();

// Attributes
	CPoint m_ptLast;            // last mouse position during drag
	CRect m_rectLast;
	CSize m_sizeLast;
	BOOL m_bDitherLast;

	// Rectangles used during dragging or resizing
	CRect m_rectTrack;

	CWnd* m_pWinObj;        // the toolbar that created this context

	CDC* m_pDC;                 // where to draw during drag
	BOOL m_bDragging;
	int m_nHitTest;
	CSize m_DefSize;


// Drag Operations
	virtual void StartDrag(CPoint pt);
	void Move(CPoint pt);       // called when mouse has moved
	void EndDrag();             // drop
	
// Resize Operations
	virtual void StartResize(int nHitTest, CPoint pt);
	void Stretch(CPoint pt);
	void EndResize();

// Operations
	void InitLoop();
	void CancelLoop();
	void _AdjustRectangle(CRect& rect, CPoint pt);
	BOOL Track();
	void DrawFocusRect(BOOL bRemoveRect = FALSE); // draws the correct outline
	CRect GetDragRect(void);
	void SetDefSize(int nCx, int nCy);
};

#endif
