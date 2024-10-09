// SceneAreaAddOperation.cpp: implementation of the CSceneAreaAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneAreaAddOperation.h"
#include "Render.h"
#include "A3D.h"
#include "SceneObjectManager.h"
#include "UndoLightAddAction.h"
#include "UndoMan.h"
#include "MainFrm.h"



//#define new A_DEBUG_NEW
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneAreaAddOperation::CSceneAreaAddOperation()
{

}

CSceneAreaAddOperation::~CSceneAreaAddOperation()
{
	m_Area.Release();
}

bool CSceneAreaAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
			m_Area.UpdateProperty(true);
			
			CSceneBoxArea* pArea = pSManager->CreateBoxArea(m_Area.GetObjectName());
			if(pArea)
			{
				BOXAREA_PROPERTY data;
				m_Area.GetProperty(data);
				pArea->SetProperty(data);
				pArea->Translate(TraceRt.vPoint);
				pArea->SetObjectID(pSManager->GenerateObjectID());
				pArea->InitNatureObjects();
				
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pArea->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
					pArea->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
					ReleaseCapture();
				}else
				{
					CreateArea();
				}
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
				pMap->SetModifiedFlag(true);
			}else g_Log.Log("CSceneAreaAddOperation::OnLButtonDown, Failed to create box area");
		}
	}
	return true;
}

bool CSceneAreaAddOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSceneAreaAddOperation::CreateArea()
{
	
	int BoxAreaNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("BoxArea_%d",BoxAreaNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				BoxAreaNum++;
			}else 
			{
				BoxAreaNum++;
				break;
			}
		}
		m_Area.SetObjectName(name);
		m_Area.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Area.m_pProperty);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneInstanceBoxAddOperation::CSceneInstanceBoxAddOperation()
{

}

CSceneInstanceBoxAddOperation::~CSceneInstanceBoxAddOperation()
{
	m_InstanceBox.Release();
}

bool CSceneInstanceBoxAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
			m_InstanceBox.UpdateProperty(true);
			
			SceneInstanceBox* pNew = pSManager->CreateInstanceBox(m_InstanceBox.GetObjectName());
			if(pNew)
			{
				INSTANCE_DATA data;
				data = m_InstanceBox.GetProperty();
				pNew->SetProperty(data);
				pNew->Translate(TraceRt.vPoint);
				pNew->SetObjectID(pSManager->GenerateObjectID());
				
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNew->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
					pNew->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
					ReleaseCapture();
				}else
				{
					CreateInstanceBox();
				}
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
				pMap->SetModifiedFlag(true);
			}else g_Log.Log("CSceneInstanceBoxAddOperation::OnLButtonDown, Failed to create instance box!");
		}
	}
	return true;
}

bool CSceneInstanceBoxAddOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSceneInstanceBoxAddOperation::CreateInstanceBox()
{
	
	int BoxAreaNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("InstanceBox_%d",BoxAreaNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				BoxAreaNum++;
			}else 
			{
				BoxAreaNum++;
				break;
			}
		}
		m_InstanceBox.SetObjectName(name);
		m_InstanceBox.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_InstanceBox.m_pProperty);
	}
}