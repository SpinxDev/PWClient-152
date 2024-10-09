// SceneObjectScaleOperation.cpp: implementation of the SceneObjectScaleOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "elementeditor.h"
#include "SceneObjectScaleOperation.h"
#include "MainFrm.h"
#include "A3D.h"
#include "render.h"
#include "SceneObjectManager.h"
#include "UndoScaleAction.h"
#include "UndoMan.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SceneObjectScaleOperation::SceneObjectScaleOperation()
{
	m_bRotate = false;
	m_pUndo = 0;
}

SceneObjectScaleOperation::~SceneObjectScaleOperation()
{
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
	}
}

bool SceneObjectScaleOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		pManager->m_coordinateDirection.SetCursor(g_hScaleCursor);
		SceneObjectMultiSelectOperation::OnMouseMove(x,y,dwFlags);
		if(!m_bMouseDown) return true;
		
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		float deltaX = (x - m_nOldX);
		float deltaY = (m_nOldY - y);
		m_nOldX = x;
		m_nOldY = y;
		
		float fUpScale,fDownScale;
		//如果Left shift按下
		SHORT s = GetKeyState(VK_LSHIFT);
		if(s >= 0) 
		{
			fUpScale = 1.01f;
			fDownScale = 0.99f;
		}else
		{
			fUpScale = 1.1f;
			fDownScale = 0.90f;
		}

		if(deltaY>0) m_fScale = m_fScale*fUpScale;
		else if(deltaY<0)m_fScale = m_fScale*fDownScale;
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			if(deltaY>0)
			ptemp->Scale(fUpScale);
			else if(deltaY<0) ptemp->Scale(fDownScale);
		}
	}
	return true;
}

void SceneObjectScaleOperation::Render(A3DViewport* pA3DViewport)
{
	SceneObjectMultiSelectOperation::Render(pA3DViewport);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			pManager->m_coordinateDirection.SetSelectedFlags(m_nDirectFlag);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			pManager->m_coordinateDirection.SetPos(vCoorPos);
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.Draw();
		}
	}
}

bool SceneObjectScaleOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_fScale = 1.0f;
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
		m_pUndo = 0;
	}
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		m_pUndo = new CUndoScaleAction(pMap->GetSceneObjectMan());
		m_pUndo->SetData();
	}
	return SceneObjectMultiSelectOperation::OnLButtonDown(x,y,dwFlags);
}

bool SceneObjectScaleOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	SceneObjectMultiSelectOperation::OnLButtonUp(x,y,dwFlags);
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_fScale!=1.0f && m_pUndo)
		{
			g_UndoMan.AddUndoAction(m_pUndo);
			m_pUndo = 0;
		}
	}
	return true;
}

