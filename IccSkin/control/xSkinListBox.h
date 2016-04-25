#ifndef XSKINLISTBOX_H
#define XSKINLISTBOX_H

#if _MSC_VER > 1000
#pragma once
#endif


// CxSkinListBox

class CxSkinListBox : public CListBox
{
	DECLARE_DYNAMIC(CxSkinListBox)

public:
	CxSkinListBox();
	virtual ~CxSkinListBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

private:
	COLORREF	m_clrBk;

public:
	void SetBackGroundColor(COLORREF clrBk);

};

#endif