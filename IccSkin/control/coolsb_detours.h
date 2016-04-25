#ifndef COOLSB_DETOURS_INCLUDED
#define COOLSB_DETOURS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL WINAPI CoolSB_InitializeApp(void);
BOOL WINAPI CoolSB_UninitializeApp(void);

#ifdef __cplusplus
}
#endif

#endif