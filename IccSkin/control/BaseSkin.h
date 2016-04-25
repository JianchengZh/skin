#ifndef BASESKIN_INCLUDE
#define BASESKIN_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif
// BaseSkin.h : header file
//
///////////////////Control ID Range//////////////////////////////////

#define IDC_MIN				1000

#define IDC_BUTTON_DEF		1100	//버튼
#define IDC_STATIC_DEF		1200	//스태틱
#define IDC_SLIDER_DEF		1300	//슬라이더 
#define IDC_EDIT_DEF		1400	//에디트박스
#define IDC_COMBO_DEF		1500	//콤보박스
#define	IDC_TEXT_DEF		1600	//텍스트
#define IDC_LIST_DEF		1700	//리스트 
#define IDC_TREE_DEF		1800	//트리

#define IDC_MAX				2000	//

#include "Define.h"

#include <afxtempl.h>
#include "ImageMap_Mask.h"
#include "ImageMap_BG.h"

///////////////////Base Control////////////////////
#include "xSkinButton.h"
#include "xSkinStatic.h"
#include "xSkinSlider.h"
#include "RoundSliderCtrl.h"

#if USE_SKINLABEL
#include "xSkinLabel.h"
#else
#include "xSkinText.h"
#endif
//////////////Optional Control Header//////////////////
#if USE_SCINTILLAWND
#include "ScintillaWnd.h"	
#elif USE_SKINEDIT
#include "xSkinEdit.h"
#else
#include "InplaceEdit.h"
#endif

#if USE_SKINLISTCTRL
#include "xSkinListCtrl.h"	
#endif

#if USE_SKINTREECTRL
#include "xSkinTreeCtrl.h"	
#endif

#if USE_SKINCOMBOBOX
#include "xSkinComboBox.h"
#endif

enum BG_STYLE{
STYLE_RGB = 0,
STYLE_FIXED,
STYLE_IMAGE,
};

enum DIALOG_STYLE{
DLGSTYLE_NSTYLE = 0,
DLGSTYLE_RESIZE,
DLGSTYLE_SKIN,
};
//////////////////////////////////////////////////

// CBaseSkin dialog

typedef BOOL ( CALLBACK* LPFN_DrawMainBG)(CDC* pDC, void* pParam);

#ifdef _AFXEXT
class AFX_EXT_CLASS CBaseSkin
#else
class CBaseSkin
#endif
{

public:

	LPFN_DrawMainBG    _lpCallback;
	BOOL AttachCallback(LPFN_DrawMainBG _CallFunc);	//배경 그리기 콜백

	CString m_strSkinFileName;//스킨 ini 파일명

	bool m_bInit;			//스킨 ini 파일 존재?
	bool m_bDefSize;		//기본 사이즈 설정?
	bool m_bIsSetResize;	//다이얼로그 리사이즈 가능?

	CWnd* m_pWnd;			//Skin이 적용된 윈도우
	
	CBitmap m_bmSkinImage;	//스킨이미지 비트맵
	CBitmap m_bmBImage;		//스킨이미지 비트맵, m_bmSkinImage를 Attach

	COLORREF m_clrBk;	//RGB 스타일에서 사용하는 창 배경색상

	CString	m_strCaption;
	int		m_nBGStyle;		//창 배경 스타일
	int		m_nDLGStyle;	//창 스타일
	bool	m_bSetMask;		//마스크 사용?
	CRect	m_rDefRect;		//디폴트 창 사이즈

	bool	m_bDrawPattern;	//배경이미지를 타일방식으로 그릴것인가
	bool	m_bDrawPattern_Mask;	//마스크이미지를 타일방식으로 그릴것인가
	bool	m_bBlockExit;		//Enter, Esc키로 종료 방지

	bool	m_bUseShadow;	//Shadow Effect
	int		m_shadowPosX;
	int		m_shadowPosY;
	int		m_shadowSize;
	int		m_shadowSharp;
	int		m_shadowDark;
	COLORREF		m_shadowColor;

	CImageMap_Mask	m_Map_Mask;	//
	CImageMap_BG	m_Map_BG;	//
	CRect			m_Map_Fixed;

	HRGN hMaskRgn;

	///////////////////////컨트롤 배열들///////////////////////////
	CTypedPtrArray<CPtrArray, CxSkinButton*> m_ArrButtons;
	CTypedPtrArray<CPtrArray, CxSkinStatic*> m_ArrStatics;
	CTypedPtrArray<CPtrArray, CxSkinSlider*> m_ArrSlider;
	CTypedPtrArray<CPtrArray, CRoundSliderCtrl*> m_ArrRndSlider;

#if USE_SCINTILLAWND
	CTypedPtrArray<CPtrArray, CScintillaWnd*> m_ArrEdit;
#elif USE_SKINEDIT
	CTypedPtrArray<CPtrArray, CxSkinEdit*> m_ArrEdit;
#else
	CTypedPtrArray<CPtrArray, CInplaceEdit*> m_ArrEdit;
#endif

#if USE_SKINLABEL
	CTypedPtrArray<CPtrArray, CxSkinLabel*> m_ArrTexts;
#else
	CTypedPtrArray<CPtrArray, CxSkinText*> m_ArrTexts;
#endif
	//TiXmlDocument m_XmlDoc; 로컬 변수로 처리

//////////////////////////////////////////////////////////////////////////

#if USE_SKINLISTCTRL
	CTypedPtrArray<CPtrArray, CxSkinListCtrl*> m_ArrList;
#endif

#if USE_SKINTREECTRL
	CTypedPtrArray<CPtrArray, CxSkinTreeCtrl*> m_ArrTree;
#endif

#if USE_SKINCOMBOBOX
	CTypedPtrArray<CPtrArray, CxSkinComboBox*> m_ArrCombo;
#endif
	//////////////////////////////////////////////////////////////

// Construction
public:

	CBaseSkin();   // standard constructor
	~CBaseSkin();

private:
	void LoadShadowValue();
	/////////////스킨을 로딩, 컨트롤 생성///////////////////////
	void AttachRndSlider();
	void AttachSlider();
	void AttachStatic();
	void AttachButton();
	void AttachEdit();
	void AttachText(void);
#if USE_SKINLISTCTRL
	void AttachList(void);
#endif

#if USE_SKINTREECTRL
	void AttachTree(void);
#endif

#if USE_SKINCOMBOBOX
	void AttachComboBox(void);
#endif

	void Attach_Control();

	//////////////스킨을 다시로딩///////////////////////
	void _ReLoadSkinButton();
	void _ReLoadSkinStatic();
	void _ReLoadSkinSlider();
	void _ReLoadSkinEdit();
	void _ReLoadSkinText();
	
	CxSkinButton* FindButton(int id);
	CxSkinStatic* FindStatic(int id);	
	CxSkinSlider* FindSlider(int id);	
#if USE_SKINLABEL
	CxSkinLabel*	FindText(int id);
#else
	CxSkinText*		FindText(int id);
#endif
#if USE_SCINTILLAWND
	CScintillaWnd*		FindEdit(int id);
#elif USE_SKINEDIT
	CxSkinEdit*		FindEdit(int id);
#else
	CInplaceEdit*		FindEdit(int id);
#endif


#if USE_SKINLISTCTRL
	void _ReLoadSkinList(void);
	CxSkinListCtrl* FindList(int id);
#endif

#if USE_SKINTREECTRL
	void _ReLoadSkinTree(void);
	CxSkinTreeCtrl* FindTree(int id);
#endif

#if USE_SKINCOMBOBOX
	void _ReLoadSkinComboBox(void);
	CxSkinComboBox* FindComboBox(int id);
#endif

	
	/////////////////////////////////////////////////

	
	
public:
	bool SetRGB(CDC* pDC);
	bool SetMapRGB();
	bool SetMapBG();
	bool SetBG(CDC* pDC);
	bool SetMapMask();
	void SetMask();
	void SetFixed(CDC *pDC);
	bool SetMapFixed();
	void SkinRedraw(void);	//컨트롤 다시 그림
	CBitmap* GetSkinImage();//스킨이미지 반환
	bool InitSkin(CString strFileName);
	void FreeSkin();
	BOOL LoadSkins(CDC *pDC, CWnd* pWnd);
	BOOL LoadSkins(CDC *pDC, CWnd* pWnd, CBitmap *bmBaseImage);
	bool _ReLoadSkins(CDC *pDC);
};

#endif 
