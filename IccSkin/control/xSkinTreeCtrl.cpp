// xSkinTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinTreeCtrl.h"

#include "SkinUtil.h"
#include "IniFile.H"

#include "ConvertString.h"
#include "tinyxml.h"

// CxSkinTreeCtrl

IMPLEMENT_DYNAMIC(CxSkinTreeCtrl, CTreeCtrl)

	CxSkinTreeCtrl::CxSkinTreeCtrl()
	:m_nMinThumbSize(0)
	,m_strFontName(FONT_NAME)
	,m_nFontSize(FONT_SIZE)
	,m_nItemHeight(20)
	,m_bTransparentThumb(false)
	,m_clrGripBk(COLOR_SB_GRIP)
	,m_clrGripOutline(COLOR_SB_GRIP)
	,m_clrText(COLOR_BLACK)
	,m_clrFocusText(COLOR_WHITE)
	,m_clrBk(COLOR_WHITE)
	,m_clrFocusBk(COLOR_BK_FOCUS)
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

CxSkinTreeCtrl::~CxSkinTreeCtrl()
{

}


BEGIN_MESSAGE_MAP(CxSkinTreeCtrl, CTreeCtrl)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// CxSkinTreeCtrl message handlers

BOOL CxSkinTreeCtrl::InitSkin(CDC *pDC, CWnd* pWnd)
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

	node = xmldoc.FirstChild(DEF_TREE);	
	if(node == NULL) return FALSE;

	itemElement = node->ToElement();
	if(itemElement == NULL) return FALSE;
	int nID = 0;
	int nret = itemElement->QueryIntAttribute(DEF_ID, &nID);

	CString strCap = ConvStr::utf8ToCstr(itemElement->Attribute(DEF_CAPTION));
	TRACE("AttachTree::Node=%s, Name=%s\n", node->Value(), strCap);
	CString strPos			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_POS));
	CString strFont			= ConvStr::utf8ToCstr(itemElement->Attribute(DEF_FONTNAME));
	CString strImage		= ConvStr::utf8ToCstr(itemElement->Attribute("Image"));
	int nItemHeight = 20;
	nret = itemElement->QueryIntAttribute("ItemHeight", &nItemHeight);
	int nFontSize = FONT_SIZE;
	nret = itemElement->QueryIntAttribute(DEF_FONTSIZE, &nFontSize);
	CString strTextColor		= ConvStr::utf8ToCstr(itemElement->Attribute("TextColor"));
	CString strFocusTextColor	= ConvStr::utf8ToCstr(itemElement->Attribute("FocusTextColor"));
	CString strBkColor			= ConvStr::utf8ToCstr(itemElement->Attribute("BkColor"));
	CString strFocusBkColor		= ConvStr::utf8ToCstr(itemElement->Attribute("FocusBkColor"));
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
	CRect Rect_Pos				= skinUtil.StringToRect(strPos);
	COLORREF clrGripBk		= skinUtil.StringToRGB(strGripColor);
	COLORREF clrGripOutline	= skinUtil.StringToRGB(strGripLineColor);

	CString strImageFile = skinUtil._GetPath(m_strSkinFile) + strImage;
	if(_taccess(strImageFile, 0) != 0) return FALSE;

	CSize mSize;
	skinUtil.LoadPictureFile(pDC->m_hDC, strImageFile, &m_bmSkinImage, mSize);

#if USE_TRANSPARENTBAR
	CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
		Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
		Rect_VertCH);
#else
	CopyListVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
		Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
		Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);
#endif

#if USE_TRANSPARENTBAR
	CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
		Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
		Rect_HorizCH);
#else
	CopyListHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
		Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
		Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);
#endif

	SetScrollGripColor(clrGripBk, clrGripOutline);
	SetTreeRect(Rect_Pos);

	int sbMode = SB_BOTH;
	if(strScrollMode.CompareNoCase(_T("HORZ")) == 0)
		sbMode = SB_HORZ;
	else if(strScrollMode.CompareNoCase(_T("VERT")) == 0)
		sbMode = SB_VERT;

	SetScrollMode(sbMode);
	SetScrollGripColor(clrGripBk, clrGripOutline);
	SetFontName(strFont);
	SetFontSize(nFontSize);
	SetTxtColor(skinUtil.StringToRGB(strTextColor), skinUtil.StringToRGB(strFocusTextColor));		
	SetBackColor(skinUtil.StringToRGB(strBkColor), skinUtil.StringToRGB(strFocusBkColor));
	SetItemHeight(nItemHeight);
	SetMinThumbSize(nMinThumbSize);

	SetCaption(strCap);

	if(m_bmSkinImage.m_hObject) m_bmSkinImage.DeleteObject();	
	xmldoc.Clear();
	return TRUE;
}

void CxSkinTreeCtrl::FreeSkin()
{
	if(m_bmSkinImage.m_hObject) m_bmSkinImage.DeleteObject();
	if(m_bmVSC_TopBtn_N.m_hObject) m_bmVSC_TopBtn_N.DeleteObject();
	if(m_bmVSC_TopBtn_O.m_hObject) m_bmVSC_TopBtn_O.DeleteObject();
	if(m_bmVSC_TopBtn_D.m_hObject) m_bmVSC_TopBtn_D.DeleteObject();
	if(m_bmVSC_BottomBtn_N.m_hObject) m_bmVSC_BottomBtn_N.DeleteObject();
	if(m_bmVSC_BottomBtn_O.m_hObject)		m_bmVSC_BottomBtn_O.DeleteObject();
	if(m_bmVSC_BottomBtn_D.m_hObject)		m_bmVSC_BottomBtn_D.DeleteObject();
	if(m_bmVSC_Thumb_T_N.m_hObject)		m_bmVSC_Thumb_T_N.DeleteObject();
	if(m_bmVSC_Thumb_T_O.m_hObject)		m_bmVSC_Thumb_T_O.DeleteObject();
	if(m_bmVSC_Thumb_T_D.m_hObject)		m_bmVSC_Thumb_T_D.DeleteObject();
	if(m_bmVSC_Thumb_C_N.m_hObject)		m_bmVSC_Thumb_C_N.DeleteObject();
	if(m_bmVSC_Thumb_C_O.m_hObject)		m_bmVSC_Thumb_C_O.DeleteObject();
	if(m_bmVSC_Thumb_C_D.m_hObject)		m_bmVSC_Thumb_C_D.DeleteObject();
	if(m_bmVSC_Thumb_B_N.m_hObject)		m_bmVSC_Thumb_B_N.DeleteObject();
	if(m_bmVSC_Thumb_B_O.m_hObject)		m_bmVSC_Thumb_B_O.DeleteObject();
	if(m_bmVSC_Thumb_B_D.m_hObject)		m_bmVSC_Thumb_B_D.DeleteObject();

#if USE_TRANSPARENTBAR
	if(m_bmVSC_Channel.m_hObject) m_bmVSC_Channel.DeleteObject();
#else
	if(m_bmVSC_ChannelTop.m_hObject)		m_bmVSC_ChannelTop.DeleteObject();
	if(m_bmVSC_ChannelMid.m_hObject)		m_bmVSC_ChannelMid.DeleteObject();
	if(m_bmVSC_ChannelBtm.m_hObject)		m_bmVSC_ChannelBtm.DeleteObject();
#endif
	if(m_bmHSC_RigthBtn_D.m_hObject)		m_bmHSC_RigthBtn_D.DeleteObject();
	if(m_bmHSC_RigthBtn_O.m_hObject)		m_bmHSC_RigthBtn_O.DeleteObject();
	if(m_bmHSC_RigthBtn_N.m_hObject)		m_bmHSC_RigthBtn_N.DeleteObject();
	if(m_bmHSC_LeftBtn_D.m_hObject)		m_bmHSC_LeftBtn_D.DeleteObject();
	if(m_bmHSC_LeftBtn_O.m_hObject)		m_bmHSC_LeftBtn_O.DeleteObject();
	if(m_bmHSC_LeftBtn_N.m_hObject)		m_bmHSC_LeftBtn_N.DeleteObject();

#if USE_TRANSPARENTBAR
	if(m_bmHSC_Channel.m_hObject)		m_bmHSC_Channel.DeleteObject();
#else	
	if(m_bmHSC_ChannelTop.m_hObject)		m_bmHSC_ChannelTop.DeleteObject();
	if(m_bmHSC_ChannelMid.m_hObject)		m_bmHSC_ChannelMid.DeleteObject();
	if(m_bmHSC_ChannelBtm.m_hObject)		m_bmHSC_ChannelBtm.DeleteObject();
#endif
	if(m_bmHSC_Thumb_B_D.m_hObject)		m_bmHSC_Thumb_B_D.DeleteObject();
	if(m_bmHSC_Thumb_B_O.m_hObject)		m_bmHSC_Thumb_B_O.DeleteObject();
	if(m_bmHSC_Thumb_B_N.m_hObject)		m_bmHSC_Thumb_B_N.DeleteObject();
	if(m_bmHSC_Thumb_C_D.m_hObject)		m_bmHSC_Thumb_C_D.DeleteObject();
	if(m_bmHSC_Thumb_C_O.m_hObject)		m_bmHSC_Thumb_C_O.DeleteObject();
	if(m_bmHSC_Thumb_C_N.m_hObject)		m_bmHSC_Thumb_C_N.DeleteObject();
	if(m_bmHSC_Thumb_T_D.m_hObject)		m_bmHSC_Thumb_T_D.DeleteObject();
	if(m_bmHSC_Thumb_T_O.m_hObject)		m_bmHSC_Thumb_T_O.DeleteObject();
	if(m_bmHSC_Thumb_T_N.m_hObject)		m_bmHSC_Thumb_T_N.DeleteObject();
}

BOOL CxSkinTreeCtrl::SetSkin(CString strSkinFile)
{
	m_strSkinFile = strSkinFile;
	if (_taccess(m_strSkinFile, 0) != 0) return FALSE;

	CDC *pDC = GetDC();
	if(!InitSkin(pDC, this)) return FALSE;

	ReleaseDC(pDC);
	return TRUE;
}

void CxSkinTreeCtrl::SetTxtColor(COLORREF clrText, COLORREF clrFocusText)
{
	m_clrText = clrText;
	m_clrFocusText = clrFocusText;
	SetTextColor(m_clrText);
}

void CxSkinTreeCtrl::SetBackColor(COLORREF clrBk, COLORREF clrFocusBk)
{
	m_clrBk = clrBk;
	m_clrFocusBk = clrFocusBk;
	SetBkColor(m_clrFocusBk);
}
#if USE_COOL_SB

LRESULT CxSkinTreeCtrl::_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm)
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

					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

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
						memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,  SRCCOPY);

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
					//objDC.SelectObject(&m_bmVSC_Thumb_T_N);
					//memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					////memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_C_N);
					//memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
					//memDC.BitBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, SRCCOPY);
					//memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
					////memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
					////memDC.TransparentBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
					////memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_B_N);
					//memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					////memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);
					//////////////////////////////////////////////////////////////////////////

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

#if USE_TRANSPARENTBAR
void CxSkinTreeCtrl::CopyListVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
	CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
	CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
	CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
	CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
	CRect rcVCH)
#else
void CxSkinTreeCtrl::CopyListVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
	CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
	CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
	CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
	CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
	CRect rcVTCH, CRect rcVMCH, CRect rcVBCH)
#endif
{
	CDC* pDC = GetDC();

	m_nSBWidth		= rcVTBN.Width();
	m_nSBHeight		= rcVTBN.Height();

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
void CxSkinTreeCtrl::CopyListHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
	CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
	CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
	CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
	CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
	CRect rcHCH)
#else
void CxSkinTreeCtrl::CopyListHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
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

void CxSkinTreeCtrl::SetScrollGripColor(COLORREF clrBk, COLORREF clrOutline)
{
	m_clrGripBk	= clrBk;
	m_clrGripOutline = clrOutline;
}

int CxSkinTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(IsWindow(this->GetSafeHwnd()))
	{
#if USE_COOL_SB
		BOOL bRet = InitializeSB(this->m_hWnd);
		bRet = SB_SetStyle(this->m_hWnd, m_sbMode, CSBS_HOTTRACKED);
		bRet = SB_SetSize(this->m_hWnd, m_sbMode, m_nSBHeight, m_nSBWidth);
		bRet = SB_SetMinThumbSize(this->m_hWnd, m_sbMode, m_nMinThumbSize);
#endif
	}

	SetBkColor(m_clrBk);
	return 0;
}

void CxSkinTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
void CxSkinTreeCtrl::GradientFillRectScroll( CDC *pDC )
{
	CRect rc;
	this->GetWindowRect(rc);
	ScreenToClient(&rc);
	rc.left = rc.right - 3;
	rc.right = rc.right -2;

	COLORREF col_from	= COLOR_BLACK;
	COLORREF col_to		= RGB( 73, 71, 73 );;

	bool vert_grad = true;
	TRIVERTEX        vert[2];
	GRADIENT_RECT    mesh;

	for(int i=0;i<2;i++){
		vert[0].x      = rc.left + (i*2);
		vert[0].y      = rc.top;
		vert[0].Alpha  = 0x0000;
		vert[0].Blue   = GetBValue(col_from) << 8;
		vert[0].Green  = GetGValue(col_from) << 8;
		vert[0].Red    = GetRValue(col_from) << 8;

		vert[1].x      = rc.right + (i*2);
		vert[1].y      = rc.bottom; 
		vert[1].Alpha  = 0x0000;
		vert[1].Blue   = GetBValue(col_to) << 8;
		vert[1].Green  = GetGValue(col_to) << 8;
		vert[1].Red    = GetRValue(col_to) << 8;

		mesh.UpperLeft  = 0;
		mesh.LowerRight = 1;
#if _MSC_VER >= 1300  // only VS7 and above has GradientFill as a pDC member
		pDC->GradientFill( vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
			GRADIENT_FILL_RECT_H );
#else
		GradientFill( pDC->m_hDC, vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
			GRADIENT_FILL_RECT_H );
#endif
	}
}

void CxSkinTreeCtrl::OnDestroy()
{
	CTreeCtrl::OnDestroy();

	FreeSkin();
#if USE_COOL_SB
	UninitializeCoolSB(m_hWnd);
#endif
}

BOOL CxSkinTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR *hdr = (NMHDR *)lParam;
#if USE_COOL_SB
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
	return CTreeCtrl::OnNotify(wParam, lParam, pResult);
}