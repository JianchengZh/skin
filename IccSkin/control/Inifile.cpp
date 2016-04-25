#include "stdafx.h"
#include <Afx.h>
#include <AfxColl.h>

#include "IniFile.H"

CIniFile::CIniFile(CString m_FName)
	:m_Name(m_FName)
{

}

CString CIniFile::ReadString(CString m_Sec, CString m_Ident, CString m_Def)
{
	TCHAR Buffer[STRIDT_SIZE];
	GetPrivateProfileString(m_Sec, m_Ident,m_Def, Buffer, STRIDT_SIZE, m_Name);

	//주석이 있는 경우 예외처리
	CString strBuf;
	strBuf.Format(_T("%s"), Buffer);
	int pos = strBuf.Find(_T("//"));
	if( pos >= 0)
	{
		strBuf = strBuf.Left(pos);
		strBuf.TrimRight();
	}
	strBuf.Trim();
	return strBuf;
}

DWORD CIniFile::WriteString(CString m_Sec, CString m_Ident, CString m_Val)
{
	BOOL bRet = WritePrivateProfileString(m_Sec, m_Ident, m_Val, m_Name);

	DWORD dwCode = GetLastError();

	return dwCode;
}

BOOL CIniFile::ReadSections(CStringArray& m_Secs)
{
	LPVOID pvData = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 16385);
	_ASSERTE(NULL != hGlobal);

	if(hGlobal !=NULL)
		pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	m_Secs.RemoveAll();

	if (GetPrivateProfileString(NULL, NULL, NULL, (TCHAR*) pvData, 16384, m_Name))
	{
		TCHAR *P = (TCHAR*) pvData;
		while (*P != 0)
		{
			m_Secs.Add(P);
			P += _tcslen(P) + 1;
		}
	}
	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);
	return m_Secs.GetSize() > 0;
}

BOOL CIniFile::ReadSection(CString m_Sec, CStringArray& m_Secs)
{
	LPVOID pvData = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 16385);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	m_Secs.RemoveAll();

	if (GetPrivateProfileString(m_Sec, NULL, NULL, (TCHAR*) pvData, 16384, m_Name))
	{
		TCHAR *P = (TCHAR*) pvData;
		while (*P != 0)
		{
			m_Secs.Add(P);
			P += _tcslen(P) + 1;
		}
	}
	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);
	return m_Secs.GetSize() > 0;
}
