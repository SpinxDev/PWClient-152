// BezierDeletePointOperation.cpp: implementation of the CBezierDeletePointOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "BezierDeletePointOperation.h"
#include "Render.h"
#include "SceneObjectManager.h"
#include "UndoMan.h"
#include "BezierUndoAction.h"
#include "A3D.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBezierDeletePointOperation::CBezierDeletePointOperation()
{

}

CBezierDeletePointOperation::~CBezierDeletePointOperation()
{

}

bool CBezierDeletePointOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pobj = pSManager->m_listSelectedObject.GetTail();
			if(pobj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
				A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
				A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
				A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
				pViewport->InvTransform(vStart,vStart);
				pViewport->InvTransform(vPos,vPos);
				int nFlag = 0;
				if(((CEditerBezier*)pobj)->PickControl(vStart,vPos,nFlag))
				{
					if(nFlag==1)
					{
						if(((CEditerBezier*)pobj)->m_list_points.GetCount()<3) return true;
						//记录Undo
						CBezierUndoAction *pUndo = new CBezierUndoAction();
						pUndo->StartRecord();
						g_UndoMan.AddUndoAction(pUndo);
						
						CEditerBezierSegment *pSegHead = NULL,*pSegTail = NULL;
						CEditerBezierPoint *pt = NULL;
						pt = ((CEditerBezier*)pobj)->GetCurPoint(&pSegHead,&pSegTail);
						if(pt)
						{
							((CEditerBezier*)pobj)->DeletePoint(pt->GetID());
							pMap->SetModifiedFlag(true);
						}						
					}
				}
			}
		}

	}
	return true;
}


//Cut operation

CBezierCutOperation::CBezierCutOperation()
{

}
CBezierCutOperation::~CBezierCutOperation()
{

}

bool CBezierCutOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount() !=1 ) return true;
		CSceneObject *pobj = pSManager->m_listSelectedObject.GetTail();
		if(pobj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			int nFlag = 0;
			if(((CEditerBezier*)pobj)->PickControl(vStart,vPos,nFlag))
			{
				if(nFlag==1)
				{
					CEditerBezierSegment *pSegHead = NULL,*pSegTail = NULL;
					CEditerBezierPoint *pt = NULL;
					pt = ((CEditerBezier*)pobj)->GetCurPoint(&pSegHead,&pSegTail);
					if(pt)
					{
						if(pSegHead && pSegTail)
						{
							ReBuildBezier(((CEditerBezier*)pobj),pSegHead,pSegTail);
							pMap->SetModifiedFlag(true);
						}
					}						
				}
			}
		}
	}
	return true;
}

void CBezierCutOperation::ReBuildBezier(CEditerBezier *pBezier,CEditerBezierSegment *pHead,CEditerBezierSegment *pTail)
{
	//记录Undo
	CBezierUndoAction2 *pUndo = new CBezierUndoAction2();
	pUndo->StartRecord();
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();

	AString strName;
	int     objectID;

	POSITION pos = pBezier->m_list_segments.GetHeadPosition();
	CEditerBezier *pOldBezier = NULL;
	CEditerBezier *pNewBezier = new CEditerBezier(0,g_Render.GetA3DDevice());
	pNewBezier->SetObjectID(pSManager->GenerateObjectID());
	pNewBezier->SetBezier(pBezier->IsBezier());
	objectID = 0;
	while(1)
	{
		if(pNewBezier->IsBezier())
			strName.Format("Bezier_%d",objectID);
		else strName.Format("Obstruct_%d",objectID);
		if(pSManager->FindSceneObject(strName)!=NULL)
		{
			objectID++;
		}else 
		{
			objectID++;
			break;
		}
	}
	pNewBezier->SetObjectName(strName);
	DWORD color;
	pBezier->GetProperty(color);
	pNewBezier->SetProperty(color);
	while(pos)
	{
		CEditerBezierSegment *ptemp = (CEditerBezierSegment*)pBezier->m_list_segments.GetNext(pos);
		CEditerBezierPoint   *pNewPt = new CEditerBezierPoint();
		CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
	
		pNewPt->Init(g_Render.GetA3DDevice(),ptemp->GetHead(),A3DCOLORRGB(255,0,0),pBezier->m_float_pointradius);
		pNewPt->SetID(ptemp->GetHeadID());
		
		pNewSeg->Init(g_Render.GetA3DDevice(),ptemp->GetHead(),ptemp->GetTail(),ptemp->GetAnchor1(),ptemp->GetAnchor2(),
			pBezier->m_float_segradius,pBezier->m_float_anchorsize,color);
		pNewSeg->SetHeadID(ptemp->GetHeadID());
		pNewSeg->SetTailID(ptemp->GetTailID());
		pNewSeg->SetLink(GetLinkID(pNewBezier));
		
		pNewBezier->AddSegmentTail(pNewSeg);
		pNewBezier->AddPointTail(pNewPt);
		if(ptemp==pHead)
		{//前半条曲线复制完成，开始复制后半条曲线
			CEditerBezierPoint   *pLastPt = new CEditerBezierPoint();
			pLastPt->Init(g_Render.GetA3DDevice(),ptemp->GetTail(),A3DCOLORRGB(255,0,0),pBezier->m_float_pointradius);
			pLastPt->SetID(ptemp->GetTailID());
			pNewBezier->AddPointTail(pLastPt);

			pNewBezier->UpdateProperty(false);
			pSManager->AddSceneObject(pNewBezier);
			//创建新的B曲线
			
			pOldBezier = pNewBezier;
			pNewBezier = new CEditerBezier(0,g_Render.GetA3DDevice());
			pNewBezier->SetObjectID(pSManager->GenerateObjectID());
			pNewBezier->SetBezier(pBezier->IsBezier());
			int r = (int)(255*(float)rand()/(float)RAND_MAX);
			int g = (int)(255*(float)rand()/(float)RAND_MAX);
			int b = (int)(255*(float)rand()/(float)RAND_MAX);
			color = A3DCOLORRGB(r,g,b);
			while(1)
			{
				if(pNewBezier->IsBezier())
					strName.Format("Bezier_%d",objectID);
				else strName.Format("Obstruct_%d",objectID);
				if(pSManager->FindSceneObject(strName)!=NULL)
				{
					objectID++;
				}else 
				{
					objectID++;
					break;
				}
			}
			pNewBezier->SetProperty(color);
			pNewBezier->SetObjectName(strName);
		}
		if(!pos)
		{//复制过程结束
			CEditerBezierPoint   *pLastPt = new CEditerBezierPoint();
			pLastPt->Init(g_Render.GetA3DDevice(),ptemp->GetTail(),A3DCOLORRGB(255,0,0),pBezier->m_float_pointradius);
			pLastPt->SetID(ptemp->GetTailID());
			pNewBezier->AddPointTail(pLastPt);
			pNewBezier->UpdateProperty(false);
 			pSManager->AddSceneObject(pNewBezier);
		}
	}

	//删除旧的B曲线
	pSManager->DeleteSceneObject(pBezier->GetObjectName());
	pUndo->SetNewBezier(pNewBezier,pOldBezier);
	g_UndoMan.AddUndoAction(pUndo);
}

int CBezierCutOperation::GetLinkID(CEditerBezier *pBezier)
{
	int newid = -1;
	POSITION pos = pBezier->m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp = (CEditerBezierSegment*)pBezier->m_list_segments.GetNext(pos);
		int id = ptemp->GetLink();
		if(id>newid) newid = id;
	}
	return newid + 1;
}

//Bezier add point operation
CBezierAddPointOperation::CBezierAddPointOperation()
{

}
CBezierAddPointOperation::~CBezierAddPointOperation()
{
}

bool CBezierAddPointOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount() !=1 ) return true;
		CSceneObject *pobj = pSManager->m_listSelectedObject.GetTail();
		if(pobj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			int nFlag = 0;
			//pViewport->InvTransform(vPos,vPos);
			if(((CEditerBezier*)pobj)->PickControl(vStart,vPos,nFlag))
			{
				if(nFlag==2)
				{
					CEditerBezierSegment *pSeg = NULL;
					A3DVECTOR3 hitPos;
					pSeg = ((CEditerBezier*)pobj)->GetCurSegment(hitPos);
					if(pSeg)
					{
						//记录Undo
						CBezierUndoAction *pUndo = new CBezierUndoAction();
						pUndo->StartRecord();
						g_UndoMan.AddUndoAction(pUndo);

						((CEditerBezier*)pobj)->InsertPoint(hitPos,pSeg);
						pMap->SetModifiedFlag(true);
					}						
				}
			}
		}
	}
	return true;
}

CBezierMergeOperation::CBezierMergeOperation()
{
}

CBezierMergeOperation::~CBezierMergeOperation()
{
}

bool CBezierMergeOperation::OnLButtonDown(int x, int y,DWORD dwFlags)
{
	g_BezierMergeDlg.ShowWindow(false);
	AUX_GetMainFrame()->SetShowMergeDlg(false);
	return true;
}

void CBezierMergeOperation::Render(A3DViewport* pA3DViewport)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount() !=2 ) return;
		CSceneObject *pobja = pSManager->m_listSelectedObject.GetTail();
		CSceneObject *pobjb = pSManager->m_listSelectedObject.GetHead();
		if(pobja->GetObjectType() != CSceneObject::SO_TYPE_BEZIER
			|| pobjb->GetObjectType() != CSceneObject::SO_TYPE_BEZIER) return;
		
		A3DVECTOR3 vpos[4];
		vpos[0] = ((CEditerBezier*)pobja)->GetHeadPos();
		vpos[1] = ((CEditerBezier*)pobja)->GetTailPos();
		vpos[2] = ((CEditerBezier*)pobjb)->GetHeadPos();
		vpos[3] = ((CEditerBezier*)pobjb)->GetTailPos();
		
		int link = g_BezierMergeDlg.GetLinkType();
		switch(link)
		{
			case 1:
				vpos[0] = vpos[0]; vpos[1] = vpos[3]; break;
			case 2:
				vpos[0] = vpos[0]; vpos[1] = vpos[2]; break;
			case 3:
				vpos[0] = vpos[1]; vpos[1] = vpos[3]; break;
			case 4:
				vpos[0] = vpos[1]; vpos[1] = vpos[2]; break;
		}
		
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		
		WORD pIndices[2];
		pIndices[0] = 0;
		pIndices[1] = 1;
		if(pWireCollector) pWireCollector->AddRenderData_3D(vpos,2,pIndices,2,A3DCOLORRGB(255,255,0));
	}
}

