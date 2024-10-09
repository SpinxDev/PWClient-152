// CSceneObjectRotateOperation.h: interface for the CSceneObjectRotateOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSceneObjectRotateOperation_H__13110067_B365_49F0_ABA4_1FF22F94EFF3__INCLUDED_)
#define AFX_CSceneObjectRotateOperation_H__13110067_B365_49F0_ABA4_1FF22F94EFF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObjectMultiSelectOperation.h"

class CUndoRotateAction;
class CUndoRotateAction2;

class CSceneObjectRotateOperation :public SceneObjectMultiSelectOperation 
{

public:
	CSceneObjectRotateOperation();
	virtual ~CSceneObjectRotateOperation();
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);

private:
	A3DVECTOR3 m_vAxis;
	float m_fRotateRad;
	CUndoRotateAction *m_pUndo;
};

class CSceneObjectRotateOperationEx :public SceneObjectMultiSelectOperation 
{
public:
	enum
	{
		ROTATE_X = 0,
		ROTATE_Y,
		ROTATE_Z,
	};
	CSceneObjectRotateOperationEx();
	virtual ~CSceneObjectRotateOperationEx();
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	void SetRotateFlag(int flag){ m_nRotateFlag = flag; };
private:
	A3DVECTOR3 m_vAxis;
	int   m_nRotateFlag;
	float m_fRotateRad;
	CUndoRotateAction *m_pUndo;
};


class CSceneObjectRotateOperation2 :public SceneObjectMultiSelectOperation 
{
public:
	enum
	{
		ROTATE_X = 0,
		ROTATE_Y,
		ROTATE_Z,
	};
	CSceneObjectRotateOperation2();
	virtual ~CSceneObjectRotateOperation2();
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	void SetRotateFlag(int flag){ m_nRotateFlag = flag; };
private:
	void Delta(CSceneObject* pTemp, float fDelta, A3DVECTOR3 vCenter, int nAxis);

	CUndoRotateAction2 *m_pUndo;
	int   m_nRotateFlag;
	float m_fRotateRad;
};

#endif // !defined(AFX_CSceneObjectRotateOperation_H__13110067_B365_49F0_ABA4_1FF22F94EFF3__INCLUDED_)
