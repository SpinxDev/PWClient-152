// SceneObjectSelectOperation.h: interface for the CSceneObjectSelectOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEOBJECTSELECTOPERATION_H__7E779889_4355_4286_BD01_EA7455076BA3__INCLUDED_)
#define AFX_SCENEOBJECTSELECTOPERATION_H__7E779889_4355_4286_BD01_EA7455076BA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"

class CSceneObjectSelectOperation :public COperation   
{

public:

	CSceneObjectSelectOperation();
	virtual ~CSceneObjectSelectOperation();

	virtual bool OnEvent(UINT message, WPARAM wParam, LPARAM lParam){ return true;};
	virtual void Tick(DWORD dwTimeDelta){};
	virtual void Render(A3DViewport* pA3DViewport);

	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);

protected:
	int m_nDirectFlag;
	bool m_bDrag;
	int m_nOldX;
	int m_nOldY;
	HCURSOR m_hCursor;
};

#endif // !defined(AFX_SCENEOBJECTSELECTOPERATION_H__7E779889_4355_4286_BD01_EA7455076BA3__INCLUDED_)
