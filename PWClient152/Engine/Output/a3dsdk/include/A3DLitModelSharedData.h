/*
 * FILE: A3DLitModelShadredData.h
 *
 * DESCRIPTION: To store shared data for lit models containing the same geometry
 *
 * CREATED BY: Yaochunhui, 2013/1/11
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLITMODEL_SHARED_DATA_H_
#define _A3DLITMODEL_SHARED_DATA_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include <AArray.h>
#include "A3DMaterial.h"

//////////////////////////////////////////////////////////////////////////
// class A3DLitModelSharedMesh
//	
//		Shared mesh data
//////////////////////////////////////////////////////////////////////////

class A3DLitMeshSharedData
{
public:
    A3DLitMeshSharedData();
    ~A3DLitMeshSharedData();

    void Init(int nVertex, int nIndex, bool bHasLightMap);
    void Release();

    int GetNumVertex() const { return m_nVertex; }
    int GetNumIndex() const  { return m_nIndex; }

    A3DVECTOR3*     GetPositions()  const { return m_pPositions; }
    A3DVECTOR3*     GetNormals()    const { return m_pNormals; }
    float*          GetUVs()        const { return m_pUVs; }
    float*          GetLMUVs()      const { return m_pLMUVs; }
    WORD*           GetIndices()    const { return m_pIndices; }

    const char*     GetTextureMap() const { return m_strTextureMap; }
    void            SetTextureMap(const char* szMap) { m_strTextureMap = szMap; }
    const A3DMaterial&  GetMaterial() const { return m_Material; }
    void            SetMaterial(const A3DMaterial& mat) { m_Material = mat; }

protected:
    int             m_nVertex;
    int             m_nIndex;
    A3DVECTOR3*     m_pPositions;
    A3DVECTOR3*     m_pNormals;
    float*          m_pUVs;
    float*          m_pLMUVs;
    WORD*           m_pIndices;
    AString         m_strTextureMap;
    A3DMaterial     m_Material;
};

//////////////////////////////////////////////////////////////////////////
// class A3DLitModelSharedData
//	
//		Shared model data
//////////////////////////////////////////////////////////////////////////

class A3DLitModelSharedData
{
public:
public:
    A3DLitModelSharedData();
    ~A3DLitModelSharedData();

    void Init();
    void Release();

    bool LoadFromFile(AFile* pFile);
    bool SaveToFile(AFile* pFile) const;
    bool LoadFromFile(const char* szFileName);
    bool SaveToFile(const char* szFileName) const;

    void ClearMeshes();
    int GetNumMeshes() const { return m_aMeshes.GetSize(); }
    A3DLitMeshSharedData* GetMesh(int i) const { return m_aMeshes[i]; }
    void AddMesh(A3DLitMeshSharedData* pMesh) { m_aMeshes.Add(pMesh); }

protected:
    AArray<A3DLitMeshSharedData*> m_aMeshes;
};


#endif