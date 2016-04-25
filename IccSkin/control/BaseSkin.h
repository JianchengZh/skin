#ifndef BASESKIN_INCLUDE
#define BASESKIN_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif
// BaseSkin.h : header file
//
///////////////////Control ID Range//////////////////////////////////

#define IDC_MIN				1000

#define IDC_BUTTON_DEF		1100	//��ư
#define IDC_STATIC_DEF		1200	//����ƽ
#define IDC_SLIDER_DEF		1300	//�����̴� 
#define IDC_EDIT_DEF		1400	//����Ʈ�ڽ�
#define IDC_COMBO_DEF		1500	//�޺��ڽ�
#define	IDC_TEXT_DEF		1600	//�ؽ�Ʈ
#define IDC_LIST_DEF		1700	//����Ʈ 
#define IDC_TREE_DEF		1800	//Ʈ��

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
	BOOL AttachCallback(LPFN_DrawMainBG _CallFunc);	//��� �׸��� �ݹ�

	CString m_strSkinFileName;//��Ų ini ���ϸ�

	bool m_bInit;			//��Ų ini ���� ����?
	bool m_bDefSize;		//�⺻ ������ ����?
	bool m_bIsSetResize;	//���̾�α� �������� ����?

	CWnd* m_pWnd;			//Skin�� ����� ������
	
	CBitmap m_bmSkinImage;	//��Ų�̹��� ��Ʈ��
	CBitmap m_bmBImage;		//��Ų�̹��� ��Ʈ��, m_bmSkinImage�� Attach

	COLORREF m_clrBk;	//RGB ��Ÿ�Ͽ��� ����ϴ� â ������

	CString	m_strCaption;
	int		m_nBGStyle;		//â ��� ��Ÿ��
	int		m_nDLGStyle;	//â ��Ÿ��
	bool	m_bSetMask;		//����ũ ���?
	CRect	m_rDefRect;		//����Ʈ â ������

	bool	m_bDrawPattern;	//����̹����� Ÿ�Ϲ������ �׸����ΰ�
	bool	m_bDrawPattern_Mask;	//����ũ�̹����� Ÿ�Ϲ������ �׸����ΰ�
	bool	m_bBlockExit;		//Enter, EscŰ�� ���� ����

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

	///////////////////////��Ʈ�� �迭��///////////////////////////
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
	//TiXmlDocument m_XmlDoc; ���� ������ ó��

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
	/////////////��Ų�� �ε�, ��Ʈ�� ����///////////////////////
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

	//////////////��Ų�� �ٽ÷ε�///////////////////////
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
	void SkinRedraw(void);	//��Ʈ�� �ٽ� �׸�
	CBitmap* GetSkinImage();//��Ų�̹��� ��ȯ
	bool InitSkin(CString strFileName);
	void FreeSkin();
	BOOL LoadSkins(CDC *pDC, CWnd* pWnd);
	BOOL LoadSkins(CDC *pDC, CWnd* pWnd, CBitmap *bmBaseImage);
	bool _ReLoadSkins(CDC *pDC);
};

#endif 
