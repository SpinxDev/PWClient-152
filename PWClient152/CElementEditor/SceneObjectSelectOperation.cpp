// SceneObjectSelectOperation.cpp: implementation of the CSceneObjectSelectOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "MainFrm.h"
#include "elementeditor.h"
#include "SceneObjectSelectOperation.h"
#include "A3D.h"
#include "render.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneObjectSelectOperation::CSceneObjectSelectOperation()
{
	m_nDirectFlag = 0;
	m_bDrag = false;
}

CSceneObjectSelectOperation::~CSceneObjectSelectOperation()
{

}

bool CSceneObjectSelectOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			pManager->m_coordinateDirection.SetPos(vCoorPos);
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.OnMouseMove(x,y,dwFlags);
		}

		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		if(pManager->RayTrace(vStart,vPos,false))
		{
			SetCursor(g_hSelectCursor);
		}
	}
	return true;
}

bool CSceneObjectSelectOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_bDrag = true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			PSCENEOBJECT ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			pManager->m_coordinateDirection.SetPos(ptemp->GetPos());
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.OnLButtonDown(x,y,dwFlags);
			m_nDirectFlag = pManager->m_coordinateDirection.GetSelectedFlags();
		}
		
		if( m_nDirectFlag==0)
		{
			SHORT s = GetKeyState(VK_LCONTROL);
			if(s >= 0)
				pManager->ClearSelectedList();//clear selected list
			pManager->RayTrace(vStart,vPos);
			
			if(pManager->m_listSelectedObject.GetCount()==1)
			{
				PSCENEOBJECT pObj = pManager->m_listSelectedObject.GetHead();
				pObj->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pObj->m_pProperty);
				AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
				AUX_GetMainFrame()->SetFocus();
			}else
			{
				AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel();
				AUX_GetMainFrame()->SetFocus();
			}
		}else
		{
			pManager->m_coordinateDirection.SetCursor(m_hCursor);
			SetCursor(m_hCursor);
		}
		m_nOldX = x;
		m_nOldY = y;
	}
	return true;
}

bool CSceneObjectSelectOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	m_bDrag = false;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		pManager->m_coordinateDirection.OnLButtonUp(x,y,dwFlags);
		m_nDirectFlag = 0;
	}
	return true;
}

void CSceneObjectSelectOperation::Render(A3DViewport* pA3DViewport)
{
};
