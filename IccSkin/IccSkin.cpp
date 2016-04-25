// IccSkin.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//

#include "stdafx.h"
#ifdef _AFXEXT


#include <afxdllx.h>
#ifdef _MANAGED
#error /clr�� ����Ͽ� �������Ϸ��� IccSkin.cpp�� �ִ� ��ħ�� �����ʽÿ�.
// ������Ʈ�� /clr�� �߰��Ϸ��� ���� �ܰ踦 �����ʽÿ�.
//	1. #include <afxdllx.h> ���ù��� ����ϴ�.
//	2. /crl�� throw���� �ʰ� �̸� �����ϵ� ����� ��Ȱ��ȭ�Ǿ� �ִ�
//	   ������Ʈ�� ���� �ؽ�Ʈ�� ����Ͽ� .cpp ������ �߰��մϴ�.
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
	// lpReserved�� ����ϴ� ��� ������ �����Ͻʽÿ�.
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE("######Initialize Skin######\n");
		
		// Ȯ�� DLL�� �� ���� �ʱ�ȭ�մϴ�.
		if (!AfxInitExtensionModule(IccSkinDLL, hInstance))
			return 0;

		// �� DLL�� ���ҽ� ü�ο� �����մϴ�.
		// ����: �� Ȯ�� DLL�� MFC ���� ���α׷���
		//  �ƴ� ActiveX ��Ʈ�Ѱ� ���� MFC �⺻ DLL��
		//  ���� ��������� ��ũ�Ǿ� �ִ� ��쿡��
		//  DllMain���� �� ���� �����ϰ�, ������ ���� �� Ȯ�� DLL����
		//  ������ ������ �Լ��� �߰��մϴ�.
		//  �׷� ���� �� Ȯ�� DLL�� ����ϴ� �⺻ DLL��
		//  �ش� �Լ��� ��������� ȣ���Ͽ� �� Ȯ�� DLL�� �߰��ؾ� �մϴ�.
		//  �׷��� ������ CDynLinkLibrary ��ü��
		//  �⺻ DLL�� ���ҽ� ü�ο� �߰����� �����Ƿ�
		//  �ɰ��� ������ �߻��մϴ�.

		new CDynLinkLibrary(IccSkinDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE("######Finalize Skin######\n");

		// �Ҹ��ڰ� ȣ��Ǳ� ���� ���̺귯���� �����մϴ�.
		AfxTermExtensionModule(IccSkinDLL);
	}
	return 1;   // Ȯ��
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif