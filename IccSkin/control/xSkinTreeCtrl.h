#ifndef XSKINTREECTRL_H
#define XSKINTREECTRL_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "Define.h"

#if USE_COOL_SB
#include "Scrollbar.h"
#endif

// CxSkinTreeCtrl

#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinTreeCtrl : public CTreeCtrl
#else
class CxSkinTreeCtrl : public CTreeCtrl
#endif
{
	DECLARE_DYNAMIC(CxSkinTreeCtrl)

public:
	CxSkinTreeCtrl();
	virtual ~CxSkinTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	int m_nId;
	//스크롤 이미지 처리용
	int		m_nSBWidth;
	int		m_nSBHeight;

	int		m_nVThumbTWidth;
	int		m_nVThumbTHeight;
	int		m_nVThumbMWidth;
	int		m_nVThumbMHeight;
	int		m_nVThumbBWidth;
	int		m_nVThumbBHeight;
#if USE_TRANSPARENTBAR
	int		m_nVChanWidth;
	int		m_nVChanHeight;
#else
	int		m_nVChanTWidth;
	int		m_nVChanTHeight;
	int		m_nVChanMWidth;
	int		m_nVChanMHeight;
	int		m_nVChanBWidth;
	int		m_nVChanBHeight;
#endif

	int		m_nHThumbTWidth;
	int		m_nHThumbTHeight;
	int		m_nHThumbMWidth;
	int		m_nHThumbMHeight;
	int		m_nHThumbBWidth;
	int		m_nHThumbBHeight;
#if USE_TRANSPARENTBAR
	int		m_nHChanWidth;
	int		m_nHChanHeight;
#else
	
	int		m_nHChanTWidth;
	int		m_nHChanTHeight;
	int		m_nHChanMWidth;
	int		m_nHChanMHeight;
	int		m_nHChanBWidth;
	int		m_nHChanBHeight;
#endif

	CString		m_strFontName;
	int			m_nFontSize;

	int			m_nMinThumbSize;
	int			m_nItemHeight;

	COLORREF	m_clrText;
	COLORREF	m_clrFocusText;
	COLORREF	m_clrBk;
	COLORREF	m_clrFocusBk;
	
	int			m_sbMode;
	COLORREF	m_clrGripBk;
	COLORREF	m_clrGripOutline;

	CBitmap		m_bmVSC_TopBtn_N;
	CBitmap		m_bmVSC_TopBtn_O;
	CBitmap		m_bmVSC_TopBtn_D;
	CBitmap		m_bmVSC_BottomBtn_N;
	CBitmap		m_bmVSC_BottomBtn_O;
	CBitmap		m_bmVSC_BottomBtn_D;
	CBitmap		m_bmVSC_Thumb_T_N;
	CBitmap		m_bmVSC_Thumb_T_O;
	CBitmap		m_bmVSC_Thumb_T_D;
	CBitmap		m_bmVSC_Thumb_C_N;
	CBitmap		m_bmVSC_Thumb_C_O;
	CBitmap		m_bmVSC_Thumb_C_D;
	CBitmap		m_bmVSC_Thumb_B_N;
	CBitmap		m_bmVSC_Thumb_B_O;
	CBitmap		m_bmVSC_Thumb_B_D;
#if USE_TRANSPARENTBAR
	CBitmap		m_bmVSC_Channel;
#else
	CBitmap		m_bmVSC_ChannelTop;
	CBitmap		m_bmVSC_ChannelMid;
	CBitmap		m_bmVSC_ChannelBtm;
#endif

	CBitmap		m_bmHSC_LeftBtn_N;
	CBitmap		m_bmHSC_LeftBtn_O;
	CBitmap		m_bmHSC_LeftBtn_D;
	CBitmap		m_bmHSC_RigthBtn_N;
	CBitmap		m_bmHSC_RigthBtn_O;
	CBitmap		m_bmHSC_RigthBtn_D;
	CBitmap		m_bmHSC_Thumb_T_N;
	CBitmap		m_bmHSC_Thumb_T_O;
	CBitmap		m_bmHSC_Thumb_T_D;
	CBitmap		m_bmHSC_Thumb_C_N;
	CBitmap		m_bmHSC_Thumb_C_O;
	CBitmap		m_bmHSC_Thumb_C_D;
	CBitmap		m_bmHSC_Thumb_B_N;
	CBitmap		m_bmHSC_Thumb_B_O;
	CBitmap		m_bmHSC_Thumb_B_D;
#if USE_TRANSPARENTBAR
	CBitmap		m_bmHSC_Channel;
#else
	CBitmap		m_bmHSC_ChannelTop;
	CBitmap		m_bmHSC_ChannelMid;
	CBitmap		m_bmHSC_ChannelBtm;
#endif

	CRect		VSC_TopBtn_N;
	CRect		VSC_TopBtn_O;
	CRect		VSC_TopBtn_D;
	CRect		VSC_BottomBtn_N;
	CRect		VSC_BottomBtn_O;
	CRect		VSC_BottomBtn_D;
	CRect		VSC_Thumb_T_N;
	CRect		VSC_Thumb_T_O;
	CRect		VSC_Thumb_T_D;
	CRect		VSC_Thumb_C_N;
	CRect		VSC_Thumb_C_O;
	CRect		VSC_Thumb_C_D;
	CRect		VSC_Thumb_B_N;
	CRect		VSC_Thumb_B_O;
	CRect		VSC_Thumb_B_D;
#if USE_TRANSPARENTBAR
	CRect		VSC_Channel;
#else
	CRect		VSC_ChannelTop;
	CRect		VSC_ChannelMid;
	CRect		VSC_ChannelBtm;
#endif

	CRect		HSC_LeftBtn_N;
	CRect		HSC_LeftBtn_O;
	CRect		HSC_LeftBtn_D;
	CRect		HSC_RightBtn_N;
	CRect		HSC_RightBtn_O;
	CRect		HSC_RightBtn_D;
	CRect		HSC_Thumb_T_N;
	CRect		HSC_Thumb_T_O;
	CRect		HSC_Thumb_T_D;
	CRect		HSC_Thumb_C_N;
	CRect		HSC_Thumb_C_O;
	CRect		HSC_Thumb_C_D;
	CRect		HSC_Thumb_B_N;
	CRect		HSC_Thumb_B_O;
	CRect		HSC_Thumb_B_D;
#if USE_TRANSPARENTBAR
	CRect		HSC_Channel;
#else
	CRect		HSC_ChannelTop;
	CRect		HSC_ChannelMid;
	CRect		HSC_ChannelBtm;
#endif

	CString		m_strSkinFile;
	CBitmap		m_bmSkinImage;

	CRect		m_Rect;

	BOOL InitSkin(CDC *pDC, CWnd* pWnd);
	void FreeSkin(void);
		
public:
	BOOL SetSkin(CString strSkinFile);
	void SetFontName(CString strFontName){m_strFontName = strFontName;};
	CString GetFontName(){return m_strFontName;};
	void SetFontSize(int nFontSize){m_nFontSize = nFontSize;};
	void SetMinThumbSize(int nSize){m_nMinThumbSize = nSize;};
	void SetTreeRect(CRect rect){m_Rect=rect;};
	CRect GetTreeRect(void){return m_Rect;};
	void SetTxtColor(COLORREF clrText, COLORREF clrFocusText);
	void SetBackColor(COLORREF clrBk, COLORREF clrFocusBk);
#if USE_COOL_SB
	LRESULT	_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm);
#endif

#if USE_TRANSPARENTBAR
	void CopyListVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
		CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
		CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
		CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
		CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
		CRect rcVCH);
#else
	void CopyListVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
		CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
		CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
		CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
		CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
		CRect rcVTCH, CRect rcVMCH, CRect rcVBCH);
#endif

#if USE_TRANSPARENTBAR
		void CopyListHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
		CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
		CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
		CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
		CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
		CRect rcHCH);
#else
		void CopyListHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
		CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
		CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
		CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
		CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
		CRect rcHTCH, CRect rcHMCH, CRect rcHBCH);
#endif
	void SetScrollGripColor(COLORREF clrBk, COLORREF clrOutline);
	void SetScrollMode(int mode){m_sbMode = mode;};

	BOOL m_bTransparentThumb;
	void SetTransparentThumb(BOOL bTrans){m_bTransparentThumb = bTrans;};
	
	void SetId(int id){m_nId = id;};
	int GetId(){return m_nId;};
	void SetCaption(CString val){SetWindowText(val);};
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void  GradientFillRectScroll( CDC *pDC);
	afx_msg void OnDestroy();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

#endif