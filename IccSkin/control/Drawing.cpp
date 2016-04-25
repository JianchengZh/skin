///////////////////////////////////////////////////////////////////////////////////
// Implementation file Drawing.cpp
// Feb 10, 2004. Jose Reyes.
//

#include "stdafx.h"
#include "Drawing.h"

///////////////////////////////////////////////////////////////////////////////////
// Construction/destruction for class CDrawing

IMPLEMENT_DYNAMIC( CDrawing, CObject );

CDrawing::CDrawing()
	:m_bSelected(FALSE)
	,m_PenStyle(PS_SOLID)
	,m_PenWidth(1)
	,m_Color(D_COLOR_BLACK)
{
}

CDrawing::~CDrawing()
{
}

///////////////////////////////////////////////////////////////////////////////////
// Main operations for class CDrawing

void CDrawing::Draw( CDC* pDC )
{
}

void CDrawing::Finish( CDC* pDC )
{
}

void CDrawing::Move( LONG DeltaX, LONG DeltaY )
{
	m_rectBounding.OffsetRect( DeltaX, DeltaY );
}

BOOL CDrawing::HitTest( CPoint point )
{
	CRect r( abs( m_rectBounding.left ),
		abs( m_rectBounding.top ),
		abs( m_rectBounding.right ),
		abs( m_rectBounding.bottom ) );
	CPoint p( abs( point.x ), abs( point.y ) );

	return r.PtInRect( p );
}