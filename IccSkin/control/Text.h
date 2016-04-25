////////////////////////////////////////////////////////////////////////////////////////////
// Header file Text.h
// Feb 10, 2004. Jose Reyes
//

#ifndef _TEXT_H_
#define _TEXT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Drawing.h"

#define	DEFAULT_TEXT_WIDTH		100
#define DEFAULT_TEXT_HEIGHT	    100

//InplaceEdit 에서 사용

class CText : public CDrawing
{
	DECLARE_DYNAMIC( CText );

private:
	CString m_Text;
	CFont m_Font;
	UINT m_TextAlignment;

public:
	// Construction/destruction
	//
	CText();
	~CText();

	// Main operations
	//
	virtual void Draw( CDC* pDC );
	virtual void Finish( CDC* pDC );

	static CText* NewText( CDC* pDC, LPCTSTR szText, INT l, INT t, INT r, INT b, 
						   LONG fh = 17, LONG fw = FW_NORMAL, LPCTSTR ff = NULL, 
						   BOOL fi = FALSE, BOOL fu = FALSE, BOOL fs = FALSE,
						   BYTE fp = DEFAULT_PITCH, COLORREF pc = D_COLOR_BLACK,
						   UINT ta = DT_LEFT );

	// Get properties
	//
	LPCTSTR GetText() const {
		return m_Text;
	}
	BOOL HasFont() const {
		return ( m_Font.m_hObject != NULL );
	}
	UINT GetTextAlignment() const {
		return m_TextAlignment;
	}

	// Set properties
	//
	void SetText( LPCTSTR s ) {
		m_Text = s;
	}
	void SetText( UINT nRes ) {
		m_Text.LoadString( nRes );
	}
	void SetFont( LONG Height = 17,
				  LONG Weight = FW_NORMAL, 
				  LPCTSTR szFaceName = NULL,
				  BOOL Italic = FALSE, 
				  BOOL Underline = FALSE, 
				  BOOL StrikeOut = FALSE,
				  BYTE PitchAndFamily = DEFAULT_PITCH );
	void SetTextAlignment( UINT ta ) {
		m_TextAlignment = ta;
	}
};

#endif 