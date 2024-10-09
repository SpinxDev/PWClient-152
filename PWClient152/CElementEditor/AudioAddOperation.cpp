// AudioAddOperation.cpp: implementation of the CAudioAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneObjectManager.h"
#include "AudioAddOperation.h"
#include "Render.h"
#include "A3D.h"
#include "UndoLightAddAction.h"
#include "UndoMan.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioAddOperation::CAudioAddOperation()
{

}

CAudioAddOperation::~CAudioAddOperation()
{
	m_tempAudio.Release();
}

bool CAudioAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
	
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		GFX_AUDIO_DATA audioData;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_tempAudio.SetEnableAudio(false);
		m_tempAudio.UpdateProperty(true);
		audioData = m_tempAudio.GetProperty();
		CSceneAudioObject* pAudio = pSManager->CreateAudio(m_tempAudio.GetObjectName(),audioData);
		if(pAudio)
		{
			pAudio->Translate(TraceRt.vPoint);
			pAudio->SetObjectID(pSManager->GenerateObjectID());
			pAudio->SetEnableAudio(true);
			pAudio->ReBuildAudio();
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			if(!g_Configs.bObjectAddContinue)
			{
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pAudio->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pAudio->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
			}else
			{
				CreateAudio();
			}
			pMap->SetModifiedFlag(true);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
		}else g_Log.Log("CSceneAudioAddOperation::OnLButtonDown, Failed to create audio");
		
	}
	return true;	
}

void CAudioAddOperation::CreateAudio()
{
	int AudioNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Audio_%d",AudioNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				AudioNum++;
			}else 
			{
				AudioNum++;
				break;
			}
		}
		m_tempAudio.SetObjectName(name);
		m_tempAudio.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_tempAudio.m_pProperty);
	}
}

bool CAudioAddOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}
