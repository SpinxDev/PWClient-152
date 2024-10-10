/*
 * FILE: A3DLitModelShadredDataMan.h
 *
 * DESCRIPTION: Manager for lit model share data
 *
 * CREATED BY: Yaochunhui, 2013/1/11
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLITMODEL_SHARED_DATA_MAN_H_
#define _A3DLITMODEL_SHARED_DATA_MAN_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include <AArray.h>
#include <map>

class A3DEngine;
class A3DLitModelSharedData;

//////////////////////////////////////////////////////////////////////////
// class A3DLitModelSharedMesh
//	
//		Shared mesh data
//////////////////////////////////////////////////////////////////////////

class A3DLitModelSharedDataMan
{
public:
    A3DLitModelSharedDataMan();
    ~A3DLitModelSharedDataMan();

    bool Init(A3DEngine* pA3DEngine);
    void Release();
    void Clear();

    A3DLitModelSharedData* LoadSharedData(const char* szFileName);
    void ReleaseSharedData(const char* szFileName);
    bool SaveSharedData(const char* szFileName, A3DLitModelSharedData* pData);
    A3DLitModelSharedData* FindSharedData(const char* szFileName);

    bool ExportAll(const char* szBaseDir);

protected:
    struct DataRec
    {
        AString                 strFileName;
        A3DLitModelSharedData*  pData;
        int                     nRefCount;
    };
    
protected:
    std::map<AString, DataRec*> m_mapRecords;
    CRITICAL_SECTION m_cs;
    DataRec* FindSharedDataRecord(const char* szFileName);

};

#endif