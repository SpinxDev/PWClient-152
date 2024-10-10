#include "A3DModel.h"
#include "A3DEngine.h"
#include "A3DMoxMan.h"
#include "A3DModelMan.h"
#include "A3DFuncs.h"
#include "A3DPI.h"
#include "A3DWorld.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DCollision.h"
#include "A3DGraphicsFX.h"
#include "A3DGFXMan.h"
#include "A3DConfig.h"
#include "A3DCDS.h"
#include "A3DWireCollector.h"
#include "AFI.h"
#include "AFile.h"
#include "AMSoundEngine.h"
#include "AMSoundBufferMan.h"
#include "AMemory.h"

#define new A_DEBUG_NEW
//#define ENABLE_MODELOBB

//	Model version
#define MODELVERSION	2

A3DModel::A3DModel() : A3DObject()
{
	m_pA3DDevice = NULL;
	m_pRealParentModel = NULL;
	m_pParentModel = NULL;
	m_pParentFrame = NULL;
	m_pConnectionCenterFrame = NULL;
	m_nFrameCount = 0;

	m_nFrameOld = -1;
	m_nFrame = 0;
	m_nHeartBeats = -1;
	m_vTimeStored = 0.0f;

	m_nVertCount = 0;
	m_nIndexCount = 0;

	m_bHasAction = false;
	m_bHasMoved = true;
	m_bBuildOBBBevels = false;

	m_bTraceMoveAABBOnly = false;

	ZeroMemory(&m_ModelAABB, sizeof(A3DAABB));
	ZeroMemory(&m_ModelOBB, sizeof(A3DOBB));

	ZeroMemory(m_pProperties, PROPERTY_SIZE);

	m_pfnActionChangeCallBack	= NULL;
	m_pActionChangeArg			= NULL;
	m_pfnActionLoopEndCallBack	= NULL;
	m_pActionLoopArg			= NULL;
	m_pfnLogicEventCallBack		= NULL;
	m_pLogicArg					= NULL;

	m_pActionDefinitionList = NULL;
	m_pActionGroupList = NULL;

	m_dwRayTraceBits  = 0xffffffff;
	m_dwAABBTraceBits = 0xffffffff;

	m_Container	= A3DCONTAINER_NULL;
	m_bZPull = false;

	m_vecScale = A3DVECTOR3(1.0f);

	m_RayTraceRt.pModelOBB		= NULL;
	m_RayTraceRt.pRootModel		= NULL;
	m_AABBTraceRt.pModelOBB		= NULL;
	m_AABBTraceRt.pRootModel	= NULL;
}

A3DModel::~A3DModel()
{
}

bool A3DModel::AddChildFrame(A3DFrame * pFrame)
{
	if( m_nFrameCount > pFrame->GetFrameCountRecursive() )
	{
		if( pFrame->GetFrameCountRecursive() != 1 )
			return false;
	}
	else if( m_nFrameCount < pFrame->GetFrameCountRecursive() )
	{
		if( m_nFrameCount != 1 )
			return false;
		else 
			m_nFrameCount = pFrame->GetFrameCountRecursive();
	}
	m_ChildFrameList.Append((LPVOID) pFrame);

	// Increase vert count and index count;
	m_nVertCount += pFrame->GetVertCount();
	m_nIndexCount += pFrame->GetIndexCount();

	//Now add all child frame's bounding box into this model;
	m_bHasMoved = true;
	return RetrieveFrameOBB(pFrame);
}

/*
	Retrieve child frame's bounding box recursively and add them into this model's
	OBB List;
*/
bool A3DModel::RetrieveFrameOBB(A3DFrame * pFrame)
{
	for(int i=0; i<pFrame->GetFrameOBBNum(); i++)
	{
		A3DFRAMEOBB frameOBB = pFrame->GetFrameOBB(i);

		MODELOBB* pModelOBB = (MODELOBB*)a_malloc(sizeof (MODELOBB));
		if( NULL == pModelOBB )
		{
			g_A3DErrLog.Log("A3DModel::RetrieveFrameOBB Not enough memory!");
			return false;
		}
		pModelOBB->frameOBB = frameOBB;
		pModelOBB->pFrame = pFrame;

		m_ModelOBBList.Append((MODELOBB*)pModelOBB, NULL);
	}

	A3DFrame * pChildFrame = pFrame->GetFirstChildFrame();
	while(pChildFrame)
	{
		if( !RetrieveFrameOBB(pChildFrame) )
			return false;
		pChildFrame = pFrame->GetNextChildFrame();
	}
	return true;
}

bool A3DModel::AddChildModel(A3DModel * pModel, const char * pszParentFrameName, const char * pszConnectionCenterFrameName, ALISTELEMENT ** ppElement)
{
#ifdef _DEBUG
	if( pModel->GetRealParentModel() != NULL )
	{
		assert(false);
		if( pModel->GetRealParentModel() == this )
			g_A3DErrLog.Log("A3DModel::AddChildModel(), The model [%s] will be added into model [%s] more than once!", pModel->GetName(), GetName());
		else
			g_A3DErrLog.Log("A3DModel::AddChildModel(), The model [%s] will be added into model [%s] while it is still the child of model [%s]!", pModel->GetName(), GetName(), pModel->GetRealParentModel()->GetName());
		return false;
	}
#endif
	pModel->SetRealParentModel(this);

	m_ChildModelList.Append((LPVOID) pModel, ppElement);
	if( pszParentFrameName )
	{
		A3DFrame * pParentFrame = FindChildFrameByName(pszParentFrameName);
		if( NULL == pParentFrame )
		{
			g_A3DErrLog.Log("A3DModel::AddChildModel(), The parent frame [%s] can not be found in model [%s]!", pszParentFrameName, GetName());
			return false;
		}
		pModel->SetParentFrame(pParentFrame);
	}
	else
		pModel->SetParentModel(this);

	if( !pModel->SetConnectionCenterFrame(pszConnectionCenterFrameName) )
		return false;

	pModel->SetPosInParent(A3DVECTOR3(0.0f));
	pModel->UpdateToFrame(0);
	pModel->UpdateModelOBB();

	// Increate vert count and index count;
	m_nVertCount += pModel->GetVertCount();
	m_nIndexCount += pModel->GetIndexCount();

	return true;
}

bool A3DModel::SetParentModel(A3DModel * pParentModel)
{
	m_pParentModel = pParentModel;

	return true;
}

bool A3DModel::SetParentFrame(A3DFrame * pParentFrame)
{
	m_pParentFrame = pParentFrame;
	
	return true;
}

bool A3DModel::SetConnectionCenterFrame(const char * pszConnectionCenterFrameName)
{
	if( pszConnectionCenterFrameName )
	{
		m_pConnectionCenterFrame = FindChildFrameByName(pszConnectionCenterFrameName);
		if( NULL == m_pConnectionCenterFrame )
		{
			g_A3DErrLog.Log("A3DModel::SetConnectionCenterFrame() Can not find connector frame[%s]", pszConnectionCenterFrameName);
			return false;
		}
	}
	else
		m_pConnectionCenterFrame = NULL;
	return true;
}

bool A3DModel::Init(A3DDevice * pDevice, bool bDuplicated)
{
	m_pA3DDevice = pDevice;

	if( !bDuplicated )
	{
		// This is the original models, so create childframelist and action and group list;
		m_pActionGroupList = new AList();
		if( NULL == m_pActionGroupList )
		{
			g_A3DErrLog.Log("A3DModel::Load() Not enough memory!");
			return false;
		}
		m_pActionGroupList->Init();

		m_pActionDefinitionList = new AList();
		if( NULL == m_pActionDefinitionList )
		{
			g_A3DErrLog.Log("A3DModel::Load() Not enough memory!");
			return false;
		}
		m_pActionDefinitionList->Init();
	}

	m_ActionScheduleList.Init();

	m_GFXEventList.Init();
	m_GFXList.Init();

	m_SFXEventList.Init();
	m_SFXList.Init();

	m_LogicEventList.Init();

	m_bHasMoved = true;
	m_ModelOBBList.Init();

	m_ChildFrameList.Init();
	m_ChildModelList.Init();

	m_matRelativeTM.Identity();
	m_matAbsoluteTM.Identity();

	m_nVertCount = 0;
	m_nIndexCount = 0;

	m_vTimeStored = 0.0f;
	m_nFrameCount = 1;
	m_nFrame = 0;
	m_nFrameOld = -1;
	m_nHeartBeats = -1;
	m_nAnimStart = m_nAnimEnd = 0;
	m_bPaused = false;
	m_bAnimLoop = true;

	m_vecPos = A3DVECTOR3(0.0f);
	m_vecDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUp  = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecVelocity = A3DVECTOR3(0.0f);
	m_vecScale = A3DVECTOR3(1.0f, 1.0f, 1.0f);
	
	m_bVisible = true;
	m_bDuplicatedOne = false;
	m_pfnActionChangeCallBack	= NULL;
	m_pfnActionLoopEndCallBack	= NULL;
	m_pfnLogicEventCallBack		= NULL;
	m_pActionChangeArg			= NULL;
	m_pActionLoopArg			= NULL;
	m_pLogicArg					= NULL;
	
	m_bBuildOBBBevels = true;
	return true;
}

bool A3DModel::Release()
{
	//g_A3DErrLog.Log("%s, A3DModel::Release()", GetName());
	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_MODEL);

	assert(m_Container == A3DCONTAINER_NULL);

	ReleasePredefinedAction();

	if( !m_bDuplicatedOne )
	{
		m_pActionDefinitionList->Release();
		delete m_pActionDefinitionList;
		m_pActionDefinitionList = NULL;

		m_pActionGroupList->Release();
		delete m_pActionGroupList;
		m_pActionGroupList = NULL;
	}
	
	m_ActionScheduleList.Release();

	// We must release my child frame here;
	ALISTELEMENT * pChildFrameElement = m_ChildFrameList.GetFirst();
	while( pChildFrameElement != m_ChildFrameList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pChildFrameElement->pData;

		m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(pFrame);
		pChildFrameElement = pChildFrameElement->pNext;
	}
	m_ChildFrameList.Release();

	// Stop all gfx and let them stop died;
	ALISTELEMENT * pGFXElement = m_GFXList.GetFirst();
	while( pGFXElement != m_GFXList.GetTail() )
	{
		GFXRECORD* pGFXRecord = (GFXRECORD*) pGFXElement->pData;
		A3DGraphicsFX * pGFX = pGFXRecord->pGFX;

		// We must remove this gfx from gfx man and release it;
		if( m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->IsGFXAlive(pGFXRecord->pGFX) )
		{
			pGFX->Stop(true);
		}

		a_free(pGFXRecord);
		pGFXElement = pGFXElement->pNext;
	}
	m_GFXList.Release();

	// Release all gfx event's gfx handle;
	ALISTELEMENT * pGFXEventElement = m_GFXEventList.GetFirst();
	while( pGFXEventElement != m_GFXEventList.GetTail() )
	{			
		GFXEVENT* pGFXEvent = (GFXEVENT*) pGFXEventElement->pData;

		// If it not playing now, we just release it;
		// But it is playing now, we should let it die by itself;
		if( m_bDuplicatedOne )
			m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->ReleaseGFX(pGFXEvent->pGFX);

		a_free(pGFXEvent);
		pGFXEventElement = pGFXEventElement->pNext;
	}
	m_GFXEventList.Release();

	// Release all sfx event's sfx handle;
	ALISTELEMENT * pSFXEventElement = m_SFXEventList.GetFirst();
	while( pSFXEventElement != m_SFXEventList.GetTail() )
	{
		SFXEVENT* pSFXEvent = (SFXEVENT*) pSFXEventElement->pData;

		a_free(pSFXEvent);
		pSFXEventElement = pSFXEventElement->pNext;
	}
	m_SFXEventList.Release();

	ALISTELEMENT * pSFXRecordElement = m_SFXList.GetFirst();
	while( pSFXRecordElement != m_SFXList.GetTail() )
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*) pSFXRecordElement->pData;

		if (pSFXRecord->pSFX)
		{
			pSFXRecord->pSFX->Stop();
			m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSFXRecord->pSFX);
		}

		a_free(pSFXRecord);
		pSFXRecordElement = pSFXRecordElement->pNext;
	}
	m_SFXList.Release();

	// Release all logic events
	ALISTELEMENT * pLogicEventElement = m_LogicEventList.GetFirst();
	while( pLogicEventElement != m_LogicEventList.GetTail() )
	{
		LOGICEVENT* pLogicEvent = (LOGICEVENT*) pLogicEventElement->pData;

		a_free(pLogicEvent);
		pLogicEventElement = pLogicEventElement->pNext;
	}
	m_LogicEventList.Release();

	//Rlease OBB Datas;
	ALISTELEMENT * pOBBElement = m_ModelOBBList.GetFirst();
	while( pOBBElement != m_ModelOBBList.GetTail() )
	{
		MODELOBB* pOBB = (MODELOBB*) pOBBElement->pData;
		a_free(pOBB);
		pOBBElement = pOBBElement->pNext;
	}
	
	m_ModelOBBList.Release();
	m_ChildModelList.Release();
	return true;
}

bool A3DModel::Render(A3DViewport * pCurrentViewport, bool bNeedSort, bool bNeedCollect)
{
	//	If this object is created outside D3D;
	if (!m_pA3DDevice || g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		return true;

	if (!m_bVisible)
		return true;

	//	Check whether model is visible
	A3DCDS* pCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
	if (pCDS)
	{
		if (!pCDS->AABBIsVisible(m_ModelAABB, VISOBJ_MODEL, (DWORD)this))
			return true;
	}
	else if (!pCurrentViewport->GetCamera()->AABBInViewFrustum(m_ModelAABB))
		return true;

//	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	float vFrontOld;
	A3DCameraBase* pCamera = pCurrentViewport->GetCamera();

	// If Pull Z flag is set, we must set z-bias of the camera and render all this model 
	// without collector;
	if (m_bZPull && pCamera->GetClassID() == A3D_CID_CAMERA)
	{
		vFrontOld = pCamera->GetZFront();
		float fZBack = pCamera->GetZBack();
		pCamera->SetZFrontAndBack(0.1f, fZBack);
		pCamera->SetZBias(0.3f);
		bNeedSort = false;
	}

	//Before we render, we should move to that frame;
	UpdateToFrame(m_nFrame);

	//	First of all, we can show my bounding box here;
	if (g_pA3DConfig->RT_GetShowBoundBoxFlag())
	{
		A3DWireCollector* pWireCollector = m_pA3DDevice->GetA3DEngine()->GetA3DWireCollector();
		pWireCollector->AddAABB(m_ModelAABB, A3DCOLORRGBA(255, 0, 0, 255));

	#ifdef ENABLE_MODELOBB
		pWireCollector->AddOBB(m_ModelOBB, A3DCOLORRGBA(0, 255, 255, 255));
	#endif
	}

	//We just use the precalculated world transform matrix;
	m_pA3DDevice->SetWorldMatrix(m_matAbsoluteTM);

	//First, render my child frames;
	if( m_ChildFrameList.GetSize() )
	{
		ALISTELEMENT * pThisChildElement = m_ChildFrameList.GetHead()->pNext;
		while( pThisChildElement != m_ChildFrameList.GetTail() )
		{
			A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
			if( !pFrame->Render(pCurrentViewport, bNeedSort, bNeedCollect) )	return false;
			pThisChildElement = pThisChildElement->pNext;
		}
	}

	//Then, render my child models;
	if( m_ChildModelList.GetSize() )
	{
		ALISTELEMENT * pThisModelElement = m_ChildModelList.GetHead()->pNext;
		while( pThisModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
			if( !pModel->Render(pCurrentViewport, bNeedSort, bNeedCollect) )	return false;
			pThisModelElement = pThisModelElement->pNext;
		}
	}

	if (m_bZPull && pCamera->GetClassID() == A3D_CID_CAMERA)
	{
		float fZBack = pCamera->GetZBack();
		pCurrentViewport->GetCamera()->SetZFrontAndBack(vFrontOld, fZBack);
		pCurrentViewport->GetCamera()->SetZBias(1.0f);
	}

//	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);
	return true;
}

bool A3DModel::PauseAnimation(bool bPause)
{
	m_bPaused = bPause;

	if( m_ChildModelList.GetSize() )
	{
		ALISTELEMENT * pThisModelElement = m_ChildModelList.GetHead()->pNext;
		while( pThisModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
			if( !pModel->PauseAnimation(bPause) )
				return false;
			pThisModelElement = pThisModelElement->pNext;
		}
	}
	return true;
}

bool A3DModel::TickAnimation(FLOAT vDeltaTime)
{
	//This function is used to loop through m_nAnimStart and m_nAnimEnd;
	if( !m_bPaused )
	{
		if( m_bAnimLoop )
		{
			m_nFrame ++;
			if( m_nFrame >= m_nAnimEnd )
			{
				// Loop's ending point has reached!
				if( m_pfnActionLoopEndCallBack )
					(*m_pfnActionLoopEndCallBack)(this, m_pActionLoopArg);
			}
			m_nFrame = m_nAnimStart + ((m_nFrame - m_nAnimStart) % (m_nAnimEnd - m_nAnimStart + 1));
		}
		else //not loop animation;
		{
			m_nFrame ++;
			if( m_nFrame >= m_nAnimEnd )
			{
				//See if there is some scheduled actions in the list;
				if( !HasScheduledAction() )
				{
					m_nFrame = m_nAnimEnd;
					// If not scheduled, we must use action loop end call back here;
					if( m_pfnActionLoopEndCallBack )
						(*m_pfnActionLoopEndCallBack)(this, m_pActionLoopArg);
				}
				else
				{
					A3DACTION * pNextAction = PopAction();
					SetAction(pNextAction);
					if( m_pfnActionChangeCallBack )
					{
						(*m_pfnActionChangeCallBack)(pNextAction, m_pActionChangeArg);
					}
				}
			}
		}
	}

	//Before I call my child's TickAnimation, all my frame must be correct;
	UpdateToFrame(m_nFrame);

	if( m_nHeartBeats == - 1 )
	{
		// For the first time we should check if there is some effects need start 
		// at the very beginning;
		RestartGFXEvents();
		/*
		ALISTELEMENT * pThisGFXEventElement = m_GFXEventList.GetFirst();
		while( pThisGFXEventElement != m_GFXEventList.GetTail() )
		{
			GFXEVENT * pGFXEvent = (GFXEVENT *) pThisGFXEventElement->pData;
			if( pGFXEvent->nFrame == -1 )
			{
				if( !AddGFX(pGFXEvent->pGFX, pGFXEvent->bLinked, pGFXEvent->vScale, pGFXEvent->szParentName, 
					pGFXEvent->vecPos, pGFXEvent->vecDir, pGFXEvent->vecUp, pGFXEvent) )
				{
					g_A3DErrLog.Log("A3DModel::RestartEvents() AddGFX Fail!");
					return false;
				}
			}
			pThisGFXEventElement = pThisGFXEventElement->pNext;
		}*/
		RestartSFXEvents();	
	}

	if( !m_bPaused )
	{
		// First of all check gfx that needs playing;
		CheckGFXEvents();
		CheckSFXEvents();
		CheckLogicEvents();
	}

	//Then we must let our child animated too;
	if( m_ChildModelList.GetSize() )
	{
		ALISTELEMENT * pThisModelElement = m_ChildModelList.GetHead()->pNext;
		while( pThisModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
			if( !pModel->TickAnimation() )
				return false;
			pThisModelElement = pThisModelElement->pNext;
		}
	}

	//We must let all gfx know that I will tick into next animation
	if( m_GFXList.GetSize() )
	{
		// These gfx will need some process here;
		ALISTELEMENT * pThisGFXElement = m_GFXList.GetFirst();
		while( pThisGFXElement != m_GFXList.GetTail() )
		{
			GFXRECORD* pGFXRecord = (GFXRECORD*)pThisGFXElement->pData;
			A3DGraphicsFX * pGFX = pGFXRecord->pGFX;

			if( pGFX->IsExpired() )
			{
				ALISTELEMENT * pTemp = pThisGFXElement;
				pThisGFXElement = pThisGFXElement->pNext;
				m_GFXList.Delete(pTemp);

				RemoveGFX(pGFXRecord);
				continue;
			}
			pGFX->UpdateParentInfo();

			pThisGFXElement = pThisGFXElement->pNext;
		}
	}

	//We must let all sfx know that I will change my position;
	// We may create another list for all sfxs;
	if( m_SFXList.GetSize() )
	{
		ALISTELEMENT * pThisSFXElement = m_SFXList.GetFirst();
		while( pThisSFXElement != m_SFXList.GetTail() )
		{
			SFXRECORD* pSFXRecord = (SFXRECORD*) pThisSFXElement->pData;
			AM3DSoundBuffer* pSFX = pSFXRecord->pSFX;

			if( pSFX && pSFX->IsPlaying() && pSFXRecord->pCurEvent )
			{
				SFXEVENT* pSFXEvent = pSFXRecord->pCurEvent;

				A3DVECTOR3 vecPos, vecDir;
				if( pSFXEvent->pParentFrame )
				{
					A3DMATRIX4 matParent = pSFXEvent->pParentFrame->GetAbsoluteTM();
					vecPos = pSFXEvent->vecPos * matParent;
					vecDir = pSFXEvent->vecDir * matParent - matParent.GetRow(3);
				}
				pSFX->SetPosition(vecPos);
				pSFX->SetConeOrientation(vecDir);
				pSFX->SetVelocity(m_vecVelocity);
				pSFX->UpdateChanges();

				pSFX->CheckEnd();
			}
			
			pThisSFXElement = pThisSFXElement->pNext;
		}
	}

	//Now Update Frame(Model) OBB List;
	if( !UpdateModelOBB() )
		return false;

	m_bHasMoved = false;
	m_nFrameOld = m_nFrame;

	++ m_nHeartBeats;

	return true;
}

bool A3DModel::UpdateAllInfos()
{
	UpdateToFrame(-1);

	//Then we must let our child animated too;
	if( m_ChildModelList.GetSize() )
	{
		ALISTELEMENT * pThisModelElement = m_ChildModelList.GetHead()->pNext;
		while( pThisModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
			if( !pModel->UpdateAllInfos() )	return false;
			pThisModelElement = pThisModelElement->pNext;
		}
	}

	//Now Update Frame(Model) OBB List;
	if( !UpdateModelOBB() )
		return false;
	return true;
}

bool A3DModel::Save(AFile * pFileToSave)
{
	//We should save the action definition list here;
	if( pFileToSave->IsBinary() )
	{

	}	
	else
	{
		char	szLineBuffer[AFILE_LINEMAXLEN];
		int		nChildFrameCount, nChildModelCount, nActionCount, nGFXEventCount, nSFXEventCount, nLogicEventCount;
		int		i;
		DWORD	dwWriteLength;

		//	Write version
		sprintf(szLineBuffer, "VERSION: %d", MODELVERSION);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "MODEL: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		//save my child frames;
		sprintf(szLineBuffer, "CHILDFRAME: %d", m_ChildFrameList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);
						 	  
		nChildFrameCount = m_ChildFrameList.GetSize();
		if( nChildFrameCount )
		{
			pFileToSave->WriteLine("{");

			for(i=0; i<nChildFrameCount; i++)
			{
				ALISTELEMENT * pChildFrameElement = m_ChildFrameList.GetElementByOrder(i);
				A3DFrame * pChildFrame = (A3DFrame *) pChildFrameElement->pData;
				sprintf(szLineBuffer, "%s", pChildFrame->GetName());
				pFileToSave->WriteLine(szLineBuffer);
			}
			pFileToSave->WriteLine("}");
		}
			
		//Save my child models;
		sprintf(szLineBuffer, "CHILDMODEL: %d", m_ChildModelList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		nChildModelCount = m_ChildModelList.GetSize();
		if( nChildModelCount )
		{
			pFileToSave->WriteLine("{");
			for(i=0; i<nChildModelCount; i++)
			{
				ALISTELEMENT * pChildModelElement = m_ChildModelList.GetElementByOrder(i);
				A3DModel * pChildModel = (A3DModel *) pChildModelElement->pData;
				sprintf(szLineBuffer, "%s", pChildModel->GetName());
				pFileToSave->WriteLine(szLineBuffer);
			}
			pFileToSave->WriteLine("}");
		}

		sprintf(szLineBuffer, "ACTIONS: %d", m_pActionDefinitionList->GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		nActionCount = m_pActionDefinitionList->GetSize();
		if( nActionCount )
		{
			pFileToSave->WriteLine("{");			
			for(i=0; i<nActionCount; i++)
			{
				ALISTELEMENT * pActionElement = m_pActionDefinitionList->GetElementByOrder(i);
				A3DACTION * pAction = (A3DACTION *) pActionElement->pData;

				sprintf(szLineBuffer, "%s", pAction->szName);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%d, %d, %d", pAction->nAnimStart, pAction->nAnimEnd, pAction->bAnimLoop);
				pFileToSave->WriteLine(szLineBuffer);
			}
			pFileToSave->WriteLine("}");			
		}

		sprintf(szLineBuffer, "GROUPACTIONS: %d", m_pActionGroupList->GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		nActionCount = m_pActionGroupList->GetSize();
		if( nActionCount )
		{
			pFileToSave->WriteLine("{");			
			for(i=0; i<nActionCount; i++)
			{
				ALISTELEMENT * pGroupActionElement = m_pActionGroupList->GetElementByOrder(i);
				A3DGROUPACTION * pGroupAction = (A3DGROUPACTION *) pGroupActionElement->pData;

				sprintf(szLineBuffer, "%s", pGroupAction->szName);
				pFileToSave->WriteLine(szLineBuffer);
				for(int j=0; j<A3D_GROUP_ACTION_MAX_ELEMENT; j++)
				{
					if( j < pGroupAction->nActionNum )
						sprintf(szLineBuffer, "%s", pGroupAction->pActionElement[j]->szName);
					else
						strcpy(szLineBuffer, "...");
					pFileToSave->WriteLine(szLineBuffer);
				}
			}
			pFileToSave->WriteLine("}");			
		}
		// <== GROUPACTIONS: %s { 
		pFileToSave->WriteLine("}");			

		sprintf(szLineBuffer, "GFXEVENT: %d", m_GFXEventList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		nGFXEventCount = m_GFXEventList.GetSize();
		if( nGFXEventCount )
		{
			pFileToSave->WriteLine("{");			
			for(i=0; i<nGFXEventCount; i++)
			{
				ALISTELEMENT * pGFXEventElement = m_GFXEventList.GetElementByOrder(i);
				GFXEVENT* pGFXEvent = (GFXEVENT*) pGFXEventElement->pData;

				sprintf(szLineBuffer, "%s", pGFXEvent->szGFXName);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%d", pGFXEvent->nFrame);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%d", pGFXEvent->bLinked);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%f", pGFXEvent->vScale);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%s", pGFXEvent->szParentName);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "(%f, %f, %f)", pGFXEvent->vecPos.x, pGFXEvent->vecPos.y, pGFXEvent->vecPos.z);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "(%f, %f, %f)", pGFXEvent->vecDir.x, pGFXEvent->vecDir.y, pGFXEvent->vecDir.z);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "(%f, %f, %f)", pGFXEvent->vecUp.x, pGFXEvent->vecUp.y, pGFXEvent->vecUp.z);
				pFileToSave->WriteLine(szLineBuffer);
			}
			pFileToSave->WriteLine("}");			
		}

		sprintf(szLineBuffer, "SFXEVENT: %d", m_SFXEventList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		nSFXEventCount = m_SFXEventList.GetSize();
		if( nSFXEventCount )
		{
			pFileToSave->WriteLine("{");			
			for(i=0; i<nSFXEventCount; i++)
			{
				ALISTELEMENT * pSFXEventElement = m_SFXEventList.GetElementByOrder(i);
				SFXEVENT* pSFXEvent = (SFXEVENT*) pSFXEventElement->pData;

				sprintf(szLineBuffer, "%s", pSFXEvent->szSFXName);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%d", pSFXEvent->nFrame);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%d", pSFXEvent->bLoop);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%d", pSFXEvent->b2DSound);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%f", pSFXEvent->vMinDis);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%f", pSFXEvent->vMaxDis);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%f", pSFXEvent->vVolume);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%s", pSFXEvent->szParentName);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "(%f, %f, %f)", pSFXEvent->vecPos.x, pSFXEvent->vecPos.y, pSFXEvent->vecPos.z);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "(%f, %f, %f)", pSFXEvent->vecDir.x, pSFXEvent->vecDir.y, pSFXEvent->vecDir.z);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "(%f, %f, %f)", pSFXEvent->vecUp.x, pSFXEvent->vecUp.y, pSFXEvent->vecUp.z);
				pFileToSave->WriteLine(szLineBuffer);
			}
			pFileToSave->WriteLine("}");			
		}
		
		sprintf(szLineBuffer, "LOGICEVENT: %d", m_LogicEventList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		nLogicEventCount = m_LogicEventList.GetSize();
		if( nLogicEventCount )
		{
			pFileToSave->WriteLine("{");			
			for(i=0; i<nLogicEventCount; i++)
			{
				ALISTELEMENT * pLogicEventElement = m_LogicEventList.GetElementByOrder(i);
				LOGICEVENT* pLogicEvent = (LOGICEVENT*) pLogicEventElement->pData;

				sprintf(szLineBuffer, "%d", pLogicEvent->nFrame);
				pFileToSave->WriteLine(szLineBuffer);
				sprintf(szLineBuffer, "%s", pLogicEvent->szNotifyString);
				pFileToSave->WriteLine(szLineBuffer);
			}
			pFileToSave->WriteLine("}");			
		}

		pFileToSave->WriteLine("USER_PROPERTY DO NOT EDIT");
		//Attached is a user defined property buffer, but maybe the property has not be saved yet;
		pFileToSave->Write(m_pProperties, PROPERTY_SIZE, &dwWriteLength);
	}
	return true;
}

bool A3DModel::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if (!af_CheckFileExt(pFileToLoad->GetFileName(), ".mod", 4))
	{
		//Errlog;
		g_A3DErrLog.Log("A3DModel::Load(), Error extension [%s]", pFileToLoad->GetFileName());
		return false;
	}
	
	if (!Init(pA3DDevice, false))
		return false;

	int iVersion = 1;

	//	We should load the action definition list here;
	if( pFileToLoad->IsBinary() )
	{

	}
	else
	{
		char	szLineBuffer[AFILE_LINEMAXLEN];
		char	szResult[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		int     nChildFrameCount, nChildModelCount, nActionCount, nGFXEventCount, nSFXEventCount, nLogicEventCount;
		int		i;

		//	Load version
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if (a_GetStringAfter(szLineBuffer, "VERSION: ", szResult))
		{
			iVersion = atoi(szResult);
			pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLen);
		}

		a_GetStringAfter(szLineBuffer, "MODEL: ", szResult);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if (strcmp(szLineBuffer, "{"))
			return false;

		//Load my child frames;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "CHILDFRAME: ", szResult);
		nChildFrameCount = atoi(szResult);
		if( nChildFrameCount )
		{
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
				return false;
			for(i=0; i<nChildFrameCount; i++)
			{
				A3DFrame * pChildFrame = NULL;
				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);

				// For current, we use only bare name of the mox file
				// But the mox file exists in some folder, now is same as model file
				// In future, mox file will contain a relative path? No, We'd better not
				// So this solution is best, the only thing to do is to modify the 
				// function call to let the relative path of modeljust passed in;
				// But please note we have to set the frame's name to the bare one;
				char szModelName[MAX_PATH];
				char szMoxName[MAX_PATH];
				char szModelPath[MAX_PATH];

				af_GetRelativePathNoBase(pFileToLoad->GetRelativeName(), 
					m_pA3DDevice->GetA3DEngine()->GetA3DModelMan()->GetFolderName(), szModelName);
				af_GetFilePath(szModelName, szModelPath, MAX_PATH);
				if( szModelPath[0] )
					sprintf(szMoxName, "%s\\%s", szModelPath, szLineBuffer);
				else
					strcpy(szMoxName, szLineBuffer);

				if( !pA3DDevice )
				{
					//Used out of A3D Engine, so we have to new a A3DFrame here;
					pChildFrame = new A3DFrame();
					if( NULL == pChildFrame )
					{
						g_A3DErrLog.Log("A3DModel::Load Not enough memory!");
						return false;
					}

					AFile modelFile;
					if( !modelFile.Open(szMoxName, AFILE_OPENEXIST) )
					{
						delete pChildFrame;
						g_A3DErrLog.Log("A3DModel::Load() Can not open file %s", szLineBuffer);
						return false;
					}
					if( !pChildFrame->Load(pA3DDevice, &modelFile) )
					{
						pChildFrame->Release();
						delete pChildFrame;
						g_A3DErrLog.Log("A3DModel::Load() Child Frame load fail!");
						return false;
					}
					modelFile.Close();
				}
				else
				{
					if( !m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->LoadMoxFile(szMoxName, &pChildFrame) )
					{
						g_A3DErrLog.Log("A3DModel::Load Can't load child mox file: %s", szLineBuffer);
						return false;
					}
				}
				pChildFrame->SetName(szLineBuffer);
				if( !AddChildFrame(pChildFrame) )
					return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}

		// If the device has not be created, we shall not process any more!
		if( !m_pA3DDevice )
			return true;

		//Load my child models;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "CHILDMODEL: ", szResult);
		nChildModelCount = atoi(szResult);
		if( nChildModelCount )
		{
			//This is not supported for some time......
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
				return false;
			for(i=0; i<nChildModelCount; i++)
			{
				A3DModel * pChildModel = NULL;
				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				if( !m_pA3DDevice->GetA3DEngine()->GetA3DModelMan()->LoadModelFile(szLineBuffer, &pChildModel, true) )
				{
					g_A3DErrLog.Log("A3DModel::Load Can't load child model file: %s", szLineBuffer);
					return false;
				}
				if( !AddChildModel(pChildModel) )
					return false;
				if( !pChildModel->SetParentModel(this) )
					return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "ACTIONS: ", szResult);
		nActionCount = atoi(szResult);
		if( nActionCount )
		{
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
				return false;
			for(i=0; i<nActionCount; i++)
			{
				char szName[32];
				int nAnimStart, nAnimEnd, nLoop;
				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				strncpy(szName, szLineBuffer, 32);
				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "%d, %d, %d", &nAnimStart, &nAnimEnd, &nLoop);
				if( !DefineAction(NULL, szName, nAnimStart, nAnimEnd, nLoop ? true : false) )
					return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "GROUPACTIONS: ", szResult);
		nActionCount = atoi(szResult);
		if( nActionCount )
		{
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
				return false;
			for(i=0; i<nActionCount; i++)
			{
				char szName[32];
				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				strncpy(szName, szLineBuffer, 32);
				if( !AddGroupAction(NULL, szName, "") )
					return false;
				for(int j=0; j<A3D_GROUP_ACTION_MAX_ELEMENT; j++)
				{
					char szActionName[32];
					pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
					strncpy(szActionName, szLineBuffer, 32);
					if( _stricmp(szActionName, "...") == 0 )//Empty Slot
						continue;
					
					if( !AddGroupAction(NULL, szName, szActionName) )
						return false;
				}
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}

		// <== MODEL: %s { 
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;

		//See if there is an extra user defined property data;
		if( pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen) )
		{
			if( _stricmp(szLineBuffer, "USER_PROPERTY DO NOT EDIT") == 0 )
			{
				//Attached is a user defined property buffer, but maybe the property has not be saved yet;
				pFileToLoad->Read(m_pProperties, PROPERTY_SIZE, &dwReadLen);
			}
			else
			{
				sscanf(szLineBuffer, "GFXEVENT: %d", &nGFXEventCount);
				if( nGFXEventCount )
				{
					pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
					if( strcmp(szLineBuffer, "{") )
						return false;
					for(i=0; i<nGFXEventCount; i++)
					{
						char szGFXFile[MAX_PATH];
						char szParentName[256];
						int  nval;
						int  nFrame;
						bool bLinked;
						FLOAT vScale;
						A3DVECTOR3 vecPos, vecDir, vecUp;

						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						strncpy(szGFXFile, szLineBuffer, MAX_PATH);
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLineBuffer, "%d", &nFrame);
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLineBuffer, "%d", &nval);
						bLinked = nval ? true : false;
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLineBuffer, "%f", &vScale);
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						strncpy(szParentName, szLineBuffer, 256);
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLineBuffer, "(%f, %f, %f)", &vecPos.x, &vecPos.y, &vecPos.z);
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLineBuffer, "(%f, %f, %f)", &vecDir.x, &vecDir.y, &vecDir.z);
						pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLineBuffer, "(%f, %f, %f)", &vecUp.x, &vecUp.y, &vecUp.z);

						if( !AddGFXEvent(nFrame, szGFXFile, bLinked, vScale, szParentName, vecPos, vecDir, vecUp) )
						{
							g_A3DErrLog.Log("A3DModel::Load() AddGFXEvent Fail!");
							return false;
						}
					}
					// <== GFXEVENT: %s { 
					pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
					if( strcmp(szLineBuffer, "}") )
						return false;	
				}

				if( pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen) )
				{
					if( _stricmp(szLineBuffer, "USER_PROPERTY DO NOT EDIT") == 0 )
					{
						//Attached is a user defined property buffer, but maybe the property has not be saved yet;
						pFileToLoad->Read(m_pProperties, PROPERTY_SIZE, &dwReadLen);
					}
					else
					{
						sscanf(szLineBuffer, "SFXEVENT: %d", &nSFXEventCount);
						if( nSFXEventCount )
						{
							pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
							if( strcmp(szLineBuffer, "{") )
								return false;
							for(i=0; i<nSFXEventCount; i++)
							{
								char szSFXFile[MAX_PATH];
								char szParentName[256];
								int  nval;
								int  nFrame;
								bool bLoop;
								bool b2DSound;
								FLOAT vMinDis, vMaxDis;
								FLOAT vVolume;
								A3DVECTOR3 vecPos, vecDir, vecUp;

								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								if (iVersion > 1)
									strncpy(szSFXFile, szLineBuffer, MAX_PATH);
								else	//	iVersion == 1
									sprintf(szSFXFile, "sfx\\%s", szLineBuffer);

								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "%d", &nFrame);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "%d", &nval);
								bLoop = nval ? true : false;
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "%d", &nval);
								b2DSound = nval ? true : false;
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "%f", &vMinDis);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "%f", &vMaxDis);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "%f", &vVolume);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								strncpy(szParentName, szLineBuffer, 256);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "(%f, %f, %f)", &vecPos.x, &vecPos.y, &vecPos.z);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "(%f, %f, %f)", &vecDir.x, &vecDir.y, &vecDir.z);
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								sscanf(szLineBuffer, "(%f, %f, %f)", &vecUp.x, &vecUp.y, &vecUp.z);

								if( !AddSFXEvent(nFrame, szSFXFile, bLoop, b2DSound, vMinDis, vMaxDis, vVolume, szParentName, vecPos, vecDir, vecUp) )
								{
									g_A3DErrLog.Log("A3DModel::Load() AddSFXEvent Fail!");
									return false;
								}
							}
							// <== SFXEVENT: %s { 
							pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
							if( strcmp(szLineBuffer, "}") )
								return false;	
						}
					}

					if( pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen) )
					{
						if( _stricmp(szLineBuffer, "USER_PROPERTY DO NOT EDIT") == 0 )
						{
							//Attached is a user defined property buffer, but maybe the property has not be saved yet;
							pFileToLoad->Read(m_pProperties, PROPERTY_SIZE, &dwReadLen);
						}
						else
						{
							sscanf(szLineBuffer, "LOGICEVENT: %d", &nLogicEventCount);
							if( nLogicEventCount )
							{
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								if( strcmp(szLineBuffer, "{") )
									return false;
								for(i=0; i<nLogicEventCount; i++)
								{
									char szNotifyString[256];
									int  nFrame;
									
									pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
									nFrame = atoi(szLineBuffer);
									pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
									strncpy(szNotifyString, szLineBuffer, 256);
									
									if( !AddLogicEvent(nFrame, szNotifyString) )
									{
										g_A3DErrLog.Log("A3DModel::Load() AddLogicEvent Fail!");
										return false;
									}
								}
								// <== LOGICEVENT: %s { 
								pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
								if( strcmp(szLineBuffer, "}") )
									return false;	
							}

							if( pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen) )
							{
								if( _stricmp(szLineBuffer, "USER_PROPERTY DO NOT EDIT") == 0 )
								{
									//Attached is a user defined property buffer, but maybe the property has not be saved yet;
									pFileToLoad->Read(m_pProperties, PROPERTY_SIZE, &dwReadLen);
								}
							}
						}
					}
				}
			}
		}
	}

	//Now Update Frame(Model) OBB List;
	//Update First Frame; ?????
	UpdateToFrame(0);
	if( !UpdateModelOBB() )
		return false;

	//g_A3DErrLog.Log("%s, A3DModel::Load()", pFileToLoad->GetRelativeName());
	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_MODEL);
	return true;
}

// Note: This function should not call recursive into child model, becuase all fucntions that calling this 
// Function is recursive called;
// This function only make this model's frame and mesh to the correct frame position;
bool A3DModel::UpdateToFrame(int nFrame)
{
	if( nFrame == -1 )
		nFrame = m_nFrame;

	if( 0 && nFrame == m_nFrameOld && !m_bHasMoved )
		return true;

	//This should be rewrote because we can only update the mesh and frame just
	//before we use it, for they are shared by all models;
	if( m_pParentModel )
		m_matAbsoluteTM = m_matRelativeTM * m_pParentModel->GetAbsoluteTM();
	else if( m_pParentFrame )
		m_matAbsoluteTM = m_matRelativeTM * m_pParentFrame->GetAbsoluteTM();
	
	//First, update my child frames;
	A3DMATRIX4 matParentTM = m_matAbsoluteTM;
	if( m_pConnectionCenterFrame )
	{
		// For connector frame, what we stored in RelativeTM is the InverseTM of AbsoluteTM;
		A3DMATRIX4 matConnector = m_pConnectionCenterFrame->GetRelativeTM(m_nFrame);
		matParentTM = matConnector * m_matAbsoluteTM;
	}

	ALISTELEMENT * pThisChildElement = m_ChildFrameList.GetFirst();
	while( pThisChildElement != m_ChildFrameList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
		if( !pFrame->UpdateToFrame(nFrame, &matParentTM) )	return false;
		pThisChildElement = pThisChildElement->pNext;
	}
	return true;
}

//Update the frame obb's transform matrix from all frames
//You must make sure that this function is called just after UpdateToFrame();
bool A3DModel::UpdateModelOBB()
{
	if( m_nFrameOld == m_nFrame && !m_bHasMoved && m_ChildModelList.GetSize() == 0 && m_pParentFrame == NULL && m_pParentModel == NULL)
		return true;

	A3DVECTOR3 vecOrg;

	//Now clear my AABB data;
	a3d_ClearAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs);
	ZeroMemory(&m_ModelOBB, sizeof(A3DOBB));

	if( m_ModelOBBList.GetSize() > 0 ) // Use ModelOBBList;
	{
		ALISTELEMENT * pModelOBBElement = m_ModelOBBList.GetFirst();
		while( pModelOBBElement != m_ModelOBBList.GetTail() )
		{
			MODELOBB* pModelOBB = (MODELOBB*) pModelOBBElement->pData;
			
			A3DMATRIX4	matTM;
			A3DOBB		bb = pModelOBB->frameOBB.a3dOBB;
			matTM = pModelOBB->tm = pModelOBB->pFrame->GetAbsoluteTM();
			matTM._41 = matTM._42 = matTM._43 = 0.0f;
			//Now update the obb according the frame's obb and this tm;
			pModelOBB->obb.Center = bb.Center * pModelOBB->tm;
			pModelOBB->obb.XAxis = bb.XAxis * matTM;
			pModelOBB->obb.YAxis = bb.YAxis * matTM;
			pModelOBB->obb.ZAxis = bb.ZAxis * matTM;
			pModelOBB->obb.Extents = bb.Extents;
			a3d_CompleteOBB(&pModelOBB->obb);

			a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, pModelOBB->obb);

#ifdef ENABLE_MODELOBB
			if( m_ModelOBB.Extents.x == 0.0f && m_ModelOBB.Extents.y == 0.0f && m_ModelOBB.Extents.z == 0.0f )
				m_ModelOBB = pModelOBB->obb;
			else
				m_ModelOBB = GetOBB(m_ModelOBB, pModelOBB->obb);
#endif
			//Then move to next element;
			pModelOBBElement = pModelOBBElement->pNext;
		}
	}
	else // Use automatically calculated obbs or aabbs;
	{
		ALISTELEMENT * pFrameElement = m_ChildFrameList.GetFirst();
		while( pFrameElement != m_ChildFrameList.GetTail() )
		{
			A3DFrame *	pChildFrame = (A3DFrame *) pFrameElement->pData;
			A3DMATRIX4	tm = pChildFrame->GetAbsoluteTM();
			A3DOBB		obb;
			A3DAABB		aabb;

			if( pChildFrame->IsEmptyFrame() ) 
				goto NEXTFRAME;

			// Now use automatic aabbs to expand aabb;
			aabb = pChildFrame->GetFrameAutoAABB(m_nFrame);
			obb.Center = aabb.Center * tm;
			obb.XAxis = tm.GetRow(0);
			obb.YAxis = tm.GetRow(1);
			obb.ZAxis = tm.GetRow(2);
			obb.Extents = aabb.Extents;
			a3d_CompleteOBB(&obb);
			a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, obb);

#ifdef ENABLE_MODELOBB
			//Now update the obb according the frame's obb and this tm;
			obb = pChildFrame->GetFrameAutoOBB(m_nFrame);
			obb.Center = obb.Center * tm;
			vecOrg = tm.GetRow(3);
			obb.XAxis = obb.XAxis * tm - vecOrg;
			obb.YAxis = obb.YAxis * tm - vecOrg;
			obb.ZAxis = obb.ZAxis * tm - vecOrg;
			obb.Extents = obb.Extents;
			a3d_CompleteOBB(&obb);

			if( m_ModelOBB.Extents.x == 0.0f && m_ModelOBB.Extents.x == 0.0f && m_ModelOBB.Extents.x == 0.0f )
				m_ModelOBB = obb;
			else
				m_ModelOBB = GetOBB(m_ModelOBB, obb);
#endif

NEXTFRAME:
			//Move to next child frame;
			pFrameElement = pFrameElement->pNext;
		}
	}

	//Expand my model aabb with my child's aabb;
	ALISTELEMENT * pChildElement = m_ChildModelList.GetFirst();
	while( pChildElement != m_ChildModelList.GetTail() )
	{
		A3DModel * pChildModel = (A3DModel *)pChildElement->pData;
	
		a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, pChildModel->GetModelAABB());

#ifdef ENABLE_MODELOBB
			if( m_ModelOBB.Extents.x == 0.0f && m_ModelOBB.Extents.x == 0.0f && m_ModelOBB.Extents.x == 0.0f )
				m_ModelOBB = pChildModel->GetModelOBB();
			else
				m_ModelOBB = GetOBB(m_ModelOBB, pChildModel->GetModelOBB());
#endif
		pChildElement = pChildElement->pNext;
	}

	a3d_CompleteAABB(&m_ModelAABB);

	m_bBuildOBBBevels = true;
	return true;
}

bool A3DModel::SetPos(A3DVECTOR3 vecPos)
{
	UpdateAbsoluteTM();

	m_vecPos = vecPos;

	m_matAbsoluteTM._41 = vecPos.x;
	m_matAbsoluteTM._42 = vecPos.y;
	m_matAbsoluteTM._43 = vecPos.z;
	
	UpdateRelativeTM();
	return true;
}

bool A3DModel::SetVelocity(A3DVECTOR3 vecVelocity)
{
	m_vecVelocity = vecVelocity;
	return true;
}

bool A3DModel::SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	UpdateAbsoluteTM();

	m_vecDir = vecDir;
	m_vecUp = vecUp;

	m_matAbsoluteTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);

	UpdateRelativeTM();
	return true;
}

bool A3DModel::Move(A3DVECTOR3 vecPosDelta)
{
	UpdateAbsoluteTM();

	m_vecPos = m_vecPos + vecPosDelta;

	m_matAbsoluteTM = m_matAbsoluteTM * a3d_Translate(vecPosDelta.x, vecPosDelta.y, vecPosDelta.z);

	UpdateRelativeTM();
	return true;
}

bool A3DModel::Turn(FLOAT vDeltaRad)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matTurn = a3d_RotateY(vDeltaRad);

	m_matAbsoluteTM = matTurn * m_matAbsoluteTM;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::SetRelativePos(A3DModel * pRefModel, A3DVECTOR3 vecPos)
{
	//Maybe we can write this function more efficient when we distingrish
	//the different reference model situation, but too absurd :(
	UpdateAbsoluteTM();

	A3DMATRIX4 matRefAbs;
	A3DVECTOR3 vecAbsPos;

	if( NULL == pRefModel )
		matRefAbs.Identity();
	else
		matRefAbs = pRefModel->GetAbsoluteTM();

	vecAbsPos = vecPos * matRefAbs;
	m_vecPos = vecAbsPos;
	m_matAbsoluteTM._41 = m_vecPos.x;
	m_matAbsoluteTM._42 = m_vecPos.y;
	m_matAbsoluteTM._43 = m_vecPos.z;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::SetRelativeDirAndUp(A3DModel * pRefModel, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matRefAbs;

	if( NULL == pRefModel )
		matRefAbs.Identity();
	else
		matRefAbs = pRefModel->GetAbsoluteTM();

	m_vecDir = vecDir * matRefAbs - A3DVECTOR3(0.0f) * matRefAbs;
	m_vecUp = vecUp * matRefAbs - A3DVECTOR3(0.0f) * matRefAbs;

	m_matAbsoluteTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);

	UpdateRelativeTM();
	return true;
}

bool A3DModel::SetPosInParent(A3DVECTOR3& vecPos)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matRefAbs;
	A3DVECTOR3 vecAbsPos;

	if( m_pParentModel )
		matRefAbs = m_pParentModel->GetAbsoluteTM();
	else if( m_pParentFrame )
		matRefAbs = m_pParentFrame->GetAbsoluteTM();
	else
		matRefAbs.Identity();

	vecAbsPos = vecPos * matRefAbs;
	m_vecPos = vecAbsPos;
	m_matAbsoluteTM._41 = m_vecPos.x;
	m_matAbsoluteTM._42 = m_vecPos.y;
	m_matAbsoluteTM._43 = m_vecPos.z;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::SetDirAndUpInParent(A3DVECTOR3& vecDir, A3DVECTOR3& vecUp)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matRefAbs;

	if( m_pParentModel )
		matRefAbs = m_pParentModel->GetAbsoluteTM();
	else if( m_pParentFrame )
		matRefAbs = m_pParentFrame->GetAbsoluteTM();
	else
		matRefAbs.Identity();

	m_vecDir = vecDir * matRefAbs - A3DVECTOR3(0.0f) * matRefAbs;
	m_vecUp = vecUp * matRefAbs - A3DVECTOR3(0.0f) * matRefAbs;

	m_matAbsoluteTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);

	UpdateRelativeTM();
	return true;
}

bool A3DModel::MoveRelative(A3DModel * pRefModel, A3DVECTOR3 vecDeltaPos)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matRefAbs;
	A3DVECTOR3 vecAbsDeltaPos;

	if( NULL == pRefModel )
		matRefAbs.Identity();
	else
		matRefAbs = pRefModel->GetAbsoluteTM();

	vecAbsDeltaPos = vecDeltaPos * matRefAbs;

	m_matAbsoluteTM = m_matAbsoluteTM * a3d_Translate(vecAbsDeltaPos.x, vecAbsDeltaPos.y,
		vecAbsDeltaPos.z);

	UpdateRelativeTM();
	return true;
}

//RotateRelative is relative to local coordinates;
bool A3DModel::RotateXRelative(FLOAT vDeltaRad, bool bRelativeLocal)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matTurn = a3d_RotateX(vDeltaRad);

	if( bRelativeLocal )
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::RotateYRelative(FLOAT vDeltaRad, bool bRelativeLocal)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matTurn = a3d_RotateY(vDeltaRad);

	if( bRelativeLocal )
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::RotateZRelative(FLOAT vDeltaRad, bool bRelativeLocal)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matTurn = a3d_RotateZ(vDeltaRad);

	if( bRelativeLocal )
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::RotateAxisRelative(A3DVECTOR3 vecAxis, FLOAT vDeltaRad, bool bRelativeLocal)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matTurn = a3d_RotateAxis(vecAxis, vDeltaRad);

	if( bRelativeLocal )
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	UpdateRelativeTM();
	return true;

}

bool A3DModel::RotateAxisRelative(A3DVECTOR3 vecPos, A3DVECTOR3 vecAxis, FLOAT vDeltaRad, bool bRelativeLocal)
{
	UpdateAbsoluteTM();

	A3DMATRIX4 matTurn = a3d_RotateAxis(vecPos, vecAxis, vDeltaRad);

	if( bRelativeLocal )
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	UpdateRelativeTM();
	return true;
}

bool A3DModel::UpdateAbsoluteTM()
{
	if( m_pParentModel )
	{
		m_matAbsoluteTM = m_matRelativeTM * m_pParentModel->GetAbsoluteTM();
	}
	else if( m_pParentFrame )
	{
		m_matAbsoluteTM = m_matRelativeTM * m_pParentFrame->GetAbsoluteTM();
	}
	else
		m_matAbsoluteTM = m_matRelativeTM;

	return true;
}

bool A3DModel::UpdateRelativeTM()
{
	A3DVECTOR3 vecRight;

	if( m_pParentModel )
	{
		A3DMATRIX4 matParent = m_pParentModel->GetAbsoluteTM();

		m_matRelativeTM = m_matAbsoluteTM * a3d_InverseTM(matParent);
	}
	else if( m_pParentFrame )
	{
		A3DMATRIX4 matParent = m_pParentFrame->GetAbsoluteTM();

		m_matRelativeTM = m_matAbsoluteTM * a3d_InverseTM(matParent);
	}
	else
		m_matRelativeTM = m_matAbsoluteTM;

	vecRight.x = m_matAbsoluteTM._11;
	vecRight.y = m_matAbsoluteTM._12;
	vecRight.z = m_matAbsoluteTM._13;

	m_vecPos.x = m_matAbsoluteTM._41;
	m_vecPos.y = m_matAbsoluteTM._42;
	m_vecPos.z = m_matAbsoluteTM._43;

	m_vecDir.x = m_matAbsoluteTM._31;
	m_vecDir.y = m_matAbsoluteTM._32;
	m_vecDir.z = m_matAbsoluteTM._33;
	m_vecDir = Normalize(m_vecDir); 

	m_vecUp.x = m_matAbsoluteTM._21;
	m_vecUp.y = m_matAbsoluteTM._22;
	m_vecUp.z = m_matAbsoluteTM._23;
	m_vecUp = Normalize(m_vecUp);
	

	m_bHasMoved = true;
	return true;
}

bool A3DModel::SetAnimRange(int nAnimStart, int nAnimEnd, bool bAnimLoop)
{
	if( nAnimStart > nAnimEnd )
		return false;

	if( nAnimStart < 0 )
		nAnimStart = 0;
	if( nAnimEnd >= m_nFrameCount)
		nAnimEnd = m_nFrameCount - 1;

	m_nAnimStart = nAnimStart;
	m_nAnimEnd = nAnimEnd;
	m_bAnimLoop = bAnimLoop;

	m_bPaused = false;
	m_nFrame = m_nAnimStart;
	return true;
}

bool A3DModel::SetAction(A3DACTION * pAction)
{
	return SetAnimRange(pAction->nAnimStart, pAction->nAnimEnd, pAction->bAnimLoop);
}

bool A3DModel::DefineAction(A3DACTION ** ppAction, const char * szActionName, int nAnimStart, int nAnimEnd, bool bAnimLoop)
{
	A3DACTION * pAction;

	//Make sure action name is unique;
	if( FindActionByName(szActionName) )
	{
		g_A3DErrLog.Log("A3DModel::DefineAction Action: %s Already defined!");
		return false;
	}

	if( nAnimStart > nAnimEnd )
	{
		g_A3DErrLog.Log("A3DModel::DefineAction animation start beyond animation end");
		return false;
	}
	pAction = (A3DACTION *)a_malloc(sizeof(A3DACTION));
	if( NULL == pAction )
	{
		g_A3DErrLog.Log("A3DModel::DefineAction Not enough memory!");
		return false;
	}

	ZeroMemory(pAction, sizeof(A3DACTION));
	strncpy(pAction->szName, szActionName, 32);
	pAction->nAnimStart = nAnimStart;
	pAction->nAnimEnd = nAnimEnd;
	pAction->bAnimLoop = bAnimLoop;

	m_pActionDefinitionList->Append((LPVOID) pAction, NULL);

	if( ppAction )
		*ppAction = pAction;
	return true;
}

A3DACTION * A3DModel::FindActionByName(const char * szActionName, ALISTELEMENT ** ppElement)
{
	A3DACTION * pAction = NULL;

	ALISTELEMENT * pThisActionElement = m_pActionDefinitionList->GetFirst();
	while( pThisActionElement != m_pActionDefinitionList->GetTail() )
	{
		pAction = (A3DACTION *) pThisActionElement->pData;
		if( _stricmp(pAction->szName, szActionName) == 0 )
		{
			if( ppElement )
				*ppElement = pThisActionElement;
			return pAction;
		}
		pThisActionElement = pThisActionElement->pNext;
	}
	return NULL;
}

A3DGROUPACTION * A3DModel::FindGroupActionByName(const char * szGroupActionName, ALISTELEMENT ** ppElement)
{
	A3DGROUPACTION * pGroupAction = NULL;

	ALISTELEMENT * pThisGroupActionElement = m_pActionGroupList->GetFirst();
	while( pThisGroupActionElement != m_pActionGroupList->GetTail() )
	{
		pGroupAction = (A3DGROUPACTION *) pThisGroupActionElement->pData;
		if( _stricmp(pGroupAction->szName, szGroupActionName) == 0 )
		{
			if( ppElement )
				*ppElement = pThisGroupActionElement;
			return pGroupAction;
		}
		pThisGroupActionElement = pThisGroupActionElement->pNext;
	}
	return NULL;
}

bool A3DModel::UpdateGroupActionName(const char * szGroupNameOld, const char * szGroupNameNew)
{
	A3DGROUPACTION * pGroupAction = NULL;

	if( FindGroupActionByName(szGroupNameNew, NULL) )
		return false;

	pGroupAction = FindGroupActionByName(szGroupNameOld, NULL);
	if( NULL == pGroupAction )
	{
		g_A3DErrLog.Log("A3DModel::UpdateGroupActionName old group name not found!");
		return false;
	}

	strncpy(pGroupAction->szName, szGroupNameNew, 32);
	return true;
}

bool A3DModel::ReleasePredefinedAction()
{
	// For duplicated model, the action and action group is just taken from the original model;
	if( m_bDuplicatedOne )
		return true;

	ALISTELEMENT * pThisActionElement = m_pActionDefinitionList->GetFirst();
	while( pThisActionElement != m_pActionDefinitionList->GetTail() )
	{
		A3DACTION * pAction = (A3DACTION *) pThisActionElement->pData;
		a_free(pAction);
		pAction = NULL;
		pThisActionElement = pThisActionElement->pNext;
	}

	ALISTELEMENT * pThisGroupActionElement = m_pActionGroupList->GetFirst();
	while( pThisGroupActionElement != m_pActionGroupList->GetTail() )
	{
		A3DGROUPACTION * pGroupAction = (A3DGROUPACTION *) pThisGroupActionElement->pData;
		a_free(pGroupAction);
		pGroupAction = NULL;
		pThisGroupActionElement = pThisGroupActionElement->pNext;
	}
	return true;
}

bool A3DModel::DiscardActionSchedule()
{
	m_ActionScheduleList.Reset();
	//Now we also have to let the current animation loop stops;
	m_bAnimLoop = false;
	m_nFrame = m_nAnimEnd;
	return true;
}

bool A3DModel::PlayActionByName(const char * szActionName, bool bDiscardOld, bool bAffectChild)
{
	int n;
	if( (n = strlen(szActionName)) == 0 )
		return false;

	m_bPaused = false;
	if( szActionName[0] == '[' && szActionName[n-1] == ']' )
	{
		//Group Action;
		A3DGROUPACTION * pGroupAction = FindGroupActionByName(szActionName, NULL);
		if( NULL == pGroupAction )
		{
			//For actions which was not found, we must set it to a default action;
			g_A3DErrLog.Log("A3DModel::PlayActionByName(), Can not find group action <%s>", szActionName);
			DiscardActionSchedule();
			return false;
		}
		else
		{
			if( bDiscardOld )
				DiscardActionSchedule();
			for(int i=0; i<pGroupAction->nActionNum; i++)
			{
				PushAction(pGroupAction->pActionElement[i], false);
			}
		}
	}
	else
	{
		//Single Action;
		A3DACTION * pAction = FindActionByName(szActionName, NULL);
		if( NULL == pAction )
		{
			//For actions which was not found, we must set it to a default action;
			g_A3DErrLog.Log("A3DModel::PlayActionByName(), Can not find element action [%s]", szActionName);
			DiscardActionSchedule();
			return false;
		}
		return PushAction(pAction, bDiscardOld);
	}

	//Further, we must let our child models to play the same action;
	if( bAffectChild )
	{
		if( m_ChildModelList.GetSize() )
		{
			ALISTELEMENT * pChildModelElement = m_ChildModelList.GetFirst();
			while( pChildModelElement != m_ChildModelList.GetTail() )
			{
				A3DModel * pChildModel = (A3DModel *) pChildModelElement->pData;
				
				pChildModel->PlayActionByName(szActionName, bDiscardOld);
				
				pChildModelElement = pChildModelElement->pNext;
			}
		}
	}
	return true;
}

bool A3DModel::PushAction(A3DACTION * pAction, bool bDiscardOld)
{
	if( bDiscardOld )
		DiscardActionSchedule();

	m_ActionScheduleList.Append((LPVOID) pAction, NULL);
	return true;
}

A3DACTION * A3DModel::PopAction()
{
	ALISTELEMENT * pFirst = m_ActionScheduleList.GetFirst();
	A3DACTION * pAction = NULL;

	if( pFirst != m_ActionScheduleList.GetTail() )
	{
		pAction = (A3DACTION *) pFirst->pData;
		m_ActionScheduleList.Delete(pFirst);
	}

	return pAction;
}

bool A3DModel::Duplicate(A3DModel ** ppNewModel)
{
	A3DModel * pNewModel;
	ALISTELEMENT * pThisElement = NULL;

	pNewModel = new A3DModel();
	if( NULL == pNewModel )
	{
		g_A3DErrLog.Log("A3DModel::Duplicate Not enough memory!");
		return false;
	}
	if( !pNewModel->Init(m_pA3DDevice) )
		return false;

	pNewModel->SetIsDuplicated(true);

	pNewModel->m_ModelAABB = m_ModelAABB;
	pNewModel->m_ModelOBB = m_ModelOBB;
	pNewModel->m_bHasMoved = true;
	pNewModel->m_nFrameOld = -1;
	pNewModel->m_nFrame = 0;
	pNewModel->m_nFrameCount = m_nFrameCount;
	pNewModel->m_bBuildOBBBevels = true;
	pNewModel->SetAbsoluteTM(m_matAbsoluteTM);
	pNewModel->UpdateRelativeTM();
	pNewModel->SetName(GetName());
	pNewModel->SetVertCount(m_nVertCount);
	pNewModel->SetIndexCount(m_nIndexCount);
	pNewModel->SetScale(m_vecScale.x, m_vecScale.y, m_vecScale.z);

	//First duplicate ChildFrames;
	pThisElement = m_ChildFrameList.GetFirst();
	while( pThisElement != m_ChildFrameList.GetTail() )
	{
		A3DFrame * pChildFrame = (A3DFrame *) pThisElement->pData;
		A3DFrame * pNewChildFrame;

		if( !m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->DuplicateFrame(pChildFrame, &pNewChildFrame) )
		{
			g_A3DErrLog.Log("A3DModel::Duplicate(), Duplciate Child frame fail!");
			return false;
		}
		
		pNewModel->GetChildFrameList()->Append((LPVOID) pNewChildFrame);
		pNewModel->RetrieveFrameOBB(pNewChildFrame);
		pThisElement = pThisElement->pNext;
	}

	// Now duplicate the action list;
	// For duplicated model, the action is just taken from the original model;
	pNewModel->SetDefinedActionList(m_pActionDefinitionList);
	// Now duplicate the action group list;
	// For duplicated model, the action group is just taken from the original model;
	pNewModel->SetGroupActionList(m_pActionGroupList);

	// Now duplicate gfx events;
	pThisElement = m_GFXEventList.GetFirst();
	while( pThisElement != m_GFXEventList.GetTail() )
	{
		GFXEVENT* pGFXEvent = (GFXEVENT*)pThisElement->pData;
		pNewModel->AddGFXEvent(pGFXEvent->nFrame, pGFXEvent->szGFXName, pGFXEvent->bLinked, pGFXEvent->vScale,
			pGFXEvent->szParentName, pGFXEvent->vecPos, pGFXEvent->vecDir, pGFXEvent->vecUp);
		pThisElement = pThisElement->pNext;
	}

	// Now duplicate sfx events;
	pThisElement = m_SFXEventList.GetFirst();
	while( pThisElement != m_SFXEventList.GetTail() )
	{
		SFXEVENT* pSFXEvent = (SFXEVENT*) pThisElement->pData;
		pNewModel->AddSFXEvent(pSFXEvent->nFrame, pSFXEvent->szSFXName, pSFXEvent->bLoop, pSFXEvent->b2DSound,
			pSFXEvent->vMinDis, pSFXEvent->vMaxDis, pSFXEvent->vVolume,
			pSFXEvent->szParentName, pSFXEvent->vecPos, pSFXEvent->vecDir, pSFXEvent->vecUp);
		pThisElement = pThisElement->pNext;
	}

	// Now duplicate logic events;
	pThisElement = m_LogicEventList.GetFirst();
	while( pThisElement != m_LogicEventList.GetTail() )
	{
		LOGICEVENT* pLogicEvent = (LOGICEVENT*) pThisElement->pData;
		pNewModel->AddLogicEvent(pLogicEvent->nFrame, pLogicEvent->szNotifyString);
		pThisElement = pThisElement->pNext;
	}

	*ppNewModel = pNewModel;
	
	//g_A3DErrLog.Log("%s, A3DModel::Duplicate()", GetName());
	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_MODEL);
	return true;
}

A3DModel * A3DModel::FindChildModelByName(const char * szName, ALISTELEMENT ** ppElement)
{
	ALISTELEMENT * pThisModelElement = m_ChildModelList.GetFirst();
	while( pThisModelElement != m_ChildModelList.GetTail() )
	{
		A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
		if( _stricmp(pModel->GetName(), szName) == 0 )
		{
			if( ppElement )
				*ppElement = pThisModelElement;
			return pModel;
		}
		pThisModelElement = pThisModelElement->pNext;
	}
	return NULL;
}

A3DFrame * A3DModel::FindChildFrameByName(const char * szName, ALISTELEMENT ** ppElement)
{
	ALISTELEMENT * pThisFrameElement = m_ChildFrameList.GetFirst();
	while( pThisFrameElement != m_ChildFrameList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisFrameElement->pData;
		if( _stricmp(pFrame->GetName(), szName) == 0 )
		{
			if( ppElement )
				*ppElement = pThisFrameElement;
			return pFrame;
		}

		//Then see if it is my child frame's child frame;
		A3DFrame * pChildFrame;
		if( pFrame->FindChildByName(szName, &pChildFrame) )
			return pChildFrame;

		pThisFrameElement = pThisFrameElement->pNext;
	}
	return NULL;
}

bool A3DModel::DeleteAction(const char * szActionName)
{
	A3DACTION * pAction;
	ALISTELEMENT * pElement;

	if( NULL != (pAction=FindActionByName(szActionName, &pElement)) )
	{
		//First we should make remove the group action's references of this action;
		ALISTELEMENT * pGroupElement = m_pActionGroupList->GetFirst();
		while( pGroupElement != m_pActionGroupList->GetTail() )
		{
			A3DGROUPACTION * pGroupAction = (A3DGROUPACTION *) pGroupElement->pData;
			
			for(int i=0; i<pGroupAction->nActionNum; i++)
			{
				if( pGroupAction->pActionElement[i] == pAction )
				{
					//Find reference now delete it;
					for(int j=i+1; j<pGroupAction->nActionNum; j++)
					{
						pGroupAction->pActionElement[j - 1] = pGroupAction->pActionElement[j];				
					}
					pGroupAction->nActionNum --;
					i --;
				}
			}
			pGroupElement = pGroupElement->pNext;
		}

		a_free(pAction);
		m_pActionDefinitionList->Delete(pElement);
		return true;
	}
	return false;
}

bool A3DModel::DeleteGroupAction(const char * szGroupActionName)
{
	A3DGROUPACTION * pGroupAction;
	ALISTELEMENT * pElement;

	if( NULL != (pGroupAction=FindGroupActionByName(szGroupActionName, &pElement)) )
	{
		a_free(pGroupAction);
		m_pActionGroupList->Delete(pElement);
		return true;
	}
	return false;
}

bool A3DModel::DeleteChildFrame(A3DFrame * pFrame)
{
	ALISTELEMENT * pElement;

	pElement = m_ChildFrameList.FindElementByData((LPVOID)pFrame);
	if( NULL == pElement )
	{
		return false;
	}

	bool		bval;
	bval = m_ChildFrameList.Delete(pElement);
	if( bval ) 
	{
		// Decrease vert count and index count;
		m_nVertCount -= pFrame->GetVertCount();
		m_nIndexCount -= pFrame->GetIndexCount();
	}
	return bval;
}

bool A3DModel::DeleteChildModel(A3DModel * pModel)
{
	if( pModel->GetRealParentModel() != this )
	{
		assert(false);
		g_A3DErrLog.Log("A3DModel::DeleteChildModel(), Model [%s] is not the child of model [%s]", pModel->GetName(), GetName());
		return false;
	}

	ALISTELEMENT * pElement;

	pElement = m_ChildModelList.FindElementByData((LPVOID)pModel);
	if( NULL == pElement )
	{
		g_A3DErrLog.Log("A3DModel::DeleteChildModel(), Can not find child model [%s]", pModel->GetName());
		return false;
	}

	return DeleteChildModel(pElement);
}

bool A3DModel::DeleteChildModel(ALISTELEMENT * pElement)
{
	A3DModel * pModel = (A3DModel *) pElement->pData;
	pModel->SetRealParentModel(NULL);
	pModel->SetParentModel(NULL);
	pModel->SetParentFrame(NULL);
	pModel->SetConnectionCenterFrame(NULL);

	bool	bval;
	bval = m_ChildModelList.Delete(pElement);
	if( bval )
	{
		// Decrease vert count and index count;
		m_nVertCount -= pModel->GetVertCount();
		m_nIndexCount -= pModel->GetIndexCount();
	}
	return bval;
}

bool A3DModel::UpdateAction(const char * szActionNameOld, const char * szActionNameNew, int nAnimStart, int nAnimEnd, bool bAnimLoop)
{
	A3DACTION * pAction;

	if( _stricmp(szActionNameOld, szActionNameNew) != 0 )
	{
		if( FindActionByName(szActionNameNew, NULL) )
			return false;
	}

	pAction = FindActionByName(szActionNameOld, NULL);
	if( pAction )
	{
		strcpy(pAction->szName, szActionNameNew);
		pAction->nAnimStart = nAnimStart;
		pAction->nAnimEnd = nAnimEnd;
		pAction->bAnimLoop = bAnimLoop;
	}
	else
		return false;

	return true;
}

bool A3DModel::AddGroupAction(A3DGROUPACTION ** ppGroupAction, const char * szGroupActionName, const char * szActionName)
{
	A3DGROUPACTION * pGroupAction;

	pGroupAction = FindGroupActionByName(szGroupActionName);
	if( NULL == pGroupAction )
	{
		//This group action has not been defined, so add one new group action;
		pGroupAction = (A3DGROUPACTION *)a_malloc(sizeof(A3DGROUPACTION));
		if( NULL == pGroupAction )
		{
			g_A3DErrLog.Log("A3DModel::AddGroupAction Not enough memory!");
			return false;
		}
		ZeroMemory(pGroupAction, sizeof(A3DGROUPACTION));
		strncpy(pGroupAction->szName, szGroupActionName, 32);
		pGroupAction->nActionNum = 0;
		m_pActionGroupList->Append((LPVOID) pGroupAction);
	}
	else if( strlen(szActionName) == 0 )
	{
		//szActionName is "" means want to create a new group action;
		g_A3DErrLog.Log("A3DModel::AddGroupAction Group % s already defined!", szGroupActionName);
		return false;
	}

	if( strlen(szActionName) == 0 )
	{
		//Empty action name, just leave the group here;
		if( ppGroupAction )
			*ppGroupAction = pGroupAction;
		return true;
	}

	if( pGroupAction->nActionNum >= A3D_GROUP_ACTION_MAX_ELEMENT )
	{
		g_A3DErrLog.Log("A3DModel::AddGroupAction Group %s has more than MAX_ELEMENT already!", szGroupActionName);
		return false;
	}

	A3DACTION * pAction = FindActionByName(szActionName);
	if( NULL == pAction )
	{
		g_A3DErrLog.Log("A3DModel::AddGroupAction The specified action element: %s has not defined!", szActionName); 
		return false;
	}
	pGroupAction->pActionElement[pGroupAction->nActionNum] = pAction;
	pGroupAction->nActionNum ++;

	if( ppGroupAction )
		*ppGroupAction = pGroupAction;
	return true;
}

bool A3DModel::DeleteGroupActionElement(const char * szGroupActionName, const char * szActionName, bool bDeleteEmpty)
{
	A3DGROUPACTION *	pGroupAction;
	A3DACTION *			pAction;
	ALISTELEMENT *		pElement;

	pGroupAction = FindGroupActionByName(szGroupActionName, &pElement);
	if( NULL == pGroupAction )
	{
		g_A3DErrLog.Log("A3DModel::DeleteGroupActionElement The specified group %s has not been defined!", szGroupActionName);
		return false;
	}
	pAction = FindActionByName(szActionName);
	if( NULL == pAction )
	{
		g_A3DErrLog.Log("A3DModel::DeleteGroupActionByElement The specifield action %s has not been defined!", szActionName);
		return false;
	}
	
	for(int i=0; i<pGroupAction->nActionNum; i++)
	{
		if( pGroupAction->pActionElement[i] == pAction )
		{
			//Find it here, no move the follows ahead;
			for(int j=i+1; j<pGroupAction->nActionNum; j++)
			{
				pGroupAction->pActionElement[j - 1] = pGroupAction->pActionElement[j];				
			}
			pGroupAction->nActionNum --;
			break;
		}
	}

	if( pGroupAction->nActionNum == 0 && bDeleteEmpty )
	{
		m_pActionGroupList->Delete(pElement);
	}
	return true;
}

/*
	This function test if the movement will collide with other objects or structures or ground;
*/
bool A3DModel::TraceMove(A3DVECTOR3& vecDelta)
{
	A3DWorld::COLLISION  collision;
	
	if( m_pA3DDevice->GetA3DEngine()->GetA3DWorld()->GetFirstCollision(this, vecDelta, &collision) )
	{
		vecDelta = collision.vecClipDelta;
		return true;
	}

	return true;
}

bool MyCLS_RayToOBB3(A3DVECTOR3& vecStart, A3DVECTOR3& vecDelta, A3DOBB& obb, A3DVECTOR3& vecPoint, FLOAT * pvFraction, A3DVECTOR3& vecNormal)
{
	A3DMATRIX4 mat = a3d_ViewMatrix(obb.Center, obb.ZAxis, obb.YAxis, 0.0f);
	
	A3DAABB aabb;

	aabb.Center = A3DVECTOR3(0.0f);
	aabb.Maxs = A3DVECTOR3(obb.Extents.x, obb.Extents.y, obb.Extents.z);
	aabb.Mins = -aabb.Maxs;

	A3DVECTOR3 vecStartL, vecDeltaL;

	vecStartL = vecStart * mat;
	vecDeltaL = vecDelta * mat - A3DVECTOR3(0.0f) * mat;
	return CLS_RayToAABB3(vecStartL, vecDeltaL, aabb.Mins, aabb.Maxs, vecPoint, pvFraction, vecNormal);
}

/*
	This function calculate its intersection with a ray;

	return true if an intersection exist;
	return false if not;
*/
bool A3DModel::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACERT* pRayTrace, DWORD dwTraceMask)
{
	if( !GetRayTraceEnable(dwTraceMask) )
		return false;

	FLOAT		vFraction;
	A3DVECTOR3	vecNormal;
	A3DVECTOR3	vecPoint;

	pRayTrace->fFraction = 1.0f;

	m_RayTraceRt.pModelOBB	= NULL;
	m_RayTraceRt.pRootModel	= NULL;

	A3DAABB aabb = GetModelAABB();
	if( GetModelOBBNum() <= 2 || CLS_RayToAABB3(vecStart, vecDelta, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
	{
		if( GetModelOBBNum() == 0 )
		{
			/*
			if( CLS_RayToAABB3(vecStart, vecDelta, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
			{
				pRayTrace->fFraction	= vFraction;
				pRayTrace->vPoint		= vecPoint;
				pRayTrace->vNormal		= vecNormal;

				pRayTrace->objectType	= TRACE_OBJECT_MODEL;
				pRayTrace->pModel		= this;
				pRayTrace->pModelOBB	= NULL;
			}*/
		}
		else
		{
			ALISTELEMENT * pOBBElement = m_ModelOBBList.GetFirst();
			while( pOBBElement != m_ModelOBBList.GetTail() )
			{
				MODELOBB* pModelOBB = (MODELOBB*)pOBBElement->pData; 

				if( CLS_RayToOBB3(vecStart, vecDelta, pModelOBB->obb, vecPoint, &vFraction, vecNormal) )
				{
					if( vFraction < pRayTrace->fFraction )
					{
						pRayTrace->fFraction	= vFraction;
						pRayTrace->vPoint		= vecPoint;
						pRayTrace->vNormal		= vecNormal;

						m_RayTraceRt.pModelOBB	= pModelOBB;
					}
				}

				pOBBElement = pOBBElement->pNext;
			}
		}
	}

	if( m_ChildModelList.GetSize() > 0 )
	{
		RAYTRACERT	rayTrace;

		ALISTELEMENT * pChildModelElement = m_ChildModelList.GetFirst();
		while( pChildModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pChildModel = (A3DModel *) pChildModelElement->pData;
			if( pChildModel->RayTrace(vecStart, vecDelta, &rayTrace) )
			{
				if( rayTrace.fFraction < pRayTrace->fFraction )
				{
					//This is nearer;
					*pRayTrace = rayTrace;

					m_RayTraceRt = pChildModel->m_RayTraceRt;
				}
			}

			pChildModelElement = pChildModelElement->pNext;
		}
	}

	if (pRayTrace->fFraction < 1.0f)
	{
		//	Yes collided;
		m_RayTraceRt.pRootModel	= this;
		return true;
	}

	return false;
}

/*	Check whether specified AABB collide this model.

	Return true if collision occured, otherwise return false.

	pInfo: AABB trace structure initialized by AABBTraceInit().
	pTrace: structure used to receive trace result.
*/
bool A3DModel::AABBTrace(AABBTRACEINFO* pInfo, AABBTRACERT* pTrace, DWORD dwTraceMask)
{
	if( !GetAABBTraceEnable(dwTraceMask) )
		return false;

	m_AABBTraceRt.pModelOBB		= NULL;
	m_AABBTraceRt.pRootModel	= NULL;

	bool bHit = false;
	A3DVECTOR3 vNormal;

	A3DAABB	aabb; 
	if( m_bTraceMoveAABBOnly )
	{
		aabb.Center = m_vecPos + m_vecAABBTraceCenter;
		aabb.Extents = m_vecAABBTraceExtents;
		aabb.Mins = aabb.Center - aabb.Extents;
		aabb.Maxs = aabb.Center + aabb.Extents;
	}
	else
	{
		aabb = GetModelAABB();
	}

	pTrace->fFraction = 1.0f;

	if (CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, aabb.Center, aabb.Extents))
	{
		// If we only use aabb to trace move;
		if (m_bTraceMoveAABBOnly)
		{
			if (TRA_AABBMoveToAABB(pInfo, aabb))
			{
				bHit = true;

				if (pInfo->fFraction < pTrace->fFraction)
				{
					pTrace->fFraction	 = pInfo->fFraction;
					pTrace->vNormal		 = pInfo->ClipPlane.vNormal;
				}
			}

			goto EXIT;
		}

		// Else we should descent into obbs;
		ALISTELEMENT* pOBBElement;
		pOBBElement = m_ModelOBBList.GetFirst();
		while (pOBBElement != m_ModelOBBList.GetTail())
		{
			MODELOBB* pModelOBB = (MODELOBB*)pOBBElement->pData;

			//	Fast check AABB at first
			aabb.Build(pModelOBB->obb);
			if (!CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, aabb.Center, aabb.Extents))
			{
				pOBBElement = pOBBElement->pNext;
				continue;
			}

			if (m_bBuildOBBBevels)
				TRA_BuildOBBBevels(pModelOBB->obb, &pModelOBB->Bevels);

			if (TRA_AABBMoveToBrush(pInfo, pModelOBB->Bevels.aPlaneAddrs, pModelOBB->Bevels.iNumPlane))
			{
				bHit = true;
				if (pInfo->fFraction < pTrace->fFraction)
				{
					pTrace->fFraction	 = pInfo->fFraction;
					pTrace->vNormal		 = pInfo->ClipPlane.vNormal;

					m_AABBTraceRt.pModelOBB = pModelOBB;
				}
			}

			pOBBElement = pOBBElement->pNext;
		}

		m_bBuildOBBBevels = false;

		if (m_ChildModelList.GetSize() > 0)
		{
			AABBTRACERT Trace;

			ALISTELEMENT* pChildModelElement = m_ChildModelList.GetFirst();
			while (pChildModelElement != m_ChildModelList.GetTail())
			{
				A3DModel* pChildModel = (A3DModel*)pChildModelElement->pData;
				if (pChildModel->AABBTrace(pInfo, &Trace))
				{
					bHit = true;
					
					if (Trace.fFraction < pTrace->fFraction)
					{
						//	This is nearer;
						*pTrace = Trace;

						m_AABBTraceRt = pChildModel->m_AABBTraceRt;
					}
				}

				pChildModelElement = pChildModelElement->pNext;
			}
		}

EXIT:
		if (bHit)	//	Yes collided
		{
			pTrace->vDestPos = pInfo->vStart + pInfo->vDelta * pTrace->fFraction;
			return true;
		}
	}

	return false;
}

bool A3DModel::GetFrameLocation(const char * szFrameName, A3DMATRIX4 matParent, A3DVECTOR3 * vecPos, A3DVECTOR3 * vecX, A3DVECTOR3 * vecY, A3DVECTOR3 * vecZ)
{
	// First we should update to that frame before we use any transform matrix;
	UpdateToFrame(-1);

	A3DFrame * pFrame = FindChildFrameByName(szFrameName);
	if( NULL == pFrame )
	{
		g_A3DErrLog.Log("A3DModel::GetFrameTM() The frame [%s] can not be found in model [%s]!", szFrameName, GetName());
		return false;
	}

	A3DMATRIX4 matAbsolute;
	A3DMATRIX4 matRelative;

	matAbsolute = pFrame->GetAbsoluteTM();
	matRelative = matAbsolute * a3d_InverseTM(matParent);

	*vecX	= matRelative.GetRow(0);
	*vecY	= matRelative.GetRow(1);
	*vecZ	= matRelative.GetRow(2);
	*vecPos = matRelative.GetRow(3);
	return true;
}

bool A3DModel::AddGFX(A3DGraphicsFX * pGFX, bool bLinked, FLOAT vScale, const char * szParentName, A3DVECTOR3 vecPos,
					A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, GFXEVENT* pGFXEvent)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// By defaults, the vecPos is in the parent frame's space;
	A3DMATRIX4 matParent = a3d_IdentityMatrix();

	// Now set all parameters necessary
	pGFX->SetHostModel(this);
	if( szParentName )
	{
		A3DFrame * pParentFrame = FindChildFrameByName(szParentName);
		if( NULL == pParentFrame )
		{
			g_A3DErrLog.Log("A3DModel::AddGFX() Can find child frame [%s]", szParentName);
			return false;
		}
		pGFX->SetParentFrame(pParentFrame);
		matParent = pParentFrame->GetAbsoluteTM();
	}
	else
		pGFX->SetParentFrame(NULL);

	if( !bLinked )
	{
		// Still not work as I expected, check it at later time ############
		// If not linked, the pos must be tranformed into world space;
		vecPos = vecPos * matParent;
		vecDir = vecDir * matParent - matParent.GetRow(3);
		vecUp  = vecUp  * matParent - matParent.GetRow(3);
	}
	
	pGFX->SetPos(vecPos);
	pGFX->SetDirAndUp(vecDir, vecUp);
	pGFX->SetLinked(bLinked);
	pGFX->SetScale(vScale);

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->AddGFX(pGFX) )
	{
		g_A3DErrLog.Log("A3DModel::AddGFX() Call A3DGFXMan->AddGFX Fail!");
		return false;
	}

	UpdateToFrame(-1);
	pGFX->UpdateParentInfo();

	// We should call RecalculateAllFX to make sure effects such as Mark can 
	// be recalculated at the now position
	pGFX->RecalculateAllFX();
	pGFX->Start(false);

	// We must record each enabled gfx here;
	// we should record that gfx and update its position at proper time;
	GFXRECORD* pNewGFX = (GFXRECORD*)a_malloc(sizeof (GFXRECORD));
	if( !pNewGFX )
	{
		g_A3DErrLog.Log("A3DModel::AddGFX() Not enough memory!");
		return false;
	}
	ZeroMemory(pNewGFX, sizeof (GFXRECORD));
	pNewGFX->pGFX		= pGFX;
	pNewGFX->pGFXEvent	= (LPVOID) pGFXEvent;
	m_GFXList.Append((LPVOID) pNewGFX);

	// Store this new gfx's record into that gfx event structure for later use;
	pGFXEvent->pGFXRecord = (LPVOID) pNewGFX;
	return true;
}

bool A3DModel::AddGFXByName(const char * szGFXFile, bool bLinked, const char * szParentName, A3DVECTOR3 vecPos,
							A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, FLOAT vScale)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// By defaults the vecPos is in world space if not linked and in parent frame space
	// if bLinked is true, we must be aware of it, this is determined by A3DGFXMan

	// First try to load that gfx file from disk;
	A3DGraphicsFX * pGFX;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile(szGFXFile, this,
		szParentName, bLinked, &pGFX, vecPos, vecDir, vecUp) )
	{
		g_A3DErrLog.Log("A3DModel::AddGFXByName() Can't load gfx file [%s]", szGFXFile);
		return false;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->AddGFX(pGFX) )
	{
		g_A3DErrLog.Log("A3DModel::AddGFXByName() Call A3DGFXMan->AddGFX Fail!");
		return false;
	}

	UpdateToFrame(-1);
	pGFX->SetPos(vecPos);
	pGFX->UpdateParentInfo();

	// If this graphics effect will be linked to the model, it should be recorded by the model;
	if( bLinked )
		pGFX->Start(false);
	else
		pGFX->Start(true);

	pGFX->SetScale(vScale);

	if( !bLinked )
		return true; // For that gfx will have no relation with the model any more;

	// Else we should record that gfx and update its position at proper time;
	GFXRECORD* pNewGFX = (GFXRECORD*)a_malloc(sizeof (GFXRECORD));
	if( !pNewGFX )
	{
		g_A3DErrLog.Log("A3DModel::AddGFXByName() Not enough memory!");
		return false;
	}
	ZeroMemory(pNewGFX, sizeof (GFXRECORD));
	pNewGFX->pGFX	= pGFX;
	
	m_GFXList.Append((LPVOID) pNewGFX);
	return true;
}

bool A3DModel::AddGFXEvent(int nFrame, const char * szGFXFile, bool bLinked, FLOAT vScale, const char * szParentName, 
						   A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	GFXEVENT* pGFXEvent = (GFXEVENT*)a_malloc(sizeof(GFXEVENT));
	if( NULL == pGFXEvent )
	{
		g_A3DErrLog.Log("A3DModel::AddGFXEvent() Not enough memory!");
		return false;
	}

	ZeroMemory(pGFXEvent, sizeof(GFXEVENT));
	pGFXEvent->nFrame	= nFrame;
	if( nFrame == 0 )
	{
		g_A3DErrLog.Log("A3DModel::AddGFXEvent(), Model [%s] has a gfx event at frame 0!", GetName());
	}
	pGFXEvent->bLinked	= bLinked;
	pGFXEvent->vScale	= vScale;

	strncpy(pGFXEvent->szGFXName, szGFXFile, MAX_PATH);
	strncpy(pGFXEvent->szParentName, szParentName, 32);
	pGFXEvent->vecPos	= vecPos;
	pGFXEvent->vecDir	= vecDir;
	pGFXEvent->vecUp	= vecUp;

	if( !m_bDuplicatedOne )
	{
		// if this is not used for rendering, we will not actually create gfx object
		pGFXEvent->pGFX = NULL;
	}
	else
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile(pGFXEvent->szGFXName,
			this, pGFXEvent->szParentName, pGFXEvent->bLinked, &pGFXEvent->pGFX, pGFXEvent->vecPos,
			pGFXEvent->vecDir, pGFXEvent->vecUp) )
		{
			g_A3DErrLog.Log("A3DModel::AddGFXEvent() Can not load gfx file [%s]!", pGFXEvent->szGFXName);
			return false;
		}
	}

	m_GFXEventList.Append((LPVOID) pGFXEvent);
	return true;
}

bool A3DModel::DeleteGFXEvent(int nIndex)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	ALISTELEMENT * pGFXEventElement = m_GFXEventList.GetElementByOrder(nIndex);
	if( !pGFXEventElement ) return false;

	// We should release some necessary resource we have allocated;
	GFXEVENT* pGFXEvent = (GFXEVENT*)pGFXEventElement->pData;
	
	// See if it is current playing now;
	if( pGFXEvent->pGFXRecord )
	{
		// First delete the coresponding element in gfx list;
		ALISTELEMENT * pGFXRecordElement = m_GFXList.FindElementByData(pGFXEvent->pGFXRecord);
		if( NULL == pGFXRecordElement )
		{
			g_A3DErrLog.Log("A3DModel::DeleteGFXEvent() Can't delete it! Because the gfx record pointer does not exist in gfx list");
			return false;
		}
		m_GFXList.Delete(pGFXRecordElement);

		// Now remove the reference in gfx list;
		RemoveGFX((GFXRECORD*)pGFXEvent->pGFXRecord);
	}
	// Now we can safely release the gfx;
	m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->ReleaseGFX(pGFXEvent->pGFX);

	// free memory allocated;
	a_free(pGFXEvent);

	return m_GFXEventList.Delete(pGFXEventElement);
}

A3DModel::GFXEVENT* A3DModel::GetGFXEvent(int nIndex)
{
	ALISTELEMENT* pGFXEventElement = m_GFXEventList.GetElementByOrder(nIndex);
	if( !pGFXEventElement ) return NULL;

	return (GFXEVENT*)pGFXEventElement->pData;
}

void A3DModel::SetVisibility(bool bVisible)
{
	m_bVisible = bVisible;

	if( m_ChildModelList.GetSize() > 0 )
	{
		ALISTELEMENT * pChildModelElement = m_ChildModelList.GetFirst();
		while( pChildModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pChildModel = (A3DModel *) pChildModelElement->pData;
			pChildModel->SetVisibility(bVisible);
			pChildModelElement = pChildModelElement->pNext;
		}
	}
}

bool A3DModel::CheckGFXEvents()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// if not visible, don't create any gfx here;
	if( !m_bVisible )
		return true;

	ALISTELEMENT* pGFXEventElement = m_GFXEventList.GetFirst();
	while( pGFXEventElement != m_GFXEventList.GetTail() )
	{
		GFXEVENT* pGFXEvent = (GFXEVENT*) pGFXEventElement->pData;
		if( m_nFrame == pGFXEvent->nFrame )
		{
			// if it is current playing now;
			if( pGFXEvent->pGFXRecord )
			{
				pGFXEvent->pGFX->Stop(false);
				// We should call RecalculateAllFX to make sure effects such as Mark can 
				// be recalculated at the now position
				pGFXEvent->pGFX->RecalculateAllFX();
				pGFXEvent->pGFX->Start(false);
			}
			else
			{
				// Add it into current playing gfx list;
				if( !AddGFX(pGFXEvent->pGFX, pGFXEvent->bLinked, pGFXEvent->vScale, pGFXEvent->szParentName,
					pGFXEvent->vecPos, pGFXEvent->vecDir, pGFXEvent->vecUp, pGFXEvent) )
				{
					g_A3DErrLog.Log("A3DModel::CheckGFXEvents() AddGFX Fail!");
					return false;
				}
			}
		}
		pGFXEventElement = pGFXEventElement->pNext;
	}
	return true;
}

bool A3DModel::RemoveGFX(GFXRECORD* pGFXRecord)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	if( !pGFXRecord->pGFXEvent )
	{
		// This gfx need not to be reused;
		if( m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->IsGFXAlive(pGFXRecord->pGFX) )
		{
			pGFXRecord->pGFX->Stop(true);
			m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->ReleaseGFX(pGFXRecord->pGFX);
		}
	}
	else
	{
		// First make sure the gfx is still alive;
		if( m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->IsGFXAlive(pGFXRecord->pGFX) )
		{
			// Must delete from gfx events list;
			pGFXRecord->pGFX->Stop(false);
			m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->DeleteGFX(pGFXRecord->pGFX);
		}

		((GFXEVENT*)pGFXRecord->pGFXEvent)->pGFXRecord = NULL;
	}
	
	a_free(pGFXRecord);
	return true;
}

bool A3DModel::StopAllGFX()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// First traverse current gfx list, and stop them all, and release some of them
	ALISTELEMENT * pThisGFXElement = m_GFXList.GetFirst();
	while( pThisGFXElement != m_GFXList.GetTail() )
	{
		GFXRECORD* pGFXRecord = (GFXRECORD*) pThisGFXElement->pData;
		RemoveGFX(pGFXRecord);
		pThisGFXElement = pThisGFXElement->pNext;
	}

	m_GFXList.Reset();
	return true;
}

// Stop the model's gfx very softly;
bool A3DModel::SoftStopAllGFX()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// First traverse current gfx list, and stop them all, and release some of them
	ALISTELEMENT * pThisGFXElement = m_GFXList.GetFirst();
	while( pThisGFXElement != m_GFXList.GetTail() )
	{
		GFXRECORD* pGFXRecord = (GFXRECORD*) pThisGFXElement->pData;

		if( !pGFXRecord->pGFXEvent )
		{
			// This gfx need not to be reused;
			if( m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->IsGFXAlive(pGFXRecord->pGFX) )
			{
				pGFXRecord->pGFX->Stop(true);
				m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->ReleaseGFX(pGFXRecord->pGFX);
			}
		}
		else
		{
			// Must delete from gfx events list;
			if( m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->IsGFXAlive(pGFXRecord->pGFX) )
			{
				pGFXRecord->pGFX->Stop(false);
			}
			((GFXEVENT*)pGFXRecord->pGFXEvent)->pGFXRecord = NULL;
		}

		a_free(pGFXRecord);

		pThisGFXElement = pThisGFXElement->pNext;
	}
	m_GFXList.Reset();

	return true;
}

bool A3DModel::RestartGFXEvents()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	StopAllGFX();

	// Now check if there is some events that want to start at -1 frame, right here;
	ALISTELEMENT * pThisGFXEventElement = m_GFXEventList.GetFirst();
	while( pThisGFXEventElement != m_GFXEventList.GetTail() )
	{
		GFXEVENT* pGFXEvent = (GFXEVENT*) pThisGFXEventElement->pData;
		if( pGFXEvent->nFrame == -1 )
		{
			if( !AddGFX(pGFXEvent->pGFX, pGFXEvent->bLinked, pGFXEvent->vScale, pGFXEvent->szParentName, 
				pGFXEvent->vecPos, pGFXEvent->vecDir, pGFXEvent->vecUp, pGFXEvent) )
			{
				g_A3DErrLog.Log("A3DModel::RestartEvents() AddGFX Fail!");
				return false;
			}
		}
		pThisGFXEventElement = pThisGFXEventElement->pNext;
	}
	return true;
}

bool A3DModel::AddSFXEvent(int nFrame, const char * szSFXFile, bool bLoop, bool b2DSound, FLOAT vMinDis, FLOAT vMaxDis,
						   FLOAT vVolume, const char * szParentName, 
						   A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	SFXEVENT* pSFXEvent = (SFXEVENT*)a_malloc(sizeof (SFXEVENT));
	if( NULL == pSFXEvent )
	{
		g_A3DErrLog.Log("A3DModel::AddSFXEvent() Not enough memory!");
		return false;
	}

	ZeroMemory(pSFXEvent, sizeof (SFXEVENT));
	pSFXEvent->nFrame	= nFrame;
	if( nFrame == 0 )
	{
		g_A3DErrLog.Log("A3DModel::AddSFXEvent(), Model [%s] has a sfx event at frame 0!", GetName());
	}
	pSFXEvent->bLoop	= bLoop;
	pSFXEvent->b2DSound	= b2DSound;
	pSFXEvent->vMinDis	= vMinDis;
	pSFXEvent->vMaxDis	= vMaxDis;
	pSFXEvent->vVolume	= vVolume;

	strncpy(pSFXEvent->szSFXName, szSFXFile, MAX_PATH);
	strncpy(pSFXEvent->szParentName, szParentName, 32);
	pSFXEvent->vecPos	= vecPos;
	pSFXEvent->vecDir	= vecDir;
	pSFXEvent->vecUp	= vecUp;

	pSFXEvent->pParentFrame = FindChildFrameByName(szParentName);
	if( !pSFXEvent->pParentFrame )
	{
		// This is only a warning message;
		g_A3DErrLog.Log("A3DModel::AddSFXEvent() Can not find child frame [%s]", szParentName);
	}

	SFXRECORD* pSFXRecord;
	pSFXRecord = FindSFXRecord(pSFXEvent->szSFXName);
	if( !pSFXRecord )
	{
		pSFXEvent->pSFX = m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Load3DSound(pSFXEvent->szSFXName, false);
		if (!pSFXEvent->pSFX)
		{
			g_A3DErrLog.Log("A3DModel::AddSFXEvent() Can not load sfx file [%s]!", pSFXEvent->szSFXName);
			a_free(pSFXEvent);
			return true;
		}

		pSFXRecord = (SFXRECORD*)a_malloc(sizeof (SFXRECORD));
		if( NULL == pSFXRecord )
		{
			g_A3DErrLog.Log("A3DModel::AddSFXEvent(), Not enough memory!");
			return false;
		}

		ZeroMemory(pSFXRecord, sizeof (SFXRECORD));
		pSFXRecord->pSFX = pSFXEvent->pSFX;
		strncpy(pSFXRecord->szSFXName, pSFXEvent->szSFXName, MAX_PATH);

		m_SFXList.Append((LPVOID) pSFXRecord);
	}
	else
	{
		pSFXEvent->pSFX = pSFXRecord->pSFX;
	}

	// Record the sfx record pointer;
	pSFXEvent->pSFXRecord = pSFXRecord;

	// set some preset properties;
	pSFXEvent->pSFX->SetMinDistance(pSFXEvent->vMinDis);
	pSFXEvent->pSFX->SetMaxDistance(pSFXEvent->vMaxDis);
	pSFXEvent->pSFX->SetForce2D(pSFXEvent->b2DSound);
	m_SFXEventList.Append((LPVOID) pSFXEvent);
	return true;
}

bool A3DModel::DeleteSFXEvent(int nIndex)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	ALISTELEMENT * pSFXEventElement = m_SFXEventList.GetElementByOrder(nIndex);
	if( !pSFXEventElement ) return false;

	// We should release some necessary resource we have allocated;
	SFXEVENT* pSFXEvent = (SFXEVENT*)pSFXEventElement->pData;
	
	// Now we can safely release the sfx;
	if( pSFXEvent->pSFX )
	{
		pSFXEvent->pSFX->Stop();
	
		//m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->Get3DSoundBufferMan()->Release3DSoundBuffer(pSFXEvent->pSFX);
	}

	// free memory allocated;
	a_free(pSFXEvent);
	return m_SFXEventList.Delete(pSFXEventElement);
}

A3DModel::SFXEVENT* A3DModel::GetSFXEvent(int nIndex)
{
	ALISTELEMENT * pSFXEventElement = m_SFXEventList.GetElementByOrder(nIndex);
	if( !pSFXEventElement ) return NULL;

	return (SFXEVENT*) pSFXEventElement->pData;
}

bool A3DModel::CheckSFXEvents()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// if not visible, should we play any sfx here?
	if( !m_bVisible )
		return true;

	ALISTELEMENT * pSFXEventElement = m_SFXEventList.GetFirst();
	while( pSFXEventElement != m_SFXEventList.GetTail() )
	{
		SFXEVENT* pSFXEvent = (SFXEVENT*) pSFXEventElement->pData;
		if (m_nFrame == pSFXEvent->nFrame)
		{
			A3DVECTOR3 vecPos = pSFXEvent->vecPos;
			A3DVECTOR3 vecDir = pSFXEvent->vecDir;

			if (pSFXEvent->pParentFrame)
			{
				A3DMATRIX4 matParent = pSFXEvent->pParentFrame->GetAbsoluteTM();
				vecPos = vecPos * matParent;
				vecDir = vecDir * matParent - matParent.GetRow(3);
			}

			//	If it is current playing now;
			if (pSFXEvent->pSFX)
			{	
				pSFXEvent->pSFX->Stop();

				pSFXEvent->pSFX->SetPosition(vecPos);
				pSFXEvent->pSFX->SetConeOrientation(vecDir);

				pSFXEvent->pSFX->UpdateChanges();

				//	For these sfx will probably moving toward us, so we have to play all
				//	of them but for action based sound, we can not here much maybe;
				if (pSFXEvent->pSFX->CanHear())
				{
					pSFXEvent->pSFXRecord->pCurEvent = pSFXEvent;
					pSFXEvent->pSFX->Play(pSFXEvent->bLoop);
				}
			}
		}
			
		pSFXEventElement = pSFXEventElement->pNext;
	}
	return true;
}

bool A3DModel::StopAllSFX()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// First traverse current sfx list, and stop them all, and release some of them
	ALISTELEMENT * pThisSFXElement = m_SFXEventList.GetFirst();
	while( pThisSFXElement != m_SFXEventList.GetTail() )
	{
		SFXEVENT* pSFXEvent = (SFXEVENT*) pThisSFXElement->pData;
		if (pSFXEvent->pSFX)
			pSFXEvent->pSFX->Stop();
		pThisSFXElement = pThisSFXElement->pNext;
	}
	return true;
}

bool A3DModel::PauseAllSFX(bool bPause)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	if( bPause )
	{
		// Pause all current playing sounds.
		ALISTELEMENT * pThisSFXElement = m_SFXEventList.GetFirst();
		while( pThisSFXElement != m_SFXEventList.GetTail() )
		{
			SFXEVENT* pSFXEvent = (SFXEVENT*) pThisSFXElement->pData;
			if (pSFXEvent->pSFX && pSFXEvent->pSFX->IsPlaying())
				pSFXEvent->pSFX->Pause();
			pThisSFXElement = pThisSFXElement->pNext;
		}
	}
	else
	{
		// Resume all paused sounds.
		ALISTELEMENT * pThisSFXElement = m_SFXEventList.GetFirst();
		while( pThisSFXElement != m_SFXEventList.GetTail() )
		{
			SFXEVENT* pSFXEvent = (SFXEVENT*) pThisSFXElement->pData;
			if (pSFXEvent->pSFX && pSFXEvent->pSFX->IsPaused())
				pSFXEvent->pSFX->Play(pSFXEvent->bLoop);
			pThisSFXElement = pThisSFXElement->pNext;
		}
	}
	return true;
}

bool A3DModel::RestartSFXEvents()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	StopAllSFX();

	// Now check if there is some events that want to start at -1 frame, right here;
	ALISTELEMENT * pThisSFXEventElement = m_SFXEventList.GetFirst();
	while( pThisSFXEventElement != m_SFXEventList.GetTail() )
	{
		SFXEVENT* pSFXEvent = (SFXEVENT*) pThisSFXEventElement->pData;
		if( pSFXEvent->nFrame == -1 )
		{
			if (pSFXEvent->pSFX)
			{
				pSFXEvent->pSFX->Stop();
				
				A3DVECTOR3 vecPos, vecDir;
				vecPos = pSFXEvent->vecPos;
				vecDir = pSFXEvent->vecDir;
				if( pSFXEvent->pParentFrame )
				{
					A3DMATRIX4 matParent = pSFXEvent->pParentFrame->GetAbsoluteTM();
					vecPos = vecPos * matParent;
					vecDir = vecDir * matParent - matParent.GetRow(3);
				}
				pSFXEvent->pSFX->SetPosition(vecPos);
				pSFXEvent->pSFX->SetConeOrientation(vecDir);
				pSFXEvent->pSFX->UpdateChanges();

				// For these sfx will probably moving toward us, so we have to play all
				// of them;
				if( 1 )//pSFXEvent->pSFX->CanHear() )
				{
					pSFXEvent->pSFXRecord->pCurEvent = pSFXEvent;
					pSFXEvent->pSFX->Play(pSFXEvent->bLoop);
				}
			}
		}
		pThisSFXEventElement = pThisSFXEventElement->pNext;
	}
	return true;
}

A3DModel::SFXRECORD* A3DModel::FindSFXRecord(const char * szFilename)
{
	ALISTELEMENT * pThisSFXRecordElement = m_SFXList.GetFirst();
	while( pThisSFXRecordElement != m_SFXList.GetTail() )
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*) pThisSFXRecordElement->pData;
		if( _stricmp(pSFXRecord->szSFXName, szFilename) == 0 )
			return pSFXRecord;
		
		pThisSFXRecordElement = pThisSFXRecordElement->pNext;
	}
	return NULL;
}

bool A3DModel::LoadImmEffect()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// Let all of my sound effects load its imm effects;
	ALISTELEMENT * pThisSFXRecordElement = m_SFXList.GetFirst();
	while( pThisSFXRecordElement != m_SFXList.GetTail() )
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*) pThisSFXRecordElement->pData;
		AM3DSoundBuffer* pSFX = pSFXRecord->pSFX;
		//if (pSFX)
		//	pSFX->LoadImmEffect();

		pThisSFXRecordElement = pThisSFXRecordElement->pNext;
	}
	return true;
}

bool A3DModel::UnloadImmEffect()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	// Let all of my sound effects load its imm effects;
	ALISTELEMENT * pThisSFXRecordElement = m_SFXList.GetFirst();
	while( pThisSFXRecordElement != m_SFXList.GetTail() )
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*) pThisSFXRecordElement->pData;
		AM3DSoundBuffer* pSFX = pSFXRecord->pSFX;
		//if (pSFX)
		//	pSFX->UnloadImmEffect();

		pThisSFXRecordElement = pThisSFXRecordElement->pNext;
	}
	return true;
}

bool A3DModel::SetSFXForce2D(bool bForce2D)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	ALISTELEMENT * pThisSFXRecordElement = m_SFXList.GetFirst();
	while( pThisSFXRecordElement != m_SFXList.GetTail() )
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*) pThisSFXRecordElement->pData;
		AM3DSoundBuffer* pSFX = pSFXRecord->pSFX;
		if (pSFX)
			pSFX->SetForce2D(bForce2D);

		pThisSFXRecordElement = pThisSFXRecordElement->pNext;
	}
	return true;
}

bool A3DModel::UnloadSFX()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	AMSoundBufferMan* pSoundBufMan = m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();

	ALISTELEMENT * pThisSFXRecordElement = m_SFXList.GetFirst();
	while( pThisSFXRecordElement != m_SFXList.GetTail() )
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*) pThisSFXRecordElement->pData;
		pSoundBufMan->Release3DSound(&pSFXRecord->pSFX);
		pThisSFXRecordElement = pThisSFXRecordElement->pNext;
	}
	return true;
}

bool A3DModel::ReloadSFX()
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	AMSoundBufferMan* pSoundBufMan = m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();

	ALISTELEMENT* pThisSFXRecordElement = m_SFXList.GetFirst();
	while (pThisSFXRecordElement != m_SFXList.GetTail())
	{
		SFXRECORD* pSFXRecord = (SFXRECORD*)pThisSFXRecordElement->pData;

		pSFXRecord->pSFX = pSoundBufMan->Load3DSound(pSFXRecord->szSFXName, false);
		if (!pSFXRecord->pSFX)
		{
			g_A3DErrLog.Log("A3DModel::ReloadSFX, Can not load sfx file [%s]!", pSFXRecord->szSFXName);
			continue;
		}

		pThisSFXRecordElement = pThisSFXRecordElement->pNext;
	}

	return true;
}

bool A3DModel::StopAction()
{
	DiscardActionSchedule();

	m_nAnimStart = m_nAnimEnd = 0;
	return true;
}

bool A3DModel::Reset()
{
	m_pRealParentModel = NULL;
	m_pParentModel = NULL;
	m_pParentFrame = NULL;
	m_pConnectionCenterFrame = NULL;
	
	m_matRelativeTM.Identity();
	m_matAbsoluteTM.Identity();

	m_vTimeStored = 0.0f;
	m_nFrame = 0;
	m_nFrameOld = -1;
	m_nHeartBeats = -1;

	m_nAnimStart = m_nAnimEnd = 0;
	m_bPaused = false;
	m_bAnimLoop = true;

	m_vecPos = A3DVECTOR3(0.0f);
	m_vecDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUp  = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecScale = A3DVECTOR3(1.0f, 1.0f, 1.0f);

	m_bVisible = true;
	m_pfnActionChangeCallBack = NULL;
	m_pfnActionLoopEndCallBack = NULL;
	m_pfnLogicEventCallBack = NULL;

	m_ActionScheduleList.Reset();

	m_ChildModelList.Reset();

	m_bHasMoved = true;
	m_bBuildOBBBevels = true;

	ZeroMemory(m_pProperties, PROPERTY_SIZE);

	m_bTraceMoveAABBOnly = false;
	m_dwRayTraceBits  = 0xffffffff;
	m_dwAABBTraceBits = 0xffffffff;

	m_Container	= A3DCONTAINER_NULL;
	m_bZPull = false;
	return true;
}

bool A3DModel::AddLogicEvent(int nFrame, const char * szNotifyString)
{
	LOGICEVENT* pLogicEvent = (LOGICEVENT*)a_malloc(sizeof (LOGICEVENT));
	if( NULL == pLogicEvent )
	{
		g_A3DErrLog.Log("A3DModel::AddLogicEvent() Not enough memory!");
		return false;
	}

	ZeroMemory(pLogicEvent, sizeof (LOGICEVENT));
	pLogicEvent->nFrame	= nFrame;
	strncpy(pLogicEvent->szNotifyString, szNotifyString, 32);
	m_LogicEventList.Append((LPVOID) pLogicEvent);
	return true;
}

bool A3DModel::DeleteLogicEvent(int nIndex)
{
	ALISTELEMENT * pLogicEventElement = m_LogicEventList.GetElementByOrder(nIndex);
	if( !pLogicEventElement ) return false;

	// We should release some necessary resource we have allocated;
	LOGICEVENT* pLogicEvent = (LOGICEVENT*)pLogicEventElement->pData;
	
	// free memory allocated;
	a_free(pLogicEvent);
	return m_LogicEventList.Delete(pLogicEventElement);
}

A3DModel::LOGICEVENT* A3DModel::GetLogicEvent(int nIndex)
{
	ALISTELEMENT * pLogicEventElement = m_LogicEventList.GetElementByOrder(nIndex);
	if( !pLogicEventElement ) return NULL;

	return (LOGICEVENT*) pLogicEventElement->pData;
}

bool A3DModel::CheckLogicEvents()
{
	ALISTELEMENT * pLogicEventElement = m_LogicEventList.GetFirst();
	while( pLogicEventElement != m_LogicEventList.GetTail() )
	{
		LOGICEVENT* pLogicEvent = (LOGICEVENT*) pLogicEventElement->pData;
		if( m_nFrame == pLogicEvent->nFrame )
		{
			if( m_pfnLogicEventCallBack )
			{
				(* m_pfnLogicEventCallBack)(pLogicEvent, m_pLogicArg);
			}
		}
			
		pLogicEventElement = pLogicEventElement->pNext;
	}

	return true;
}

// Ray trace model's mesh triangles;
bool A3DModel::RayTraceMesh(A3DVECTOR3& vecStart, A3DVECTOR3& vecDelta, A3DVECTOR3 * pvecPos, A3DVECTOR3 * pvecNormal, float * pvFraction)
{
	float		vMinFraction = 99999.9f;
	float		vFraction;
	A3DVECTOR3  vecHitPos;
	A3DVECTOR3	vecHitNormal;
	bool		bIntersect = false;

	// First if it is not intersect my bounding box, it will not intersect me
	if( !CLS_RayToAABB3(vecStart, vecDelta, m_ModelAABB.Mins, m_ModelAABB.Maxs, vecHitPos, &vFraction, vecHitNormal) )
		return false;

	ALISTELEMENT * pElement;
	pElement = m_ChildModelList.GetFirst();
	while( pElement != m_ChildModelList.GetTail() )
	{
		A3DModel * pModel = (A3DModel *) pElement->pData;
		if( pModel->RayTraceMesh(vecStart, vecDelta, &vecHitPos, &vecHitNormal, &vFraction) )
		{
			if( vFraction < vMinFraction )
			{
				*pvFraction = vMinFraction = vFraction;
				*pvecPos = vecHitPos;
				*pvecNormal = vecHitNormal;
				bIntersect = true;
			}
		}

		pElement = pElement->pNext;
	}

	pElement = m_ChildFrameList.GetFirst();
	while( pElement != m_ChildFrameList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pElement->pData;

		if( pFrame->RayTraceMesh(vecStart, vecDelta, &vecHitPos, &vecHitNormal, &vFraction) )
		{
			if( vFraction < vMinFraction )
			{
				*pvFraction = vMinFraction = vFraction;
				*pvecPos = vecHitPos;
				*pvecNormal = vecHitNormal;
				bIntersect = true;
			}
		}

		pElement = pElement->pNext;;
	}

	if( bIntersect )
		return true;

	return false;
}

bool A3DModel::SetScale(float vScaleX, float vScaleY, float vScaleZ)
{
	A3DVECTOR3 vecPos, vecDir, vecUp;
	if( vScaleX < 1e-4f )
		vScaleX = 1e-4f;
	if( vScaleY < 1e-4f )
		vScaleY = 1e-4f;
	if( vScaleZ < 1e-4f )
		vScaleZ = 1e-4f;

	m_vecScale = A3DVECTOR3(vScaleX, vScaleY, vScaleZ);

	vecPos.x = m_matRelativeTM._41;
	vecPos.y = m_matRelativeTM._42;
	vecPos.z = m_matRelativeTM._43;

	vecDir.x = m_matRelativeTM._31;
	vecDir.y = m_matRelativeTM._32;
	vecDir.z = m_matRelativeTM._33;
	vecDir = Normalize(vecDir);

	vecUp.x = m_matRelativeTM._21;
	vecUp.y = m_matRelativeTM._22;
	vecUp.z = m_matRelativeTM._23;
	vecUp = Normalize(vecUp);

	A3DMATRIX4 matScale = a3d_Scaling(m_vecScale.x, m_vecScale.y, m_vecScale.z);
	m_matRelativeTM = a3d_TransformMatrix(vecDir, vecUp, vecPos);
	m_matRelativeTM = matScale * m_matRelativeTM;
	
	UpdateAbsoluteTM();
	m_bHasMoved = true;
	return true;
}

bool A3DModel::SetExtraMaterial(A3DMaterial * pMaterial)
{
	// First my child models;
	if( m_ChildModelList.GetSize() )
	{
		ALISTELEMENT * pThisModelElement = m_ChildModelList.GetHead()->pNext;
		while( pThisModelElement != m_ChildModelList.GetTail() )
		{
			A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
			if( !pModel->SetExtraMaterial(pMaterial) )	
				return false;

			pThisModelElement = pThisModelElement->pNext;
		}
	}

	if( m_ChildFrameList.GetFirst() )
	{
		ALISTELEMENT * pThisElement = m_ChildFrameList.GetFirst();
		while( pThisElement != m_ChildFrameList.GetTail() )
		{
			A3DFrame * pChildFrame = (A3DFrame *) pThisElement->pData;
			if( !pChildFrame->SetExtraMaterial(pMaterial) )
				return false;

			pThisElement = pThisElement->pNext;
		}
	}
	return true;
}
