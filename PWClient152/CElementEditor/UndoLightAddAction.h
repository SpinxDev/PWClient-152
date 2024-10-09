// UndoLightAddAction.h: interface for the CUndoLightAddAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOLIGHTADDACTION_H__ABC2CECF_2119_4CD1_A9AB_2DC33710EA5E__INCLUDED_)
#define AFX_UNDOLIGHTADDACTION_H__ABC2CECF_2119_4CD1_A9AB_2DC33710EA5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "A3DTypes.h"

class CUndoSceneObjectAddAction :public CUndoAction 
{
public:
	CUndoSceneObjectAddAction(CSceneObjectManager *pMan);
	virtual ~CUndoSceneObjectAddAction();

	//����Զ����浱ǰ����ѡ�������
	void GetSeletedListData();
	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

private:
	CSceneObjectManager *m_pManager;
	//���б�ѡ��Ķ�����б�
	APtrList<PSCENEOBJECT> m_listObject;
};

#endif // !defined(AFX_UNDOLIGHTADDACTION_H__ABC2CECF_2119_4CD1_A9AB_2DC33710EA5E__INCLUDED_)
