#ifndef SCROLLBAR_INCLUDE
#define SCROLLBAR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif


#ifdef SB_API_EXPORTS
#ifdef _AFXEXT
#define SB_API __declspec(dllexport)
#else
#define SB_API
#endif
#else
#ifdef _AFXEXT
#define SB_API __declspec(dllimport)
#else
#define SB_API
#endif
#endif

#include "coolscroll.h"
#include "coolsb_detours.h"

SB_API BOOL SB_InitializeApp(void);

SB_API BOOL SB_UninitializeApp(void);

SB_API BOOL InitializeSB(HWND hwnd);

SB_API HRESULT UninitializeSB(HWND hwnd);

SB_API BOOL SB_SetMinThumbSize(HWND hwnd, UINT wBar, UINT size);

SB_API BOOL SB_IsThumbTracking(HWND hwnd);

SB_API BOOL SB_IsCoolScrollEnabled(HWND hwnd);

SB_API BOOL SB_EnableScrollBar(HWND hwnd, int wSBflags, UINT wArrows);

SB_API BOOL SB_GetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi);

SB_API int SB_GetScrollPos(HWND hwnd, int nBar);

SB_API BOOL SB_GetScrollRange(HWND hwnd, int nBar, LPINT lpMinPos, LPINT lpMaxPos);

SB_API int SB_SetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi, BOOL fRedraw);

SB_API int SB_SetScrollPos(HWND hwnd, int nBar, int nPos, BOOL fRedraw);

SB_API int SB_SetScrollRange(HWND hwnd, int nBar, int nMinPos, int nMaxPos, BOOL fRedraw);

SB_API BOOL SB_ShowScrollBar(HWND hwnd, int wBar, BOOL fShow);

SB_API BOOL SB_SetSize(HWND hwnd, int wBar, int nLength, int nWidth);

SB_API BOOL SB_SetStyle(HWND hwnd, int wBar, UINT nStyle);

SB_API BOOL SB_SetThumbAlways(HWND hwnd, int wBar, BOOL fThumbAlways);

#endif