// BezierUndoAction.cpp: implementation of the CBezierUndoAction class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "elementeditor.h"
#include "BezierUndoAction.h"
#include "render.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBezierUndoAction::CBezierUndoAction()
{
	m_pBezier = NULL;
}

CBezierUndoAction::~CBezierUndoAction()
{

}

//	Release action
void CBezierUndoAction::Release()
{
	if(m_pBezier) 
	{
		m_pBezier->Release();
		delete m_pBezier;
	}

}

void CBezierUndoAction::StartRecord()
{
	RecordUpdate();
}

void CBezierUndoAction::RecordUpdate()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount()==1)
		{
			CEditerBezier* pOld = (CEditerBezier*)pSManager->m_listSelectedObject.GetTail();
			if(pOld->GetObjectType() != CSceneObject::SO_TYPE_BEZIER)	
				ASSERT(false);
			m_pBezier = new CEditerBezier();
			ASSERT(m_pBezier);
			
			//先记录属性
			AString name = pOld->GetObjectName();
			m_pBezier->SetObjectName(name);
			m_pBezier->SetMap(pOld->GetMap());
			DWORD bclr;
			pOld->GetProperty(bclr);
			m_pBezier->SetProperty(bclr);
			m_pBezier->m_float_anchorsize = pOld->m_float_anchorsize;
			m_pBezier->m_float_pointradius = pOld->m_float_pointradius;
			m_pBezier->m_float_segradius = pOld->m_float_segradius;
			m_pBezier->SetBezier(pOld->IsBezier());
			
			//保存点和段的信息
			POSITION pos = pOld->m_list_points.GetHeadPosition();
			while(pos)
			{
				CEditerBezierPoint* ptemp = (CEditerBezierPoint*)pOld->m_list_points.GetNext(pos);
				CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
				pNewPt->Init(g_Render.GetA3DDevice(),ptemp->GetPos(),A3DCOLORRGB(255,0,0),pOld->m_float_pointradius);
				pNewPt->SetID(ptemp->GetID());
				m_pBezier->AddPointTail(pNewPt);
			}
			pos = pOld->m_list_segments.GetHeadPosition();
			while(pos)
			{
				CEditerBezierSegment* ptemp1 = (CEditerBezierSegment*)pOld->m_list_segments.GetNext(pos);
				CEditerBezierSegment* pNewSeg = new CEditerBezierSegment();
				pNewSeg->Init(g_Render.GetA3DDevice(),ptemp1->GetHead(),ptemp1->GetTail(),ptemp1->GetAnchor1(),ptemp1->GetAnchor2());
				pNewSeg->SetHeadID( ptemp1->GetHeadID());
				pNewSeg->SetTailID( ptemp1->GetTailID());
				pNewSeg->SetLink( ptemp1->GetLink());
  				m_pBezier->AddSegmentTail(pNewSeg);
			}
			m_pBezier->UpdateSegmentProperty();
		}
	}
}

//Undo action
bool CBezierUndoAction::Undo()
{
	ASSERT(m_pBezier);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CEditerBezier *pObj = (CEditerBezier*)pSManager->FindSceneObject(m_pBezier->GetObjectName());
		if(pObj)
		{
			ASSERT(pObj->GetObjectType()==CSceneObject::SO_TYPE_BEZIER);
			//记录原来的曲线
			CEditerBezier *pOld = new CEditerBezier();
			pOld->SetBezier(pObj->IsBezier());
			pOld->m_float_pointradius = pObj->m_float_pointradius;
			POSITION pos = pObj->m_list_points.GetHeadPosition();
			pOld->SetObjectName(pObj->GetObjectName());
			while(pos)
			{
				CEditerBezierPoint* ptemp = (CEditerBezierPoint*)pObj->m_list_points.GetNext(pos);
				CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
				pNewPt->Init(g_Render.GetA3DDevice(),ptemp->GetPos(),A3DCOLORRGB(255,0,0),pOld->m_float_pointradius);
				pNewPt->SetID(ptemp->GetID());
				pOld->AddPointTail(pNewPt);
			}
			pos = pObj->m_list_segments.GetHeadPosition();
			while(pos)
			{
				CEditerBezierSegment* ptemp1 = (CEditerBezierSegment*)pObj->m_list_segments.GetNext(pos);
				CEditerBezierSegment* pNewSeg = new CEditerBezierSegment();
				pNewSeg->Init(g_Render.GetA3DDevice(),ptemp1->GetHead(),ptemp1->GetTail(),ptemp1->GetAnchor1(),ptemp1->GetAnchor2());
				pNewSeg->SetHeadID( ptemp1->GetHeadID());
				pNewSeg->SetTailID( ptemp1->GetTailID());
				pNewSeg->SetLink( ptemp1->GetLink());
  				pOld->AddSegmentTail(pNewSeg);
			}
			pOld->UpdateSegmentProperty();
			
			//替换原来的曲线
			pObj->Release();
			pos = m_pBezier->m_list_points.GetHeadPosition();
			while(pos)
			{
				CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_pBezier->m_list_points.GetNext(pos);
				CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
				pNewPt->Init(g_Render.GetA3DDevice(),ptemp->GetPos(),A3DCOLORRGB(255,0,0),m_pBezier->m_float_pointradius);
				pNewPt->SetID(ptemp->GetID());
				pObj->AddPointTail(pNewPt);
			}
			pos = m_pBezier->m_list_segments.GetHeadPosition();
			while(pos)
			{
				CEditerBezierSegment* ptemp1 = (CEditerBezierSegment*)m_pBezier->m_list_segments.GetNext(pos);
				CEditerBezierSegment* pNewSeg = new CEditerBezierSegment();
				pNewSeg->Init(g_Render.GetA3DDevice(),ptemp1->GetHead(),ptemp1->GetTail(),ptemp1->GetAnchor1(),ptemp1->GetAnchor2());
				pNewSeg->SetHeadID( ptemp1->GetHeadID());
				pNewSeg->SetTailID( ptemp1->GetTailID());
				pNewSeg->SetLink( ptemp1->GetLink());
  				pObj->AddSegmentTail(pNewSeg);
			}
			pObj->UpdateSegmentProperty();

			//释放新数据，指向原数据
			m_pBezier->Release();
			delete m_pBezier;
			m_pBezier = pOld;
		}
	}
	return true;
}

//	Redo action
bool CBezierUndoAction::Redo()
{
	return Undo();
}

//----------------------------------------------------------------
//Bezier undo class 2
//----------------------------------------------------------------

CBezierUndoAction2::CBezierUndoAction2()
{
	m_pBezierOld = NULL;
	m_pBezierNew[0] = NULL;
	m_pBezierNew[1] = NULL;
}

CBezierUndoAction2::~CBezierUndoAction2()
{

}

//	Release action
void CBezierUndoAction2::Release()
{
}

void CBezierUndoAction2::StartRecord()
{
	RecordUpdate();
}

void CBezierUndoAction2::SetNewBezier(CEditerBezier *pb1,CEditerBezier *pb2)
{
	m_pBezierNew[0] = pb1;
	m_pBezierNew[1] = pb2;
}

void CBezierUndoAction2::RecordUpdate()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount()==1)
		{
			m_pBezierOld = (CEditerBezier*)pSManager->m_listSelectedObject.GetTail();
		}
	}
}

//	Undo action
bool CBezierUndoAction2::Undo()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		ASSERT(m_pBezierOld);
		ASSERT(m_pBezierNew[0]);
		ASSERT(m_pBezierNew[1]);
		
		pSManager->DeleteSceneObject(m_pBezierNew[0]->GetObjectName());
		pSManager->DeleteSceneObject(m_pBezierNew[1]->GetObjectName());
		pSManager->AddSceneObject(m_pBezierOld);
	}
	return true;
}

//	Redo action
bool CBezierUndoAction2::Redo()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		ASSERT(m_pBezierOld);
		ASSERT(m_pBezierNew[0]);
		ASSERT(m_pBezierNew[1]);
		pSManager->DeleteSceneObject(m_pBezierOld->GetObjectName());
		pSManager->AddSceneObject(m_pBezierNew[0]);
		pSManager->AddSceneObject(m_pBezierNew[1]);
	}
	return true;
}

