// BitmapSlider.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinSlider.h"

#include "Define.h"
#include "SkinUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CxSkinSlider::CxSkinSlider()
	:m_nPos(0)
	,m_nMin(0)
	,m_nMax(100)
	,m_nPage(20)
	,m_nMarginLeft(0)
	,m_nMarginRight(0)
	,m_nMarginTop(0)
	,m_nMarginBottom(0)
	,m_nBkMarginLeft(0)
	,m_nBkMarginRight(0)
	,m_nBkMarginTop(0)
	,m_nBkMarginBottom(0)
	,m_nThumbWidth(0)
	,m_nThumbHeight(0)
	,m_bChannel(false)
	,m_bVertical(false)
	,m_bThumb(false)
	,m_bLButtonDown(false)
	,m_bFocusRect(false)
	,m_bFocus(false)
	,m_bDrawFocusRect(true)
	,m_bEnable(true)
	,m_nThumbBgX(-1)
	,m_nThumbBgY(-1)
	,m_bSetBGBitmap(false)
	,m_nId(0)
	,m_bThumbLock(true)
	,m_bTransparentChannel(false)
{
}

CxSkinSlider::~CxSkinSlider()
{
	if(m_bmpBk.m_hObject)
		m_bmpBk.DeleteObject();

	if(m_bmpLeft.m_hObject)
		m_bmpLeft.DeleteObject();

	if(m_bmpCenterL.m_hObject)
		m_bmpCenterL.DeleteObject();

	if(m_bmpCenterR.m_hObject)
		m_bmpCenterR.DeleteObject();
	
	if(m_bmpRight.m_hObject)
		m_bmpRight.DeleteObject();

	if(m_bmpLeftU.m_hObject)
		m_bmpLeftU.DeleteObject();

	if(m_bmpRightU.m_hObject)
		m_bmpRightU.DeleteObject();
}


BEGIN_MESSAGE_MAP(CxSkinSlider, CStatic)
	//{{AFX_MSG_MAP(CxSkinSlider)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CxSkinSlider message handlers

BOOL CxSkinSlider::OnEraseBkgnd(CDC* pDC) 
{	
	if(m_bSetBGBitmap)
	{
		if(m_bVertical)
		{
			CRect clientRect;
			GetClientRect(&clientRect);

			CDC memDC;
			CDC bitmapDC;
			memDC.CreateCompatibleDC(pDC);
			bitmapDC.CreateCompatibleDC(pDC);

			int bottom	= clientRect.bottom;
			int top		= clientRect.top;
			int left	= clientRect.left;
			int right	= clientRect.right;
			int height	= bottom - top;
			int width	= right - left;

			CBitmap bmpBuffer;
			bmpBuffer.CreateCompatibleBitmap(pDC, width, height);

			CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

			CBitmap* poldtmpDCbitmap = bitmapDC.SelectObject(&m_bmpBk);
			memDC.BitBlt(0,0,m_bmBk.bmWidth,m_bmBk.bmHeight,&bitmapDC,0,0,SRCCOPY);

			int nYPos = Pos2Pixel(m_nPos);
			//CBitmap* poldtmpDCbitmap;
			//Top을 그림
			if(m_nPos>this->m_nMin){
				if(m_bmpRightU.m_hObject)
				{
					poldtmpDCbitmap = bitmapDC.SelectObject(&m_bmpRightU);
					memDC.BitBlt(0+m_nBkMarginLeft, 0+m_nBkMarginTop, m_bmRightU.bmWidth, m_bmRightU.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}else{
				if(m_bmpRight.m_hObject)
				{
					poldtmpDCbitmap = bitmapDC.SelectObject(&m_bmpRight);
					memDC.BitBlt(0+m_nBkMarginLeft, 0+m_nBkMarginTop, m_bmRight.bmWidth, m_bmRight.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}

			//Top부터 배경
			if(m_bmpCenterR.m_hObject)
			{
				bitmapDC.SelectObject(&m_bmpCenterR);
				int nHeightR = nYPos;

				float fs = (nHeightR % m_bmCenterR.bmHeight);
				int cal = 0;
				if(fs > 0) cal = 1;
				nHeightR = (nHeightR / m_bmCenterR.bmHeight)+cal;
				int rHeight = m_bmRight.bmHeight+m_nBkMarginTop;
				for(int i=0; i<nHeightR; i++){
					memDC.BitBlt(0+m_nBkMarginLeft, rHeight + (i*m_bmCenterR.bmHeight),
						m_bmCenterR.bmWidth,m_bmCenterR.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
				if(nYPos <= 0)
					nYPos = m_bmRight.bmHeight;
			}
			//Pos 이후부터 색칠
			if(m_bmpCenterL.m_hObject)
			{
				bitmapDC.SelectObject(&m_bmpCenterL);

				int nLHeight = (height - nYPos) - m_bmLeft.bmHeight;

				float fs = (nLHeight % m_bmCenterL.bmHeight);
				int cal = 0;
				if(fs > 0) cal = 1;
				nLHeight = (nLHeight / m_bmCenterL.bmHeight)+cal;
				for(int i=0; i< nLHeight; i++){
					memDC.BitBlt(0+m_nBkMarginLeft, nYPos + (i*m_bmCenterL.bmHeight),
						m_bmCenterL.bmWidth,m_bmCenterL.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}
			//Bottom그림
			if(m_nPos>this->m_nMax){
				if(m_bmpLeft.m_hObject)
				{
					bitmapDC.SelectObject(&m_bmpLeft);
					memDC.BitBlt(0+m_nBkMarginLeft,bottom - m_bmLeft.bmHeight-m_nBkMarginBottom, m_bmLeft.bmWidth, m_bmLeft.bmHeight, &bitmapDC,0,0,SRCCOPY);
				}
			}else{
				if(m_bmpLeftU.m_hObject)
				{
					bitmapDC.SelectObject(&m_bmpLeftU);
					memDC.BitBlt(0+m_nBkMarginLeft,bottom - m_bmLeftU.bmHeight-m_nBkMarginBottom, m_bmLeftU.bmWidth, m_bmLeftU.bmHeight, &bitmapDC,0,0,SRCCOPY);
				}
			}
			
			CRgn rgn;
			CRect rect = GetThumbClipBox();

			m_nThumbBgY = Pos2Pixel(m_nPos) - m_nThumbHeight/2;
			m_nThumbBgX = m_nMarginLeft;
			CopyBackground(&memDC, m_nThumbBgX, m_nThumbBgY, m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );

			rgn.CreateRectRgn(0,0,0,0);
			rgn.SetRectRgn(&rect);
			pDC->SelectClipRgn(&rgn, RGN_DIFF);
			pDC->BitBlt(0, 0, width, height, &memDC, 0, 0, SRCCOPY);
			rgn.DeleteObject();


			bitmapDC.SelectObject(poldtmpDCbitmap);
			memDC.SelectObject(pOldBitmap);
			DeleteObject(bmpBuffer.m_hObject);
			memDC.DeleteDC();		
		}
		else
		{
			CRect clientRect;
			GetClientRect(&clientRect);

			CDC memDC;
			CDC bitmapDC;
			memDC.CreateCompatibleDC(pDC);
			bitmapDC.CreateCompatibleDC(pDC);

			int bottom	= clientRect.bottom;
			int top		= clientRect.top;
			int left	= clientRect.left;
			int right	= clientRect.right;
			int height	= bottom - top;
			int width	= right - left;

						CBitmap bmpBuffer;
			bmpBuffer.CreateCompatibleBitmap(pDC, width, height);			
			
			CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

			CBitmap* poldtmpDCbitmap = bitmapDC.SelectObject(&m_bmpBk);
			memDC.BitBlt(0,0,m_bmBk.bmWidth,m_bmBk.bmHeight,&bitmapDC,0,0,SRCCOPY);

			int nXPos = Pos2Pixel(m_nPos);

			top += m_nBkMarginTop;
			
			if(m_nPos>this->m_nMin){
				if(m_bmpLeft.m_hObject)
				{
					bitmapDC.SelectObject(&m_bmpLeft);
					memDC.BitBlt(0,top, m_bmLeft.bmWidth, m_bmLeft.bmHeight, &bitmapDC,0,0,SRCCOPY);		
				}
			}else{
				if(m_bmpLeftU.m_hObject)
				{
					bitmapDC.SelectObject(&m_bmpLeftU);
					memDC.BitBlt(0,top, m_bmLeftU.bmWidth, m_bmLeftU.bmHeight, &bitmapDC,0,0,SRCCOPY);
				}
			}

			if(m_bmpCenterL.m_hObject)
			{
				poldtmpDCbitmap = bitmapDC.SelectObject(&m_bmpCenterL);
				int nLWidth = nXPos - m_bmLeft.bmWidth;

				float fs = (nLWidth % m_bmCenterL.bmWidth);
				int cal = 0;
				if(fs > 0) cal = 1;
				nLWidth = (nLWidth / m_bmCenterL.bmWidth) +cal;
				for(int i=0; i<(nLWidth); i++){
					memDC.BitBlt( m_bmLeft.bmWidth + (i*m_bmCenterL.bmWidth),top,m_bmCenterL.bmWidth,m_bmCenterL.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}

			if(m_bmpCenterR.m_hObject)
			{
				bitmapDC.SelectObject(&m_bmpCenterR);
				int nWidthR = clientRect.Width() - nXPos;

				float fs = (nWidthR % m_bmCenterR.bmWidth);
				int cal = 0;
				if(fs > 0) cal = 1;
				nWidthR = (nWidthR / m_bmCenterR.bmWidth) +cal;
				for(int i=0; i<nWidthR; i++){
					memDC.BitBlt( nXPos + (i*m_bmCenterR.bmWidth),top,m_bmCenterR.bmWidth,m_bmCenterR.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}


			if(m_nPos<this->m_nMax){
				if(m_bmpRightU.m_hObject)
				{
					bitmapDC.SelectObject(&m_bmpRightU);
					memDC.BitBlt(clientRect.right - m_bmRightU.bmWidth,top,m_bmRightU.bmWidth,m_bmRightU.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}else{
				if(m_bmpRight.m_hObject)
				{
					bitmapDC.SelectObject(&m_bmpRight);
					memDC.BitBlt(clientRect.right - m_bmRight.bmWidth,top,m_bmRight.bmWidth,m_bmRight.bmHeight,&bitmapDC,0,0,SRCCOPY);
				}
			}

			CRgn rgn;
			CRect rect = GetThumbClipBox();

			m_nThumbBgX = Pos2Pixel(m_nPos) - m_nThumbWidth/2;
			m_nThumbBgY = m_nMarginTop;
			CopyBackground(&memDC, m_nThumbBgX, m_nThumbBgY, m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );

			rgn.CreateRectRgn(0,0,0,0);
			rgn.SetRectRgn(&rect);
			pDC->SelectClipRgn(&rgn, RGN_DIFF);
			pDC->BitBlt(0, 0, width, height, &memDC, 0, 0, SRCCOPY);
			rgn.DeleteObject();

			bitmapDC.SelectObject(poldtmpDCbitmap);
			memDC.SelectObject(pOldBitmap);
			DeleteObject(bmpBuffer.m_hObject);
		}
		return TRUE;

	}else{
		CRect clientRect;
		GetClientRect(&clientRect);

		CDC memDC;
		CDC bitmapDC;
		memDC.CreateCompatibleDC(pDC);
		bitmapDC.CreateCompatibleDC(pDC);

		int bottom	= clientRect.bottom;
		int top		= clientRect.top;
		int left	= clientRect.left;
		int right	= clientRect.right;
		int height	= bottom - top;
		int width	= right - left;

		CBitmap bmpBuffer;
		bmpBuffer.CreateCompatibleBitmap(pDC, width, height);			

		CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

		int nXPos = Pos2Pixel(m_nPos);

		CBitmap* poldtmpDCbitmap = bitmapDC.SelectObject(&m_bmpBk);
		memDC.BitBlt(left,top,m_bmBk.bmWidth,m_bmBk.bmHeight,&bitmapDC,0,0,SRCCOPY);

		CRgn rgn;
		CRect rect = GetThumbClipBox();

		m_nThumbBgX = Pos2Pixel(m_nPos) - m_nThumbWidth/2;
		m_nThumbBgY = m_nMarginTop;
		CopyBackground(&memDC, m_nThumbBgX, m_nThumbBgY, m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );

		rgn.CreateRectRgn(0,0,0,0);
		rgn.SetRectRgn(&rect);
		pDC->SelectClipRgn(&rgn, RGN_DIFF);
		pDC->BitBlt(0, 0, width, height, &memDC, 0, 0, SRCCOPY);
		rgn.DeleteObject();

		bitmapDC.SelectObject(poldtmpDCbitmap);
		memDC.SelectObject(pOldBitmap);
		DeleteObject(bmpBuffer.m_hObject);

		return TRUE;
	}
}

// 컨트롤을 그린다
//
void CxSkinSlider::OnPaint() 
{
	CPaintDC dc(this);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap *pbmTmp;
	if( m_bThumb && m_bEnable ) {
		if( m_bThumbActive && m_bLButtonDown )
			pbmTmp = dcMem.SelectObject( &m_bmThumbActive ); // 활성화
		else
			pbmTmp = dcMem.SelectObject( &m_bmThumb ); // 보통

		if(m_bVertical){
			if( m_bTransparentChannel || !m_bChannel ) {
				m_nThumbBgX = m_nMarginLeft;
				m_nThumbBgY = Pos2Pixel(m_nPos) - m_nThumbHeight/2;
			}

			CRgn rgn;
			CRect rect = GetThumbClipBox();
			rgn.CreateRectRgn(0,0,0,0);
			rgn.SetRectRgn(&rect);
			dc.SelectClipRgn(&rgn, RGN_OR);
			//DrawBitmap(	&dc, m_nMarginLeft, Pos2Pixel(m_nPos) - m_nThumbHeight/2, m_nThumbWidth, m_nThumbHeight, &dcMem, 0, 0, &m_bmThumbMask2, m_bTransparentThumb );  //__thumb mask 변경...
			DrawBitmap(	&dc, m_nMarginLeft, Pos2Pixel(m_nPos) - m_nThumbHeight/2, m_nThumbWidth, m_nThumbHeight, &dcMem, 0, 0, &m_bmThumbMask, m_bTransparentThumb );  //__thumb mask 변경...
			rgn.DeleteObject();
			dcMem.SelectObject( pbmTmp );
		}else{
			//if( m_bTransparentChannel || !m_bChannel ) {
			//m_nThumbBgX = Pos2Pixel(m_nPos) - m_nThumbWidth/2;
			//m_nThumbBgY = m_nMarginTop;
			//CopyBackground(&dc, m_nThumbBgX, m_nThumbBgY, m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );
			//}
			CRgn rgn;
			CRect rect = GetThumbClipBox();
			rgn.CreateRectRgn(0,0,0,0);
			rgn.SetRectRgn(&rect);
			dc.SelectClipRgn(&rgn, RGN_OR);
			//DrawBitmap(	&dc, Pos2Pixel(m_nPos) - m_nThumbWidth/2, m_nMarginTop,	m_nThumbWidth, m_nThumbHeight, &dcMem, 0, 0, &m_bmThumbMask2, m_bTransparentThumb );  //__thumb mask 변경...
			DrawBitmap(	&dc, Pos2Pixel(m_nPos) - m_nThumbWidth/2, m_nMarginTop,	m_nThumbWidth, m_nThumbHeight, &dcMem, 0, 0, &m_bmThumbMask, m_bTransparentThumb );  //__thumb mask 변경...
			rgn.DeleteObject();
			dcMem.SelectObject( pbmTmp );
		}
	} 
	dcMem.DeleteDC();

}

// 슬라이더의 최대값을 설정합니다.
//
// 매개변수:
//		[IN]	nMax
//				최대값.
//		[IN]	bRedraw
//				TRUE이면 컨트롤을 다시 그리고
//				FALSE이면 최대값만 변경합니다.
//
void CxSkinSlider::SetRangeMax(int nMax, BOOL bRedraw)
{
	m_nMax = nMax;
	if( bRedraw )
		Invalidate();
}

// 슬라이더의 최소값을 설정합니다.
//
// 매개변수:
//		[IN]	nMin
//				최소값.
//		[IN]	bRedraw
//				TRUE이면 컨트롤을 다시 그리고
//				FALSE이면 최대값만 변경합니다.
//
void CxSkinSlider::SetRangeMin(int nMin, BOOL bRedraw)
{
	m_nMin = nMin;
	if( bRedraw )
		Invalidate();
}

// 슬라이더의 범위를 설정합니다.
//
// 매개변수:
//		[IN]	nMin
//				최소값.
//		[IN]	nMax
//				최대값.
//		[IN]	bRedraw
//				TRUE이면 컨트롤을 다시 그리고
//				FALSE이면 최대값만 변경합니다.
//
void CxSkinSlider::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	SetRangeMin( nMin, FALSE );
	SetRangeMax( nMax, bRedraw );
}

// 슬라이더의 위치 값을 설정합니다.
//
// 매개변수:
//		[IN]	nPos
//				새로운 위치 값.
//
void CxSkinSlider::SetPos(int nPos)
{
	//TRACE(L"SetPos:%d\n", nPos);
	if(m_nPos == nPos) return;
	m_nPos = nPos;

	// 범위를 검사한다.
	if( m_nPos > m_nMax )
		m_nPos = m_nMax;
	if( m_nPos < m_nMin )
		m_nPos = m_nMin;

	Invalidate(true);
	/*	CRect RcClient;
	GetClientRect(&RcClient);
	int nX = Pos2Pixel(m_nPos);
	InvalidateRect(CRect(nX-20, 0, nX+20, RcClient.bottom), TRUE);
	*/

}

// 슬라이더의 페이지 값을 설정합니다.
//
// 매개변수:
//		[IN]	nSize
//				새로운 페이지 값..
//
// 리턴값:
//		이전의 페이지 값.
//
int CxSkinSlider::SetPageSize(int nSize)
{
	int nRet = m_nPage;

	m_nPage = nSize;

	return nRet;
}

void CxSkinSlider::SetBkMargin(int nLeft, int nTop, int nRight, int nBottom)
{
	SetBkMarginLeft( nLeft );
	SetBkMarginTop( nTop );
	SetBkMarginRight( nRight );
	SetBkMarginBottom( nBottom );
}

// 컨트롤의 여백을 설정합니다. 손잡이는 여백 안쪽에서 움직입니다.
//
void CxSkinSlider::SetMargin(int nLeft, int nTop, int nRight, int nBottom )
{
	SetMarginLeft( nLeft );
	SetMarginTop( nTop );
	SetMarginRight( nRight );
	SetMarginBottom( nBottom );
}

// 슬라이더의 동작을 억제할 수 있습니다.
//
//		[IN]	bEnable
//				TRUE는 슬라이더를 동작하게 하고,
//				FALSE는 동작을 억제합니다.
//
void CxSkinSlider::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;

	// 드래그하는 중일 경우
	if( !m_bEnable && m_bLButtonDown ) {

		m_bLButtonDown = FALSE;
		ReleaseCapture();
	}

	Invalidate();
}

// 컨트롤이 포커스를 얻었을 때, 이를 표시할 것인지 여부를 설정합니다.
//
//		[IN]	bDraw
//				TRUE는 표시를,
//				FALSE는 표시하지 않음을 의미합니다.
//
//		[IN]	bRedraw
//				TRUE값은 컨트롤을 다시 그리고,
//				FALSE는 상태값만을 바꿉니다.
//
void CxSkinSlider::DrawFocusRect(BOOL bDraw, BOOL bRedraw)
{
	m_bDrawFocusRect = bDraw;

	if( bRedraw )
		Invalidate();
}

// 배경에 사용할 비트맵을 불러옵니다.
//
// 매개변수:
//		[IN]	nChannelID
//				배경 비트맵의 ID 값.
//		[IN]	nActiveID
//				활성화된 배경 비트맵의 ID 값.
//		[IN]	bTransparent
//				투명 효과를 적용하려면 TRUE, 그렇지 않으면 FALSE입니다.
//		[IN]	clrpTransColor
//				투명 효과를 적용할 색상의 RGB 값.
//		[IN]	iTransPixelX
//				투명한 효과를 줄 색상이 위치한 포인트의 x좌표 값.
//		[IN]	iTransPixelY
//				투명한 효과를 줄 색상이 위치한 포인트의 y좌표 값.
//
// 리턴값:
//		성공했으면 TRUE 를 그렇지 않으면FALSE를 리턴합니다
//
BOOL CxSkinSlider::SetBitmapChannel(
	UINT nChannelID, UINT nActiveID , BOOL bTransparent,
	COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY )
{
	// 비트맵의 ID가 정해지지 않았을 때
	if( !nChannelID ) {

		m_bChannel = FALSE;

		m_bmChannel.DeleteObject();
		m_bmChannelMask.DeleteObject();
		m_bmChannelActive.DeleteObject();
		m_bmChannelActiveMask.DeleteObject();

		return TRUE;
	}

	// 비트맵을 불러온다
	m_bmChannel.DeleteObject();

	if( !m_bmChannel.LoadBitmap( nChannelID ) )
		return FALSE;

	// 투명한 비트맵을 그리기 위해 마스크를 만든다.
	if( bTransparent ) {

		PrepareMask( &m_bmChannel, &m_bmChannelMask,
			clrpTransColor, iTransPixelX, iTransPixelY );
	}

	// 활성화 배경 비트맵을 불러온다.
	if( nActiveID ) {

		m_bmChannelActive.DeleteObject();

		if( !m_bmChannelActive.LoadBitmap( nActiveID ) ) {

			m_bmChannel.DeleteObject();
			if( bTransparent )
				m_bmChannelMask.DeleteObject();

			return FALSE;
		}

		if( bTransparent ) {

			PrepareMask( &m_bmChannelActive, &m_bmChannelActiveMask,
				clrpTransColor, iTransPixelX, iTransPixelY );
		}

		m_bChannelActive = TRUE;

		// 활성화 상태의 비트맵이 존재하지 않을 때.
	} else
		m_bChannelActive = FALSE;

	// 비트맵의 크기 정보를 가져온다.
	BITMAP	bitmap;
	m_bmChannel.GetBitmap( &bitmap );

	m_nWidth = bitmap.bmWidth;
	m_nHeight = bitmap.bmHeight;

	// 크기를 비교한다
	if( m_bChannelActive ) {
		m_bmChannelActive.GetBitmap( &bitmap );

		ASSERT( m_nWidth == bitmap.bmWidth && m_nHeight == bitmap.bmHeight );
	}

	// 컨트롤의 크기를 비트맵의 크기로 바꾼다.
	SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOZORDER | SWP_NOMOVE);

	GetClientRect( &m_rect );

	m_bTransparentChannel = bTransparent;
	m_bChannel = TRUE;

	return TRUE;
}

// 손잡이에 사용할 비트맵을 불러옵니다.
//
// 매개변수:
//		[IN]	nThumbID
//				손잡이 비트맵의 ID.
//		[IN]	nActiveID
//				활성화된 손잡이 비트맵의 ID.
//		[IN]	bTransparent
//				투명 효과를 적용하려면 TRUE, 그렇지 않으면 FALSE입니다.
//		[IN]	clrpTransColor
//				투명 효과를 적용할 색상의 RGB 값.
//		[IN]	iTransPixelX
//				투명한 효과를 줄 색상이 위치한 포인트의 x좌표 값.
//		[IN]	iTransPixelY
//				투명한 효과를 줄 색상이 위치한 포인트의 y좌표 값.
//
// 리턴값:
//		성공했으면 TRUE 를 그렇지 않으면FALSE를 리턴합니다
//
BOOL CxSkinSlider::SetBitmapThumb(
	UINT nThumbID, UINT nActiveID, BOOL bTransparent,
	COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY )
{
	// 손잡이의 ID값이 NULL일때
	if( !nThumbID ) {

		m_bThumb = FALSE;

		m_bmThumb.DeleteObject();
		m_bmThumbMask.DeleteObject();
		m_bmThumbActive.DeleteObject();
		m_bmThumbActiveMask.DeleteObject();
		m_bmThumbBg.DeleteObject();

		return TRUE;
	}

	// 비트맵을 불러온다
	m_bmThumb.DeleteObject();

	if( !m_bmThumb.LoadBitmap( nThumbID ) )
		return FALSE;

	// 투명한 비트맵을 그리기 위해 마스크를 만든다.
	if( bTransparent ) {

		PrepareMask( &m_bmThumb, &m_bmThumbMask,
			clrpTransColor, iTransPixelX, iTransPixelY );
	}

	// 활성화 상태를 위한 비트맵을 불러온다.
	if( nActiveID ) {

		m_bmThumbActive.DeleteObject();

		if( !m_bmThumbActive.LoadBitmap( nActiveID ) ) {

			m_bmThumb.DeleteObject();
			if( bTransparent )
				m_bmThumbMask.DeleteObject();

			return FALSE;
		}

		if( bTransparent ) {

			PrepareMask( &m_bmThumbActive, &m_bmThumbActiveMask,
				clrpTransColor, iTransPixelX, iTransPixelY );
		}

		m_bThumbActive = TRUE;

		// 활성화 상태의 비트맵이 존재하지 않을 때.
	} else
		m_bThumbActive = FALSE;

	// 비트맵의 크기 정보를 가져온다.
	BITMAP	bitmap;
	m_bmThumb.GetBitmap( &bitmap );

	m_nThumbWidth = bitmap.bmWidth;
	m_nThumbHeight = bitmap.bmHeight;

	//	m_nThumbWidth = 17;
	//	m_nThumbHeight = 13;


	// 배경을 위한 비트맵이 없으면 컨트롤의 사이즈를 기억한다.
	if( !m_bChannel ) {

		GetClientRect( &m_rect );
		m_nHeight = m_rect.Height();
		m_nWidth = m_rect.Width();
	}

	ASSERT( m_nThumbWidth <= m_nWidth && m_nThumbHeight <= m_nHeight );

	// 크기를 비교한다
	if( m_bThumbActive ) {
		m_bmThumbActive.GetBitmap( &bitmap );

		ASSERT(
			m_nThumbWidth == bitmap.bmWidth &&
			m_nThumbHeight == bitmap.bmHeight );
	}

	// 속성값을 설정한다
	m_bTransparentThumb = bTransparent;
	if(m_nThumbWidth>0 &&m_nThumbHeight>0)
		m_bThumb = TRUE;

	return TRUE;
}

BOOL CxSkinSlider::CopyFromThumb(CBitmap& bmSkinImage, CRect Rect_Thumb, CRect Rect_Active, BOOL bTransparent,
	COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY )
{

	//clrpTransColor = COLOR_WHITE;
	if (!bmSkinImage.m_hObject){
		m_bThumb = FALSE;

		m_bmThumb.DeleteObject();
		m_bmThumbMask.DeleteObject();
		m_bmThumbActive.DeleteObject();
		m_bmThumbActiveMask.DeleteObject();
		m_bmThumbBg.DeleteObject();

		return TRUE;
	}


	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	m_bmThumb.DeleteObject();
	skinUtil.CopyBitmap(pDC, m_bmThumb, bmSkinImage, Rect_Thumb);


	// 투명한 비트맵을 그리기 위해 마스크를 만든다. //__처리요망..
	if( bTransparent ) {
		PrepareMask( &m_bmThumb, &m_bmThumbMask,
			clrpTransColor, iTransPixelX, iTransPixelY );
	}

	// 활성화 상태를 위한 비트맵을 불러온다.
	if (Rect_Active.Width()  <= 0){
		m_bThumbActive = FALSE;
	}else{
		m_bmThumbActive.DeleteObject();
		skinUtil.CopyBitmap(pDC, m_bmThumbActive, bmSkinImage, Rect_Active);

		if( bTransparent ) {
			PrepareMask( &m_bmThumbActive, &m_bmThumbActiveMask,
				clrpTransColor, iTransPixelX, iTransPixelY );
		}

		m_bThumbActive = TRUE;
	}

	ReleaseDC(pDC);	

	// 비트맵의 크기 정보를 가져온다.
	BITMAP	bitmap;
	m_bmThumb.GetBitmap( &bitmap );

	m_nThumbWidth = bitmap.bmWidth;
	m_nThumbHeight = bitmap.bmHeight;


	// 배경을 위한 비트맵이 없으면 컨트롤의 사이즈를 기억한다.
	if( !m_bChannel ) {
		GetClientRect( &m_rect );
		m_nHeight = m_rect.Height();
		m_nWidth = m_rect.Width();
	}

	//Thumb H가 Slider H보다 작지 않은 경우가 있음
	//ASSERT( m_nThumbWidth <= m_nWidth && m_nThumbHeight <= m_nHeight );

	// 크기를 비교한다
	if( m_bThumbActive ) {
		m_bmThumbActive.GetBitmap( &bitmap );

		ASSERT(
			m_nThumbWidth == bitmap.bmWidth &&
			m_nThumbHeight == bitmap.bmHeight );
	}

	// 속성값을 설정한다
	m_bTransparentThumb = bTransparent;
	if(m_nThumbWidth>1 || m_nThumbHeight>1)
		m_bThumb = TRUE;

	return TRUE;
}

// OnLButtonDown
//
// 드래그가 시작될 때
//
void CxSkinSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !m_bEnable )
		return;

	if(m_bThumbLock)
		return;

	SetCapture();
	SetFocus();

	m_bLButtonDown = TRUE;

	// 손잡이를 마우스로 클릭했을 때
	// 마우스의 좌표와 손잡이 중심의 좌표를 비교하여
	// 둘 사이의 거리를 구해둔다
	if( m_bVertical ) {

		if( abs( point.y - Pos2Pixel( m_nPos ) ) <= m_nThumbHeight / 2 )
			m_nMouseOffset = point.y - Pos2Pixel( m_nPos );
		else
			m_nMouseOffset = 0;

	} else {

		if( abs( point.x - Pos2Pixel( m_nPos ) ) <= m_nThumbWidth / 2 )
			m_nMouseOffset = point.x - Pos2Pixel( m_nPos );
		else
			m_nMouseOffset = 0;
	}

	OnMouseMove( nFlags, point );
	Invalidate();

	CStatic::OnLButtonDown(nFlags, point);
}

// OnMouseMove
//
// 드래그 중일 때
//
void CxSkinSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( !m_bLButtonDown || !m_bEnable )
		return;

	int nPixel;

	// 범위를 벗어났는지 검사한다
	if( m_bVertical ) {

		nPixel = point.y - m_nMouseOffset;

		if( nPixel > m_nHeight - m_nMarginBottom - m_nThumbHeight/2 )
			nPixel = m_nHeight - m_nMarginBottom - m_nThumbHeight/2;

		if( nPixel < m_nMarginTop + m_nThumbHeight/2 )
			nPixel = m_nMarginTop + m_nThumbHeight/2;

	} else {

		nPixel = point.x - m_nMouseOffset;

		if( nPixel < m_nMarginLeft + m_nThumbWidth/2 )
			nPixel = m_nMarginLeft + m_nThumbWidth/2;

		if( nPixel > m_nWidth - m_nMarginRight - m_nThumbWidth/2 )
			nPixel = m_nWidth - m_nMarginRight - m_nThumbWidth/2;
	}

	// 변한 내용을 적용한다
	if( Pos2Pixel(m_nPos) != nPixel ) 
	{
		SetPos( Pixel2Pos( nPixel ) );
		::PostMessage(
			GetParent()->GetSafeHwnd(), 
			WM_SLIDER_MOVING,
			GetDlgCtrlID(), m_nPos );
	}

	::SetCursor(LoadCursor(NULL, IDC_HAND));

	CStatic::OnMouseMove(nFlags, point);
}

// OnLButtonUp
//
// 드래그가 끝났을 때
//
void CxSkinSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !m_bEnable )
		return;

	if(m_bThumbLock) return;

	ReleaseCapture();
	m_bLButtonDown = FALSE;

	Invalidate();

	::PostMessage(
		GetParent()->GetSafeHwnd(),	
		WM_SLIDER_MOVED,
		GetDlgCtrlID(), m_nPos );

	CStatic::OnLButtonUp(nFlags, point);
}

// 컨트롤의 위치값에 해당하는 손잡이 중심의 좌표값을 구한다
//
int CxSkinSlider::Pos2Pixel(int nPos)
{
	if( m_bVertical ) {

		return
			m_nMarginTop + m_nThumbHeight/2 +
			(int)(
			( m_nHeight - m_nMarginTop - m_nMarginBottom - m_nThumbHeight ) *
			((double) ( nPos - m_nMin ) / ( m_nMax - m_nMin ) )
			);

	} else {

		return (int)(
			( m_nWidth - m_nMarginLeft - m_nMarginRight - m_nThumbWidth ) *
			((double) ( nPos - m_nMin ) / ( m_nMax - m_nMin ) )
			) + m_nMarginLeft + m_nThumbWidth/2;
	}
}

// 마우스의 좌표값에 해당하는 컨트롤의 위치값을 구한다
//
int CxSkinSlider::Pixel2Pos(int nPixel)
{
	if( m_bVertical ) {

		return (int)(
			m_nMin +
			(double)( nPixel - m_nMarginTop - m_nThumbHeight/2) /
			( m_nHeight - m_nMarginBottom - m_nMarginTop - m_nThumbHeight ) *
			( m_nMax - m_nMin )
			);

	} else {

		return (int)(
			m_nMin +
			(double)( nPixel - m_nMarginLeft - m_nThumbWidth/2 ) /
			( m_nWidth - m_nMarginLeft - m_nMarginRight - m_nThumbWidth ) *
			( m_nMax - m_nMin )
			);
	}
}

// 지정한 영역의 비트맵 정보를 가져온다
//
void CxSkinSlider::CopyBackground(
	CDC *pDC, int nXSrc, int nYSrc, int nWidth, int nHeight, CBitmap *pBmDst)
{
	pBmDst->DeleteObject();
	pBmDst->CreateCompatibleBitmap( pDC, nWidth, nHeight );

	CDC memDC;

	memDC.CreateCompatibleDC( pDC );
	CBitmap *pBmTmp = memDC.SelectObject( pBmDst );

	memDC.BitBlt( 0, 0, nWidth, nHeight, pDC, nXSrc, nYSrc, SRCCOPY );

	memDC.SelectObject( pBmTmp );
	memDC.DeleteDC();
}

// 비트맵을 지정한 영역에 그린다
//
void CxSkinSlider::RestoreBackground(
	CDC *pDC, int nXDst, int nYDst, int nWidth, int nHeight, CBitmap *pBmSrc)
{
	CDC memDC;

	memDC.CreateCompatibleDC( pDC );
	CBitmap *pBmTmp = memDC.SelectObject( pBmSrc );

	pDC->BitBlt( nXDst, nYDst, nWidth, nHeight, &memDC, 0, 0, SRCCOPY );

	memDC.SelectObject( pBmTmp );
	memDC.DeleteDC();
}

// DrawBitmap
//
// OnPaint의 코드를 읽기 쉽게 하기위한 함수
//
void CxSkinSlider::DrawBitmap(
	CDC *pDC, int xStart, int yStart, int wWidth, int wHeight,
	CDC *pTmpDC, int xSource, int ySource, CBitmap *bmMask, BOOL bTransparent )
{
	if( bTransparent ) {

		DrawTransparentBitmap(
			pDC, xStart, yStart,
			wWidth, wHeight,
			pTmpDC, xSource, ySource, bmMask );

	} else {

		pDC->BitBlt( xStart, yStart,
			wWidth, wHeight,
			pTmpDC, xSource, ySource, SRCCOPY );
	}
}

// PrepareMask
//
// "Drawing Transparent Bitmap with ease with on the fly masks in MFC"
// By Raja Segar
//
// clrpTransColor의 기본값을 NULL(black)에서
// 0xFF000000(RGB 값이 아닌 값)으로 바꿨습니다.
//
void CxSkinSlider::PrepareMask(
	CBitmap *pBmpSource, CBitmap *pBmpMask,
	COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY)
{
	BITMAP bm;

	// Get the dimensions of the source bitmap
	pBmpSource->GetObject(sizeof(BITMAP), &bm);

	// Create the mask bitmap
	pBmpMask->DeleteObject();
	pBmpMask->CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// We will need two DCs to work with. One to hold the Image
	// (the source), and one to hold the mask (destination).
	// When blitting onto a monochrome bitmap from a color, pixels
	// in the source color bitmap that are equal to the background
	// color are blitted as white. All the remaining pixels are
	// blitted as black.

	CDC hdcSrc, hdcDst;

	hdcSrc.CreateCompatibleDC(NULL);
	hdcDst.CreateCompatibleDC(NULL);

	// Load the bitmaps into memory DC
	CBitmap* hbmSrcT = (CBitmap*) hdcSrc.SelectObject(pBmpSource);
	CBitmap* hbmDstT = (CBitmap*) hdcDst.SelectObject(pBmpMask);

	// Dynamically get the transparent color
	COLORREF clrTrans;
	if (clrpTransColor == 0xFF000000)
	{
		// User did not specify trans color so get it from bmp
		clrTrans = hdcSrc.GetPixel(iTransPixelX, iTransPixelY);
	}
	else
	{
		clrTrans = clrpTransColor;
	}


	// Change the background to trans color
	COLORREF clrSaveBk  = hdcSrc.SetBkColor(clrTrans);

	// This call sets up the mask bitmap.
	hdcDst.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &hdcSrc,0,0,SRCCOPY);

	// Now, we need to paint onto the original image, making
	// sure that the "transparent" area is set to black. What
	// we do is AND the monochrome image onto the color Image
	// first. When blitting from mono to color, the monochrome
	// pixel is first transformed as follows:
	// if  1 (black) it is mapped to the color set by SetTextColor().
	// if  0 (white) is is mapped to the color set by SetBkColor().
	// Only then is the raster operation performed.

	COLORREF clrSaveDstText = hdcSrc.SetTextColor(COLOR_WHITE);
	hdcSrc.SetBkColor(RGB(0,0,0));

	hdcSrc.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &hdcDst,0,0,SRCAND);

	// Clean up by deselecting any objects, and delete the
	// DC's.
	hdcDst.SetTextColor(clrSaveDstText);

	hdcSrc.SetBkColor(clrSaveBk);
	hdcSrc.SelectObject(hbmSrcT);
	hdcDst.SelectObject(hbmDstT);

	hdcSrc.DeleteDC();
	hdcDst.DeleteDC();
}

// DrawTransparentBitmap
//
// "Drawing Transparent Bitmap with ease with on the fly masks in MFC"
// By Raja Segar
//
void CxSkinSlider::DrawTransparentBitmap(
	CDC *pDC, int xStart, int yStart, int wWidth, int wHeight,
	CDC *pTmpDC, int xSource, int ySource, CBitmap *bmMask )
{
	// We are going to paint the two DDB's in sequence to the destination.
	// 1st the monochrome bitmap will be blitted using an AND operation to
	// cut a hole in the destination. The color image will then be ORed
	// with the destination, filling it into the hole, but leaving the
	// surrounding area untouched.

	//CDC memDC;
	//memDC.CreateCompatibleDC(pDC);

	//CDC hMaskDC;
	//hMaskDC.CreateCompatibleDC(NULL);
	//CBitmap* hbmT = hMaskDC.SelectObject(&m_bmThumbBg);

	//CBitmap bmpBuffer;
	//bmpBuffer.CreateCompatibleBitmap(pDC, wWidth, wHeight);

	//CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

	//memDC.BitBlt(0, 0, wWidth, wHeight, &hMaskDC, xSource,ySource, SRCCOPY);

	//hMaskDC.SelectObject(bmMask);

	//memDC.BitBlt(0, 0, wWidth, wHeight, pTmpDC,  0,0, SRCINVERT);
	//memDC.BitBlt(0, 0, wWidth, wHeight, &hMaskDC, 0,0, SRCAND);
	//memDC.BitBlt(0, 0, wWidth, wHeight, pTmpDC,  0,0, SRCINVERT);

	//pDC->BitBlt(xStart, yStart, wWidth, wHeight, &memDC, 0,0, SRCCOPY);

	//hMaskDC.SelectObject(hbmT);
	//hMaskDC.DeleteDC();

	//memDC.SelectObject(pOldBitmap);
	//DeleteDC(memDC);

	//DeleteObject(bmpBuffer.m_hObject);



	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);

	CBitmap* hbmT = hdcMem.SelectObject(&m_bmThumbBg);

	pDC->BitBlt( xStart, yStart, wWidth, wHeight, &hdcMem,
		xSource, ySource, SRCCOPY);


	hdcMem.SelectObject(bmMask);


	pDC->BitBlt( xStart, yStart, wWidth, wHeight, &hdcMem,
		xSource, ySource, SRCAND);

	// Also note the use of SRCPAINT rather than SRCCOPY.

	pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC,
		xSource, ySource,SRCPAINT);

	// Now, clean up.
	hdcMem.SelectObject(hbmT);
	hdcMem.DeleteDC();

}

// 키보드 입력을 받기 위해
//
UINT CxSkinSlider::OnGetDlgCode() 
{
	if( GetKeyState(VK_TAB) >= 0 ) {

		return  DLGC_WANTALLKEYS;
	}

	return CStatic::OnGetDlgCode();
}

// 키보드의 입력을 처리한다
//
void CxSkinSlider::OnKeyDown(UINT nTCHAR, UINT nRepCnt, UINT nFlags) 
{
	if( !m_bEnable )
		return;
	/*
	switch( nTCHAR ) {

	// 왼쪽, 위로
	case VK_LEFT :
	case VK_UP :

	SetPos( m_nPos-1 );
	break;

	// 오른쪽, 아래로
	case VK_RIGHT :
	case VK_DOWN :

	SetPos( m_nPos+1 );
	break;

	// Home
	case VK_HOME :

	SetPos( m_nMin );
	break;

	// End
	case VK_END :

	SetPos( m_nMax );
	break;

	// Page up
	case VK_PRIOR :

	SetPos( m_nPos - m_nPage );
	break;

	// Page down
	case VK_NEXT :

	SetPos( m_nPos + m_nPage );
	break;

	default :

	CStatic::OnKeyDown(nTCHAR, nRepCnt, nFlags);
	return;
	}

	::PostMessage(
	GetParent()->GetSafeHwnd(),	WM_BITMAPSLIDER_MOVED,
	GetDlgCtrlID(), m_nPos );
	*/
	CStatic::OnKeyDown(nTCHAR, nRepCnt, nFlags);
}

// 컨트롤이 포커스를 잃었을 때
//
void CxSkinSlider::OnKillFocus(CWnd* pNewWnd) 
{
	CStatic::OnKillFocus(pNewWnd);

	m_bFocus = FALSE;
	Invalidate();
}

// 컨트롤이 포커스를 얻었을 때
//
void CxSkinSlider::OnSetFocus(CWnd* pOldWnd) 
{
	CStatic::OnSetFocus(pOldWnd);

	m_bFocus = TRUE;
	Invalidate();
}

// 비트맵을 해제한다
//
void CxSkinSlider::OnDestroy() 
{
	CStatic::OnDestroy();

	m_bmThumb.DeleteObject();
	m_bmThumbMask.DeleteObject();

	m_bmThumbActive.DeleteObject();
	m_bmThumbActiveMask.DeleteObject();

	m_bmThumbBg.DeleteObject();

	m_bmChannel.DeleteObject();
	m_bmChannelMask.DeleteObject();

	m_bmChannelActive.DeleteObject();
	m_bmChannelActiveMask.DeleteObject();

	//m_bmThumbMask2.DeleteObject();
}

void CxSkinSlider::SetBGBitmap(UINT nIDLeft, UINT nIDCenterL, UINT nIDCenterR, UINT nIDRight)
{
	if(m_bmpLeft.m_hObject)
		m_bmpLeft.DeleteObject();

	if(m_bmpCenterL.m_hObject)
		m_bmpCenterL.DeleteObject();

	if(m_bmpCenterR.m_hObject)
		m_bmpCenterR.DeleteObject();


	if(m_bmpRight.m_hObject)
		m_bmpRight.DeleteObject();


	if(!m_bmpLeft.LoadBitmap(nIDLeft)) return;
	if(!m_bmpCenterL.LoadBitmap(nIDCenterL)) return;
	if(!m_bmpCenterR.LoadBitmap(nIDCenterR)) return;
	if(!m_bmpRight.LoadBitmap(nIDRight)) return;


	m_bmpLeft.GetBitmap(&m_bmLeft);
	m_bmpCenterL.GetBitmap(&m_bmCenterL);
	m_bmpCenterR.GetBitmap(&m_bmCenterR);
	m_bmpRight.GetBitmap(&m_bmRight);

	m_bSetBGBitmap = TRUE;
}
void CxSkinSlider::CopyFromBG(CBitmap& bmSkinImage, CRect Rect_Left, CRect Rect_Right, CRect Rect_CenterLeft, CRect Rect_CenterRight, CRect Rect_ThumbMask, CRect Rect_LeftU, CRect Rect_RightU)
{
	if(m_bmpLeft.m_hObject)
		m_bmpLeft.DeleteObject();

	if(m_bmpCenterL.m_hObject)
		m_bmpCenterL.DeleteObject();

	if(m_bmpCenterR.m_hObject)
		m_bmpCenterR.DeleteObject();

	if(m_bmpRight.m_hObject)
		m_bmpRight.DeleteObject();

	if(m_bmpRightU.m_hObject)
		m_bmpRightU.DeleteObject();

	if(m_bmpLeftU.m_hObject)
		m_bmpLeftU.DeleteObject();

	//if(m_bmThumbMask2.m_hObject)
	//	m_bmThumbMask2.DeleteObject();

	if(m_bmThumbMask.m_hObject)
		m_bmThumbMask.DeleteObject();


	CSize mSize;
	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmpLeft, bmSkinImage, Rect_Left);
	skinUtil.CopyBitmap(pDC, m_bmpRight, bmSkinImage, Rect_Right);

	skinUtil.CopyBitmap(pDC, m_bmpLeftU, bmSkinImage, Rect_LeftU);
	skinUtil.CopyBitmap(pDC, m_bmpRightU, bmSkinImage, Rect_RightU);

	skinUtil.CopyBitmap(pDC, m_bmpCenterL, bmSkinImage, Rect_CenterLeft);
	skinUtil.CopyBitmap(pDC, m_bmpCenterR, bmSkinImage, Rect_CenterRight);
	//skinUtil.CopyBitmap(pDC, m_bmThumbMask2, bmSkinImage, Rect_ThumbMask);
	skinUtil.CopyBitmap(pDC, m_bmThumbMask, bmSkinImage, Rect_ThumbMask);

	m_bmpCenterL.GetBitmap(&m_bmCenterL);
	m_bmpCenterR.GetBitmap(&m_bmCenterR);
	m_bmpRight.GetBitmap(&m_bmRight);
	m_bmpLeft.GetBitmap(&m_bmLeft);
	m_bmpRightU.GetBitmap(&m_bmRightU);
	m_bmpLeftU.GetBitmap(&m_bmLeftU);

	m_bSetBGBitmap = TRUE;
	ReleaseDC(pDC);
}

void CxSkinSlider::CopyFromBk(CBitmap& bmSkinImage, CRect Rect_Bk)
{
	if(m_bmpBk.m_hObject)
		m_bmpBk.DeleteObject();

	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmpBk, bmSkinImage, Rect_Bk);
	m_bmpBk.GetBitmap(&m_bmBk);
	ReleaseDC(pDC);
}

void CxSkinSlider::CopyFromBG(CBitmap& bmSkinImage, CRect Rect_Left, CRect Rect_Right, CRect Rect_CenterLeft, CRect Rect_CenterRight, CRect Rect_ThumbMask)
{
	if(m_bmpLeft.m_hObject)
		m_bmpLeft.DeleteObject();

	if(m_bmpCenterL.m_hObject)
		m_bmpCenterL.DeleteObject();

	if(m_bmpCenterR.m_hObject)
		m_bmpCenterR.DeleteObject();


	if(m_bmpRight.m_hObject)
		m_bmpRight.DeleteObject();

	//if(m_bmThumbMask2.m_hObject)
	//	m_bmThumbMask2.DeleteObject();

	if(m_bmThumbMask.m_hObject)
		m_bmThumbMask.DeleteObject();

	if(m_bmpRightU.m_hObject)
		m_bmpRightU.DeleteObject();

	if(m_bmpLeftU.m_hObject)
		m_bmpLeftU.DeleteObject();


	CSize mSize;
	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmpLeft, bmSkinImage, Rect_Left);
	skinUtil.CopyBitmap(pDC, m_bmpRight, bmSkinImage, Rect_Right);

	skinUtil.CopyBitmap(pDC, m_bmpLeftU, bmSkinImage, Rect_Left);
	skinUtil.CopyBitmap(pDC, m_bmpRightU, bmSkinImage, Rect_Right);

	skinUtil.CopyBitmap(pDC, m_bmpCenterL, bmSkinImage, Rect_CenterLeft);
	skinUtil.CopyBitmap(pDC, m_bmpCenterR, bmSkinImage, Rect_CenterRight);

	//skinUtil.CopyBitmap(pDC, m_bmThumbMask2, bmSkinImage, Rect_ThumbMask);
	skinUtil.CopyBitmap(pDC, m_bmThumbMask, bmSkinImage, Rect_ThumbMask);
	

	m_bmpCenterL.GetBitmap(&m_bmCenterL);
	m_bmpCenterR.GetBitmap(&m_bmCenterR);
	m_bmpLeft.GetBitmap(&m_bmLeft);
	m_bmpRight.GetBitmap(&m_bmRight);
	m_bmpRightU.GetBitmap(&m_bmRightU);
	m_bmpLeftU.GetBitmap(&m_bmLeftU);

	m_bSetBGBitmap = TRUE;
	ReleaseDC(pDC);
}


void CxSkinSlider::OnSize(UINT nType, int cx, int cy)
{
	UpdatePos();

	CStatic::OnSize(nType, cx, cy);
}

void CxSkinSlider::UpdatePos()
{
	GetClientRect( &m_rect );
	m_nHeight = m_rect.Height();
	m_nWidth = m_rect.Width();
}



void CxSkinSlider::ChangeSkinImage(UINT nChannelID, UINT nActiveID, UINT nMASKID)
{

	if(nChannelID != NULL){
		m_bmThumb.DeleteObject();
		m_bmThumb.LoadBitmap( nChannelID );
	}

	if(nActiveID != NULL){
		m_bmThumbActive.DeleteObject();
		m_bmThumbActive.LoadBitmap( nActiveID );
	}

	//if(nMASKID != NULL){
	//	m_bmThumbMask2.DeleteObject();
	//	m_bmThumbMask2.LoadBitmap( nMASKID );
	//}

	if(nMASKID != NULL){
		m_bmThumbMask.DeleteObject();
		m_bmThumbMask.LoadBitmap( nMASKID );
	}


}



void CxSkinSlider::SetBitmapThumbMask(UINT nThumbMask)
{

	//if(nThumbMask != NULL){
	//	m_bmThumbMask2.DeleteObject();
	//	m_bmThumbMask2.LoadBitmap( nThumbMask );
	//}

	if(nThumbMask != NULL){
		m_bmThumbMask.DeleteObject();
		m_bmThumbMask.LoadBitmap( nThumbMask );
	}

}

void CxSkinSlider::SetThumbLock(bool bLock)
{
	m_bThumbLock = bLock;
}

CRect CxSkinSlider::GetThumbClipBox(void)
{
	if(m_bVertical)
	{
		return CRect( m_nMarginLeft, Pos2Pixel(m_nPos) - (m_nThumbHeight/2),	
		m_nMarginLeft + m_nThumbWidth, Pos2Pixel(m_nPos) - (m_nThumbHeight/2) + m_nThumbHeight);
	}
	else{
	return CRect(Pos2Pixel(m_nPos) - (m_nThumbWidth/2), m_nMarginTop,	
		Pos2Pixel(m_nPos) - (m_nThumbWidth/2) + m_nThumbWidth, m_nMarginTop+ m_nThumbHeight);
	}
}