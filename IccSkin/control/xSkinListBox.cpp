// xSkinListBox.cpp : implementation file
//

#include "stdafx.h"
#include "xSkinListBox.h"

#include "Define.h"

// CxSkinListBox

IMPLEMENT_DYNAMIC(CxSkinListBox, CListBox)
	CxSkinListBox::CxSkinListBox()
	:m_clrBk(COLOR_WHITE)
{
}

CxSkinListBox::~CxSkinListBox()
{
}

BEGIN_MESSAGE_MAP(CxSkinListBox, CListBox)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CxSkinListBox message handlers

void CxSkinListBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CListBox::OnPaint() for painting messages

	RECT rect;
	GetClientRect(&rect);

	CBrush brush;
	brush.CreateSolidBrush(m_clrBk);	//Combo ListBox BG
	dc.FillRect(&rect,&brush);
	brush.DeleteObject();
}

void CxSkinListBox::SetBackGroundColor(COLORREF clrBk)
{
	m_clrBk = clrBk;
}