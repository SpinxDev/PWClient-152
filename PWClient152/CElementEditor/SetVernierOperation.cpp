// SetVernierOperation.cpp: implementation of the CSetVernierOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "SetVernierOperation.h"
#include "Render.h"
#include "A3D.h"
#include "SceneVernier.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"



//#define new A_DEBUG_NEW


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetVernierOperation::CSetVernierOperation()
{

}

CSetVernierOperation::~CSetVernierOperation()
{

}

bool CSetVernierOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
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
				CSceneVernier *pVernier = pSManager->GetVernier();
				pVernier->SetPos(TraceRt.vPoint);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdateVernierDlg();
		}
	}
	return true;
}

bool CSetVernierOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}
