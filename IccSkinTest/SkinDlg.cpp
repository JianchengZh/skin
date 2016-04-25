// SkinDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "IccSkinTest.h"
#include "SkinDlg.h"
#include "afxdialogex.h"

// CSkinDlg ��ȭ �����Դϴ�.

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

// CSkinDlg �޽��� ó�����Դϴ�.

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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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