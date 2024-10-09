// UndoScaleAction.h: interface for the CUndoScaleAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOSCALEACTION_H__AC9302F1_12BC_4B61_90FE_271A39DEE59E__INCLUDED_)
#define AFX_UNDOSCALEACTION_H__AC9302F1_12BC_4B61_90FE_271A39DEE59E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "A3DTypes.h"

struct UNDO_SCALE_BOJECT 
{
	PSCENEOBJECT pObj;
	A3DVECTOR3 vs;
};

class CUndoScaleAction :public CUndoAction   
{
public:

	CUndoScaleAction(CSceneObjectManager *pMan);
	virtual ~CUndoScaleAction();

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

	void SetData();
private:
	//����Զ����浱ǰ����ѡ�������
	void GetSeletedListData();

	CSceneObjectManager *m_pManager;
	//���б�ѡ��Ķ�����б�
	APtrList<UNDO_SCALE_BOJECT*> m_listObject;
};

#endif // !defined(AFX_UNDOSCALEACTION_H__AC9302F1_12BC_4B61_90FE_271A39DEE59E__INCLUDED_)
