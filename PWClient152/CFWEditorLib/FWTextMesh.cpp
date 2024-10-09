// TextMesh.cpp : Defines the entry point for the DLL application.
//

#include "FWTextMesh.h"
#include "Mesh.h"
#include "CodeTemplate.h"

#define new A_DEBUG_NEW


int GetMeshFromChar(MeshParam *pMeshParam)
{
	CMesh mesh;
	if (!mesh.CreateFromString(pMeshParam->pString, pMeshParam->cbString, pMeshParam->hDC, pMeshParam->fThickness))
		return CREAT_MESH_FAILED;

	return FillMeshParam(pMeshParam, &mesh);
}


int FillMeshParam(MeshParam *pMeshParam, CMesh *pMesh)
{
	if (pMeshParam->nMaxVerticesCount < pMesh->m_aryVertices.GetSize())
	{
		SAFE_DELETE_ARRAY(pMeshParam->pVertices);
		pMeshParam->pVertices = new A3DVECTOR3[pMesh->m_aryVertices.GetSize()];
		pMeshParam->nMaxVerticesCount = pMesh->m_aryVertices.GetSize();
	}
	pMeshParam->nVerticesCount = pMesh->m_aryVertices.GetSize();
	
	int i(0);
	for (i = 0; i < (int)pMesh->m_aryVertices.GetSize(); i++)
	{
		pMeshParam->pVertices[i].x = pMesh->m_aryVertices[i].x;
		pMeshParam->pVertices[i].y = pMesh->m_aryVertices[i].y;
		pMeshParam->pVertices[i].z = pMesh->m_aryVertices[i].z;
	}
	
	if (pMeshParam->nMaxIndicesCount < (int)pMesh->m_aryTriangles.GetSize() * 3)
	{
		SAFE_DELETE_ARRAY(pMeshParam->pIndices);
		pMeshParam->pIndices = new int[pMesh->m_aryTriangles.GetSize() * 3];
		pMeshParam->nMaxIndicesCount = pMesh->m_aryTriangles.GetSize() * 3;
	}
	pMeshParam->nIndicesCount = pMesh->m_aryTriangles.GetSize() * 3;
	
	int *p = pMeshParam->pIndices;
	for (i = 0; i < (int)pMesh->m_aryTriangles.GetSize(); i++)
	{
		*p++ = pMesh->m_aryTriangles[i].Index0;
		*p++ = pMesh->m_aryTriangles[i].Index1;
		*p++ = pMesh->m_aryTriangles[i].Index2;
	}
	
	pMeshParam->fXMin = pMesh->m_xMin;
	pMeshParam->fXMax = pMesh->m_xMax;
	pMeshParam->fYMin = pMesh->m_yMin;
	pMeshParam->fYMax = pMesh->m_yMax;
	pMeshParam->fZMin = pMesh->m_zMin;
	pMeshParam->fZMax = pMesh->m_zMax;

	return 0;
}

extern void CopyMeshParam(const MeshParam *pSrc, MeshParam *pDest)
{
	if (pDest->nVerticesCount < pSrc->nVerticesCount)
	{
		pDest->nVerticesCount = pSrc->nVerticesCount;
		delete [] pDest->pVertices;
		pDest->pVertices = new A3DVECTOR3[pDest->nVerticesCount];
	}

	if (pDest->nIndicesCount < pSrc->nIndicesCount)
	{
		pDest->nIndicesCount = pSrc->nIndicesCount;
		delete [] pDest->pIndices;
		pDest->pIndices = new int[pDest->nIndicesCount];
	}

	memcpy(pDest->pVertices, pSrc->pVertices, sizeof(A3DVECTOR3) * pSrc->nVerticesCount);
	memcpy(pDest->pIndices, pSrc->pIndices, sizeof(int) * pSrc->nIndicesCount);
	
	A3DVECTOR3 *pBak1 = pDest->pVertices;
	int *pBak2 = pDest->pIndices;
	memcpy(pDest, pSrc, sizeof(MeshParam));
	pDest->pVertices = pBak1;
	pDest->pIndices = pBak2;
}
