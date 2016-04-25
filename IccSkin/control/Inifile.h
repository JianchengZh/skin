#ifndef INIFILE_H__
#define INIFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define STRIDT_SIZE 2048// Size 문제가 있어 새로 선언함

#ifdef _AFXEXT
class AFX_EXT_CLASS CIniFile
#else
class CIniFile
#endif
{
	CString m_Name;
public:
	CIniFile(CString m_FName);

	void SetFileName(CString strFile){m_Name = strFile;};
	CString ReadString(CString m_Sec, CString m_Ident, CString m_Def);
	DWORD WriteString(CString m_Sec, CString m_Ident, CString m_Val);
	BOOL ReadSections(CStringArray& m_Secs);
	BOOL ReadSection(CString m_Sec, CStringArray& m_Secs);
};

#endif