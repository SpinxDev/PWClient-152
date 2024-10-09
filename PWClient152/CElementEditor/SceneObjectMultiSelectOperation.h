// SceneObjectMultiSelectOperation.h: interface for the SceneObjectMultiSelectOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEOBJECTMULTISELECTOPERATION_H__8A6E235D_46D8_46E1_A2B9_004DB0E69EFD__INCLUDED_)
#define AFX_SCENEOBJECTMULTISELECTOPERATION_H__8A6E235D_46D8_46E1_A2B9_004DB0E69EFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "A3DTypes.h"
#include "Operation.h"
#include "SceneObjectManager.h"

class CSceneObject;

class SceneObjectMultiSelectOperation :public COperation    
{
public:
	SceneObjectMultiSelectOperation();
	virtual ~SceneObjectMultiSelectOperation();
	
	virtual void Render(A3DViewport* pA3DViewport);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);

	void SetOperationCursor(HCURSOR hc){ m_hCursor = hc; };
protected:
	void ScreenTraceAxis(A3DVECTOR3 &pos, CPoint pt, A3DVECTOR3 *pXY, A3DVECTOR3 *pXZ, A3DVECTOR3 *pYZ);
	void SelectObjectToList();
	ARectI m_rcArea;
	int m_nDirectFlag;
	bool m_bMouseDown;
	bool m_bDrawRect;
	int m_nOldX;
	int m_nOldY;
	CSceneObject * m_pSelectedObject;
	HCURSOR m_hCursor;
	bool m_bRotate;
	A3DVECTOR3 m_vXY;
	A3DVECTOR3 m_vXZ;
	A3DVECTOR3 m_vYZ;
};

#endif // !defined(AFX_SCENEOBJECTMULTISELECTOPERATION_H__8A6E235D_46D8_46E1_A2B9_004DB0E69EFD__INCLUDED_)
