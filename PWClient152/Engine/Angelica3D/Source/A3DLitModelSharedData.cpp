#include "A3DLitModelSharedData.h"
#include "AFileImage.h"

#include "A3DPI.h"
#include "A3DMacros.h"

static const DWORD FILE_MAGIC = ('L') | ('M' << 8) | ('S' << 16) | ('D' << 24);
static const DWORD FILE_VERSION = 1;

//////////////////////////////////////////////////////////////////////////
// class A3DLitModelSharedMesh
//	
//		Shared mesh data
//////////////////////////////////////////////////////////////////////////

A3DLitMeshSharedData::A3DLitMeshSharedData()
{
    m_nVertex       = 0;
    m_nIndex        = 0;
    m_pPositions    = NULL;
    m_pNormals      = NULL;
    m_pUVs          = NULL;
    m_pLMUVs        = NULL;
    m_pIndices      = NULL;
    m_strTextureMap = "";
}

A3DLitMeshSharedData::~A3DLitMeshSharedData()
{
}

void A3DLitMeshSharedData::Init(int nVertex, int nIndex, bool bHasLightMap)
{
    Release();
    m_nVertex = nVertex;
    m_nIndex = nIndex;
    m_pPositions = new A3DVECTOR3[nVertex];
    m_pNormals = new A3DVECTOR3[nVertex];
    m_pUVs = new float[nVertex * 2];
    if (bHasLightMap)
        m_pLMUVs = new float[nVertex * 2];
    m_pIndices = new WORD[m_nIndex];
}

void A3DLitMeshSharedData::Release()
{
    m_nVertex       = 0;
    m_nIndex        = 0;
    delete m_pPositions;
    m_pPositions    = NULL;
    delete m_pNormals;
    m_pNormals      = NULL;
    delete m_pUVs;
    m_pUVs          = NULL;
    delete m_pLMUVs;
    m_pLMUVs        = NULL;
    delete m_pIndices;
    m_pIndices      = NULL;
}

//////////////////////////////////////////////////////////////////////////
// class A3DLitModelSharedData
//	
//		Shared model data
//////////////////////////////////////////////////////////////////////////
A3DLitModelSharedData::A3DLitModelSharedData()
{

}

A3DLitModelSharedData::~A3DLitModelSharedData()
{

}

void A3DLitModelSharedData::Init()
{

}

void A3DLitModelSharedData::Release()
{
    ClearMeshes();
}

bool A3DLitModelSharedData::LoadFromFile(AFile* pFile)
{
    ClearMeshes();
    DWORD dwMagic;
    DWORD dwLen;
    pFile->Read(&dwMagic, sizeof(DWORD), &dwLen);
    if (dwMagic != FILE_MAGIC)
    {
        g_A3DErrLog.Log("A3DLitModelSharedData::LoadFromFile, failed to read file: %s", pFile->GetFileName());
        return false;
    }
    DWORD dwVersion;
    pFile->Read(&dwVersion, sizeof(DWORD), &dwLen);
    if (dwVersion > FILE_VERSION)
    {
        g_A3DErrLog.Log("A3DLitModelSharedData::LoadFromFile, unknown file version: %s", pFile->GetFileName());
        return false;
    }

    DWORD dwNumMesh = 0;
    pFile->Read(&dwNumMesh, sizeof(DWORD), &dwLen);
    
    for (DWORD i = 0; i < dwNumMesh; i++)
    {
        A3DLitMeshSharedData* pMesh = new A3DLitMeshSharedData;
        int nVertex = 0;
        int nIndex = 0;
        bool bLightMap = false;
        pFile->Read(&nVertex, sizeof(int), &dwLen);
        pFile->Read(&nIndex, sizeof(int), &dwLen);
        pFile->Read(&bLightMap, sizeof(bool), &dwLen);

        pMesh->Init(nVertex, nIndex, bLightMap);

        pFile->Read(pMesh->GetPositions(), nVertex * sizeof(A3DVECTOR3), &dwLen);
        pFile->Read(pMesh->GetNormals(), nVertex * sizeof(A3DVECTOR3), &dwLen);
        pFile->Read(pMesh->GetUVs(), nVertex * sizeof(float) * 2, &dwLen);
        if (bLightMap)
            pFile->Read(pMesh->GetLMUVs(), nVertex * sizeof(float) * 2, &dwLen);
        pFile->Read(pMesh->GetIndices(), nIndex * sizeof(WORD), &dwLen);
        AString strTextureMap;
        pFile->ReadString(strTextureMap);
        pMesh->SetTextureMap(strTextureMap);

        A3DMaterial mat;
        mat.Load(NULL, pFile);
        pMesh->SetMaterial(mat);
        m_aMeshes.Add(pMesh);
    }

    return true;
}

bool A3DLitModelSharedData::LoadFromFile(const char* szFileName)
{
    AFileImage File;
    if (!File.Open(szFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
    {
        g_A3DErrLog.Log("A3DLitModelSharedData::LoadFromFile, failed to open file: %s", szFileName);
        return false;
    }
    bool bResult = LoadFromFile(&File);
    File.Close();
    return bResult;
}

bool A3DLitModelSharedData::SaveToFile(AFile* pFile) const
{
    DWORD dwMagic = FILE_MAGIC;
    DWORD dwLen;
    pFile->Write(&dwMagic, sizeof(DWORD), &dwLen);
    DWORD dwVersion = FILE_VERSION;
    pFile->Write(&dwVersion, sizeof(DWORD), &dwLen);

    DWORD dwNumMesh = m_aMeshes.GetSize();
    pFile->Write(&dwNumMesh, sizeof(DWORD), &dwLen);

    for (int i = 0; i < m_aMeshes.GetSize(); i++)
    {
        A3DLitMeshSharedData* pMesh = m_aMeshes[i];
        int nVertex = pMesh->GetNumVertex();
        int nIndex = pMesh->GetNumIndex();
        bool bLightMap = pMesh->GetLMUVs() != NULL;
        pFile->Write(&nVertex, sizeof(int), &dwLen);
        pFile->Write(&nIndex, sizeof(int), &dwLen);
        pFile->Write(&bLightMap, sizeof(bool), &dwLen);

        pFile->Write(pMesh->GetPositions(), nVertex * sizeof(A3DVECTOR3), &dwLen);
        pFile->Write(pMesh->GetNormals(), nVertex * sizeof(A3DVECTOR3), &dwLen);
        pFile->Write(pMesh->GetUVs(), nVertex * sizeof(float) * 2, &dwLen);
        if (bLightMap)
            pFile->Write(pMesh->GetLMUVs(), nVertex * sizeof(float) * 2, &dwLen);
        pFile->Write(pMesh->GetIndices(), nIndex * sizeof(WORD), &dwLen);
        pFile->WriteString(pMesh->GetTextureMap());
        A3DMaterial mat = pMesh->GetMaterial();
        mat.Save(pFile);
    }
    return true;
}

bool A3DLitModelSharedData::SaveToFile(const char* szFileName) const
{
    AFile File;
    if (!File.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY))
    {
        g_A3DErrLog.Log("A3DLitModelSharedData::SaveToFile, failed to open file: %s", szFileName);
        return false;
    }
    bool bResult = SaveToFile(&File);
    File.Close();
    return bResult;

}

void A3DLitModelSharedData::ClearMeshes()
{
    for (int i = 0; i < m_aMeshes.GetSize(); i++)
    {
        A3DRELEASE(m_aMeshes[i]);
    }
    m_aMeshes.RemoveAll();
}