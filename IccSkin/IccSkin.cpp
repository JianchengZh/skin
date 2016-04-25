// IccSkin.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#ifdef _AFXEXT


#include <afxdllx.h>
#ifdef _MANAGED
#error /clr를 사용하여 컴파일하려면 IccSkin.cpp에 있는 지침을 따르십시오.
// 프로젝트에 /clr를 추가하려면 다음 단계를 따르십시오.
//	1. #include <afxdllx.h> 지시문을 지웁니다.
//	2. /crl를 throw하지 않고 미리 컴파일된 헤더가 비활성화되어 있는
//	   프로젝트에 다음 텍스트를 사용하여 .cpp 파일을 추가합니다.
//			#include <afxwin.h>
//			#include <afxdllx.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE IccSkinDLL = { NULL, NULL };

#ifdef _MANAGED
#pragma managed(push, off)
#endif

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// lpReserved를 사용하는 경우 다음을 제거하십시오.
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE("######Initialize Skin######\n");
		
		// 확장 DLL을 한 번만 초기화합니다.
		if (!AfxInitExtensionModule(IccSkinDLL, hInstance))
			return 0;

		// 이 DLL을 리소스 체인에 삽입합니다.
		// 참고: 이 확장 DLL이 MFC 응용 프로그램이
		//  아닌 ActiveX 컨트롤과 같은 MFC 기본 DLL에
		//  의해 명시적으로 링크되어 있는 경우에는
		//  DllMain에서 이 줄을 제거하고, 제거한 줄은 이 확장 DLL에서
		//  내보낸 별도의 함수에 추가합니다.
		//  그런 다음 이 확장 DLL을 사용하는 기본 DLL은
		//  해당 함수를 명시적으로 호출하여 이 확장 DLL을 추가해야 합니다.
		//  그렇지 않으면 CDynLinkLibrary 개체가
		//  기본 DLL의 리소스 체인에 추가되지 않으므로
		//  심각한 문제가 발생합니다.

		new CDynLinkLibrary(IccSkinDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE("######Finalize Skin######\n");

		// 소멸자가 호출되기 전에 라이브러리를 종료합니다.
		AfxTermExtensionModule(IccSkinDLL);
	}
	return 1;   // 확인
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif