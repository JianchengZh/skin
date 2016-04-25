// BaseSkin.cpp : implementation file
//

#include "stdafx.h"
#include "SkinUtil.h"
#include <olectl.h>

#include <iostream>
#include <string>
#include <vector>
#include "ConvertString.h"
#include "Calculator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinUtil


CSkinUtil::CSkinUtil()
{
}

void CSkinUtil::LoadPictureFile(HDC hdc, LPCTSTR szFile, CBitmap *pBitmap, CSize &mSize)
{
	// open file
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_ASSERTE(-1 != dwFileSize);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	_ASSERTE(FALSE != bRead);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	_ASSERTE(SUCCEEDED(hr) && pstm);

	// Create IPicture from image file
	LPPICTURE gpPicture;

	hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&gpPicture);
	_ASSERTE(SUCCEEDED(hr) && gpPicture);	
	pstm->Release();
	GlobalFree(hGlobal);

	OLE_HANDLE m_picHandle;
	/*
	long hmWidth, hmHeight;
	gpPicture->get_Width(&hmWidth);
	gpPicture->get_Height(&hmHeight);
	int nWidth	= MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_INCH);
	int nHeight	= MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_INCH);
	*/
	gpPicture->get_Handle(&m_picHandle);
	pBitmap->DeleteObject();
	pBitmap->Attach((HGDIOBJ) m_picHandle);

	BITMAP bm;
	GetObject(pBitmap->m_hObject, sizeof(bm), &bm);
	mSize.cx = bm.bmWidth; //nWidth;
	mSize.cy = bm.bmHeight; //nHeight;
}

CString CSkinUtil::_GetPath(CString strFileName)
{
	TCHAR szPath[_MAX_PATH];
	LPTSTR lpszName;

	GetFullPathName(strFileName, _MAX_PATH, szPath, &lpszName);

	CString mPath = szPath;
	int p = mPath.Find(lpszName, 0);
	if (p > -1)
	{
		mPath.Delete(p, _tcslen(lpszName));
	}
	p = mPath.GetLength() - 1;
	if(mPath.GetAt(p) == _T('\\'))
		mPath.Delete(p, 1);
	return mPath + _T('\\');
}

HRGN CSkinUtil::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{

	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
				sizeof(BITMAPINFOHEADER),	// biSize 
				bm.bmWidth,					// biWidth; 
				bm.bmHeight,				// biHeight; 
				1,							// biPlanes; 
				32,							// biBitCount 
				BI_RGB,						// biCompression; 
				0,							// biSizeImage; 
				0,							// biXPelsPerMeter; 
				0,							// biYPelsPerMeter; 
				0,							// biClrUsed; 
				0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}	
	}

	return hRgn;
}

void CSkinUtil::CopyBitmap(CDC *dc, CBitmap &mRes, const CBitmap &hbmp, RECT r)
{
	if(!hbmp.m_hObject) return;
	int w = r.right - r.left,
		h = r.bottom - r.top;

	CDC memdc, hDC;

	mRes.CreateCompatibleBitmap(dc, w, h);

	/*	hDC.CreateCompatibleDC(dc);
	CBitmap *pOldBitmap = (CBitmap *)hDC.SelectObject(mRes);

	memdc.CreateCompatibleDC(dc);
	CBitmap *pOldBitmap2 = (CBitmap *)memdc.SelectObject(hbmp);

	//hDC.StretchBlt(0, 0, w, h, &memdc, r.left, r.top, w, h, SRCCOPY);
	hDC.BitBlt(0, 0, w, h, &memdc, r.left, r.top, SRCCOPY); 

	hDC.SelectObject(pOldBitmap);
	hDC.DeleteDC();

	memdc.SelectObject(pOldBitmap2);
	memdc.DeleteDC();
	*/
	hDC.CreateCompatibleDC(dc);
	hDC.SelectObject((HBITMAP) mRes);

	memdc.CreateCompatibleDC(dc);
	memdc.SelectObject(hbmp);

	hDC.BitBlt(0, 0, w, h, &memdc, r.left, r.top, SRCCOPY); 

	hDC.DeleteDC();
	memdc.DeleteDC();
}

int CSkinUtil::ExpressionToInt(CString val)
{
	std::string postfix = GetPostFix(ConvStr::CstrtoString(val));
	int result = Calculate(postfix);
	return result;
}

CRect CSkinUtil::StringToRect(CString val)
{
	CString tmp = val.Trim();
	if(tmp == "") return RGB(0,0,0);

	CString l = _T(""), t = _T(""), w = _T(""), h = _T("");

	AfxExtractSubString( l, val, 0, _T(','));
	AfxExtractSubString( t, val, 1, _T(','));
	AfxExtractSubString( w, val, 2, _T(','));
	AfxExtractSubString( h, val, 3, _T(','));
	
	CRect r;
	r.left = ExpressionToInt(l);//_ttoi(l);
	r.top  = ExpressionToInt(t);//_ttoi(t);
	r.right = r.left + ExpressionToInt(w);//_ttoi(w);
	r.bottom = r.top + ExpressionToInt(h);//_ttoi(h);
	return r;
}

COLORREF CSkinUtil::StringToRGB(CString val)
{
	if(val.Left(1) == _T("#"))
		return HexToRgb(val);

	CString tmp = val.Trim();
	if(tmp == "") return RGB(0,0,0);

	CString sr = _T(""), sg = _T(""), sb = _T("");

	AfxExtractSubString( sr, val, 0, _T(','));
	AfxExtractSubString( sg, val, 1, _T(','));
	AfxExtractSubString( sb, val, 2, _T(','));

	BYTE r,g,b;
	r = _ttoi(sr);
	g = _ttoi(sg);
	b = _ttoi(sb);


	return RGB(r,g,b);
}


using namespace std;

int convertFromHex(string hex)
{
    int value = 0;
	int a = 0;
    int b = hex.length() - 1;

    for (; b >= 0; a++, b--)
    {
        if (hex[b] >= '0' && hex[b] <= '9')
        {
            value += (hex[b] - '0') * (1 << (a * 4));
        }
        else
        {
            switch (hex[b])
            {
                case 'A':
                case 'a':
                    value += 10 * (1 << (a * 4));
                    break;
                case 'B':
                case 'b':
                    value += 11 * (1 << (a * 4));
                    break;
                case 'C':
                case 'c':
                    value += 12 * (1 << (a * 4));
                    break;
                case 'D':
                case 'd':
                    value += 13 * (1 << (a * 4));
                    break;
                case 'E':
                case 'e':
                    value += 14 * (1 << (a * 4));
                    break;
                case 'F':
				case 'f':
                    value += 15 * (1 << (a * 4));
                    break;
                default:
                    //cout << "Error, invalid charactare '" << hex[a] << "' in hex number" << endl;
                    break;
            }
        }
    }   
    return value;
}
 
void hextodec(string hex, vector<unsigned char>& rgb)
{
    // since there is no prefix attached to hex, use this code
    string redString = hex.substr(0, 2);
    string greenString = hex.substr(2, 2);
    string blueString = hex.substr(4, 2);

/*
    if the prefix # was attached to hex, use the following code
    string redString = hex.substr(1, 2);
    string greenString = hex.substr(3, 2);
    string blueString = hex.substr(5, 2);
*/

/*
    if the prefix 0x was attached to hex, use the following code
    string redString = hex.substr(2, 2);
    string greenString = hex.substr(4, 2);
    string blueString = hex.substr(6, 2);
*/

    unsigned char red = (unsigned char)(convertFromHex(redString));
    unsigned char green = (unsigned char)(convertFromHex(greenString));
    unsigned char blue = (unsigned char)(convertFromHex(blueString));

    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
}

COLORREF CSkinUtil::HexToRgb(CString val)
{
	val.Replace(L"#", L"");
	string hex = ConvStr::CstrtoString(val);
	return convertFromHex(hex);
}
