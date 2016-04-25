/* @doc
 * @module ScintillaWnd.cpp | Implementation of a Scintilla syntax coloring edit control for MFC
 * This program is an example how to use the excellent scintilla edit control of Neil Hodgson.<nl>
 * See www.scintilla.org for details<nl>
 * Author: Horst Br?kner, hb@ec-logic.com<nl>
 * Environment: VisualC++ Version 6, static Build of Scintilla, SciLexer.dll as Lexer<nl>
 */
#include "stdafx.h"
#include "ScintillaWnd.h"

#include "..\..\scintilla\include\scintilla.h"
#include "..\..\scintilla\include\SciLexer.h"
#include "..\..\scintilla\include\Accessor.h"
#include "..\..\scintilla\include\Propset.h"
#include "..\..\scintilla\include\keywords.h"

// the next 2 arrays are used to determine lexer format from file extensions
static TCHAR *szExtensions[] = 
{
   _T("py"),                                                                // SCLEX_PYTHON
   _T("c|cc|cpp|cxx|cs|h|hh|hpp|hxx|sma|dlg|rc|rc2|idl|odl|mak|js|java"),   // SCLEX_CPP
   _T("htm|html|shtml|htt|cfm|tpl|hta"),         // SCLEX_HTML
   _T("xml|gcl|xsl|svg|xul|xsd|dtd|xslt|axl"),   // SCLEX_XML
   _T("pl|pm|cgi|pod"),                          // SCLEX_PERL
   _T("sql|spec|body|sps|spb|sf|sp"),            // SCLEX_SQL
   _T("vb|bas|frm|cls|ctl|pag|dsr|dob"),         // SCLEX_VB
   _T("properties"),                             // SCLEX_PROPERTIES
   _T("err"),                                    // SCLEX_ERRORLIST
   _T("iface|mak"),                              // SCLEX_MAKEFILE
   _T("bat|cmd|nt"),                             // SCLEX_BATCH
   _T("xcode"),                                  // SCLEX_XCODE
   _T("tex|sty"),                                // SCLEX_LATEX
   _T("lua"),                                    // SCLEX_LUA
   _T("diff"),                                   // SCLEX_DIFF
   _T("conf"),                                   // SCLEX_CONF
   _T("pas|inc|pp"),                             // SCLEX_PASCAL
   _T("ave"),                                    // SCLEX_AVE
   _T("ada|ads|adb"),                            // SCLEX_ADA
   _T("lsp|lisp|scm|smd|ss"),                    // SCLEX_LISP
   _T("rb"),                                     // SCLEX_RUBY
   _T("e"),                                      // SCLEX_EIFFEL
   _T("e"),                                      // SCLEX_EIFFELKW
   _T("tcl"),                                    // SCLEX_TCL
   _T("tab|spf"),                                // SCLEX_NNCRONTAB
   _T("ant"),                                    // SCLEX_BULLANT
   _T("vbs|dsm"),                                // SCLEX_VBSCRIPT
   _T("asp|aspx"),                               // SCLEX_ASP
   _T("php|php3|php4"),                          // SCLEX_PHP
   _T("bc|cln"),                                 // SCLEX_BAAN
   _T("m"),                                      // SCLEX_MATLAB
   _T("sol"),                                    // SCLEX_SCRIPTOL
   _T("asm"),                                    // SCLEX_ASM
   _T("cpp"),                                    // SCLEX_CPPNOCASE
   _T("f|for|f90|f95"),                          // SCLEX_FORTRAN
   _T("f77"),                                    // SCLEX_F77
   _T("css"),                                    // SCLEX_CSS
   _T("pov"),                                    // SCLEX_POV
   _T("lt|lot"),                                 // SCLEX_LOUT
   _T("src|em"),                                 // SCLEX_ESCRIPT
   0,
};
static int nFormats[] = {
   SCLEX_PYTHON,
   SCLEX_CPP,
   SCLEX_HTML,
   SCLEX_XML,
   SCLEX_PERL,
   SCLEX_SQL,
   SCLEX_VB,
   SCLEX_PROPERTIES,
   SCLEX_ERRORLIST,
   SCLEX_MAKEFILE,
   SCLEX_BATCH,
   SCLEX_XCODE,
   SCLEX_LATEX,
   SCLEX_LUA,
   SCLEX_DIFF,
   SCLEX_CONF,
   SCLEX_PASCAL,
   SCLEX_AVE,
   SCLEX_ADA,
   SCLEX_LISP,
   SCLEX_RUBY,
   SCLEX_EIFFEL,
   SCLEX_EIFFELKW,
   SCLEX_TCL,
   SCLEX_NNCRONTAB,
   SCLEX_BULLANT,
   SCLEX_VBSCRIPT,
   SCLEX_ASP,
   SCLEX_PHP,
   SCLEX_BAAN,
   SCLEX_MATLAB,
   SCLEX_SCRIPTOL,
   SCLEX_ASM,
   SCLEX_CPPNOCASE,
   SCLEX_FORTRAN,
   SCLEX_F77,
   SCLEX_CSS,
   SCLEX_POV,
   SCLEX_LOUT,
   SCLEX_ESCRIPT,
   0,
};

#include <fstream>
#include <iostream>
#include <io.h>
using namespace std;

#include "SkinUtil.h"
#include "IniFile.H"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/////////////////////////////////////
// @mfunc This is an empty constructor
// @rvalue void | not used
//
CScintillaWnd::CScintillaWnd()
{
   m_bLinenumbers = FALSE;
   m_bSelection = TRUE;
   m_bFolding = FALSE;
   m_nSearchflags = 0;
   m_bMultiline = TRUE;
   m_bReadonly = FALSE;
   m_bTransparentThumb = FALSE;
   m_nId = 0;
}

/////////////////////////////////////
// @mfunc This is a destructor
// @rvalue void | not used
//
CScintillaWnd::~CScintillaWnd()
{

}
/////////////////////////////////////
// @mfunc Create the window
// @rvalue BOOL | TRUE on success else FALSE on error
//
BOOL CScintillaWnd::Create (LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	//if (!CWnd::CreateEx(WS_EX_CLIENTEDGE, STR_SCINTILLAWND,lpszWindowName, dwStyle, rect, pParentWnd,(UINT)nID))
	if (!CWnd::CreateEx(NULL, STR_SCINTILLAWND,lpszWindowName, dwStyle, rect, pParentWnd,(UINT)nID))
   {   
      LPVOID lpMsgBuf;
      ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,0,NULL);
      // Write to stderr
      TRACE (_T("%s\n"), (LPCTSTR)lpMsgBuf);
      // Free the buffer.
      LocalFree( lpMsgBuf );
      return FALSE;
   }
   return TRUE;
}
/////////////////////////////////////
// @mfunc Try to load the Scintilla dll - usually named "SciLexer.dll" or "Scintilla.dll".  We try to locate the dll in 
// the current dirtectory and along the path environment.
// Call this function in your CWinApp derived application in the InitInstance function by calling:<nl>
// CScintillaWnd::LoadScintillaDll()<nl>
// @rvalue BOOL | FALSE on error - TRUE on success
//
HMODULE CScintillaWnd::LoadScintillaDll (
                                         LPCSTR szDllFile) //@parm filename of the lexer dll - default "SciLexer.dll"
{
   CString strLexer = STR_LEXERDLL;
   if (szDllFile != NULL)
      strLexer = szDllFile;
// this call to LoadLibrary searches in:
// 1.) current directory
// 2.) wint
// 3.) winnt/system32
// 4.) path
	HMODULE hModule = ::LoadLibrary(strLexer);
// if load fails get an extended error message 
	if (hModule == NULL)
	{
      LPVOID lpMsgBuf;
      ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,0,NULL);
      // Write to stderr
      //TRACE (_T("%s:%s\n"), (LPCSTR)strLexer, (LPCTSTR)lpMsgBuf);
	  TRACE (_T("%s:%s\n"), (LPCTSTR)strLexer, (LPCTSTR)lpMsgBuf);
      // Free the buffer.
      LocalFree( lpMsgBuf );
      return NULL;
	}
   return hModule;
}
/////////////////////////////////////
// @mfunc Reset the Scintiall control and add new Text
// @rvalue void | not used
//
void CScintillaWnd::SetText (
                             LPCSTR szText) //@parm pointer to new text
{
   LRESULT lResult = 0;
   if (szText != NULL)
	   lResult = SendMessage(SCI_SETTEXT,0,(LPARAM)szText);
   GotoPosition(0);
   SetFocus();
}
/////////////////////////////////////
// @mfunc Get the text from the control
// @rvalue void | not used
//
void CScintillaWnd::GetText (
                             CString &strText) //@parm handle to receive text
{
   //LPSTR szText = GetText();
	LPTSTR szText = GetText();
   if (szText != NULL)
   {
      strText = szText;
      delete [] szText;
   }
}
/////////////////////////////////////
// @mfunc Get the text from the control
// @rvalue LPSTR | a character string with text from the control - NULL on error - the caller has to free pointer
//
//LPSTR CScintillaWnd::GetText ()
LPTSTR CScintillaWnd::GetText ()
{
   long lLen = SendMessage(SCI_GETLENGTH, 0, 0) + 1;
   if (lLen > 0)
   {
      TCHAR *pReturn = new TCHAR[lLen];
      if (pReturn != NULL)
      {
         *pReturn = '\0';
         SendMessage(SCI_GETTEXT, lLen, (long)pReturn);
         return pReturn;
      }
   }
   return NULL;
}
/////////////////////////////////////
// @mfunc Try to load a new file
// @rvalue BOOL | FALSE on error - TRUE on success
//
BOOL CScintillaWnd::LoadFile (
                              LPCTSTR szPath) //@parm filename of to load
{
// if pathname is empty do nothing
   if (szPath == NULL || *szPath == '\0')
      return TRUE;
// try to get extension and figure out what lexer to use
   CString strFile(szPath);
   int nIndex = strFile.ReverseFind('.');
   CString strExtension = strFile.Right(strFile.GetLength()-nIndex-1);
   SendMessage(SCI_SETLEXER, GetFormatFromExtension(strExtension), 0);

   BOOL bReturn = TRUE;
// ty to open file in sharing mode
   ::ifstream file;

   long len = 0L;
   long nTotal;
   CHAR *szBuffer = NULL;

   file.open(szPath, ios::in | ios::binary);//, filebuf::sh_read);

// ok success - try to get length of file
   if (file.is_open())
   {
	   file.seekg(0, ios::end);
	   len = file.tellg();
	   file.seekg(0, ios::beg);

	   bool bUnicode = false;

      //len = _filelength(file.fd());
      if (len > 0)
      {
		  char bom;//byte-order mark
		  file.read(&bom, sizeof(char));
		  if(bom == char(0xfeff))
			  bUnicode = true;

//       alloc new buffer of sie = filesize+1 for termination NULL
         szBuffer = new CHAR[len+1];
         if (szBuffer != NULL)
         {
            file.read(szBuffer, len);
            nTotal = file.gcount();
            if (nTotal > 0 && nTotal <= len)
               szBuffer[nTotal] = '\0';
//          read error
            if (file.bad())
            {
               file.close();
               bReturn = FALSE;
            }
         }
         else
         {
            file.close();
            bReturn = FALSE;
         }
      }
      file.close();
//    set text to control

      SetText (szBuffer);
//    tell scintilla that we have an unmodified document
      SendMessage(SCI_SETSAVEPOINT,0 , 0);
      GotoPosition(0);

   }
// file open error - return
   else
      bReturn = FALSE;
// clean up
   if (szBuffer != NULL)
      delete [] szBuffer;
   return bReturn;
}
/////////////////////////////////////
// @mfunc Try to save the file
// @rvalue BOOL | FALSE on error - TRUE on success
//
BOOL CScintillaWnd::SaveFile (
                              LPCTSTR szPath) //@parm filename to save to
{
#ifdef _UNICODE
	::wofstream file;
#else
   ::ofstream file;
#endif
   file.open(szPath);
   if (file.fail())
   {
     return FALSE;
   }
	int buflen = SendMessage(SCI_GETLENGTH)+1; //last NULL
	TCHAR *pBuffer = new TCHAR[buflen];
   if (pBuffer != NULL)
   {
	   SendMessage(SCI_GETTEXT, buflen,(long)pBuffer);
      file.write(pBuffer, buflen-1);
      delete [] pBuffer;
   }
   file << ends;
   file.close();
   return TRUE;
}
/////////////////////////////////////
// @mfunc Try to find format for lexer by looking at the file extension.<nl>
// See global arrays at top of file.
// @rvalue int | Scintilla integer format for lexer
//
int CScintillaWnd::GetFormatFromExtension (LPCTSTR szExtension) //@parm filename extension without dot e.g. "cpp"
{
   //int i = 0;
   //TCHAR *pExtension = szExtensions[i];
   //while (pExtension != NULL)
   //{
   //   CTokenizer st(szExtensions[i], _T("|"));
   //   while (st.HasMoreTokens())
   //   {
   //      if (st.Next().CompareNoCase(szExtension) == 0)
   //         return nFormats[i];
   //   }
   //   i++;
   //   pExtension = szExtensions[i];
   //}
   return SCLEX_NULL;
}
/////////////////////////////////////
// @mfunc Try to calculate the number of characters needed for the display of the linenumbers.
// This function returns 3 for 10-99 lines in file, 4 for 100-999 lines and so on.
// @rvalue int | number of characters needed to display linenumbers
//
int CScintillaWnd::GetLinenumberChars ()
{
// try to get number of lines in control
   LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);

   int nChars = 1;
   while (lLines > 0)
   {
      lLines = lLines / 10;
      nChars++;
   }
   return nChars; // allow an extra char for safety
}
/////////////////////////////////////
// @mfunc Calculate a reasonable width for Linenumberdisplay
// @rvalue int | number of pixels for the margin width of margin (0) 
//
int CScintillaWnd::GetLinenumberWidth ()
{
// get number of chars needed to display highest linenumber
   int nChars = GetLinenumberChars ()+1;
// get width of character '9' in pixels
   LRESULT lWidth = SendMessage(SCI_TEXTWIDTH, STYLE_LINENUMBER, (long)_T("9"));
   return nChars * lWidth;
}
/////////////////////////////////////
// @mfunc Set the display of line numbers on or off.
// Scintilla uses 3 "Margin" at the left of the edit window. The Margin (0)
// is used to display the linenumbers. If set to 0 no numbers are displayed. If
// set to a width > 0 - linenumbers are displayed. We use a helper function to 
// calculate the size of this margin.
// @rvalue int | number of pixels for the margin width of margin (0) 
//
void CScintillaWnd::SetDisplayLinenumbers(
                                          BOOL bFlag) //@parm flag if we shuld display line numbers
{
// if nothing changes just return
   if (GetDisplayLinenumbers() == bFlag)
      return;
// if display is turned off we set margin 0 to 0
   if (!bFlag)
   {
	   SendMessage(SCI_SETMARGINWIDTHN, 0, 0);
   }
// if display is turned o we set margin 0 to the calculated width
   else
   {
      int nWidth = GetLinenumberWidth() + 4;
	   SendMessage(SCI_SETMARGINWIDTHN, 0, nWidth);
   }
   m_bLinenumbers = bFlag;
}
/////////////////////////////////////
// @mfunc Cut the selected text to the clipboard
// @rvalue void | not used
//
void CScintillaWnd::Cut()
{
   SendMessage(SCI_CUT, 0, 0);
}
/////////////////////////////////////
// @mfunc Copy the selected text to the clipboard
// @rvalue void | not used
//
void CScintillaWnd::Copy()
{
   SendMessage(SCI_COPY, 0, 0);
}
/////////////////////////////////////
// @mfunc Paste the text from the clipboard to the control
// @rvalue void | not used
//
void CScintillaWnd::Paste()
{
   SendMessage(SCI_PASTE, 0, 0);
}
/////////////////////////////////////
// @mfunc Delete the selected text
// @rvalue void | not used
//
void CScintillaWnd::Clear()
{
   SendMessage(SCI_CLEAR, 0, 0);
}
/////////////////////////////////////
// @mfunc Select the complete text
// @rvalue void | not used
//
void CScintillaWnd::SelectAll()
{
   SendMessage(SCI_SELECTALL, 0, 0);
}
/////////////////////////////////////
// @mfunc Undo the last action
// @rvalue void | not used
//
void CScintillaWnd::Undo()
{
   SendMessage(SCI_UNDO, 0, 0);
}
/////////////////////////////////////
// @mfunc Redo the last undone action
// @rvalue void | not used
//
void CScintillaWnd::Redo()
{
   SendMessage(SCI_REDO, 0, 0);
}
/////////////////////////////////////
// @mfunc Returns a flag if we can undo the last action
// @rvalue BOOL | TRUE if we can undo - else FALSE
//
BOOL CScintillaWnd::CanUndo()
{
   return SendMessage(SCI_CANUNDO, 0, 0) != 0;
}
/////////////////////////////////////
// @mfunc Returns a flag if we can redo the last undone action
// @rvalue BOOL | TRUE if we can redo - else FALSE
//
BOOL CScintillaWnd::CanRedo()
{
   return SendMessage(SCI_CANREDO, 0, 0) != 0;
}
/////////////////////////////////////
// @mfunc Returns a flag if there is text in the clipboard which we can paste
// @rvalue BOOL | TRUE if the clipboard contains text to paste - else FALSE
//
BOOL CScintillaWnd::CanPaste()
{
   return SendMessage(SCI_CANPASTE, 0, 0) != 0;
}
/////////////////////////////////////
// @mfunc Get the current line number - this the with the caret in it
// @rvalue long | line number with the caret in it - starts with 1
//
long CScintillaWnd::GetCurrentLine()
{
   long lPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   return SendMessage(SCI_LINEFROMPOSITION, lPos, 0) + 1;
}
/////////////////////////////////////
// @mfunc Get the current column number = position of the caret within the line.
// This return value my be affected by the TAB setting! Starts with 1
// @rvalue long | current column number
//
long CScintillaWnd::GetCurrentColumn()
{
   long lPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   return SendMessage(SCI_GETCOLUMN, lPos, 0) + 1;
}
/////////////////////////////////////
// @mfunc Return the current character position within the file.
// @rvalue long | current character position
//
long CScintillaWnd::GetCurrentPosition()
{
   return SendMessage(SCI_GETCURRENTPOS, 0, 0);
}
/////////////////////////////////////
// @mfunc Return the current style at the caret
// @rvalue int | the current style index (0...127)
//
int CScintillaWnd::GetCurrentStyle()
{
   long lPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   return SendMessage(SCI_GETSTYLEAT, lPos, 0);
}
/////////////////////////////////////
// @mfunc Return the current folding level at the caret line
// @rvalue int | the current folding level (0...n)
//
int CScintillaWnd::GetCurrentFoldinglevel()
{
   long lLine = GetCurrentLine();
   int level = (SendMessage(SCI_GETFOLDLEVEL, lLine, 0)) & SC_FOLDLEVELNUMBERMASK;
   return level-1024;
}
/////////////////////////////////////
// @mfunc set the fontname e.g. "Arial"
// @rvalue void | not used
//
void CScintillaWnd::SetFontname(int nStyle, LPCSTR szFontname)
{
   SendMessage(SCI_STYLESETFONT, nStyle, (long)szFontname);
}
/////////////////////////////////////
// @mfunc Set the font height in points
// @rvalue void | not used
//
void CScintillaWnd::SetFontheight(int nStyle, int nHeight)
{
   SendMessage(SCI_STYLESETSIZE, nStyle, (long)nHeight);
}
/////////////////////////////////////
// @mfunc Set the foreground color
// @rvalue void | not used
//
void CScintillaWnd::SetForeground(int nStyle, COLORREF crForeground)
{
   SendMessage(SCI_STYLESETFORE, nStyle, (long)crForeground);
}
/////////////////////////////////////
// @mfunc set the backgroundcolor
// @rvalue void | not used
//
void CScintillaWnd::SetBackground(int nStyle, COLORREF crBackground)
{
   SendMessage(SCI_STYLESETBACK, nStyle, (long)crBackground);
}
/////////////////////////////////////
// @mfunc Set the selected foreground color
// @rvalue void | not used
//
void CScintillaWnd::SetSelForeground(int nStyle, COLORREF crForeground)
{
	SendMessage(SCI_SETSELFORE, nStyle, (long)crForeground);
}
/////////////////////////////////////
// @mfunc set the selected backgroundcolor
// @rvalue void | not used
//
void CScintillaWnd::SetSelBackground(int nStyle, COLORREF crBackground)
{
	SendMessage(SCI_SETSELBACK, nStyle, (long)crBackground);
}
/////////////////////////////////////
// @mfunc set given style to bold
// @rvalue void | not used
//
void CScintillaWnd::SetBold(int nStyle, BOOL bBold)
{
   SendMessage(SCI_STYLESETBOLD, nStyle, (long)bBold);
}
/////////////////////////////////////
// @mfunc set given style to bold
// @rvalue void | not used
//
void CScintillaWnd::SetItalic(int nStyle, BOOL bItalic)
{
   SendMessage(SCI_STYLESETITALIC, nStyle, (long)bItalic);
}
/////////////////////////////////////
// @mfunc set given style to bold
// @rvalue void | not used
//
void CScintillaWnd::SetUnderline(int nStyle, BOOL bUnderline)
{
   SendMessage(SCI_STYLESETUNDERLINE, nStyle, (long)bUnderline);
}
/////////////////////////////////////
// @mfunc Return flag if we are in overstrike mode
// @rvalue BOOL | TRUE if we are in overstrike mode - else FALSE
//
BOOL CScintillaWnd::GetOverstrike()
{
   return SendMessage(SCI_GETOVERTYPE , 0, 0);
}
/////////////////////////////////////
// @mfunc set set overstrike mode (TRUE) or insert mode (FALSE)
// @rvalue void | not used
//
void CScintillaWnd::SetOverstrike(BOOL bOverstrike)
{
   SendMessage(SCI_SETOVERTYPE, bOverstrike, 0);
}
/////////////////////////////////////
// @mfunc Toggle the display of the selection bookmark margin
// @rvalue void | not used
//
void CScintillaWnd::SetDisplaySelection(BOOL bFlag)
{
   m_bSelection = bFlag;
   if (bFlag)
	   SendMessage(SCI_SETMARGINWIDTHN, 1, 16);
   else
	   SendMessage(SCI_SETMARGINWIDTHN, 1, 0);
}
/////////////////////////////////////
// @mfunc Toggle the display of the folding margin
// @rvalue void | not used
//
void CScintillaWnd::SetDisplayFolding(BOOL bFlag)
{
   m_bFolding = bFlag;
   if (bFlag)
	   SendMessage(SCI_SETMARGINWIDTHN, 2, 16);
   else
	   SendMessage(SCI_SETMARGINWIDTHN, 2, 0);
}
/////////////////////////////////////
// @mfunc init the view with reasonable defaults
// @rvalue void | not used
//
void CScintillaWnd::Init()
{
// clear all text styles
   SendMessage(SCI_CLEARDOCUMENTSTYLE, 0, 0);
// set the number of styling bits to 7 - the asp/html views need a lot of styling - default is 5
// If you leave the default you will see twiggle lines instead of ASP code
	SendMessage(SCI_SETSTYLEBITS, 7, 0);
// set the display for indetation guides to on - this displays virtical dotted lines from the beginning of 
// a code block to the end of the block
	SendMessage(SCI_SETINDENTATIONGUIDES, TRUE, 0);
// set tabwidth to 3
	SendMessage(SCI_SETTABWIDTH,3,0);
// set indention to 3
	SendMessage(SCI_SETINDENT,3,0);
// set the caret blinking time to 400 milliseconds
	SendMessage(SCI_SETCARETPERIOD,400,0);
// source folding section
// tell the lexer that we want folding information - the lexer supplies "folding levels"
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold.html"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold.html.preprocessor"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold.comment"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold.at.else"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold.flags"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("fold.preprocessor"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("styling.within.preprocessor"), (LPARAM)_T("1"));
   SendMessage(SCI_SETPROPERTY, (WPARAM)_T("asp.default.language"), (LPARAM)_T("1"));
// Tell scintilla to draw folding lines UNDER the folded line
   SendMessage(SCI_SETFOLDFLAGS, 16,0);
// Set margin 2 = folding margin to display folding symbols
	SendMessage(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
// allow notifications for folding actions
   SendMessage(SCI_SETMODEVENTMASK, SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT, 0);
//   SendMessage(SCI_SETMODEVENTMASK, SC_MOD_CHANGEFOLD|SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT, 0);
// make the folding margin sensitive to folding events = if you click into the margin you get a notification event
	SendMessage(SCI_SETMARGINSENSITIVEN, 2, TRUE);
// define a set of markers to displa folding symbols
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PLUS);
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY);
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY);
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
   SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
// set the forground color for some styles
   SendMessage(SCI_STYLESETFORE, 0, COLOR_BLACK);
   SendMessage(SCI_STYLESETFORE, 2, RGB(0,64,0));
   SendMessage(SCI_STYLESETFORE, 5, COLOR_BLUE);
   SendMessage(SCI_STYLESETFORE, 6, RGB(200,20,0));
   SendMessage(SCI_STYLESETFORE, 9, COLOR_BLUE);
   SendMessage(SCI_STYLESETFORE, 10, RGB(255,0,64));
   SendMessage(SCI_STYLESETFORE, 11, COLOR_BLACK);
// set the backgroundcolor of brace highlights (),{}
  // SendMessage(SCI_STYLESETBACK, STYLE_BRACELIGHT, RGB(0,255,0));
// set end of line mode to CRLF
   SendMessage(SCI_CONVERTEOLS, 2, 0);
   SendMessage(SCI_SETEOLMODE, 2, 0);
//   SendMessage(SCI_SETVIEWEOL, TRUE, 0);
// set markersymbol for marker type 0 - bookmark
   SendMessage(SCI_MARKERDEFINE, 0, SC_MARK_CIRCLE);
// display all margins
   SetDisplayLinenumbers(FALSE);
   SetDisplayFolding(FALSE);
   SetDisplaySelection(FALSE);

}
/////////////////////////////////////
// @mfunc Update UI and do brace matching
// @rvalue void | not used
//
void CScintillaWnd::UpdateUI()
{
// do brace matching
   long lStart = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   long lEnd = SendMessage(SCI_BRACEMATCH, lStart-1, 0);
// if there is a matching brace highlight it
   if (lEnd >= 0)
      SendMessage(SCI_BRACEHIGHLIGHT, lStart-1, lEnd);
// if there is NO matching brace erase old highlight
   else
      SendMessage(SCI_BRACEHIGHLIGHT, -1, -1);
}
/////////////////////////////////////
// @mfunc Do default folding for given margin and position - usually called from notification handler
// @rvalue void | not used
//
void CScintillaWnd::DoDefaultFolding(
                                     int nMargin,      //@parm maring we want to handle - normally (2)
                                     long lPosition)   //@parm character position where user clicked margin
{
// simply toggle fold
   if (nMargin == 2)
   {
      long lLine = SendMessage(SCI_LINEFROMPOSITION, lPosition, 0);
      SendMessage(SCI_TOGGLEFOLD, lLine, 0);
   }
}
/////////////////////////////////////
// @mfunc Do default folding for given margin and position - usually called from notification handler
// @rvalue void | not used
//
void CScintillaWnd::Refresh()
{
   SendMessage(SCI_COLOURISE, 0, -1);
}
/////////////////////////////////////
// @mfunc Add a bookmark at given line
// @rvalue void | not used
//
void CScintillaWnd::AddBookmark(
                                long lLine) //@parm line where to add bookmark - lines start at 1
{
   SendMessage(SCI_MARKERADD, lLine-1, 0);
}
/////////////////////////////////////
// @mfunc Delete a bookmark at given line
// @rvalue void | not used
//
void CScintillaWnd::DeleteBookmark(
                                   long lLine) //@parm line where to delete bookmark - lines start at 1
{
   if (HasBookmark(lLine))
   {
      SendMessage(SCI_MARKERDELETE, lLine-1, 0);
   }
}
/////////////////////////////////////
// @mfunc Check if given line has a bookmark
// @rvalue BOOL | TRUE if given line has a bookmark - else FALSE
//
BOOL CScintillaWnd::HasBookmark(
                                long lLine) //@parm line where to add bookmark - lines start at 1
{
   int n = SendMessage(SCI_MARKERGET, lLine-1, 0);
// check mask for markerbit 0
   if (n & 0x1)
      return TRUE;
   return FALSE;
}
/////////////////////////////////////
// @mfunc Find next bookmark from current line
// @rvalue void | not used
//
void CScintillaWnd::FindNextBookmark()
{
	long lLine = SendMessage(SCI_MARKERNEXT, GetCurrentLine(), 1);
   if (lLine >= 0)
      SendMessage(SCI_GOTOLINE, lLine,0);
}
/////////////////////////////////////
// @mfunc Find previous bookmark from current line
// @rvalue void | not used
//
void CScintillaWnd::FindPreviousBookmark()
{
	long lLine = SendMessage(SCI_MARKERPREVIOUS, GetCurrentLine()-2, 1);
   if (lLine >= 0)
      SendMessage(SCI_GOTOLINE, lLine,0);
}
/////////////////////////////////////
// @mfunc Goto givven character position
// @rvalue void | not used
//
void CScintillaWnd::GotoPosition(
                                 long lPos) //@parm new character position
{
	SendMessage(SCI_GOTOPOS, lPos, 0);
}
/////////////////////////////////////
// @mfunc Goto givven line position
// @rvalue void | not used
//
void CScintillaWnd::GotoLine(
                             long lLine) //@parm new line - lines start at 1
{
	SendMessage(SCI_GOTOLINE, lLine-1, 0);
}
/////////////////////////////////////
// @mfunc Search forward for a given string and select it if found. You may use regular expressions on the text.
// @rvalue BOOL | TRUE if text is ffound else FALSE
//
BOOL CScintillaWnd::SearchForward(
                                  LPSTR szText) //@parm text to search
{
   if (szText == NULL)
      return FALSE;
   long lPos = GetCurrentPosition();
   TextToFind tf;
   tf.lpstrText = szText;
   tf.chrg.cpMin = lPos+1;
   tf.chrg.cpMax = SendMessage(SCI_GETLENGTH, 0, 0);
   lPos = SendMessage(SCI_FINDTEXT, m_nSearchflags, (long)&tf);
   if (lPos >= 0)
   {
      SetFocus();
      GotoPosition(lPos);
      SendMessage(SCI_SETSEL, tf.chrgText.cpMin, tf.chrgText.cpMax);
      SendMessage(SCI_FINDTEXT, m_nSearchflags, (long)&tf);
      return TRUE;
   }
   return FALSE;
}
/////////////////////////////////////
// @mfunc Search backward for a given string and select it if found. You may use regular expressions on the text.
// @rvalue BOOL | TRUE if text is ffound else FALSE
//
BOOL CScintillaWnd::SearchBackward(
                                  LPSTR szText) //@parm text to search
{
   if (szText == NULL)
      return FALSE;
   long lPos = GetCurrentPosition();
   long lMinSel = GetSelectionStart();
   TextToFind tf;
   tf.lpstrText = szText;
   if (lMinSel >= 0)
      tf.chrg.cpMin = lMinSel-1;
   else
      tf.chrg.cpMin = lPos-1;
   tf.chrg.cpMax = 0;
   lPos = SendMessage(SCI_FINDTEXT, m_nSearchflags, (long)&tf);
   if (lPos >= 0)
   {
      SetFocus();
      GotoPosition(lPos);
      SendMessage(SCI_SETSEL, tf.chrgText.cpMin, tf.chrgText.cpMax);
      SendMessage(SCI_FINDTEXT, m_nSearchflags, (long)&tf);
      return TRUE;
   }
   return FALSE;
}
/////////////////////////////////////
// @mfunc Replace a text with a new text. You can use regular expression with tagging on the replacement text.
// @rvalue void | not used
//
void CScintillaWnd::ReplaceSearchedText(
                                        LPCSTR szText) //@parm new text
{
   if (szText == NULL)
      return;
	SendMessage(SCI_TARGETFROMSELECTION, 0, 0);
   if (m_nSearchflags & SCFIND_REGEXP)
      SendMessage(SCI_REPLACETARGETRE, strlen(szText), (long)szText);
   else
      SendMessage(SCI_REPLACETARGET, strlen(szText), (long)szText);
}
/////////////////////////////////////
// @mfunc Set lexer format - see scintilla.h
// @rvalue void | not used
//
void CScintillaWnd::SetLexer(
                             int nLexer) //@parm lexer code e.g. 
{
   SendMessage(SCI_SETLEXER, nLexer, 0);
}
/////////////////////////////////////
// @mfunc Get start of selection (anchor) as character position
// @rvalue long | character position of selection start (anchor) - -1 on error
//
long CScintillaWnd::GetSelectionStart()
{
   return SendMessage(SCI_GETSELECTIONSTART, 0, 0);
}
/////////////////////////////////////
// @mfunc Get end of selection as character position
// @rvalue long | character position of selection end - -1 on error
//
long CScintillaWnd::GetSelectionEnd()
{
   return SendMessage(SCI_GETSELECTIONEND, 0, 0);
}
/////////////////////////////////////
// @mfunc Get selected string
// @rvalue CString | string with currentliy selected text
//
CString CScintillaWnd::GetSelectedText()
{
   long lLen = (GetSelectionEnd() - GetSelectionStart()) + 1;
   if (lLen > 0)
   {
      TCHAR *p = new TCHAR[lLen+1];
      if (p != NULL)
      {
         *p = '\0';
         SendMessage(SCI_GETSELTEXT, 0, (long)p);
         CString strReturn = p;
         delete [] p;
         return strReturn;
      }
   }
   return _T("");
}
/////////////////////////////////////
// @mfunc Replace a text in a selection or in the complete file multiple times
// @rvalue int | number of replacements
//
int CScintillaWnd::ReplaceAll(
                              LPCSTR szFind, 
                              LPCSTR szReplace, 
                              BOOL bUseSelection)
{
   int nCount = 0;
// different branches for replace in selection or total file
   if (bUseSelection)
   {
//    get starting selection range
      long lLen = 0;
      long lStart = GetSelectionStart();
      long lEnd = GetSelectionEnd();
//    set target to selection
      SendMessage(SCI_SETTARGETSTART, lStart);
      SendMessage(SCI_SETTARGETEND, lEnd);
//    try to find text in target for the first time
	   long lPos = SendMessage(SCI_SEARCHINTARGET, strlen(szFind), (long)szFind);
//    loop over selection until end of selection reached - moving the target start each time
      while (lPos < lEnd && lPos >= 0)
      {
         if (m_nSearchflags & SCFIND_REGEXP) // check for regular expression flag
            lLen = SendMessage(SCI_REPLACETARGETRE, strlen(szReplace), (long)szReplace);
         else
            lLen = SendMessage(SCI_REPLACETARGET, strlen(szReplace), (long)szReplace);
//       the end of the selection was changed - recalc the end
         lEnd = GetSelectionEnd();
//       move start of target behind last change and end of target to new end of selection
         SendMessage(SCI_SETTARGETSTART, lPos+lLen);
         SendMessage(SCI_SETTARGETEND, lEnd);
//       find again - if nothing found loop exits
	      lPos = SendMessage(SCI_SEARCHINTARGET, strlen(szFind), (long)szFind);
         nCount++;
      }
   }
   else
   {
//    start with first and last char in buffer
      long lLen = 0;
      long lStart = 0;
      long lEnd = SendMessage(SCI_GETTEXTLENGTH, 0, 0);
//    set target to selection
      SendMessage(SCI_SETTARGETSTART, lStart, 0);
      SendMessage(SCI_SETTARGETEND, lEnd, 0);
//    try to find text in target for the first time
	   long lPos = SendMessage(SCI_SEARCHINTARGET, strlen(szFind), (long)szFind);
//    loop over selection until end of selection reached - moving the target start each time
      while (lPos < lEnd && lPos >= 0)
      {
         if (m_nSearchflags & SCFIND_REGEXP) // check for regular expression flag
            lLen = SendMessage(SCI_REPLACETARGETRE, strlen(szReplace), (long)szReplace);
         else
            lLen = SendMessage(SCI_REPLACETARGET, strlen(szReplace), (long)szReplace);
//       the end of the selection was changed - recalc the end
         lEnd = SendMessage(SCI_GETTEXTLENGTH, 0, 0);;
//       move start of target behind last change and end of target to new end of buffer
         SendMessage(SCI_SETTARGETSTART, lPos+lLen);
         SendMessage(SCI_SETTARGETEND, lEnd);
//       find again - if nothing found loop exits
	      lPos = SendMessage(SCI_SEARCHINTARGET, strlen(szFind), (long)szFind);
         nCount++;
      }
   }
   return nCount;
}

void CScintillaWnd::SetMultiline(BOOL bMultiline)
{
	m_bMultiline = bMultiline;
}

void CScintillaWnd::SetScrollbar(BOOL bShowHScrollbar, BOOL bShowVScrollbar)
{
	SendMessage(SCI_STYLEHSCROLL, 0, bShowHScrollbar);
	SendMessage(SCI_STYLEVSCROLL, 0, bShowVScrollbar);

}

void CScintillaWnd::SetCodepage(int nCodepage)
{
	int charSet = CP_ACP;

	switch (nCodepage)	
	{
	case CP_CHINESE_TRADITIONAL : 
		charSet = SC_CHARSET_CHINESEBIG5;
		break;
	case CP_CHINESE_SIMPLIFIED : 
		charSet = SC_CHARSET_GB2312;
		break;
	case CP_KOREAN : 
		charSet = SC_CHARSET_HANGUL;
		break;
	case CP_JAPANESE : 
		charSet = SC_CHARSET_SHIFTJIS;
		break;
	case CP_GREEK : 
		charSet = SC_CHARSET_GREEK;
		break;
	default : charSet = 0;
	}

	SendMessage(SCI_SETCODEPAGE, nCodepage, 0);

	SendMessage(SCI_STYLESETCHARACTERSET, SCE_C_STRING, charSet);
}

void CScintillaWnd::SetReadOnly(BOOL bReadonly)
{
	m_bReadonly = bReadonly;
	SendMessage(SCI_SETREADONLY, m_bReadonly,0);
}

BOOL CScintillaWnd::PreTranslateMessage(MSG* pMsg)
{
	if(!m_bMultiline)
	{
		if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
			return FALSE;
	}
	return CWnd::PreTranslateMessage(pMsg);
}
BEGIN_MESSAGE_MAP(CScintillaWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

int CScintillaWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CScintillaWnd::OnDestroy()
{
	CWnd::OnDestroy();

#if USE_COOL_SB
	UninitializeCoolSB(m_hWnd);
#endif
	FreeSkin();
}


#if USE_TRANSPARENTBAR
void CScintillaWnd::CopyVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
									  CRect rcVBBN, CRect rcVBBO, CRect rcVBBD, 
									  CRect rcVTHTN, CRect rcVTHTO, CRect rcVTHTD, 
									  CRect rcVTHMN, CRect rcVTHMO, CRect rcVTHMD, 
									  CRect rcVTHBN, CRect rcVTHBO, CRect rcVTHBD,
									  CRect rcVCH)
#else
void CScintillaWnd::CopyVScrollbarFromBG(CBitmap& bmSkinImage, CRect rcVTBN, CRect rcVTBO, CRect rcVTBD, 
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
void CScintillaWnd::CopyHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
									  CRect rcHRBN, CRect rcHRBO, CRect rcHRBD, 
									  CRect rcHTHTN, CRect rcHTHTO, CRect rcHTHTD, 
									  CRect rcHTHMN, CRect rcHTHMO, CRect rcHTHMD, 
									  CRect rcHTHBN, CRect rcHTHBO, CRect rcHTHBD,
									  CRect rcHCH)
#else
void CScintillaWnd::CopyHScrollbarFromBG(CBitmap& bmSkinImage, CRect rcHLBN, CRect rcHLBO, CRect rcHLBD, 
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
LRESULT CScintillaWnd::_SBCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm)
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

					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
					{
						nCenter += 1;
					}

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}
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

					//Thumb 패턴이 1픽셀 못그리는 문제, 나머지 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
						nCenter += 1;
					
					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}

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

					//Thumb 패턴이 1픽셀 못그리는 문제, 나머지 반올림 처리
					int nRest = (ThumbRect.Width()-nWidthC)%2;
					if (nRest == 1)
						nCenter += 1;

					if(ThumbRect.Height() > nHeightT){
						nStartH = (ThumbRect.Height() - nHeightT)/2;
					}

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
					//objDC.SelectObject(&m_bmVSC_Thumb_T_O);
					////memDC.BitBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);
					//
					//objDC.SelectObject(&m_bmVSC_Thumb_C_O);
					////memDC.StretchBlt(nStartW, nHeightT,  nWidthT,nCenter, &objDC, 0,0,nWidthT, 1,SRCCOPY);				
					////memDC.BitBlt(nStartW, nCenter, nWidthC, ThumbRect.Height() - (nHeightT+nHeightB), &objDC, 0, 0, SRCCOPY);
					////memDC.StretchBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0,0,nWidthT, 1,SRCCOPY);
					//memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
					//memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

					//objDC.SelectObject(&m_bmVSC_Thumb_B_O);
					////memDC.BitBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0,0, SRCCOPY);
					//memDC.TransparentBlt(nStartW, ThumbRect.Height() - nHeightB, nWidthB, nHeightB, &objDC, 0, 0, nWidthB, nHeightB,COLOR_WHITE);

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

					//////////////////////////////////////////////////////////////////////////
#endif
					//////////////////////////////////////////////////////////////////////////
					
					if (m_bTransparentThumb)
					{
						objDC.SelectObject(&m_bmVSC_Thumb_T_D);
						memDC.TransparentBlt(nStartW, 0, nWidthT, nHeightT, &objDC, 0, 0, nWidthT, nHeightT, COLOR_WHITE);
						objDC.SelectObject(&m_bmVSC_Thumb_C_D);
						memDC.TransparentBlt(nStartW, nHeightT, nWidthT, nCenter, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter, nWidthC, nHeightC, &objDC, 0, 0, nWidthC, 1, COLOR_WHITE);
						memDC.TransparentBlt(nStartW, nCenter+nHeightC, nWidthT, nCenter-nHeightB, &objDC, 0, 0, nWidthT, 1, COLOR_WHITE);

						objDC.SelectObject(&m_bmVSC_Thumb_B_D);
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
					
					pDC->BitBlt(ThumbRect.left, ThumbRect.top, ThumbRect.Width(), ThumbRect.Height(), &memDC, 0, 0, SRCCOPY);

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

BOOL CScintillaWnd::SetSkin(LPCTSTR lpszIniFile)
{
	m_strSkinFile = lpszIniFile;

	if (_taccess(m_strSkinFile, 0) != 0)
		return FALSE;

	CDC *pDC = GetDC();

	FreeSkin();

	CSkinUtil skinUtil;

	CIniFile Obj_SkinFile(m_strSkinFile);

	CString strSection = _T("EDIT");
	CString strName				= Obj_SkinFile.ReadString(strSection, _T("Name"), strSection);
	CString strID				= Obj_SkinFile.ReadString(strSection, _T("ID"), _T(""));
	CString strStyle			= Obj_SkinFile.ReadString(strSection, _T("ResizeStyle"), _T("M_TOP_LEFT"));
	CString strImage			= Obj_SkinFile.ReadString(strSection, _T("Image"), _T(""));
	CString strPos				= Obj_SkinFile.ReadString(strSection, _T("Position"), _T(""));

	//Scrollbar
	CString strGripColor		= Obj_SkinFile.ReadString(strSection, _T("ScrollGripColor"), _T(""));
	CString strGripLineColor	= Obj_SkinFile.ReadString(strSection, _T("ScrollGripLineColor"), _T(""));

	m_clrGripBk			= skinUtil.StringToRGB(strGripColor);
	m_clrGripOutline		= skinUtil.StringToRGB(strGripLineColor);

	//Vertical button
	CString strVertTopBtnNormalRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalTopBtnNormalRect"), _T(""));
	CString strVertTopBtnOverRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalTopBtnOverRect"), _T(""));
	CString strVertTopBtnDownRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalTopBtnDownRect"), _T(""));

	CString strVertBtmBtnNormalRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalBottomBtnNormalRect"), _T(""));
	CString strVertBtmBtnOverRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalBottomBtnOverRect"), _T(""));
	CString strVertBtmBtnDownRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalBottomBtnDownRect"), _T(""));

	//Vertical Thumb
	CString strVertThumbTopNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbTopNormalRect"), _T(""));
	CString strVertThumbTopOverRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbTopOverRect"), _T(""));
	CString strVertThumbTopDownRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbTopDownRect"), _T(""));

	CString strVertThumbMidNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbMiddleNormalRect"), _T(""));
	CString strVertThumbMidOverRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbMiddleOverRect"), _T(""));
	CString strVertThumbMidDownRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbMiddleDownRect"), _T(""));

	CString strVertThumbBtmNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbBottomNormalRect"), _T(""));
	CString strVertThumbBtmOverRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbBottomOverRect"), _T(""));
	CString strVertThumbBtmDownRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalThumbBottomDownRect"), _T(""));

	//Vertical Channel

	CString strVertChanRectTop			= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalChannelTopRect"), _T(""));
	CString strVertChanRectMid			= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalChannelMiddleRect"), _T(""));
	CString strVertChanRectBtm			= Obj_SkinFile.ReadString(strSection, _T("ScrollVerticalChannelBottomRect"), _T(""));

	//horizontal button
	CString strHorizLeftBtnNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalLeftBtnNormalRect"), _T(""));
	CString strHorizLeftBtnOverRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalLeftBtnOverRect"), _T(""));
	CString strHorizLeftBtnDownRect		= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalLeftBtnDownRect"), _T(""));

	CString strHorizRightBtnNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalRightBtnNormalRect"), _T(""));
	CString strHorizRightBtnOverRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalRightBtnOverRect"), _T(""));
	CString strHorizRightBtnDownRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalRightBtnDownRect"), _T(""));

	//horizontal Thumb
	CString strHorizThumbTopNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbTopNormalRect"), _T(""));
	CString strHorizThumbTopOverRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbTopOverRect"), _T(""));
	CString strHorizThumbTopDownRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbTopDownRect"), _T(""));

	CString strHorizThumbMidNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbMiddleNormalRect"), _T(""));
	CString strHorizThumbMidOverRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbMiddleOverRect"), _T(""));
	CString strHorizThumbMidDownRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbMiddleDownRect"), _T(""));

	CString strHorizThumbBtmNormalRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbBottomNormalRect"), _T(""));
	CString strHorizThumbBtmOverRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbBottomOverRect"), _T(""));
	CString strHorizThumbBtmDownRect	= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalThumbBottomDownRect"), _T(""));

	//horizontal Channel

	CString strHorizChanRectTop			= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalChannelTopRect"), _T(""));
	CString strHorizChanRectMid			= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalChannelMiddleRect"), _T(""));
	CString strHorizChanRectBtm			= Obj_SkinFile.ReadString(strSection, _T("ScrollHorizontalChannelBottomRect"), _T(""));

	CString strMinThumbSize				= Obj_SkinFile.ReadString(strSection, _T("ScrollThumbMinimumSize"), _T("15"));

	//convert string to rect
	CRect Rect_Pos						= skinUtil.StringToRect(strPos);

	m_Rect = Rect_Pos;

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

	CRect Rect_VertTCH		= skinUtil.StringToRect(strVertChanRectTop);
	CRect Rect_VertMCH		= skinUtil.StringToRect(strVertChanRectMid);
	CRect Rect_VertBCH		= skinUtil.StringToRect(strVertChanRectBtm);

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

	CRect Rect_HorizTCH		= skinUtil.StringToRect(strHorizChanRectTop);
	CRect Rect_HorizMCH		= skinUtil.StringToRect(strHorizChanRectMid);
	CRect Rect_HorizBCH		= skinUtil.StringToRect(strHorizChanRectBtm);

	CString strImageFile = skinUtil._GetPath(m_strSkinFile) + strImage;
	if(_taccess(strImageFile, 0) != 0)
		return FALSE;

	CSize mSize;
	skinUtil.LoadPictureFile(pDC->m_hDC, strImageFile, &m_bmSkinImage, mSize);

	CopyVScrollbarFromBG(m_bmSkinImage, Rect_VertTBN, Rect_VertTBO, Rect_VertTBD, Rect_VertBBN, Rect_VertBBO, Rect_VertBBD, 
		Rect_VertTHTN, Rect_VertTHTO, Rect_VertTHTD, Rect_VertTHMN, Rect_VertTHMO, Rect_VertTHMD, Rect_VertTHBN, Rect_VertTHBO, Rect_VertTHBD,
		Rect_VertTCH, Rect_VertMCH, Rect_VertBCH);

	CopyHScrollbarFromBG(m_bmSkinImage, Rect_HorizLBN, Rect_HorizLBO, Rect_HorizLBD, Rect_HorizRBN, Rect_HorizRBO, Rect_HorizRBD, 
		Rect_HorizTHTN, Rect_HorizTHTO, Rect_HorizTHTD, Rect_HorizTHMN, Rect_HorizTHMO, Rect_HorizTHMD, Rect_HorizTHBN, Rect_HorizTHBO, Rect_HorizTHBD,
		Rect_HorizTCH, Rect_HorizMCH, Rect_HorizBCH);

	//ModifyStyle(0,  WS_HSCROLL | WS_VSCROLL );

	m_nMinThumbSize = 18;
	m_nMinThumbSize = _ttoi(strMinThumbSize);

	ReleaseDC(pDC);

	if(IsWindow(this->GetSafeHwnd()))
	{
		SetWindowText(_T(""));//if text is empty, Don't display scrollbar

#if USE_COOL_SB
		InitializeSB(this->m_hWnd);
		SB_SetStyle(this->m_hWnd, SB_BOTH, CSBS_HOTTRACKED);
		SB_SetSize(this->m_hWnd, SB_BOTH, m_nSBWidth, m_nSBHeight);
		SB_SetMinThumbSize(this->m_hWnd, SB_BOTH, m_nMinThumbSize);		
#endif
	}

	return TRUE;
}

void CScintillaWnd::FreeSkin()
{
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
}
