// AIGeneratorOperation.cpp: implementation of the CAIGeneratorOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "AIGeneratorOperation.h"
#include "SceneObjectManager.h"
#include "UndoLightAddAction.h"
#include "UndoMan.h"
#include "Render.h"
#include "A3D.h"
#include "MainFrm.h"
#include "SceneGatherObject.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAIGeneratorOperation::CAIGeneratorOperation()
{
	m_bDrawRect = false;
}

CAIGeneratorOperation::~CAIGeneratorOperation()
{
	m_AI.Release();
}

bool CAIGeneratorOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_bDrawRect = false;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		AI_DATA aiData;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);

		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_AI.UpdateProperty(true);
		aiData = m_AI.GetProperty();
		if(aiData.dwAreaType==CSceneAIGenerator::TYPE_BOX)
		{//不需要在地图上绘制矩形
			
			CSceneAIGenerator* pNewAI = pSManager->CreateAIGenerator(m_AI.GetObjectName());
			if(pNewAI)
			{
				pNewAI->Translate(TraceRt.vPoint);
				pNewAI->SetProperty(aiData);
				pNewAI->SetObjectID(pSManager->GenerateObjectID());
				pNewAI->SetCopyFlags(g_Configs.nShowCopyNum);
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				if(!g_Configs.bObjectAddContinue)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewAI->m_pProperty);//把灯光的属性给对象属性表
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
					pNewAI->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
					ReleaseCapture();
				}else
				{
					CreateAI();
				}
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
				pMap->SetModifiedFlag(true);
			}else g_Log.Log("CAIGeneratorOperation::OnLButtonDown, Failed to create AI area!");
		}else
		{//需要在地形上绘制矩形
			m_bDrawRect = true;
			m_vStartPoint = TraceRt.vPoint;
			m_vEndPoint = m_vStartPoint;
		}
	}
	return true;
}

bool CAIGeneratorOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if(m_bDrawRect)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		A3DVECTOR3 vDelta = m_vEndPoint - m_vStartPoint;
		float len = fabs(vDelta.x);
		float wdt = fabs(vDelta.z);
		if(pMap && len>0.0f && wdt>0.0f)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CSceneAIGenerator* pNewAI = pSManager->CreateAIGenerator(m_AI.GetObjectName());
			if(pNewAI)
			{
				AI_DATA data = m_AI.GetProperty();
				pNewAI->Translate(m_AI.GetPos());
				pNewAI->SetProperty(data);
				pNewAI->SetObjectID(pSManager->GenerateObjectID());
				pNewAI->SetCopyFlags(g_Configs.nShowCopyNum);
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewAI->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pNewAI->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
				pMap->SetModifiedFlag(true);
			}
		}
	}
	m_bDrawRect = false;
	return true;
}

bool CAIGeneratorOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(m_bDrawRect)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			
			RAYTRACERT TraceRt;	//	Used to store trace result
			if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
			m_vEndPoint = TraceRt.vPoint;
		}
	}
	return true;
}

void CAIGeneratorOperation::CreateAI()
{
	int AiNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("AiGenerator_%d",AiNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				AiNum++;
			}else 
			{
				AiNum++;
				break;
			}
		}
		m_AI.SetObjectName(name);
		AI_DATA data;
		init_ai_data(data);
		m_AI.SetProperty(data);
		m_AI.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_AI.m_pProperty);
	}
}

void CAIGeneratorOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_bDrawRect)
	{
		g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(false);
		DrawRect(pA3DViewport);
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		pWireCollector->Flush();
		g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(true);
	}
}

void CAIGeneratorOperation::DrawRect(A3DViewport *pViewport)
{
	A3DVECTOR3 vDelta = m_vEndPoint - m_vStartPoint;
	float len = fabs(vDelta.x);
	float wdt = fabs(vDelta.z);
	A3DVECTOR3 vCenter = m_vStartPoint + vDelta/2.0f;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap) vCenter.y = pMap->GetTerrain()->GetPosHeight(vCenter) + 0.1f;
	
	AI_DATA data = m_AI.GetProperty();
	if(len>0.0f && wdt>0.0f)
	{
		data.fEdgeLen = len;
		data.fEdgeWth = wdt;
		m_AI.SetPos(vCenter);
		m_AI.SetProperty(data);
		m_AI.Render(pViewport);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
//class CSceneGatherOperation
CSceneGatherOperation::CSceneGatherOperation()
{
	m_bDrawRect = false;
}

CSceneGatherOperation::~CSceneGatherOperation()
{
	m_Gather.Release();
}

bool CSceneGatherOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_bDrawRect = false;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		GATHER_DATA gData;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);

		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Gather.UpdateProperty(true);
		gData = m_Gather.GetProperty();
		m_bDrawRect = true;
		m_vStartPoint = TraceRt.vPoint;
		m_vEndPoint = m_vStartPoint;
	}
	return true;
}

bool CSceneGatherOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if(m_bDrawRect)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		A3DVECTOR3 vDelta = m_vEndPoint - m_vStartPoint;
		float len = fabs(vDelta.x);
		float wdt = fabs(vDelta.z);
		if(pMap && len>0.0f && wdt>0.0f)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CSceneGatherObject* pNew = pSManager->CreateGather(m_Gather.GetObjectName());
			if(pNew)
			{
				GATHER_DATA data = m_Gather.GetProperty();
				pNew->Translate(m_Gather.GetPos());
				pNew->SetProperty(data);
				pNew->SetObjectID(pSManager->GenerateObjectID());
				pNew->SetCopyFlags(g_Configs.nShowCopyNum);
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNew->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pNew->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
				pMap->SetModifiedFlag(true);
			}
		}
	}
	m_bDrawRect = false;
	return true;
}

bool CSceneGatherOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(m_bDrawRect)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			
			RAYTRACERT TraceRt;	//	Used to store trace result
			if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
			m_vEndPoint = TraceRt.vPoint;
		}
	}
	return true;
}

void CSceneGatherOperation::CreateGather()
{
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Gather_%d",Num);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				Num++;
			}else 
			{
				Num++;
				break;
			}
		}
		m_Gather.SetObjectName(name);
		GATHER_DATA data;
		Init_Gather_Data(data);
		m_Gather.SetProperty(data);
		m_Gather.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Gather.m_pProperty);
	}
}

void CSceneGatherOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_bDrawRect)
	{
		g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(false);
		DrawRect(pA3DViewport);
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		pWireCollector->Flush();
		g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(true);
	}
}

void CSceneGatherOperation::DrawRect(A3DViewport *pViewport)
{
	A3DVECTOR3 vDelta = m_vEndPoint - m_vStartPoint;
	float len = fabs(vDelta.x);
	float wdt = fabs(vDelta.z);
	A3DVECTOR3 vCenter = m_vStartPoint + vDelta/2.0f;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap) vCenter.y = pMap->GetTerrain()->GetPosHeight(vCenter) + 0.1f;
	
	GATHER_DATA data = m_Gather.GetProperty();
	if(len>0.0f && wdt>0.0f)
	{
		data.fEdgeLen = len;
		data.fEdgeWth = wdt;
		m_Gather.SetPos(vCenter);
		m_Gather.SetProperty(data);
		m_Gather.Render(pViewport);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//class CFixedNpcGeneratorOperation

CFixedNpcGeneratorOperation::CFixedNpcGeneratorOperation()
{
/*	m_pData = new BYTE[OBS_MAP_WIDTH*OBS_MAP_WIDTH*4];
	ASSERT(m_pData);
	memset(m_pData,255,OBS_MAP_WIDTH*OBS_MAP_WIDTH*4);*/
	m_bLoad = false;
	m_pSprite = 0;
	m_iObsMapWidth = 0;
	m_pData = NULL;
}

CFixedNpcGeneratorOperation::~CFixedNpcGeneratorOperation()
{
	m_Npc.Release();
	if(m_pData) 
	{
		delete []m_pData;
		m_pData = NULL;
	}
	if(m_pSprite) 
	{
		m_pSprite->Release();
		delete m_pSprite;
	}

}

void CFixedNpcGeneratorOperation::TransObstructData()
{
	// 准备装载已经计算好的可达图
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(!pMap) return;
	
	if(!pMap->GetPassMapGen()->GetPassMap())
	{
		// Test if we have generated the pass map in the directory!
		CString strRMapPath, strDHMapPath;
		strRMapPath.Format("%s%s\\%s\\%s.rmap",g_szWorkDir,g_szEditMapDir,pMap->GetMapName(),pMap->GetMapName());
		strDHMapPath.Format("%s%s\\%s\\%s.dhmap",g_szWorkDir,g_szEditMapDir,pMap->GetMapName(),pMap->GetMapName());
		
		FILE *fRMap=fopen(strRMapPath, "rb");
		FILE *fDHMap=fopen(strDHMapPath, "rb");
		
		if(!(fRMap && fDHMap))
		{
			if(!pMap->GetPassMapGen()->GetPassMap()) 
			{
				m_bLoad = false;
				return;	// 如果尚未有任何可达图，则直接返回
			}
		}
		
		// 装载可达图，高度图
		CBitImage ReachMap;
		CBlockImage<FIX16> DHMap;
		if(!(ReachMap.Load(fRMap) && DHMap.Load(fDHMap)))
		{
			if(!pMap->GetPassMapGen()->GetPassMap()) 
			{
				fclose(fRMap);
				fclose(fDHMap);
				m_bLoad = false;
				return;	// 如果尚未有任何可达图，则直接返回
			}
			
		}
		
		fclose(fRMap);
		fclose(fDHMap);
		if(!pMap->GetPassMapGen()->ImportMaps(&ReachMap, &DHMap))
		{
			if(!pMap->GetPassMapGen()->GetPassMap()) 
			{
				m_bLoad = false;
				return;	// 如果尚未有任何可达图，则直接返回
			}
		}
	}

	m_pSprite->Release();
	A3DRECT rc[OBS_MAP_DIVIDE*OBS_MAP_DIVIDE];
	int nWidth = m_iObsMapWidth/OBS_MAP_DIVIDE;
	for(int i = 0; i < OBS_MAP_DIVIDE; i++)
		for( int j = 0; j < OBS_MAP_DIVIDE; j++)
			rc[i*OBS_MAP_DIVIDE + j] = A3DRECT(nWidth*j,i*nWidth,nWidth*(j+1),nWidth*(i+1));
	m_pSprite->InitWithoutSurface(g_Render.GetA3DDevice(),m_iObsMapWidth,m_iObsMapWidth,A3DFMT_X8R8G8B8,OBS_MAP_DIVIDE*OBS_MAP_DIVIDE,rc);
	m_pSprite->SetColor(A3DCOLORRGBA(255,255,255,120));
				
	UCHAR *pData = pMap->GetPassMapGen()->GetPassMap();	
	if(pData==NULL) return;
	for(int h = 0; h < m_iObsMapWidth; h ++)
	{
		for( int w = 0; w < m_iObsMapWidth; w ++)
		{
			int idx = h*m_iObsMapWidth*4 + w*4;
			
			int idh= pMap->GetPassMapGen()->GetDeltaHeightMap()[h * m_iObsMapWidth + w];
			
			if(pData[h * m_iObsMapWidth + w]==0)
			{
				m_pData[idx + 0] = 255;
				m_pData[idx + 1] = 0;
				m_pData[idx + 2] = 0;
			}else if(pData[h * m_iObsMapWidth + w]==1)
			{
				if(idh)
				{
					m_pData[idx + 0] = 255;
					m_pData[idx + 1] = 128;
					m_pData[idx + 2] = 0;
					m_pData[idx + 3] = 255;
				}else
				{
					m_pData[idx + 0] = 0;
					m_pData[idx + 1] = 255;
					m_pData[idx + 2] = 0;
					m_pData[idx + 3] = 255;
				}
			}else
			{
				m_pData[idx + 0] = 0;
				m_pData[idx + 1] = 0;
				m_pData[idx + 2] = 255;
				m_pData[idx + 3] = 255;
			}
		}
	}
	m_bLoad = true;
}


bool CFixedNpcGeneratorOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		FIXED_NPC_DATA npcData;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Npc.UpdateProperty(true);
		npcData = m_Npc.GetProperty();
		
		CSceneFixedNpcGenerator* pNewNpc = pSManager->CreateFixedNpc(m_Npc.GetObjectName());
		if(pNewNpc)
		{
			pNewNpc->Translate(TraceRt.vPoint);
			pNewNpc->SetProperty(npcData);
			pNewNpc->SetObjectID(pSManager->GenerateObjectID());
			pNewNpc->SetCopyFlags(g_Configs.nShowCopyNum);
			
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			if(!g_Configs.bObjectAddContinue)
			{
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewNpc->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pNewNpc->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
			}else
			{
				CreateNpc();
			}
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
			pMap->SetModifiedFlag(true);
		}
	}
	return true;
}

bool CFixedNpcGeneratorOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CFixedNpcGeneratorOperation::Render(A3DViewport *pA3DViewport)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL || !m_bLoad) return;

	CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
	CRenderWnd *pRWnd = pFrame->GetRenderWnd();
	
	if(pRWnd->GetActiveViewport() != VIEW_XZ) return;
	
	CTerrain *pTerrain =  pMap->GetTerrain();
	ARectF rc = pTerrain->GetTerrainArea();

	float fScaleX,fScaleY;
	A3DVECTOR3 vLeftUp,vRightUp,vLeftBottom,vRightBottom;
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.left,0,rc.top), vLeftUp);
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.right,0,rc.top), vRightUp);
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.left,0,rc.bottom), vLeftBottom);
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.right,0,rc.bottom), vRightBottom);
	
	float w,h;
	w = vRightUp.x - vLeftUp.x + 1;
	h = vLeftBottom.y - vLeftUp.y + 1;
	
	m_pSprite->UpdateTextures(m_pData,m_iObsMapWidth*4,A3DFMT_X8R8G8B8);
	
	float		x, y;
	int			nX, nY;
	
	float		wc = w / OBS_MAP_DIVIDE;
	float		hc = h / OBS_MAP_DIVIDE;
	
	y = vLeftUp.y;
	for(int i=0; i<OBS_MAP_DIVIDE; i++)
	{
		x = vLeftUp.x;
		nY = (int) y;
		for(int j=0; j<OBS_MAP_DIVIDE; j++)
		{
			nX = (int) x;
			m_pSprite->SetPosition(nX, nY);
			
			fScaleX = (float)(int(x + wc) - nX) / (m_iObsMapWidth /OBS_MAP_DIVIDE);
			fScaleY = (float)(int(y + hc) - nY) / (m_iObsMapWidth /OBS_MAP_DIVIDE);
			
			m_pSprite->SetScaleX(fScaleX);
			m_pSprite->SetScaleY(fScaleY);
			m_pSprite->SetCurrentItem(i * OBS_MAP_DIVIDE + j);
			m_pSprite->DrawToBack();	
			x += wc;
		}
		y += hc;
	}
	COperation::Render(pA3DViewport);
}

void CFixedNpcGeneratorOperation::CreateNpc()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	
	CTerrain *pTerrain =  pMap->GetTerrain();
	m_iObsMapWidth = (int)(pTerrain->GetTerrainSize());
	
	if(m_pData) delete []m_pData;
	
	m_pData = new BYTE[m_iObsMapWidth*m_iObsMapWidth*4];
	ASSERT(m_pData);
	memset(m_pData,255,m_iObsMapWidth*m_iObsMapWidth*4);

	if(m_pSprite == 0) m_pSprite = new A2DSprite;
	int AiNum = 0;
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("NpcGenerator_%d",AiNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				AiNum++;
			}else 
			{
				AiNum++;
				break;
			}
		}
		m_Npc.SetObjectName(name);
		FIXED_NPC_DATA data;
		init_fixed_npc_data(data);
		m_Npc.SetProperty(data);
		m_Npc.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Npc.m_pProperty);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//class CPrecinctGeneratorOperation

CPrecinctOperation::CPrecinctOperation()
{
}

CPrecinctOperation::~CPrecinctOperation()
{
	m_Precinct.Release();
}

bool CPrecinctOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		PRECINCT_DATA dat;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);

		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Precinct.UpdateProperty(true);
		CScenePrecinctObject* pNew = pSManager->CreatePrecinct(m_Precinct.GetObjectName());
		if(pNew)
		{
			PRECINCT_DATA data = m_Precinct.GetProperty();
			pNew->Translate(TraceRt.vPoint);
			pNew->SetProperty(data);
			pNew->SetObjectID(pSManager->GenerateObjectID());
			
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			
			AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNew->m_pProperty);//把灯光的属性给对象属性表
			AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
			pNew->UpdateProperty(false);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
			ReleaseCapture();
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
			pMap->SetModifiedFlag(true);
		}
	}
	return true;
}

bool CPrecinctOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPrecinctOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	return true;
}

void CPrecinctOperation::CreatePrecinct()
{
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Precinct_%d",Num);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				Num++;
			}else 
			{
				Num++;
				break;
			}
		}
		m_Precinct.SetObjectName(name);
		PRECINCT_DATA data;
		init_precinct_data(data);
		m_Precinct.SetProperty(data);
		m_Precinct.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Precinct.m_pProperty);
	}
}

void CPrecinctOperation::Render(A3DViewport* pA3DViewport)
{
}


///////////////////////////////////////////////////////////////////////////////////////
//class RangeOperation

CRangeOperation::CRangeOperation()
{
}

CRangeOperation::~CRangeOperation()
{
	m_Range.Release();
}

bool CRangeOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Range.UpdateProperty(true);
		CSceneRangeObject* pNew = pSManager->CreateRange(m_Range.GetObjectName());
		if(pNew)
		{
			pNew->Translate(TraceRt.vPoint);
			pNew->SetObjectID(pSManager->GenerateObjectID());
			
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			
			AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNew->m_pProperty);//把灯光的属性给对象属性表
			AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
			pNew->UpdateProperty(false);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
			ReleaseCapture();
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
			pMap->SetModifiedFlag(true);
		}
	}
	return true;
}

bool CRangeOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CRangeOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CRangeOperation::CreateRange()
{
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Range_%d",Num);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				Num++;
			}else 
			{
				Num++;
				break;
			}
		}
		m_Range.SetObjectName(name);
		m_Range.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Range.m_pProperty);
	}
}

void CRangeOperation::Render(A3DViewport* pA3DViewport)
{
}


///////////////////////////////////////////////////////////////////////////////////////
//class cloud operation

CCloudBoxOperation::CCloudBoxOperation()
{
	m_bDrawRect   = false;
}

CCloudBoxOperation::~CCloudBoxOperation()
{
	m_Cloud.Release();
}

bool CCloudBoxOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_Cloud.UpdateProperty(true);
		
		m_bDrawRect = true;
		m_vStartPoint = TraceRt.vPoint;
		m_vEndPoint = m_vStartPoint;
	}
	return true;
}

bool CCloudBoxOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if(m_bDrawRect)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CCloudBox* pNew = pSManager->CreateCloudBox(m_Cloud.GetObjectName());
			if(pNew)
			{
				pNew->Translate(m_Cloud.GetPos());
				pNew->SetObjectID(pSManager->GenerateObjectID());
				pNew->SetProperty(m_Cloud.GetProperty());
				//for undo
				CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
				pUndo->GetSeletedListData();
				g_UndoMan.AddUndoAction(pUndo);
				
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNew->m_pProperty);//把灯光的属性给对象属性表
				AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
				pNew->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
				ReleaseCapture();
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
				pMap->SetModifiedFlag(true);
			}
		}
		m_bDrawRect = false;
	}
	return true;
}

bool CCloudBoxOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(m_bDrawRect)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			
			RAYTRACERT TraceRt;	//	Used to store trace result
			if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
			m_vEndPoint = TraceRt.vPoint;
		}
	}
	return true;
}

void CCloudBoxOperation::CreateCloud()
{
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("CloudBox_%d",Num);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				Num++;
			}else 
			{
				Num++;
				break;
			}
		}
		m_Cloud.SetObjectName(name);
		CLOUD_BOX_DATA dat = m_Cloud.GetProperty();
		dat.dwRandSeed = 9999999 * rand()/RAND_MAX;
		m_Cloud.SetProperty(dat);

		m_Cloud.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_Cloud.m_pProperty);
	}
}

void CCloudBoxOperation::Render(A3DViewport* pA3DViewport)
{
	CLOUD_BOX_DATA dat = m_Cloud.GetProperty();
	A3DVECTOR3 vpos;
	if(m_bDrawRect)
	{
		dat.fEdgeLen = fabs(m_vStartPoint.x - m_vEndPoint.x);
		dat.fEdgeWth = fabs(m_vStartPoint.z - m_vEndPoint.z);
		vpos = m_vEndPoint + (m_vStartPoint - m_vEndPoint)/2.0f;
		vpos.y = m_vStartPoint.y + 20.0f;
		m_Cloud.SetPos(vpos);
		m_Cloud.SetProperty(dat);
		m_Cloud.SetSelected(true);
		m_Cloud.Render(pA3DViewport);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//class DynamicObjectOperation

CDynamicObjectOperation::CDynamicObjectOperation()
{
}

CDynamicObjectOperation::~CDynamicObjectOperation()
{
	m_DynamicObject.Release();
}

bool CDynamicObjectOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
		if(!pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint)) return true;
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
		m_DynamicObject.UpdateProperty(true);
		CSceneDynamicObject* pNew = pSManager->CreateDynamic(m_DynamicObject.GetObjectName());
		if(pNew)
		{
			pNew->Translate(TraceRt.vPoint);
			pNew->SetObjectID(pSManager->GenerateObjectID());
			
			//for undo
			CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
			pUndo->GetSeletedListData();
			g_UndoMan.AddUndoAction(pUndo);
			
			AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNew->m_pProperty);//把灯光的属性给对象属性表
			AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();//恢复所有的按纽
			pNew->UpdateProperty(false);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
			ReleaseCapture();
			AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
			pMap->SetModifiedFlag(true);
		}
	}
	return true;
}

bool CDynamicObjectOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CDynamicObjectOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CDynamicObjectOperation::CreateDynamic()
{
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Dynamic_%d",Num);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				Num++;
			}else 
			{
				Num++;
				break;
			}
		}
		m_DynamicObject.SetObjectName(name);
		m_DynamicObject.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_DynamicObject.m_pProperty);
	}
}

void CDynamicObjectOperation::Render(A3DViewport* pA3DViewport)
{
}


