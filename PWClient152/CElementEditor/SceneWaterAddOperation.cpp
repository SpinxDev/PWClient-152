// SceneWaterAddOperation.cpp: implementation of the CSceneWaterAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "SceneWaterAddOperation.h"
#include "Render.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"
#include "UndoLightAddAction.h"
#include "UndoMan.h"
#include "a3d.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneWaterAddOperation::CSceneWaterAddOperation() : m_Water(NULL)
{
}

CSceneWaterAddOperation::~CSceneWaterAddOperation()
{
	m_Water.Release();
}

bool CSceneWaterAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_Water.SetSceneManager(AUX_GetMainFrame()->GetMap()->GetSceneObjectMan());

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		//Transfrom 2D screen position to 3D world position
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(pMap->GetTerrain()->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt))
		{
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//¶ÁÊôÐÔÊý¾Ý
			m_Water.UpdateProperty(true);
			int waterID = pSManager->m_IDGenerator.GenerateID();
			CSceneWaterObject* pWater = pSManager->CreateWater(waterID,m_Water.GetObjectName());
			if(pWater)
			{
				int nAreaX,nAreaZ;
				float fGridSize;
				DWORD dwColor;
				float fWaveSize;
				m_Water.GetProperty(nAreaX,nAreaZ,fGridSize,fWaveSize,dwColor);
				pWater->SetProperty(nAreaX,nAreaZ,fGridSize,fWaveSize,dwColor);
				pWater->Translate(TraceRt.vPoint);
				
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pWater->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
					ReleaseCapture();
				}else
				{
					CreateWater();
				}
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
				pMap->SetModifiedFlag(true);
				pWater->UpdateProperty(false);
				pWater->BuildRenderWater();
			}else g_Log.Log("CSceneWaterAddOperation::OnLButtonDown, Failed to create water surface");
		}
	}
	return true;
}

bool CSceneWaterAddOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CSceneWaterAddOperation::CreateWater()
{
	
	int WaterNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Water_%d",WaterNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				WaterNum++;
			}else 
			{
				WaterNum++;
				break;
			}
		}
		m_Water.SetObjectName(name);
		m_Water.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Water.m_pProperty);
	}
}


