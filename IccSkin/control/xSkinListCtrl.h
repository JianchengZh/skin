#ifndef XSKINLISTCTRL_H
#define XSKINLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "Define.h"

#if USE_COOL_SB
#include "Scrollbar.h"
#endif

#include "xSkinHeaderCtrl.h"	//skin headerctrl

#define MINCOLWIDTH	10

#define DELETES(ptr) if ((ptr)) { delete[] (ptr); (ptr) = NULL; }

// CxSkinListCtrl

#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinListCtrl : public CListCtrl
#else
class CxSkinListCtrl : public CListCtrl
#endif
{
	DECLARE_DYNAMIC(CxSkinListCtrl)

	//헤더 컬럼정보
	typedef struct _COLUMNINFO
	{
		int		nColHdr;		// Column string Resource ID
		int		nColFmt;		// Column format
		int		nColWidth;		// Column width
		int		nColOrder;		// Column order
		int		nHeadType;		// Column type
		int		nFlag;			// Column flag (checked?)
		BOOL	bSorted;		// Column sorted
		CString	strText;		// Column String

	}COLUMNINFO;

public:
	CxSkinListCtrl();
	virtual ~CxSkinListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	CArray<COLUMNINFO*, COLUMNINFO*> m_ArrColInfo;	//전체 컬럼 배열
	CArray<COLUMNINFO*, COLUMNINFO*> m_ArrNowCol;	//현재 컬럼 배열(추가/삭제/변경/)

	BOOL	m_bUseBkImage;		//use background image
	BOOL	m_bUseCheckbox;		//use checkbox?
	BOOL	m_bUseSort;			//use sort
	BOOL	m_bDragdrop;		//use dragdrop
	CFont	m_ListFont;			//list font
	CFont	m_HeaderFont;		//header font

	BOOL	m_bAllCheck;		//all checked?

	CxSkinHeaderCtrl	m_ListHeader;	//header control

	COLORREF	m_clrBk;		//bk color
	COLORREF	m_clrText;		//text color
	COLORREF	m_clrOddbk;		//oll bk color
	COLORREF	m_clrEvenbk;	//even bk color
	COLORREF	m_clrFocusBk;	//focus bk color
	COLORREF	m_clrFocusText;	//focus text color
	COLORREF	m_clrSelectBk;	//select bk color
	COLORREF	m_clrSelectText;//select text color

	BOOL	m_bAscend;			//asc sort?
	int		m_nSortCol;			//sort column

	int		m_nMinColumnSize;	//minimum column size

	//drag & drop
	BOOL	m_bDrag;			//drag item?
	CWnd*	m_pDragWnd;			//source wnd
	CWnd*	m_pDropWnd;			//target wnd
	int		m_nDragIndex;		//drag item index
	int		m_nDropIndex;		//drop item index

	CImageList* m_pDragImage;	//drag image
	CImageList	m_ImageList;	//image list
	CRect	m_rcDrag;
	CDC*	m_pDC;


	//스크롤 이미지 처리용
	int		m_sbMode;
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

	CBitmap		m_bmOddCheck;
	CBitmap		m_bmOddUnCheck;
	CBitmap		m_bmEvenCheck;
	CBitmap		m_bmEvenUnCheck;
	CBitmap		m_bmFocusCheck;
	CBitmap		m_bmFocusUnCheck;
	CString		m_strSkinFile;
	CBitmap		m_bmSkinImage;
	BOOL		m_bShowVScroll;
	BOOL		m_bShowHScroll;
	CString		m_strListFont;
	CString		m_strHeadFont;
	int			m_nListHeight;
	int			m_nHeadHeight;
	CRect		m_Rect;
	int			m_nListFontSize;
	int			m_nHeadFontSize;
	BOOL		m_bHeaderDrag;
	int			m_nHeadCheckIdx;
	BOOL		m_bHeadFontBold;
	BOOL		m_bUseHeaderCtrl;
	CBitmap		m_bmListBkImage;
	int			m_nHeadCheckMargin;
	BOOL m_bTransparentThumb;
	int m_nId;

	BOOL InitSkin(CDC *pDC, CWnd* pWnd);
	void FreeSkin(void);
	CImageList*	CreateDragImageEx(LPPOINT lpPoint);	//create drag image

public:
	BOOL SetSkin(CString strSkinFile);
	void SetFontName(CString strListFont, CString strHeadFont);
	CString GetFontName(void){return m_strListFont;};
	void SetFontSize(int nListSize, int nHeadSize);
	void SetListRect(CRect rect){m_Rect = rect;};
	CRect GetListRect(void){return m_Rect;};
	void ShowScroll(BOOL bShowVScroll, BOOL bShowHScroll);
	void AddColumnInfo(COLUMNINFO* pColInfo);
	void DeleteAllColumnInfo(void);
	void SetColumnInfo(void);
	void SetUseBkImage(BOOL bUseBkImage = FALSE){m_bUseBkImage = bUseBkImage;};
	void SetUseCheck(BOOL bUseCheckbox){m_bUseCheckbox = bUseCheckbox;};
	void SetCheckboxIdx(int nIndex){m_nHeadCheckIdx = nIndex;};
	void SetCheckboxMargin(int nMargin){m_nHeadCheckMargin = nMargin;};
	void SetUseSort(BOOL buseSort){m_bUseSort = buseSort;};
	void SetUseDragDrop(BOOL bDragdrop){m_bDragdrop = bDragdrop;};
	void SetUseHeaderDrag(BOOL bHeaderDrag){m_bHeaderDrag = bHeaderDrag;};
	void SetUseHeaderCtrl(BOOL bUse){m_bUseHeaderCtrl = bUse;};
	void SortItem(int nColumn);
	void SortItem(int nColumn, BOOL bAscend);
	void SetNormalColor(COLORREF clrText, COLORREF clrOdd, COLORREF clrEven);
	void SetFocusColor(COLORREF clrText, COLORREF clrBk);
	void SetSelectColor(COLORREF clrText, COLORREF clrBk);
	void SetBkColor(COLORREF clrBk){m_clrBk = clrBk;};
	
	void SetHeadTxtColor(COLORREF clrHeadTxt);
	void SetHeadBackColor(COLORREF clrHeadBk);
	void SetHeadFontBold(BOOL bBold){m_bHeadFontBold = bBold;};

	void SetMinColumnSize(int nColSize){m_nMinColumnSize = nColSize;};
	void SetMinThumbSize(int nThumbSize){m_nMinThumbSize = nThumbSize;};
	void DeleteAllItem(void);
	void SetScrollGripColor(COLORREF clrBk, COLORREF clrOutline);
	void SetScrollMode(int mode){m_sbMode=mode;};
	void CopyListCheckFromBG(CBitmap& bmSkinImage, CRect rcOddChecked, CRect rcOddUnChecked, 
		CRect rcEvenChecked, CRect rcEvenUnChecked, CRect rcFocusChecked, CRect rcFocusUnChecked);
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
	void CopyListHeaderFromBG(CBitmap& bmSkinImage, CRect rcCheck, CRect rcUnCheck, 
		CRect rcLeft, CRect rcCN, CRect rcCO, CRect rcCD, CRect rcCS, CRect rcD, CRect rcDD, CRect rcRight,
		CRect rcAsc, CRect rcDesc);
#if USE_COOL_SB
	LRESULT	_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm);
#endif
	void SetRowHeight(int nHeight){m_nListHeight = nHeight;};
	void SetHeaderHeight(int nHeight){m_nHeadHeight = nHeight;};
	void SetTransparentThumb(BOOL bTrans){m_bTransparentThumb = bTrans;};
	
	void SetCaption(CString val){SetWindowText(val);};
	void SetId(int id){m_nId = id;};
	int GetId(){return m_nId;};
	void AutoSizeColumns(int col = -1);
	int GetColumnCount() const;
	void InitHeader();
	void SetHeader();
	void SetColumnList(CString strList);

	afx_msg void OnHdnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEnddrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnDestroy();
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif