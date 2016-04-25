// SkinDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "IccSkinTest.h"
#include "SkinDlg.h"
#include "afxdialogex.h"

// CSkinDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkinDlg, CDialog)

CSkinDlg::CSkinDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSkinDlg::IDD, pParent)
	,m_strSkinFile("")
{
}

CSkinDlg::~CSkinDlg()
{
}

void CSkinDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSkinDlg, CResizableDialog)

END_MESSAGE_MAP()

// CSkinDlg 메시지 처리기입니다.

BOOL CSkinDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	if(!SetSkin(m_strSkinFile)){
		AfxMessageBox(_T("Skin Error!"), MB_OK);
		OnCancel();
		return FALSE;
	}

	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	int nXDef= (cx-m_rDefRect.Width())/2;
	int nYDef = (cy-m_rDefRect.Height())/2;
	MoveWindow(nXDef, nYDef, m_rDefRect.Width(), m_rDefRect.Height(), true);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

CString CSkinDlg::GetModulePath(HINSTANCE hInst)
{
	TCHAR szCurDir[_MAX_PATH];
	ZeroMemory(szCurDir, _MAX_PATH);
	CString strPath;

	GetModuleFileName(hInst, szCurDir, MAX_PATH);
	strPath.Format(_T("%s"), szCurDir);
	int nlocation = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Mid(0,nlocation+1);
	return strPath;
}