// SceneObjectScaleOperation.h: interface for the SceneObjectScaleOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEOBJECTSCALEOPERATION_H__FBD791AF_E83E_463B_9382_349A5896DE6A__INCLUDED_)
#define AFX_SCENEOBJECTSCALEOPERATION_H__FBD791AF_E83E_463B_9382_349A5896DE6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObjectMultiSelectOperation.h"
class CUndoScaleAction;

class SceneObjectScaleOperation  :public SceneObjectMultiSelectOperation 
{
public:
	SceneObjectScaleOperation();
	virtual ~SceneObjectScaleOperation();
	
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);

private:
	float m_fScale;
	CUndoScaleAction *m_pUndo;
};

#endif // !defined(AFX_SCENEOBJECTSCALEOPERATION_H__FBD791AF_E83E_463B_9382_349A5896DE6A__INCLUDED_)
