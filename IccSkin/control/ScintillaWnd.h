/* @doc
 * @module ScintillaWnd.h | Definition of a Scintilla syntax coloring edit control for MFC<nl>
 * This program is an example how to use the excellent scintilla edit control of Neil Hodgson.<nl>
 * See www.scintilla.org for details<nl>
 * Author: Horst Br?kner, hb@ec-logic.com<nl>
 * Environment: VisualC++ Version 6, static Build of Scintilla, SciLexer.dll as Lexer<nl>
 */

#ifndef __SCINTILLAWND_H__
#define __SCINTILLAWND_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "..\..\scintilla\include\scintilla.h"
#include "..\..\scintilla\include\scilexer.h"

#include "Define.h"

#if USE_COOL_SB
#include "Scrollbar.h"
#endif

// Codepage
const int CP_CHINESE_TRADITIONAL = 950;
const int CP_CHINESE_SIMPLIFIED = 936;
const int CP_JAPANESE = 932;
const int CP_KOREAN = 949;
const int CP_GREEK = 1253;


////////////////////////////////////
// @class CScintillaWnd | Class of a GCL Scintilla syntax coloring edit control for MFC
// @base public | CWnd
//
#ifdef _AFXEXT
class AFX_EXT_CLASS CScintillaWnd : public CWnd  
#else
class CScintillaWnd : public CWnd  
#endif
{
public:
// @access public constructor - destructor
// @cmember empty Constructor
	CScintillaWnd();
// @cmember destructor
	virtual ~CScintillaWnd();

public:
// @access public macro members
// @cmember return linenumber display flag
   BOOL GetDisplayLinenumbers (){return m_bLinenumbers;};
// @cmember return selection display flag
   BOOL GetDisplaySelection (){return m_bSelection;};
// @cmember return folding margin display flag
   BOOL GetDisplayFolding (){return m_bFolding;};
// @cmember set search flags
   virtual void SetSearchflags (int nSearchflags){m_nSearchflags = nSearchflags;};
   

public:
// @access public function members
// @cmember register a window class to use scintilla in a dialog
   static BOOL Register(CWinApp *app, UINT id);
// @cmember try to load Lexer DLL
   static HMODULE LoadScintillaDll (LPCSTR szLexerDll = NULL);
// @cmember create window
   virtual BOOL Create (LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
// @cmember Set Text to the Scintilla control
   virtual void SetText (LPCSTR szText);
// @cmember Get Text from the Scintilla control
   virtual void GetText (CString &strText);
// @cmember Get Text from the Scintilla control
   //virtual LPSTR GetText();
   virtual LPTSTR GetText();
// @cmember Load a file
   virtual BOOL LoadFile (LPCTSTR szFile);
// @cmember Save a file
   virtual BOOL SaveFile (LPCTSTR szFile);
// @cmember try to find lexer format from extension
   virtual int GetFormatFromExtension (LPCTSTR szExtension);
// @cmember calcluate number of chars needed for linenumberdisplay
   virtual int GetLinenumberChars ();
// @cmember calcluate number of pixels for linenumber display
   virtual int GetLinenumberWidth ();
// @cmember set display of linenumbers on/off
   virtual void SetDisplayLinenumbers(BOOL bFlag = TRUE);
// @cmember set display of selection/bookmark margin on/off
   virtual void SetDisplaySelection(BOOL bFlag = TRUE);
// @cmember set display of source folding margin on/off
   virtual void SetDisplayFolding(BOOL bFlag = TRUE);
// @cmember cut selection to clipboard
   virtual void Cut();
// @cmember copy selection to clipboard
   virtual void Copy();
// @cmember paste from clipboard
   virtual void Paste();
// @cmember clear selection
   virtual void Clear();
// @cmember undo last change
   virtual void Undo();
// @cmember redo last change
   virtual void Redo();
// @cmember check if we can undo
   virtual BOOL CanUndo();
// @cmember check if we can redo
   virtual BOOL CanRedo();
// @cmember check if we have something to paste from clipboard
   virtual BOOL CanPaste();
// @cmember select complete text
   virtual void SelectAll();
// @cmember return the current line number
   virtual long GetCurrentLine();
// @cmember return the current column number
   virtual long GetCurrentColumn();
// @cmember return the current character position within the file
   virtual long GetCurrentPosition();
// @cmember return the current style number at the current character position
   virtual int GetCurrentStyle();
// @cmember return the current folding level at the current character position
   virtual int GetCurrentFoldinglevel();
// @cmember set the fontname for a style number
   virtual void SetFontname(int nStyle, LPCSTR szFontname);
// @cmember set the fontname height in points for a style number
   virtual void SetFontheight(int nStyle, int nHeight);
// @cmember set the foregroundcolor for a style number
   virtual void SetForeground(int nStyle, COLORREF crForeground);
// @cmember set the backgroundcolor for a style number
   virtual void SetBackground(int nStyle, COLORREF crBackground);
// @cmember set the selected foregroundcolor for a style number
   virtual void SetSelForeground(int nStyle, COLORREF crForeground);
// @cmember set the selected backgroundcolor for a style number
   virtual void SetSelBackground(int nStyle, COLORREF crBackground);
// @cmember set given style to bold
   virtual void SetBold(int nStyle, BOOL bFlag);
// @cmember set given style to bold
   virtual void SetItalic(int nStyle, BOOL bFlag);
// @cmember set given style to bold
   virtual void SetUnderline(int nStyle, BOOL bFlag);
// @cmember get flag if we are in overstrike mode
   virtual BOOL GetOverstrike();
// @cmember set flag for overstrike mode
   virtual void SetOverstrike(BOOL bOverstrike);
// @cmember init the view with reasonable defaults
   virtual void Init();
// @cmember called whenever the text is changed - we can update any indicators and do brace matching
   virtual void UpdateUI();
// @cmember do default folding 
   virtual void DoDefaultFolding(int nMargin, long lPosition);
// @cmember refresh lexer and display after new settings
   virtual void Refresh();
// @cmember add a bookmark = marker type 0
   virtual void AddBookmark(long lLine);
// @cmember delete a bookmark = marker type 0
   virtual void DeleteBookmark(long lLine);
// @cmember check for bookmark = marker type 0
   virtual BOOL HasBookmark(long lLine);
// @cmember Find next bookmark
   virtual void FindNextBookmark();
// @cmember Find previous bookmark
   virtual void FindPreviousBookmark();
// @cmember goto line
   virtual void GotoLine(long lLine);
// @cmember goto position
   virtual void GotoPosition(long lPos);
// @cmember search forward for a given text
   virtual BOOL SearchForward(LPSTR szText);
// @cmember search backward for a given text
   virtual BOOL SearchBackward(LPSTR szText);
// @cmember replace a text found by SearchForward or SearchBackward
   virtual void ReplaceSearchedText(LPCSTR szText);
// @cmember Set your own lexer
   virtual void SetLexer(int nLexer);
// @cmember return start position of selection
   virtual long GetSelectionStart();
// @cmember return end position of selection
   virtual long GetSelectionEnd();
// @cmember get selected text
   virtual CString GetSelectedText();
// @cmember replace all in buffer or selection
   virtual int ReplaceAll(LPCSTR szFind, LPCSTR szReplace, BOOL bSelection = TRUE);
// @cmember set multiline flags
   virtual void SetMultiline(BOOL bMultiline);
// @cmember set scrollbar flags
   virtual void SetScrollbar(BOOL bShowHScrollbar, BOOL bShowVScrollbar);
// @cmember set codepage
   virtual void SetCodepage(int nCodepage);
// @cmember set readonly flags
   virtual void SetReadOnly(BOOL bReadonly);

protected:
// @access protected data members
// @cmember flag if we should display line numbers
   BOOL m_bLinenumbers;
// @cmember flag if we should display selection and bookmark margin
   BOOL m_bSelection;
// @cmember flag if we should display folding margin
   BOOL m_bFolding;
// @cmember search flags
   int  m_nSearchflags;
// @cmember multiline flags
   BOOL m_bMultiline;
// @cmember readonly flags
   BOOL m_bReadonly;

   //스크롤바
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
   int m_nId;
   CRect	m_Rect;
   CString	m_strSkinFile;
   CBitmap	m_bmSkinImage;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

public:
	BOOL SetSkin(LPCTSTR lpszIniFile);
	void FreeSkin(void);

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

#if USE_COOL_SB
	LRESULT	_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm);
#endif
	
	bool m_bTransparentThumb;
	void SetTransparentThumb(bool bTrans){m_bTransparentThumb = bTrans;};
	void SetCaption(CString val){};
	void SetId(int id){m_nId = id;};
	int GetId() { return m_nId;}
};
#define STR_SCINTILLAWND _T("Scintilla")
#define STR_LEXERDLL     _T("scilexer.dll")

#endif // !__SCINTILLAWND_H__
