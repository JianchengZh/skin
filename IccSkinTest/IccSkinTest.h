
// IccSkinTest.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CIccSkinTestApp:
// �� Ŭ������ ������ ���ؼ��� IccSkinTest.cpp�� �����Ͻʽÿ�.
//

class CIccSkinTestApp : public CWinApp
{
public:
	CIccSkinTestApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CIccSkinTestApp theApp;