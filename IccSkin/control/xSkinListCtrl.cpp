// xSkinListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinListCtrl.h"

#include "SkinUtil.h"
#include "IniFile.H"

#include "ConvertString.h"
#include "tinyxml.h"

// CxSkinListCtrl

IMPLEMENT_DYNAMIC(CxSkinListCtrl, CListCtrl)

	CxSkinListCtrl::CxSkinListCtrl()
	:m_nMinThumbSize(0)
	,m_strListFont(FONT_NAME)
	,m_strHeadFont(FONT_NAME)
	,m_nListFontSize(FONT_SIZE)
	,m_nHeadFontSize(FONT_SIZE)
	,m_nListHeight(24)
	,m_nHeadHeight(20)
	,m_bHeaderDrag(false)
	,m_nHeadCheckIdx(0)
	,m_bHeadFontBold(false)
	,m_bUseHeaderCtrl(true)
	,m_nHeadCheckMargin(0)
	,m_bUseBkImage(false)
	,m_bUseCheckbox(false)
	,m_bUseSort(false)
	,m_bDragdrop(false)
	,m_bAscend(true)
	,m_nSortCol(-1)
	,m_nMinColumnSize(0)
	,m_bDrag(false)
	,m_pDragWnd(NULL)
	,m_pDropWnd(NULL)
	,m_nDragIndex(-1)
	,m_nDropIndex(-1)
	,m_pDragImage(NULL)
	,m_pDC(NULL)
	,m_bShowVScroll(true)
	,m_bShowHScroll(true)
	,m_clrOddbk(COLOR_BK_FOCUS)
	,m_clrEvenbk(COLOR_WHITE)
	,m_bTransparentThumb(false)
	,m_clrGripBk(COLOR_SB_GRIP)
	,m_clrGripOutline(COLOR_SB_GRIP)
	,m_clrText(COLOR_BLACK)
	,m_clrFocusText(COLOR_WHITE)
	,m_clrFocusBk(COLOR_BK_FOCUS)
	,m_clrSelectText(COLOR_BLACK)
	,m_clrSelectBk(COLOR_WHITE)
	,m_sbMode(SB_BOTH)
	,m_nSBWidth(0)
	,m_nSBHeight(0)
	,m_nVThumbTWidth(0)
	,m_nVThumbTHeight(0)
	,m_nVThumbMWidth(0)
	,m_nVThumbMHeight(0)
	,m_nVThumbBWidth(0)
	,m_nVThumbBHeight(0)
#if USE_TRANSPARENTBAR
	,m_nVChanWidth(0)
	,m_nVChanHeight(0)
#else
	,m_nVChanTWidth(0)
	,m_nVChanTHeight(0)
	,m_nVChanMWidth(0)
	,m_nVChanMHeight(0)
	,m_nVChanBWidth(0)
	,m_nVChanBHeight(0)
#endif
	,m_nHThumbTWidth(0)
	,m_nHThumbTHeight(0)
	,m_nHThumbMWidth(0)
	,m_nHThumbMHeight(0)
	,m_nHThumbBWidth(0)
	,m_nHThumbBHeight(0)
#if USE_TRANSPARENTBAR
	,m_nHChanWidth(0)
	,m_nHChanHeight(0)
#else
	,m_nHChanTWidth(0)
	,m_nHChanTHeight(0)
	,m_nHChanMWidth(0)
	,m_nHChanMHeight(0)
	,m_nHChanBWidth(0)
	,m_nHChanBHeight(0)
#endif
{

}

CxSkinListCtrl::~CxSkinListCtrl()
{

}


BEGIN_MESSAGE_MAP(CxSkinListCtrl, CListCtrl)
	ON_NOTIFY(HDN_BEGINDRAG, 0, &CxSkinListCtrl::OnHdnBegindrag)
	ON_NOTIFY(HDN_ENDDRAG, 0, &CxSkinListCtrl::OnHdnEnddrag)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CxSkinListCtrl::OnLvnColumnclick)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CxSkinListCtrl::OnLvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CxSkinListCtrl::OnLvnItemchanged)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ENDTRACKA, 0, &CxSkinListCtrl::OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, &CxSkinListCtrl::OnHdnEndtrack)
END_MESSAGE_MAP()



// CxSkinListCtrl message handlers

BOOL CxSkinListCtrl::InitSkin(CDC *pDC, CWnd* pWnd)
{
	FreeSkin();
	CSkinUtil skinUtil;

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

	TiXmlDocument xmldoc;
	if(!xmldoc.LoadFile((CStringA)m_strSkinFile)){
		return FALSE;
	}

	node = xmldoc.FirstChild(DEF_LIST);	
	if(node == NULL) return FALSE;

	itemElement = node->ToElement();
	if(itemElement == NULL) return FALSE;
	int nID = 0;
	int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);

	CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
	TRACE("AttachList::Node=%s, Name=%s\n", node->Value(), strCap);
	CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
	CString	strColList		= ConvStr::utf8ToCstr(itemElement->Attribute("ColumnList"));
	CString	strImage		= ConvStr::utf8ToCstr(itemElement->Attribute("Image"));
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

	CString strBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
	CString strTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
	CString strOddBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("OddBkColor"));
	CString strEvenBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("EvenBkColor"));
	CString strSelectBkColor	= ConvStr::utf8ToCstr(itemElement->Attribute("SelectBkColor"));
	CString strSelectTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("SelectTextColor"));
	CString strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("FocusBkColor"));
	CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FocusTextColor"));

	COLORREF clrBk			= skinUtil.StringToRGB(strBkColor);
	COLORREF clrText		= skinUtil.StringToRGB(strTextColor);
	COLORREF clrOddBk		= skinUtil.StringToRGB(strOddBkColor);
	COLORREF clrEvenBk		= skinUtil.StringToRGB(strEvenBkColor);
	COLORREF clrFocusText	= skinUtil.StringToRGB(strFocusTextColor);
	COLORREF clrFocusBk		= skinUtil.StringToRGB(strFocusBkColor);
	COLORREF clrSelectText	= skinUtil.StringToRGB(strSelectTextColor);
	COLORREF clrSelectBk	= skinUtil.StringToRGB(strSelectBkColor);

	int nRowHeight = 12;
	nret	= itemElement->QueryIntAttribute("RowHeight", &nRowHeight);
	CString strListBk	= ConvStr::utf8ToCstr(itemElement->Attribute("ListBkImage"));

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
	if(bUseHeader)
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
	CString strScrollMode		= ConvStr::utf8ToCstr(itemElement->Attribute("ScrollMode"));
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


	SetListRect(Rect_Pos);

	CString strImageFile = skinUtil._GetPath(m_strSkinFile) + strImage;
	if(_taccess(strImageFile, 0) != 0)
		return FALSE;

	CSize mSize;
	skinUtil.LoadPictureFile(pDC->m_hDC, strImageFile, &m_bmSkinImage, mSize);

	if (strListBk != _T(""))//배경이미지
	{
		CRect Rect_bkImage		= skinUtil.StringToRect(strListBk);
		skinUtil.CopyBitmap(pDC, m_bmListBkImage, m_bmSkinImage, Rect_bkImage);
		SetUseBkImage(TRUE);
	}	

	if (bUseHeader)
	{
		CopyListHeaderFromBG(m_bmSkinImage, Rect_HeadCheck, Rect_HeadUnCheck, 
			Rect_HeadLeft, Rect_HeadBkCNormal, Rect_HeadBkCOver, Rect_HeadBkCDown, Rect_HeadBkCSelect, Rect_HeadDivider, Rect_HeadDividerDown, Rect_HeadRight,
			Rect_HeadAsc, Rect_HeadDesc);

		CopyListCheckFromBG(m_bmSkinImage, Rect_OddChecked, Rect_OddUnChecked, Rect_EvenChecked, Rect_EvenUnChecked, Rect_FocusChecked, Rect_FocusUnChecked);
	}

	if(m_bShowVScroll)
	{
#if USE_TRANSPARENTBAR
		CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertCH);
#else
		CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
			Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
			Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif
	}

	if(m_bShowHScroll)
	{

#if USE_TRANSPARENTBAR
		CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizCH);
#else
		CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
			Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
			Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif
	}

	SetUseHeaderCtrl(bUseHeader);
	SetUseCheck(bCheckbox);
	SetUseDragDrop(bDragDrop);			
	SetNormalColor(clrText, clrOddBk, clrEvenBk);
	SetFocusColor(clrFocusText, clrFocusBk);
	SetSelectColor(clrSelectText, clrSelectBk);
	SetBkColor(clrBk);
	int sbMode = SB_BOTH;
	if(strScrollMode.CompareNoCase(_T("HORZ")) == 0)
		sbMode = SB_HORZ;
	else if(strScrollMode.CompareNoCase(_T("VERT")) == 0)
		sbMode = SB_VERT;

	SetScrollMode(sbMode);
	SetScrollGripColor(clrGripBk, clrGripOutline);
	SetHeadBackColor(clrHeadBk);
	SetHeadTxtColor(clrHeadTxt);		
	SetUseSort(bSorting);
	SetHeadFontBold(bHeaderBold);
	SetMinColumnSize(nMinThumbSize);

	SetMinThumbSize(nMinThumbSize);
	SetFontSize(nFontSize, nHeaderFontSize);
	SetFontName(strFont, strHeaderFont);
	SetRowHeight(nRowHeight);
	SetHeaderHeight(nHeaderHeight);

	SetTransparentThumb(bTransThumb);
	SetCaption(strCap);

	if(m_bmSkinImage.m_hObject) m_bmSkinImage.DeleteObject();	

	xmldoc.Clear();

	return TRUE;
}

void CxSkinListCtrl::FreeSkin(void)
{
	if(m_bmSkinImage.m_hObject)
		m_bmSkinImage.DeleteObject();

	if(m_bmVSC_TopBtn_N.m_hObject)
		m_bmVSC_TopBtn_N.DeleteObject();

	if(m_bmVSC_TopBtn_O.m_hObject)
		m_bmVSC_TopBtn_O.DeleteObject();

	if(m_bmVSC_TopBtn_D.m_hObject)
		m_bmVSC_TopBtn_D.DeleteObject();

	if(m_bmVSC_BottomBtn_N.m_hObject)
		m_bmVSC_BottomBtn_N.DeleteObject();

	if(m_bmVSC_BottomBtn_O.m_hObject)
		m_bmVSC_BottomBtn_O.DeleteObject();

	if(m_bmVSC_BottomBtn_D.m_hObject)
		m_bmVSC_BottomBtn_D.DeleteObject();

	if(m_bmVSC_Thumb_T_N.m_hObject)
		m_bmVSC_Thumb_T_N.DeleteObject();

	if(m_bmVSC_Thumb_T_O.m_hObject)
		m_bmVSC_Thumb_T_O.DeleteObject();

	if(m_bmVSC_Thumb_T_D.m_hObject)
		m_bmVSC_Thumb_T_D.DeleteObject();

	if(m_bmVSC_Thumb_C_N.m_hObject)
		m_bmVSC_Thumb_C_N.DeleteObject();

	if(m_bmVSC_Thumb_C_O.m_hObject)
		m_bmVSC_Thumb_C_O.DeleteObject();

	if(m_bmVSC_Thumb_C_D.m_hObject)
		m_bmVSC_Thumb_C_D.DeleteObject();

	if(m_bmVSC_Thumb_B_N.m_hObject)
		m_bmVSC_Thumb_B_N.DeleteObject();

	if(m_bmVSC_Thumb_B_O.m_hObject)
		m_bmVSC_Thumb_B_O.DeleteObject();

	if(m_bmVSC_Thumb_B_D.m_hObject)
		m_bmVSC_Thumb_B_D.DeleteObject();

#if USE_TRANSPARENTBAR
	if(m_bmVSC_Channel.m_hObject)
		m_bmVSC_Channel.DeleteObject();
#else
	if(m_bmVSC_ChannelTop.m_hObject)
		m_bmVSC_ChannelTop.DeleteObject();

	if(m_bmVSC_ChannelMid.m_hObject)
		m_bmVSC_ChannelMid.DeleteObject();

	if(m_bmVSC_ChannelBtm.m_hObject)
		m_bmVSC_ChannelBtm.DeleteObject();
#endif

	if(m_bmHSC_RigthBtn_D.m_hObject)
		m_bmHSC_RigthBtn_D.DeleteObject();

	if(m_bmHSC_RigthBtn_O.m_hObject)
		m_bmHSC_RigthBtn_O.DeleteObject();

	if(m_bmHSC_RigthBtn_N.m_hObject)
		m_bmHSC_RigthBtn_N.DeleteObject();

	if(m_bmHSC_LeftBtn_D.m_hObject)
		m_bmHSC_LeftBtn_D.DeleteObject();

	if(m_bmHSC_LeftBtn_O.m_hObject)
		m_bmHSC_LeftBtn_O.DeleteObject();

	if(m_bmHSC_LeftBtn_N.m_hObject)
		m_bmHSC_LeftBtn_N.DeleteObject();

#if USE_TRANSPARENTBAR
	if(m_bmHSC_Channel.m_hObject)
		m_bmHSC_Channel.DeleteObject();
#else	
	if(m_bmHSC_ChannelTop.m_hObject)
		m_bmHSC_ChannelTop.DeleteObject();

	if(m_bmHSC_ChannelMid.m_hObject)
		m_bmHSC_ChannelMid.DeleteObject();

	if(m_bmHSC_ChannelBtm.m_hObject)
		m_bmHSC_ChannelBtm.DeleteObject();
#endif

	if(m_bmHSC_Thumb_B_D.m_hObject)
		m_bmHSC_Thumb_B_D.DeleteObject();

	if(m_bmHSC_Thumb_B_O.m_hObject)
		m_bmHSC_Thumb_B_O.DeleteObject();

	if(m_bmHSC_Thumb_B_N.m_hObject)
		m_bmHSC_Thumb_B_N.DeleteObject();

	if(m_bmHSC_Thumb_C_D.m_hObject)
		m_bmHSC_Thumb_C_D.DeleteObject();

	if(m_bmHSC_Thumb_C_O.m_hObject)
		m_bmHSC_Thumb_C_O.DeleteObject();

	if(m_bmHSC_Thumb_C_N.m_hObject)
		m_bmHSC_Thumb_C_N.DeleteObject();

	if(m_bmHSC_Thumb_T_D.m_hObject)
		m_bmHSC_Thumb_T_D.DeleteObject();

	if(m_bmHSC_Thumb_T_O.m_hObject)
		m_bmHSC_Thumb_T_O.DeleteObject();

	if(m_bmHSC_Thumb_T_N.m_hObject)
		m_bmHSC_Thumb_T_N.DeleteObject();

	if(m_bmOddCheck.m_hObject)
		m_bmOddCheck.DeleteObject();
	if(m_bmOddUnCheck.m_hObject)
		m_bmOddUnCheck.DeleteObject();
	if(m_bmEvenCheck.m_hObject)
		m_bmEvenCheck.DeleteObject();
	if(m_bmEvenUnCheck.m_hObject)
		m_bmEvenUnCheck.DeleteObject();
	if(m_bmFocusCheck.m_hObject)
		m_bmFocusCheck.DeleteObject();
	if(m_bmFocusUnCheck.m_hObject)
		m_bmFocusUnCheck.DeleteObject();

	if(m_bmListBkImage.m_hObject)
		m_bmListBkImage.DeleteObject();
}


CImageList*	CxSkinListCtrl::CreateDragImageEx(LPPOINT lpPoint)
{
	if (GetSelectedCount() <= 0)
		return NULL;

	CRect rectSingle;
	CRect rectComplete(0,0,0,0);

	// Determine List Control Client width size
	GetClientRect(&rectSingle);

	int nWidth  = rectSingle.Width();

	int nIndex = GetTopIndex() - 1;
	int nBottomIndex = GetTopIndex() + GetCountPerPage() - 1;

	if (nBottomIndex > (GetItemCount() - 1))
		nBottomIndex = GetItemCount() - 1;

	while ((nIndex = this->GetNextItem(nIndex, LVNI_SELECTED)) != -1)
	{
		if (nIndex > nBottomIndex)
			break; 

		this->GetItemRect(nIndex, rectSingle, LVIR_ICON|LVIR_LABEL);

		if (rectSingle.left < 0) 
			rectSingle.left = 0;

		if (rectSingle.right > nWidth)
			rectSingle.right = nWidth;

		rectComplete.UnionRect(rectComplete, rectSingle);
	}

	CClientDC dcClient(this);
	CDC dcMem;
	CBitmap Bitmap;
	CPoint		cPt;
	int nCnt;
	nCnt = 0;

	if (!dcMem.CreateCompatibleDC(&dcClient))
		return NULL;

	if (!Bitmap.CreateCompatibleBitmap(&dcClient, 1, 1))
		return NULL;

	CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);

	dcMem.SelectObject(pOldMemDCBitmap);

	CImageList* pCompleteImageList = new CImageList;

	pCompleteImageList->Create(1,1 , ILC_COLOR8, 0, 1);	
	pCompleteImageList->Add(&Bitmap, COLOR_BLACK);

	Bitmap.DeleteObject();

	if (lpPoint)
	{
		lpPoint->x = rectComplete.left;
		lpPoint->y = rectComplete.top;
	}

	return pCompleteImageList;
}

BOOL CxSkinListCtrl::SetSkin(CString strSkinFile)
{
	m_strSkinFile = strSkinFile;

	if (_taccess(m_strSkinFile, 0) != 0)
		return FALSE;

	CDC *pDC = GetDC();

	if(!InitSkin(pDC, this))
		return FALSE;

	ReleaseDC(pDC);

	return TRUE;
}

void CxSkinListCtrl::SetFontName(CString strListFont, CString strHeadFont)
{
	m_strListFont	= strListFont;
	m_strHeadFont	= strHeadFont;
}

void CxSkinListCtrl::SetFontSize(int nListSize, int nHeadSize)
{
	m_nListFontSize = nListSize;
	m_nHeadFontSize = nHeadSize;
}

void CxSkinListCtrl::ShowScroll(BOOL bShowVScroll, BOOL bShowHScroll)
{
	m_bShowVScroll	= bShowVScroll;
	m_bShowHScroll	= bShowHScroll;
}

void CxSkinListCtrl::AddColumnInfo(COLUMNINFO* pColInfo)
{
	if(pColInfo == NULL)
		return;

	LVCOLUMN lvc;
	memset(&lvc, 0, sizeof(LVCOLUMN));

	int nCnt = m_ListHeader.GetItemCount();
	GetColumn(nCnt, &lvc);

	//CString strHead;
	//VERIFY(strHead.LoadString(pColInfo->nColHdr));
	lvc.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem	= nCnt;
	lvc.pszText		= (LPTSTR)(LPCTSTR)pColInfo->strText;
	lvc.fmt			= pColInfo->nColFmt;
	lvc.cx			= pColInfo->nColWidth;

	if(pColInfo->nColFmt == LVCFMT_LEFT)
	{
		CString strTmp;
	}
	InsertColumn(nCnt, &lvc);

	pColInfo->nColOrder = nCnt;

	m_ArrNowCol.Add(pColInfo);
}

void CxSkinListCtrl::DeleteAllColumnInfo()
{
	for(int i = 0; i < m_ArrNowCol.GetCount(); i++)
	{
		COLUMNINFO* pColInfo = (COLUMNINFO*)m_ArrNowCol.GetAt(i);
		if(pColInfo != NULL)
		{
			delete pColInfo;
			pColInfo = NULL;
		}

	}
	m_ArrNowCol.RemoveAll();
	while(DeleteColumn(0));
}

void CxSkinListCtrl::SetColumnInfo(void)
{
	int nCnt = m_ArrColInfo.GetCount();

	for(int i=0; i < nCnt; i++)
	{
		COLUMNINFO* pColInfo = m_ArrColInfo.GetAt(i);

		if(pColInfo != NULL)
		{
			LVCOLUMN lvc;
			memset(&lvc, 0, sizeof(LVCOLUMN));

			GetColumn(i, &lvc);

			lvc.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem	= i;
			lvc.pszText		= (LPTSTR)(LPCTSTR)pColInfo->strText;//strHead;
			lvc.fmt			= pColInfo->nColFmt;
			lvc.cx			= pColInfo->nColWidth;

			InsertColumn(i, &lvc);

			m_ArrNowCol.Add(pColInfo);
		}
	}
}

void CxSkinListCtrl::SortItem(int nColumn)
{
	if(m_nSortCol == nColumn)
		m_bAscend = !m_bAscend;
	else
		m_bAscend = TRUE;

	SortItem(nColumn, m_bAscend);
}

void CxSkinListCtrl::SortItem(int nColumn, BOOL bAscend)
{
	m_nSortCol = nColumn;
	m_bAscend = bAscend;

	if(m_bUseHeaderCtrl)
	{
		m_ListHeader.SetSortImage(nColumn, bAscend);
	}
}

void CxSkinListCtrl::SetNormalColor(COLORREF clrText, COLORREF clrOdd, COLORREF clrEven)
{
	m_clrText = clrText;
	//m_clrBk	= clrBk;
	m_clrOddbk		= clrOdd;
	m_clrEvenbk		= clrEven;
}

void CxSkinListCtrl::SetFocusColor(COLORREF clrText, COLORREF clrBk)
{
	m_clrFocusText	= clrText;
	m_clrFocusBk	= clrBk;
}

void CxSkinListCtrl::SetSelectColor(COLORREF clrText, COLORREF clrBk)
{
	m_clrSelectText	= clrText;
	m_clrSelectBk	= clrBk;
}

void CxSkinListCtrl::SetHeadTxtColor(COLORREF clrHeadTxt)
{
	if(m_bUseHeaderCtrl)
	{
		m_ListHeader.SetTxtColor(clrHeadTxt);
	}
}

void CxSkinListCtrl::SetHeadBackColor(COLORREF clrHeadBk)
{
	if(m_bUseHeaderCtrl)
	{
		m_ListHeader.SetBGColor(clrHeadBk);
	}
}

void CxSkinListCtrl::DeleteAllItem()
{
	this->DeleteAllItems();
}

void CxSkinListCtrl::SetScrollGripColor(COLORREF clrBk, COLORREF clrOutline)
{
	m_clrGripBk	= clrBk;
	m_clrGripOutline = clrOutline;
}

void CxSkinListCtrl::CopyListCheckFromBG(CBitmap& bmSkinImage, CRect rcOddChecked, CRect rcOddUnChecked, CRect rcEvenChecked, 
	CRect rcEvenUnChecked, CRect rcFocusChecked, CRect rcFocusUnChecked)
{
	CDC* pDC = GetDC();

	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmOddCheck, bmSkinImage, rcOddChecked);
	skinUtil.CopyBitmap(pDC, m_bmOddUnCheck, bmSkinImage, rcOddUnChecked);
	skinUtil.CopyBitmap(pDC, m_bmEvenCheck, bmSkinImage, rcEvenChecked);
	skinUtil.CopyBitmap(pDC, m_bmEvenUnCheck, bmSkinImage, rcEvenUnChecked);
	skinUtil.CopyBitmap(pDC, m_bmFocusCheck, bmSkinImage, rcFocusChecked);
	skinUtil.CopyBitmap(pDC, m_bmFocusUnCheck, bmSkinImage, rcFocusUnChecked);

	ReleaseDC(pDC);
}

void CxSkinListCtrl::CopyListHeaderFromBG(CBitmap& bmSkinImage, CRect rcCheck, CRect rcUnCheck, 
	CRect rcLeft, CRect rcCN, CRect rcCO, CRect rcCD, CRect rcCS, CRect rcD, CRect rcDD, CRect rcRight,
	CRect rcAsc, CRect rcDesc)
{
	CDC* pDC = GetDC();

	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmChecker, bmSkinImage, rcCheck);
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmUnChecker, bmSkinImage, rcUnCheck);

	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmLeft, bmSkinImage, rcLeft);

	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmCenter, bmSkinImage, rcCN);
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmCenterOver, bmSkinImage, rcCO);
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmCenterDown, bmSkinImage, rcCD);
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmCenterSelect, bmSkinImage, rcCS);

	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmDivider, bmSkinImage, rcD);
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmDividerDown, bmSkinImage, rcDD);

	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmRight, bmSkinImage, rcRight);

	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmSortAsc, bmSkinImage, rcAsc);
	skinUtil.CopyBitmap(pDC, m_ListHeader.m_bmSortDesc, bmSkinImage, rcDesc);

	m_ListHeader.SetCheckWH(rcCheck.Width(), rcCheck.Height());
	m_ListHeader.SetBkWH(rcLeft.Width(), rcCN.Width(), rcRight.Width(), rcD.Width(), rcCN.Height());
	m_ListHeader.SetSortWH(rcAsc.Width(), rcAsc.Height());

	ReleaseDC(pDC);
}

#if USE_TRANSPARENTBAR
void CxSkinListCtrl::CopyListVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
	CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
	CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
	CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
	CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
	CRect rcVCH)
#else
void CxSkinListCtrl::CopyListVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
	CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
	CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
	CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
	CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
	CRect rcVTCH, CRect rcVMCH, CRect rcVBCH)
#endif
{
	CDC* pDC = GetDC();

	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmVSC_TopBtn_N, bmSkinImage, rcVTBN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_TopBtn_O, bmSkinImage, rcVTBO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_TopBtn_D, bmSkinImage, rcVTBD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_BottomBtn_N, bmSkinImage, rcVBBN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_BottomBtn_O, bmSkinImage, rcVBBO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_BottomBtn_D, bmSkinImage, rcVBBD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_T_N, bmSkinImage, rcVTHTN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_T_O, bmSkinImage, rcVTHTO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_T_D, bmSkinImage, rcVTHTD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_C_N, bmSkinImage, rcVTHMN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_C_O, bmSkinImage, rcVTHMO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_C_D, bmSkinImage, rcVTHMD);

	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_B_N, bmSkinImage, rcVTHBN);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_B_O, bmSkinImage, rcVTHBO);
	skinUtil.CopyBitmap(pDC, m_bmVSC_Thumb_B_D, bmSkinImage, rcVTHBD);

#if USE_TRANSPARENTBAR
	skinUtil.CopyBitmap(pDC, m_bmVSC_Channel, bmSkinImage, rcVCH);
#else
	skinUtil.CopyBitmap(pDC, m_bmVSC_ChannelTop, bmSkinImage, rcVTCH);
	skinUtil.CopyBitmap(pDC, m_bmVSC_ChannelMid, bmSkinImage, rcVMCH);
	skinUtil.CopyBitmap(pDC, m_bmVSC_ChannelBtm, bmSkinImage, rcVBCH);
#endif

	m_nSBWidth		= rcVTHTN.Width();
	m_nSBHeight		= rcVTHTN.Height();

	ReleaseDC(pDC);

	m_nVThumbTWidth		= rcVTHTN.Width();
	m_nVThumbTHeight	= rcVTHTN.Height();
	m_nVThumbMWidth		= rcVTHMN.Width();
	m_nVThumbMHeight	= rcVTHMN.Height();
	m_nVThumbBWidth		= rcVTHBN.Width();
	m_nVThumbBHeight	= rcVTHBN.Height();
#if USE_TRANSPARENTBAR
	m_nVChanWidth		= rcVCH.Width();
	m_nVChanHeight		= rcVCH.Height();
#else
	m_nVChanTWidth		= rcVTCH.Width();
	m_nVChanTHeight		= rcVTCH.Height();
	m_nVChanMWidth		= rcVMCH.Width();
	m_nVChanMHeight		= rcVMCH.Height();
	m_nVChanBWidth		= rcVBCH.Width();
	m_nVChanBHeight		= rcVBCH.Height();
#endif
}

#if USE_TRANSPARENTBAR
void CxSkinListCtrl::CopyListHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
	CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
	CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
	CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
	CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
	CRect rcHCH)
#else
void CxSkinListCtrl::CopyListHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
	CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
	CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
	CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
	CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
	CRect rcHTCH, CRect rcHMCH, CRect rcHBCH)
#endif
{
	CDC* pDC = GetDC();

	CSkinUtil skinUtil;
	skinUtil.CopyBitmap(pDC, m_bmHSC_LeftBtn_N, bmSkinImage, rcHLBN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_LeftBtn_O, bmSkinImage, rcHLBO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_LeftBtn_D, bmSkinImage, rcHLBD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_RigthBtn_N, bmSkinImage, rcHRBN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_RigthBtn_O, bmSkinImage, rcHRBO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_RigthBtn_D, bmSkinImage, rcHRBD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_T_N, bmSkinImage, rcHTHTN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_T_O, bmSkinImage, rcHTHTO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_T_D, bmSkinImage, rcHTHTD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_C_N, bmSkinImage, rcHTHMN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_C_O, bmSkinImage, rcHTHMO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_C_D, bmSkinImage, rcHTHMD);

	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_B_N, bmSkinImage, rcHTHBN);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_B_O, bmSkinImage, rcHTHBO);
	skinUtil.CopyBitmap(pDC, m_bmHSC_Thumb_B_D, bmSkinImage, rcHTHBD);

#if USE_TRANSPARENTBAR
	skinUtil.CopyBitmap(pDC, m_bmHSC_Channel, bmSkinImage, rcHCH);
#else
	skinUtil.CopyBitmap(pDC, m_bmHSC_ChannelTop, bmSkinImage, rcHTCH);
	skinUtil.CopyBitmap(pDC, m_bmHSC_ChannelMid, bmSkinImage, rcHMCH);
	skinUtil.CopyBitmap(pDC, m_bmHSC_ChannelBtm, bmSkinImage, rcHBCH);
#endif

	m_nSBWidth		= rcHLBN.Width();
	m_nSBHeight		= rcHLBN.Height();

	ReleaseDC(pDC);

	m_nHThumbTWidth		= rcHTHTN.Width();
	m_nHThumbTHeight	= rcHTHTN.Height();
	m_nHThumbMWidth		= rcHTHMN.Width();
	m_nHThumbMHeight	= rcHTHMN.Height();
	m_nHThumbBWidth		= rcHTHBN.Width();
	m_nHThumbBHeight	= rcHTHBN.Height();
#if USE_TRANSPARENTBAR
	m_nHChanWidth		= rcHCH.Width();
	m_nHChanHeight		= rcHCH.Height();
#else
	m_nHChanTWidth		= rcHTCH.Width();
	m_nHChanTHeight		= rcHTCH.Height();
	m_nHChanMWidth		= rcHMCH.Width();
	m_nHChanMHeight		= rcHMCH.Height();
	m_nHChanBWidth		= rcHBCH.Width();
	m_nHChanBHeight		= rcHBCH.Height();
#endif
}

#if USE_COOL_SB
LRESULT CxSkinListCtrl::_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm)
{ 
	UNREFERENCED_PARAMETER(ctrlid);

#if USE_TRANSPARENTBAR
	int nWidthCC, nHeightCC;
#else
	int nWidthCCT, nHeightCCT, nWidthCCM, nHeightCCM, nWidthCCB, nHeightCCB;
#endif
	int nHeightT, nHeightC, nHeightB, nWidthT, nWidthC, nWidthB, nStartH=0,nStartW=0;;

	CDC *pDC = CDC::FromHandle(nm->hdc);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	if(memDC == NULL)
		return CDRF_DODEFAULT;                        

	CBitmap *pOldBitmap;

	if(nm->nBar == SB_BOTH)
	{//the sizing gripper in the bottom-right corner
		RECT *rc = &nm->rect;

		pDC->FillSolidRect(rc, m_clrGripOutline);
		rc->top += 1;
		rc->left += 1;
		pDC->FillSolidRect(rc, m_clrGripBk);

		return CDRF_SKIPDEFAULT;

	}
	else if(nm->nBar == SB_HORZ)
	{	
		//nWidthCC = HSC_Channel.Width();
		//nHeightCC = HSC_Channel.Height();

		//nWidthT = HSC_Thumb_T_N.Width();
		//nHeightT = HSC_Thumb_T_N.Height();
		//nHeightC = HSC_Thumb_C_N.Height();
		//nWidthC = HSC_Thumb_C_N.Width();
		//nWidthB = HSC_Thumb_B_N.Width();
		//nHeightB = HSC_Thumb_B_N.Height();
#if USE_TRANSPARENTBAR
		nWidthCC	= m_nHChanWidth;
		nHeightCC	= m_nHChanHeight;
#else
		nWidthCCT	= m_nHChanTWidth;
		nHeightCCT	= m_nHChanTHeight;
		nWidthCCM	= m_nHChanMWidth;
		nHeightCCM	= m_nHChanMHeight;
		nWidthCCB	= m_nHChanBWidth;
		nHeightCCB	= m_nHChanBHeight;
#endif

		nWidthT		= m_nHThumbTWidth;
		nHeightT	= m_nHThumbTHeight;
		nWidthC		= m_nHThumbMWidth;
		nHeightC	= m_nHThumbMHeight;
		nWidthB		= m_nHThumbBWidth;
		nHeightB	= m_nHThumbBHeight;

		if(nm->uState == CDIS_HOT)
		{			
			switch(nm->uItem)
			{
			case HTSCROLL_LEFT: //up arrow   ACTIVE
				{					
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_LeftBtn_O);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_RIGHT: //down arrow ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_RigthBtn_O);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);				
					break;
				}
			case HTSCROLL_PAGELEFT: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGERIGHT: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					CBitmap *pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //horz thumb
				{
					CDC objDC;
					CRect ThumbRect;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);					

					int nCenter =(ThumbRect.Width()-nWidthC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}
					//nStartH++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelMid);
					memDC.StretchBlt(nWidthCCT, 0, ThumbRect.Width()-(nWidthCCT+nWidthCCB), nHeightCCM, &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelBtm);
					memDC.BitBlt(ThumbRect.Width()-nWidthCCB, 0, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_O);
						memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_C_O);
						memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
						memDC.TransparentBlt(nCenter, nStartH, nWidthC, nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
						memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_B_O);
						memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_O);
						memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_C_O);
						memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
						memDC.BitBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_B_O);
						memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmHSC_Thumb_T_O);
					////memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmHSC_Thumb_C_O);
					////memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
					////memDC.BitBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightC, &objDC, 0, 0, SRCCOPY);
					////memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
					//memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
					//memDC.TransparentBlt(nCenter, nStartH, nWidthC, nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
					//memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmHSC_Thumb_B_O);
					////memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					//////////////////////////////////////////////////////////////////////////

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);

					DeleteObject(bmpBuffer.m_hObject);					
					break;
				}
			}
		}
		else if(nm->uState == CDIS_SELECTED) 
		{
			switch(nm->uItem)
			{
			case HTSCROLL_LEFT: //up arrow   ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_LeftBtn_D);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					break;
				}
			case HTSCROLL_RIGHT: //down arrow ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_RigthBtn_D);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					break;
				}
			case HTSCROLL_PAGELEFT: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);						
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGERIGHT: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);					
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//pDC->TransparentBlt(-1, -1, 0, 0, &memDC,0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //horz thumb
				{
					CRect ThumbRect;
					CDC objDC;						
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter = (ThumbRect.Width()-nWidthC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}

					//nStartH++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					//////////////////////////////////////////////////////////////////////////
					

					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelMid);
					memDC.StretchBlt(nWidthCCT, 0, ThumbRect.Width()-(nWidthCCT+nWidthCCB), nHeightCCM, &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelBtm);
					memDC.BitBlt(ThumbRect.Width()-nWidthCCB, 0, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_D);						
						memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_C_D);
						memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
						memDC.TransparentBlt(nCenter, nStartH, nWidthC, nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
						memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_B_D);						
						memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_D);						
						memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_C_D);						
						memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
						memDC.BitBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_B_D);						
						memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmHSC_Thumb_T_D);						
					////memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmHSC_Thumb_C_D);						
					////memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
					////memDC.BitBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightC, &objDC, 0, 0, SRCCOPY);
					////memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
					//memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
					//memDC.TransparentBlt(nCenter, nStartH, nWidthC, nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
					//memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
					//
					//objDC.SelectObject(&m_bmHSC_Thumb_B_D);						
					////memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					//////////////////////////////////////////////////////////////////////////

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);

					DeleteObject(bmpBuffer.m_hObject);

					break;
				}
			}
		}
		else
		{
			switch(nm->uItem)
			{
			case HTSCROLL_LEFT: //up arrow   NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_LeftBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);				
					break;
				}
			case HTSCROLL_RIGHT: //down arrow NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_RigthBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					//pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, 14, 14, SRCCOPY);
					break;
				}
			case HTSCROLL_PAGELEFT: //page up	 NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGERIGHT: //page down  NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmHSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_THUMB: //horz thumb
				{
					CRect ThumbRect;
					CDC objDC;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);


					int nCenter =(ThumbRect.Width()-nWidthC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}
					//nStartH++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelMid);
					memDC.StretchBlt(nWidthCCT, 0, ThumbRect.Width()-(nWidthCCT+nWidthCCB), nHeightCCM, &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmHSC_ChannelBtm);
					memDC.BitBlt(ThumbRect.Width()-nWidthCCB, 0, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmHSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_N);
						memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_C_N);
						memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
						memDC.TransparentBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
						memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmHSC_Thumb_B_N);
						memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmHSC_Thumb_T_N);
						memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_C_N);
						memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);				
						memDC.BitBlt(nCenter, nStartH, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);

						objDC.SelectObject(&m_bmHSC_Thumb_B_N);
						memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmHSC_Thumb_T_N);
					//memDC.BitBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					////memDC.TransparentBlt(0, nStartH, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmHSC_Thumb_C_N);
					//memDC.StretchBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);				
					//memDC.BitBlt(nCenter, nStartH, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
					//memDC.StretchBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0,0,1, nHeightT,SRCCOPY);
					////memDC.TransparentBlt(nWidthT, nStartH, nCenter, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
					////memDC.TransparentBlt(nCenter, nStartH, ThumbRect.Width() - (nWidthT+nWidthB), nHeightT, &objDC, 0, 0, 1, nHeightC, COLOR_WHITE);
					////memDC.TransparentBlt(nCenter+nWidthC, nStartH, nCenter-nWidthB, nHeightT, &objDC, 0, 0, 1, nHeightT, COLOR_WHITE);
					//
					//objDC.SelectObject(&m_bmHSC_Thumb_B_N);
					//memDC.BitBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					////memDC.TransparentBlt(ThumbRect.Width() - nWidthB, nStartH, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB, COLOR_WHITE);

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);
					memDC.SelectObject(pOldBitmap);
					DeleteDC(memDC);
					DeleteObject(bmpBuffer.m_hObject);

					break;
				}
			}//__End Switch
		}//__End if

		return CDRF_SKIPDEFAULT;
	}
	else if(nm->nBar == SB_VERT)
	{		
		//nWidthCC = VSC_Channel.Width();
		//nHeightCC = VSC_Channel.Height();

		//nWidthT = VSC_Thumb_T_N.Width();
		//nHeightT = VSC_Thumb_T_N.Height();
		//nHeightC = VSC_Thumb_C_N.Height();
		//nWidthC = VSC_Thumb_C_N.Width();
		//nWidthB = VSC_Thumb_B_N.Width();
		//nHeightB = VSC_Thumb_B_N.Height();
#if USE_TRANSPARENTBAR
		nWidthCC	= m_nVChanWidth;
		nHeightCC	= m_nVChanHeight;
#else
		nWidthCCT	= m_nVChanTWidth;
		nHeightCCT	= m_nVChanTHeight;
		nWidthCCM	= m_nVChanMWidth;
		nHeightCCM	= m_nVChanMHeight;
		nWidthCCB	= m_nVChanBWidth;
		nHeightCCB	= m_nVChanBHeight;
#endif
		nWidthT		= m_nVThumbTWidth;
		nHeightT	= m_nVThumbTHeight;
		nWidthC		= m_nVThumbMWidth;
		nHeightC	= m_nVThumbMHeight;
		nWidthB		= m_nVThumbBWidth;
		nHeightB	= m_nVThumbBHeight;


		if(nm->uState == CDIS_HOT) 
		{
			switch(nm->uItem)
			{
			case HTSCROLL_UP: //up arrow   ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_TopBtn_O);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_DOWN: //down arrow ACTIVE
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_BottomBtn_O);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_PAGEGUP: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGEGDOWN: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(-1, -1, 0, 0, &memDC,0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //vert thumb
				{
					CRect ThumbRect;
					CDC objDC;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter =( ThumbRect.Height()-nHeightC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Height()-nHeightC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Width() > nWidthT){
						nStartW = (ThumbRect.Width() - nWidthT)/2;
					}
					//nStartW++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//memDC.TransparentBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelMid);
					memDC.StretchBlt(0, nHeightCCT, nWidthCCM, ThumbRect.Height()-(nHeightCCT+nHeightCCB), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelBtm);
					memDC.BitBlt(0, ThumbRect.Height() - nHeightB, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////

#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_O);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_C_O);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_O);
						memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_O);
						memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_C_O);
						memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_B_O);
						memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					
					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					DeleteDC(objDC);
					DeleteObject(bmpBuffer.m_hObject);
					break;
				}		
			}
		}
		else if(nm->uState == CDIS_SELECTED) 
		{
			switch(nm->uItem)
			{				
			case HTSCROLL_UP: //up arrow   ACTIVE
				{					
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_TopBtn_D);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_DOWN: //down arrow ACTIVE
				{					
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_BottomBtn_D);					
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);					
					break;
				}
			case HTSCROLL_PAGEGUP: //page up	 ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);					
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGEGDOWN: //page down  ACTIVE
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);				
					pDC->StretchBlt(-1, -1, 0, 0, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);	

					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(-1, -1, 0, 0, &memDC,0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //vert thumb
				{
					CRect ThumbRect;
					CDC  objDC;					
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter =(ThumbRect.Height() - nHeightC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Height()-nHeightC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Width() > nWidthT){
						nStartW = (ThumbRect.Width() - nWidthT)/2;
					}
					//nStartW++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//memDC.TransparentBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelMid);
					memDC.StretchBlt(0, nHeightCCT, nWidthCCM, ThumbRect.Height()-(nHeightCCT+nHeightCCB), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelBtm);
					memDC.BitBlt(0, ThumbRect.Height() - nHeightB, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_D);
						//memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_C_D);
						//memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						//memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
						//memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_D);
						//memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
						memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);

					}
					else
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_D);
						memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_C_D);
						memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_B_D);
						memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					//objDC.SelectObject(&m_bmVSC_Thumb_T_D);
					////memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_C_D);
					////memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
					////memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
					////memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
					//memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_B_D);
					////memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					//////////////////////////////////////////////////////////////////////////

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);
					DeleteObject(bmpBuffer.m_hObject);
					break;
				}
			}
		}
		else
		{
			switch(nm->uItem)
			{
			case HTSCROLL_UP: //up arrow   NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_TopBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					//pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, 14, 14, SRCCOPY);
					break;
				}
			case HTSCROLL_DOWN: //down arrow NORMAL
				{
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_BottomBtn_N);
					pDC->BitBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, SRCCOPY);
					//pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, 14, 14, SRCCOPY);
					break;
				}
			case HTSCROLL_PAGEGUP: //page up	 NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case HTSCROLL_PAGEGDOWN: //page down  NORMAL
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCC, nHeightCC, RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_ChannelMid);	
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right - nm->rect.left, nm->rect.bottom - nm->rect.top, &memDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
#endif
					break;
				}
			case 4: //padding
				{
#if USE_TRANSPARENTBAR
					pOldBitmap = (CBitmap*)memDC.SelectObject(&m_bmVSC_Channel);
					pDC->StretchBlt(nm->rect.left, nm->rect.top, nm->rect.right-nm->rect.left, nm->rect.bottom-nm->rect.top, &memDC, -1, -1, -1, -1, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//pDC->TransparentBlt(nm->rect.left, nm->rect.top, nm->rect.right-nm->rect.left, nm->rect.bottom-nm->rect.top, &memDC, -1, -1, -1, -1, RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
#endif
					break;
				}
			case HTSCROLL_THUMB: //vert thumb
				{
					CRect ThumbRect;
					CDC objDC;
					objDC.CreateCompatibleDC(pDC);

					ThumbRect.SetRect(nm->rect.left, nm->rect.top, nm->rect.right, nm->rect.bottom);
					CBitmap bmpBuffer;
					bmpBuffer.CreateCompatibleBitmap(pDC, ThumbRect.Width(), ThumbRect.Height());
					pOldBitmap = (CBitmap*)memDC.SelectObject(&bmpBuffer);

					int nCenter =( ThumbRect.Height()-nHeightC)/2;

					//Thumb 패턴이 1픽셀 못그리는 현상이 발생하여 나머지값을 반올림 처리
					int nRest = (ThumbRect.Height()-nHeightC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}
					//

					if(ThumbRect.Width() > nWidthT){
						nStartW = (ThumbRect.Width()- nWidthT)/2;
					}
					//nStartW++;

#if USE_TRANSPARENTBAR
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_Channel);
					memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC, SRCCOPY);
					//GradientFillRectScroll(pDC);
					//memDC.TransparentBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCC, nHeightCC,RGB(255,0,255));
					//GradientFillRectScroll(pDC);
#else
					//////////////////////////////////////////////////////////////////////////
					
					CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelTop);
					memDC.BitBlt(0, 0, nWidthCCT, nHeightCCT, &objDC, 0, 0, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelMid);
					memDC.StretchBlt(0, nHeightCCT, nWidthCCM, ThumbRect.Height()-(nHeightCCT+nHeightCCB), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);

					objDC.SelectObject(&m_bmVSC_ChannelBtm);
					memDC.BitBlt(0, ThumbRect.Height() - nHeightB, nWidthCCB, nHeightCCB, &objDC, 0, 0, SRCCOPY);

					//CBitmap *poldtmpbitmap = (CBitmap*)objDC.SelectObject(&m_bmVSC_ChannelMid);
					//memDC.StretchBlt(0, 0, ThumbRect.Width(), ThumbRect.Height(), &objDC, 0, 0, nWidthCCM, nHeightCCM, SRCCOPY);
					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_N);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_C_N);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_N);
						memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					}
					else
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_N);
						memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_C_N);
						memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
						memDC.BitBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);

						objDC.SelectObject(&m_bmVSC_Thumb_B_N);
						memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					}
					
					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

					//objDC.SelectObject(poldtmpbitmap);
					DeleteDC(objDC);
					DeleteObject(bmpBuffer.m_hObject);
					break;
				}
			}//__End Switch
		}//__End if
		return CDRF_SKIPDEFAULT;
	}
	else
	{
		return CDRF_DODEFAULT;

	}

	return CDRF_SKIPDEFAULT;
}
#endif
//LRESULT CxSkinListCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
//{
//	CRect rc;
//	GetWindowRect(&rc);
//
//	WINDOWPOS wp;
//	wp.hwnd = this->m_hWnd;
//	wp.cx = rc.Width();
//	wp.cy = rc.Height();
//	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER;
//
//	LRESULT lrReturn(Default());
//	SendMessage(WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&wp));
//	return lrReturn;
//
//}

void CxSkinListCtrl::OnHdnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	m_ListHeader.SetDragStatus(TRUE);

	*pResult = 0;
}

void CxSkinListCtrl::OnHdnEnddrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	m_ListHeader.SetDragStatus(FALSE);

	Invalidate(FALSE);

	*pResult = 0;
}

int CxSkinListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(IsWindow(this->GetSafeHwnd()))
	{
#if USE_COOL_SB
		BOOL bRet = InitializeSB(this->m_hWnd);
		bRet = SB_SetStyle(this->m_hWnd, SB_BOTH, CSBS_HOTTRACKED);		
		bRet = SB_SetSize(this->m_hWnd, SB_BOTH, m_nSBWidth, m_nSBHeight);		
		bRet = SB_SetMinThumbSize(this->m_hWnd, SB_BOTH, m_nMinThumbSize);	
#endif			
	}

	m_ImageList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 0, 1);

	//////////////////////////////////////////////////////////////////////////
	//리스트 Row 높이 설정
	m_ListFont.DeleteObject();
	int nSize = m_nListHeight;// - 4; //16 -> 20, 17 -> 21 18 -> 22 (-> row height)
	VERIFY(m_ListFont.CreateFont(
		nSize,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		m_strListFont));

	SetFont(&m_ListFont, TRUE);

	SetBkColor(m_clrBk);
	if (m_bUseBkImage)
		SetBkImage((HBITMAP)m_bmListBkImage, TRUE);

	InitHeader();

	return 0;
}

void CxSkinListCtrl::InitHeader()
{
	m_ListHeader.SetFontSize(m_nHeadFontSize);
	m_ListHeader.SetFontName(m_strHeadFont);
	m_ListHeader.SetFontBold(m_bHeadFontBold);
	m_ListHeader.SetCheckbox(m_bUseCheckbox);
	m_ListHeader.SetSort(m_bUseSort);
	m_ListHeader.SetCheckIndex(m_nHeadCheckIdx);
	m_ListHeader.SetCheckboxMargin(m_nHeadCheckMargin);
	m_ListHeader.SetUseDrag(m_bHeaderDrag);
	m_ListHeader.SetHeight(m_nHeadHeight);

	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if(pHeader != NULL)
		m_ListHeader.SubclassWindow(pHeader->m_hWnd);
}

void CxSkinListCtrl::SetHeader()
{
	if(m_bUseSort){
		if (m_ListHeader.GetSafeHwnd())
			m_ListHeader.SetSortImage(-1, TRUE);
	}

	//헤더를 추가한후에 숨긴다.
	if(!m_bUseHeaderCtrl)
		SetExtendedStyle(LVS_NOCOLUMNHEADER);
}

void CxSkinListCtrl::SetColumnList(CString strList)
{
	CString col;
	int i = 0;
	while(TRUE == ::AfxExtractSubString(col, strList, i, _T('|')))
	{
		LVCOLUMN lvc;
		memset(&lvc, 0, sizeof(LVCOLUMN));	
		lvc.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.iSubItem	= i;
		lvc.pszText		= (LPTSTR)(LPCTSTR)col;
		lvc.fmt			= LVCFMT_LEFT;
		lvc.cx			= 50;
		InsertColumn(i, &lvc);
		i++;
	}
}

BOOL CxSkinListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY   *pHDN = (HD_NOTIFY*)lParam;

	//if(m_bUseCheckbox)
	//{
	//	if(pHDN->hdr.code == HDN_BEGINTRACKA || pHDN->hdr.code == HDN_BEGINTRACKW)
	//	{           

	//		if(pHDN->iItem == 0 )//Do not resize "0" column (number) 
	//		{
	//			*pResult = TRUE;
	//			return true;
	//		}
	//	}
	//}

	if(!m_bHeaderDrag)
	{
		if(pHDN->hdr.code == HDN_BEGINDRAG)
		{
			*pResult = TRUE;
			return TRUE;
		}
	}

	if(m_nMinColumnSize > 0)
	{
		if((pHDN->hdr.code == HDN_ITEMCHANGINGW || pHDN->hdr.code == HDN_ITEMCHANGINGA) && pHDN->pitem->cxy < m_nMinColumnSize)
		{
			*pResult = TRUE;                // disable change
			return TRUE;                    // Processed message
		}
	}

#if USE_COOL_SB
	NMHDR *hdr = (NMHDR *)lParam;
	if(hdr->code == NM_COOLSB_CUSTOMDRAW)
	{
		NMCSBCUSTOMDRAW *nm;
		nm = (NMCSBCUSTOMDRAW *)lParam;

		if(nm->dwDrawStage == CDDS_PREPAINT)
		{
			return CDRF_SKIPDEFAULT;
		}

		if(nm->hdr.hwndFrom == m_hWnd)
			return _SBCustomDraw(wParam, nm);

		return CDRF_SKIPDEFAULT;
	}
#endif
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void CxSkinListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();

	DeleteAllItem();

	DeleteAllColumnInfo();

	m_ListFont.DeleteObject();
	m_HeaderFont.DeleteObject();

#if USE_COOL_SB
	UninitializeCoolSB(m_hWnd);
#endif
	FreeSkin();
}

void CxSkinListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	int nSelcol = pNMLV->iSubItem;

	if(m_bUseCheckbox && nSelcol == m_nHeadCheckIdx)
	{
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);

		CRect rcItem;
		m_ListHeader.GetItemRect(0, &rcItem);

		CRect rcChkbox;
		rcChkbox = rcItem;
		rcChkbox.left	= rcItem.left + m_nHeadCheckMargin;
		rcChkbox.right	= rcChkbox.left + m_ListHeader.GetCheckWidth();
		rcChkbox.top	= (rcItem.Height() - m_ListHeader.GetCheckHeight())/2;
		rcChkbox.bottom	= rcChkbox.top + m_ListHeader.GetCheckHeight();

		//체크박스내에 커서가 있을 경우,
		if(rcChkbox.PtInRect(pt))
		{
			m_bAllCheck = !m_bAllCheck;
			if(m_bAllCheck)
			{
				for(int i=0 ; i < GetItemCount() ; i++)
				{
					SetItemState( i, LVIS_SELECTED, LVIS_SELECTED);
				}

				m_ListHeader.SetAllCheck(TRUE);
			}
			else
			{
				for(int i=0 ; i < GetItemCount() ; i++)
				{
					SetItemState( i, 0, LVIS_SELECTED);
				}

				m_ListHeader.SetAllCheck(FALSE);
			}
		}
	}
	else
	{
		SortItem(nSelcol);
	}

	*pResult = 0;
}

void CxSkinListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	if (m_bUseHeaderCtrl)
	{

		BOOL bCheck = FALSE;

		LVHITTESTINFO Hitinfo;
		Hitinfo.pt = point;
		Hitinfo.flags=0;
		Hitinfo.iItem = Hitinfo.iSubItem = -1;

		m_ListHeader.SetAllCheck(FALSE);
		m_bAllCheck = false;
		m_ListHeader.Invalidate(FALSE);


		//Checkbox를 클릭
		if(ListView_SubItemHitTest(m_hWnd, &Hitinfo) != -1)
		{
			if(Hitinfo.iSubItem == m_ListHeader.GetCheckIndex() && m_bUseCheckbox)
			{
				CRect rect, ItemRect;
				GetSubItemRect(Hitinfo.iItem, Hitinfo.iSubItem, LVIR_BOUNDS, ItemRect);
				rect = ItemRect;

				if(Hitinfo.iSubItem == 0)
				{
					GetSubItemRect(Hitinfo.iItem, Hitinfo.iSubItem+1, LVIR_BOUNDS, ItemRect);
					rect.right = ItemRect.left;			
				}

				int nChkW = m_ListHeader.GetCheckWidth();
				int nChkH =  m_ListHeader.GetCheckHeight();

				//중앙정렬
				//if(rect.left >= 0)
				//	rect.left += (rect.Width() - nChkW)/2;
				//else
				//	rect.left += 9;

				//좌측정렬
				rect.left += 5;
				rect.top = rect.top + (rect.Height() - nChkH)/2;

				if( rect.PtInRect(Hitinfo.pt) )
				{
					BOOL bSelect = FALSE;
					if (GetItemState(Hitinfo.iItem, LVIS_SELECTED))
					{
						bSelect = TRUE;
					}

					bSelect = !bSelect;

					if(bSelect)
					{
						SetItemState(Hitinfo.iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
					}
					else
					{
						SetItemState(Hitinfo.iItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
					}
					InvalidateRect(&rect, true);

					return;
				}
			}
		}

		int nCnt = GetItemCount();
		if (nCnt != 0)
		{
			int nSelCnt = GetSelectedCount();

			if(nSelCnt == nCnt)
			{
				m_bAllCheck = TRUE;
				m_ListHeader.SetAllCheck(TRUE);
			}
			else
			{
				m_bAllCheck = FALSE;
				m_ListHeader.SetAllCheck(FALSE);
			}
			m_ListHeader.Invalidate(FALSE);
		}
	}
	CListCtrl::OnLButtonDown(nFlags, point);
}

void CxSkinListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CWT_Main_Win* pDlg =  (CWT_Main_Win*)AfxGetMainWnd();

	//if (m_pDragImage && m_pDragWnd && m_bDrag)
	//{
	//	CWnd* pWnd = CWnd::GetDesktopWindow();
	//	if (m_pDC != NULL)
	//	{
	//		pWnd->ReleaseDC(m_pDC);
	//		m_pDC = NULL;
	//	}

	//	m_bDrag = FALSE;
	//	ReleaseCapture();
	//	m_pDragImage->DragLeave(this);
	//	m_pDragImage->EndDrag();
	//	delete m_pDragImage;
	//	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));


	//	CPoint ptDropPoint(point);
	//	ClientToScreen(&ptDropPoint);
	//	//m_pDragImage->DragMove(ptDropPoint);
	//	// Unlock window updates (this allows the dragging image to be shown smoothly)

	//	//m_pDragImage->DragShowNolock(false);
	//	CWnd* pDropWnd = CWnd::WindowFromPoint(ptDropPoint);

	//	// Save current window pointer as the CListCtrl we are dropping onto
	//	pDropWnd->ScreenToClient(&ptDropPoint);

	//	//아이템 순서 변경
	//	if(pDropWnd->IsKindOf(RUNTIME_CLASS(CxSkinListCtrl)))
	//	{
	//		if(!ItemReorder(nFlags, point))
	//		{
	//			return;
	//		}
	//	}
	//	//다른 리스트로 이동
	//	//else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CFileListCtrl)))
	//	//{
	//	//	pDlg->m_dlgLibraryL.OnButtonToDevice(TRUE);
	//	//	CFileListCtrl* pFileList = (CFileListCtrl*)pDropWnd;
	//	//	for(int i = 0; i< pFileList->GetItemCount(); i++){
	//	//		if(pFileList->GetItemState(i, LVIS_DROPHILITED)){
	//	//			pFileList->SetItemState (i, 0, LVIS_DROPHILITED);
	//	//		}
	//	//	}

	//	//}
	//	//else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CLibTree_Music)))
	//	//{
	//	//	DropNAddPlaylist(point);
	//	//	CLibTree_Music* pTree = (CLibTree_Music*)pDropWnd;
	//	//	pTree->SelectDropTarget(NULL);

	//	//}
	//	//else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CDeviceTreeCtrl)))
	//	//{
	//	//	//SetCursor(AfxGetApp()->LoadCursor(IDC_BEGINDROP));
	//	//	//pDlg->m_dlgLibraryL.OnButtonToDevice(TRUE);
	//	//}
	//}

	CListCtrl::OnLButtonUp(nFlags, point);
}

void CxSkinListCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);


	//if (this->GetSelectedCount() <= 0)
	//	return;  // No row selected

	//m_nDragIndex = pNMLV->iItem;

	//m_bDrag = TRUE;
	//m_nDropIndex = -1;

	//POINT pt= pNMLV->ptAction;

	//m_pDragImage = CreateDragImageEx(&pt);

	//if (m_pDragImage == NULL)
	//	return;

	//CPoint ptStart = pNMLV->ptAction;
	//ClientToScreen(&ptStart);
	////ptStart -= pt;

	//m_pDragWnd = this;

	//m_pDragImage->BeginDrag(0, ptStart);
	//m_pDragImage->DragEnter(this, pNMLV->ptAction);

	//m_rcDrag.SetRectEmpty();
	//CWnd* pWnd = CWnd::GetDesktopWindow();
	//m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);

	//if (m_pDC->GetLayout() & 0x00000001)
	//	m_pDC->SetLayout(0x00000000);


	//SetCapture();

	*pResult = 0;
}

void CxSkinListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	
	//if (m_pDragImage && m_pDragWnd && m_bDrag)
	//{
	//	CPoint ptDropPoint(point);
	//	ClientToScreen(&ptDropPoint);
	//	m_pDragImage->DragMove(ptDropPoint);
	//	// Unlock window updates (this allows the dragging image to be shown smoothly)

	//	//m_pDragImage->DragShowNolock(true);
	//	CWnd* pDropWnd = CWnd::WindowFromPoint(ptDropPoint);

	//	// Save current window pointer as the CListCtrl we are dropping onto
	//	pDropWnd->ScreenToClient(&ptDropPoint);

	//	//현재 리스트
	//	if ( pDropWnd == m_pDragWnd)
	//	{
	//		if ( pDropWnd->m_hWnd == this->m_hWnd)
	//		{			
	//			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_REORDER));
	//		}

	//		//__여기서 해당 아이템에 밑줄을 그린다.
	//		CPoint					pt;
	//		LVHITTESTINFO       lvhti;

	//		::GetCursorPos(&pt);
	//		ScreenToClient(&pt);

	//		lvhti.pt = pt;

	//		ListView_HitTest(m_hWnd, &lvhti);

	//		if (lvhti.iItem == -1)
	//		{
	//			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DRAGNO));
	//			CListCtrl::OnMouseMove(nFlags, point);
	//			return;
	//		}

	//		/*if (lvhti.iItem == m_nDragIndex){
	//		CListCtrl::OnMouseMove(nFlags, point);
	//		return;
	//		}*/

	//		if ((lvhti.flags & LVHT_ONITEMLABEL == 0) && (lvhti.flags & LVHT_ONITEMSTATEICON == 0))
	//		{
	//			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DRAGNO));
	//			CListCtrl::OnMouseMove(nFlags, point);
	//			return;
	//		}

	//		CRect ItemRect;
	//		GetSubItemRect(lvhti.iItem, 0, LVIR_BOUNDS, ItemRect);


	//		CPoint ptTemp(point);
	//		if((ItemRect.top + (ItemRect.Height() / 2)) > ptTemp.y)
	//		{
	//			ItemRect.top -= 1;
	//			ItemRect.bottom = ItemRect.top + 2;
	//		}
	//		else
	//		{
	//			ItemRect.bottom += 1;
	//			ItemRect.top = ItemRect.bottom - 2;
	//		}
	//		ClientToScreen(&ItemRect);
	//		RECT RcList;
	//		GetClientRect(&RcList);
	//		int nWidth = RcList.right - RcList.left;

	//		ItemRect.left += 1;
	//		ItemRect.right = ItemRect.left + (nWidth-2);


	//		CSize size(1, 1);
	//		CBrush* pWhiteBrush = CBrush::FromHandle((HBRUSH)::GetStockObject(WHITE_BRUSH));
	//		CBrush* pDitherBrush = CDC::GetHalftoneBrush();
	//		CBrush* pBrush = pWhiteBrush;

	//		size.cx = GetSystemMetrics(SM_CXFRAME) - 1;
	//		size.cy = GetSystemMetrics(SM_CYFRAME) - 1;

	//		pBrush = pDitherBrush;


	//		// draw it and remember last size
	//		CWindowDC dc(NULL);
	//		dc.DrawDragRect(&ItemRect, size, &m_rcDrag, size,	pBrush, pBrush ? pDitherBrush : pWhiteBrush);
	//		m_rcDrag = ItemRect;
	//		//m_bDitherLast = (pBrush == pDitherBrush);
	//	}
	//	//다른 리스트
	//	//else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CFileListCtrl))){
	//	//	CWindowDC dc(NULL);
	//	//	CSize size(1, 1);
	//	//	CRect rcDummy(0,0,0,0);
	//	//	dc.DrawDragRect(&rcDummy, size, &m_DragRect, size,	NULL);
	//	//	m_DragRect = rcDummy;

	//	//	SetCursor(AfxGetApp()->LoadCursor(IDC_BEGINDROP));	
	//	//	//__Folder위를 지가고 있는지 확인
	//	//	SetDevFolderCursor(point);

	//	//}else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CLibTree_Music))){
	//	//	CWindowDC dc(NULL);
	//	//	CSize size(1, 1);
	//	//	CRect rcDummy(0,0,0,0);
	//	//	dc.DrawDragRect(&rcDummy, size, &m_DragRect, size,	NULL);
	//	//	m_DragRect = rcDummy;

	//	//	SetLibTreeCursor(point);
	//	//	/*}else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CDeviceTreeCtrl))){
	//	//	CWindowDC dc(NULL);
	//	//	CSize size(1, 1);
	//	//	CRect rcDummy(0,0,0,0);
	//	//	dc.DrawDragRect(&rcDummy, size, &m_DragRect, size,	NULL);
	//	//	m_DragRect = rcDummy;

	//	//	SetCursor(AfxGetApp()->LoadCursor(IDC_BEGINDROP));		*/
	//	//}else{
	//	//	CWindowDC dc(NULL);
	//	//	CSize size(1, 1);
	//	//	CRect rcDummy(0,0,0,0);
	//	//	dc.DrawDragRect(&rcDummy, size, &m_DragRect, size,	NULL);
	//	//	m_DragRect = rcDummy;
	//	//	SetCursor(AfxGetApp()->LoadCursor(IDC_BEGINGRAG_NO));
	//	//}

	//}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CxSkinListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
	if (nChar == VK_DELETE)
	{
		//DeleteSelectItem_Ex();
	}
	else if (GetKeyState(VK_CONTROL) < 0 && nChar == 0x41) // Ctrl+A
	{
		for(int i=0 ; i < GetItemCount() ; i++)
		{
			SetItemState( i, LVIS_SELECTED, LVIS_SELECTED);
		}
		return;
	}
	else if(GetAsyncKeyState(VK_SHIFT)< 0)
	{
		return;
	}

	if (m_pDragImage && m_pDragWnd && m_bDrag)
	{
		CPoint			pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		OnMouseMove(0, pt);
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CxSkinListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(m_bUseHeaderCtrl)
	{
		int nListCnt = GetItemCount();
		int nSelCnt = GetSelectedCount();

		if(nListCnt == nSelCnt)
		{
			m_bAllCheck = TRUE;
			m_ListHeader.SetAllCheck(TRUE);
		}
		else
		{
			m_bAllCheck = FALSE;
			m_ListHeader.SetAllCheck(FALSE);
		}
		m_ListHeader.Invalidate(FALSE);
	}
	*pResult = 0;
}

void CxSkinListCtrl::AutoSizeColumns(int col)
{
	// Call this after your list control is filled
	return;
	SetRedraw(FALSE);
	
	if (col == -1)
	{
		CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();

		if (pHeaderCtrl != NULL)
		{
			int nCol = pHeaderCtrl->GetItemCount()-1;

			if(pHeaderCtrl->GetItemCount() == 1)
			{
				CRect rcHead;
				pHeaderCtrl->GetClientRect(rcHead);
				SetColumnWidth(nCol, rcHead.Width());
			}
			else
			{
				SetColumnWidth(nCol, LVSCW_AUTOSIZE);				
			}
		}

	}
	else
	{
		int mincol = col < 0 ? 0 : col;
		int maxcol = col < 0 ? GetColumnCount()-1 : col;

		for (col = mincol; col <= maxcol; col++)
		{
			SetColumnWidth(col,LVSCW_AUTOSIZE);
			int wc1 = GetColumnWidth(col);
			SetColumnWidth(col,LVSCW_AUTOSIZE_USEHEADER);
			int wc2 = GetColumnWidth(col);
			int wc = max(MINCOLWIDTH, max(wc1,wc2));
			SetColumnWidth(col,wc);
		}
	}
	m_ListHeader.Invalidate();
	SetRedraw(TRUE);
}

int CxSkinListCtrl::GetColumnCount() const
{
	// get the header control
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	// return the number of items in it - ie. the number of columns
	return pHeader->GetItemCount();
}

void CxSkinListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	if (m_bUseHeaderCtrl)
	{
		AutoSizeColumns();
	}
}

void CxSkinListCtrl::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	if (m_bUseHeaderCtrl)
	{
		AutoSizeColumns();//디바이더 위치를 옮기고 난후,
	}
	*pResult = 0;
}
