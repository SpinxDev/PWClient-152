// UndoLightDeleteAction.cpp: implementation of the CUndoSceneObjectDeleteAction class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "UndoLightDeleteAction.h"
#include "MainFrm.h"
#include "SceneWaterObject.h"
#include "SceneBoxArea.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoSceneObjectDeleteAction::CUndoSceneObjectDeleteAction(CSceneObjectManager *pMan)
{
	m_pManager = pMan;
}

CUndoSceneObjectDeleteAction::~CUndoSceneObjectDeleteAction()
{

}

//	Release action
void CUndoSceneObjectDeleteAction::Release()
{
	/*
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listObject.GetPrev(pos);
		if(!m_pManager->IsObjectInScene(ptemp->GetObjectName()))
		{
			m_pManager->DeleteSceneObjectForever(ptemp);
			ptemp = NULL;
		}

	}*/
	m_listObject.RemoveAll();
}

//	Undo action
bool CUndoSceneObjectDeleteAction::Undo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listObject.GetPrev(pos);
		//水是特殊对象要做特殊处理
		if(ptemp->GetObjectType() == CSceneObject::SO_TYPE_WATER)
		{
			((CSceneWaterObject*)ptemp)->CreateRenderWater();
			((CSceneWaterObject*)ptemp)->BuildRenderWater();
		}
		ptemp->SetDeleted(false);
		ptemp->EntryScene();
		m_pManager->AddSceneObject(ptemp);
		m_pManager->AddObjectPtrToSelected(ptemp);
	}
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	return true;
}

//	Redo action
bool CUndoSceneObjectDeleteAction::Redo()
{
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listObject.GetPrev(pos);
		ptemp->LeaveScene();
		ptemp->SetDeleted(true);
		m_pManager->DeleteSceneObject(ptemp->GetObjectName());
	}
	m_pManager->ClearSelectedList();
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	return true;
}

bool CUndoSceneObjectDeleteAction::GetSeletedListData()
{
	ASSERT(m_pManager);
	
	ALISTPOSITION pos = m_pManager->m_listSelectedObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_pManager->m_listSelectedObject.GetPrev(pos);
		
		//跳过默认区域
		if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_AREA)
		if(((CSceneBoxArea*)ptemp)->IsDefaultArea()) continue;
		
		
		//清除对象关联
		if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_BEZIER)
		{
			int id = ptemp->GetObjectID();
			m_pManager->CutLink_Bezier(id);
		}
		
		if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_DUMMY)
		{
			int id = ptemp->GetObjectID();
			m_pManager->CutLink_Precinct_Dummy(id);
		}

		ptemp->SetDeleted(true);
		ptemp->LeaveScene();
		m_pManager->DeleteSceneObject(ptemp->GetObjectName());
		m_listObject.AddTail(ptemp);
	}
	m_pManager->ClearSelectedList();
	
	if(m_listObject.GetCount()>0) return true;
	else return false;
}

