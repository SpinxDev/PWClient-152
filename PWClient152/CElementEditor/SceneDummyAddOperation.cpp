// SceneDummyAddOperation.cpp: implementation of the CSceneDummyAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneDummyAddOperation.h"
#include "SceneDummyObject.h"
#include "SceneObjectManager.h"
#include "UndoMan.h"
#include "UndoLightAddAction.h"
#include "Render.h"
#include "A3D.h"
#include "MainFrm.h"


//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneDummyAddOperation::CSceneDummyAddOperation()
{

}

CSceneDummyAddOperation::~CSceneDummyAddOperation()
{
	m_tempDummy.Release();
}

bool CSceneDummyAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint))
		{
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//¶ÁÊôÐÔÊý¾Ý
			m_tempDummy.UpdateProperty(true);
			
			CSceneDummyObject* pDummy = pSManager->CreateDummy(m_tempDummy.GetObjectName());
			if(pDummy)
			{
				pDummy->SetObjectName(m_tempDummy.GetObjectName());
				pDummy->Translate(TraceRt.vPoint);
				pDummy->SetObjectID(pSManager->GenerateObjectID());
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pDummy->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
					pDummy->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
					ReleaseCapture();
				}else
				{
					CreateDummy();
				}
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
				pMap->SetModifiedFlag(true);
			}else g_Log.Log("CSceneDummyAddOperation::OnLButtonDown, Failed to create dummy object");
		}
	}
	return true;
}

bool CSceneDummyAddOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSceneDummyAddOperation::CreateDummy()
{
	
	int DummyNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Dummy_%d",DummyNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				DummyNum++;
			}else 
			{
				DummyNum++;
				break;
			}
		}
		m_tempDummy.SetObjectName(name);
		m_tempDummy.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_tempDummy.m_pProperty);
	}
}
