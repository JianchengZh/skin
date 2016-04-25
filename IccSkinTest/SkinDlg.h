#pragma once

#include "ResizableDialog.h"
// CSkinDlg ��ȭ �����Դϴ�.

class CSkinDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CSkinDlg)

public:
	CSkinDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSkinDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SKINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	
	CString m_strSkinFile;

public:	
	virtual BOOL OnInitDialog();
	void SetSkinFile(CString strFile){m_strSkinFile = strFile;};
	CString GetModulePath(HINSTANCE hInst);
	
};
