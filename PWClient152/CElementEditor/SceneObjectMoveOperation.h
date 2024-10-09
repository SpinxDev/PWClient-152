// SceneObjectMoveOperation.h: interface for the CSceneObjectMoveOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEOBJECTMOVEOPERATION_H__FE33559B_C40D_4A5E_9614_7041E0899AC4__INCLUDED_)
#define AFX_SCENEOBJECTMOVEOPERATION_H__FE33559B_C40D_4A5E_9614_7041E0899AC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObjectMultiSelectOperation.h"

class CUndoObjectMoveAction;

class CSceneObjectMoveOperation :public SceneObjectMultiSelectOperation   
{
public:
	CSceneObjectMoveOperation();
	virtual ~CSceneObjectMoveOperation();
	
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);

protected:
	A3DVECTOR3 GetDeltaVector(A3DVECTOR3 pos,int x,int y);
	A3DVECTOR3 m_vDelta;
	CUndoObjectMoveAction *m_pUndo;
};

#endif // !defined(AFX_SCENEOBJECTMOVEOPERATION_H__FE33559B_C40D_4A5E_9614_7041E0899AC4__INCLUDED_)
