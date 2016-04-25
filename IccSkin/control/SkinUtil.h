#ifndef SKINUTIL_INCLUDED
#define SKINUTIL_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

// SkinUtil.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkinUtil

#ifdef _AFXEXT
class AFX_EXT_CLASS CSkinUtil
#else
class CSkinUtil
#endif
{
public:
	CSkinUtil();   // standard constructo
	int ExpressionToInt(CString val);
	CRect StringToRect(CString m_Data);
	void CopyBitmap(CDC* dc, CBitmap& mRes, const CBitmap& hbmp, RECT r);
	HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010);
	CString _GetPath(CString strFileName);
	void LoadPictureFile(HDC hdc, LPCTSTR szFile, CBitmap* pBitmap, CSize& mSize);
	COLORREF StringToRGB(CString m_Data);
	COLORREF HexToRgb(CString m_Data);
};

#endif 
