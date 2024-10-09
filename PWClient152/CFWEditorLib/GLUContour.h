#ifndef _FWGLUCONTOUR_H_
#define _FWGLUCONTOUR_H_

class CCharContour;
class CVertex;

class CGLUContour  
{
public:
	int m_nVerticesCount;
	double (* m_paGLUVertices)[3];
public:
	bool FindInVertices(double x, double y, int nCount);
	int ComputeVerticesArraySize() const;
	int GetVerticesCount() const { return m_nVerticesCount; }
	int GetVerticesArraySize() const;
	void Destory();
	bool CreateFromCharContour(const CCharContour *pCharContour);
	CGLUContour();
	virtual ~CGLUContour();
};

#endif 