//////////////////////////////////////////////////////////////////////////////////////
// Implementation file InplaceEdit.cpp
// March 18, 2004. Jose Reyes.
//

#include "stdafx.h"
#include "InplaceEdit.h"

//////////////////////////////////////////////////////////////////////////////////////
// Construction/destruction for class CInplaceEdit

IMPLEMENT_DYNCREATE(CInplaceEdit, CEdit)

BEGIN_MESSAGE_MAP(CInplaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInplaceEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_UPDATE, CInplaceEdit::OnUpdate)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

CInplaceEdit::CInplaceEdit()
:m_strFont(FONT_NAME)
,m_nFontSize(FONT_SIZE)
,m_corFont(COLOR_BLACK)
,m_corBG(COLOR_WHITE)
,m_corFrame(COLOR_WHITE)
{
}

CInplaceEdit::~CInplaceEdit()
{
	if(m_Font.m_hObject)
		m_Font.DeleteObject();
	//delete brush
	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
}

BOOL CInplaceEdit::CreateEditor( CWnd* pParentWnd )
{
	BOOL bResult = FALSE;

	CRect r( 0, 0, ED_DEFAULT_WIDTH, ED_DEFAULT_HEIGHT );

	if ( Create( ES_WANTRETURN | ES_LEFT | ES_MULTILINE | WS_CHILD | WS_VISIBLE,
		r, pParentWnd, IDC_INPLACE_EDIT ) )
	{
		SetParent( pParentWnd );
		SetLimitText( 0 );

		bResult = TRUE;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////////
// Main operations for class CInplaceEdit

void CInplaceEdit::Deselect()
{
	SendMessage( EM_SETSEL, 0x7FFFFFFF, 0xFFFFFFFF );
}

void CInplaceEdit::Hide()
{
	if ( m_hWnd != NULL )
	{
		HWND focus = ::GetFocus();
		ShowWindow( SW_HIDE );
		MoveWindow( CRect( 0, 0, 0, 0 ), FALSE );
		if ( focus == m_hWnd )
			GetParent()->SetFocus();
		m_pText = NULL;
	}
}

void CInplaceEdit::UpdateEditor( CText* pText )
{
	UpdateContents( pText );
	SetFocus();
	SelectAll();
}

void CInplaceEdit::UpdateContents( CText* pText )
{
	CRect TextR;
	CString Text;

	m_pText = pText;
	CClientDC dc( GetParent() );

	INT OldMapMode = dc.SetMapMode( MM_LOENGLISH );

	Text = m_pText->GetText();
	TextR = m_pText->GetBoundingRect();

	TextR.right++; // ???
	TextR.OffsetRect( -3, 0 ); // The border of the editor ???
	PrepareBackground( TextR );

	CSize sz( 0, 17 );
	dc.LPtoDP( &sz );
	UpdateFont( sz.cy );

	dc.LPtoDP( TextR );

	MoveWindow( TextR, TRUE );
	SetWindowText( Text );

	SendMessage( WM_SETFONT, (WPARAM)m_Font.m_hObject, (LPARAM)TRUE );
	ShowWindow( SW_SHOW );

	dc.SetMapMode( OldMapMode );
}

void CInplaceEdit::SetFocus()
{
	if ( IsWindowVisible() )
		CEdit::SetFocus();
}

void CInplaceEdit::SelectAll()
{
	SendMessage( EM_SETSEL, 0, -1 );
}

// Take a chunk of the parents bitmap and create a background brush with it
//
void CInplaceEdit::PrepareBackground( CRect pos )
{
	CClientDC thisDC( this );
	CClientDC parentDC( GetParent() );
	CDC thisMem;

	INT OldParentMapMode = parentDC.SetMapMode( MM_LOENGLISH );

	CBitmap* pOldThisBmp;

	thisMem.CreateCompatibleDC( &thisDC );
	INT OldThisMapMode = thisMem.SetMapMode( MM_LOENGLISH );

	CSize sz = pos.Size();

	m_Background.DeleteObject();
	m_Background.CreateCompatibleBitmap( &thisDC, sz.cx, -sz.cy );

	pOldThisBmp = thisMem.SelectObject( &m_Background );

	thisMem.BitBlt( 0, 0, sz.cx, sz.cy, &parentDC, pos.left, pos.top, SRCCOPY );

	thisMem.SetMapMode( OldThisMapMode );
	thisMem.SelectObject( pOldThisBmp );

	parentDC.SetMapMode( OldParentMapMode );

	m_Brush.DeleteObject();

	m_Brush.CreatePatternBrush( &m_Background );
	if ( m_Brush.m_hObject != NULL )
		m_Brush.UnrealizeObject();

	//m_Brush.GetStockObject(NULL_BRUSH); // This option is very simple but selected text is not repainted
}

void CInplaceEdit::UpdateFont( LONG nHeight )
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = nHeight;
	lf.lfWeight = FW_NORMAL;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	_tcscpy_s(lf.lfFaceName, m_strFont);

	if(m_Font.m_hObject)
		m_Font.DeleteObject();
	VERIFY(m_Font.CreateFontIndirect(&lf));
}

/////////////////////////////////////////////////////////////////////////////
// CInplaceEdit message handlers

void CInplaceEdit::OnKillfocus()
{
	//CWnd* pParent = GetParent();

	//ASSERT_VALID( pParent );

	//pParent->SendMessage( UM_HIDEEDITOR, 0L, 0L );
}

HBRUSH CInplaceEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	//pDC->SelectObject( &m_Brush );

	//pDC->SetBrushOrg( 0, 0 );

	//pDC->SetBkMode(TRANSPARENT);

	pDC->SetTextColor(m_corFont);
	pDC->SetBkColor(m_corBG);

	//return (HBRUSH)m_Brush;
	return m_brBackGnd;
}

void CInplaceEdit::OnUpdate()
{
	InvalidateRect( NULL, TRUE );
}

void CInplaceEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//if ( nChar == VK_ESCAPE )
	//{
	//	CWnd* pParent = GetParent();

	//	ASSERT_VALID( pParent );

	//	pParent->SendMessage( UM_HIDEEDITOR, 0L, 0L );
	//}

	CEdit::OnKeyUp( nChar, nRepCnt, nFlags );
}

void CInplaceEdit::SetFontColor(COLORREF fontRGB)
{
	m_corFont = fontRGB;

	//redraw
	Invalidate(TRUE);
}

void CInplaceEdit::SetBGColor(COLORREF bgRGB)
{
	m_corBG=bgRGB;

	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();

	m_brBackGnd.CreateSolidBrush(m_corBG);

	Invalidate(TRUE);
}

void CInplaceEdit::SetFrameColor(COLORREF frRGB)
{
	m_corFrame=frRGB;

	Invalidate(TRUE);
}

void CInplaceEdit::SetFontType(CString strFont, COLORREF corFont,int nFontSize,BOOL bBold)
{
	m_corFont = corFont;

	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure

	NONCLIENTMETRICS nm;
	nm.cbSize = sizeof (NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0);

	memcpy(&lf, &(nm.lfMenuFont), sizeof(LOGFONT));

	if(0 == strFont.CompareNoCase(_T("")))
		strFont = FONT_NAME;
	m_strFont = strFont;
	lstrcpy(lf.lfFaceName, m_strFont);

	if(nFontSize <= 0)
		nFontSize = FONT_SIZE;
	m_nFontSize = nFontSize;
	lf.lfHeight = m_nFontSize;
	if(bBold) lf.lfWeight = FW_BOLD;
	else lf.lfWeight = FW_MEDIUM;

	//#define FW_MEDIUM           500
	//#define FW_SEMIBOLD         600
	//#define FW_BOLD             700

	VERIFY(m_Font.CreateFontIndirect(&lf));

	SendMessage( WM_SETFONT, (WPARAM)m_Font.m_hObject, (LPARAM)TRUE );
}

void CInplaceEdit::SetID(CString strName, int nID)
{
	m_strName = strName;
	m_nID = nID;
}

int CInplaceEdit::GetId()
{
	return m_nID;
}