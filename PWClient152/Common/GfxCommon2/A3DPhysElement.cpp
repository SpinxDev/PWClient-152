#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DPhysElement.h"
#include <AF.h>
#include "APhysX.h"
#include "A3DGFXPhysDataMan.h"
#include "A3DGFXExMan.h"
#include "A3DPhysPointEmitter.h"
#include "A3DPhysEmitter.h"

static const char _phys_desc_name[] = "PhysDesc: %s";

/*
 * Impl for A3DPhysElement
 */
A3DPhysElement::A3DPhysElement(A3DGFXEx* pGfx) :
A3DGFXElement(pGfx)
{ 
	m_bIsPhysEle = true;
	m_pPhysDataMan = pGfx->GetPhysDataMan();
	ASSERT(m_pPhysDataMan);
}

bool A3DPhysElement::Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion)
{
	// when this func is called
	// the A3DGFXElement's part has already been loaded

	char szLine[AFILE_LINEMAXLEN];
	char szBuf[MAX_PATH];
	DWORD dwReadLen;

	if (pFileToLoad->IsBinary())
	{
		pFileToLoad->ReadString(szBuf, MAX_PATH, &dwReadLen);
		
		if (!SetPhysSyncData(m_pPhysDataMan->GetPhysSyncData(szBuf)))
			return false;
	}
	else
	{
		// Read the phys desc name
		szBuf[0] = '\0';
		pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _phys_desc_name, szBuf);

		// Now Register the Desc
		if (!SetPhysSyncData(m_pPhysDataMan->GetPhysSyncData(szBuf)))
			return false;
	}

	return true;
}


bool A3DPhysElement::Save(AFile* pFile)
{
	char szLine[AFILE_LINEMAXLEN];

	IGFXPhysSyncData* pSyncData = GetPhysSyncData();
	if (!pSyncData)
		return false;
	
	if (pFile->IsBinary())
	{
		pFile->WriteString(pSyncData->GetName());
	}
	else
	{
		sprintf(szLine, _phys_desc_name, pSyncData->GetName());
		pFile->WriteLine(szLine);
	}
	return true;
}

A3DPhysElement* A3DPhysElement::CreatePhysElement(A3DGFXEx* pGfx, int nEleId)
{
	if (!gPhysXEngine || !gPhysXEngine->IsValid())
		return NULL;

	switch(nEleId) {
	case ID_ELE_TYPE_PHYS_EMITTER:
		return new A3DPhysEmitter(pGfx);
	case ID_ELE_TYPE_PHYS_POINTEMITTER:
		return new A3DPhysPointEmitter(pGfx);
	default:
		return NULL;
	}
}

#endif