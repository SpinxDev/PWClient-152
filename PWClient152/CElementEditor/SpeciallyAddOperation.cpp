// SpeciallyAddOperation.cpp: implementation of the CSpeciallyAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneObjectManager.h"
#include "SpeciallyAddOperation.h"
#include "UndoLightAddAction.h"
#include "UndoMan.h"
#include "A3D.h"
#include "render.h"
#include "MainFrm.h"



//#define new A_DEBUG_NEW


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpeciallyAddOperation::CSpeciallyAddOperation()
{
	m_Specially.DisableLoad();
}

CSpeciallyAddOperation::~CSpeciallyAddOperation()
{
	m_Specially.Release();
}

bool CSpeciallyAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
		//if(pMap->GetTerrain()->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt))
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;

		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Specially.UpdateProperty(true);
		CSceneSpeciallyObject* pSpecially = pSManager->CreateSpecially(m_Specially.GetObjectName());
		if(pSpecially)
		{
			pSpecially->Translate(TraceRt.vPoint);
			pSpecially->UpdateGfx();
			pSpecially->SetObjectID(pSManager->GenerateObjectID());
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			if(!g_Configs.bObjectAddContinue)
			{
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pSpecially->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pSpecially->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
			}else
			{
				CreateSpecially();
			}
			pMap->SetModifiedFlag(true);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
		}else g_Log.Log("CSpeciallyAddOperation::OnLButtonDown, Failed to create specially object");
		
	}
	return true;
}

bool CSpeciallyAddOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSpeciallyAddOperation::CreateSpecially()
{
	int SpeciallyNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("GFX_%d",SpeciallyNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				SpeciallyNum++;
			}else 
			{
				SpeciallyNum++;
				break;
			}
		}
		m_Specially.SetObjectName(name);
		m_Specially.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Specially.m_pProperty);
	}
}
