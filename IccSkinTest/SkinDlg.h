#pragma once

#include "ResizableDialog.h"
// CSkinDlg 대화 상자입니다.

class CSkinDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CSkinDlg)

public:
	CSkinDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSkinDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SKINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
	CString m_strSkinFile;

public:	
	virtual BOOL OnInitDialog();
	void SetSkinFile(CString strFile){m_strSkinFile = strFile;};
	CString GetModulePath(HINSTANCE hInst);
	
};
