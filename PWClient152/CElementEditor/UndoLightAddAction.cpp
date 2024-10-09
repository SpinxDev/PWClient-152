// UndoLightAddAction.cpp: implementation of the CUndoSceneObjectAddAction class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "UndoLightAddAction.h"
#include "MainFrm.h"
#include "SceneWaterObject.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoSceneObjectAddAction::CUndoSceneObjectAddAction(CSceneObjectManager *pMan)
{
	m_pManager = pMan;
}

CUndoSceneObjectAddAction::~CUndoSceneObjectAddAction()
{
	Release();
}

//	Release action
void CUndoSceneObjectAddAction::Release()
{
	m_listObject.RemoveAll();
}

//	Undo action
bool CUndoSceneObjectAddAction::Undo()
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

//	Redo action
bool CUndoSceneObjectAddAction::Redo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listObject.GetPrev(pos);
		ptemp->SetDeleted(false);
		ptemp->EntryScene();
		if(ptemp->GetObjectType() == CSceneObject::SO_TYPE_WATER)
		{
			((CSceneWaterObject*)ptemp)->CreateRenderWater();
			((CSceneWaterObject*)ptemp)->BuildRenderWater();
		}
		m_pManager->AddSceneObject(ptemp);
		m_pManager->AddObjectPtrToSelected(ptemp);
	}
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	return true;
}

void CUndoSceneObjectAddAction::GetSeletedListData()
{
	ASSERT(m_pManager);
	ALISTPOSITION pos = m_pManager->m_listSelectedObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_pManager->m_listSelectedObject.GetPrev(pos);
		ptemp->EntryScene();
		m_listObject.AddTail(ptemp);
	}
}
