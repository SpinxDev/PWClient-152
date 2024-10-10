#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DGFXPhysDataMan.h"
#include "APhysX.h"
#include "A3DGFXExMan.h"
#include "A3DGFXElement.h"
#include "A3DPhysElement.h"
#include "A3DPhysFluidSyncData.h"

#define A3DGFXPHYSMAGIC (DWORD('g' << 24 | 'p' << 16 | 'h' << 8 | 'y'))
#define A3DGFXPHYSFILEVER 1

#define A3DPHYSEMITTER_PREFIX "GFXAPE_"

static const char * szGFXPhysExt = ".gphy";

//////////////////////////////////////////////////////////////////////////
//
//	Global Functions
//
//////////////////////////////////////////////////////////////////////////

//	Write string to stream
bool WriteString(NxStream* ps, const AString& str)
{
	//	Write length of string
	int iLen = str.GetLength();
	ps->storeDword((NxI32)iLen);

	//	Write string data
	if (iLen)
		ps->storeBuffer((const char*)str, iLen);

	return true;
}

//	Read string from stream
bool ReadString(NxStream* ps, AString& str)
{
	//	Read length of string
	int iLen = (int)ps->readDword();

	//	Read string data
	if (iLen)
	{
		char* szBuf = new char [iLen+1];
		if (!szBuf)
			return false;

		ps->readBuffer(szBuf, iLen);
		szBuf[iLen] = '\0';
		str = szBuf;

		delete [] szBuf;
	}
	else
		str = "";

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement IGFXPhysSyncData
//
//////////////////////////////////////////////////////////////////////////

IGFXPhysSyncData::IGFXPhysSyncData(int iPhysSyncDataType)
: m_iDataType(iPhysSyncDataType)
{

}

void IGFXPhysSyncData::DestroyPhysSyncData(IGFXPhysSyncData* pData)
{
	delete pData;
}

IGFXPhysSyncData* IGFXPhysSyncData::CreateFromStream(NxStream* ps)
{
	int iPhysSyncDataType = (int)ps->readDword();
	switch (iPhysSyncDataType)
	{
	case GFXPHYSSYNCDAT_FLUID:
		return new A3DPhysFluidSyncData();
	default:
		return NULL;
	}
}

void IGFXPhysSyncData::StorePhysSyncDataTypeToStream(NxStream* ps, const int iDataType)
{
	ps->storeDword(iDataType);
}

void IGFXPhysSyncData::SetName(const char * szName)
{
	m_strName = szName;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DGFXPhysDataMan
//
//////////////////////////////////////////////////////////////////////////

// GetPhysFileName from GfxFileName
AString A3DGFXPhysDataMan::GfxFileName2PhysFileName(const char * szGfxFileName)
{
	char buf[MAX_PATH];
	af_GetFileTitle(szGfxFileName, buf, MAX_PATH);
	af_ChangeFileExt(buf, MAX_PATH, szGFXPhysExt);
	return AString(buf);
}

AString A3DGFXPhysDataMan::GfxFileExt2PhysFileExt(const char * szGfxFileName)
{
	AString strRet(szGfxFileName);
	af_ChangeFileExt(strRet, szGFXPhysExt);
	return strRet;
}

A3DGFXPhysDataMan::A3DGFXPhysDataMan(A3DGFXEx* pGFXEx)
: m_pGfxEx(pGFXEx),
  m_nCounter(0)
{
	m_pPhysXScene = NULL;
}

A3DGFXPhysDataMan::~A3DGFXPhysDataMan()
{
	Release();
}

void A3DGFXPhysDataMan::Release()
{
	for (A3DPhysSyncDataMap::iterator itr = m_mapPhysSyncData.begin()
		; itr != m_mapPhysSyncData.end()
		; ++itr)
	{
		IGFXPhysSyncData::DestroyPhysSyncData(itr->second);
	}
	m_mapPhysSyncData.clear();
	m_physEleLst.clear();
	m_nCounter = 0;
}

bool A3DGFXPhysDataMan::LoadPhys(const char* szPhysGfxFile)
{	
	if (!af_IsFileExist(szPhysGfxFile)) 
		return false;
		
	APhysXFileImageStream stream(szPhysGfxFile, true);
	if (!stream.IsValid())
	{
		a_LogOutput(1, "A3DGFXPhysDataMan::LoadPhys, Failed to open gfx phys data stream %s", szPhysGfxFile);
		return false;
	}

	DWORD dwMagic = stream.readDword();
	if (dwMagic != A3DGFXPHYSMAGIC)
	{
		a_LogOutput(1, "A3DGFXPhysDataMan::LoadPhys, Unknown file format for gfx physics file %s", szPhysGfxFile);
		return false;
	}

	DWORD dwVersion = stream.readDword();
	if (dwVersion > A3DGFXPHYSFILEVER)
	{
		a_LogOutput(1, "A3DGFXPhysDataMan::LoadPhys, Invalid file version %d (required: %d)", dwVersion, A3DGFXPHYSFILEVER);
		return false;
	}

	int iDataNum = (int)stream.readDword();
	for (int iDataIdx = 0; iDataIdx < iDataNum; ++iDataIdx)
	{
		IGFXPhysSyncData* pSyncData = IGFXPhysSyncData::CreateFromStream(&stream);
		if (!pSyncData || !pSyncData->Load(&stream, dwVersion))
		{
			a_LogOutput(1, "A3DGFXPhysDataMan::LoadPhys, Failed to create or load sync data from stream (%s).", szPhysGfxFile);
			return false;
		}

		if (m_mapPhysSyncData.find(pSyncData->GetName()) != m_mapPhysSyncData.end())
		{
			a_LogOutput(1, "A3DGFXPhysDataMan::LoadPhys, Duplicated physics sync data found.");
			return false;
		}

		m_mapPhysSyncData[pSyncData->GetName()] = pSyncData;
	}

	return true;
}

bool A3DGFXPhysDataMan::SavePhys(const char* szPhysGfxFile)
{
	if (!m_physEleLst.size())
		return false;

	APhysXUserStream stream(szPhysGfxFile, false);

	if (!stream.IsValid())
	{
		a_LogOutput(1, "A3DGFXPhysDataMan::SavePhys, failed to create file %s", szPhysGfxFile);
		return false;
	}

	//	Write file magic identifier
	stream.storeDword(A3DGFXPHYSMAGIC);

	//	Write file version
	stream.storeDword(A3DGFXPHYSFILEVER);

	//	Write data number
	stream.storeDword(m_physEleLst.size());

	for (size_t iEleIdx = 0; iEleIdx < m_physEleLst.size(); ++iEleIdx)
	{
		IGFXPhysSyncData* pPhysSyncData = m_physEleLst[iEleIdx]->GetPhysSyncData();
		if (!pPhysSyncData)
		{
			a_LogOutput(1, "A3DGFXPhysDataMan::SavePhys, failed to get phys sync data");
			return false;
		}

		IGFXPhysSyncData::StorePhysSyncDataTypeToStream(&stream, pPhysSyncData->GetPhysSyncDataType());
		if (!pPhysSyncData->Save(&stream))
		{
			a_LogOutput(1, "A3DGFXPhysDataMan::SavePhys, failed to save sync data.");
			return false;
		}
	}

	return true;
}

// Get fluid sync data by name
IGFXPhysSyncData* A3DGFXPhysDataMan::GetPhysSyncData(const char * szDataName)
{
	A3DPhysSyncDataMap::iterator itr = m_mapPhysSyncData.find(szDataName);
	if (itr != m_mapPhysSyncData.end())
		return itr->second;

	return NULL;
}

void A3DGFXPhysDataMan::RegisterElement(A3DGFXElement* pEle)
{
	if (!pEle->IsPhysEle())
		return;

	A3DPhysElement* pPhysEle = (A3DPhysElement*)pEle;	
	if (std::find(m_physEleLst.begin(), m_physEleLst.end(), pPhysEle) == m_physEleLst.end())
		m_physEleLst.push_back(pPhysEle);
}

void A3DGFXPhysDataMan::RemoveElement(A3DGFXElement* pEle)
{
	if (!pEle->IsPhysEle()) 
		return;

	PhysEleList::iterator itr = std::find(m_physEleLst.begin(), m_physEleLst.end(), (A3DPhysElement*)pEle);
	if (itr != m_physEleLst.end())
		m_physEleLst.erase(itr);
}

#endif
