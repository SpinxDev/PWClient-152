// UndoRotateAction.h: interface for the CUndoRotateAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOROTATEACTION_H__33DDDF75_74AA_4FAC_AE78_57D6B291CC04__INCLUDED_)
#define AFX_UNDOROTATEACTION_H__33DDDF75_74AA_4FAC_AE78_57D6B291CC04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "A3DTypes.h"

struct UNDO_ROTATE_BOJECT
{
	PSCENEOBJECT pObj;
	A3DVECTOR3 vu;
	A3DVECTOR3 vr;
	A3DVECTOR3 vl;
};

struct UNDO_ROTATE2_BOJECT
{
	PSCENEOBJECT pObj;
	A3DVECTOR3 vu;
	A3DVECTOR3 vr;
	A3DVECTOR3 vl;
	A3DVECTOR3 vpos;
};

class CUndoRotateAction :public CUndoAction   
{
public:
	CUndoRotateAction(CSceneObjectManager *pMan);
	virtual ~CUndoRotateAction();

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

	void SetData();
private:
	//这儿自动保存当前场景选择表数据
	void GetSeletedListData();

	CSceneObjectManager *m_pManager;
	//所有被选择的对象的列表
	APtrList<UNDO_ROTATE_BOJECT*> m_listObject;
};

class CUndoRotateAction2 :public CUndoAction   
{
public:
	CUndoRotateAction2(CSceneObjectManager *pMan);
	virtual ~CUndoRotateAction2();

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

	void SetData();
private:
	//这儿自动保存当前场景选择表数据
	void GetSeletedListData();

	CSceneObjectManager *m_pManager;
	//所有被选择的对象的列表
	APtrList<UNDO_ROTATE2_BOJECT*> m_listObject;
};

#endif // !defined(AFX_UNDOROTATEACTION_H__33DDDF75_74AA_4FAC_AE78_57D6B291CC04__INCLUDED_)
