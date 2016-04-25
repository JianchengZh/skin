#ifndef IMAGEMAP_INCLUDED
#define IMAGEMAP_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif


#define MAX_AREAS	20

class CImageMap  
{
public:
	CImageMap();

	virtual ~CImageMap();
	CRect OffSet(int Index){return m_ImageMap[Index];}

protected:
	CRect m_ImageMap[MAX_AREAS];
};

#endif
