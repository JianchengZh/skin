#ifndef DEFINE_INCLUDED
#define DEFINE_INCLUDED

#define USE_COOL_SB			1	//스크롤바
///////////////Optional Control////////////////////
#define USE_SKINLISTCTRL	1
#define USE_SKINTREECTRL	1
#define USE_SKINCOMBOBOX	1
#define USE_SKINLABEL		1
#define USE_SCINTILLAWND	0

//EDIT는 
#if USE_SCINTILLAWND
#define USE_SKINEDIT		0
#else
#define USE_SKINEDIT		1	//InplaceEdit는 이것으로 조절
#endif

///////////////////////////////////////////////////

#define	FONT_NAME	_T("MS Sans Serif")
#define	FONT_SIZE	12
#define COLOR_WHITE		RGB(255,255,255)
#define COLOR_BLACK		RGB(0,0,0)
#define COLOR_MAGENTA	RGB(255,0,255)
#define COLOR_BLUE		RGB(0,0,255)
#define COLOR_RED		RGB(255,0,0)
#define COLOR_SB_GRIP	RGB(240,240,240)
#define COLOR_BK_FOCUS	RGB(51,153,255)
#define COLOR_DISABLE	RGB(127,127,127)

#define DEF_RECT		CRect(0,0,300,300)

#define DEF_MAIN		"Main"
#define DEF_MAIN_BK		"MainBk"
#define DEF_MAIN_MASK	"MainMask"
#define DEF_FIXED		"fixed"
#define DEF_RGB			"rgb"

#define	DEF_BUTTON		"Button"
#define	DEF_STATIC		"Static"
#define	DEF_TEXT		"Text"
#define	DEF_SLIDER		"Slider"
#define	DEF_RSLIDER		"RoundSlider"
#define	DEF_EDIT		"Edit"
#define	DEF_COMBOBOX	"ComboBox"
#define	DEF_LIST		"List"
#define	DEF_TREE		"Tree"
#define DEF_SHADOW		"Shadow"

#define DEF_CAPTION		"Caption"
#define DEF_ID			"Id"
#define DEF_FONTNAME	"FontName"
#define DEF_FONTSIZE	"FontSize"
#define DEF_BOLD		"FontBold"
#define DEF_POS			"Position"

#define DEF_SB_GRIP_COLOR			"ScrollGripColor"
#define DEF_SB_GRIP_LINE			"ScrollGripLineColor"
#define DEF_SB_VERT_TBTN_NRECT		"ScrollVerticalTopBtnNormalRect"
#define DEF_SB_VERT_TBTN_ORECT		"ScrollVerticalTopBtnOverRect"
#define DEF_SB_VERT_TBTN_DRECT		"ScrollVerticalTopBtnDownRect"
#define DEF_SB_VERT_BBTN_NRECT		"ScrollVerticalBottomBtnNormalRect"
#define DEF_SB_VERT_BBTN_ORECT		"ScrollVerticalBottomBtnOverRect"
#define DEF_SB_VERT_BBTN_DRECT		"ScrollVerticalBottomBtnDownRect"
#define DEF_SB_VERT_THUMBT_NRECT	"ScrollVerticalThumbTopNormalRect"
#define DEF_SB_VERT_THUMBT_ORECT	"ScrollVerticalThumbTopOverRect"
#define DEF_SB_VERT_THUMBT_DRECT	"ScrollVerticalThumbTopDownRect"
#define DEF_SB_VERT_THUMBM_NRECT	"ScrollVerticalThumbMiddleNormalRect"
#define DEF_SB_VERT_THUMBM_ORECT	"ScrollVerticalThumbMiddleOverRect"
#define DEF_SB_VERT_THUMBM_DRECT	"ScrollVerticalThumbMiddleDownRect"
#define DEF_SB_VERT_THUMBB_NRECT	"ScrollVerticalThumbBottomNormalRect"
#define DEF_SB_VERT_THUMBB_ORECT	"ScrollVerticalThumbBottomOverRect"
#define DEF_SB_VERT_THUMBB_DRECT	"ScrollVerticalThumbBottomDownRect"
#define DEF_SB_VERT_CHAN_RECT		"ScrollVerticalChannelRect"
#define DEF_SB_VERT_CHAN_TRECT		"ScrollVerticalChannelTopRect"
#define DEF_SB_VERT_CHAN_MRECT		"ScrollVerticalChannelMiddleRect"
#define DEF_SB_VERT_CHAN_BRECT		"ScrollVerticalChannelBottomRect"
#define DEF_SB_HORZ_LBTN_NRECT		"ScrollHorizontalLeftBtnNormalRect"
#define DEF_SB_HORZ_LBTN_ORECT		"ScrollHorizontalLeftBtnOverRect"
#define DEF_SB_HORZ_LBTN_DRECT		"ScrollHorizontalLeftBtnDownRect"
#define DEF_SB_HORZ_RBTN_NRECT		"ScrollHorizontalRightBtnNormalRect"
#define DEF_SB_HORZ_RMTN_ORECT		"ScrollHorizontalRightBtnOverRect"
#define DEF_SB_HORZ_RBTN_DRECT		"ScrollHorizontalRightBtnDownRect"
#define DEF_SB_HORZ_THUMBT_NRECT	"ScrollHorizontalThumbTopNormalRect"
#define DEF_SB_HORZ_THUMBT_ORECT	"ScrollHorizontalThumbTopOverRect"
#define DEF_SB_HORZ_THUMBT_DRECT	"ScrollHorizontalThumbTopDownRect"
#define DEF_SB_HORZ_THUMBM_NRECT	"ScrollHorizontalThumbMiddleNormalRect"
#define DEF_SB_HORZ_THUMBM_ORECT	"ScrollHorizontalThumbMiddleOverRect"
#define DEF_SB_HORZ_THUMBM_DRECT	"ScrollHorizontalThumbMiddleDownRect"
#define DEF_SB_HORZ_THUMBB_NRECT	"ScrollHorizontalThumbBottomNormalRect"
#define DEF_SB_HORZ_THUMBB_ORECT	"ScrollHorizontalThumbBottomOverRect"
#define DEF_SB_HORZ_THUMBB_DRECT	"ScrollHorizontalThumbBottomDownRect"
#define DEF_SB_HORZ_CHAN_RECT		"ScrollHorizontalChannelRect"
#define DEF_SB_HORZ_CHAN_TRECT		"ScrollHorizontalChannelTopRect"
#define DEF_SB_HORZ_CHAN_MRECT		"ScrollHorizontalChannelMiddleRect"
#define DEF_SB_HORZ_CHAN_BRECT		"ScrollHorizontalChannelBottomRect"		
#define DEF_SB_HORZ_THUMB_MIN_SIZE	"ScrollThumbMinimumSize"

#endif
