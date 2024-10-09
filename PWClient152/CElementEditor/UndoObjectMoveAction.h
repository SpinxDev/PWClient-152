// UndoObjectMoveAction.h: interface for the CUndoObjectMoveAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOOBJECTMOVEACTION_H__31AB2DE6_BE8D_4727_956B_0279196BFBDF__INCLUDED_)
#define AFX_UNDOOBJECTMOVEACTION_H__31AB2DE6_BE8D_4727_956B_0279196BFBDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "A3DTypes.h"

typedef struct UNDO_OBJECT_POS
{
	PSCENEOBJECT pObject;
	A3DVECTOR3   vOldPos;
}* PUNDOOBJECTPOS;

class CUndoObjectMoveAction :public CUndoAction  
{
public:
	CUndoObjectMoveAction(CSceneObjectManager *pMan);
	virtual ~CUndoObjectMoveAction();

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

	//����Զ����浱ǰ����ѡ�������
	void GetSeletedListData();
private:
	

	CSceneObjectManager *m_pManager;
	//���б�ѡ��Ķ�����б�
	APtrList<PUNDOOBJECTPOS> m_listObject;
};

#endif // !defined(AFX_UNDOOBJECTMOVEACTION_H__31AB2DE6_BE8D_4727_956B_0279196BFBDF__INCLUDED_)
