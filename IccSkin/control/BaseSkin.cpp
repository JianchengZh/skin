// BaseSkin.cpp : implementation file
//

#include "stdafx.h"
#include "BaseSkin.h"

#if USE_COOL_SB
#include "Scrollbar.h"
#pragma comment(lib, "coolsb.lib")
#pragma comment(lib, "coolsb_detours.lib")
#pragma comment(lib, "detours.lib")
#endif 

#include "ConvertString.h"
#include "SkinUtil.h"
#include <io.h>

#include "tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseSkin dialog


CBaseSkin::CBaseSkin()
	:m_bInit(false)
	,_lpCallback(NULL)
	,m_clrBk(COLOR_WHITE)
	,m_strSkinFileName(_T("IccSkin.ini"))
	,m_nBGStyle(STYLE_RGB)
	,m_nDLGStyle(DLGSTYLE_NSTYLE)
	,m_rDefRect(DEF_RECT)
	,hMaskRgn(NULL)
	,m_bSetMask(false)	
	,m_bIsSetResize(false)
	,m_bDefSize(false)
	,m_bDrawPattern(false)
	,m_bBlockExit(false)
	,m_bUseShadow(false)
	,m_shadowPosX(0)
	,m_shadowPosY(0)
	,m_shadowSize(0)
	,m_shadowSharp(0)
	,m_shadowDark(0)
	,m_shadowColor(RGB(0,0,0))
{

}

CBaseSkin::~CBaseSkin()
{
	//m_XmlDoc.Clear();
	FreeSkin();
	if(hMaskRgn != NULL)
		DeleteObject(hMaskRgn);
}
/////////////////////////////////////////////////////////////////////////////
// CBaseSkin message handlers


bool CBaseSkin::InitSkin(CString strFileName)
{
	TRACE("######InitSkin::filename=%s \n", strFileName);
	m_strSkinFileName = strFileName;	
	
	if (_taccess(m_strSkinFileName, 0) != 0)
		return false;
	
	m_bInit = true;

	return m_bInit;
}


bool CBaseSkin::_ReLoadSkins(CDC *pDC)
{
	TRACE("######_ReloadSkins######\n");
	CString strImage;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild( DEF_MAIN );
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	node = node->FirstChild();
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	
	strImage = ConvStr::utf8ToCstr(itemElement->Attribute("Image"));

	xmldoc.Clear(); 
	CSize mSize;
	CSkinUtil skinUtil;
	if (_taccess(skinUtil._GetPath(m_strSkinFileName) + strImage, 0) != 0)
		return false;

	if(m_bmSkinImage.m_hObject)
		m_bmSkinImage.DeleteObject();

	if(m_bmBImage.m_hObject)
		m_bmBImage.DeleteObject();

	skinUtil.LoadPictureFile(pDC->m_hDC, skinUtil._GetPath(m_strSkinFileName) + strImage, &m_bmSkinImage, mSize);
	skinUtil.LoadPictureFile(pDC->m_hDC, skinUtil._GetPath(m_strSkinFileName) + strImage, &m_bmBImage, mSize);

	_ReLoadSkinButton();
	_ReLoadSkinStatic();
	_ReLoadSkinSlider();
	_ReLoadSkinText();
	_ReLoadSkinEdit();

#if USE_SKINLISTCTRL
	_ReLoadSkinList();
#endif

#if USE_SKINTREECTRL
	_ReLoadSkinTree();
#endif

#if USE_SKINCOMBOBOX
	_ReLoadSkinComboBox();
#endif

	if(m_bUseShadow)
		LoadShadowValue();

	return true;

}

BOOL CBaseSkin::LoadSkins(CDC *pDC, CWnd* pWnd)
{
	TRACE("LoadSkins dc, wnd\n");
	if(!pWnd) return FALSE;
	m_pWnd = pWnd;

	FreeSkin();
	CString strMainStyle,strResize,strMask,strBGStyle		
		,strImage		
		,strDefSize		
		,strPattern		
		,strPattern_Mask	
		,strBlockExit;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	itemElement = node->ToElement();
	if(itemElement == NULL) return FALSE;
	strMainStyle = ConvStr::utf8ToCstr(itemElement->Attribute("MainStyle"));

	m_strCaption = ConvStr::utf8ToCstr(itemElement->Attribute("Caption"));
	int nret = itemElement->QueryBoolAttribute("Resizable", &m_bIsSetResize);
	nret = itemElement->QueryBoolAttribute("UseMask", &m_bSetMask);

	strBGStyle = ConvStr::utf8ToCstr(itemElement->Attribute("BkStyle"));
	strImage = ConvStr::utf8ToCstr(itemElement->Attribute("ImageName"));
	strDefSize = ConvStr::utf8ToCstr(itemElement->Attribute("Position"));
	nret = itemElement->QueryBoolAttribute("UsePattern", &m_bDrawPattern);
	nret = itemElement->QueryBoolAttribute("UsePatternMask", &m_bDrawPattern_Mask);
	nret = itemElement->QueryBoolAttribute("BlockEscExit", &m_bBlockExit);
	nret = itemElement->QueryBoolAttribute("UseShadow", &m_bUseShadow);

	xmldoc.Clear();

	CSize mSize;
	CSkinUtil skinUtil;	
	if (_taccess(skinUtil._GetPath(m_strSkinFileName) + strImage, 0) != 0) return FALSE;

	if(strDefSize != _T(""))
	{
		m_rDefRect = skinUtil.StringToRect(strDefSize);
		m_bDefSize = true;
	}
	TRACE("LoadSkins::MainStyle=%s\n", strMainStyle);
	if(0 == strMainStyle.CompareNoCase(_T("RESIZE")))
		m_nDLGStyle = DLGSTYLE_RESIZE;
	else if(0 == strMainStyle.CompareNoCase(_T("SKIN")))
		m_nDLGStyle = DLGSTYLE_SKIN;
	else
		m_nDLGStyle = DLGSTYLE_NSTYLE;

	if(m_bSetMask)
	{
		SetMapMask();
		SetMask();
	}
	TRACE("LoadSkins::BackGroundStyle=%s\n", strBGStyle);
	bool bRet = false;
	if(0 == strBGStyle.CompareNoCase(_T("IMAGE")))
	{
		m_nBGStyle = STYLE_IMAGE;
		bRet = SetMapBG();
	}else if(0 == strBGStyle.CompareNoCase(_T("RGB"))){
		m_nBGStyle = STYLE_RGB;
		bRet = SetMapRGB();
	}else if(0 == strBGStyle.CompareNoCase(_T("FIXED"))){
		m_nBGStyle = STYLE_FIXED;
		bRet = SetMapFixed();
	}else{
		return FALSE;
	}
	if(!bRet) return bRet;

	if(strImage == _T("")) return false;
	skinUtil.LoadPictureFile(pDC->m_hDC, skinUtil._GetPath(m_strSkinFileName) + strImage, &m_bmSkinImage, mSize);
	m_bmBImage.Attach(m_bmSkinImage.m_hObject);

	Attach_Control();

	if(m_bUseShadow)
		LoadShadowValue();

	return bRet;
}

void CBaseSkin::LoadShadowValue()
{
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_SHADOW);	
	
	if(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) return;
		
		CString strColor = ConvStr::utf8ToCstr(itemElement->Attribute("Color"));
		m_shadowColor	= skinUtil.StringToRGB(strColor);
		int nret = itemElement->QueryIntAttribute("Size", &m_shadowSize);
		nret = itemElement->QueryIntAttribute("Sharp", &m_shadowSharp);
		nret = itemElement->QueryIntAttribute("Dark", &m_shadowDark);
		nret = itemElement->QueryIntAttribute("PosX", &m_shadowPosX);
		nret = itemElement->QueryIntAttribute("PosY", &m_shadowPosY);		
	}
	xmldoc.Clear();
}

BOOL CBaseSkin::LoadSkins(CDC *pDC, CWnd* pWnd, CBitmap *bmBaseImage)
{
	TRACE("LoadSkins dc, wnd, bitmap\n");
	if(!pWnd) return FALSE;	
	m_pWnd = pWnd;
	FreeSkin();
	CString strMainStyle,strResize,strMask,strBGStyle		
		,strImage		
		,strDefSize		
		,strPattern		
		,strPattern_Mask	
		,strBlockExit;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	itemElement = node->ToElement();
	if(itemElement == NULL) return FALSE;
	strMainStyle = ConvStr::utf8ToCstr(itemElement->Attribute("MainStyle"));

	m_strCaption = ConvStr::utf8ToCstr(itemElement->Attribute("Caption"));
	int nret = itemElement->QueryBoolAttribute("Resizable", &m_bIsSetResize);	
	nret = itemElement->QueryBoolAttribute("UseMask", &m_bSetMask);

	strBGStyle = ConvStr::utf8ToCstr(itemElement->Attribute("BkStyle"));
	strImage = ConvStr::utf8ToCstr(itemElement->Attribute("ImageName"));
	strDefSize = ConvStr::utf8ToCstr(itemElement->Attribute("Position"));
	nret = itemElement->QueryBoolAttribute("UsePattern", &m_bDrawPattern);
	nret = itemElement->QueryBoolAttribute("UsePatternMask", &m_bDrawPattern_Mask);
	nret = itemElement->QueryBoolAttribute("BlockEscExit", &m_bBlockExit);

	xmldoc.Clear();

	CSkinUtil skinUtil;	
	if (_taccess(skinUtil._GetPath(m_strSkinFileName) + strImage, 0) != 0) return FALSE;	
	if(!strDefSize.IsEmpty())
	{
		m_rDefRect = skinUtil.StringToRect(strDefSize);
		m_bDefSize = true;
	}

	TRACE("LoadSkins::MainStyle=%s\n", strMainStyle);
	if(0 == strMainStyle.CompareNoCase(_T("RESIZE")))
		m_nDLGStyle = DLGSTYLE_RESIZE;
	else if(0 == strMainStyle.CompareNoCase(_T("SKIN")))
		m_nDLGStyle = DLGSTYLE_SKIN;
	else
		m_nDLGStyle = DLGSTYLE_NSTYLE;

	if(m_bSetMask)
	{
		SetMapMask();
		SetMask();
	}

	TRACE("LoadSkins::BackGroundStyle=%s\n", strBGStyle);
	bool bRet = false;
	if(0 == strBGStyle.CompareNoCase(_T("IMAGE")))
	{
		m_nBGStyle = STYLE_IMAGE;
		bRet = SetMapBG();
	}
	else if(0 == strBGStyle.CompareNoCase(_T("RGB")))
	{
		m_nBGStyle = STYLE_RGB;
		bRet = SetMapRGB();
	}
	else if(0 == strBGStyle.CompareNoCase(_T("FIXED")))
	{
		m_nBGStyle = STYLE_FIXED;
		bRet = SetMapFixed();
	}
	else
	{
		return FALSE;
	}
	if(!bRet) return bRet;
	
	m_bmSkinImage.Attach(bmBaseImage->m_hObject);
	m_bmBImage.Attach(bmBaseImage->m_hObject);

	Attach_Control();

	return TRUE;
}
void CBaseSkin::FreeSkin()
{
	if(m_bmSkinImage.m_hObject) m_bmSkinImage.DeleteObject();
	if(m_bmBImage.m_hObject) m_bmBImage.DeleteObject();

	for(int i = 0; i < m_ArrButtons.GetSize(); i++)
	{
		CxSkinButton* pBtn = (CxSkinButton*)m_ArrButtons.GetAt(i);
		if(pBtn){
			delete pBtn;
		}
	}
	m_ArrButtons.RemoveAll();
	for(int i = 0; i < m_ArrStatics.GetSize(); i++)
	{
		CxSkinStatic* pSt = (CxSkinStatic*)m_ArrStatics.GetAt(i);
		if(pSt)
		{
			delete pSt;
		}
	}
	m_ArrStatics.RemoveAll();
	for(int i = 0; i < m_ArrSlider.GetSize(); i++)
	{
		CxSkinSlider* pSlr = (CxSkinSlider*)m_ArrSlider.GetAt(i);
		if(pSlr){
			delete pSlr;
		}
	}
	m_ArrSlider.RemoveAll();

	for(int i = 0; i < m_ArrRndSlider.GetSize(); i++)
	{
		CRoundSliderCtrl* pSlr = (CRoundSliderCtrl*)m_ArrRndSlider.GetAt(i);
		if(pSlr){
			delete pSlr;
		}
	}
	m_ArrRndSlider.RemoveAll();

	for(int i = 0; i < m_ArrEdit.GetSize(); i++)
	{
#if USE_SCINTILLAWND
		CScintillaWnd* pEdit = (CScintillaWnd*)m_ArrEdit.GetAt(i);
#elif USE_SKINEDIT
		CxSkinEdit* pEdit = (CxSkinEdit*)m_ArrEdit.GetAt(i);
#else
		CInplaceEdit* pEdit = (CInplaceEdit*)m_ArrEdit.GetAt(i);
#endif
		if(pEdit) delete pEdit;
	}
	m_ArrEdit.RemoveAll();

	for(int i = 0; i < m_ArrTexts.GetSize(); i++)
	{
#if USE_SKINLABEL
		CxSkinLabel* pTxt = (CxSkinLabel*)m_ArrTexts.GetAt(i);
#else
		CxSkinText* pTxt = (CxSkinText*)m_ArrTexts.GetAt(i);
#endif
		if(pTxt) delete pTxt;
	}
	m_ArrTexts.RemoveAll();

#if USE_SKINLISTCTRL
	for(int i = 0; i < this->m_ArrList.GetSize(); i++)
	{
		CxSkinListCtrl* pList = (CxSkinListCtrl*)m_ArrList.GetAt(i);
		if(pList) {
			delete pList;
		}
	}
	m_ArrList.RemoveAll();
#endif

#if USE_SKINTREECTRL
	for(int i = 0; i < this->m_ArrTree.GetSize(); i++)
	{
		CxSkinTreeCtrl* pTree = (CxSkinTreeCtrl*)m_ArrTree.GetAt(i);
		if(pTree){
			delete pTree;
		}
	}
	m_ArrTree.RemoveAll();
#endif

#if USE_SKINCOMBOBOX
	for (int i = 0; i < this->m_ArrCombo.GetSize(); i++)
	{
		CxSkinComboBox* pCombo = (CxSkinComboBox*)m_ArrCombo.GetAt(i);
		if (pCombo) delete pCombo;
	}
	m_ArrCombo.RemoveAll();
#endif

}

void CBaseSkin::SetFixed(CDC *pDC)
{
	if(!m_bmSkinImage.m_hObject) return;

	CDC objDC;
	CDC* memDC;
	memDC = new CDC();

	memDC->CreateCompatibleDC(pDC);
	objDC.CreateCompatibleDC(pDC);

	CRect cRect;
	m_pWnd->GetClientRect(&cRect);

	CBitmap bmpBuffer;
	bmpBuffer.CreateCompatibleBitmap(pDC, cRect.Width(), cRect.Height());
	CBitmap *pOldBitmap = (CBitmap*)memDC->SelectObject(&bmpBuffer);
	CBitmap *pobjBitmap = objDC.SelectObject(&m_bmSkinImage);

	memDC->FillSolidRect(&cRect, GetSysColor(COLOR_BTNFACE));
	memDC->BitBlt(0, 0, m_Map_Fixed.Width(), m_Map_Fixed.Height(), &objDC, m_Map_Fixed.left, m_Map_Fixed.top, SRCCOPY);

	if(_lpCallback){
		_lpCallback(memDC, m_pWnd); 
	}

	pDC->BitBlt(0, 0, cRect.Width(), cRect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->SelectObject(pOldBitmap);
	memDC->DeleteDC();
	delete memDC;

	objDC.SelectObject(pobjBitmap);
	DeleteDC(objDC);
	DeleteObject(bmpBuffer.m_hObject);
}

void CBaseSkin::SetMask()
{
	if(!m_Map_Mask.m_bInit || !m_bInit) return;
	const TCHAR* MsgCaption =_T("Skin Mask");

	if (m_pWnd->GetSafeHwnd())
	{
		HRGN hRgn = NULL;
		COLORREF cTransparentColor = COLOR_WHITE;
		COLORREF cTolerance = COLOR_BLACK;

		CRect rcWnd;
		m_pWnd->GetWindowRect(&rcWnd);

		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
				sizeof(BITMAPINFOHEADER),	// biSize 
				rcWnd.Width(),				// biWidth; 
				rcWnd.Height(),				// biHeight; 
				1,							// biPlanes; 
				32,							// biBitCount 
				BI_RGB,						// biCompression; 
				0,							// biSizeImage; 
				0,							// biXPelsPerMeter; 
				0,							// biYPelsPerMeter; 
				0,							// biClrUsed; 
				0							// biClrImportant; 
			};

			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if(hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					holdBmp = (HBITMAP)SelectObject(hDC, m_bmSkinImage);

					//__Test code
					/*					CBrush hBrush;
					hBrush.CreatePatternBrush (&m_bmBrush);
					CRect FillBGRect(0,0,rcWnd.Width(), rcWnd.Height());
					FillRect(hMemDC, &FillBGRect, (HBRUSH)hBrush);  
					*/

					//////////////////////////////////////////////////////////////////////////
					//__Top Part

					//__TopLeft
					BitBlt(hMemDC, 0, 0, m_Map_Mask.OffSet(M_TOP_LEFT).Width(), m_Map_Mask.OffSet(M_TOP_LEFT).Height(), 
						hDC, m_Map_Mask.OffSet(M_TOP_LEFT).left, m_Map_Mask.OffSet(M_TOP_LEFT).top, SRCCOPY);

					//__TopCenter
					if (m_bDrawPattern_Mask)//마스크용 플래그를 따로 구성 2008.10.17 이영혁
					{
						//int xFillsNeeded = (rcWnd.Width() - (m_Map_Mask.OffSet(M_TOP_LEFT).Width() + m_Map_Mask.OffSet(M_TOP_RIGHT).Width())) / m_Map_Mask.OffSet(M_TOP_CENTER).Width();
						int nMLW = m_Map_Mask.OffSet(M_TOP_LEFT).Width();
						int nMRW = m_Map_Mask.OffSet(M_TOP_RIGHT).Width();
						int nMCW = m_Map_Mask.OffSet(M_TOP_CENTER).Width();

						if(nMCW <= 0)
							nMCW = 1;

						int xFillsNeeded = (rcWnd.Width() - (nMLW + nMRW)) / nMCW;

						for(int i = 0; i <= xFillsNeeded; i++)
						{
							BitBlt(hMemDC, m_Map_Mask.OffSet(M_TOP_LEFT).Width() + (m_Map_Mask.OffSet(M_TOP_CENTER).Width() * i),
								0, m_Map_Mask.OffSet(M_TOP_CENTER).Width(), m_Map_Mask.OffSet(M_TOP_CENTER).Height(), 
								hDC, m_Map_Mask.OffSet(M_TOP_CENTER).left, m_Map_Mask.OffSet(M_TOP_CENTER).top, SRCCOPY);
						}
					}
					else
					{
						int xStretch = rcWnd.Width() - (m_Map_Mask.OffSet(M_TOP_LEFT).Width() + m_Map_Mask.OffSet(M_TOP_RIGHT).Width());

						StretchBlt(hMemDC, m_Map_Mask.OffSet(M_TOP_LEFT).Width(), 0, xStretch, 
							m_Map_Mask.OffSet(M_TOP_CENTER).Height(), 
							hDC, m_Map_Mask.OffSet(M_TOP_CENTER).left, m_Map_Mask.OffSet(M_TOP_CENTER).top, 
							m_Map_Mask.OffSet(M_TOP_CENTER).Width(), m_Map_Mask.OffSet(M_TOP_CENTER).Height(), SRCCOPY);
					}

					//__TopRight
					BitBlt(hMemDC, (rcWnd.Width() - m_Map_Mask.OffSet(M_TOP_RIGHT).Width()), 0, 
						m_Map_Mask.OffSet(M_TOP_RIGHT).Width(), m_Map_Mask.OffSet(M_TOP_RIGHT).Height(), 
						hDC, m_Map_Mask.OffSet(M_TOP_RIGHT).left, m_Map_Mask.OffSet(M_TOP_RIGHT).top, SRCCOPY);

					//////////////////////////////////////////////////////////////////////////
					//__Middle Part	

					int y = 0;

					if (m_bDrawPattern_Mask)//마스크용 플래그를 따로 구성 2008.10.17 이영혁
					{
						//int yFillsNeeded = (rcWnd.Height() - (m_Map_Mask.OffSet(M_TOP_LEFT).Height() + m_Map_Mask.OffSet(M_BOTTOM_LEFT).Height())) / m_Map_Mask.OffSet(M_MIDDLE_LEFT).Height();

						int nTLH = m_Map_Mask.OffSet(M_TOP_LEFT).Height();
						int nBLH = m_Map_Mask.OffSet(M_BOTTOM_LEFT).Height();
						int nMLH = m_Map_Mask.OffSet(M_MIDDLE_LEFT).Height();

						if(nMLH <= 0)
							nMLH = 1;

						int yFillsNeeded = (rcWnd.Height() - (nTLH + nBLH)) / nMLH;

						//xFillsNeeded = (rcWnd.Width() - (m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width() + m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width())) / m_Map_Mask.OffSet(M_MIDDLE_CENTER).Width();;

						int nMLW = m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width();
						int nMRW = m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width();
						int nMCW = m_Map_Mask.OffSet(M_MIDDLE_CENTER).Width();

						if(nMCW <= 0)
							nMCW = 1;

						int xFillsNeeded = (rcWnd.Width() - (nMLW + nMRW)) / nMCW;

						for(int i = 0; i <= yFillsNeeded; i++){
							y = m_Map_Mask.OffSet(M_TOP_LEFT).Height() + (m_Map_Mask.OffSet(M_MIDDLE_LEFT).Height() * i);
							//__MiddleLeft						
							BitBlt(hMemDC, 0, y, m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width(), 
								m_Map_Mask.OffSet(M_MIDDLE_LEFT).Height(), hDC, m_Map_Mask.OffSet(M_MIDDLE_LEFT).left, 
								m_Map_Mask.OffSet(M_MIDDLE_LEFT).top, SRCCOPY);
							for(int j = 0; j <= xFillsNeeded; j++){
								//__MiddleCenter
								BitBlt(hMemDC, m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width() + 
									(m_Map_Mask.OffSet(M_MIDDLE_CENTER).Width() * j), y, 
									m_Map_Mask.OffSet(M_MIDDLE_CENTER).Width(), m_Map_Mask.OffSet(M_MIDDLE_CENTER).Height(), 
									hDC, m_Map_Mask.OffSet(M_MIDDLE_CENTER).left, m_Map_Mask.OffSet(M_MIDDLE_CENTER).top, SRCCOPY);
							}
							//__MiddleRight
							BitBlt(hMemDC, (rcWnd.Width() - m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width()), y, 
								m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width(), m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Height(), 
								hDC, m_Map_Mask.OffSet(M_MIDDLE_RIGHT).left, m_Map_Mask.OffSet(M_MIDDLE_RIGHT).top, SRCCOPY);
						}
					}
					else
					{
						int yStretch = rcWnd.Height() - (m_Map_Mask.OffSet(M_TOP_LEFT).Height() + m_Map_Mask.OffSet(M_BOTTOM_LEFT).Height());
						int xStretch = rcWnd.Width() - (m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width() + m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width());

						StretchBlt(hMemDC,0, m_Map_BG.OffSet(M_TOP_LEFT).Height(), m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width(), yStretch, 
							hDC, m_Map_Mask.OffSet(M_MIDDLE_LEFT).left, m_Map_Mask.OffSet(M_MIDDLE_LEFT).top, 
							m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width(), m_Map_Mask.OffSet(M_MIDDLE_LEFT).Height(), SRCCOPY);
						StretchBlt(hMemDC,m_Map_Mask.OffSet(M_MIDDLE_LEFT).Width(), m_Map_Mask.OffSet(M_TOP_LEFT).Height(), xStretch, yStretch, 
							hDC, m_Map_Mask.OffSet(M_MIDDLE_CENTER).left, m_Map_Mask.OffSet(M_MIDDLE_CENTER).top, 
							m_Map_Mask.OffSet(M_MIDDLE_CENTER).Width(), m_Map_Mask.OffSet(M_MIDDLE_CENTER).Height(), SRCCOPY);
						StretchBlt(hMemDC, rcWnd.Width() - m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width(), m_Map_Mask.OffSet(M_TOP_RIGHT).Height(), 
							m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width(), yStretch, hDC, m_Map_Mask.OffSet(M_MIDDLE_RIGHT).left, 
							m_Map_Mask.OffSet(M_MIDDLE_RIGHT).top, m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Width(), 
							m_Map_Mask.OffSet(M_MIDDLE_RIGHT).Height(), SRCCOPY);
					}

					//////////////////////////////////////////////////////////////////////////
					//__Bottom Part

					//Left
					BitBlt(hMemDC, 0, rcWnd.Height() - m_Map_Mask.OffSet(M_BOTTOM_LEFT).Height(), m_Map_Mask.OffSet(M_BOTTOM_LEFT).Width(), m_Map_Mask.OffSet(M_BOTTOM_LEFT).Height(), hDC, m_Map_Mask.OffSet(M_BOTTOM_LEFT).left, m_Map_Mask.OffSet(M_BOTTOM_LEFT).top, SRCCOPY);

					if (m_bDrawPattern_Mask)//마스크용 플래그를 따로 구성 2008.10.17 이영혁
					{
						//xFillsNeeded = (rcWnd.Width() - (m_Map_Mask.OffSet(M_BOTTOM_LEFT).Width() + m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Width())) / m_Map_Mask.OffSet(M_BOTTOM_CENTER).Width();
						int nMLW = m_Map_Mask.OffSet(M_BOTTOM_LEFT).Width();
						int nMRW = m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Width();
						int nMCW = m_Map_Mask.OffSet(M_BOTTOM_CENTER).Width();

						if(nMCW <= 0)
							nMCW = 1;

						int xFillsNeeded = (rcWnd.Width() - (nMLW + nMRW)) / nMCW;

						for(int i = 0; i <= xFillsNeeded; i++)
						{
							BitBlt(hMemDC, m_Map_Mask.OffSet(M_BOTTOM_LEFT).Width() + 
								(m_Map_Mask.OffSet(M_BOTTOM_CENTER).Width() * i), 
								rcWnd.Height() - m_Map_Mask.OffSet(M_BOTTOM_CENTER).Height(), 
								m_Map_Mask.OffSet(M_BOTTOM_CENTER).Width(), m_Map_Mask.OffSet(M_BOTTOM_CENTER).Height(), 
								hDC, m_Map_Mask.OffSet(M_BOTTOM_CENTER).left, m_Map_Mask.OffSet(M_BOTTOM_CENTER).top, SRCCOPY);
						}
					}
					else
					{
						int xStretch = rcWnd.Width() - (m_Map_BG.OffSet(M_BOTTOM_LEFT).Width() + 
							m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Width());

						StretchBlt(hMemDC, m_Map_Mask.OffSet(M_BOTTOM_LEFT).Width(), rcWnd.Height() - m_Map_Mask.OffSet(M_BOTTOM_LEFT).Height(), 
							xStretch, m_Map_Mask.OffSet(M_BOTTOM_CENTER).Height(), hDC, m_Map_Mask.OffSet(M_BOTTOM_CENTER).left, 
							m_Map_Mask.OffSet(M_BOTTOM_CENTER).top, m_Map_Mask.OffSet(M_BOTTOM_CENTER).Width(), 
							m_Map_Mask.OffSet(M_BOTTOM_CENTER).Height(), SRCCOPY);
					}

					BitBlt(hMemDC, (rcWnd.Width() - m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Width()), rcWnd.Height() - m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Height(), m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Width(), m_Map_Mask.OffSet(M_BOTTOM_RIGHT).Height(), hDC, m_Map_Mask.OffSet(M_BOTTOM_RIGHT).left, m_Map_Mask.OffSet(M_BOTTOM_RIGHT).top, SRCCOPY);

					//////////////////////////////////////////////////////////////////////////


					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (y = 0; y < rcWnd.Height(); y++){
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < rcWnd.Width(); x++){
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < rcWnd.Width()){
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr){
									b = GetGValue(*p);
									if (b >= lg && b <= hg){
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0){
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects){
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000){
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn){
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn){
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalUnlock(hData);
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}//__End if (hMemDC)


		if(hRgn != NULL){

			if(hMaskRgn != NULL)
				DeleteObject(hMaskRgn);
			CombineRgn(hMaskRgn, hRgn, NULL, RGN_COPY);

			if (!::SetWindowRgn (m_pWnd->GetSafeHwnd(), hRgn, TRUE)) //(HRGN) *m_Rgn, TRUE))
				MessageBox (m_pWnd->m_hWnd, _T("Error mask region"), MsgCaption, MB_OK);
			DeleteObject(hRgn);


		}else{

		}
	}
}

bool CBaseSkin::SetBG(CDC* pDC)
{
	if(!m_Map_BG.m_bInit || !m_bInit) return false;
	int i = 0;
	CDC* memDC;
	memDC = new CDC;
	CDC objDC;
	memDC->CreateCompatibleDC(pDC);
	objDC.CreateCompatibleDC(pDC);

	CRect cRect;
	m_pWnd->GetClientRect(&cRect);

	CBitmap bmpBuffer;
	bmpBuffer.CreateCompatibleBitmap(pDC, cRect.Width(), cRect.Height());

	CBitmap *pOldBitmap = (CBitmap*)memDC->SelectObject(&bmpBuffer);
	CBitmap *poldtmpbitmap = objDC.SelectObject(&m_bmSkinImage);

	//////////////////////////////////////////////////////////////////////////
	//__Top Part

	//__TopLeft
	memDC->BitBlt(0, 0, m_Map_BG.OffSet(M_TOP_LEFT).Width(), m_Map_BG.OffSet(M_TOP_LEFT).Height(), 
		&objDC, m_Map_BG.OffSet(M_TOP_LEFT).left, m_Map_BG.OffSet(M_TOP_LEFT).top, SRCCOPY);

	//__TopCenter
	if(m_bDrawPattern)
	{
		//int xFillsNeeded = (cRect.Width() - (m_Map_BG.OffSet(M_TOP_LEFT).Width() + m_Map_BG.OffSet(M_TOP_RIGHT).Width())) / m_Map_BG.OffSet(M_TOP_CENTER).Width();
		int nMLW = m_Map_BG.OffSet(M_TOP_LEFT).Width();
		int nMRW = m_Map_BG.OffSet(M_TOP_RIGHT).Width();
		int nMCW = m_Map_BG.OffSet(M_TOP_CENTER).Width();

		if(nMCW <= 0)
			nMCW = 1;

		int xFillsNeeded = (cRect.Width() - (nMLW + nMRW)) / nMCW;

		for(i = 0; i <= xFillsNeeded; i++)
		{
			memDC->BitBlt(m_Map_BG.OffSet(M_TOP_LEFT).Width() + (m_Map_BG.OffSet(M_TOP_CENTER).Width() * i), 0, 
				m_Map_BG.OffSet(M_TOP_CENTER).Width(), m_Map_BG.OffSet(M_TOP_CENTER).Height(), 
				&objDC, m_Map_BG.OffSet(M_TOP_CENTER).left, m_Map_BG.OffSet(M_TOP_CENTER).top, SRCCOPY);
		}
	}
	else
	{
		int xStretch = cRect.Width() - (m_Map_BG.OffSet(M_TOP_LEFT).Width() + m_Map_BG.OffSet(M_TOP_RIGHT).Width());

		memDC->StretchBlt(m_Map_BG.OffSet(M_TOP_LEFT).Width(), 0, xStretch, m_Map_BG.OffSet(M_TOP_CENTER).Height(), 
			&objDC, m_Map_BG.OffSet(M_TOP_CENTER).left, m_Map_BG.OffSet(M_TOP_CENTER).top, m_Map_BG.OffSet(M_TOP_CENTER).Width(), m_Map_BG.OffSet(M_TOP_CENTER).Height(), SRCCOPY);
	}

	//__TopRight
	memDC->BitBlt((cRect.Width() - m_Map_BG.OffSet(M_TOP_RIGHT).Width()), 0, m_Map_BG.OffSet(M_TOP_RIGHT).Width(), 
		m_Map_BG.OffSet(M_TOP_RIGHT).Height(), &objDC, m_Map_BG.OffSet(M_TOP_RIGHT).left, m_Map_BG.OffSet(M_TOP_RIGHT).top, SRCCOPY);

	//////////////////////////////////////////////////////////////////////////
	//__Middle Part	
	if(m_bDrawPattern)
	{
		int nTLH = m_Map_BG.OffSet(M_TOP_LEFT).Height();
		int nBLH = m_Map_BG.OffSet(M_BOTTOM_LEFT).Height();
		int nMLH = m_Map_BG.OffSet(M_MIDDLE_LEFT).Height();

		if(nMLH <= 0)
			nMLH = 1;

		int yFillsNeeded = (cRect.Height() - (nTLH + nBLH)) / nMLH;

		int nMLW = m_Map_BG.OffSet(M_MIDDLE_LEFT).Width();
		int nMRW = m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width();
		int nMCW = m_Map_BG.OffSet(M_MIDDLE_CENTER).Width();

		if(nMCW <= 0)
			nMCW = 1;

		int xFillsNeeded = (cRect.Width() - (nMLW + nMRW)) / nMCW;


		int y = 0;
		for(i = 0; i <= yFillsNeeded; i++)
		{
			y = m_Map_BG.OffSet(M_TOP_LEFT).Height() + (m_Map_BG.OffSet(M_MIDDLE_LEFT).Height() * i);

			//__MiddleLeft						
			memDC->BitBlt(0, y, m_Map_BG.OffSet(M_MIDDLE_LEFT).Width(), m_Map_BG.OffSet(M_MIDDLE_LEFT).Height(), 
				&objDC, m_Map_BG.OffSet(M_MIDDLE_LEFT).left, m_Map_BG.OffSet(M_MIDDLE_LEFT).top, SRCCOPY);

			for(int j = 0; j <= xFillsNeeded; j++)
			{
				//__MiddleCenter
				memDC->BitBlt(m_Map_BG.OffSet(M_MIDDLE_LEFT).Width() + (m_Map_BG.OffSet(M_MIDDLE_CENTER).Width() * j), y, 
					m_Map_BG.OffSet(M_MIDDLE_CENTER).Width(), m_Map_BG.OffSet(M_MIDDLE_CENTER).Height(), 
					&objDC, m_Map_BG.OffSet(M_MIDDLE_CENTER).left, m_Map_BG.OffSet(M_MIDDLE_CENTER).top, SRCCOPY);
			}

			//__MiddleRight
			memDC->BitBlt((cRect.Width() - m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width()), y, 
				m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width(), m_Map_BG.OffSet(M_MIDDLE_RIGHT).Height(), 
				&objDC, m_Map_BG.OffSet(M_MIDDLE_RIGHT).left, m_Map_BG.OffSet(M_MIDDLE_RIGHT).top, SRCCOPY);
		}
	}
	else
	{
		int yStretch = cRect.Height() - (m_Map_BG.OffSet(M_TOP_LEFT).Height() + m_Map_BG.OffSet(M_BOTTOM_LEFT).Height());
		int xStretch = cRect.Width() - (m_Map_BG.OffSet(M_MIDDLE_LEFT).Width() + m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width());

		//Left
		memDC->StretchBlt(0, m_Map_BG.OffSet(M_TOP_LEFT).Height(), m_Map_BG.OffSet(M_MIDDLE_LEFT).Width(), yStretch, 
			&objDC, m_Map_BG.OffSet(M_MIDDLE_LEFT).left, m_Map_BG.OffSet(M_MIDDLE_LEFT).top, 
			m_Map_BG.OffSet(M_MIDDLE_LEFT).Width(), m_Map_BG.OffSet(M_MIDDLE_LEFT).Height(), SRCCOPY);
		//Center
		memDC->StretchBlt(m_Map_BG.OffSet(M_MIDDLE_LEFT).Width(), m_Map_BG.OffSet(M_TOP_LEFT).Height(), xStretch, yStretch, 
			&objDC, m_Map_BG.OffSet(M_MIDDLE_CENTER).left, m_Map_BG.OffSet(M_MIDDLE_CENTER).top, 
			m_Map_BG.OffSet(M_MIDDLE_CENTER).Width(), m_Map_BG.OffSet(M_MIDDLE_CENTER).Height(), SRCCOPY);
		//Right
		memDC->StretchBlt(cRect.Width() - m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width(), m_Map_BG.OffSet(M_TOP_RIGHT).Height(), 
			m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width(), yStretch, &objDC, m_Map_BG.OffSet(M_MIDDLE_RIGHT).left, 
			m_Map_BG.OffSet(M_MIDDLE_RIGHT).top, m_Map_BG.OffSet(M_MIDDLE_RIGHT).Width(), 
			m_Map_BG.OffSet(M_MIDDLE_RIGHT).Height(), SRCCOPY);
	}

	//////////////////////////////////////////////////////////////////////////
	//__Bottom Part

	//Left
	memDC->BitBlt(0, cRect.Height() - m_Map_BG.OffSet(M_BOTTOM_LEFT).Height(), m_Map_BG.OffSet(M_BOTTOM_LEFT).Width(), m_Map_BG.OffSet(M_BOTTOM_LEFT).Height(), &objDC, m_Map_BG.OffSet(M_BOTTOM_LEFT).left, m_Map_BG.OffSet(M_BOTTOM_LEFT).top, SRCCOPY);

	//Center
	if(m_bDrawPattern)
	{
		//xFillsNeeded = (cRect.Width() - (m_Map_BG.OffSet(M_BOTTOM_LEFT).Width() + m_Map_BG.OffSet(M_BOTTOM_RIGHT).Width())) / m_Map_BG.OffSet(M_BOTTOM_CENTER).Width();
		int nMLW = m_Map_BG.OffSet(M_BOTTOM_LEFT).Width();
		int nMRW = m_Map_BG.OffSet(M_BOTTOM_RIGHT).Width();
		int nMCW = m_Map_BG.OffSet(M_BOTTOM_CENTER).Width();

		if(nMCW <= 0)
			nMCW = 1;

		int xFillsNeeded = (cRect.Width() - (nMLW + nMRW)) / nMCW;

		for(i = 0; i <= xFillsNeeded; i++)
		{
			memDC->BitBlt(m_Map_BG.OffSet(M_BOTTOM_LEFT).Width() + (m_Map_BG.OffSet(M_BOTTOM_CENTER).Width() * i), 
				cRect.Height() - m_Map_BG.OffSet(M_BOTTOM_CENTER).Height(), m_Map_BG.OffSet(M_BOTTOM_CENTER).Width(), 
				m_Map_BG.OffSet(M_BOTTOM_CENTER).Height(), &objDC, m_Map_BG.OffSet(M_BOTTOM_CENTER).left, 
				m_Map_BG.OffSet(M_BOTTOM_CENTER).top, SRCCOPY);
		}
	}
	else
	{
		int xStretch = cRect.Width() - (m_Map_BG.OffSet(M_BOTTOM_LEFT).Width() + m_Map_BG.OffSet(M_BOTTOM_RIGHT).Width());

		memDC->StretchBlt(m_Map_BG.OffSet(M_BOTTOM_LEFT).Width(), cRect.Height() - m_Map_BG.OffSet(M_BOTTOM_LEFT).Height(), 
			xStretch, m_Map_BG.OffSet(M_BOTTOM_CENTER).Height(), &objDC, m_Map_BG.OffSet(M_BOTTOM_CENTER).left, 
			m_Map_BG.OffSet(M_BOTTOM_CENTER).top, m_Map_BG.OffSet(M_BOTTOM_CENTER).Width(), 
			m_Map_BG.OffSet(M_BOTTOM_CENTER).Height(), SRCCOPY);
	}

	//Right
	memDC->BitBlt((cRect.Width() - m_Map_BG.OffSet(M_BOTTOM_RIGHT).Width()), 
		cRect.Height() - m_Map_BG.OffSet(M_BOTTOM_RIGHT).Height(), m_Map_BG.OffSet(M_BOTTOM_RIGHT).Width(), 
		m_Map_BG.OffSet(M_BOTTOM_RIGHT).Height(), &objDC, m_Map_BG.OffSet(M_BOTTOM_RIGHT).left, 
		m_Map_BG.OffSet(M_BOTTOM_RIGHT).top, SRCCOPY);

	//////////////////////////////////////////////////////////////////////////

	objDC.SelectObject(poldtmpbitmap);
	DeleteDC(objDC);

	if(_lpCallback){
		_lpCallback(memDC, m_pWnd); 
	}


	pDC->BitBlt(0, 0, cRect.Width(), cRect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->SelectObject(pOldBitmap);
	memDC->DeleteDC();
	delete memDC;

	DeleteObject(bmpBuffer.m_hObject);
	return true;
}

CBitmap* CBaseSkin::GetSkinImage()
{
	return &m_bmBImage;
}

void CBaseSkin::Attach_Control()
{
	TRACE("Attach_Control\n");

	AttachButton();	
	AttachStatic();
	AttachText();
	AttachSlider();
	AttachRndSlider();
	AttachEdit();

#if USE_SKINCOMBOBOX
	AttachComboBox();
#endif
#if USE_SKINLISTCTRL
	AttachList();
#endif
#if USE_SKINTREECTRL
	AttachTree();
#endif

}

void CBaseSkin::AttachButton()
{
	TRACE("######AttachButton######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_BUTTON);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_BUTTON_DEF && nID < IDC_STATIC_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachButton::Node=%s, Name=%s, Id=%d\n", node->Value(), strCap, nID);
		CString strToolTip		= ConvStr::utf8ToCstr(itemElement->Attribute("Tooltip"));
		CString strToolTipDw	= ConvStr::utf8ToCstr(itemElement->Attribute("TooltipChecked"));//체크상태 툴팁
		bool bCheckbox = false;
		nret = itemElement->QueryBoolAttribute("Toggle", &bCheckbox);
		bool bUseHandCursor = false;	
		nret = itemElement->QueryBoolAttribute("UseHandCursor", &bUseHandCursor); //핸드커서
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute("NormalRect"));
		CString strOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute("OverRect"));
		CString strDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute("DownRect"));
		CString strDisabledRect	= ConvStr::utf8ToCstr(itemElement->Attribute("DisabledRect"));
		CString strMaskRect		= ConvStr::utf8ToCstr(itemElement->Attribute("MaskRect"));
		CString strFontName = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize	= FONT_SIZE;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		bool bFontBold = false;
		nret = itemElement->QueryBoolAttribute(DEF_BOLD, &bFontBold);
		CString strTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FontColorFocus"));

		CRect Rect_Pos		= skinUtil.StringToRect(strPos);
		CRect Rect_Normal	= skinUtil.StringToRect(strNormalRect);
		CRect Rect_Over		= skinUtil.StringToRect(strOverRect);
		CRect Rect_Dwon		= skinUtil.StringToRect(strDownRect);
		CRect Rect_Disabled	= skinUtil.StringToRect(strDisabledRect);
		CRect Rect_Mask		= skinUtil.StringToRect(strMaskRect);

		DWORD dwStyle = WS_VISIBLE | WS_TABSTOP;
		if(bCheckbox)
			dwStyle |= BS_AUTOCHECKBOX;

		CxSkinButton* pButton = new CxSkinButton;
		BOOL ret = pButton->Create(NULL, dwStyle, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pButton;
			node = node->NextSibling();
			continue;
		}
		pButton->SetCaption(strCap);
		pButton->SetId(nID);
		pButton->SetUseHandCursor(bUseHandCursor);
		pButton->CopyFrom(m_bmSkinImage, Rect_Normal, Rect_Over, Rect_Dwon, Rect_Disabled, Rect_Mask,0,0,0);
		pButton->SetToolTipText(strToolTip, strToolTipDw);
		pButton->SetFontName(strFontName);
		pButton->SetFontSize(nFontSize);
		pButton->SetFontBold(bFontBold);
		pButton->Init();
		m_ArrButtons.Add(pButton);
		
		node = node->NextSibling();
		continue;
	}
	xmldoc.Clear();

}

void CBaseSkin::_ReLoadSkinButton()
{
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;	
	node = node->FirstChild(DEF_BUTTON);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_BUTTON_DEF && nID < IDC_STATIC_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinButton::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strToolTip		= ConvStr::utf8ToCstr(itemElement->Attribute("Tooltip"));
		CString strToolTipDw	= ConvStr::utf8ToCstr(itemElement->Attribute("TooltipChecked"));//체크상태 툴팁
		bool bCheckbox = false;
		nret = itemElement->QueryBoolAttribute("CheckStyle", &bCheckbox);
		bool bUseHandCursor = false;	
		nret = itemElement->QueryBoolAttribute("UseHandCursor", &bUseHandCursor); //핸드커서
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute("NormalRect"));
		CString strOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute("OverRect"));
		CString strDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute("DownRect"));
		CString strDisabledRect	= ConvStr::utf8ToCstr(itemElement->Attribute("DisabledRect"));
		CString strMaskRect		= ConvStr::utf8ToCstr(itemElement->Attribute("MaskRect"));
		CString strFontName = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize		= FONT_SIZE;
		itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		bool bFontBold = false;
		nret = itemElement->QueryBoolAttribute(DEF_BOLD, &bFontBold);
		CString strTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FontColorFocus"));
		
		CRect Rect_Pos		= skinUtil.StringToRect(strPos);
		CRect Rect_Normal	= skinUtil.StringToRect(strNormalRect);
		CRect Rect_Over		= skinUtil.StringToRect(strOverRect);
		CRect Rect_Dwon		= skinUtil.StringToRect(strDownRect);
		CRect Rect_Disabled	= skinUtil.StringToRect(strDisabledRect);
		CRect Rect_Mask		= skinUtil.StringToRect(strMaskRect);

		CxSkinButton* pButton = FindButton(nID);
		if(pButton != NULL)
		{
			pButton->CopyFrom(m_bmSkinImage, Rect_Normal, Rect_Over, Rect_Dwon, Rect_Disabled, Rect_Mask,0,0,0);
		}
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

CxSkinButton* CBaseSkin::FindButton(int id)
{
	int nSz = (int)m_ArrButtons.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CxSkinButton* pButton = m_ArrButtons.GetAt(j);
		if(pButton->GetId() == id){
			return pButton;
		}
	}
	return NULL;
}

void CBaseSkin::AttachText(void)
{
	TRACE("######AttachText######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;	
	node = node->FirstChild(DEF_TEXT);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_TEXT_DEF && nID < IDC_LIST_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachText::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strText			= ConvStr::utf8ToCstr(itemElement->Attribute("Value"));
		CString strLinkText		= ConvStr::utf8ToCstr(itemElement->Attribute("HyperLink"));
		bool bNotifyLink = false;
		nret = itemElement->QueryBoolAttribute("NotifyLink", &bNotifyLink);
		bool bUnderline = false,
			bEllipsis = false;
		nret = itemElement->QueryBoolAttribute("Underline", &bUnderline);
		nret = itemElement->QueryBoolAttribute("Ellipsis", &bEllipsis);
		int nLineSpace = 0;
		nret = itemElement->QueryIntAttribute("LineSpace", &nLineSpace);
		CString strAlign		= ConvStr::utf8ToCstr(itemElement->Attribute("HorzAlign"));
		bool bVertCenter = false;
		nret = itemElement->QueryBoolAttribute("VertCenter", &bVertCenter);		
		bool bWordWrap = false;
		nret = itemElement->QueryBoolAttribute("WordWrap", &bWordWrap);		
		CString strFontName		= FONT_NAME;
		strFontName = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize		= FONT_SIZE;
		itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		bool bFontBold = false;
		nret = itemElement->QueryBoolAttribute(DEF_BOLD, &bFontBold);
		CString strTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strTextColorF	= ConvStr::utf8ToCstr(itemElement->Attribute("TextColorFocus"));
		CString strTextColorU	= ConvStr::utf8ToCstr(itemElement->Attribute("TextColorUnlink"));
		CString	strBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		
		UINT nAlign = SS_LEFT;
		if (0 == strAlign.CompareNoCase(_T("RIGHT")))
			nAlign = SS_RIGHT;
		else if (0 == strAlign.CompareNoCase(_T("CENTER")))
			nAlign = SS_CENTER;

		CRect Rect_Pos			= skinUtil.StringToRect(strPos);

		if(strTextColorF == _T(""))
			strTextColorF = strTextColor;

		if(strTextColorU == _T(""))
			strTextColorU = strTextColor;

		COLORREF clrText		= skinUtil.StringToRGB(strTextColor);
		COLORREF clrTextFocus	= skinUtil.StringToRGB(strTextColorF);
		COLORREF clrTextUnlink	= skinUtil.StringToRGB(strTextColorU);
		bool bTrans = true;
		if (!strBkColor.IsEmpty()) bTrans = false;
		COLORREF	clrBk	= skinUtil.StringToRGB(strBkColor);
		DWORD dwStyle = WS_VISIBLE;
		dwStyle |= nAlign;

#if USE_SKINLABEL
		CxSkinLabel* pLabel = new CxSkinLabel;

		BOOL ret = pLabel->Create(NULL, dwStyle, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pLabel;
			node = node->NextSibling();
			continue;
		}
		
		pLabel->SetId(nID);
		pLabel->SetVertCenter( bVertCenter);
		pLabel->SetEllipse( bEllipsis);
		pLabel->SetTransparent(bTrans);
		pLabel->SetBkColor(clrBk);
		pLabel->SetTextColor(clrText, clrTextFocus, clrTextUnlink);
		pLabel->SetWordWrap(bWordWrap);
		pLabel->SetFontName(strFontName);
		pLabel->SetFontSize(nFontSize);
		pLabel->SetFontBold(bFontBold);
		pLabel->SetFontUnderline(bUnderline);
		pLabel->SetCaption(strCap);
		pLabel->SetLineSpace(nLineSpace);

		if (strLinkText != _T(""))
		{
			pLabel->SetHyperLink(strLinkText);
			pLabel->SetLink(TRUE, bNotifyLink);
			pLabel->SetLinkCursor(LoadCursor(NULL, IDC_HAND));
		}
		pLabel->SetText(strText);		
		m_ArrTexts.Add(pLabel);
#else
		CxSkinText* pLabel = new CxSkinText;		
		BOOL ret = pLabel->Create(NULL, dwStyle, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pLabel;
			node = node->NextSibling();
			continue;
		}
		
		pLabel->SetId(nID);
		//pLabel->SetVertAlign( bValign);
		//pLabel->SetEllipse( bEllipsis);
		//pLabel->SetTransparent(bTrans);
		//pLabel->SetBkColor(clrBk);
		pLabel->SetTextColor(corFont);
		pLabel->SetFontName(strFontName);
		pLabel->SetFontSize(nFontSize);
		pLabel->SetFontBold(bFontBold);
		pLabel->SetFontUnderline(bUnderline);
		pLabel->SetText(strText);		
		pLabel->SetCaption(strCap);
		
		/*
		if (strLinkText != _T(""))
		{
			pLabel->SetHyperLink(strLinkText);
			pLabel->SetLink(TRUE, FALSE);
			pLabel->SetLinkCursor(LoadCursor(NULL, IDC_HAND));
		}
		*/
		m_ArrTexts.Add(pLabel);
#endif
		node = node->NextSibling();
	}
	xmldoc.Clear();

}

void CBaseSkin::_ReLoadSkinText()
{
	TRACE("######_ReLoadSkinText######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;	
	node = node->FirstChild(DEF_TEXT);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_STATIC_DEF && nID < IDC_SLIDER_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinText::Node=%s, Name=%s\n", node->Value(), strCap);

		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strText			= ConvStr::utf8ToCstr(itemElement->Attribute("Text"));
		CString strLinkText		= ConvStr::utf8ToCstr(itemElement->Attribute("TextHyperLink"));
		bool bUnderline = false,
			bEllipsis = false;
		nret = itemElement->QueryBoolAttribute("Underline", &bUnderline);		
		nret = itemElement->QueryBoolAttribute("Ellipsis", &bEllipsis);
		int nLineSpace = 0;
		nret = itemElement->QueryIntAttribute("LineSpace", &nLineSpace);
		CString strAlign		= ConvStr::utf8ToCstr(itemElement->Attribute("HorzAlign"));
		bool bVertCenter = false;
		nret = itemElement->QueryBoolAttribute("VertCenter", &bVertCenter);		
		CString strFontName = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize		= FONT_SIZE;
		itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		bool bFontBold = false;
		nret = itemElement->QueryBoolAttribute(DEF_BOLD, &bFontBold);
		CString strFontColor	= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString	strBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));

		
		UINT nAlign = SS_LEFT;
		if (0 == strAlign.CompareNoCase(_T("RIGHT")))
			nAlign = SS_RIGHT;
		else if (0 == strAlign.CompareNoCase(_T("CENTER")))
			nAlign = SS_CENTER;

		CRect Rect_Pos			= skinUtil.StringToRect(strPos);
		COLORREF corFont		= skinUtil.StringToRGB(strFontColor);
		BOOL bTrans = TRUE;
		if (!strBkColor.IsEmpty()) bTrans = FALSE;
		COLORREF	clrBk	= skinUtil.StringToRGB(strBkColor);
#if USE_SKINLABEL
		CxSkinLabel* pLabel = FindText(nID);
		if(pLabel != NULL)
		{
			pLabel->SetVertCenter( bVertCenter);
			pLabel->SetEllipse( bEllipsis);
			pLabel->SetTransparent(bTrans);
			pLabel->SetBkColor(clrBk);
			pLabel->SetTextColor(corFont);
			pLabel->SetFontName(strFontName);
			pLabel->SetFontSize(nFontSize);
			pLabel->SetFontBold(bFontBold);
			pLabel->SetFontUnderline(bUnderline);
			pLabel->SetText(strText);
			pLabel->SetLineSpace(nLineSpace);
			if(!strLinkText.IsEmpty())
			{
				pLabel->SetHyperLink(strLinkText);
				pLabel->SetLink(TRUE, FALSE);
				pLabel->SetLinkCursor(LoadCursor(NULL, IDC_HAND));
			}
		}
#else
		CxSkinText* pLabel = FindText(nID);
		if(pLabel != NULL)
		{
			//pLabel->SetVertAlign( bValign);
			//pLabel->SetEllipse( bEllipsis);
			//pLabel->SetTransparent(bTrans);
			//pLabel->SetBkColor(clrBk);
			pLabel->SetTextColor(corFont);
			pLabel->SetFontName(strFontName);
			pLabel->SetFontSize(nFontSize);
			pLabel->SetFontBold(bFontBold);
			pLabel->SetFontUnderline(bUnderline);
			pLabel->SetText(strText);
			/*
			if(!strLinkText.IsEmpty())
			{
				pLabel->SetHyperLink(strLinkText);
				pLabel->SetLink(TRUE, FALSE);
				pLabel->SetLinkCursor(LoadCursor(NULL, IDC_HAND));
			}
			*/
		}
#endif
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

#if USE_SKINLABEL
CxSkinLabel* CBaseSkin::FindText(int id)
{
	int nSz = (int)m_ArrTexts.GetSize();
	for(int i = 0; i < nSz; i++)
	{
		CxSkinLabel* pLabel = m_ArrTexts.GetAt(i);
		if(pLabel->GetId() == id)
			return pLabel;
	}
	return NULL;
}
#else
CxSkinText* CBaseSkin::FindText(int id)
{
	int nSz = (int)m_ArrTexts.GetSize();
	for(int i = 0; i < nSz; i++)
	{
		CxSkinText* pLabel = m_ArrTexts.GetAt(i);
		if(pLabel->GetId() == id)
			return pLabel;
	}
	return NULL;
}
#endif

#if USE_SKINCOMBOBOX
void CBaseSkin::AttachComboBox()
{
	TRACE("######AttachComboBox######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_COMBOBOX);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_COMBO_DEF && nID < IDC_TEXT_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachComboBox::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strStyle		= ConvStr::utf8ToCstr(itemElement->Attribute("Style"));
		CString	strValue		= ConvStr::utf8ToCstr(itemElement->Attribute("Value"));
		int nEditHeight = 20;
		nret = itemElement->QueryIntAttribute("EditHeight", &nEditHeight);//콤보박스 에디트 높이 추가
		int nRowHeight = 20;
		int nMarginLeft  = 0, 
			nMarginTop  = 0, 
			nMarginRight = 0, 
			nMarginBottom = 0;
		nret = itemElement->QueryIntAttribute("RowHeight", &nRowHeight);//콤보박스 아이템 높이 추가
		nret = itemElement->QueryIntAttribute("MarginLeft", &nMarginLeft);
		nret = itemElement->QueryIntAttribute("MarginTop", &nMarginTop);
		nret = itemElement->QueryIntAttribute("MarginRight", &nMarginRight);
		nret = itemElement->QueryIntAttribute("MarginBottom", &nMarginBottom);
		bool bCustomList = false;
		nret = itemElement->QueryBoolAttribute("UseCustomList", &bCustomList);//커스텀 리스트
		CString strBoxLeft		= ConvStr::utf8ToCstr(itemElement->Attribute("BoxLeft"));
		CString strBoxCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxCenter"));
		CString strBoxRightNM	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightNormal"));
		CString strBoxRightOV	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightOver"));
		CString strBoxRightDN	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightDown"));
		CString strBoxRightDS	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightDisable"));
		CString strListLeft		= ConvStr::utf8ToCstr(itemElement->Attribute("ListLeft"));
		CString strListTop		= ConvStr::utf8ToCstr(itemElement->Attribute("ListTop"));
		CString strListBottom	= ConvStr::utf8ToCstr(itemElement->Attribute("ListBottom"));
		CString strListRight	= ConvStr::utf8ToCstr(itemElement->Attribute("ListRight"));		
		CString strFontName		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		CString strTxtClr		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strTxtFClr		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColorFocus"));
		CString strBkClr		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		CString strBkFClr		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColorFocus"));
		
		CRect Rect_Pos		= skinUtil.StringToRect(strPos);
		CRect Rect_BoxL		= skinUtil.StringToRect(strBoxLeft);
		CRect Rect_BoxC		= skinUtil.StringToRect(strBoxCenter);
		CRect Rect_BoxRNM	= skinUtil.StringToRect(strBoxRightNM);
		CRect Rect_BoxROV	= skinUtil.StringToRect(strBoxRightOV);
		CRect Rect_BoxRDN	= skinUtil.StringToRect(strBoxRightDN);
		CRect Rect_BoxRDS	= skinUtil.StringToRect(strBoxRightDS);
		CRect Rect_ListL	= skinUtil.StringToRect(strListLeft);
		CRect Rect_ListT	= skinUtil.StringToRect(strListTop);
		CRect Rect_ListB	= skinUtil.StringToRect(strListBottom);
		CRect Rect_ListR	= skinUtil.StringToRect(strListRight);
		COLORREF clrText	= skinUtil.StringToRGB(strTxtClr);
		COLORREF clrFText	= skinUtil.StringToRGB(strTxtFClr);
		COLORREF clrBk		= skinUtil.StringToRGB(strBkClr);
		COLORREF clrFBk		= skinUtil.StringToRGB(strBkFClr);

		UINT nDrawMode = DRAWMODE_IMAGE;
		if(0 == strStyle.CompareNoCase(_T(DEF_RGB)))
			nDrawMode = DRAWMODE_RGB;

		CxSkinComboBox* pCombo = new CxSkinComboBox;	
		BOOL ret = pCombo->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pCombo;
			node = node->NextSibling();
			continue;
		}
		
		pCombo->SetDrawMode(nDrawMode);
		pCombo->CopyFrom(m_bmSkinImage, Rect_BoxL, Rect_BoxC, Rect_BoxRNM, Rect_BoxROV, Rect_BoxRDN, Rect_BoxRDS, Rect_ListL, Rect_ListT, Rect_ListB, Rect_ListR);
		pCombo->SetMargin(nMarginLeft, nMarginTop, nMarginBottom, nMarginRight);
		pCombo->SetDrawMode(DRAWMODE_IMAGE, bCustomList);
		pCombo->SetNormalColor(clrBk, clrText);
		pCombo->SetFocusColor(clrFBk, clrFText);
		pCombo->SetFontName(strFontName);
		pCombo->SetFontSize(nFontSize);
		pCombo->SetEditHeight(nEditHeight);
		pCombo->SetRowHeight(nRowHeight);
		pCombo->Init();		
		pCombo->SetId(nID);
		pCombo->SetValue(strValue);
		pCombo->SetCaption(strCap);

		m_ArrCombo.Add(pCombo);

		node = node->NextSibling();
	}
	xmldoc.Clear();
}

void CBaseSkin::_ReLoadSkinComboBox()
{
	TRACE("######_ReLoadSkinComboBox######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_COMBOBOX);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_COMBO_DEF && nID < IDC_TEXT_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinComboBox::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strStyle		= ConvStr::utf8ToCstr(itemElement->Attribute("Style"));
		int nEditHeight = 20;
		nret = itemElement->QueryIntAttribute("EditHeight", &nEditHeight);//콤보박스 에디트 높이 추가
		int nRowHeight = 20;
		int nMarginLeft = 0,
			nMarginTop = 0,
			nMarginRight = 0,
			nMarginBottom = 0;
		nret = itemElement->QueryIntAttribute("RowHeight", &nRowHeight);//콤보박스 아이템 높이 추가
		nret = itemElement->QueryIntAttribute("MarginLeft", &nMarginLeft);
		nret = itemElement->QueryIntAttribute("MarginTop", &nMarginTop);
		nret = itemElement->QueryIntAttribute("MarginRight", &nMarginRight);
		nret = itemElement->QueryIntAttribute("MarginBottom", &nMarginBottom);
		bool bCustomList = false;
		nret = itemElement->QueryBoolAttribute("UseCustomList", &bCustomList);//커스텀 리스트
		CString strBoxLeft		= ConvStr::utf8ToCstr(itemElement->Attribute("BoxLeft"));
		CString strBoxCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxCenter"));
		CString strBoxRightNM	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightNormal"));
		CString strBoxRightOV	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightOver"));
		CString strBoxRightDN	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightDown"));
		CString strBoxRightDS	= ConvStr::utf8ToCstr(itemElement->Attribute("BoxRightDisable"));
		CString strListLeft		= ConvStr::utf8ToCstr(itemElement->Attribute("ListLeft"));
		CString strListTop		= ConvStr::utf8ToCstr(itemElement->Attribute("ListTop"));
		CString strListBottom	= ConvStr::utf8ToCstr(itemElement->Attribute("ListBottom"));
		CString strListRight	= ConvStr::utf8ToCstr(itemElement->Attribute("ListRight"));		
		CString strFontName		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		CString strTxtClr		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strTxtFClr		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColorFocus"));
		CString strBkClr		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		CString strBkFClr		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColorFocus"));
		
		CRect Rect_Pos		= skinUtil.StringToRect(strPos);
		CRect Rect_BoxL		= skinUtil.StringToRect(strBoxLeft);
		CRect Rect_BoxC		= skinUtil.StringToRect(strBoxCenter);
		CRect Rect_BoxRNM	= skinUtil.StringToRect(strBoxRightNM);
		CRect Rect_BoxROV	= skinUtil.StringToRect(strBoxRightOV);
		CRect Rect_BoxRDN	= skinUtil.StringToRect(strBoxRightDN);
		CRect Rect_BoxRDS	= skinUtil.StringToRect(strBoxRightDS);
		CRect Rect_ListL	= skinUtil.StringToRect(strListLeft);
		CRect Rect_ListT	= skinUtil.StringToRect(strListTop);
		CRect Rect_ListB	= skinUtil.StringToRect(strListBottom);
		CRect Rect_ListR	= skinUtil.StringToRect(strListRight);
		COLORREF clrText	= skinUtil.StringToRGB(strTxtClr);
		COLORREF clrFText	= skinUtil.StringToRGB(strTxtFClr);
		COLORREF clrBk		= skinUtil.StringToRGB(strBkClr);
		COLORREF clrFBk		= skinUtil.StringToRGB(strBkFClr);

		UINT nDrawMode = DRAWMODE_IMAGE;
		if(0 == strStyle.CompareNoCase(_T(DEF_RGB)))
			nDrawMode = DRAWMODE_RGB;

		CxSkinComboBox* pCombo = FindComboBox(nID);
		if(pCombo != NULL)
		{
			pCombo->CopyFrom(m_bmSkinImage, Rect_BoxL, Rect_BoxC, Rect_BoxRNM, Rect_BoxROV, Rect_BoxRDN, Rect_BoxRDS, Rect_ListL, Rect_ListT, Rect_ListB, Rect_ListR);

			pCombo->SetMargin(nMarginLeft, nMarginTop, nMarginBottom, nMarginRight);
			pCombo->SetNormalColor(clrBk, clrText);
			pCombo->SetFocusColor(clrFBk, clrFText);
			pCombo->SetFontName(strFontName);
			pCombo->SetFontSize(nFontSize);
			pCombo->Init();
		}

		node = node->NextSibling();
	}
	xmldoc.Clear();
}

CxSkinComboBox* CBaseSkin::FindComboBox(int id)
{
	int nSz = (int)m_ArrCombo.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CxSkinComboBox* pCombo = m_ArrCombo.GetAt(j);
		if(pCombo->GetId() == id)
		{
			return pCombo;
		}
	}
	return NULL;
}
#endif
void CBaseSkin::AttachStatic()
{
	TRACE("######AttachStatic######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_STATIC);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_STATIC_DEF && nID < IDC_SLIDER_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachStatic::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strTopLeft		= ConvStr::utf8ToCstr(itemElement->Attribute("TopLeft"));
		CString strTopCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("TopCenter"));
		CString strTopRight		= ConvStr::utf8ToCstr(itemElement->Attribute("TopRight"));
		CString strMiddleLeft	= ConvStr::utf8ToCstr(itemElement->Attribute("MiddleLeft"));
		CString strMiddleCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("MiddleCenter"));
		CString strMiddleRight	= ConvStr::utf8ToCstr(itemElement->Attribute("MiddleRight"));
		CString strBottomLeft	= ConvStr::utf8ToCstr(itemElement->Attribute("BottomLeft"));
		CString strBottomCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("BottomCenter"));
		CString strBottomRight	= ConvStr::utf8ToCstr(itemElement->Attribute("BottomRight"));
		bool bPattern = false;
		nret = itemElement->QueryBoolAttribute("UsePattern", &bPattern);

		CRect Rect_Pos			= skinUtil.StringToRect(strPos);
		CRect Rect_TopLeft		= skinUtil.StringToRect(strTopLeft);
		CRect Rect_TopCenter	= skinUtil.StringToRect(strTopCenter);
		CRect Rect_TopRight		= skinUtil.StringToRect(strTopRight);
		CRect Rect_MiddleLeft	= skinUtil.StringToRect(strMiddleLeft);
		CRect Rect_MiddleCenter	= skinUtil.StringToRect(strMiddleCenter);
		CRect Rect_MiddleRight	= skinUtil.StringToRect(strMiddleRight);
		CRect Rect_BottomLeft	= skinUtil.StringToRect(strBottomLeft);
		CRect Rect_BottomCenter	= skinUtil.StringToRect(strBottomCenter);
		CRect Rect_BottomRight	= skinUtil.StringToRect(strBottomRight);

		CxSkinStatic* pStatic = new CxSkinStatic;
		BOOL ret = pStatic->Create(NULL, WS_VISIBLE | WS_TABSTOP, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pStatic;
			node = node->NextSibling();
			continue;
		}
		
		pStatic->SetId(nID);
		
		if(!strTopLeft.IsEmpty())
		{
			pStatic->SetDrawPattern(bPattern);
			pStatic->CopyFromBG(m_bmSkinImage, Rect_TopLeft, Rect_TopCenter, Rect_TopRight, Rect_MiddleLeft,
				Rect_MiddleCenter, Rect_MiddleRight, Rect_BottomLeft, Rect_BottomCenter, Rect_BottomRight);

			if(strTopCenter	!= _T("") && strTopRight != _T("") && 		
				strMiddleLeft != _T("") && strMiddleCenter != _T("") && strMiddleRight != _T("") &&
				strBottomLeft != _T("") && strBottomCenter != _T("") && strBottomRight != _T("")){
					pStatic->SetResizeType(RESIZEF);
			}else if(strTopCenter	== _T("") && strTopRight == _T("") && 		
				strMiddleLeft != _T("") && strMiddleCenter == _T("") && strMiddleRight == _T("") &&
				strBottomLeft != _T("") && strBottomCenter == _T("") && strBottomRight == _T("")){
					pStatic->SetResizeType(RESIZEV);
			}else if(strTopCenter	!= _T("") && strTopRight != _T("") && 		
				strMiddleLeft == _T("") && strMiddleCenter == _T("") && strMiddleRight == _T("") &&
				strBottomLeft == _T("") && strBottomCenter == _T("") && strBottomRight == _T("")){
					pStatic->SetResizeType(RESIZEH);
			}else{
				pStatic->SetResizeType(NOSIZE);
			}
		}
		pStatic->SetCaption(strCap);
		m_ArrStatics.Add(pStatic);

		node = node->NextSibling();
	}
	xmldoc.Clear();
}

void CBaseSkin::_ReLoadSkinStatic()
{
	TRACE("######_ReleadSkinStatic######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_STATIC);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_STATIC_DEF && nID < IDC_SLIDER_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinStatic::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strTopLeft		= ConvStr::utf8ToCstr(itemElement->Attribute("TopLeft"));
		CString strTopCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("TopCenter"));
		CString strTopRight		= ConvStr::utf8ToCstr(itemElement->Attribute("TopRight"));
		CString strMiddleLeft	= ConvStr::utf8ToCstr(itemElement->Attribute("MiddleLeft"));
		CString strMiddleCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("MiddleCenter"));
		CString strMiddleRight	= ConvStr::utf8ToCstr(itemElement->Attribute("MiddleRight"));
		CString strBottomLeft	= ConvStr::utf8ToCstr(itemElement->Attribute("BottomLeft"));
		CString strBottomCenter	= ConvStr::utf8ToCstr(itemElement->Attribute("BottomCenter"));
		CString strBottomRight	= ConvStr::utf8ToCstr(itemElement->Attribute("BottomRight"));
		bool bPattern = false;
		nret = itemElement->QueryBoolAttribute("UsePattern", &bPattern);

		CRect Rect_Pos			= skinUtil.StringToRect(strPos);
		CRect Rect_TopLeft		= skinUtil.StringToRect(strTopLeft);
		CRect Rect_TopCenter	= skinUtil.StringToRect(strTopCenter);
		CRect Rect_TopRight		= skinUtil.StringToRect(strTopRight);
		CRect Rect_MiddleLeft	= skinUtil.StringToRect(strMiddleLeft);
		CRect Rect_MiddleCenter	= skinUtil.StringToRect(strMiddleCenter);
		CRect Rect_MiddleRight	= skinUtil.StringToRect(strMiddleRight);
		CRect Rect_BottomLeft	= skinUtil.StringToRect(strBottomLeft);
		CRect Rect_BottomCenter	= skinUtil.StringToRect(strBottomCenter);
		CRect Rect_BottomRight	= skinUtil.StringToRect(strBottomRight);

		CxSkinStatic* pStatic = FindStatic(nID);
		if(pStatic != NULL)
		{
			if(!strTopLeft.IsEmpty())
			{
				pStatic->CopyFromBG(m_bmSkinImage, Rect_TopLeft, Rect_TopCenter, Rect_TopRight, Rect_MiddleLeft,
					Rect_MiddleCenter, Rect_MiddleRight, Rect_BottomLeft, Rect_BottomCenter, Rect_BottomRight);
			}
		}
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

CxSkinStatic* CBaseSkin::FindStatic(int id)
{
	int nSz = (int)m_ArrStatics.GetSize();
	for(int j = 0; j < nSz; j++){
		CxSkinStatic* pStatic = m_ArrStatics.GetAt(j);
		if(pStatic->GetId() == id)
		{
			return pStatic;
		}
	}
	return NULL;
}

void CBaseSkin::AttachEdit()
{
	TRACE("######AttachEdit######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_EDIT);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_EDIT_DEF && nID < IDC_COMBO_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachEdit::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strText			= ConvStr::utf8ToCstr(itemElement->Attribute("Value"));			
		CString strFrameColor		= ConvStr::utf8ToCstr(itemElement->Attribute("OutlineColor"));
		CString strFontColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FontColorFocus"));
		CString strBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		CString	strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColorFocus"));
		CString strFontName			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize = FONT_SIZE;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		bool bFontBold= false;
		nret = itemElement->QueryBoolAttribute(DEF_BOLD, &bFontBold);
		bool bPassword = false,
			bNumeric = false;
		nret = itemElement->QueryBoolAttribute("PasswordStyle", &bPassword);		
		nret = itemElement->QueryBoolAttribute("NumericStyle", &bNumeric);
		bool bMultiline = false;
		nret = itemElement->QueryBoolAttribute("MultiLine", &bMultiline);
		bool bReadonly = false;
		nret = itemElement->QueryBoolAttribute("ReadOnly", &bReadonly);
		bool bBorder = false;
		nret = itemElement->QueryBoolAttribute("Border", &bBorder);
		bool bAutowrap = false;
		nret = itemElement->QueryBoolAttribute("AutoWrap", &bAutowrap);
		bool bTransThumb = false;
		nret = itemElement->QueryBoolAttribute("TransparentThumb", &bTransThumb);
		
		
		//Scrollbar
		CString strGripColor		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_COLOR));
		CString strGripLineColor	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_LINE));
		COLORREF clrGripBk			= skinUtil.StringToRGB(strGripColor);
		COLORREF clrGripOutline		= skinUtil.StringToRGB(strGripLineColor);
		//Vertical button
		CString strVertTopBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_NRECT));
		CString strVertTopBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_ORECT));
		CString strVertTopBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_DRECT));
		CString strVertBtmBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_NRECT));
		CString strVertBtmBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_ORECT));
		CString strVertBtmBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_DRECT));
		//Vertical Thumb
		CString strVertThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_NRECT));
		CString strVertThumbTopOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_ORECT));
		CString strVertThumbTopDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_DRECT));
		CString strVertThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_NRECT));
		CString strVertThumbMidOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_ORECT));
		CString strVertThumbMidDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_DRECT));
		CString strVertThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_NRECT));
		CString strVertThumbBtmOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_ORECT));
		CString strVertThumbBtmDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_DRECT));
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CString strVertChanRect				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_RECT));
#else
		CString strVertChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_TRECT));
		CString strVertChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_MRECT));
		CString strVertChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_BRECT));
#endif
		//horizontal button
		CString strHorizLeftBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_NRECT));
		CString strHorizLeftBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_ORECT));
		CString strHorizLeftBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_DRECT));
		CString strHorizRightBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_NRECT));
		CString strHorizRightBtnOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RMTN_ORECT));
		CString strHorizRightBtnDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_DRECT));
		//horizontal Thumb
		CString strHorizThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_NRECT));
		CString strHorizThumbTopOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_ORECT));
		CString strHorizThumbTopDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_DRECT));
		CString strHorizThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_NRECT));
		CString strHorizThumbMidOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_ORECT));
		CString strHorizThumbMidDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_DRECT));
		CString strHorizThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_NRECT));
		CString strHorizThumbBtmOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_ORECT));
		CString strHorizThumbBtmDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_DRECT));
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CString strHorizChanRect			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_RECT));
#else
		CString strHorizChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_TRECT));
		CString strHorizChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_MRECT));
		CString strHorizChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_BRECT));
#endif
		
		int nMinThumbSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_SB_HORZ_THUMB_MIN_SIZE, &nMinThumbSize);
		//Vertical button
		CRect Rect_VertTBN		= skinUtil.StringToRect(strVertTopBtnNormalRect);
		CRect Rect_VertTBO		= skinUtil.StringToRect(strVertTopBtnOverRect);
		CRect Rect_VertTBD		= skinUtil.StringToRect(strVertTopBtnDownRect);
		CRect Rect_VertBBN		= skinUtil.StringToRect(strVertBtmBtnNormalRect);
		CRect Rect_VertBBO		= skinUtil.StringToRect(strVertBtmBtnOverRect);
		CRect Rect_VertBBD		= skinUtil.StringToRect(strVertBtmBtnDownRect);
		//Vertical Thumb
		CRect Rect_VertTHTN		= skinUtil.StringToRect(strVertThumbTopNormalRect);
		CRect Rect_VertTHTO		= skinUtil.StringToRect(strVertThumbTopOverRect);
		CRect Rect_VertTHTD		= skinUtil.StringToRect(strVertThumbTopDownRect);
		CRect Rect_VertTHMN		= skinUtil.StringToRect(strVertThumbMidNormalRect);
		CRect Rect_VertTHMO		= skinUtil.StringToRect(strVertThumbMidOverRect);
		CRect Rect_VertTHMD		= skinUtil.StringToRect(strVertThumbMidDownRect);
		CRect Rect_VertTHBN		= skinUtil.StringToRect(strVertThumbBtmNormalRect);
		CRect Rect_VertTHBO		= skinUtil.StringToRect(strVertThumbBtmOverRect);
		CRect Rect_VertTHBD		= skinUtil.StringToRect(strVertThumbBtmDownRect);
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CRect Rect_VertCH		= skinUtil.StringToRect(strVertChanRect);
#else
		CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
		CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
		CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);
#endif
		//horizontal button
		CRect Rect_HorizLBN		= skinUtil.StringToRect(strHorizLeftBtnNormalRect);
		CRect Rect_HorizLBO		= skinUtil.StringToRect(strHorizLeftBtnOverRect);
		CRect Rect_HorizLBD		= skinUtil.StringToRect(strHorizLeftBtnDownRect);
		CRect Rect_HorizRBN		= skinUtil.StringToRect(strHorizRightBtnNormalRect);
		CRect Rect_HorizRBO		= skinUtil.StringToRect(strHorizRightBtnOverRect);
		CRect Rect_HorizRBD		= skinUtil.StringToRect(strHorizRightBtnDownRect);
		//horizontal Thumb
		CRect Rect_HorizTHTN		= skinUtil.StringToRect(strHorizThumbTopNormalRect);
		CRect Rect_HorizTHTO		= skinUtil.StringToRect(strHorizThumbTopOverRect);
		CRect Rect_HorizTHTD		= skinUtil.StringToRect(strHorizThumbTopDownRect);
		CRect Rect_HorizTHMN		= skinUtil.StringToRect(strHorizThumbMidNormalRect);
		CRect Rect_HorizTHMO		= skinUtil.StringToRect(strHorizThumbMidOverRect);
		CRect Rect_HorizTHMD		= skinUtil.StringToRect(strHorizThumbMidDownRect);
		CRect Rect_HorizTHBN		= skinUtil.StringToRect(strHorizThumbBtmNormalRect);
		CRect Rect_HorizTHBO		= skinUtil.StringToRect(strHorizThumbBtmOverRect);
		CRect Rect_HorizTHBD		= skinUtil.StringToRect(strHorizThumbBtmDownRect);
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CRect Rect_HorizCH		= skinUtil.StringToRect(strHorizChanRect);
#else
		CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
		CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
		CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);
#endif
		CRect Rect_Pos = skinUtil.StringToRect(strPos);

		COLORREF clrFont	= skinUtil.StringToRGB(strFontColor);
		COLORREF clrFrame	= skinUtil.StringToRGB(strFrameColor);
		COLORREF clrBk		= skinUtil.StringToRGB(strBkColor);
		COLORREF clrFocusBk = skinUtil.StringToRGB(strFocusBkColor);
		COLORREF clrFocusText = skinUtil.StringToRGB(strFocusTextColor);

		if(strFocusBkColor.IsEmpty()) clrFocusBk = clrBk;
		if(strFocusTextColor.IsEmpty()) clrFocusText = clrFont;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;/*| ES_AUTOHSCROLL;*/

		if (bPassword){
			dwStyle |= ES_AUTOHSCROLL | ES_PASSWORD;
			bAutowrap = false;
			bMultiline = false;
		}
		if(!bAutowrap)
			dwStyle |= ES_AUTOHSCROLL;
		if(bMultiline)
			dwStyle |= ES_WANTRETURN;

#if USE_SCINTILLAWND
		CScintillaWnd* pEdit = new CScintillaWnd;

		BOOL ret = pEdit->Create(_T("Edit"), dwStyle, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE)
		{
			delete pEdit;
			node = node->NextSibling();
			continue;
		}
		
		pEdit->SetId(nID);
		pEdit->SetForeground(0, clrFont);
		pEdit->SetBackground(0, clrBk);

		char *szFont = NULL;
		szFont = ConvStr::unicodeToMbcs(strFontName);			
		free(szFont);

		pEdit->SetFontname(32, szFont);

		pEdit->SetFontheight(32, nFontSize);

		pEdit->SetSelForeground(1, clrFocusText);
		pEdit->SetSelBackground(1, clrFocusBk);

		bool bShowHScroll = false;
		bool bShowVScroll = false;

		if(bMultiline)
		{
			bShowHScroll = true;
			bShowVScroll = true;
		}
		pEdit->SetScrollbar(bShowHScroll, bShowVScroll);
		pEdit->SetMultiline(bMultiline);

		int nCodepage = ::GetACP();
		pEdit->SetCodepage(nCodepage);

		pEdit->SetWindowText(strText);
		pEdit->Init();
		pEdit->SetCaption(strCap);

		m_ArrEdit.Add(pEdit);
#elif USE_SKINEDIT
		CxSkinEdit* pEdit = new CxSkinEdit;
		BOOL ret = pEdit->Create(dwStyle, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE)
		{
			delete pEdit;
			node = node->NextSibling();
			continue;
		}		
		pEdit->SetId(nID);		
		pEdit->SetFontName(strFontName);
		pEdit->SetFontSize(nFontSize);
		pEdit->SetFontBold(bFontBold);
		pEdit->SetShowScrollbar(bMultiline);
		pEdit->SetBorder(bBorder);
		pEdit->SetTextColor(clrFont, clrFocusText);
		pEdit->SetBGColor(clrBk, clrFocusBk);
		pEdit->SetLineColor(clrFrame, clrFrame);
		pEdit->SetScrollGripColor(clrGripBk, clrGripOutline);
		pEdit->SetReadOnlyState(bReadonly);		
		pEdit->SetNumeric(bNumeric);
		pEdit->SetMinDigits(0);
		pEdit->SetMaxDigits(100);
		pEdit->SetShowHorizScrollBar(!bAutowrap);
		pEdit->SetTransparentThumb(bTransThumb);
		pEdit->SetCaption(strCap);
#if USE_TRANSPARENTBAR
		pEdit->CopyVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertCH);
#else
		pEdit->CopyVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
#if USE_TRANSPARENTBAR
		pEdit->CopyHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizCH);
#else
		pEdit->CopyHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif
		pEdit->Init();

		strText.Replace(_T("\\n"), _T("\n"));
		strText.Replace(_T("\\r"), _T("\r"));
		pEdit->SetWindowText(strText);
		m_ArrEdit.Add(pEdit);

#else
		CInplaceEdit* cEdit = new CInplaceEdit;
		cEdit->Create(dwStyle, Rect_Pos, m_pWnd, nID);		
		//cEdit->Create(WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Rect_Pos, m_pWnd, nID);
		cEdit->SetMargins(0,0);

		cEdit->SetFontColor(corFontRGB);
		cEdit->SetBGColor(corBgRGB);
		cEdit->SetFrameColor(corFrRGB);
		cEdit->SetFontType(strFont, corFontRGB, nFontSize, bBold);

		m_ArrEdit.Add(cEdit);
#endif	
		node = node->NextSibling();//다음버튼으로
	}
	xmldoc.Clear();
}

void CBaseSkin::_ReLoadSkinEdit()
{
	TRACE("######_ReLoadSkinEdit######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;	
	node = node->FirstChild(DEF_EDIT);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_EDIT_DEF && nID < IDC_COMBO_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinEdit::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strText			= ConvStr::utf8ToCstr(itemElement->Attribute("Value"));			
		CString strFrameColor		= ConvStr::utf8ToCstr(itemElement->Attribute("OutlineColor"));
		CString strFontColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FontColorFocus"));
		CString strBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		CString	strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColorFocus"));
		CString strFontName			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		bool bFontBold= false;
		nret = itemElement->QueryBoolAttribute(DEF_BOLD, &bFontBold);
		bool bPassword=false,
			bNumeric = false;
		nret = itemElement->QueryBoolAttribute("PasswordStyle", &bPassword);		
		nret = itemElement->QueryBoolAttribute("NumericStyle", &bNumeric);
		bool bMultiline = false;
		nret = itemElement->QueryBoolAttribute("MultiLine", &bMultiline);
		bool bReadonly = false;
		nret = itemElement->QueryBoolAttribute("ReadOnly", &bReadonly);
		bool bBorder = false;
		nret = itemElement->QueryBoolAttribute("Border", &bBorder);
		bool bAutowrap = false;
		nret = itemElement->QueryBoolAttribute("AutoWrap", &bAutowrap);
		bool bTransThumb = false;
		nret = itemElement->QueryBoolAttribute("TransparentThumb", &bTransThumb);
		//Scrollbar
		CString strGripColor		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_COLOR));
		CString strGripLineColor	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_LINE));
		COLORREF clrGripBk			= skinUtil.StringToRGB(strGripColor);
		COLORREF clrGripOutline		= skinUtil.StringToRGB(strGripLineColor);
		//Vertical button
		CString strVertTopBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_NRECT));
		CString strVertTopBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_ORECT));
		CString strVertTopBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_DRECT));
		CString strVertBtmBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_NRECT));
		CString strVertBtmBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_ORECT));
		CString strVertBtmBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_DRECT));
		//Vertical Thumb
		CString strVertThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_NRECT));
		CString strVertThumbTopOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_ORECT));
		CString strVertThumbTopDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_DRECT));
		CString strVertThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_NRECT));
		CString strVertThumbMidOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_ORECT));
		CString strVertThumbMidDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_DRECT));
		CString strVertThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_NRECT));
		CString strVertThumbBtmOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_ORECT));
		CString strVertThumbBtmDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_DRECT));
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CString strVertChanRect				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_RECT));
#else
		CString strVertChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_TRECT));
		CString strVertChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_MRECT));
		CString strVertChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_BRECT));
#endif
		//horizontal button
		CString strHorizLeftBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_NRECT));
		CString strHorizLeftBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_ORECT));
		CString strHorizLeftBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_DRECT));
		CString strHorizRightBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_NRECT));
		CString strHorizRightBtnOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RMTN_ORECT));
		CString strHorizRightBtnDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_DRECT));
		//horizontal Thumb
		CString strHorizThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_NRECT));
		CString strHorizThumbTopOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_ORECT));
		CString strHorizThumbTopDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_DRECT));
		CString strHorizThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_NRECT));
		CString strHorizThumbMidOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_ORECT));
		CString strHorizThumbMidDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_DRECT));
		CString strHorizThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_NRECT));
		CString strHorizThumbBtmOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_ORECT));
		CString strHorizThumbBtmDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_DRECT));
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CString strHorizChanRect			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_RECT));
#else
		CString strHorizChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_TRECT));
		CString strHorizChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_MRECT));
		CString strHorizChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_BRECT));
#endif

		int nMinThumbSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_SB_HORZ_THUMB_MIN_SIZE, &nMinThumbSize);
		//Vertical button
		CRect Rect_VertTBN		= skinUtil.StringToRect(strVertTopBtnNormalRect);
		CRect Rect_VertTBO		= skinUtil.StringToRect(strVertTopBtnOverRect);
		CRect Rect_VertTBD		= skinUtil.StringToRect(strVertTopBtnDownRect);
		CRect Rect_VertBBN		= skinUtil.StringToRect(strVertBtmBtnNormalRect);
		CRect Rect_VertBBO		= skinUtil.StringToRect(strVertBtmBtnOverRect);
		CRect Rect_VertBBD		= skinUtil.StringToRect(strVertBtmBtnDownRect);
		//Vertical Thumb
		CRect Rect_VertTHTN		= skinUtil.StringToRect(strVertThumbTopNormalRect);
		CRect Rect_VertTHTO		= skinUtil.StringToRect(strVertThumbTopOverRect);
		CRect Rect_VertTHTD		= skinUtil.StringToRect(strVertThumbTopDownRect);
		CRect Rect_VertTHMN		= skinUtil.StringToRect(strVertThumbMidNormalRect);
		CRect Rect_VertTHMO		= skinUtil.StringToRect(strVertThumbMidOverRect);
		CRect Rect_VertTHMD		= skinUtil.StringToRect(strVertThumbMidDownRect);
		CRect Rect_VertTHBN		= skinUtil.StringToRect(strVertThumbBtmNormalRect);
		CRect Rect_VertTHBO		= skinUtil.StringToRect(strVertThumbBtmOverRect);
		CRect Rect_VertTHBD		= skinUtil.StringToRect(strVertThumbBtmDownRect);
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CRect Rect_VertCH		= skinUtil.StringToRect(strVertChanRect);
#else
		CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
		CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
		CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);
#endif
		//horizontal button
		CRect Rect_HorizLBN		= skinUtil.StringToRect(strHorizLeftBtnNormalRect);
		CRect Rect_HorizLBO		= skinUtil.StringToRect(strHorizLeftBtnOverRect);
		CRect Rect_HorizLBD		= skinUtil.StringToRect(strHorizLeftBtnDownRect);
		CRect Rect_HorizRBN		= skinUtil.StringToRect(strHorizRightBtnNormalRect);
		CRect Rect_HorizRBO		= skinUtil.StringToRect(strHorizRightBtnOverRect);
		CRect Rect_HorizRBD		= skinUtil.StringToRect(strHorizRightBtnDownRect);
		//horizontal Thumb
		CRect Rect_HorizTHTN		= skinUtil.StringToRect(strHorizThumbTopNormalRect);
		CRect Rect_HorizTHTO		= skinUtil.StringToRect(strHorizThumbTopOverRect);
		CRect Rect_HorizTHTD		= skinUtil.StringToRect(strHorizThumbTopDownRect);
		CRect Rect_HorizTHMN		= skinUtil.StringToRect(strHorizThumbMidNormalRect);
		CRect Rect_HorizTHMO		= skinUtil.StringToRect(strHorizThumbMidOverRect);
		CRect Rect_HorizTHMD		= skinUtil.StringToRect(strHorizThumbMidDownRect);
		CRect Rect_HorizTHBN		= skinUtil.StringToRect(strHorizThumbBtmNormalRect);
		CRect Rect_HorizTHBO		= skinUtil.StringToRect(strHorizThumbBtmOverRect);
		CRect Rect_HorizTHBD		= skinUtil.StringToRect(strHorizThumbBtmDownRect);
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CRect Rect_HorizCH		= skinUtil.StringToRect(strHorizChanRect);
#else
		CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
		CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
		CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);
#endif
		CRect Rect_Pos = skinUtil.StringToRect(strPos);

		COLORREF clrFont	= skinUtil.StringToRGB(strFontColor);
		COLORREF clrFrame	= skinUtil.StringToRGB(strFrameColor);
		COLORREF clrBk		= skinUtil.StringToRGB(strBkColor);
		COLORREF clrFocusBk = skinUtil.StringToRGB(strFocusBkColor);
		COLORREF clrFocusText = skinUtil.StringToRGB(strFocusTextColor);
		
		if(strFocusBkColor.IsEmpty()) clrFocusBk = clrBk;
		if(strFocusTextColor.IsEmpty()) clrFocusText = clrFont;

		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;/*| ES_AUTOHSCROLL;*/

		if (bPassword){
			dwStyle |= ES_AUTOHSCROLL | ES_PASSWORD;
			bAutowrap = false;
			bMultiline = false;
		}
		if(!bAutowrap)
			dwStyle |= ES_AUTOHSCROLL;
		if(bMultiline)
			dwStyle |= ES_WANTRETURN;

#if USE_SCINTILLAWND
		CScintillaWnd* pEdit = FindEdit(nID);

		if(pEdit != NULL)
		{
			pEdit->SetForeground(1, clrFont);
			pEdit->SetBackground(1, clrBk);

			char *szFont = NULL;
			szFont = ConvStr::unicodeToMbcs(strFontName);
			free(szFont);

			pEdit->SetFontname(1, szFont);

			pEdit->SetFontheight(1, nFontSize);

			pEdit->SetSelForeground(1, clrFocusText);
			pEdit->SetSelBackground(1, clrFocusBk);

			bool bShowHScroll = false;
			bool bShowVScroll = false;

			if(bMultiline)
			{
				bShowHScroll = true;
				bShowVScroll = true;
			}
			pEdit->SetScrollbar(bShowHScroll, bShowVScroll);
			pEdit->SetMultiline(bMultiline);

			int nCodepage = ::GetACP();
			pEdit->SetCodepage(nCodepage);

			pEdit->SetWindowText(strText);
		}
#elif USE_SKINEDIT
		CxSkinEdit* pEdit = FindEdit(nID);
		if(pEdit != NULL)
		{
			pEdit->SetFontName(strFontName);
			pEdit->SetFontSize(nFontSize);
			pEdit->SetFontBold(bFontBold);			
			pEdit->SetBorder(bBorder);
			pEdit->SetTextColor(clrFont, clrFocusText);
			pEdit->SetBGColor(clrBk, clrFocusBk);
			pEdit->SetLineColor(clrFrame, clrFrame);
			pEdit->SetReadOnlyState(bReadonly);
			pEdit->SetWindowText(strText);
		}
#if USE_TRANSPARENTBAR
		pEdit->CopyVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertCH);
#else
		pEdit->CopyVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
#if USE_TRANSPARENTBAR
		pEdit->CopyHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizCH);
#else
		pEdit->CopyHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif

#else
		CInplaceEdit* pEdit = FindEdit(nID);
		if(pEdit != NULL)
		{
			pEdit->SetFontColor(corFontRGB);
			pEdit->SetBGColor(corBgRGB);
			pEdit->SetFrameColor(corFrRGB);
			pEdit->SetFontType(strFont, corFontRGB, nFontSize, bBold);
		}
#endif
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

#if USE_SCINTILLAWND
CScintillaWnd* CBaseSkin::FindEdit(int id)
{
	int nSz = (int)m_ArrEdit.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CScintillaWnd* pEdit = m_ArrEdit.GetAt(j);
		if(pEdit->GetId() == id)
		{
			return pEdit;
		}
	}
	return NULL;
}
#elif USE_SKINEDIT
CxSkinEdit* CBaseSkin::FindEdit(int id)
{
	int nSz = (int)m_ArrEdit.GetSize();
	for(int j = 0; j < nSz; j++)
	{	
		CxSkinEdit* pEdit = m_ArrEdit.GetAt(j);
		if(pEdit->GetId() == id)
		{
			return pEdit;
		}			
	}
	return NULL;
}
#else
CInplaceEdit* CBaseSkin::FindEdit(int id)
{
	int nSz = (int)m_ArrEdit.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CInplaceEdit* pEdit = m_ArrEdit.GetAt(j);
		if(pEdit->GetId() == id)
		{
			return pEdit;
		}
	}
	return NULL;
}
#endif

void CBaseSkin::AttachRndSlider()
{
		TRACE("######AttachRndSlider######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_RSLIDER);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_SLIDER_DEF && nID < IDC_EDIT_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachRndSlider::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		int nMax = 0,
			nProgress = 0;
		nret = itemElement->QueryIntAttribute("Max", &nMax);
		nret = itemElement->QueryIntAttribute("Progress", &nProgress);
		CString strThumb		= ConvStr::utf8ToCstr(itemElement->Attribute("Thumb"));
		CString strThumbMask	= ConvStr::utf8ToCstr(itemElement->Attribute("ThumbMask"));
		CString strThumbActive	= ConvStr::utf8ToCstr(itemElement->Attribute("ThumbActive"));
		CString strBk			= ConvStr::utf8ToCstr(itemElement->Attribute("Bk"));
		bool bThumbLock = false;
		nret = itemElement->QueryBoolAttribute("ThumbLock", &bThumbLock);
		bool bInvert = false;
		nret = itemElement->QueryBoolAttribute("Invert", &bInvert);

		int nCenterX = 0, nCenterY = 0;
		nret = itemElement->QueryIntAttribute("CenterX", &nCenterX);
		nret = itemElement->QueryIntAttribute("CenterY", &nCenterY);
		int nDiameter = 0;//지름
		nret = itemElement->QueryIntAttribute("Diameter", &nDiameter);
		int nZero = 0;//시작
		nret = itemElement->QueryIntAttribute("Zero", &nZero);
		bool bShowText = false;
		nret = itemElement->QueryBoolAttribute("ShowText", &bShowText);
		
		CRect Rect_Pos			= skinUtil.StringToRect(strPos);
		CRect Rect_Thumb		= skinUtil.StringToRect(strThumb);
		CRect Rect_ThumbMask	= skinUtil.StringToRect(strThumbMask);
		CRect Rect_ThumbActive	= skinUtil.StringToRect(strThumbActive);
		CRect Rect_Bk			= skinUtil.StringToRect(strBk);
		CRoundSliderCtrl* pSlider = new CRoundSliderCtrl;
		BOOL ret = pSlider->Create(WS_CHILD  |SS_NOTIFY |WS_VISIBLE, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pSlider;
			node = node->NextSibling();
			continue;
		}
		
		pSlider->SetId(nID);
		bool bTransparency = true;
		if(strThumbMask.IsEmpty())
			bTransparency = false;
		
		bool bThumb = false;
		if(!strThumb.IsEmpty())
			bThumb = true;
		
		if(!strBk.IsEmpty())//패턴이 아닌 전체 전체배경
			pSlider->CopyFromBk(m_bmSkinImage, Rect_Bk);
		
		if(bThumb)
			pSlider->CopyFromThumb(m_bmSkinImage, Rect_Thumb, Rect_ThumbActive,Rect_ThumbMask, bTransparency);
		pSlider->SetRange( 0, nMax );
		pSlider->SetThumbLock(bThumbLock);
		pSlider->SetPos( nProgress );
		pSlider->SetCaption(strCap);
		pSlider->SetInverted(bInvert);
		pSlider->SetZero(nZero);
		pSlider->SetCenter(nCenterX, nCenterY);
		pSlider->SetDiameter(nDiameter);
		pSlider->SetShowText(bShowText);
		pSlider->Init();

		m_ArrRndSlider.Add(pSlider);

		node = node->NextSibling();
	}
	xmldoc.Clear();
}

void CBaseSkin::AttachSlider()
{
	TRACE("######AttachSlider######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_SLIDER);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_SLIDER_DEF && nID < IDC_EDIT_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachSlider::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strOrientation	= ConvStr::utf8ToCstr(itemElement->Attribute("Orientation"));
		int nMax = 0,
			nProgress = 0;
		nret = itemElement->QueryIntAttribute("Max", &nMax);
		nret = itemElement->QueryIntAttribute("Progress", &nProgress);
		CString strLeft			= ConvStr::utf8ToCstr(itemElement->Attribute("Left"));
		CString strRight		= ConvStr::utf8ToCstr(itemElement->Attribute("Right"));
		CString strLeftU		= ConvStr::utf8ToCstr(itemElement->Attribute("LeftU"));
		CString strRightU		= ConvStr::utf8ToCstr(itemElement->Attribute("RightU"));
		CString strCenterLeft	= ConvStr::utf8ToCstr(itemElement->Attribute("CenterLeft"));
		CString strCenterRight	= ConvStr::utf8ToCstr(itemElement->Attribute("CenterRight"));
		CString strThumb		= ConvStr::utf8ToCstr(itemElement->Attribute("Thumb"));
		CString strThumbActive	= ConvStr::utf8ToCstr(itemElement->Attribute("ThumbActive"));
		CString strThumbMask	= ConvStr::utf8ToCstr(itemElement->Attribute("ThumbMask"));
		CString strMargin		= ConvStr::utf8ToCstr(itemElement->Attribute("Margin"));
		CString strBkMargin		= ConvStr::utf8ToCstr(itemElement->Attribute("BkMargin"));
		CString strBk			= ConvStr::utf8ToCstr(itemElement->Attribute("Bk"));
		
		bool bThumbLock = false;
		nret = itemElement->QueryBoolAttribute("ThumbLock", &bThumbLock);
		
		CRect Rect_Pos			= skinUtil.StringToRect(strPos);
		CRect Rect_Left			= skinUtil.StringToRect(strLeft);
		CRect Rect_Right		= skinUtil.StringToRect(strRight);

		CRect Rect_LeftU, Rect_RightU;
		if(strLeftU.IsEmpty() || strRightU.IsEmpty() )
		{
			Rect_LeftU		= Rect_Left;
			Rect_RightU		= Rect_Right;
		}else{
			Rect_LeftU		= skinUtil.StringToRect(strLeftU);
			Rect_RightU		= skinUtil.StringToRect(strRightU);
		}
		CRect Rect_CenterLeft	= skinUtil.StringToRect(strCenterLeft);
		CRect Rect_CenterRight	= skinUtil.StringToRect(strCenterRight);
		CRect Rect_Thumb		= skinUtil.StringToRect(strThumb);
		CRect Rect_ThumbActive	= skinUtil.StringToRect(strThumbActive);
		CRect Rect_ThumbMask	= skinUtil.StringToRect(strThumbMask);
		
		CRect Rect_Bk			= skinUtil.StringToRect(strBk);

		CxSkinSlider* pSlider = new CxSkinSlider;
		BOOL ret = pSlider->Create(_T(""), WS_CHILD  |SS_NOTIFY |WS_VISIBLE, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE){
			delete pSlider;
			node = node->NextSibling();
			continue;
		}

		pSlider->SetId(nID);
		bool bTransparency = true;
		if(strThumbMask.IsEmpty())
			bTransparency = false;
		if(0 == strOrientation.CompareNoCase(_T("V")))
			pSlider->SetVertical(true);
		bool bThumb = false;
		if(!strThumb.IsEmpty())
			bThumb = true;
		
		if(!strBk.IsEmpty())//패턴이 아닌 전체 전체배경
		{
			pSlider->CopyFromBk(m_bmSkinImage, Rect_Bk);
		}
		
		pSlider->CopyFromBG(m_bmSkinImage, Rect_Left, Rect_Right, Rect_CenterLeft, Rect_CenterRight, Rect_ThumbMask,Rect_LeftU,Rect_RightU);

		if(bThumb)
			pSlider->CopyFromThumb(m_bmSkinImage, Rect_Thumb, Rect_ThumbActive, bTransparency);
		pSlider->DrawFocusRect( FALSE );
		if(strMargin != _T(""))
		{
			CString l = _T(""), t = _T(""), r = _T(""), b = _T("");

			AfxExtractSubString( l, strMargin, 0, _T(','));
			AfxExtractSubString( t, strMargin, 1, _T(','));
			AfxExtractSubString( r, strMargin, 2, _T(','));
			AfxExtractSubString( b, strMargin, 3, _T(','));
			
			pSlider->SetMargin(_ttoi(l),_ttoi(t),_ttoi(r),_ttoi(b));
		}
		if(strBkMargin != _T(""))
		{
			CString l = _T(""), t = _T(""), r = _T(""), b = _T("");

			AfxExtractSubString( l, strBkMargin, 0, _T(','));
			AfxExtractSubString( t, strBkMargin, 1, _T(','));
			AfxExtractSubString( r, strBkMargin, 2, _T(','));
			AfxExtractSubString( b, strBkMargin, 3, _T(','));
			pSlider->SetBkMargin(_ttoi(l),_ttoi(t),_ttoi(r),_ttoi(b));
		}
		pSlider->SetRange( 0, nMax );
		pSlider->SetThumbLock(bThumbLock);
		pSlider->SetPos( nProgress );
		pSlider->SetCaption(strCap);

		m_ArrSlider.Add(pSlider);

		node = node->NextSibling();
	}
	xmldoc.Clear();

}

void CBaseSkin::_ReLoadSkinSlider()
{
	TRACE("######_ReLoadSkinSlider######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_SLIDER);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_SLIDER_DEF && nID < IDC_EDIT_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinSlider::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strOrientation	= ConvStr::utf8ToCstr(itemElement->Attribute("Orientation"));
		int nMax = 100, 
			nProgress = 0;
		nret = itemElement->QueryIntAttribute("Max", &nMax);
		nret = itemElement->QueryIntAttribute("Progress", &nProgress);
		CString strLeft			= ConvStr::utf8ToCstr(itemElement->Attribute("Left"));
		CString strRight		= ConvStr::utf8ToCstr(itemElement->Attribute("Right"));
		CString strLeftU		= ConvStr::utf8ToCstr(itemElement->Attribute("LeftU"));
		CString strRightU		= ConvStr::utf8ToCstr(itemElement->Attribute("RightU"));
		CString strCenterLeft	= ConvStr::utf8ToCstr(itemElement->Attribute("CenterLeft"));
		CString strCenterRight	= ConvStr::utf8ToCstr(itemElement->Attribute("CenterRight"));
		CString strThumb		= ConvStr::utf8ToCstr(itemElement->Attribute("Thumb"));
		CString strThumbActive	= ConvStr::utf8ToCstr(itemElement->Attribute("ThumbActive"));
		CString strThumbMask	= ConvStr::utf8ToCstr(itemElement->Attribute("ThumbMask"));
		bool bThumbLock = false;
		nret = itemElement->QueryBoolAttribute("ThumbLock", &bThumbLock);
		
		CRect Rect_Pos			= skinUtil.StringToRect(strPos);
		CRect Rect_Left			= skinUtil.StringToRect(strLeft);
		CRect Rect_Right		= skinUtil.StringToRect(strRight);
		CRect Rect_LeftU, Rect_RightU;
		if(strLeftU.IsEmpty() || strRightU.IsEmpty()){
			Rect_LeftU		= Rect_Left;
			Rect_RightU		= Rect_Right;
		}else{
			Rect_LeftU		= skinUtil.StringToRect(strLeftU);
			Rect_RightU		= skinUtil.StringToRect(strRightU);
		}
		CRect Rect_CenterLeft	= skinUtil.StringToRect(strCenterLeft);
		CRect Rect_CenterRight	= skinUtil.StringToRect(strCenterRight);
		CRect Rect_Thumb		= skinUtil.StringToRect(strThumb);
		CRect Rect_ThumbActive	= skinUtil.StringToRect(strThumbActive);
		CRect Rect_ThumbMask	= skinUtil.StringToRect(strThumbMask);

		CxSkinSlider* pSlider = FindSlider(nID);
		if(pSlider != NULL)
		{
			pSlider->CopyFromBG(m_bmSkinImage, Rect_Left, Rect_Right, Rect_CenterLeft, Rect_CenterRight, Rect_ThumbMask,Rect_LeftU,Rect_RightU);
			pSlider->CopyFromThumb(m_bmSkinImage, Rect_Thumb, Rect_ThumbActive, TRUE);
		}
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

CxSkinSlider* CBaseSkin::FindSlider(int id)
{
	int nSz = (int)m_ArrSlider.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CxSkinSlider* pSlider = m_ArrSlider.GetAt(j);
		if(pSlider->GetId() == id)
		{
			return pSlider; 
		}
	}
	return NULL;
}


#if USE_SKINLISTCTRL
void CBaseSkin::AttachList(void)
{
	TRACE("######AttachList######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_LIST);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_LIST_DEF && nID < IDC_TREE_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachList::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString	strColList		= ConvStr::utf8ToCstr(itemElement->Attribute("ColumnList"));
		bool bCheckbox = false,
			bSorting = false,
			bDragDrop = false;
		nret = itemElement->QueryBoolAttribute("UseCheckbox", &bCheckbox);
		int nCheckCol = -1;
		nret = itemElement->QueryIntAttribute("CheckColumn", &nCheckCol);
		nret = itemElement->QueryBoolAttribute("UseSorting", &bSorting);
		nret = itemElement->QueryBoolAttribute("UseDragDrop", &bDragDrop);		
		CString strFont				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize = FONT_SIZE;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);

		CString strTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strOddBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("OddBkColor"));
		CString strEvenBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("EvenBkColor"));
		CString strSelectBkColor	= ConvStr::utf8ToCstr(itemElement->Attribute("SelectBkColor"));
		CString strSelectTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("SelectTextColor"));
		CString strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("FocusBkColor"));
		CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FocusTextColor"));
		
		COLORREF clrText		= skinUtil.StringToRGB(strTextColor);
		COLORREF clrOddBk		= skinUtil.StringToRGB(strOddBkColor);
		COLORREF clrEvenBk		= skinUtil.StringToRGB(strEvenBkColor);
		COLORREF clrFocusText	= skinUtil.StringToRGB(strFocusTextColor);
		COLORREF clrFocusBk		= skinUtil.StringToRGB(strFocusBkColor);
		COLORREF clrSelectText	= skinUtil.StringToRGB(strSelectTextColor);
		COLORREF clrSelectBk	= skinUtil.StringToRGB(strSelectBkColor);

		//Checkbox
		CString strOddCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowOddCheckedRect"));
		CString strOddUnCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowOddUnCheckedRect"));
		CString strEvenCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowEvenCheckedRect"));
		CString strEvenUnCheckedRect	= ConvStr::utf8ToCstr(itemElement->Attribute("RowEvenUnCheckedRect"));
		CString strFocusCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowFocusCheckedRect"));
		CString strFocusUnCheckedRect	= ConvStr::utf8ToCstr(itemElement->Attribute("RowFocusUnCheckedRect"));
		//HeaderCtrl
		bool bUseHeader = false,
			bHeaderDraggable = false;
		nret = itemElement->QueryBoolAttribute("UseHeader", &bUseHeader);
		nret = itemElement->QueryBoolAttribute("HeaderDraggable", &bHeaderDraggable);
		int nHeaderHeight = 0,
			nHeaderFontSize = 0;
		nret = itemElement->QueryIntAttribute("HeaderHeight", &nHeaderHeight);
		CString strHeaderFont		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderFont"));
		nret = itemElement->QueryIntAttribute("HeaderFontSize", &nHeaderFontSize);
		bool bHeaderBold = false;
		nret = itemElement->QueryBoolAttribute("HeaderFontBold", &bHeaderBold);
		CString strHeadTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderTextColor"));
		CString strHeadBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkColor"));
		CString strHeadCheck			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderCheckRect"));
		CString strHeadUnCheck			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderUnCheckRect"));
		CString strHeadBkLeft				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkLeftRect"));
		CString strHeadBkCenterNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterNormalRect"));
		CString strHeadBkCenterOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterOverRect"));
		CString strHeadBkCenterDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterDownRect"));
		CString strHeadBkCenterSelectRect	= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterSelectRect"));
		CString strHeadDivider				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderDividerRect"));
		CString strHeadDividerDown			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderDividerDownRect"));
		CString strHeadBkRight				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkRightRect"));
		CString strHeadSortAsc				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderSortAscRect"));
		CString strHeadSortDesc				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderSortDescRect"));
		bool bTransThumb = false;
		nret = itemElement->QueryBoolAttribute("TransparentThumb", &bTransThumb);
		//Scrollbar
		CString strGripColor		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_COLOR));
		CString strGripLineColor	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_LINE));
		//Vertical button
		CString strVertTopBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_NRECT));
		CString strVertTopBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_ORECT));
		CString strVertTopBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_DRECT));
		CString strVertBtmBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_NRECT));
		CString strVertBtmBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_ORECT));
		CString strVertBtmBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_DRECT));
		//Vertical Thumb
		CString strVertThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_NRECT));
		CString strVertThumbTopOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_ORECT));
		CString strVertThumbTopDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_DRECT));
		CString strVertThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_NRECT));
		CString strVertThumbMidOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_ORECT));
		CString strVertThumbMidDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_DRECT));
		CString strVertThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_NRECT));
		CString strVertThumbBtmOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_ORECT));
		CString strVertThumbBtmDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_DRECT));
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CString strVertChanRect				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_RECT));
#else
		CString strVertChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_TRECT));
		CString strVertChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_MRECT));
		CString strVertChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_BRECT));
#endif
		//horizontal button
		CString strHorizLeftBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_NRECT));
		CString strHorizLeftBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_ORECT));
		CString strHorizLeftBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_DRECT));
		CString strHorizRightBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_NRECT));
		CString strHorizRightBtnOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RMTN_ORECT));
		CString strHorizRightBtnDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_DRECT));
		//horizontal Thumb
		CString strHorizThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_NRECT));
		CString strHorizThumbTopOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_ORECT));
		CString strHorizThumbTopDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_DRECT));
		CString strHorizThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_NRECT));
		CString strHorizThumbMidOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_ORECT));
		CString strHorizThumbMidDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_DRECT));
		CString strHorizThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_NRECT));
		CString strHorizThumbBtmOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_ORECT));
		CString strHorizThumbBtmDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_DRECT));
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CString strHorizChanRect			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_RECT));
#else
		CString strHorizChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_TRECT));
		CString strHorizChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_MRECT));
		CString strHorizChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_BRECT));
#endif
		
		int nMinThumbSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_SB_HORZ_THUMB_MIN_SIZE, &nMinThumbSize);
		//Vertical button
		CRect Rect_VertTBN		= skinUtil.StringToRect(strVertTopBtnNormalRect);
		CRect Rect_VertTBO		= skinUtil.StringToRect(strVertTopBtnOverRect);
		CRect Rect_VertTBD		= skinUtil.StringToRect(strVertTopBtnDownRect);
		CRect Rect_VertBBN		= skinUtil.StringToRect(strVertBtmBtnNormalRect);
		CRect Rect_VertBBO		= skinUtil.StringToRect(strVertBtmBtnOverRect);
		CRect Rect_VertBBD		= skinUtil.StringToRect(strVertBtmBtnDownRect);
		//Vertical Thumb
		CRect Rect_VertTHTN		= skinUtil.StringToRect(strVertThumbTopNormalRect);
		CRect Rect_VertTHTO		= skinUtil.StringToRect(strVertThumbTopOverRect);
		CRect Rect_VertTHTD		= skinUtil.StringToRect(strVertThumbTopDownRect);
		CRect Rect_VertTHMN		= skinUtil.StringToRect(strVertThumbMidNormalRect);
		CRect Rect_VertTHMO		= skinUtil.StringToRect(strVertThumbMidOverRect);
		CRect Rect_VertTHMD		= skinUtil.StringToRect(strVertThumbMidDownRect);
		CRect Rect_VertTHBN		= skinUtil.StringToRect(strVertThumbBtmNormalRect);
		CRect Rect_VertTHBO		= skinUtil.StringToRect(strVertThumbBtmOverRect);
		CRect Rect_VertTHBD		= skinUtil.StringToRect(strVertThumbBtmDownRect);
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CRect Rect_VertCH		= skinUtil.StringToRect(strVertChanRect);
#else
		CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
		CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
		CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);
#endif
		//horizontal button
		CRect Rect_HorizLBN		= skinUtil.StringToRect(strHorizLeftBtnNormalRect);
		CRect Rect_HorizLBO		= skinUtil.StringToRect(strHorizLeftBtnOverRect);
		CRect Rect_HorizLBD		= skinUtil.StringToRect(strHorizLeftBtnDownRect);
		CRect Rect_HorizRBN		= skinUtil.StringToRect(strHorizRightBtnNormalRect);
		CRect Rect_HorizRBO		= skinUtil.StringToRect(strHorizRightBtnOverRect);
		CRect Rect_HorizRBD		= skinUtil.StringToRect(strHorizRightBtnDownRect);
		//horizontal Thumb
		CRect Rect_HorizTHTN		= skinUtil.StringToRect(strHorizThumbTopNormalRect);
		CRect Rect_HorizTHTO		= skinUtil.StringToRect(strHorizThumbTopOverRect);
		CRect Rect_HorizTHTD		= skinUtil.StringToRect(strHorizThumbTopDownRect);
		CRect Rect_HorizTHMN		= skinUtil.StringToRect(strHorizThumbMidNormalRect);
		CRect Rect_HorizTHMO		= skinUtil.StringToRect(strHorizThumbMidOverRect);
		CRect Rect_HorizTHMD		= skinUtil.StringToRect(strHorizThumbMidDownRect);
		CRect Rect_HorizTHBN		= skinUtil.StringToRect(strHorizThumbBtmNormalRect);
		CRect Rect_HorizTHBO		= skinUtil.StringToRect(strHorizThumbBtmOverRect);
		CRect Rect_HorizTHBD		= skinUtil.StringToRect(strHorizThumbBtmDownRect);
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CRect Rect_HorizCH		= skinUtil.StringToRect(strHorizChanRect);
#else
		CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
		CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
		CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);
#endif
		CRect Rect_Pos = skinUtil.StringToRect(strPos);
		//headerctrl
		CRect Rect_HeadCheck		= skinUtil.StringToRect(strHeadCheck);
		CRect Rect_HeadUnCheck		= skinUtil.StringToRect(strHeadUnCheck);
		CRect Rect_HeadLeft			= skinUtil.StringToRect(strHeadBkLeft);
		CRect Rect_HeadBkCNormal	= skinUtil.StringToRect(strHeadBkCenterNormalRect);
		CRect Rect_HeadBkCOver		= skinUtil.StringToRect(strHeadBkCenterOverRect);
		CRect Rect_HeadBkCDown		= skinUtil.StringToRect(strHeadBkCenterDownRect);
		CRect Rect_HeadBkCSelect	= skinUtil.StringToRect(strHeadBkCenterSelectRect);
		CRect Rect_HeadDivider		= skinUtil.StringToRect(strHeadDivider);
		CRect Rect_HeadDividerDown	= skinUtil.StringToRect(strHeadDividerDown);
		CRect Rect_HeadRight		= skinUtil.StringToRect(strHeadBkRight);
		CRect Rect_HeadAsc			= skinUtil.StringToRect(strHeadSortAsc);
		CRect Rect_HeadDesc			= skinUtil.StringToRect(strHeadSortDesc);
		//checkbox rect
		CRect Rect_OddChecked		= skinUtil.StringToRect(strOddCheckedRect);
		CRect Rect_OddUnChecked		= skinUtil.StringToRect(strOddUnCheckedRect);
		CRect Rect_EvenChecked		= skinUtil.StringToRect(strEvenCheckedRect);
		CRect Rect_EvenUnChecked	= skinUtil.StringToRect(strEvenUnCheckedRect);
		CRect Rect_FocusChecked		= skinUtil.StringToRect(strFocusCheckedRect);
		CRect Rect_FocusUnChecked	= skinUtil.StringToRect(strFocusUnCheckedRect);
		COLORREF clrHeadTxt		= skinUtil.StringToRGB(strHeadTextColor);
		COLORREF clrHeadBk		= skinUtil.StringToRGB(strHeadBkColor);
		COLORREF clrGripBk			= skinUtil.StringToRGB(strGripColor);
		COLORREF clrGripOutline		= skinUtil.StringToRGB(strGripLineColor);

		CxSkinListCtrl* pList = new CxSkinListCtrl;
		BOOL ret = pList->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, Rect_Pos, m_pWnd, nID);
		if(ret == FALSE)
		{
			delete pList;
			node = node->NextSibling();
			continue;
		}
		
		pList->SetId(nID);
		pList->SetColumnList(strColList);
		pList->CopyListHeaderFromBG(m_bmSkinImage, Rect_HeadCheck, Rect_HeadUnCheck, 
			Rect_HeadLeft, Rect_HeadBkCNormal, Rect_HeadBkCOver, Rect_HeadBkCDown, Rect_HeadBkCSelect, Rect_HeadDivider, Rect_HeadDividerDown, Rect_HeadRight,
			Rect_HeadAsc, Rect_HeadDesc);
		if(bCheckbox)
			pList->CopyListCheckFromBG(m_bmSkinImage, Rect_OddChecked, Rect_OddUnChecked, Rect_EvenChecked, Rect_EvenUnChecked, Rect_FocusChecked, Rect_FocusUnChecked);
#if USE_TRANSPARENTBAR
		pList->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertCH);
#else
		pList->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
#if USE_TRANSPARENTBAR
		pList->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizCH);
#else
		pList->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif
		pList->SetUseCheck(bCheckbox);
		pList->SetUseDragDrop(bDragDrop);			
		pList->SetNormalColor(clrText, clrOddBk, clrEvenBk);
		pList->SetFocusColor(clrFocusText, clrFocusBk);
		pList->SetSelectColor(clrSelectText, clrSelectBk);
		pList->SetScrollGripColor(clrGripBk, clrGripOutline);
		pList->SetHeadBackColor(clrHeadBk);
		pList->SetHeadTxtColor(clrHeadTxt);		
		pList->SetUseSort(bSorting);
		pList->SetHeadFontBold(bHeaderBold);
		pList->SetMinColumnSize(nMinThumbSize);
		pList->InitHeader();
		pList->SetCaption(strCap);

		m_ArrList.Add(pList);
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

void CBaseSkin::_ReLoadSkinList(void)
{
	TRACE("######_ReLoadSkinList######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	
	node = node->FirstChild(DEF_LIST);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_LIST_DEF && nID < IDC_TREE_DEF))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachList::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		bool bCheckbox=false,
			bSorting=false,
			bDragDrop = false;
		nret = itemElement->QueryBoolAttribute("Checkbox", &bCheckbox);
		nret = itemElement->QueryBoolAttribute("Sorting", &bSorting);
		nret = itemElement->QueryBoolAttribute("DragDrop", &bDragDrop);		
		CString strFont				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nFontSize = FONT_SIZE;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		CString strTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FocusTextColor"));
		CString strOddBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("OddBkColor"));
		CString strEvenBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("EvenBkColor"));
		CString strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("FocusBkColor"));
		CString strHeight			= ConvStr::utf8ToCstr(itemElement->Attribute("RowHeight"));		
		//Checkbox
		CString strOddCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowOddCheckedRect"));
		CString strOddUnCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowOddUnCheckedRect"));
		CString strEvenCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowEvenCheckedRect"));
		CString strEvenUnCheckedRect	= ConvStr::utf8ToCstr(itemElement->Attribute("RowEvenUnCheckedRect"));
		CString strFocusCheckedRect		= ConvStr::utf8ToCstr(itemElement->Attribute("RowFocusCheckedRect"));
		CString strFocusUnCheckedRect	= ConvStr::utf8ToCstr(itemElement->Attribute("RowFocusUnCheckedRect"));
		//HeaderCtrl
		int nHeaderHeight = 20,
			nHeaderFontSize = FONT_SIZE;
		nret = itemElement->QueryIntAttribute("HeaderHeight", &nHeaderHeight);
		CString strHeaderFont		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderFont"));
		nret = itemElement->QueryIntAttribute("HeaderFontSize", &nHeaderFontSize);
		bool bHeaderBold = false;
		nret = itemElement->QueryBoolAttribute("HeaderFontBold", &bHeaderBold);
		CString strHeadTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderTextColor"));
		CString strHeadBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkColor"));
		CString strHeadCheck			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderCheckRect"));
		CString strHeadUnCheck			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderUnCheckRect"));
		CString strHeadBkLeft				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkLeftRect"));
		CString strHeadBkCenterNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterNormalRect"));
		CString strHeadBkCenterOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterOverRect"));
		CString strHeadBkCenterDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterDownRect"));
		CString strHeadBkCenterSelectRect	= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkCenterSelectRect"));
		CString strHeadDivider				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderDividerRect"));
		CString strHeadDividerDown			= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderDividerDownRect"));
		CString strHeadBkRight				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderBkRightRect"));
		CString strHeadSortAsc				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderSortAscRect"));
		CString strHeadSortDesc				= ConvStr::utf8ToCstr(itemElement->Attribute("HeaderSortDescRect"));
		bool bTransThumb = false;
		nret = itemElement->QueryBoolAttribute("TransparentThumb", &bTransThumb);
		//Scrollbar
		CString strGripColor		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_COLOR));
		CString strGripLineColor	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_LINE));
		//Vertical button
		CString strVertTopBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_NRECT));
		CString strVertTopBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_ORECT));
		CString strVertTopBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_DRECT));
		CString strVertBtmBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_NRECT));
		CString strVertBtmBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_ORECT));
		CString strVertBtmBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_DRECT));
		//Vertical Thumb
		CString strVertThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_NRECT));
		CString strVertThumbTopOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_ORECT));
		CString strVertThumbTopDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_DRECT));
		CString strVertThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_NRECT));
		CString strVertThumbMidOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_ORECT));
		CString strVertThumbMidDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_DRECT));
		CString strVertThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_NRECT));
		CString strVertThumbBtmOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_ORECT));
		CString strVertThumbBtmDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_DRECT));
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CString strVertChanRect				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_RECT));
#else
		CString strVertChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_TRECT));
		CString strVertChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_MRECT));
		CString strVertChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_BRECT));
#endif
		//horizontal button
		CString strHorizLeftBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_NRECT));
		CString strHorizLeftBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_ORECT));
		CString strHorizLeftBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_DRECT));
		CString strHorizRightBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_NRECT));
		CString strHorizRightBtnOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RMTN_ORECT));
		CString strHorizRightBtnDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_DRECT));
		//horizontal Thumb
		CString strHorizThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_NRECT));
		CString strHorizThumbTopOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_ORECT));
		CString strHorizThumbTopDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_DRECT));
		CString strHorizThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_NRECT));
		CString strHorizThumbMidOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_ORECT));
		CString strHorizThumbMidDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_DRECT));
		CString strHorizThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_NRECT));
		CString strHorizThumbBtmOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_ORECT));
		CString strHorizThumbBtmDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_DRECT));
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CString strHorizChanRect			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_RECT));
#else
		CString strHorizChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_TRECT));
		CString strHorizChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_MRECT));
		CString strHorizChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_BRECT));
#endif
		
		int nMinThumbSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_SB_HORZ_THUMB_MIN_SIZE, &nMinThumbSize);
		//Vertical button
		CRect Rect_VertTBN		= skinUtil.StringToRect(strVertTopBtnNormalRect);
		CRect Rect_VertTBO		= skinUtil.StringToRect(strVertTopBtnOverRect);
		CRect Rect_VertTBD		= skinUtil.StringToRect(strVertTopBtnDownRect);
		CRect Rect_VertBBN		= skinUtil.StringToRect(strVertBtmBtnNormalRect);
		CRect Rect_VertBBO		= skinUtil.StringToRect(strVertBtmBtnOverRect);
		CRect Rect_VertBBD		= skinUtil.StringToRect(strVertBtmBtnDownRect);
		//Vertical Thumb
		CRect Rect_VertTHTN		= skinUtil.StringToRect(strVertThumbTopNormalRect);
		CRect Rect_VertTHTO		= skinUtil.StringToRect(strVertThumbTopOverRect);
		CRect Rect_VertTHTD		= skinUtil.StringToRect(strVertThumbTopDownRect);
		CRect Rect_VertTHMN		= skinUtil.StringToRect(strVertThumbMidNormalRect);
		CRect Rect_VertTHMO		= skinUtil.StringToRect(strVertThumbMidOverRect);
		CRect Rect_VertTHMD		= skinUtil.StringToRect(strVertThumbMidDownRect);
		CRect Rect_VertTHBN		= skinUtil.StringToRect(strVertThumbBtmNormalRect);
		CRect Rect_VertTHBO		= skinUtil.StringToRect(strVertThumbBtmOverRect);
		CRect Rect_VertTHBD		= skinUtil.StringToRect(strVertThumbBtmDownRect);
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CRect Rect_VertCH		= skinUtil.StringToRect(strVertChanRect);
#else
		CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
		CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
		CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);
#endif
		//horizontal button
		CRect Rect_HorizLBN		= skinUtil.StringToRect(strHorizLeftBtnNormalRect);
		CRect Rect_HorizLBO		= skinUtil.StringToRect(strHorizLeftBtnOverRect);
		CRect Rect_HorizLBD		= skinUtil.StringToRect(strHorizLeftBtnDownRect);
		CRect Rect_HorizRBN		= skinUtil.StringToRect(strHorizRightBtnNormalRect);
		CRect Rect_HorizRBO		= skinUtil.StringToRect(strHorizRightBtnOverRect);
		CRect Rect_HorizRBD		= skinUtil.StringToRect(strHorizRightBtnDownRect);
		//horizontal Thumb
		CRect Rect_HorizTHTN		= skinUtil.StringToRect(strHorizThumbTopNormalRect);
		CRect Rect_HorizTHTO		= skinUtil.StringToRect(strHorizThumbTopOverRect);
		CRect Rect_HorizTHTD		= skinUtil.StringToRect(strHorizThumbTopDownRect);
		CRect Rect_HorizTHMN		= skinUtil.StringToRect(strHorizThumbMidNormalRect);
		CRect Rect_HorizTHMO		= skinUtil.StringToRect(strHorizThumbMidOverRect);
		CRect Rect_HorizTHMD		= skinUtil.StringToRect(strHorizThumbMidDownRect);
		CRect Rect_HorizTHBN		= skinUtil.StringToRect(strHorizThumbBtmNormalRect);
		CRect Rect_HorizTHBO		= skinUtil.StringToRect(strHorizThumbBtmOverRect);
		CRect Rect_HorizTHBD		= skinUtil.StringToRect(strHorizThumbBtmDownRect);
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CRect Rect_HorizCH		= skinUtil.StringToRect(strHorizChanRect);
#else
		CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
		CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
		CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);
#endif

		CRect rcPos = skinUtil.StringToRect(strPos);
		//headerctrl
		CRect Rect_HeadCheck		= skinUtil.StringToRect(strHeadCheck);
		CRect Rect_HeadUnCheck		= skinUtil.StringToRect(strHeadUnCheck);
		CRect Rect_HeadLeft			= skinUtil.StringToRect(strHeadBkLeft);
		CRect Rect_HeadBkCNormal	= skinUtil.StringToRect(strHeadBkCenterNormalRect);
		CRect Rect_HeadBkCOver		= skinUtil.StringToRect(strHeadBkCenterOverRect);
		CRect Rect_HeadBkCDown		= skinUtil.StringToRect(strHeadBkCenterDownRect);
		CRect Rect_HeadBkCSelect	= skinUtil.StringToRect(strHeadBkCenterSelectRect);
		CRect Rect_HeadDivider		= skinUtil.StringToRect(strHeadDivider);
		CRect Rect_HeadDividerDown	= skinUtil.StringToRect(strHeadDividerDown);
		CRect Rect_HeadRight		= skinUtil.StringToRect(strHeadBkRight);
		CRect Rect_HeadAsc			= skinUtil.StringToRect(strHeadSortAsc);
		CRect Rect_HeadDesc			= skinUtil.StringToRect(strHeadSortDesc);
		//checkbox rect
		CRect Rect_OddChecked		= skinUtil.StringToRect(strOddCheckedRect);
		CRect Rect_OddUnChecked		= skinUtil.StringToRect(strOddUnCheckedRect);
		CRect Rect_EvenChecked		= skinUtil.StringToRect(strEvenCheckedRect);
		CRect Rect_EvenUnChecked	= skinUtil.StringToRect(strEvenUnCheckedRect);
		CRect Rect_FocusChecked		= skinUtil.StringToRect(strFocusCheckedRect);
		CRect Rect_FocusUnChecked	= skinUtil.StringToRect(strFocusUnCheckedRect);
		COLORREF clrHeadTxt			= skinUtil.StringToRGB(strHeadTextColor);
		COLORREF clrHeadBk			= skinUtil.StringToRGB(strHeadBkColor);
		COLORREF clrGripBk			= skinUtil.StringToRGB(strGripColor);
		COLORREF clrGripOutline		= skinUtil.StringToRGB(strGripLineColor);

		COLORREF clrText			= skinUtil.StringToRGB(strTextColor);
		COLORREF clrFocusText		= skinUtil.StringToRGB(strFocusTextColor);
		COLORREF clrOddBkColor		= skinUtil.StringToRGB(strOddBkColor);
		COLORREF clrEvenBkColor		= skinUtil.StringToRGB(strEvenBkColor);
		COLORREF clrFocusBkColor	= skinUtil.StringToRGB(strFocusBkColor);

		CxSkinListCtrl* pList = FindList(nID);
		if(pList != NULL)
		{
			pList->CopyListHeaderFromBG(m_bmSkinImage, Rect_HeadCheck, Rect_HeadUnCheck, 
				Rect_HeadLeft, Rect_HeadBkCNormal, Rect_HeadBkCOver, Rect_HeadBkCDown, Rect_HeadBkCSelect, Rect_HeadDivider, Rect_HeadDividerDown, Rect_HeadRight,
				Rect_HeadAsc, Rect_HeadDesc);
			if(bCheckbox)
				pList->CopyListCheckFromBG(m_bmSkinImage, Rect_OddChecked, Rect_OddUnChecked, Rect_EvenChecked, Rect_EvenUnChecked, Rect_FocusChecked, Rect_FocusUnChecked);
#if USE_TRANSPARENTBAR
			pList->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
				Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
				Rect_VertCH);
#else
			pList->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
				Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
				Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
#if USE_TRANSPARENTBAR
			pList->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
				Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
				Rect_HorizCH);
#else
			pList->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
				Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
				Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
		}
#endif
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

CxSkinListCtrl* CBaseSkin::FindList(int id)
{
	int nSz = (int)m_ArrList.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CxSkinListCtrl* pList = m_ArrList.GetAt(j);
		if(pList->GetId() == id)
			return pList;
	}
	return NULL;
}
#endif

#if USE_SKINTREECTRL
void CBaseSkin::AttachTree(void)
{
	TRACE("######AttachTree######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	
	node = node->FirstChild(DEF_TREE);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_TREE_DEF && nID < IDC_MAX))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("AttachTree::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strFont				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nItemHeight = 20;
		nret = itemElement->QueryIntAttribute("ItemHeight", &nItemHeight);
		int nFontSize = FONT_SIZE;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		CString strTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FontColorFocus"));
		CString strBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		CString strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColorFocus"));
		bool bTransThumb = false;
		nret = itemElement->QueryBoolAttribute("TransparentThumb", &bTransThumb);
		//Scrollbar
		CString strGripColor		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_COLOR));
		CString strGripLineColor	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_LINE));
		//Vertical button
		CString strVertTopBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_NRECT));
		CString strVertTopBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_ORECT));
		CString strVertTopBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_DRECT));
		CString strVertBtmBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_NRECT));
		CString strVertBtmBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_ORECT));
		CString strVertBtmBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_DRECT));
		//Vertical Thumb
		CString strVertThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_NRECT));
		CString strVertThumbTopOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_ORECT));
		CString strVertThumbTopDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_DRECT));
		CString strVertThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_NRECT));
		CString strVertThumbMidOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_ORECT));
		CString strVertThumbMidDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_DRECT));
		CString strVertThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_NRECT));
		CString strVertThumbBtmOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_ORECT));
		CString strVertThumbBtmDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_DRECT));
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CString strVertChanRect				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_RECT));
#else
		CString strVertChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_TRECT));
		CString strVertChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_MRECT));
		CString strVertChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_BRECT));
#endif
		//horizontal button
		CString strHorizLeftBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_NRECT));
		CString strHorizLeftBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_ORECT));
		CString strHorizLeftBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_DRECT));
		CString strHorizRightBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_NRECT));
		CString strHorizRightBtnOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RMTN_ORECT));
		CString strHorizRightBtnDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_DRECT));
		//horizontal Thumb
		CString strHorizThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_NRECT));
		CString strHorizThumbTopOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_ORECT));
		CString strHorizThumbTopDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_DRECT));
		CString strHorizThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_NRECT));
		CString strHorizThumbMidOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_ORECT));
		CString strHorizThumbMidDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_DRECT));
		CString strHorizThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_NRECT));
		CString strHorizThumbBtmOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_ORECT));
		CString strHorizThumbBtmDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_DRECT));
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CString strHorizChanRect			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_RECT));
#else
		CString strHorizChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_TRECT));
		CString strHorizChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_MRECT));
		CString strHorizChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_BRECT));
#endif
		
		int nMinThumbSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_SB_HORZ_THUMB_MIN_SIZE, &nMinThumbSize);
		//Vertical button
		CRect Rect_VertTBN		= skinUtil.StringToRect(strVertTopBtnNormalRect);
		CRect Rect_VertTBO		= skinUtil.StringToRect(strVertTopBtnOverRect);
		CRect Rect_VertTBD		= skinUtil.StringToRect(strVertTopBtnDownRect);
		CRect Rect_VertBBN		= skinUtil.StringToRect(strVertBtmBtnNormalRect);
		CRect Rect_VertBBO		= skinUtil.StringToRect(strVertBtmBtnOverRect);
		CRect Rect_VertBBD		= skinUtil.StringToRect(strVertBtmBtnDownRect);
		//Vertical Thumb
		CRect Rect_VertTHTN		= skinUtil.StringToRect(strVertThumbTopNormalRect);
		CRect Rect_VertTHTO		= skinUtil.StringToRect(strVertThumbTopOverRect);
		CRect Rect_VertTHTD		= skinUtil.StringToRect(strVertThumbTopDownRect);
		CRect Rect_VertTHMN		= skinUtil.StringToRect(strVertThumbMidNormalRect);
		CRect Rect_VertTHMO		= skinUtil.StringToRect(strVertThumbMidOverRect);
		CRect Rect_VertTHMD		= skinUtil.StringToRect(strVertThumbMidDownRect);
		CRect Rect_VertTHBN		= skinUtil.StringToRect(strVertThumbBtmNormalRect);
		CRect Rect_VertTHBO		= skinUtil.StringToRect(strVertThumbBtmOverRect);
		CRect Rect_VertTHBD		= skinUtil.StringToRect(strVertThumbBtmDownRect);
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CRect Rect_VertCH		= skinUtil.StringToRect(strVertChanRect);
#else
		CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
		CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
		CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);
#endif
		//horizontal button
		CRect Rect_HorizLBN		= skinUtil.StringToRect(strHorizLeftBtnNormalRect);
		CRect Rect_HorizLBO		= skinUtil.StringToRect(strHorizLeftBtnOverRect);
		CRect Rect_HorizLBD		= skinUtil.StringToRect(strHorizLeftBtnDownRect);
		CRect Rect_HorizRBN		= skinUtil.StringToRect(strHorizRightBtnNormalRect);
		CRect Rect_HorizRBO		= skinUtil.StringToRect(strHorizRightBtnOverRect);
		CRect Rect_HorizRBD		= skinUtil.StringToRect(strHorizRightBtnDownRect);
		//horizontal Thumb
		CRect Rect_HorizTHTN		= skinUtil.StringToRect(strHorizThumbTopNormalRect);
		CRect Rect_HorizTHTO		= skinUtil.StringToRect(strHorizThumbTopOverRect);
		CRect Rect_HorizTHTD		= skinUtil.StringToRect(strHorizThumbTopDownRect);
		CRect Rect_HorizTHMN		= skinUtil.StringToRect(strHorizThumbMidNormalRect);
		CRect Rect_HorizTHMO		= skinUtil.StringToRect(strHorizThumbMidOverRect);
		CRect Rect_HorizTHMD		= skinUtil.StringToRect(strHorizThumbMidDownRect);
		CRect Rect_HorizTHBN		= skinUtil.StringToRect(strHorizThumbBtmNormalRect);
		CRect Rect_HorizTHBO		= skinUtil.StringToRect(strHorizThumbBtmOverRect);
		CRect Rect_HorizTHBD		= skinUtil.StringToRect(strHorizThumbBtmDownRect);
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CRect Rect_HorizCH		= skinUtil.StringToRect(strHorizChanRect);
#else
		CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
		CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
		CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);
#endif
		CRect rcPos				= skinUtil.StringToRect(strPos);
		COLORREF clrGripBk		= skinUtil.StringToRGB(strGripColor);
		COLORREF clrGripOutline	= skinUtil.StringToRGB(strGripLineColor);

		CxSkinTreeCtrl* pTree = new CxSkinTreeCtrl;
		BOOL ret = pTree->Create(WS_VISIBLE | WS_CHILD | TVS_SHOWSELALWAYS | TVS_TRACKSELECT | WS_TABSTOP, rcPos, m_pWnd, nID);
		if(ret == FALSE) 
		{
			delete pTree;
			node = node->NextSibling();
			continue;
		}
		
		pTree->SetId(nID);
#if USE_TRANSPARENTBAR
		pTree->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertCH);
#else
		pTree->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
#if USE_TRANSPARENTBAR
		pTree->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizCH);
#else
		pTree->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif		
		pTree->SetScrollGripColor(clrGripBk, clrGripOutline);
		pTree->SetFontName(strFont);
		pTree->SetFontSize(nFontSize);
		pTree->SetTextColor(skinUtil.StringToRGB(strTextColor));		
		pTree->SetBkColor(skinUtil.StringToRGB(strBkColor));
		pTree->SetItemHeight(nItemHeight);
		pTree->SetMinThumbSize(nMinThumbSize);
		pTree->SetCaption(strCap);

		m_ArrTree.Add(pTree);

		node = node->NextSibling();
	}
	xmldoc.Clear();
}

void CBaseSkin::_ReLoadSkinTree(void)
{
	TRACE("######_ReLoadSkinTree######\n");
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	int nCnt = 0;
	node = node->FirstChild(DEF_TREE);	
	if(node == NULL) return;
	itemElement = node->ToElement();
	if(itemElement == NULL) return;
	node = itemElement->FirstChild();//첫번째 자식 노드(첫버튼)
	while(node){		
		itemElement = node->ToElement();
		if(itemElement == NULL) break;
		int nID = 0;
		int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);
		if( !(nID >= IDC_TREE_DEF && nID < IDC_MAX))
		{
			node = node->NextSibling();
			continue;
		}
		CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
		TRACE("_ReLoadSkinTree::Node=%s, Name=%s\n", node->Value(), strCap);
		CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
		CString strFont				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
		int nItemHeight = 18;
		nret = itemElement->QueryIntAttribute("ItemHeight", &nItemHeight);
		int nFontSize = FONT_SIZE;
		nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
		CString strTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
		CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FontColorFocus"));
		CString strBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
		CString strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("BkColorFocus"));
		bool bTransThumb = false;
		nret = itemElement->QueryBoolAttribute("TransparentThumb", &bTransThumb);
		
		////////////////Scrollbar
		CString strGripColor		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_COLOR));
		CString strGripLineColor	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_GRIP_LINE));
		//Vertical button
		CString strVertTopBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_NRECT));
		CString strVertTopBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_ORECT));
		CString strVertTopBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_TBTN_DRECT));
		CString strVertBtmBtnNormalRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_NRECT));
		CString strVertBtmBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_ORECT));
		CString strVertBtmBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_BBTN_DRECT));
		//Vertical Thumb
		CString strVertThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_NRECT));
		CString strVertThumbTopOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_ORECT));
		CString strVertThumbTopDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBT_DRECT));
		CString strVertThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_NRECT));
		CString strVertThumbMidOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_ORECT));
		CString strVertThumbMidDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBM_DRECT));
		CString strVertThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_NRECT));
		CString strVertThumbBtmOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_ORECT));
		CString strVertThumbBtmDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_THUMBB_DRECT));
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CString strVertChanRect				= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_RECT));
#else
		CString strVertChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_TRECT));
		CString strVertChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_MRECT));
		CString strVertChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_VERT_CHAN_BRECT));
#endif
		//horizontal button
		CString strHorizLeftBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_NRECT));
		CString strHorizLeftBtnOverRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_ORECT));
		CString strHorizLeftBtnDownRect		= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_LBTN_DRECT));
		CString strHorizRightBtnNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_NRECT));
		CString strHorizRightBtnOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RMTN_ORECT));
		CString strHorizRightBtnDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_RBTN_DRECT));
		//horizontal Thumb
		CString strHorizThumbTopNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_NRECT));
		CString strHorizThumbTopOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_ORECT));
		CString strHorizThumbTopDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBT_DRECT));
		CString strHorizThumbMidNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_NRECT));
		CString strHorizThumbMidOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_ORECT));
		CString strHorizThumbMidDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBM_DRECT));
		CString strHorizThumbBtmNormalRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_NRECT));
		CString strHorizThumbBtmOverRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_ORECT));
		CString strHorizThumbBtmDownRect	= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_THUMBB_DRECT));
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CString strHorizChanRect			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_RECT));
#else
		CString strHorizChanRectTop			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_TRECT));
		CString strHorizChanRectMid			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_MRECT));
		CString strHorizChanRectBtm			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_SB_HORZ_CHAN_BRECT));
#endif
		
		int nMinThumbSize = 0;
		nret = itemElement->QueryIntAttribute(DEF_SB_HORZ_THUMB_MIN_SIZE, &nMinThumbSize);
		//Vertical button
		CRect Rect_VertTBN		= skinUtil.StringToRect(strVertTopBtnNormalRect);
		CRect Rect_VertTBO		= skinUtil.StringToRect(strVertTopBtnOverRect);
		CRect Rect_VertTBD		= skinUtil.StringToRect(strVertTopBtnDownRect);
		CRect Rect_VertBBN		= skinUtil.StringToRect(strVertBtmBtnNormalRect);
		CRect Rect_VertBBO		= skinUtil.StringToRect(strVertBtmBtnOverRect);
		CRect Rect_VertBBD		= skinUtil.StringToRect(strVertBtmBtnDownRect);
		//Vertical Thumb
		CRect Rect_VertTHTN		= skinUtil.StringToRect(strVertThumbTopNormalRect);
		CRect Rect_VertTHTO		= skinUtil.StringToRect(strVertThumbTopOverRect);
		CRect Rect_VertTHTD		= skinUtil.StringToRect(strVertThumbTopDownRect);
		CRect Rect_VertTHMN		= skinUtil.StringToRect(strVertThumbMidNormalRect);
		CRect Rect_VertTHMO		= skinUtil.StringToRect(strVertThumbMidOverRect);
		CRect Rect_VertTHMD		= skinUtil.StringToRect(strVertThumbMidDownRect);
		CRect Rect_VertTHBN		= skinUtil.StringToRect(strVertThumbBtmNormalRect);
		CRect Rect_VertTHBO		= skinUtil.StringToRect(strVertThumbBtmOverRect);
		CRect Rect_VertTHBD		= skinUtil.StringToRect(strVertThumbBtmDownRect);
		//Vertical Channel
#if USE_TRANSPARENTBAR
		CRect Rect_VertCH		= skinUtil.StringToRect(strVertChanRect);
#else
		CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
		CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
		CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);
#endif
		//horizontal button
		CRect Rect_HorizLBN		= skinUtil.StringToRect(strHorizLeftBtnNormalRect);
		CRect Rect_HorizLBO		= skinUtil.StringToRect(strHorizLeftBtnOverRect);
		CRect Rect_HorizLBD		= skinUtil.StringToRect(strHorizLeftBtnDownRect);
		CRect Rect_HorizRBN		= skinUtil.StringToRect(strHorizRightBtnNormalRect);
		CRect Rect_HorizRBO		= skinUtil.StringToRect(strHorizRightBtnOverRect);
		CRect Rect_HorizRBD		= skinUtil.StringToRect(strHorizRightBtnDownRect);
		//horizontal Thumb
		CRect Rect_HorizTHTN		= skinUtil.StringToRect(strHorizThumbTopNormalRect);
		CRect Rect_HorizTHTO		= skinUtil.StringToRect(strHorizThumbTopOverRect);
		CRect Rect_HorizTHTD		= skinUtil.StringToRect(strHorizThumbTopDownRect);
		CRect Rect_HorizTHMN		= skinUtil.StringToRect(strHorizThumbMidNormalRect);
		CRect Rect_HorizTHMO		= skinUtil.StringToRect(strHorizThumbMidOverRect);
		CRect Rect_HorizTHMD		= skinUtil.StringToRect(strHorizThumbMidDownRect);
		CRect Rect_HorizTHBN		= skinUtil.StringToRect(strHorizThumbBtmNormalRect);
		CRect Rect_HorizTHBO		= skinUtil.StringToRect(strHorizThumbBtmOverRect);
		CRect Rect_HorizTHBD		= skinUtil.StringToRect(strHorizThumbBtmDownRect);
		//horizontal Channel
#if USE_TRANSPARENTBAR
		CRect Rect_HorizCH		= skinUtil.StringToRect(strHorizChanRect);
#else
		CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
		CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
		CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);
#endif
		CRect rcPos				= skinUtil.StringToRect(strPos);
		COLORREF clrGripBk		= skinUtil.StringToRGB(strGripColor);
		COLORREF clrGripOutline	= skinUtil.StringToRGB(strGripLineColor);

		CxSkinTreeCtrl*	pTree = FindTree(nID);
		if(pTree != NULL)
		{
#if USE_TRANSPARENTBAR
			pTree->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
				Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
				Rect_VertCH);
#else
			pTree->CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
				Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
				Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
#if USE_TRANSPARENTBAR
			pTree->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
				Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
				Rect_HorizCH);
#else
			pTree->CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
				Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
				Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif
			pTree->SetScrollGripColor(clrGripBk, clrGripOutline);
		}
		node = node->NextSibling();
	}
	xmldoc.Clear();
}

CxSkinTreeCtrl* CBaseSkin::FindTree(int id)
{
	int nSz = (int)m_ArrTree.GetSize();
	for(int j = 0; j < nSz; j++)
	{
		CxSkinTreeCtrl* pTree = m_ArrTree.GetAt(j);
		if(pTree->GetId() == id)
			return pTree;
	}
	return NULL;
}
#endif

bool CBaseSkin::SetMapMask()
{
	CSkinUtil skinUtil;
	CString strRect;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;

	node = node->FirstChild(DEF_MAIN_MASK);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("TopLeft"));
	if(strRect.IsEmpty()) return false;
	m_Map_Mask.SetRect(M_TOP_LEFT, skinUtil.StringToRect(strRect));
	
	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("TopCenter"));
	m_Map_Mask.SetRect(M_TOP_CENTER, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("TopRight"));
	m_Map_Mask.SetRect(M_TOP_RIGHT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("MiddleLeft"));
	m_Map_Mask.SetRect(M_MIDDLE_LEFT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("MiddleCenter"));
	m_Map_Mask.SetRect(M_MIDDLE_CENTER, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("MiddleRight"));
	m_Map_Mask.SetRect(M_MIDDLE_RIGHT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("BottomLeft"));
	m_Map_Mask.SetRect(M_BOTTOM_LEFT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("BottomCenter"));
	m_Map_Mask.SetRect(M_BOTTOM_CENTER, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("BottomRight"));
	m_Map_Mask.SetRect(M_BOTTOM_RIGHT, skinUtil.StringToRect(strRect));

	xmldoc.Clear();

	m_Map_Mask.m_bInit = true;

	return true;
}

bool CBaseSkin::SetMapFixed()
{
	CSkinUtil skinUtil;
	CString strRect;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	node = node->FirstChild(DEF_MAIN_BK);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	strRect = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FIXED));

	xmldoc.Clear();
	if(strRect.IsEmpty()) return false;
	m_Map_Fixed = skinUtil.StringToRect(strRect);

	return true;
}

bool CBaseSkin::SetMapBG()
{
	TRACE("######SetMapBG######\n");
	CSkinUtil skinUtil;
	CString strRect = _T("");

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	node = node->FirstChild(DEF_MAIN_BK);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("TopLeft"));
	if(strRect.IsEmpty()) return false;

	m_Map_BG.SetRect(M_TOP_LEFT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("TopCenter"));
	m_Map_BG.SetRect(M_TOP_CENTER, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("TopRight"));
	m_Map_BG.SetRect(M_TOP_RIGHT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("MiddleLeft"));
	m_Map_BG.SetRect(M_MIDDLE_LEFT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("MiddleCenter"));
	m_Map_BG.SetRect(M_MIDDLE_CENTER, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("MiddleRight"));
	m_Map_BG.SetRect(M_MIDDLE_RIGHT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("BottomLeft"));
	m_Map_BG.SetRect(M_BOTTOM_LEFT, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("BottomCenter"));
	m_Map_BG.SetRect(M_BOTTOM_CENTER, skinUtil.StringToRect(strRect));

	strRect = ConvStr::utf8ToCstr(itemElement->Attribute("BottomRight"));
	m_Map_BG.SetRect(M_BOTTOM_RIGHT, skinUtil.StringToRect(strRect));

	xmldoc.Clear();

	m_Map_BG.m_bInit = true;

	return true;
}

bool CBaseSkin::SetMapRGB()
{
	CSkinUtil skinUtil;
	CString strRGB;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	
	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFileName)){
		return false;
	}
	node = xmldoc.FirstChild(DEF_MAIN);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	node = node->FirstChild(DEF_MAIN_BK);
	if(node == NULL) return false;
	itemElement = node->ToElement();
	if(itemElement == NULL) return false;
	strRGB = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_RGB));
	
	xmldoc.Clear();

	if(strRGB.IsEmpty()) return false;
	
	m_clrBk = skinUtil.StringToRGB(strRGB);

	return true;
}

bool CBaseSkin::SetRGB(CDC* pDC)
{
	CDC* memDC;
	memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CRect cRect;
	m_pWnd->GetClientRect(&cRect);

	CBitmap bmpBuffer;
	bmpBuffer.CreateCompatibleBitmap(pDC, cRect.Width(), cRect.Height());
	CBitmap *pOldBitmap = (CBitmap*)memDC->SelectObject(&bmpBuffer);

	memDC->FillSolidRect(&cRect, m_clrBk);

	if(_lpCallback){
		_lpCallback(memDC, m_pWnd); 
	}

	pDC->BitBlt(0, 0, cRect.Width(), cRect.Height(), memDC, 0, 0, SRCCOPY);
	memDC->SelectObject(pOldBitmap);
	memDC->DeleteDC();
	delete memDC;

	DeleteObject(bmpBuffer.m_hObject);
	return true;
}

BOOL CBaseSkin::AttachCallback(LPFN_DrawMainBG _CallFunc)
{
	_lpCallback = _CallFunc;
	return 0;
}

void CBaseSkin::SkinRedraw(void)
{
	for(int i = 0; i < m_ArrButtons.GetSize(); i++)
		m_ArrButtons.GetAt(i)->Invalidate(true);

	for(int i = 0; i < m_ArrStatics.GetSize(); i++)
		m_ArrStatics.GetAt(i)->Invalidate(true);

	for(int i = 0; i < m_ArrSlider.GetSize(); i++)
		m_ArrSlider.GetAt(i)->Invalidate(true);

	for(int i = 0; i < m_ArrRndSlider.GetSize(); i++)
		m_ArrRndSlider.GetAt(i)->Invalidate(true);

	for(int i = 0; i < m_ArrEdit.GetSize(); i++)
		m_ArrEdit.GetAt(i)->Invalidate(true);
#if USE_SKINCOMBOBOX
	for(int i = 0; i < m_ArrCombo.GetSize(); i++)
		m_ArrCombo.GetAt(i)->Invalidate(true);
#endif
#if USE_SKINLISTCTRL
	for(int i = 0; i < m_ArrList.GetSize(); i++)
		m_ArrList.GetAt(i)->Invalidate(true);
#endif
#if USE_SKINTREECTRL
	for(int i = 0; i < m_ArrTree.GetSize(); i++)
		m_ArrTree.GetAt(i)->Invalidate(true);
#endif

}

#if USE_COOL_SB
//////////////////////////////////__Cool ScrollBar__/////////////////////////////////////////////////////////////////////////////////////////////////////////////
SB_API BOOL SB_InitializeApp(void)
{
	return CoolSB_InitializeApp();
}

SB_API BOOL SB_UninitializeApp(void)
{
	return CoolSB_UninitializeApp();
}

SB_API BOOL InitializeSB(HWND hwnd)
{
	return InitializeCoolSB(hwnd);
}

SB_API HRESULT UninitializeSB(HWND hwnd)
{
	return UninitializeCoolSB(hwnd);
}

SB_API BOOL SB_SetMinThumbSize(HWND hwnd, UINT wBar, UINT size)
{
	return CoolSB_SetMinThumbSize(hwnd, wBar, size);
}

SB_API BOOL SB_IsThumbTracking(HWND hwnd)
{
	return CoolSB_IsThumbTracking(hwnd);
}

SB_API BOOL SB_IsCoolScrollEnabled(HWND hwnd)
{
	return CoolSB_IsCoolScrollEnabled(hwnd);
}

SB_API BOOL SB_EnableScrollBar(HWND hwnd, int wSBflags, UINT wArrows)
{
	return CoolSB_EnableScrollBar(hwnd, wSBflags, wArrows);
}

SB_API BOOL SB_GetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi)
{
	return CoolSB_GetScrollInfo(hwnd, fnBar, lpsi);
}

SB_API int SB_GetScrollPos(HWND hwnd, int nBar)
{
	return CoolSB_GetScrollPos(hwnd, nBar);
}

SB_API BOOL SB_GetScrollRange(HWND hwnd, int nBar, LPINT lpMinPos, LPINT lpMaxPos)
{
	return CoolSB_GetScrollRange(hwnd, nBar, lpMinPos, lpMaxPos);
}

SB_API int SB_SetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi, BOOL fRedraw)
{
	return CoolSB_SetScrollInfo(hwnd, fnBar, lpsi, fRedraw);
}

SB_API int SB_SetScrollPos(HWND hwnd, int nBar, int nPos, BOOL fRedraw)
{
	return CoolSB_SetScrollPos(hwnd, nBar, nPos, fRedraw);
}

SB_API int SB_SetScrollRange(HWND hwnd, int nBar, int nMinPos, int nMaxPos, BOOL fRedraw)
{
	return CoolSB_SetScrollRange(hwnd, nBar, nMinPos, nMaxPos, fRedraw);
}

SB_API BOOL SB_ShowScrollBar(HWND hwnd, int wBar, BOOL fShow)
{
	return CoolSB_ShowScrollBar(hwnd, wBar, fShow);
}

SB_API BOOL SB_SetSize(HWND hwnd, int wBar, int nLength, int nWidth)
{
	return CoolSB_SetSize(hwnd, wBar, nLength, nWidth);
}

SB_API BOOL SB_SetStyle(HWND hwnd, int wBar, UINT nStyle)
{
	return CoolSB_SetStyle(hwnd, wBar, nStyle);
}

SB_API BOOL SB_SetThumbAlways(HWND hwnd, int wBar, BOOL fThumbAlways)
{
	return CoolSB_SetThumbAlways(hwnd, wBar, fThumbAlways);
}
#endif

