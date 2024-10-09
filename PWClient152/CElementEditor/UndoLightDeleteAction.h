// UndoLightDeleteAction.h: interface for the CUndoSceneObjectDeleteAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOLIGHTDELETEACTION_H__4219F5D6_207E_4D36_A9D0_8F7DBA1BCBAE__INCLUDED_)
#define AFX_UNDOLIGHTDELETEACTION_H__4219F5D6_207E_4D36_A9D0_8F7DBA1BCBAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "A3DTypes.h"

class CUndoSceneObjectDeleteAction  :public CUndoAction 
{
public:
	CUndoSceneObjectDeleteAction(CSceneObjectManager *pMan);
	virtual ~CUndoSceneObjectDeleteAction();

	//这儿自动保存当前场景选择表数据
	bool GetSeletedListData();
	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

private:
	CSceneObjectManager *m_pManager;
	//所有被选择的对象的列表
	APtrList<PSCENEOBJECT> m_listObject;

};

#endif // !defined(AFX_UNDOLIGHTDELETEACTION_H__4219F5D6_207E_4D36_A9D0_8F7DBA1BCBAE__INCLUDED_)
