#ifndef IMAGEMAPSTATIC_INCLUDED
#define IMAGEMAPSTATIC_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif


#define M_TOP_LEFT			0
#define M_TOP_CENTER		1
#define M_TOP_RIGHT			2
#define M_MIDDLE_LEFT		3
#define M_MIDDLE_CENTER		4
#define M_MIDDLE_RIGHT		5
#define M_BOTTOM_LEFT		6
#define M_BOTTOM_CENTER		7
#define M_BOTTOM_RIGHT		8


#include "ImageMap.h"

class CImageMap_Static : public CImageMap  
{
public:
	bool m_bInit;
	void SetRect(int nIndex, CRect rect);
	void SetRect(int nIndex, int nX1, int nY1, int nX2, int nY2);
	CImageMap_Static();
	virtual ~CImageMap_Static();
};

#endif 
