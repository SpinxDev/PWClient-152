// SceneModelAddOperation.cpp: implementation of the SceneModelAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "StaticModelObject.h"
#include "Render.h"
#include "SceneObjectManager.h"
#include "UndoLightAddAction.h"
#include "UndoLightDeleteAction.h"
#include "UndoMan.h"
#include "MainFrm.h"
#include "a3d.h"
#include "EC_Model.h"
#include "SceneModelAddOperation.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SceneModelAddOperation::SceneModelAddOperation()
{
}

SceneModelAddOperation::~SceneModelAddOperation()
{
	m_Model.Release();
}

bool SceneModelAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	
	if(m_strModelName.IsEmpty()) return true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Model.UpdateProperty(true);
		if(pSManager) 
		{
			CStaticModelObject*pModel = new CStaticModelObject();
			if(pModel)
			{
				PESTATICMODEL pNewModel = g_ModelContainer.GetStaticModel(m_strModelName);
				if(pNewModel==NULL)
				{
					delete pModel;
					return true;
				}
				pModel->SetA3dModel(pNewModel);
				pModel->SetModelPath(m_strModelName);
				AString strHull = m_strModelName;
				strHull.CutRight(4);
				strHull = strHull + ".chf";
				pModel->SetHullPath(strHull);

				pModel->Translate(TraceRt.vPoint);
				pModel->SetObjectID(pSManager->GenerateObjectID());
				pModel->SetObjectName(m_Model.GetObjectName());
				pModel->SetShawdow(m_Model.IsShawdow());
				
				pSManager->InsertSceneObject((CSceneObject*)pModel);
				pSManager->AddObjectPtrToSelected((CSceneObject*)pModel);
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pModel->m_pProperty);
					SetStaticModel(m_strModelName);
					pModel->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					ReleaseCapture();
				}else
				{
					SetStaticModel(m_strModelName);
				}
				
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
				pMap->SetModifiedFlag(true);
			}else g_Log.Log("CSceneLightAddOperation::OnLButtonDown, Failed to create static model");
		}
	}
	return true;
}

bool SceneModelAddOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void SceneModelAddOperation::SetStaticModel(AString strName)
{ 
	m_strModelName = strName; 
	int modelNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("StaticModel_%d",modelNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				modelNum++;
			}else 
			{
				modelNum++;
				break;
			}
		}
		m_Model.SetObjectName(name);
		m_Model.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Model.m_pProperty);
	}
}

CSceneSkinModelAddOperation::CSceneSkinModelAddOperation()
{
}

CSceneSkinModelAddOperation::~CSceneSkinModelAddOperation()
{
	m_ECModel.Release();
}

bool CSceneSkinModelAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_ECModel.UpdateProperty(true);
		if(pSManager) 
		{
			CSceneSkinModel *pECModel = new CSceneSkinModel();
			if(pECModel)
			{
				bool bLoaded = pECModel->LoadSkinModel(m_strModelPath);
				if(!bLoaded)
				{
					AString msg("加载SkinModel失败！ --");
					msg += m_strModelPath;
					AfxMessageBox(msg);
					delete pECModel;
					return true;
				}
				pECModel->Translate(TraceRt.vPoint);
				pECModel->SetObjectID(pSManager->GenerateObjectID());
				pECModel->SetObjectName(m_ECModel.GetObjectName());
				pECModel->SetProperty(m_ECModel.GetProperty());
				pECModel->BuildPropertyEx();

				pSManager->InsertSceneObject((CSceneObject*)pECModel);
				pSManager->AddObjectPtrToSelected((CSceneObject*)pECModel);
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pECModel->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
					pECModel->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					ReleaseCapture();
				}else
				{
					SetSkinModel(m_strModelPath);
				}
				
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
				pMap->SetModifiedFlag(true);
			}else g_Log.Log("CSceneLightAddOperation::OnLButtonDown, Failed to create static model");
		}
	}
	return true;
}

bool CSceneSkinModelAddOperation::OnMouseMove( int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}



void CSceneSkinModelAddOperation::SetSkinModel(AString strPathName)
{
	m_strModelPath = strPathName;
	int modelNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("SkinModel_%d",modelNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				modelNum++;
			}else 
			{
				modelNum++;
				break;
			}
		}
		m_ECModel.SetObjectName(name);
		EC_SKINMODEL_PROPERTY esp;
		esp.dwActionID = 0;
		esp.strActionName = "";
		esp.strPathName = strPathName;
		esp.fAlphaFlags = -1.0f;
		m_ECModel.SetProperty(esp);
		m_ECModel.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_ECModel.m_pProperty);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//class 

CSceneCritterAddOperation::CSceneCritterAddOperation()
{
}

CSceneCritterAddOperation::~CSceneCritterAddOperation()
{
	m_CritterGroup.Release();
}

bool CSceneCritterAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CRITTER_DATA critterData;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_CritterGroup.UpdateProperty(true);
		critterData = m_CritterGroup.GetProperty();
		
		CCritterGroup* pCritter = pSManager->CreateCritterGroup(m_CritterGroup.GetObjectName());
		if(pCritter)
		{
			pCritter->Translate(TraceRt.vPoint);
			pCritter->SetProperty(critterData);
			pCritter->SetObjectID(pSManager->GenerateObjectID());
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			if(!g_Configs.bObjectAddContinue)
			{
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pCritter->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pCritter->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
			}else
			{
				CreateCritter();
			}
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
			pMap->SetModifiedFlag(true);
		}else g_Log.Log("CSceneLightAddOperation::OnLButtonDown, Failed to create critter group!");
		
	}
	return true;
}

bool CSceneCritterAddOperation::OnMouseMove( int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSceneCritterAddOperation::CreateCritter()
{
	int CritterNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("CritterGroup_%d",CritterNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				CritterNum++;
			}else 
			{
				CritterNum++;
				break;
			}
		}
		m_CritterGroup.SetObjectID(pSManager->GenerateObjectID());
		m_CritterGroup.SetObjectName(name);
		m_CritterGroup.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_CritterGroup.m_pProperty);
	}
}


