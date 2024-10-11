// Mesh.cpp: implementation of the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "Mesh.h"
#include "GLUTess.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"
#include "CharOutline.h"
#include "GLUPolygon.h"
#include <AAssist.h>

#define new A_DEBUG_NEW

static const float dd = 1e-6f;

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	Destory();
}

void CMesh::Destory()
{
	m_aryVertices.RemoveAll();
	m_aryTriangles.RemoveAll();
}

bool CMesh::CreateFromGLUTess(const CGLUTess *pTess)
{
	ASSERT(pTess);
	ASSERT(m_aryTriangles.GetSize() == 0);
	ASSERT(m_aryVertices.GetSize() == 0);
	ASSERT(pTess->GetVertexList()->GetCount() % 3 == 0);

	const AGLUVertexList *pListGLUVertex = pTess->GetVertexList();
	if (pListGLUVertex->GetCount() == 0)
		return true;

	int nIndexVertex = 0;
	ALISTPOSITION pos = pListGLUVertex->GetHeadPosition();
	while (pos)
	{
		int nFoundIndex;
		int nIndex[3];
		for (int i = 0; i < 3; i++)
		{
			GLdouble *pv = pListGLUVertex->GetNext(pos);
			A3DVECTOR3 vertex = A3DVECTOR3((float)pv[0], (float)pv[1], (float)pv[2]);
#ifdef _FOR_RENDER
			vertex.NormalX = 0; vertex.NormalY = 0; vertex.NormalZ = 1;
#endif
			if ((nFoundIndex = FindInVertices(m_aryVertices, vertex)) != -1)
			{
				nIndex[i] = nFoundIndex;
			}
			else
			{
				nIndex[i] = nIndexVertex++;
				m_aryVertices.Add(vertex);
			}
		}
		CTriangle triangle = CTriangle(nIndex[0], nIndex[2], nIndex[1]);
		m_aryTriangles.Add(triangle);
	}

	return true;
}

int CMesh::FindInVertices(const AVertexArray &aryVertices, const A3DVECTOR3 &val)
{
	int nSize = aryVertices.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		if (aryVertices[i] == val)
			return i;
	}
	return -1;
}


bool CMesh::CreateFromCharContour(const CCharContour *pCharContour, float fThickness)
{
	// generate z-direction mesh for char
	const CCharVerticesBuffer* pBuffer = pCharContour->GetCharVerticesBuffer();
	int nCount = pBuffer->GetVerticesCount();
	if (nCount < 3)
		return true;
	double (* paVertices)[2] = pBuffer->m_paVertices;

	int i;
	int nIndex = 0;
	for (i = 0; i < nCount; i++)
	{
		A3DVECTOR3 v1((float)paVertices[i][0], (float)paVertices[i][1], 0);
		A3DVECTOR3 v2((float)paVertices[i][0], (float)paVertices[i][1], -1 * fThickness);
		
		// this is only for 3d render
#ifdef _FOR_RENDER
		A3DVECTOR3 nv = GetNormalOnCountor(paVertices, nCount, i);
		v1.NormalX = nv.x; v1.NormalY = nv.y; v1.NormalZ = nv.z;
		v2.NormalX = nv.x; v2.NormalY = nv.y; v2.NormalZ = nv.z;
#endif
		m_aryVertices.Add(v1);
		m_aryVertices.Add(v2);

		if (i < nCount - 1)
		{
			CTriangle t1(nIndex, nIndex + 2, nIndex + 1);
			CTriangle t2(nIndex + 2, nIndex + 3, nIndex + 1);
			m_aryTriangles.Add(t1);
			m_aryTriangles.Add(t2);
		}
		else
		{
			CTriangle t1(nIndex, 0, nIndex + 1);
			CTriangle t2(0, 1, nIndex + 1);
			m_aryTriangles.Add(t1);
			m_aryTriangles.Add(t2);
		}

		nIndex += 2;
	}

	return true;
}

bool CMesh::CreateFromCharOutline(const CCharOutline *pCharOutline, float fThickness)
{
	const APtrList<CCharContour *>* pListContours = pCharOutline->GetCharContoursList();
	if (pListContours->GetCount() == 0)
		return true;

	ALISTPOSITION pos = pListContours->GetHeadPosition();
	while (pos)
	{
		CCharContour *pCountour = pListContours->GetNext(pos);
		CMesh mesh;
		mesh.CreateFromCharContour(pCountour, fThickness);
		Append(&mesh);
	}
	return true;
}

void CMesh::Append(const CMesh *pMesh)
{
	int nOldCount = m_aryVertices.GetSize();
	
	m_aryVertices.Append(pMesh->m_aryVertices);

	ATriangleArray aryTriangleTemp;
	aryTriangleTemp.Append(pMesh->m_aryTriangles);

	for (int i = 0; i < (int)pMesh->m_aryTriangles.GetSize(); i++)
	{
		aryTriangleTemp[i].Index0 += nOldCount;
		aryTriangleTemp[i].Index1 += nOldCount;
		aryTriangleTemp[i].Index2 += nOldCount;
	}
	m_aryTriangles.Append(aryTriangleTemp);
}

bool CMesh::Create3D(const CCharOutline *pOutline, const CGLUTess *pTess, float fThickness, bool bResposition)
{
	if (!CreateFromCharOutline(pOutline, fThickness))
		return false;

	CMesh mesh1;
	mesh1.CreateFromGLUTess(pTess);
	Append(&mesh1);

	CMesh mesh2;
	if (!mesh2.CreateAsContrary(&mesh1, fThickness))
		return false;
	Append(&mesh2);

	//ShrinkVertices(m_aryTriangles, m_aryVertices);
	ComputeRefPoint();
	
	if (bResposition)
	{
		RepositionToCenter();
	}

	return true;
}

bool CMesh::CreateAsContrary(const CMesh *pMesh, float fThickness)
{
	m_aryVertices.RemoveAll();
	m_aryVertices.Append(pMesh->m_aryVertices);
	m_aryTriangles.RemoveAll();
	m_aryTriangles.Append(pMesh->m_aryTriangles);

	int i;
	int nCountVertices = m_aryVertices.GetSize();
	int nCountTriangles = m_aryTriangles.GetSize();
	for (i = 0; i < nCountTriangles; i++)
	{
		a_Swap(m_aryTriangles[i].Index1, m_aryTriangles[i].Index2);
	}
	for (i = 0; i < nCountVertices; i++)
	{
		A3DVECTOR3 &vertex = m_aryVertices[i];
		m_aryVertices[i].z = -1 * fThickness;
#ifdef _FOR_RENDER
		m_aryVertices[i].NormalZ = -1;
#endif
	}

	return true;
}

void CMesh::ShrinkVertices(ATriangleArray& aryTriangles, AVertexArray& aryVertices)
{
	AVertexArray destVertices;
	//destVertices.SetSize(m_aryVertices.GetSize(), 1);

	int nCountTriangle = m_aryTriangles.GetSize();

	int nIndexTriangle, i, nCurrentVertexIndex = 0;
	for (nIndexTriangle = 0; nIndexTriangle < nCountTriangle; nIndexTriangle++)
	{
		CTriangle& triangle = aryTriangles[nIndexTriangle];
		for (i = 0; i < 3; i++)
		{
			A3DVECTOR3& vertex = aryVertices[triangle.Index[i]];
			int nFoundIndex;
			if ((nFoundIndex = FindInVertices(destVertices, vertex)) == -1)
			{
				destVertices.Add(vertex);
				triangle.Index[i] = nCurrentVertexIndex++;
			}
			else
			{
				triangle.Index[i] = nFoundIndex;
			}
		}
	}
	aryVertices.RemoveAll(false);
	for (i = 0; i < destVertices.GetSize(); ++i)
		aryVertices.Add(destVertices[i]);
}


bool CMesh::CreateFromChar(int nChar, HDC hDC, float fThickness, bool bReposition, bool bSimplify)
{
	CCharOutline charOutline;
	charOutline.CreateFromChar(nChar, hDC);
	if (bSimplify)
		charOutline.Simplify();

	CGLUPolygon gluPolygon;
	gluPolygon.CreateFromCharOutline(&charOutline);
	
	CGLUTess gluTess;
	gluTess.CreateFromGLUPolygon(&gluPolygon);

	return Create3D(&charOutline, &gluTess, fThickness, bReposition);
}

bool CMesh::CreateFromString(LPCTSTR tcsString, int nCount, HDC hDC, float fThickness, bool bSimplify)
{
	bool bSucceed = true;
	int nXOffset = 0;
	int nChar;
	for (int i = 0; i < nCount; i++)
	{
#ifndef UNICODE
		if (IsDBCSLeadByte(tcsString[i]))
		{
			BYTE *p = (BYTE *)&nChar;
			*p = tcsString[i + 1];
			*(p + 1) = tcsString[i];
			i++;
		}
		else
		{
			nChar = tcsString[i];
		}
#else
		nChar = tcsString[i];
#endif

		CCharOutline charOutline;
		if (!charOutline.CreateFromChar(nChar, hDC))
		{
			bSucceed = false;
			break;
		}
		if (bSimplify)
			charOutline.Simplify();

		CGLUPolygon gluPolygon;
		if (!gluPolygon.CreateFromCharOutline(&charOutline))
		{
			bSucceed = false;
			break;
		}
		CGLUTess gluTess;
		if (!gluTess.CreateFromGLUPolygon(&gluPolygon))
		{
			bSucceed = false;
			break;
		}
		
		CMesh mesh;
		if (fThickness)
		{
			if (!mesh.Create3D(&charOutline, &gluTess, fThickness, false))
			{
				bSucceed = false;
				break;
			}
		}
		else
		{
			if (!mesh.CreateFromGLUTess(&gluTess))
			{
				bSucceed = false;
				break;
			}
		}

		A3DMATRIX4 m;
		m.Translate((float)nXOffset, 0.0f, 0.0f);
		mesh.TransformVertices(mesh.m_aryVertices, m);

		Append(&mesh);

		nXOffset += charOutline.GetXOffset();
	}

	if (!bSucceed)
		Destory();

	RepositionToCenter();

	return bSucceed;
}

#ifdef _FOR_RENDER

A3DVECTOR3 CMesh::GetNormalOnCountor(double (*paVertices)[2], int nCount, int nIndex)
{
	int nIndexPre, nIndexNext;
	nIndexPre = nIndex - 1;
	nIndexNext = nIndex + 1;
	if (nIndexPre < 0)
		nIndexPre = nCount -1;
	if (nIndexNext > nCount - 1)
		nIndexNext = 0;
	
	A3DVECTOR3 v0(paVertices[nIndexPre][0], paVertices[nIndexPre][1], 0);
	A3DVECTOR3 v1(paVertices[nIndex][0], paVertices[nIndex][1], 0);
	A3DVECTOR3 v2(paVertices[nIndexNext][0], paVertices[nIndexNext][1], 0);
	
	bool bConvex = true;
	A3DVECTOR3 v01 = v1 - v0;
	A3DVECTOR3 v12 = v2 - v1;
	bConvex = ((v01 * v12).z < 0);
	
	A3DVECTOR3 v10 = v0 - v1;
	v10.Normalize();
	//	A3DVECTOR3 v12 = v2 - v1;
	v12.Normalize();
	A3DVECTOR3 nv = (v10 + v12);
	float len = nv.GetLength();
	if (len < dd)
	{
		A3DMATRIX4 m;
		m = A3DMATRIX4(A3D_PI / 2, AXIS_Z);
		nv = v12;
		nv.Transform(m);
		nv.Normalize();
	}
	else
	{
		if (bConvex)
			nv = nv * -1;
		nv.Normalize();
	}
	
	return nv;
}
#endif

void CMesh::ComputeRefPoint()
{
	int nCountVertices = m_aryVertices.GetSize();
	float xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = ymin = zmin = 1000000;
	xmax = ymax = zmax = -1000000;
	for (int i = 0; i < nCountVertices; i++)
	{
		A3DVECTOR3 &vertex = m_aryVertices[i];
		if (vertex.x < xmin) xmin = vertex.x;
		if (vertex.x > xmax) xmax = vertex.x;
		if (vertex.y < ymin) ymin = vertex.y;
		if (vertex.y > ymax) ymax = vertex.y;
		if (vertex.z < zmin) zmin = vertex.z;
		if (vertex.z > zmax) zmax = vertex.z;
	}
	
	m_xMin = xmin;
	m_xMax = xmax;
	m_yMin = ymin;
	m_yMax = ymax;
	m_zMin = zmin;
	m_zMax = zmax;
	
	
	m_vecRef.x = (xmin + xmax) / 2;
	m_vecRef.y = (ymin + ymax) / 2;
	m_vecRef.z = (zmin + zmax) / 2;
}

void CMesh::SetPosition(float x, float y, float z)
{
	m_WorldMatrix.Translate(
		x - GetRefPoint().x,
		y - GetRefPoint().y,
		z - GetRefPoint().z);
}

void CMesh::TransformVertices(AVertexArray &aryVertices, const A3DMATRIX4 &m)
{
	int nSize = aryVertices.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		A3DVECTOR3 &vertex = aryVertices[i];
		vertex = vertex * m;
	}
}


const A3DVECTOR3 & CMesh::GetRefPoint()
{
	return m_vecRef;
}

void CMesh::RepositionToCenter()
{
	ComputeRefPoint();
	SetPosition(0.0f, 0.0f, 0.0f);
	TransformVertices(m_aryVertices, m_WorldMatrix);
	ComputeRefPoint();
}

void CMesh::MakeAntiClockwise()
{
	for (int nIndexTriangle = 0; nIndexTriangle < m_aryTriangles.GetSize(); nIndexTriangle++)
	{
		CTriangle &triangle = m_aryTriangles[nIndexTriangle];
		A3DVECTOR3 &v1 = m_aryVertices[triangle.Index0];
		A3DVECTOR3 &v2 = m_aryVertices[triangle.Index1];
		A3DVECTOR3 &v3 = m_aryVertices[triangle.Index2];
		if (CrossProduct(v2 - v1, v3 - v2).z < 0)
		{
			a_Swap(triangle.Index1, triangle.Index2);
		}
	}
}

void CMesh::Clear()
{
	m_aryVertices.RemoveAll();
	m_aryTriangles.RemoveAll();
}
