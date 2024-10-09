// BezierAddOperation.h: interface for the CBezierAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIERADDOPERATION_H__7CB464A4_5A1C_40B3_9BAA_314838880EA2__INCLUDED_)
#define AFX_BEZIERADDOPERATION_H__7CB464A4_5A1C_40B3_9BAA_314838880EA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"
#include "EditerBezier.h"

class CBezierUndoAction;

class CBezierAddOperation  :public COperation   
{
public:
	CBezierAddOperation();
	virtual ~CBezierAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp( int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);

	void CreateBezier();
	void InitBezierCreateState();

private:
	A3DVECTOR3 PointAlign(A3DVECTOR3 v);

	int m_nPointID;
	int m_nBezierID;

	CEditerBezier *m_pBezier;
	CEditerBezier m_tempBezier;
	
};

class CBezierDragOperation :public COperation
{
public:
	CBezierDragOperation();
	virtual ~CBezierDragOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x,int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);
	

private:
	A3DVECTOR3 GetDeltaVector(A3DVECTOR3 pos,int x,int y);
	bool m_bHasHit;
	bool m_bMouseDown;
	int m_nDirectFlag;
	int m_nOldX;
	int m_nOldY;
	CBezierUndoAction *m_pUndo;
	A3DVECTOR3 m_vDelta;
};

class CBezierTestOperation :public COperation
{
	public:
	CBezierTestOperation();
	virtual ~CBezierTestOperation();
	
	virtual void Tick(DWORD dwTimeDelta);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	
	void Play();
	void Stop();
	void UpdateCamra(DWORD dwTimeDelta);
	CEditerBezierSegment* GetNextSeg();
private:
	A3DVECTOR3 GetSpot(CEditerBezierSegment *pseg,float u);
	bool m_bStartPlay;
	CEditerBezierSegment *m_pSeg;
	CEditerBezier*       m_pBezier;
	DWORD m_dwSegTime;
	DWORD m_dwEscapeTime;
	POSITION m_SegPos;
};

#endif // !defined(AFX_BEZIERADDOPERATION_H__7CB464A4_5A1C_40B3_9BAA_314838880EA2__INCLUDED_)
