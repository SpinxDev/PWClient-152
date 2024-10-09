// UndoScaleAction.cpp: implementation of the CUndoScaleAction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "UndoScaleAction.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoScaleAction::CUndoScaleAction(CSceneObjectManager *pMan)
{
	m_pManager = pMan;
}

CUndoScaleAction::~CUndoScaleAction()
{

}

//	Release action
void CUndoScaleAction::Release()
{
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_SCALE_BOJECT* ptemp = m_listObject.GetPrev(pos);
		delete ptemp;
	}
	
	m_listObject.RemoveAll();
}

//	Undo action
bool CUndoScaleAction::Undo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_SCALE_BOJECT* ptemp = m_listObject.GetPrev(pos);
		A3DMATRIX4 mat = ptemp->pObj->GetScaleMatrix();
		A3DVECTOR3 vs(mat._11,mat._22,mat._33);
		ptemp->pObj->SetScale(ptemp->vs);
		m_pManager->AddObjectPtrToSelected(ptemp->pObj);
		ptemp->vs = vs;
	}
	return true;
}

//	Redo action
bool CUndoScaleAction::Redo()
{
	return Undo();
}

void CUndoScaleAction::SetData()
{
	GetSeletedListData();
}

void CUndoScaleAction::GetSeletedListData()
{
	ASSERT(m_pManager);
	ALISTPOSITION pos = m_pManager->m_listSelectedObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_pManager->m_listSelectedObject.GetPrev(pos);
		A3DMATRIX4 mat = ptemp->GetScaleMatrix();
		UNDO_SCALE_BOJECT *pNew = new UNDO_SCALE_BOJECT;
		pNew->pObj = ptemp;
		pNew->vs = A3DVECTOR3(mat._11, mat._22, mat._33);
		m_listObject.AddTail(pNew);
	}
}


