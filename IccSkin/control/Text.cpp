///////////////////////////////////////////////////////////////////////////////////////////
// Implementation file Text.cpp

#include "stdafx.h"
#include "Text.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Construction/destruction for class CText

IMPLEMENT_DYNAMIC( CText, CDrawing );

CText::CText()
	:m_Text("")
	,m_TextAlignment(DT_LEFT)
{
}

CText::~CText()
{
	if(m_Font.m_hObject)
		m_Font.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////////////////
// Main operations for class CText

void CText::Draw( CDC* pDC )
{
	//	Save the old state
	//
	CFont* pOldFont;
	int OldBkMode = pDC->SetBkMode( TRANSPARENT );
	if ( HasFont() )
		pOldFont = pDC->SelectObject( &m_Font );

	CPen pen;
	CBrush brush;

	pen.CreatePen( m_PenStyle, m_PenWidth, m_Color );
	CPen* pOldPen = pDC->SelectObject( &pen );

	brush.CreateSolidBrush( m_Color );
	CBrush* pOldBrush = pDC->SelectObject( &brush );

	// Do the dew
	//
	pDC->DrawText( m_Text, m_rectBounding, DT_WORDBREAK | DT_EDITCONTROL |
		DT_EXPANDTABS | m_TextAlignment | DT_VCENTER );

	if ( IsSelected() && !pDC->IsPrinting() )
	{
		pDC->Rectangle( m_rectBounding.left - SELECTION_WITDH,
			m_rectBounding.top + SELECTION_WITDH,
			m_rectBounding.left + SELECTION_WITDH,
			m_rectBounding.top - SELECTION_WITDH );

		pDC->Rectangle( m_rectBounding.right - SELECTION_WITDH,
			m_rectBounding.top + SELECTION_WITDH,
			m_rectBounding.right + SELECTION_WITDH,
			m_rectBounding.top - SELECTION_WITDH );

		pDC->Rectangle( m_rectBounding.right - SELECTION_WITDH,
			m_rectBounding.bottom + SELECTION_WITDH,
			m_rectBounding.right + SELECTION_WITDH,
			m_rectBounding.bottom - SELECTION_WITDH );

		pDC->Rectangle( m_rectBounding.left - SELECTION_WITDH,
			m_rectBounding.bottom + SELECTION_WITDH,
			m_rectBounding.left + SELECTION_WITDH,
			m_rectBounding.bottom - SELECTION_WITDH );
	}

	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );

	// Restore objects
	//
	pDC->SetBkMode( OldBkMode );
	if ( HasFont() )
		pDC->SelectObject( pOldFont );
}

void CText::Finish( CDC* pDC )
{
	CFont* pOldFont;
	if ( HasFont() )
		pOldFont = pDC->SelectObject( &m_Font );

	CString text = _T("   ") + m_Text + _T("   ");
	INT nOldRight = m_rectBounding.right;

	pDC->DrawText( text, m_rectBounding, DT_EDITCONTROL | DT_CALCRECT | DT_EXPANDTABS );

	if ( nOldRight > m_rectBounding.right )
		m_rectBounding.right = nOldRight;

	if ( HasFont() )
		pDC->SelectObject( pOldFont );
}

CText* CText::NewText( CDC* pDC, LPCTSTR szText, INT l, INT t, INT r, INT b,
	LONG fh /*= 17*/, LONG fw /*= FW_NORMAL*/, LPCTSTR ff /*= NULL*/,
	BOOL fi /*= FALSE*/, BOOL fu /*= FALSE*/, BOOL fs /*= FALSE*/,
	BYTE fp /*= DEFAULT_PITCH*/, COLORREF pc /*= D_COLOR_BLACK*/,
	UINT ta /*= DT_LEFT*/ )
{
	CText* pResult = new CText;

	pResult->SetText( szText );
	pResult->SetColor( pc );
	pResult->SetFont( fh, fw, ff, fi, fu, fs, fp );
	pResult->SetBoundingRect( CRect( l, t, r, b ) );
	pResult->SetTextAlignment( ta );
	pResult->Finish( pDC );

	return pResult;
}

////////////////////////////////////////////////////////////////////////////////////
// Set properties for class CText

void CText::SetFont( LONG Height /*= 17*/,
	LONG Weight /*= FW_NORMAL*/,
	LPCTSTR szFaceName /*= NULL*/,
	BOOL Italic /*= FALSE*/,
	BOOL Underline /*= FALSE*/,
	BOOL StrikeOut /*= FALSE*/,
	BYTE PitchAndFamily /*= DEFAULT_PITCH*/)
{
	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	LOGFONT lf;

	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = Height;
	lf.lfWeight = Weight;
	lf.lfPitchAndFamily = PitchAndFamily;
	lf.lfItalic = Italic;
	lf.lfUnderline = Underline;
	lf.lfStrikeOut = StrikeOut;
	if ( szFaceName != NULL )
		_tcsncpy_s( lf.lfFaceName, szFaceName, LF_FACESIZE - 1);
	else
		_tcsncpy_s( lf.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE - 1);

	VERIFY(m_Font.CreateFontIndirect(&lf));
}