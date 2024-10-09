#ifndef _FWTEXTMESH_H_
#define _FWTEXTMESH_H_

#include <A3DVector.h>
#include "FWConfig.h"
#include <WTypes.h>

struct MeshParam
{
	HDC			hDC;
	LPCTSTR		pString;
	int			cbString;
	float		fThickness;
	A3DVECTOR3	*pVertices;
	int			nVerticesCount;
	int			nMaxVerticesCount;
	int			*pIndices;
	int			nIndicesCount;
	int			nMaxIndicesCount;
	float		fXMin, fXMax, fYMin, fYMax, fZMin, fZMax;
};

#define CREAT_MESH_FAILED 1
#define	VERTICES_BUFFER_TOO_SMALL 2
#define	INDICES_BUFFER_TOO_SMALL 3

class CMesh;
extern int GetMeshFromChar(MeshParam *pMeshParam);
extern int FillMeshParam(MeshParam *pMeshParam, CMesh *pMesh);
extern void CopyMeshParam(const MeshParam *pSrc, MeshParam *pDest);

#endif
