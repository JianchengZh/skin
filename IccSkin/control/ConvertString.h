#ifndef CONVERTSTRING_INCLUDED
#define CONVERTSTRING_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <string>

#ifdef CS_API_EXPORTS
#ifdef _AFXEXT
#define CS_API __declspec(dllexport)
#else
#define CS_API
#endif
#else
#ifdef _AFXEXT
#define CS_API __declspec(dllimport)
#else
#define CS_API
#endif
#endif

namespace ConvStr
{
	CS_API WCHAR* utf8ToUnicode(const char *zFilename);
	CS_API char* unicodeToUtf8(const WCHAR *zWideFilename);
	CS_API WCHAR* mbcsToUnicode(const char *zFilename);
	CS_API char* unicodeToMbcs(const WCHAR *zWideFilename);
	CS_API char* mbcsToUtf8(const char *zFilename);
	CS_API char* utf8ToMbcs(const char *zFilename);
	
	CS_API std::string CstrtoString(CString& cstr);
	CS_API CString StrtoCString(std::string& str);

	CS_API CString utf8ToCstr(const char* utf8);	
}
#endif