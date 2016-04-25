// ImageMap_Mask.cpp: implementation of the CImageMap_Mask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageMap_Mask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageMap_Mask::CImageMap_Mask()
:m_bInit(false)
{
}

CImageMap_Mask::~CImageMap_Mask()
{
}

void CImageMap_Mask::SetRect(int nIndex, int nX1, int nY1, int nX2, int nY2)
{
	m_ImageMap[nIndex].SetRect(nX1, nY1, nX2, nY2);
}

void CImageMap_Mask::SetRect(int nIndex, CRect rect)
{
	m_ImageMap[nIndex].SetRect(rect.left, rect.top, rect.right, rect.bottom);
}