// BezierDeletePointOperation.h: interface for the CBezierDeletePointOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIERDELETEPOINTOPERATION_H__999148B9_5095_4294_905E_117BD9EC8065__INCLUDED_)
#define AFX_BEZIERDELETEPOINTOPERATION_H__999148B9_5095_4294_905E_117BD9EC8065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"
#include "EditerBezier.h"

class CBezierDeletePointOperation :public COperation    
{
public:
	CBezierDeletePointOperation();
	virtual ~CBezierDeletePointOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
};

class CBezierCutOperation : public COperation
{
public:
	CBezierCutOperation();
	virtual ~CBezierCutOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
private:
	void ReBuildBezier(CEditerBezier *pBezier,CEditerBezierSegment *pHead,CEditerBezierSegment *pTail);
	int GetLinkID(CEditerBezier *pBezier);
};

class CBezierAddPointOperation : public COperation
{
	public:
	CBezierAddPointOperation();
	virtual ~CBezierAddPointOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
};

class CBezierMergeOperation : public COperation
{
public:
	CBezierMergeOperation();
	virtual ~CBezierMergeOperation();
	
	virtual bool OnLButtonDown(int x, int y,DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);
};

#endif // !defined(AFX_BEZIERDELETEPOINTOPERATION_H__999148B9_5095_4294_905E_117BD9EC8065__INCLUDED_)
