// UndoRotateAction.cpp: implementation of the CUndoRotateAction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "UndoRotateAction.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoRotateAction::CUndoRotateAction(CSceneObjectManager *pMan)
{
	m_pManager = pMan;
}

CUndoRotateAction::~CUndoRotateAction()
{

}

//	Release action
void CUndoRotateAction::Release()
{
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_ROTATE_BOJECT* ptemp = m_listObject.GetPrev(pos);
		if(ptemp) delete ptemp;
	}
	m_listObject.RemoveAll();
}

//	Undo action
bool CUndoRotateAction::Undo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_ROTATE_BOJECT* ptemp = m_listObject.GetPrev(pos);
		
		A3DMATRIX4 mat = ptemp->pObj->GetRotateMatrix();
	    A3DVECTOR3 vl = mat.GetRow(0);
	    A3DVECTOR3 vu = mat.GetRow(1);
		A3DVECTOR3 vr = mat.GetRow(2);
		
		ptemp->pObj->SetDirAndUp(ptemp->vr,ptemp->vu);
		m_pManager->AddObjectPtrToSelected(ptemp->pObj);

	    ptemp->vl = vl;
		ptemp->vu = vu;
		ptemp->vr = vr;
		
	}
	return true;
}

//	Redo action
bool CUndoRotateAction::Redo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_ROTATE_BOJECT* ptemp = m_listObject.GetPrev(pos);
		
		A3DMATRIX4 mat = ptemp->pObj->GetRotateMatrix();
	    A3DVECTOR3 vl = mat.GetRow(0);
	    A3DVECTOR3 vu = mat.GetRow(1);
		A3DVECTOR3 vr = mat.GetRow(2);
		
		ptemp->pObj->SetDirAndUp(ptemp->vr,ptemp->vu);
		m_pManager->AddObjectPtrToSelected(ptemp->pObj);

	    ptemp->vl = vl;
		ptemp->vu = vu;
		ptemp->vr = vr;
	}
	return true;
}

void CUndoRotateAction::SetData()
{
	GetSeletedListData();
}

void CUndoRotateAction::GetSeletedListData()
{
	ASSERT(m_pManager);
	ALISTPOSITION pos = m_pManager->m_listSelectedObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_pManager->m_listSelectedObject.GetPrev(pos);
		A3DMATRIX4 mat = ptemp->GetRotateMatrix();
		UNDO_ROTATE_BOJECT *pNew = new UNDO_ROTATE_BOJECT;
		ASSERT(pNew);
		pNew->pObj = ptemp;
		pNew->vl = mat.GetRow(0);
		pNew->vu = mat.GetRow(1);
		pNew->vr = mat.GetRow(2);
		m_listObject.AddTail(pNew);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

CUndoRotateAction2::CUndoRotateAction2(CSceneObjectManager *pMan)
{
	m_pManager = pMan;
}

CUndoRotateAction2::~CUndoRotateAction2()
{

}

//	Release action
void CUndoRotateAction2::Release()
{
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_ROTATE2_BOJECT* ptemp = m_listObject.GetPrev(pos);
		if(ptemp) delete ptemp;
	}
	m_listObject.RemoveAll();
}

//	Undo action
bool CUndoRotateAction2::Undo()
{
	m_pManager->ClearSelectedList();
	ALISTPOSITION pos = m_listObject.GetTailPosition();
	while(pos)
	{
		UNDO_ROTATE2_BOJECT* ptemp = m_listObject.GetPrev(pos);
		
		A3DMATRIX4 mat = ptemp->pObj->GetRotateMatrix();
	    A3DVECTOR3 vl = mat.GetRow(0);
	    A3DVECTOR3 vu = mat.GetRow(1);
		A3DVECTOR3 vr = mat.GetRow(2);
		A3DVECTOR3 vpos = ptemp->pObj->GetPos();
		
		ptemp->pObj->SetDirAndUp(ptemp->vr,ptemp->vu);
		ptemp->pObj->SetPos(ptemp->vpos);
		m_pManager->AddObjectPtrToSelected(ptemp->pObj);

	    ptemp->vl = vl;
		ptemp->vu = vu;
		ptemp->vr = vr;
		ptemp->vpos = vpos;
		
	}
	return true;
}

//	Redo action
bool CUndoRotateAction2::Redo()
{
	return Undo();
}

void CUndoRotateAction2::SetData()
{
	GetSeletedListData();
}

void CUndoRotateAction2::GetSeletedListData()
{
	ASSERT(m_pManager);
	ALISTPOSITION pos = m_pManager->m_listSelectedObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_pManager->m_listSelectedObject.GetPrev(pos);
		A3DMATRIX4 mat = ptemp->GetRotateMatrix();
		UNDO_ROTATE2_BOJECT *pNew = new UNDO_ROTATE2_BOJECT;
		ASSERT(pNew);
		pNew->pObj = ptemp;
		pNew->vl = mat.GetRow(0);
		pNew->vu = mat.GetRow(1);
		pNew->vr = mat.GetRow(2);
		pNew->vpos = ptemp->GetPos();
		m_listObject.AddTail(pNew);
	}
}
