#ifndef XSKINEDIT_INCLUDED
#define XSKINEDIT_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif
// SkinEdit.h : header file
//
/////////////////////////////////////////////////////////////////////////////
/*
[Description]
     This  class provide a appearance the same as the "Master of Windows optimization".That is
when the mouse move over the edit control,the background turns to White!When the 
mouse move out of the edit control,the background turns to the origion color!
[History]
     2002/3/3 Coded by David!
[Flaws]
     Something about the scroll bar isn't good!
	 Feel free to use it!
*/////////////////////////////////////////////////////////////////////////////

#include "Define.h"

#if USE_COOL_SB
#include "Scrollbar.h"
#endif

// CSkinEdit window
#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinEdit : public CEdit
#else
class CxSkinEdit : public CEdit
#endif
{
// Construction
public:
	CxSkinEdit(); 
	
// Attributes
private:
	CFont		m_Font;
	BOOL		m_bReadOnly;

	CString		m_strFontName;
	int			m_nFontSize;
	BOOL		m_bFontBold;

	BOOL		m_bIsFocused;
	BOOL		m_bMouseOver;
	BOOL		m_bBorder;

	CBrush		m_brLine;
	CBrush		m_brBG;
	CBrush		m_brFocusLine;
	CBrush		m_brFocusBG;
	
	COLORREF	m_clrOutline;
	COLORREF	m_clrOutlineFocus;
	COLORREF	m_clrBk;	
	COLORREF	m_clrFocusBk;

	COLORREF	m_clrFocusText;
	COLORREF	m_clrText;

	//Check Text Message
	static const UINT UWM_CHECKTEXT;
	//Keep the current values
	BOOL m_bShowHoriz;
	BOOL m_bShowVert;

	int		m_nType;
	DWORD	m_dwStyle;
	BOOL	m_bShowScroll;

	UINT	m_nId;
	BOOL	m_bNumeric;
	int		m_nMaxChar;
	int		m_nMaxDigit;
	int		m_nMinDigit;
	BOOL	m_bValidDigit;
public:
	
	////////////////////////////스크롤바///////////////////////////
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


	int		m_nMinThumbSize;

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
	/////////////////////////////////////////////////////////////////////

	CString m_strDelim;// 소수점

// Operations
public:
	//Overloaded Function
	void SetWindowText(LPCTSTR lpszString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinEdit)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CxSkinEdit();

	// Generated message map functions
protected:
	
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	DECLARE_MESSAGE_MAP()

	//Scroll
	void ShowHorizScrollBar(BOOL bShow);
	void ShowVertScrollBar(BOOL bShow);
	void CheckScrolling(LPCTSTR lpszString);

	afx_msg LRESULT OnCheckText(WPARAM wParam, LPARAM lParam);

public:
#if USE_TRANSPARENTBAR
	void CopyVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
		CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
		CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
		CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
		CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
		CRect rcVCH);
#else
	void CopyVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
		CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
		CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
		CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
		CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
		CRect rcVTCH, CRect rcVMCH, CRect rcVBCH);
#endif

#if USE_TRANSPARENTBAR
	void CopyHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
		CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
		CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
		CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
		CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
		CRect rcHCH);
#else
	void CopyHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
		CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
		CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
		CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
		CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
		CRect rcHTCH, CRect rcHMCH, CRect rcHBCH);
#endif

	void FreeSkin(void);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	void SetTextColor(COLORREF clrNormalText, COLORREF clrFocusText);
	void SetBGColor(COLORREF clrNormalBG, COLORREF clrFocusBG);
	void SetLineColor(COLORREF clrNomalLine, COLORREF clrFocusLine);

	void SetScrollGripColor(COLORREF clrBk, COLORREF clrOutline);

	void SetFontName(CString strFontName){m_strFontName = strFontName;};
	void SetFontSize(int nFontSize){m_nFontSize = nFontSize;};
	void SetFontBold(BOOL bFontBold){m_bFontBold = bFontBold;};
	void SetBorder(BOOL bBorder){m_bBorder = bBorder;};
	void SetShowScrollbar(BOOL bShow);
	void SetReadOnlyState(BOOL bReadonly){m_bReadOnly = bReadonly;};
	void Init(void);
	void SetCaption(CString val){};
	void SetId(int id){m_nId = id;};
	int GetId(void){return m_nId;};
	CString GetIDName(void);
	void SetNumeric(BOOL bNumeric){m_bNumeric = bNumeric;};
	void SetMaxCharacters(int nMaxChar){m_nMaxChar = nMaxChar;};
	void SetMaxDigits(int nMaxDigit){m_nMaxDigit = nMaxDigit;};
	void SetMinDigits(int nMinDigit){m_nMinDigit = nMinDigit;};
	void SetShowHorizScrollBar(BOOL bShow){m_bShowHoriz = bShow;};
	void SetShowVertScrollBar(BOOL bShow){m_bShowVert = bShow;};

	void AppendText(CString strText);
	
	BOOL m_bTransparentThumb;
	void SetTransparentThumb(BOOL bTrans){m_bTransparentThumb = bTrans;};
	
public:
#if USE_COOL_SB
	LRESULT	_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm);
#endif
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


/////////////////////////////////////////////////////////////////////////////

inline void CxSkinEdit::ShowHorizScrollBar(BOOL bShow)
{
	if(m_bShowHoriz != bShow)
	{
		ShowScrollBar(SB_HORZ, bShow);
		m_bShowHoriz = bShow;
	}
}

inline void CxSkinEdit::ShowVertScrollBar(BOOL bShow)
{
	if(m_bShowVert != bShow)
	{
		ShowScrollBar(SB_VERT, bShow);
		m_bShowVert = bShow;
	}
}

#endif 
