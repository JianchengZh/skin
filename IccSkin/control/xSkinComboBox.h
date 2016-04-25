#ifndef XSKINCOMBOBOX_INCLUDED
#define XSKINCOMBOBOX_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "xSkinListBox.h"

/////////////////////////////////////////////////////////////////////////////
//-------------Combo Box image scheme-----------------------------
//		-------------------------------------------------------
//	   |(m_bmComboBoxLeft)	-----(m_bmComboBoxCenter)---     |v|(m_bmComboBoxRight: contain drop down arrow)
//		-------------------------------------------------------
//-----------------------------------------------------------------

//------------List Box image scheme--------------------------------

//		ALL IMAGES ARE STRETCHED TO FIT THE DIMENSIONS

//		-------------m_bmComboListTop---------------------
//	   |												  |
//	   |												  |
//	   |												  |
//	m_bmComboListLeft							   m_bmComboListRight
//	   |												  |
//	   |												  |
//	   |												  |
//		-------------m_bmComboListBottom------------------
//------------------------------------------------------------------


#define DRAWMODE_RGB	0
#define DRAWMODE_IMAGE	1


// CxSkinComboBox

class CxSkinComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CxSkinComboBox)

public:
	CxSkinComboBox();
	virtual ~CxSkinComboBox();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);

public:
	CxSkinListBox	m_skinListBox;

private:
	//combobox image (left, center, right[include dropdown button])
	CBitmap		m_bmComboBoxLeft;
	CBitmap		m_bmComboBoxCenter;
	CBitmap		m_bmComboBoxRight;

	//listbox line image 
	CBitmap		m_bmComboListLeft;
	CBitmap		m_bmComboListTop;
	CBitmap		m_bmComboListBottom;
	CBitmap		m_bmComboListRight;

	//Text color
	COLORREF	m_clrNormalBG;
	COLORREF	m_clrNormalText;
	COLORREF	m_clrFocusBG;
	COLORREF	m_clrFocusText;

	CFont		m_Font;

	int	m_nEditHeight;//Edit 높이
	int	m_nRowHeight;//Item 높이 추가
	int m_nMarginLeft;
	int m_nMarginTop;
	int m_nMarginBottom;
	int m_nMarginRight;
	CString m_strFontName;
	int m_nFontSize;	
	int m_nId;
	int m_nDrawMode;
	BOOL m_bCustomList;
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//배경 및 텍스트 색상설정
	void SetNormalColor(COLORREF clrBG, COLORREF clrText);
	void SetFocusColor(COLORREF clrBG, COLORREF clrText);
	//리스트박스 라인 그리기
	void DrawComboListLeft(int nHeight = 0);
	void DrawComboListTop(int nWidth = 0);
	void DrawComboListBottom(int nWidth = 0);
	void DrawComboListRight(int nHeight = 0);

	void SetMargin(int left, int top, int bottom, int right);
	void SetEditHeight(int nHeight){m_nEditHeight = nHeight;};
	void SetRowHeight(int nHeight){m_nRowHeight = nHeight;};
	
	void SetDrawMode(int nMode/* FillRect */, BOOL bCustomList /* custom draw listbox */);
	//비트맵 복사
	void CopyFrom(CBitmap& bmSkinImage, CRect Rect_BoxL, CRect Rect_BoxC, CRect Rect_BoxRNM, 
		CRect Rect_BoxROV, CRect Rect_BoxRDN, CRect Rect_BoxRDS, CRect Rect_ListL, 
		CRect Rect_ListT, CRect Rect_ListB, CRect Rect_ListR);
	//비트맵 해제
	void FreeSkin(void);
	void SetFontName(CString strFontName){m_strFontName = strFontName;};
	void SetFontSize(int nFontSize){m_nFontSize = nFontSize;};
	void SetCaption(CString val){};
	void SetId(int id){m_nId = id;};
	int GetId(){return m_nId;};
	void Init(void);
	void SetValue(CString val);
	void SetDrawMode(int mode){m_nDrawMode=mode;};
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};

#endif