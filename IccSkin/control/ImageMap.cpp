// ImageMap.cpp: implementation of the CImageMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageMap::CImageMap()
{
	for(int i = 0; i < MAX_AREAS; i++)
		m_ImageMap[i].SetRect(0,0,0,0);
}

CImageMap::~CImageMap()
{
}