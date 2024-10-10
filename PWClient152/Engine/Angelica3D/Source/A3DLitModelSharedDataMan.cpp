#include "A3DLitModelSharedDataMan.h"
#include "A3DLitModelSharedData.h"
#include "AFileImage.h"

#include "A3DPI.h"
#include "A3DMacros.h"
#include "ACSWrapper.h"

A3DLitModelSharedDataMan::A3DLitModelSharedDataMan()
{
    InitializeCriticalSection(&m_cs);
}

A3DLitModelSharedDataMan::~A3DLitModelSharedDataMan()
{
    DeleteCriticalSection(&m_cs);
}

bool A3DLitModelSharedDataMan::Init(A3DEngine* pA3DEngine)
{
    Clear();
    return true;
}

void A3DLitModelSharedDataMan::Release()
{
    Clear();
}

A3DLitModelSharedData* A3DLitModelSharedDataMan::LoadSharedData(const char* szFileName)
{
    ACSWrapper csw(&m_cs);
    DataRec* pDataRec = FindSharedDataRecord(szFileName);
    if (pDataRec)
    {
        pDataRec->nRefCount++;
        return pDataRec->pData;
    }

    A3DLitModelSharedData* pData = new A3DLitModelSharedData;
    if (!pData->LoadFromFile(szFileName))
    {
        A3DRELEASE(pData);
        return NULL;
    }
    pDataRec = new DataRec;
    pDataRec->strFileName = szFileName;
    pDataRec->pData = pData;
    pDataRec->nRefCount = 1;

    m_mapRecords[szFileName] = pDataRec;

    return pData;
}

bool A3DLitModelSharedDataMan::SaveSharedData(const char* szFileName, A3DLitModelSharedData* pData)
{
    ACSWrapper csw(&m_cs);

    DataRec* pDataRec = FindSharedDataRecord(szFileName);
    if (pDataRec)
    {
        return true;
    }

    pDataRec = new DataRec;
    pDataRec->strFileName = szFileName;
    pDataRec->pData = pData;
    pDataRec->nRefCount = 1;

    m_mapRecords[szFileName] = pDataRec;

    return true;

}

A3DLitModelSharedData* A3DLitModelSharedDataMan::FindSharedData(const char* szFileName)
{
    DataRec* pDataRec = FindSharedDataRecord(szFileName);
    if (pDataRec)
        return pDataRec->pData;
    else
        return NULL;
}

A3DLitModelSharedDataMan::DataRec* A3DLitModelSharedDataMan::FindSharedDataRecord(const char* szFileName)
{
    ACSWrapper csw(&m_cs);

    std::map<AString, DataRec*>::const_iterator itr = m_mapRecords.find(szFileName);
    if (itr != m_mapRecords.end())
    {
        return itr->second;
    }
    else
    {
        return NULL;
    }
}

void A3DLitModelSharedDataMan::Clear()
{
    ACSWrapper csw(&m_cs);

    std::map<AString, DataRec*>::const_iterator itr = m_mapRecords.begin();
    for (; itr != m_mapRecords.end(); itr++)
    {
        A3DRELEASE(itr->second->pData);
        delete itr->second;
    }
    m_mapRecords.clear();
}

bool A3DLitModelSharedDataMan::ExportAll(const char* szBaseDir)
{
    ACSWrapper csw(&m_cs);

    std::map<AString, DataRec*>::const_iterator itr = m_mapRecords.begin();
    for (; itr != m_mapRecords.end(); itr++)
    {
        DataRec* pDataRec = itr->second;
        AString strFileName;
        strFileName.Format("%s\\%s", szBaseDir, pDataRec->strFileName);
        pDataRec->pData->SaveToFile(strFileName);
    }
    return true;
}

void A3DLitModelSharedDataMan::ReleaseSharedData(const char* szFileName)
{
    ACSWrapper csw(&m_cs);
    std::map<AString, DataRec*>::const_iterator itr = m_mapRecords.find(szFileName);
    if (itr != m_mapRecords.end())
    {
        DataRec* pDataRec = itr->second;
        if (pDataRec)
        {
            pDataRec->nRefCount--;
            if (pDataRec->nRefCount <= 0)
            {
                A3DRELEASE(pDataRec->pData);
                delete pDataRec;
                m_mapRecords.erase(itr);
            }
        }
    }
    return;
}