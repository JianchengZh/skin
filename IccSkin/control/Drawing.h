#ifndef DRAWING_INCLUDE
#define DRAWING_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif

// Colors definition
//

#define	D_COLOR_RED			RGB( 0xFF, 0x00, 0x00 )
#define	D_COLOR_GREEN		RGB( 0x00, 0xFF, 0x00 )
#define	D_COLOR_BLUE		RGB( 0x00, 0x00, 0xFF )
#define	D_COLOR_MAGENTA		RGB( 0xFF, 0x00, 0xFF )
#define	D_COLOR_YELLOW		RGB( 0xFF, 0xFF, 0x00 )
#define	D_COLOR_WHITE		RGB( 0xFF, 0xFF, 0xFF )
#define	D_COLOR_BLACK		RGB( 0x00, 0x00, 0x00)
#define	D_COLOR_CYAN		RGB( 0x00, 0xFF, 0xFF )
#define	D_COLOR_DARK_GRAY	RGB( 0xC0, 0xC0, 0xC0 )
#define	D_COLOR_GRAY		RGB( 0x80, 0x80, 0x80 )
#define D_COLOR_SILVER		( D_COLOR_YELLOW | D_COLOR_GRAY )

#define	D_COLOR_LTGREEN		RGB( 0x00, 0xC0, 0x80 )

#define SELECTION_WITDH		5

class CDrawing : public CObject
{
	DECLARE_DYNAMIC(CDrawing)

protected:
	CRect m_rectBounding;		// Smallest rect that surrounds all
								// of the points in the draw

	BOOL m_bSelected;			// Is the item selected

	// Draw attributes
	//
	INT m_PenStyle;
	INT m_PenWidth;

	COLORREF m_Color;

public:
	// Construction/destruction
	//
	CDrawing();
	~CDrawing();

	// Main operations
	//
	virtual void Draw( CDC* pDC );
	virtual void Finish( CDC* pDC );
	virtual void Move( LONG DeltaX, LONG DeltaY );
	virtual BOOL HitTest( CPoint point );

	// Get properties
	//
	CRect& GetBoundingRect() { 
		return m_rectBounding; 
	}
	INT GetPenStyle() const {
		return m_PenStyle;
	}
	INT GetPenWidth() const {
		return m_PenWidth;
	}
	COLORREF GetColor() const {
		return m_Color;
	}
	BOOL IsSelected() const {
		return m_bSelected;
	}
	
	// Set properties
	//
	void SetBoundingRect( const CRect& r ) {
		m_rectBounding = r;
	}
	void SetPenStyle( INT ps ) {
		m_PenStyle = ps;
	}
	void SetPenWidth( INT pw ) {
		m_PenWidth = pw;
	}
	void SetColor( INT pc ) {
		m_Color = pc;
	}
	void SetSelected( BOOL b ) {
		m_bSelected = b;
	}
};
#endif 