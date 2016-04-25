#include "StdAfx.h"
#include "ConvertString.h"

#include <memory>
namespace ConvStr
{
	// Convert a UTF-8 string to microsoft unicode (UTF-16?). 
	WCHAR* utf8ToUnicode(const char *zFilename){
		if(zFilename == NULL) return _T("");
		int nChar;
		WCHAR *zWideFilename;

		nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
		zWideFilename = (WCHAR*)malloc( nChar*sizeof(zWideFilename[0]) );
		if( zWideFilename==0 ){
			return 0;
		}
		nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zWideFilename, nChar);
		if( nChar==0 ){
			free(zWideFilename);
			zWideFilename = 0;
		}
		return zWideFilename;
	}

	// Convert microsoft unicode to UTF-8.
	char* unicodeToUtf8(const WCHAR *zWideFilename){
		if(zWideFilename == NULL) return "";
		int nByte;
		char *zFilename;

		nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
		zFilename = (char*)malloc( nByte );
		if( zFilename==0 ){
			return 0;
		}
		nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte,
			0, 0);
		if( nByte == 0 ){
			free(zFilename);
			zFilename = 0;
		}
		return zFilename;
	}

	// Convert an ansi string to microsoft unicode, based on the current codepage settings for file apis.
	WCHAR* mbcsToUnicode(const char *zFilename){
		if(zFilename == NULL) return _T("");
		int nByte;
		WCHAR *zMbcsFilename;
		//int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
		int codepage = CP_ACP;

		nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0)*sizeof(WCHAR);
		zMbcsFilename = (WCHAR*)malloc( nByte*sizeof(zMbcsFilename[0]) );
		if( zMbcsFilename==0 ){
			return 0;
		}
		nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
		if( nByte==0 ){
			free(zMbcsFilename);
			zMbcsFilename = 0;
		}
		return zMbcsFilename;
	}

	// Convert microsoft unicode to multibyte character string, based on the user's Ansi codepage.
	char* unicodeToMbcs(const WCHAR *zWideFilename){
		if(zWideFilename == NULL) return "";
		int nByte;
		char *zFilename;
		//int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
		int codepage = CP_ACP;

		nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
		zFilename = (char*)malloc( nByte );
		if( zFilename==0 ){
			return 0;
		}
		nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilename, nByte,
			0, 0);
		if( nByte == 0 ){
			free(zFilename);
			zFilename = 0;
		}

		return zFilename;
	}

	// Convert multibyte character string to UTF-8. 
	char* mbcsToUtf8(const char *zFilename){
		if(zFilename == NULL) return "";
		char *zFilenameUtf8;
		WCHAR *zTmpWide;

		zTmpWide = mbcsToUnicode(zFilename);
		if( zTmpWide==0 ){
			return 0;
		}
		zFilenameUtf8 = unicodeToUtf8(zTmpWide);
		free(zTmpWide);
		return zFilenameUtf8;
	}

	//** Convert UTF-8 to multibyte character string.  Space to hold the returned string is obtained from sqliteMalloc().
	char* utf8ToMbcs(const char *zFilename){
		if(zFilename == NULL) return "";
		char *zFilenameMbcs;
		WCHAR *zTmpWide;

		zTmpWide = utf8ToUnicode(zFilename);
		if( zTmpWide==0 ){
			return 0;
		}
		zFilenameMbcs = unicodeToMbcs(zTmpWide);
		free(zTmpWide);
		return zFilenameMbcs;
	}

	std::string CstrtoString(CString& cstr)
	{
		if(cstr.IsEmpty()) return ""; 
		char* buf = unicodeToMbcs(cstr);
		std::string ss(buf);
		free(buf);
		return ss;
	}

	CString StrtoCString(std::string& str)
	{
		if(str.empty()) return _T("");
		WCHAR* buf = mbcsToUnicode(str.c_str());
		CString cstr(buf);
		free(buf);
		return cstr;
	}

	CString utf8ToCstr(const char* utf8)
	{
		if(utf8 == NULL) return _T("");
		WCHAR* wsz = utf8ToUnicode(utf8);
		CString cstr;
		cstr.Format(L"%s", wsz);
		free(wsz);
		return cstr;
	}

}