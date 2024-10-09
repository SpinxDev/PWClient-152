// SceneLightAddOperation.cpp: implementation of the CSceneLightAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "elementeditor.h"
#include "MainFrm.h"
#include "SceneLightAddOperation.h"
#include "SceneObjectManager.h"
#include "UndoLightAddAction.h"
#include "UndoLightDeleteAction.h"
#include "UndoMan.h"
#include "Render.h"
#include "A3D.h"



//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneLightAddOperation::CSceneLightAddOperation()
{

}

CSceneLightAddOperation::~CSceneLightAddOperation()
{
	m_TempLightObject.Release();
}

bool CSceneLightAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		LIGHTDATA lightData;
		
		//Transfrom 2D screen position to 3D world position
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_TempLightObject.UpdateProperty(true);
		m_TempLightObject.GetLightData(lightData);
		
		CSceneLightObject* pLight = pSManager->CreateLight(lightData);
		if(pLight)
		{
			pLight->Translate(TraceRt.vPoint);
			pLight->SetObjectID(pSManager->GenerateObjectID());
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			if(!g_Configs.bObjectAddContinue)
			{
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pLight->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pLight->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
			}else
			{
				CreateLight();
			}
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
			pMap->SetModifiedFlag(true);
		}else g_Log.Log("CSceneLightAddOperation::OnLButtonDown, Failed to create light");
		
	}
	return true;
}

bool CSceneLightAddOperation::OnLButtonUp(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

void CSceneLightAddOperation::Render(A3DViewport* pA3DViewport)
{
};

bool CSceneLightAddOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSceneLightAddOperation::CreateLight()
{
	int lightNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Light_%d",lightNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				lightNum++;
			}else 
			{
				lightNum++;
				break;
			}
		}
		m_TempLightObject.SetObjectName(name);
		m_TempLightObject.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_TempLightObject.m_pProperty);//把灯光的属性给对象属性表
		
		//AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
}

