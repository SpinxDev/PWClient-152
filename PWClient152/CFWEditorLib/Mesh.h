#ifndef _CMESH_H_
#define _CMESH_H_

#include "Triangle.h"
#include <AArray.h>
#include <A3DVector.h>
#include <A3DMatrix.h>

class CGLUTess;
class CMyDevice;
class CCharContour;
class CCharOutline;


typedef AArray<A3DVECTOR3, A3DVECTOR3 &> AVertexArray;

class CMesh  
{
public:
	ATriangleArray m_aryTriangles;
	AVertexArray m_aryVertices;
	A3DVECTOR3 m_vecRef;
	A3DMATRIX4 m_WorldMatrix;
	float m_xMin, m_xMax, m_yMin, m_yMax, m_zMin, m_zMax;
public:
	void Clear();
	void MakeAntiClockwise();
	void RepositionToCenter();
	const A3DVECTOR3 & GetRefPoint();
	void TransformVertices(AVertexArray &lstVertices, const A3DMATRIX4 &m);
	void ComputeRefPoint();
	void SetPosition(float x, float y, float z);
#ifdef _FOR_RENDER
	A3DVECTOR3 GetNormalOnCountor(double (*paVertices)[2], int nCount, int nIndex);
#endif
	bool CreateFromString(LPCTSTR tcsString, int nCount, HDC hDC, float fThickness, bool bSimplify = true);
	bool CreateFromChar(int nChar, HDC hDC, float fThickness, bool bReposition = true, bool bSimplify = true);
	bool Create3D(const CCharOutline *pOutline, const CGLUTess *pTess, float fThickness, bool bResposition = true);
	bool CreateAsContrary(const CMesh *pMesh, float fThickness);
	bool CreateFromCharOutline(const CCharOutline *pCharOutline, float fThickness);
	bool CreateFromCharContour(const CCharContour *pCharContour, float fThickness);
	bool CreateFromGLUTess(const CGLUTess *pTess);
	void Append(const CMesh *pMesh);
	void Destory();
	CMesh();
	virtual ~CMesh();
private:
	int FindInVertices(const AVertexArray &aryVertices, const A3DVECTOR3 &val);
	void ShrinkVertices(ATriangleArray &aryTriangles, AVertexArray &aryVertices);

};

#endif 