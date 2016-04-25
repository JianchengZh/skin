#include "stdafx.h"
#include "xSkinButton.h"

#include "Define.h"
#include "SkinUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CxSkinButton
CxSkinButton::CxSkinButton()
	:m_DrawMode(1)
	,m_FocusRectMargin(0)
	,hClipRgn(NULL)
	,m_button_down(false)
	,m_tracking(false)
	,m_Checked(false)
	,m_nId(0)
	,m_clrText(COLOR_BLACK)
	,m_clrFocusText(COLOR_WHITE)
	,m_bUseHandCursor(false)
{
}
/////////////////////////////////////////////////////////////////////////////
CxSkinButton::~CxSkinButton()
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bMask.DeleteObject();
	m_bFocus.DeleteObject();

	if(m_Font.m_hObject)
		m_Font.DeleteObject();

	if (hClipRgn)
		DeleteObject(hClipRgn);	// free clip region

}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CxSkinButton, CButton)
	//{{AFX_MSG_MAP(CxSkinButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	//ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_CXSHADE_RADIO , OnRadioInfo)
	ON_MESSAGE(BM_SETCHECK , OnBMSetCheck)
	ON_MESSAGE(BM_GETCHECK , OnBMGetCheck)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton message handlers
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::PreSubclassWindow() 
{
	m_Style=GetButtonStyle();	///get specific BS_ styles
	if ((m_Style & BS_AUTOCHECKBOX)==BS_AUTOCHECKBOX)
		//		||((m_Style & BS_CHECKBOX)==BS_CHECKBOX))
		m_Style=BS_CHECKBOX;
	else if ((m_Style & BS_AUTORADIOBUTTON)==BS_AUTORADIOBUTTON)
		//			||((m_Style & BS_RADIOBUTTON)==BS_RADIOBUTTON))
		m_Style=BS_RADIOBUTTON;
	else { m_Style=BS_PUSHBUTTON; }

	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CxSkinButton::OnEraseBkgnd(CDC* pDC) 
{	return 1; }	// doesn't erase the button background
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

	ASSERT (lpDrawItemStruct);
	//TRACE("* Captured: %08X\n", ::GetCapture());

	//Check if the button state in not in inconsistent mode...
	POINT mouse_position;
	if ((m_button_down) && (::GetCapture() == m_hWnd) && (::GetCursorPos(&mouse_position))){
		if (::WindowFromPoint(mouse_position) == m_hWnd){
			if ((GetState() & BST_PUSHED) != BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");

				SetState(TRUE);
				return;
			}
		} else {
			if ((GetState() & BST_PUSHED) == BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(FALSE);
				return;
			}
		}
	}

	//TRACE("* Drawing: %08x\n", lpDrawItemStruct->itemState);
	CString sCaption;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	// get device context
	RECT r=lpDrawItemStruct->rcItem;					// context rectangle

	// get text box position
	RECT tr={r.left+m_FocusRectMargin+2,r.top,r.right-m_FocusRectMargin-2,r.bottom};

	GetWindowText(sCaption);							// get button text
	pDC->SetBkMode(TRANSPARENT);

	// Select the correct skin 
	if (lpDrawItemStruct->itemState & ODS_DISABLED){	// DISABLED BUTTON
		if(m_bDisabled.m_hObject==NULL)
			// no skin selected for disabled state -> standard button
			pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
		else // paint the skin
			DrawBitmap(pDC,(HBITMAP)m_bDisabled,r,m_DrawMode);
		// if needed, draw the standard 3D rectangular border
		if (m_Border) pDC->DrawEdge(&r,EDGE_RAISED,BF_RECT);
		// paint the etched button text
		pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
		pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		OffsetRect(&tr,-1,-1);
		pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	} else {										// SELECTED (DOWN) BUTTON
		if ((lpDrawItemStruct->itemState & ODS_SELECTED)||m_Checked){
			if(m_bDown.m_hObject==NULL)
				// no skin selected for selected state -> standard button
				pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
			else { // paint the skin
				DrawBitmap(pDC,(HBITMAP)m_bDown,r,m_DrawMode);
			}
			//OffsetRect(&tr,1,1);  //shift text //보다폰에서만 사용.
			// if needed, draw the standard 3D rectangular border
			if (m_Border) pDC->DrawEdge(&r,EDGE_SUNKEN,BF_RECT);
		} else {											// DEFAULT BUTTON
			if(m_bNormal.m_hObject==NULL)
				// no skin selected for normal state -> standard button
				pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
			else // paint the skin
				if ((m_tracking)&&(m_bOver.m_hObject!=NULL)){
					DrawBitmap(pDC,(HBITMAP)m_bOver,r,m_DrawMode);
					pDC->SetTextColor(m_clrFocusText);
				} else {
					if ((lpDrawItemStruct->itemState & ODS_FOCUS)&&(m_bFocus.m_hObject!=NULL)){
						DrawBitmap(pDC,(HBITMAP)m_bFocus,r,m_DrawMode);
						pDC->SetTextColor(m_clrFocusText);
					} else {
						DrawBitmap(pDC,(HBITMAP)m_bNormal,r,m_DrawMode);
						pDC->SetTextColor(m_clrText);
					}
				}
				// if needed, draw the standard 3D rectangular border
				if (m_Border) pDC->DrawEdge(&r,EDGE_RAISED,BF_RECT);
		}
		// paint the focus rect
		if ((lpDrawItemStruct->itemState & ODS_FOCUS)&&(m_FocusRectMargin>0)){
			r.left   += m_FocusRectMargin ;
			r.top    += m_FocusRectMargin ;
			r.right  -= m_FocusRectMargin ;
			r.bottom -= m_FocusRectMargin ;
			DrawFocusRect (lpDrawItemStruct->hDC, &r) ;
		}

		// 보다폰용으로 하드코딩됨.. 추후 다른데서 사용할경우 알맞게 폰트& Text위치 셋팅할것
		if (m_tracking || m_Checked
			/*|| lpDrawItemStruct->itemState & ODS_FOCUS */
			|| lpDrawItemStruct->itemState & ODS_SELECTED
			/*|| lpDrawItemStruct->itemState & ODS_CHECKED */)
		{
				//m_TextColor = RGB(255,0,0);
				pDC->SetTextColor(m_clrFocusText);
		}else{
			//m_TextColor = COLOR_WHITE;
			pDC->SetTextColor(m_clrText);
		}

		CFont *old_font;
		old_font = pDC->SelectObject( &m_Font );		
		
		// paint the enabled button text
		//pDC->SetTextColor(m_TextColor);
		//pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		pDC->SelectObject(&old_font);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CxSkinButton::GetBitmapWidth (HBITMAP hBitmap)
{ BITMAP bm; GetObject(hBitmap,sizeof(BITMAP),(PSTR)&bm); return bm.bmWidth;}
/////////////////////////////////////////////////////////////////////////////
int CxSkinButton::GetBitmapHeight (HBITMAP hBitmap)
{ BITMAP bm; GetObject(hBitmap,sizeof(BITMAP),(PSTR)&bm); return bm.bmHeight;}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode)
{
	//	DrawMode: 0=Normal; 1=stretch; 2=tiled fill
	if(DrawMode==2){
		FillWithBitmap(dc,hbmp,r);
		return;
	}
	if(!hbmp) return;	//safe check

	CBitmap *pImage;
	pImage = CBitmap::FromHandle(hbmp);

	int cx=r.right  - r.left;
	int cy=r.bottom - r.top;
	CDC dcBmp,dcMask;
	dcBmp.CreateCompatibleDC(dc);
	CBitmap* pOlddcBmp = dcBmp.SelectObject(pImage);


	if (m_bMask.m_hObject!=NULL){
		dcMask.CreateCompatibleDC(dc);
		dcMask.SelectObject(&m_bMask);

		CDC hdcMem;
		hdcMem.CreateCompatibleDC(dc);
		CBitmap hBitmap;
		hBitmap.CreateCompatibleBitmap(dc,cx,cy);
		CBitmap* poldbitmap = hdcMem.SelectObject(&hBitmap);

		//hdcMem.BitBlt(r.left,r.top,cx,cy,dc,0,0,SRCCOPY);
		hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCCOPY);

		if(!DrawMode){
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcMask,0,0,SRCAND);
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
		} else {
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcMask,0,0,bx,by,SRCAND);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
		}
		dc->BitBlt(r.left,r.top,cx,cy,&hdcMem,0,0,SRCCOPY);

		hdcMem.SelectObject(&poldbitmap);
		hdcMem.DeleteDC();
		hBitmap.DeleteObject();

		DeleteDC(dcMask);
	} else {
		if(!DrawMode){
			dc->BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCCOPY);
		} else {
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			dc->StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCCOPY);
		}
	}

	dcBmp.SelectObject(pOlddcBmp);
	DeleteDC(dcBmp);
}

//void CxSkinButton::DrawBitmap(CDC* dc, CBitmap &bmp, RECT r, int DrawMode)
//{
////	DrawMode: 0=Normal; 1=stretch; 2=tiled fill
//	if(DrawMode==2){
//		FillWithBitmap(dc,hbmp,r);
//		return;
//	}
//	if(!hbmp) return;	//safe check
//
//	int cx=r.right  - r.left;
//	int cy=r.bottom - r.top;
//	CDC dcBmp,dcMask;
//	dcBmp.CreateCompatibleDC(dc);
//	CBitmap *pOlddcBmp = dcBmp.SelectObject(&bmp);
//
//	//dc->FillSolidRect(0,0,cx,cy, RGB(0,0,0));
//	
//	if (m_bMask.m_hObject!=NULL){
//		dcMask.CreateCompatibleDC(dc);
//		dcMask.SelectObject(&m_bMask);
//
//		CDC hdcMem;
//		hdcMem.CreateCompatibleDC(dc);
//	    CBitmap hBitmap;
//		hBitmap.CreateCompatibleBitmap(dc,cx,cy);
//	    CBitmap* poldbitmap = hdcMem.SelectObject(&hBitmap);
//		
//        hdcMem.BitBlt(r.left,r.top,cx,cy,dc,0,0,SRCCOPY);
//		if(!DrawMode){
//			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
//			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcMask,0,0,SRCAND);
//			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
//		} else {
//			int bx=GetBitmapWidth(hbmp);
//			int by=GetBitmapHeight(hbmp);
//			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
//			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcMask,0,0,bx,by,SRCAND);
//			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
//		}
//        dc->BitBlt(r.left,r.top,cx,cy,&hdcMem,0,0,SRCCOPY);
//
//		hdcMem.SelectObject(&poldbitmap);
//		hdcMem.DeleteDC();
//		hBitmap.DeleteObject();
//
//		DeleteDC(dcMask);
//	} else {
//		if(!DrawMode){
//			dc->BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCCOPY);
//		} else {
//			int bx=GetBitmapWidth(hbmp);
//			int by=GetBitmapHeight(hbmp);
//			dc->StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCCOPY);
//		}
//	}
//
//	pOlddcBmp = dcBmp.SelectObject(pOlddcBmp);
//	DeleteDC(dcBmp);
//}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r)
{
	if(!hbmp) return;
	CDC memdc;
	memdc.CreateCompatibleDC(dc);
	memdc.SelectObject(hbmp);
	int w = r.right - r.left;
	int	h = r.bottom - r.top;
	int x,y,z;
	int	bx=GetBitmapWidth(hbmp);
	int	by=GetBitmapHeight(hbmp);
	for (y = r.top ; y < h ; y += by){
		if ((y+by)>h) by=h-y;
		z=bx;
		for (x = r.left ; x < w ; x += z){
			if ((x+z)>w) z=w-x;
			dc->BitBlt(x, y, z, by, &memdc, 0, 0, SRCCOPY);
		}
	}
	DeleteDC(memdc);
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::SetSkin(UINT normal,UINT down,UINT over,UINT disabled, UINT focus,UINT mask,
	short drawmode, short border, short margin)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bMask.DeleteObject();
	m_bFocus.DeleteObject();

	if (normal>0) m_bNormal.LoadBitmap(normal);
	if (down>0)	  m_bDown.LoadBitmap(down);
	if (over>0)	  m_bOver.LoadBitmap(over);
	if (focus>0)  m_bFocus.LoadBitmap(focus);

	if (disabled>0) m_bDisabled.LoadBitmap(disabled);
	else if (normal>0) m_bDisabled.LoadBitmap(normal);

	m_DrawMode=max(0,min(drawmode,2));
	m_Border=border;
	m_FocusRectMargin=max(0,margin);

	if (mask>0){
		m_bMask.LoadBitmap(mask);
		if (hClipRgn) DeleteObject(hClipRgn);
		hClipRgn = CreateRgnFromBitmap(m_bMask,COLOR_WHITE);
		if (hClipRgn){
			SetWindowRgn(hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(),hClipRgn);
		}
		if (m_DrawMode==0){
			SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),
				GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
		}
	}
}

void CxSkinButton::CopyFrom(CBitmap& bmSkinImage, CRect NormalRect, CRect OverRect, CRect DownRect, CRect DisabledRect, CRect MaskRect, short drawmode, short border, short margin)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bMask.DeleteObject();
	m_bFocus.DeleteObject();

	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bNormal, bmSkinImage, NormalRect);
	skinUtil.CopyBitmap(pDC, m_bOver, bmSkinImage, OverRect);
	skinUtil.CopyBitmap(pDC, m_bDown, bmSkinImage, DownRect);
	skinUtil.CopyBitmap(pDC, m_bDisabled, bmSkinImage, DisabledRect);
	if(MaskRect.Width() > 0)
		skinUtil.CopyBitmap(pDC, m_bMask, bmSkinImage, MaskRect);
	ReleaseDC(pDC);

	m_DrawMode=max(0,min(drawmode,2));
	m_Border=border;
	m_FocusRectMargin=max(0,margin);


	if (m_bMask.m_hObject){
		if (hClipRgn) DeleteObject(hClipRgn);
		hClipRgn = CreateRgnFromBitmap(m_bMask,COLOR_WHITE);
		if (hClipRgn){
			SetWindowRgn(hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(),hClipRgn);
		}
		if (m_DrawMode==0){
			SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),
				GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
		}
	}
}


void CxSkinButton::CopyFrom(/*CDC* pDC, */CRect rect, CBitmap& bmNormal, CBitmap& bmOver, CBitmap& bmClick, CBitmap& bmDisable, CBitmap& bmMask, short drawmode, short border, short margin)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bMask.DeleteObject();
	m_bFocus.DeleteObject();

	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bNormal, bmNormal, rect);
	skinUtil.CopyBitmap(pDC, m_bOver, bmOver, rect);
	skinUtil.CopyBitmap(pDC, m_bDown, bmClick, rect);
	skinUtil.CopyBitmap(pDC, m_bDisabled, bmDisable, rect);
	skinUtil.CopyBitmap(pDC, m_bMask, bmMask, rect);
	ReleaseDC(pDC);

	/*
	if (normal>0) m_bNormal.LoadBitmap(normal);
	if (down>0)	  m_bDown.LoadBitmap(down);
	if (over>0)	  m_bOver.LoadBitmap(over);
	if (focus>0)  m_bFocus.LoadBitmap(focus);

	if (disabled>0) m_bDisabled.LoadBitmap(disabled);
	else if (normal>0) m_bDisabled.LoadBitmap(normal);
	*/
	m_DrawMode=max(0,min(drawmode,2));
	m_Border=border;
	m_FocusRectMargin=max(0,margin);

	//if (mask>0){
	if (m_bMask.m_hObject){
		//		m_bMask.LoadBitmap(mask);
		if (hClipRgn) DeleteObject(hClipRgn);
		hClipRgn = CreateRgnFromBitmap(m_bMask,COLOR_WHITE);
		if (hClipRgn){
			SetWindowRgn(hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(),hClipRgn);
		}
		if (m_DrawMode==0){
			SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),
				GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
HRGN CxSkinButton::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	if (!hBmp) return NULL;

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes

	CDC dcBmp;
	dcBmp.CreateCompatibleDC(GetDC());	//Creates a memory device context for the bitmap
	dcBmp.SelectObject(hBmp);			//selects the bitmap in the device context

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
	const DWORD MAXBUF = 40;		// size of one block in RECTs
	// (i.e. MAXBUF*sizeof(RECT) in bytes)
	LPRECT	pRects;								
	DWORD	cBlocks = 0;			// number of allocated blocks

	INT		i, j;					// current position in mask image
	INT		first = 0;				// left position of current scan line
	// where mask was found
	bool	wasfirst = false;		// set when if mask was found in current scan line
	bool	ismask;					// set when current color is mask color

	// allocate memory for region data
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
	memset( pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RDHDR;
	pRgnData->iType		= RDH_RECTANGLES;
	pRgnData->nCount	= 0;
	for ( i = 0; i < bm.bmHeight; i++ )
		for ( j = 0; j < bm.bmWidth; j++ ){
			// get color
			ismask=(dcBmp.GetPixel(j,bm.bmHeight-i-1) != color);
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if (wasfirst && ((ismask && (j==(bm.bmWidth-1)))||(ismask ^ (j<bm.bmWidth)))){
				// get offset to RECT array if RGNDATA buffer
				pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
				// save current RECT
				pRects[ pRgnData->nCount++ ] = CRect( first, bm.bmHeight - i - 1, j+(j==(bm.bmWidth-1)), bm.bmHeight - i );
				// if buffer full reallocate it
				if ( pRgnData->nCount >= cBlocks * MAXBUF ){
					LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
					memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT) );

					delete [] pRgnData;
					pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				}
				wasfirst = false;
			} else if ( !wasfirst && ismask ){		// set wasfirst when mask is found
				first = j;
				wasfirst = true;
			}
		}
		dcBmp.DeleteDC();	//release the bitmap
		// create region
		/*  Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net) */
		//	HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
		/* ExtCreateRegion replacement { */
		HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
		ASSERT( hRgn!=NULL );
		pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
		for(i=0;i<(int)pRgnData->nCount;i++)
		{
			HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
			VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
			if (hr) DeleteObject(hr);
		}
		ASSERT( hRgn!=NULL );
		/* } ExtCreateRegion replacement */


		delete [] pRgnData;
		return hRgn;
}
/////////////////////////////////////////////////////////////////////////////

void CxSkinButton::SetTextColor(COLORREF clrText, COLORREF clrFocusText)
{
	m_clrText = clrText;
	m_clrFocusText = clrFocusText;
}

void CxSkinButton::Init(void)
{
	int nWeight = FW_NORMAL;

	if (m_bFontBold)
	{
		nWeight = FW_SEMIBOLD;
	}
	m_Font.DeleteObject();
	VERIFY(m_Font.CreateFont(
		m_nFontSize, 
		0, 
		0, 
		0, 
		nWeight, 
		FALSE, 
		FALSE, 
		0, 
		DEFAULT_CHARSET,OUT_SCREEN_OUTLINE_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, 
		DEFAULT_PITCH|FF_MODERN,
		m_strFontName));
}

/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::SetToolTipText(CString s)
{
	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this, TTS_ALWAYSTIP)){	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(TRUE);


			m_tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);
			//m_tooltip.SetDelayTime(300);
		}
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
		//m_tooltip.SetDelayTime(300);
	}
}

void CxSkinButton::SetToolTipText(CString s, CString sDown)
{
	m_ToolTipUp = s;		//normal tooltip
	m_ToolTipDw = sDown;	//tooltip for checked state

	//select the down tooltip if the button is already checked <Jesper Kinn?>
	if (m_Checked && !m_ToolTipDw.IsEmpty()) s = m_ToolTipDw;

	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this))	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s)){
				m_tooltip.Activate(1);
				// enable multiline tooltips <Derek Lakin>
				m_tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);
				m_tooltip.SetDelayTime(300);
			}
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
		m_tooltip.SetDelayTime(300);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// This function will create a MSG structure, fill it in a pass it to
	// the ToolTip control, m_tooltip.  Note that we ensure the point is in window
	// coordinates (relative to the control's window).
	if(NULL != m_tooltip.m_hWnd){
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.RelayEvent(&msg);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonDblClk
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: Used only to be forwarded as WM_LBUTTONDOWN message parameters
//Exceptions..: NONE
//------------
//Description :
//
//  > We do not care about doublelicks - handle this event
//    like an ordinary left-button-down event
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonDblClk(UINT flags, CPoint point) 
{
	SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(point.x, point.y));
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonDown
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle event when left button is pressed down
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: down\n", ::GetTickCount());

	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	//If we are tracking this button, cancel it
	if (m_tracking) {
		TRACKMOUSEEVENT t = {
			sizeof(TRACKMOUSEEVENT),
			TME_CANCEL | TME_LEAVE,
			m_hWnd,
			0
		};
		if (::_TrackMouseEvent(&t)) {
			m_tracking = false;
		}
	}

	//Default-process the message
	CButton::OnLButtonDown(nFlags, point);
	m_button_down = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonUp
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle event when left button is released (goes up)
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: up\n", ::GetTickCount());

	if (m_Style){ //track mouse for radio & check buttons
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);
		if (mouse_wnd == m_hWnd){ // mouse is in button
			if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
			if (m_Style==BS_RADIOBUTTON) SetCheck(1);
		}
	}
	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	//Default-process the message
	m_button_down = false;
	CButton::OnLButtonUp(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnMouseMove
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle change of mouse position: see the comments in the
//    method for further info.
//
//---------------------------------------------------------
void CxSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: Mouse\n", ::GetTickCount());
	if(NULL != m_tooltip.m_hWnd){
		m_tooltip.Activate(TRUE);//클릭후, 툴팁이 사라지는 문제 수정

		//Pass this message to the ToolTip control
		RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	}
	//If we are in capture mode, button has been pressed down
	//recently and not yet released - therefore check is we are
	//actually over the button or somewhere else. If the mouse
	//position changed considerably (e.g. we moved mouse pointer
	//from the button to some other place outside button area)
	//force the control to redraw
	//
	if ((m_button_down) && (::GetCapture() == m_hWnd)) {
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);

		bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
		bool need_pressed = (mouse_wnd == m_hWnd);
		if (pressed != need_pressed) {
			//TRACE("* %08X Redraw\n", GetTickCount());
			SetState(need_pressed ? TRUE : FALSE);
			Invalidate();
		}
	} else {

		//Otherwise the button is released. That means we should
		//know when we leave its area - and so if we are not tracking
		//this mouse leave event yet, start now!
		//
		if (!m_tracking) {
			TRACKMOUSEEVENT t = {
				sizeof(TRACKMOUSEEVENT),
				TME_LEAVE,
				m_hWnd,
				0
			};
			if (::_TrackMouseEvent(&t)) {
				
				m_tracking = true;
				Invalidate();
			}
		}
	}

	if (m_bUseHandCursor)
	{
		::SetCursor(LoadCursor(NULL, IDC_HAND));
	}


	//Forward this event to superclass
	CButton::OnMouseMove(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnMouseLeave
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NULL
//Parameters..: NOT USED
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle situation when mouse cursor leaves area of this
//    window (button). This event might be generated ONLY
//    if we explicitely call 'TrackMouseEvent'. This is
//    signalled by setting the m_tracking flag (see the assert
//    precondition) - in 'OnMouseMove' method
//
//  > When a mouse pointer leaves area of this button (i.e.
//    when this method is invoked), presumably the look of
//    the button changes (e.g. when hover/non-hover images are set)
//    and therefore we force the control to redraw.
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnMouseLeave(WPARAM, LPARAM)
{
	ASSERT (m_tracking);
	
	m_tracking = false;
	if(NULL != m_tooltip.m_hWnd){
		m_tooltip.Activate(FALSE);//클릭후, 툴팁이 사라지는 문제
	}
	Invalidate();
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnKillFocus
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: See superclass documentation
//Exceptions..: NONE
//------------
//Description :
//
//  > If focus is killed during capture, we may no longer
//    have the exclusive access to user input and therefore
//    release it.
//
//  > Such a situation might happens when the user left-clicks
//    this button, keeps the button down and simultaneously
//    presses TAB key.
//
//---------------------------------------------------------
void CxSkinButton::OnKillFocus(CWnd *new_wnd)
{
	if (::GetCapture() == m_hWnd) {
		::ReleaseCapture();
		ASSERT (!m_tracking);
		m_button_down = false;
	}
	CButton::OnKillFocus(new_wnd);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnClicked
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: FALSE (do not stop in this handler - forward to parent)
//Parameters..: NONE
//Exceptions..: NONE
//------------
//Description :
//
//  > Keep consistency of attributes of this instance before
//    submitting click event to the parent.
//
//  > Currently NOT used. To use, umcomment line
//    "ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)" in message map
//    at the beginning of this file.
//
//---------------------------------------------------------
BOOL CxSkinButton::OnClicked() 
{
	if (::GetCapture() == m_hWnd) {
		::ReleaseCapture();
		ASSERT (!m_tracking);
	}
	m_button_down = false;
	//Invalidate();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnRadioInfo
//Class.......: CxSkinButton
//
//Author......: Rainer Mangold
//Created.....: JUN-2001
//
//Return value: NULL
//Parameters..: WPARAM and LPARAM (LPARAM not used)
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle notification, that a Button in the same group was pushed
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnRadioInfo(WPARAM wparam, LPARAM)
{
	if (m_Checked){	//only checked buttons need to be unchecked
		m_Checked = false;

		if(m_tooltip.m_hWnd!=NULL)
			m_tooltip.UpdateTipText((LPCTSTR)m_ToolTipUp,this);

		Invalidate();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((m_Style)&&(nChar==_T(' '))){ //needed stuff for check & radio buttons
		if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
		if (m_Style==BS_RADIOBUTTON) SetCheck(1);
	}
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
//
//Method......: SetCheck
//Class.......: CxSkinButton
//
//Author......: Rainer Mangold
//Created.....: JUN-2001
//
//Return value: NONE
//Parameters..: bool
//Exceptions..: NONE
//------------
//Description :
//
//  > Set the state of this button (pushed or not). 
//    Works for both, Radio and CheckBox - Buttons
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnBMSetCheck(WPARAM wparam, LPARAM)
{
	m_Checked=wparam!=0;
	switch (m_Style)
	{
	case BS_RADIOBUTTON:
		if (m_Checked) { //uncheck the other radio buttons (in the same group)
			HWND hthis,hwnd2,hpwnd;
			hpwnd=GetParent()->GetSafeHwnd();	//get button parent handle
			hwnd2=hthis=GetSafeHwnd();			//get this button handle
			if (hthis && hpwnd){				//consistency check
				for( ; ; ){	//scan the buttons within the group
					hwnd2=::GetNextDlgGroupItem(hpwnd,hwnd2,0);
					//until we reach again this button
					if ((hwnd2==hthis)||(hwnd2==NULL)) break;
					//post the uncheck message
					::PostMessage(hwnd2, WM_CXSHADE_RADIO, 0, 0);
				}
			}
		}
		break;
	case BS_PUSHBUTTON:
		m_Checked=false;
		ASSERT(false); // Must be a Check or Radio button to use this function
	}

	//set the correct tooltip
	if (m_tooltip.m_hWnd!=NULL){
		m_tooltip.Pop();	//to avoid flicker
		if (m_Checked && !m_ToolTipDw.IsEmpty())
			m_tooltip.UpdateTipText((LPCTSTR)m_ToolTipDw,this);
		else
			m_tooltip.UpdateTipText((LPCTSTR)m_ToolTipUp,this);
	}


	Invalidate();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CxSkinButton::OnBMGetCheck(WPARAM wparam, LPARAM)
{ return m_Checked; }	//returns the state for check & radio buttons
/////////////////////////////////////////////////////////////////////////////
//EOF

void CxSkinButton::ChangeSkin(UINT normal, UINT down, UINT over, UINT disabled, UINT focus)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bFocus.DeleteObject();


	if (normal>0) m_bNormal.LoadBitmap(normal);
	if (down>0)	  m_bDown.LoadBitmap(down);
	if (over>0)	  m_bOver.LoadBitmap(over);
	if (focus>0)  m_bFocus.LoadBitmap(focus);

	if (disabled>0) m_bDisabled.LoadBitmap(disabled);
	else if (normal>0) m_bDisabled.LoadBitmap(normal);



}

void CxSkinButton::ChangeNormalImg(UINT chimg_NOR, UINT chimg_CLK, UINT chimg_OVR)
{
	m_bNormal.DeleteObject();
	m_bNormal.LoadBitmap(chimg_NOR);
	m_bOver.DeleteObject();
	m_bOver.LoadBitmap(chimg_OVR);
	m_bDown.DeleteObject();
	m_bDown.LoadBitmap(chimg_CLK);
	Invalidate(true);
}

bool CxSkinButton::GetTracking(void)
{
	return m_tracking;
}