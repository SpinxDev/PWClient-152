#ifndef _FWGLUPOLYGON_H_
#define _FWGLUPOLYGON_H_

#include <AList2.h>

class CCharOutline;
class CGLUContour;


class CGLUPolygon  
{
	APtrList<CGLUContour *> * m_pListGLUContours;
public:
	const APtrList<CGLUContour *> * GetGLUContourList() const { return m_pListGLUContours; }
	APtrList<CGLUContour *> * GetGLUContourList() { return m_pListGLUContours; }
	bool CreateFromCharOutline(const CCharOutline *pCharOutline);
	void Destory();
	CGLUPolygon();
	virtual ~CGLUPolygon();

};

#endif 