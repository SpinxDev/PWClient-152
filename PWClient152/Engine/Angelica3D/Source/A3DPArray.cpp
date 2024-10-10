/*
 * FILE: A3DPArray.cpp
 *
 * DESCRIPTION: Object Based Particle System for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPArray.h"
#include "A3DPI.h"
#include "A3DGraphicsFX.h"
#include "A3DGFXMan.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DMesh.h"
#include "A3DFrame.h"
#include "A3DModel.h"

A3DPArray::A3DPArray()
{
	m_dwClassID		= A3D_CID_PARRAY;
	m_pA3DDevice	= NULL;
	m_pObjectModel	= NULL;
	m_fMinFragArea	= 3.0f;
	m_fMaxFragArea	= 4.5f;
	m_iCurTexID		= 0;
	m_iNumMesh		= 0;
	m_iNumTexture	= 0;

	m_aFragVerts	= NULL;
	m_iNumFragVert	= 0;
	m_iCurFragVert	= 0;
	m_aFragIndices	= NULL;
	m_iNumFragIdx	= 0;
	m_iCurFragIdx	= 0;

	//	Initialize fragment buffer
	memset(&m_FragBuf, 0, sizeof (m_FragBuf));
	m_FragBuf.iMaxNumTri = 8;

	m_vGravity		= 9.8f;
}

A3DPArray::~A3DPArray()
{
}

// Init object fragments from an A3DModel, this function will calculate the needed fragments from the model;;
bool A3DPArray::CreateObjectFragment(A3DModel* pObjectModel)
{
	m_pObjectModel	= pObjectModel;
	m_iNumMesh		= 0;
	m_iNumTexture	= 0;

	int	iNumFace	= pObjectModel->GetIndexCount() / 3;
	m_iNumFragVert	= iNumFace * 8;
	m_iCurFragVert	= 0;
	
	if (m_iNumFragVert > 2048)
		m_iNumFragVert = 2048;

	m_aFragVerts = (A3DLVERTEX*)malloc(m_iNumFragVert * sizeof (A3DLVERTEX));
	if (!m_aFragVerts)
	{
		g_A3DErrLog.Log("A3DPArray::InitObjectFragment Not enough memory");
		return false;
	}

	m_iNumFragIdx	= m_iNumFragVert * 4;
	m_iCurFragIdx	= 0;
	m_aFragIndices	= (WORD*)malloc(m_iNumFragIdx * sizeof (WORD));
	if (!m_aFragIndices)
	{
		g_A3DErrLog.Log("A3DPArray::InitObjectFragment Not enough memory");
		return false;
	}

	InitMeshInfo(pObjectModel);

	//	Initialize base particle system
	if (!A3DParticleSystem::CreateObjectFragment(m_iNumTexture, m_aTextures))
	{
		g_A3DErrLog.Log("A3DPArray::InitObjectFragment call parent CreateObjectFragment fail!");
		return false;
	}

	return true;
}

bool A3DPArray::Release()
{
	if (m_aFragVerts)
	{
		free(m_aFragVerts);
		m_aFragVerts	= NULL;
		m_iNumFragVert	= 0;
		m_iCurFragVert	= 0;
	}

	if (m_aFragIndices)
	{
		free(m_aFragIndices);
		m_aFragIndices	= NULL;
		m_iNumFragIdx	= 0;
		m_iCurFragIdx	= 0;
	}

	return A3DParticleSystem::Release();
}

//	Initialize mesh's information
void A3DPArray::InitMeshInfo(A3DModel* pModel)
{
	if (!pModel)
		return;

	pModel->UpdateToFrame(-1);

	//A3DModel* pChildModel;
	ALISTELEMENT* pElem = pModel->GetChildFrameList()->GetFirst();

	while (pElem != pModel->GetChildFrameList()->GetTail())
	{
		AddFrameInfo((A3DFrame*)pElem->pData);
		pElem = pElem->pNext;
	}
	
	//	Handle child models
	/*
	pElem = pModel->GetChildModelList()->GetFirst();

	while (pElem != pModel->GetChildModelList()->GetTail())
	{
		pChildModel = (A3DModel*)pElem->pData;
		InitMeshInfo(pChildModel);

		pElem = pElem->pNext;
	}*/
}

/*	Add all meshes' information in specified frame.

	pBaseFrame: valid frame object
*/
void A3DPArray::AddFrameInfo(A3DFrame* pBaseFrame)
{
	A3DFrame* pFrame = pBaseFrame->GetFirstChildFrame();
	while (pFrame)
	{
		AddFrameInfo(pFrame);

		//	Next child frame
		pFrame = pBaseFrame->GetNextChildFrame();
	}

	A3DMesh* pMesh = pBaseFrame->GetFirstMesh();
	while (pMesh)
	{
		AddMeshInfo(pBaseFrame, pMesh, pMesh->GetTexture());
		
		//	Next mesh
		pMesh = pBaseFrame->GetNextMesh();
	}
}

/*	Add a mesh's information.

	Return true for success, otherwise return false.

	pFrame: frame in which pMesh lays.
	pMesh: address of mesh
	pTexture: mesh's texture.
*/
bool A3DPArray::AddMeshInfo(A3DFrame* pFrame, A3DMesh* pMesh, A3DTexture* pTexture)
{
	if (m_iNumMesh >= 64 || m_iNumTexture >= 64)
	{
		g_A3DErrLog.Log("A3DPArray::AddMeshInfo(), Too much mesh or texture");
		return false;
	}

	int i, iIndex = -1;

	for (i=0; i < m_iNumTexture; i++)
	{
		if (m_aTextures[i] == pTexture)
			iIndex = i;
	}

	if (iIndex == -1)
	{
		iIndex = m_iNumTexture;
		m_aTextures[m_iNumTexture++] = pTexture;
	}

	m_aMeshes[m_iNumMesh].pFrame	= pFrame;
	m_aMeshes[m_iNumMesh].pMesh		= pMesh;
	m_aMeshes[m_iNumMesh].idTexture	= iIndex;
	
	m_iNumMesh++;

	return true;
}

//	Split model
void A3DPArray::SplitModel()
{
	if (!m_pObjectModel)
		return;

	m_pObjectModel->UpdateToFrame(-1);

	//	Clear fragment buffer
	m_iCurFragVert	= 0;
	m_iCurFragIdx	= 0;
	m_iNumGFXs		= 0;

	//	Get current transfrom matrix of model
	m_pObjectModel->UpdateModelOBB();
	m_vObjCenter = m_pObjectModel->GetModelAABB().Center;

	int i, j, iNumVert, iNumIndex, iCurFrame;
	WORD* aIndices;
	A3DMesh* pMesh;
	A3DVERTEX* aSrcVerts;
	A3DLVERTEX* aVerts;
	A3DMATRIX4 mat;

	for (i=0; i < m_iNumMesh; i++)
	{
		m_iCurTexID	= m_aMeshes[i].idTexture;
		pMesh		= m_aMeshes[i].pMesh;
		mat			= m_aMeshes[i].pFrame->GetAbsoluteTM();

		iCurFrame	= pMesh->GetFrame();
		aSrcVerts	= pMesh->GetVerts(iCurFrame);
		aIndices	= pMesh->GetIndices();
		iNumVert	= pMesh->GetVertCount();
		iNumIndex	= pMesh->GetIndexCount();

		//	Translate all vertices into world coordinates
		if (!(aVerts = (A3DLVERTEX*)malloc(iNumVert * sizeof (A3DLVERTEX))))
		{
			g_A3DErrLog.Log("A3DPArray::SplitModel(), Not enough memory");
			return;
		}

		for (j=0; j < iNumVert; j++)
		{
			aVerts[j].x  = mat._11*aSrcVerts[j].x + mat._21*aSrcVerts[j].y + mat._31*aSrcVerts[j].z + mat._41;
			aVerts[j].y  = mat._12*aSrcVerts[j].x + mat._22*aSrcVerts[j].y + mat._32*aSrcVerts[j].z + mat._42;
			aVerts[j].z  = mat._13*aSrcVerts[j].x + mat._23*aSrcVerts[j].y + mat._33*aSrcVerts[j].z + mat._43;
			aVerts[j].tu = aSrcVerts[j].tu;
			aVerts[j].tv = aSrcVerts[j].tv;
			aVerts[j].diffuse  = 0xffffffff;
			aVerts[j].specular = 0xff000000;
		}

		bool bRet = SplitMesh(aVerts, iNumVert, aIndices, iNumIndex);

		free(aVerts);

		if (!bRet)
			break;	
	}
}

/*	Split a mesh.
	
	Return true for success, otherwise return false

	aVerts: vertex array.
	iNumVert: number of vertex
	aIndices: index array.
	iNumIndex: number of index
*/
bool A3DPArray::SplitMesh(A3DLVERTEX* aVerts, int iNumVert, WORD* aIndices, int iNumIndex)
{
	int i;
	A3DVECTOR3 vEdge1, vEdge2, vNormal, vTemp;
	A3DLVERTEX Tri[3];
	float fArea;

	InitFragBuffer(aVerts);

	for (i=0; i < iNumIndex; i+=3)
	{
		Tri[0] = aVerts[aIndices[i]];
		Tri[1] = aVerts[aIndices[i+1]];
		Tri[2] = aVerts[aIndices[i+2]];

		vEdge1.x = Tri[1].x - Tri[0].x;
		vEdge1.y = Tri[1].y - Tri[0].y;
		vEdge1.z = Tri[1].z - Tri[0].z;

		vEdge2.x = Tri[2].x - Tri[0].x;
		vEdge2.y = Tri[2].y - Tri[0].y;
		vEdge2.z = Tri[2].z - Tri[0].z;

		vTemp	= CrossProduct(vEdge1, vEdge2);
		fArea	= Normalize(vTemp, vNormal);

		if (fArea < m_fMinFragArea && m_FragBuf.iMaxNumTri)
		{
			if (!PushTriangleIntoFragBuffer(&aIndices[i], vNormal, fArea))
				return false;
		}
		else if (!SplitTriangle(Tri, vNormal, fArea))
			return false;
	}

	return FlushFragBuffer();
}

//	Initialize fragment buffer
void A3DPArray::InitFragBuffer(A3DLVERTEX* aVerts)
{
	m_FragBuf.aVerts		= aVerts;
	m_FragBuf.iNumTriangle	= 0;
	m_FragBuf.iNumIdx		= 0;
	m_FragBuf.fArea			= 0.0f;
}

/*	Push triangle into fragment buffer.

	Return true for success, otherwise return false

	aIndices: array contain triangle's 3 indices.
	vNormal: triangle's normal
	fArea: triangle's area
*/
bool A3DPArray::PushTriangleIntoFragBuffer(WORD* aIndices, A3DVECTOR3 vNormal, float fArea)
{
	if (m_FragBuf.iNumTriangle >= m_FragBuf.iMaxNumTri)
	{
		if (!FlushFragBuffer())
			return false;
	}

	WORD a = aIndices[0];
	WORD b = aIndices[1];
	WORD c = aIndices[2];

	int i, aFlags[3] = {0, 0, 0};

	//	Check whether this triangle has common vertices with exist triangles
	for (i=0; i < m_FragBuf.iNumIdx; i++)
	{
		if (m_FragBuf.aIndices[i] == a)
			aFlags[0] = 1;
		else if (m_FragBuf.aIndices[i] == b)
			aFlags[1] = 1;
		else if (m_FragBuf.aIndices[i] == c)
			aFlags[2] = 1;
	}

	bool bNeedFlush = true;

	//	2 or more common vertices exist ?
	if (aFlags[0])
	{
		if (aFlags[1] || aFlags[2])
			bNeedFlush = false;
	}
	else if (aFlags[1] && aFlags[2])
		bNeedFlush = false;

	//	No or only one common vertex exist, then flush buffer
	if (bNeedFlush && m_FragBuf.iNumIdx)
	{
		if (!FlushFragBuffer())
			return false;
	}

	//	Use the first triangle's normal as whole fragment's normal
	if (!m_FragBuf.iNumIdx)
		m_FragBuf.vNormal = vNormal;

	//	Add this triangle into fragment buffer
	m_FragBuf.aIndices[m_FragBuf.iNumIdx++] = a;
	m_FragBuf.aIndices[m_FragBuf.iNumIdx++] = b;
	m_FragBuf.aIndices[m_FragBuf.iNumIdx++] = c;

	m_FragBuf.iNumTriangle++;
	m_FragBuf.fArea += fArea;

	return true;
}

/*	Split a triangle.

	Return true for success, otherwise return false

	aVerts: vertices of triangle.
	vNormal: triangle's normal.
	fAreaVal: pre-calculated area of this triangle, negative means this value
			  is invalid.
*/
bool A3DPArray::SplitTriangle(A3DLVERTEX* aVerts, A3DVECTOR3 vNormal, float fAreaVal)
{
	A3DVECTOR3 d1, d2;
	
	d1.x = aVerts[1].x - aVerts[0].x;
	d1.y = aVerts[1].y - aVerts[0].y;
	d1.z = aVerts[1].z - aVerts[0].z;

	d2.x = aVerts[2].x - aVerts[0].x;
	d2.y = aVerts[2].y - aVerts[0].y;
	d2.z = aVerts[2].z - aVerts[0].z;

	float fArea = fAreaVal >= 0.0f ? fAreaVal : a3d_Magnitude(CrossProduct(d1, d2)) * 0.5f;
	if (fArea < m_fMinFragArea)
	{
		//	Discard this fragment ?
		if (RandFloat(0.5f, 0.5f) < 0.4f)
			return true;

		return EmitOneFragment(aVerts, 3, vNormal);
	}

	if (fArea < m_fMaxFragArea)
	{
		if (RandFloat(0.5f, 0.5f) < 0.5f)
			return EmitOneFragment(aVerts, 3, vNormal);
	}

	//	Split this triangle to a triangle and a quadrangle
	float fFactor1 = RandFloat(0.5f, 0.3f);
	float fFactor2 = 1.0f - fFactor1;
	float fNewArea = fArea * fFactor1 * fFactor2;

	A3DLVERTEX v1, v2;

	v1.x	= aVerts[0].x + d1.x * fFactor1;
	v1.y	= aVerts[0].y + d1.y * fFactor1;
	v1.z	= aVerts[0].z + d1.z * fFactor1;
	v1.tu	= aVerts[0].tu + (aVerts[1].tu - aVerts[0].tu) * fFactor1;
	v1.tv	= aVerts[0].tv + (aVerts[1].tv - aVerts[0].tv) * fFactor1;
	v1.diffuse	= aVerts[0].diffuse;
	v1.specular	= aVerts[0].specular;

	v2.x	= aVerts[0].x + d2.x * fFactor2;
	v2.y	= aVerts[0].y + d2.y * fFactor2;
	v2.z	= aVerts[0].z + d2.z * fFactor2;
	v2.tu	= aVerts[0].tu + (aVerts[2].tu - aVerts[0].tu) * fFactor2;
	v2.tv	= aVerts[0].tv + (aVerts[2].tv - aVerts[0].tv) * fFactor2;
	v2.diffuse	= aVerts[0].diffuse;
	v2.specular	= aVerts[0].specular;

	A3DLVERTEX Poly[4];
	Poly[0]	= aVerts[0];
	Poly[1]	= v1;
	Poly[2]	= v2;

	if (!SplitTriangle(Poly, vNormal, fNewArea))
		return false;

	Poly[0]	= v1;
	Poly[1] = aVerts[1];
	Poly[2] = aVerts[2];
	Poly[3] = v2;

	return SplitQuadrangle(Poly, vNormal, fArea - fNewArea);
}

/*	Split a quadrangle.

	Return true for success, otherwise return false

	aVerts: vertices of quadrangle.
	vNormal: quadrangle's normal.
	fAreaVal: pre-calculated area of this triangle, negative means this value
			  is invalid.
*/
bool A3DPArray::SplitQuadrangle(A3DLVERTEX* aVerts, A3DVECTOR3 vNormal, float fAreaVal)
{
	float fArea, fArea1, fArea2;

	if (fAreaVal < 0.0f)
	{
		A3DVECTOR3 d1, d2, d3;
		
		d1.x = aVerts[1].x - aVerts[0].x;
		d1.y = aVerts[1].y - aVerts[0].y;
		d1.z = aVerts[1].z - aVerts[0].z;
		
		d2.x = aVerts[2].x - aVerts[0].x;
		d2.y = aVerts[2].y - aVerts[0].y;
		d2.z = aVerts[2].z - aVerts[0].z;
		
		d3.x = aVerts[3].x - aVerts[0].x;
		d3.y = aVerts[3].y - aVerts[0].y;
		d3.z = aVerts[3].z - aVerts[0].z;
		
		fArea1	= a3d_Magnitude(CrossProduct(d1, d2)) * 0.5f;
		fArea2	= a3d_Magnitude(CrossProduct(d2, d3)) * 0.5f;
		fArea	= fArea1 + fArea2;
	}
	else
		fArea = fAreaVal;

	if (fArea < m_fMinFragArea)
	{
		//	Discard this fragment ?
		if (RandFloat(0.5f, 0.5f) < 0.4f)
			return true;

		return EmitOneFragment(aVerts, 4, vNormal);
	}

	if (fArea < m_fMaxFragArea)
	{
		if (RandFloat(0.5f, 0.5f) < 0.6f)
			return EmitOneFragment(aVerts, 4, vNormal);
	}

	//	Split this quadrangle to two triangles
	A3DLVERTEX Tri[3];
	Tri[0] = aVerts[0];
	Tri[1] = aVerts[1];
	Tri[2] = aVerts[2];
	fArea = fAreaVal < 0.0f ? fArea1 : -1.0f;

	if (!SplitTriangle(Tri, vNormal, fArea))
		return false;

	Tri[0] = aVerts[0];
	Tri[1] = aVerts[2];
	Tri[2] = aVerts[3];
	fArea = fAreaVal < 0.0f ? fArea2 : -1.0f;

	return SplitTriangle(Tri, vNormal, fArea);
}

/*	Emit a fragment. Fragment is a triangle or quadrangle.

	Return true for success, otherwise return false

	aVerts: fragment's vertices.
	iNumVert: number of vertex.
	vNormal: fragment's normal.
*/
bool A3DPArray::EmitOneFragment(A3DLVERTEX* aVerts, int iNumVert, A3DVECTOR3 vNormal)
{
	OBJECT_FRAGMENT_PARTICLE Particle;
	COMMON_PARTICLE* pCommon;
	
	memset(&Particle, 0, sizeof (OBJECT_FRAGMENT_PARTICLE));
	pCommon = &Particle.common;

	A3DLVERTEX* pVert = NULL;
	A3DVECTOR3 vPos;
	WORD* pIndices;
	int i, iNumIndex;
	float u, v, aTemp[3];

	if (iNumVert == 3)	//	Is a triangle
	{
		iNumVert  = 4;
		iNumIndex = 12;

		if (m_iCurFragVert + iNumVert > m_iNumFragVert ||
			m_iCurFragIdx + iNumIndex > m_iNumFragIdx)
		{
			g_A3DErrLog.Log("A3DPArray::EmitOneFragment() buffer overflow!");
			return false;
		}

		pVert = &m_aFragVerts[m_iCurFragVert];

		//	The first 3 vertices
		memcpy(pVert, aVerts, 3 * sizeof (A3DLVERTEX));

		float fTemp = 1.0f / 3.0f;
		aTemp[0]	= aVerts[0].x + aVerts[1].x + aVerts[2].x;
		aTemp[1]	= aVerts[0].y + aVerts[1].y + aVerts[2].y;
		aTemp[2]	= aVerts[0].z + aVerts[1].z + aVerts[2].z;
		vPos.x		= aTemp[0] * fTemp;
		vPos.y		= aTemp[1] * fTemp;
		vPos.z		= aTemp[2] * fTemp;
		u	= (aVerts[0].tu + aVerts[1].tu + aVerts[2].tu) * fTemp;
		v	= (aVerts[0].tv + aVerts[1].tv + aVerts[2].tv) * fTemp;

		//	The vertex make fragment's thickness
		pVert[3].x  = vPos.x - vNormal.x * m_vFragmentThickness;
		pVert[3].y  = vPos.y - vNormal.y * m_vFragmentThickness;
		pVert[3].z  = vPos.z - vNormal.z * m_vFragmentThickness;
		pVert[3].tu	= u;
		pVert[3].tv = v;
		pVert[3].diffuse  = aVerts[0].diffuse;
		pVert[3].specular = aVerts[0].specular;

		vPos.x = (aTemp[0] + pVert[3].x) * 0.25f;
		vPos.y = (aTemp[1] + pVert[3].y) * 0.25f;
		vPos.z = (aTemp[2] + pVert[3].z) * 0.25f;

		//	All particle use relative position to vPos, so translate them
		for (i=0; i < iNumVert; i++)
		{
			pVert[i].x -= vPos.x;
			pVert[i].y -= vPos.y;
			pVert[i].z -= vPos.z;
		}

		m_iCurFragVert += 4;

		//	Build indices
		pIndices	= &m_aFragIndices[m_iCurFragIdx];
		pIndices[0] = 0;	pIndices[1] = 1;	pIndices[2] = 2;
		pIndices[3] = 1;	pIndices[4] = 0;	pIndices[5] = 3;
		pIndices[6] = 2;	pIndices[7] = 1;	pIndices[8] = 3;
		pIndices[9] = 0;	pIndices[10] = 2;	pIndices[11] = 3;

		m_iCurFragIdx += 12;
	}
	else	//	Convert quadrangle to triangles
	{
		iNumVert  = 5;
		iNumIndex = 18;

		if (m_iCurFragVert + iNumVert > m_iNumFragVert ||
			m_iCurFragIdx + iNumIndex > m_iNumFragIdx)
		{
			g_A3DErrLog.Log("A3DPArray::EmitOneFragment() buffer overflow!");
			return false;
		}

		pVert = &m_aFragVerts[m_iCurFragVert];

		//	The first 4 vertices
		memcpy(pVert, aVerts, 4 * sizeof (A3DLVERTEX));

		aTemp[0]	= aVerts[0].x + aVerts[1].x + aVerts[2].x + aVerts[3].x;
		aTemp[1]	= aVerts[0].y + aVerts[1].y + aVerts[2].y + aVerts[3].y;
		aTemp[2]	= aVerts[0].z + aVerts[1].z + aVerts[2].z + aVerts[3].z;
		vPos.x		= aTemp[0] * 0.25f;
		vPos.y		= aTemp[1] * 0.25f;
		vPos.z		= aTemp[2] * 0.25f;
		u	= (aVerts[0].tu + aVerts[1].tu + aVerts[2].tu + aVerts[3].tu) * 0.25f;
		v	= (aVerts[0].tv + aVerts[1].tv + aVerts[2].tv + aVerts[3].tv) * 0.25f;

		//	The vertex make fragment's thickness
		pVert[4].x  = vPos.x - vNormal.x * m_vFragmentThickness;
		pVert[4].y  = vPos.y - vNormal.y * m_vFragmentThickness;
		pVert[4].z  = vPos.z - vNormal.z * m_vFragmentThickness;
		pVert[4].tu	= u;
		pVert[4].tv = v;
		pVert[4].diffuse  = aVerts[0].diffuse;
		pVert[4].specular = aVerts[0].specular;

		vPos.x = (aTemp[0] + pVert[4].x) * 0.2f;
		vPos.y = (aTemp[1] + pVert[4].y) * 0.2f;
		vPos.z = (aTemp[2] + pVert[4].z) * 0.2f;

		//	All particle use relative position to vPos, so translate them
		for (i=0; i < iNumVert; i++)
		{
			pVert[i].x -= vPos.x;
			pVert[i].y -= vPos.y;
			pVert[i].z -= vPos.z;
		}

		m_iCurFragVert += 5;

		//	Build indices
		pIndices	= &m_aFragIndices[m_iCurFragIdx];
		pIndices[0] = 0;	pIndices[1] = 1;	pIndices[2] = 2;
		pIndices[3] = 0;	pIndices[4] = 2;	pIndices[5] = 3;
		pIndices[6] = 1;	pIndices[7] = 0;	pIndices[8] = 4;
		pIndices[9] = 2;	pIndices[10] = 1;	pIndices[11] = 4;
		pIndices[12] = 3;	pIndices[13] = 2;	pIndices[14] = 4;
		pIndices[15] = 0;	pIndices[16] = 3;	pIndices[17] = 4;

		m_iCurFragIdx += 18;
	}
	
	Particle.nTextureID	= m_iCurTexID;
	Particle.pVertex	= pVert;
	Particle.nVertCount	= iNumVert;
	Particle.nIndexCount= iNumIndex;
	Particle.pIndex		= pIndices;

	A3DMATRIX4 matRotateVar = a3d_RotateX(RandFloat(0, 30.0f)) * a3d_RotateY(RandFloat(0, 30.0f));
	pCommon->vecDir		= Normalize(vPos - m_vObjCenter) * matRotateVar;
	pCommon->vecSpeed	= GetNextParticleSpeedValue() * pCommon->vecDir;
	pCommon->vecPos		= vPos;
	
	//	Fill these common parameters;
	FillCommonParticle(pCommon);
	
	// Add some tail smoke effects to some particles;
	if (a_Random(0, 99) < 100 && m_iNumGFXs < 1)
	{
		if( NULL == strstr(GetName(), "nosmoke_") )
		{
			m_iNumGFXs ++;
			A3DGraphicsFX * pGFX;
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile("SmokeTail.gfx", NULL, NULL, false, &pGFX) )
				return false;
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->AddGFX(pGFX) )
				return false;
			pGFX->SetDir(A3DVECTOR3(0.0f,1.0f,0.0f));
			pGFX->SetPos(vPos);
			pGFX->Start(false);
			Particle.pGFX = pGFX;
		}
	}

	AddParticle((LPBYTE)&Particle);
	return true;
}

//	Flush frag buffer
bool A3DPArray::FlushFragBuffer()
{
	if (!m_FragBuf.aVerts || !m_FragBuf.iNumIdx)
		return true;

	//	Randomly discard this fragment if it's too small
	if (m_FragBuf.fArea < m_fMinFragArea && RandFloat(0.5f, 0.5f) < 0.4f)
	{
		//	Now clear buffer
		m_FragBuf.iNumTriangle	= 0;
		m_FragBuf.iNumIdx		= 0;
		m_FragBuf.fArea			= 0.0f;

		return true;
	}

	OBJECT_FRAGMENT_PARTICLE Particle;
	COMMON_PARTICLE* pCommon;
	
	memset(&Particle, 0, sizeof (OBJECT_FRAGMENT_PARTICLE));
	pCommon = &Particle.common;

	A3DLVERTEX* pVert;
	A3DVECTOR3 vPos;
	WORD* pIndices;
	int i, iCnt1, iCnt2, iNumVert, iNumIndex;
	float u=0.0f, v=0.0f, aTemp[3] = {0.0f, 0.0f, 0.0f};

	iNumVert	= m_FragBuf.iNumTriangle * 3 + 1;
	iNumIndex	= m_FragBuf.iNumTriangle * 12;

	if (m_iCurFragVert + iNumVert > m_iNumFragVert ||
		m_iCurFragIdx + iNumIndex > m_iNumFragIdx)
	{
		g_A3DErrLog.Log("A3DPArray::EmitOneFragment() buffer overflow!");
		return false;
	}

	pVert	 = &m_aFragVerts[m_iCurFragVert];
	pIndices = &m_aFragIndices[m_iCurFragIdx];

	for (i=0, iCnt1=0, iCnt2=0; i < m_FragBuf.iNumTriangle; i++, iCnt1+=3, iCnt2+=12)
	{
		pVert[iCnt1]	= m_FragBuf.aVerts[m_FragBuf.aIndices[iCnt1]];
		pVert[iCnt1+1]	= m_FragBuf.aVerts[m_FragBuf.aIndices[iCnt1+1]];
		pVert[iCnt1+2]	= m_FragBuf.aVerts[m_FragBuf.aIndices[iCnt1+2]];

		aTemp[0] += pVert[iCnt1].x + pVert[iCnt1+1].x + pVert[iCnt1+2].x;
		aTemp[1] += pVert[iCnt1].y + pVert[iCnt1+1].y + pVert[iCnt1+2].y;
		aTemp[2] += pVert[iCnt1].z + pVert[iCnt1+1].z + pVert[iCnt1+2].z;
		u += pVert[iCnt1].tu + pVert[iCnt1+1].tu + pVert[iCnt1+2].tu;
		v += pVert[iCnt1].tv + pVert[iCnt1+1].tv + pVert[iCnt1+2].tv;

		//	Build indices
		pIndices[iCnt2] = iCnt1;		pIndices[iCnt2+1] = iCnt1+1;	pIndices[iCnt2+2] = iCnt1+2;
		pIndices[iCnt2+3] = iCnt1+1;	pIndices[iCnt2+4] = iCnt1;		pIndices[iCnt2+5] = iNumVert-1;
		pIndices[iCnt2+6] = iCnt1+2;	pIndices[iCnt2+7] = iCnt1+1;	pIndices[iCnt2+8] = iNumVert-1;
		pIndices[iCnt2+9] = iCnt1;		pIndices[iCnt2+10] = iCnt1+2;	pIndices[iCnt2+11] = iNumVert-1;
	}

	float fTemp = 1.0f / (iNumVert-1);
	vPos.x = aTemp[0] * fTemp;
	vPos.y = aTemp[1] * fTemp;
	vPos.z = aTemp[2] * fTemp;
	u *= fTemp;
	v *= fTemp;

	//	The vertex make fragment's thickness
	iCnt1 = iNumVert - 1;
	pVert[iCnt1].x  = vPos.x - m_FragBuf.vNormal.x * m_vFragmentThickness;
	pVert[iCnt1].y  = vPos.y - m_FragBuf.vNormal.y * m_vFragmentThickness;
	pVert[iCnt1].z  = vPos.z - m_FragBuf.vNormal.z * m_vFragmentThickness;
	pVert[iCnt1].tu	= u;
	pVert[iCnt1].tv = v;
	pVert[iCnt1].diffuse  = pVert[0].diffuse;
	pVert[iCnt1].specular = pVert[0].specular;

	fTemp  = 1.0f / iNumVert;
	vPos.x = (aTemp[0] + pVert[iCnt1].x) * fTemp;
	vPos.y = (aTemp[1] + pVert[iCnt1].y) * fTemp;
	vPos.z = (aTemp[2] + pVert[iCnt1].z) * fTemp;

	//	All particle use relative position to vPos, so translate them
	for (i=0; i < iNumVert; i++)
	{
		pVert[i].x -= vPos.x;
		pVert[i].y -= vPos.y;
		pVert[i].z -= vPos.z;
	}
	
	m_iCurFragVert	+= iNumVert;
	m_iCurFragIdx	+= iNumIndex;

	Particle.nTextureID	= m_iCurTexID;
	Particle.pVertex	= pVert;
	Particle.nVertCount	= iNumVert;
	Particle.nIndexCount= iNumIndex;
	Particle.pIndex		= pIndices;

	A3DMATRIX4 matRotateVar = a3d_RotateX(RandFloat(0, 30.0f)) * a3d_RotateY(RandFloat(0, 30.0f));
	pCommon->vecDir		= Normalize(vPos - m_vObjCenter) * matRotateVar;
	pCommon->vecSpeed	= GetNextParticleSpeedValue() * pCommon->vecDir;
	pCommon->vecPos		= vPos;
	
	//	Fill these common parameters;
	FillCommonParticle(pCommon);
	
	// Add some tail smoke effects to some particles;
	if (a_Random(0, 99) < 100 && m_iNumGFXs < 1)
	{
		if( NULL == strstr(GetName(), "nosmoke_") )
		{
			m_iNumGFXs ++;
			A3DGraphicsFX * pGFX;
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile("SmokeTail.gfx", NULL, NULL, false, &pGFX) )
				return false;
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->AddGFX(pGFX) )
				return false;
			pGFX->SetDir(A3DVECTOR3(0.0f,1.0f,0.0f));
			pGFX->SetPos(vPos);
			pGFX->Start(false);
			Particle.pGFX = pGFX;
		}
	}

	AddParticle((LPBYTE)&Particle);
	
	//	Now clear buffer
	m_FragBuf.iNumTriangle	= 0;
	m_FragBuf.iNumIdx		= 0;
	m_FragBuf.fArea			= 0.0f;

	return true;
}

// Emit the particles;
bool A3DPArray::EmitParticles()
{
	int nToNew = GetNumberToNew();
	STANDARD_PARTICLE NewParticle;

	for(int n=0; n<nToNew; n++)
	{
		switch(m_ParticleType)
		{
		case A3DPARTICLE_STANDARD_PARTICLES:
			COMMON_PARTICLE * pNewCommonParticle;

			ZeroMemory(&NewParticle, sizeof(STANDARD_PARTICLE));
			pNewCommonParticle = &NewParticle.common;

			// ......
			// At this point you must set the particle's speed and dir;

			// Fill these common parameters;
			FillCommonParticle(pNewCommonParticle);

			AddParticle((LPBYTE)&NewParticle);
			break;

		case A3DPARTICLE_META_PARTICLES:

			g_A3DErrLog.Log("A3DPArray::EmitParticles(), Not implemented now!");
			return false;

		case A3DPARTICLE_OBJECT_FRAGMENTS:

			// For object fragments, all generation occurs at InitObjectFragment function;
			//	Explode model and create fragments
			if (m_nTicks == m_nEmitStart)
				SplitModel();

			return true;

		case A3DPARTICLE_INSTANCED_GEOMETRY:

			g_A3DErrLog.Log("A3DPArray::EmitParticles(), Not implemented now!");
			return false;
		}
	}
	return true;
}

bool A3DPArray::MakeDead(LPVOID pParticle)
{
	switch( m_ParticleType )
	{
	case A3DPARTICLE_STANDARD_PARTICLES:
		break;

	case A3DPARTICLE_META_PARTICLES:
		g_A3DErrLog.Log("A3DPArray::MakeDead(), Not implemented now!");
		return false;

	case A3DPARTICLE_OBJECT_FRAGMENTS:
		OBJECT_FRAGMENT_PARTICLE * pObjectFragmentParticle;
		pObjectFragmentParticle = (OBJECT_FRAGMENT_PARTICLE *) pParticle;

		if( pObjectFragmentParticle->pGFX )
			pObjectFragmentParticle->pGFX->Stop(true);
		break;

	case A3DPARTICLE_INSTANCED_GEOMETRY:

		g_A3DErrLog.Log("A3DPArray::MakeDead(), Not implemented now!");
		return false;
	}

	return true;
}
