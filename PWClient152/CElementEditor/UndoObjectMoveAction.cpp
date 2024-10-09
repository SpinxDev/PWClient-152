// UndoObjectMoveAction.cpp: implementation of the CUndoObjectMoveAction class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "UndoObjectMoveAction.h"
#include "MainFrm.h"
#include "SceneWaterObject.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoObjectMoveAction::CUndoObjectMoveAction( CSceneObjectManager *pMan )
{
	m_pManager = pMan;
}

CUndoObjectMoveAction::~CUndoObjectMoveAction()
{
	
}

//	Release action
void CUndoObjectMoveAction::Release()
{
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_OBJECT_POS* ptemp = m_listObject.GetPrev(pos);
		delete ptemp;
	}
	m_listObject.RemoveAll();
}

//	Undo action
bool CUndoObjectMoveAction::Undo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_OBJECT_POS* ptemp = m_listObject.GetPrev(pos);
		A3DVECTOR3 vOldPos = ptemp->pObject->GetPos();
		(ptemp->pObject)->SetPos(ptemp->vOldPos);
		ptemp->vOldPos = vOldPos;
		ptemp->pObject->UpdateProperty(false);
		if(ptemp->pObject->GetObjectType()==CSceneObject::SO_TYPE_WATER)
		((CSceneWaterObject*)ptemp->pObject)->BuildRenderWater();
		m_pManager->AddObjectPtrToSelected(ptemp->pObject);
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
	}
	return true;
}

//	Redo action
bool CUndoObjectMoveAction::Redo()
{
	return Undo();
}

void CUndoObjectMoveAction::GetSeletedListData()
{
	ASSERT(m_pManager);
	ALISTPOSITION pos = m_pManager->m_listSelectedObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_pManager->m_listSelectedObject.GetPrev(pos);
		UNDO_OBJECT_POS *pNewPos = new UNDO_OBJECT_POS;
		pNewPos->pObject = ptemp;
		pNewPos->vOldPos = ptemp->GetPos();
		m_listObject.AddTail(pNewPos);
	}
}
