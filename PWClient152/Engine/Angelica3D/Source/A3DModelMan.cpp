#include "A3DModelMan.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DModelCollector.h"
#include "A3DDevice.h"
#include "A3DModel.h"
#include "AFileImage.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DModelMan::A3DModelMan()
{
	m_pA3DDevice = NULL;

	m_pModelCollector = NULL;
}

A3DModelMan::~A3DModelMan()
{
}

bool A3DModelMan::Init(A3DDevice * pDevice)
{
	m_strFolderName = "Models";
	m_pA3DDevice = pDevice;
	m_ListModel.Init();

	m_pModelCollector = new A3DModelCollector(100);
	if( NULL == m_pModelCollector )
	{
		g_A3DErrLog.Log("A3DModelMan::Init() Not enough memory!");
		return false;
	}
	return true;
}

bool A3DModelMan::Release()
{
	if( m_pModelCollector )
	{
		A3DModelCollector * pCollector = m_pModelCollector;

		// first set m_pModelCollector to NULL to avoid repush back model
		m_pModelCollector = NULL;
		pCollector->Release();
		delete pCollector;
	}

	ALISTELEMENT * pThisModelElement = m_ListModel.GetFirst();

	while( pThisModelElement != m_ListModel.GetTail() )
	{
		MODELRECORD * pModelRecord = (MODELRECORD *) pThisModelElement->pData;
		
		A3DModel * pModel = pModelRecord->pModel;

		pModel->Release();
		delete pModel;
		pModel = NULL;

		delete pModelRecord;
		pThisModelElement = pThisModelElement->pNext;
	}

	m_ListModel.Release();
	return true;
}

bool A3DModelMan::ReleaseModel(A3DModel *& pModel)
{
	// Stop all gfx and let them stop softly;
	pModel->SoftStopAllGFX();
	// force stop all sfx;
	pModel->StopAllSFX();
	pModel->UnloadSFX();
	pModel->UnloadImmEffect();

	assert(pModel->GetContainer() == A3DCONTAINER_NULL);
	
	// Reset model's state to the initial state;
	pModel->Reset();

	// Return to garbage collector, if it still exist;
	if( m_pModelCollector )
		m_pModelCollector->PushBack(pModel->GetName(), pModel);

	pModel = NULL;
	return true;
}

bool A3DModelMan::FindModelFile(const char * szFilename, A3DModel ** ppModel)
{
	ALISTELEMENT * pThisModelElement = m_ListModel.GetFirst();

	while( pThisModelElement != m_ListModel.GetTail() )
	{
		MODELRECORD * pModelRecord = (MODELRECORD *) pThisModelElement->pData;
		if (!pModelRecord->strFileName.CompareNoCase(szFilename))
		{
			*ppModel = pModelRecord->pModel;
			return true;
		}
		pThisModelElement = pThisModelElement->pNext;
	}
	return true;
}

bool A3DModelMan::LoadModelFile(const char* szFilename, A3DModel** ppModel, bool bChild)
{
	A3DModel * pNewModel = NULL;

	if( ppModel ) *ppModel = NULL;

	pNewModel = m_pModelCollector->PopUp(szFilename);
	if( pNewModel )
	{
		// If found, we use it;
		pNewModel->ReloadSFX();

		// Reset model's state to the initial state;
		pNewModel->Reset();

		*ppModel = pNewModel;
		return true;
	}

	if( !FindModelFile(szFilename, &pNewModel) )
		return false;

	if( pNewModel ) 
	{
		if( ppModel )
		{
			if( bChild )
				*ppModel = pNewModel;
			else
				return pNewModel->Duplicate(ppModel);
			return true;
		}
		else
			return true;
	}

	// Now open the file;
	AFileImage aFile;
	// First init a file image from seperate files or package;
	if (!aFile.Open(m_strFolderName, szFilename, AFILE_OPENEXIST | AFILE_TEMPMEMORY))
	{
		g_A3DErrLog.Log("A3DModelMan::LoadModelFile(), [%s] can not be located in the package.", szFilename);
		return false;
	}

	pNewModel = new A3DModel();
	if( NULL == pNewModel )
	{
		g_A3DErrLog.Log("A3DModelMan::LoadModelFile Not enough memory!");
		return false;
	}
	
	if( !pNewModel->Load(m_pA3DDevice, &aFile) )
	{								
		g_A3DErrLog.Log("A3DModelMan::LoadModelFile File %s parsed error!", szFilename);
		if( pNewModel ) delete pNewModel;
		aFile.Close();
		return false;
	}

	aFile.Close();
	
	pNewModel->SetName(szFilename);

	MODELRECORD * pNewModelRecord = new MODELRECORD;
	if( NULL == pNewModelRecord ) 
	{
		g_A3DErrLog.Log("A3DModelMan::LoadModelFile Not Enough Memory!");
		if( pNewModel )
		{
			pNewModel->Release();
			delete pNewModel;
		}
		return false;
	}
	pNewModelRecord->strFileName = szFilename;
	pNewModelRecord->pModel = pNewModel;
	
	if( !m_ListModel.Append((LPVOID)pNewModelRecord) )
		return false;

	if( ppModel )
	{
		if( bChild )
			*ppModel = pNewModel;
		else
		{
			// We can unload the record model's sound
			pNewModel->UnloadSFX();
			pNewModel->UnloadImmEffect();
			return pNewModel->Duplicate(ppModel);
		}
		return true;
	}
	else
		return true;
}

bool A3DModelMan::Reset()
{
	if( m_pModelCollector )
	{
		m_pModelCollector->Release();
	}

	ALISTELEMENT * pThisModelElement = m_ListModel.GetFirst();

	while( pThisModelElement != m_ListModel.GetTail() )
	{
		MODELRECORD * pModelRecord = (MODELRECORD *) pThisModelElement->pData;
		
		A3DModel * pModel = pModelRecord->pModel;
		pModel->Release();
		delete pModel;
		pModel = NULL;

		delete pModelRecord;
		pThisModelElement = pThisModelElement->pNext;
	}

	m_ListModel.Reset();
	return true;
}