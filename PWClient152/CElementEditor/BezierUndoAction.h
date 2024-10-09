// BezierUndoAction.h: interface for the CBezierUndoAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEZIERUNDOACTION_H__57100946_6DAA_4E4A_8371_E2534D3B8CA6__INCLUDED_)
#define AFX_BEZIERUNDOACTION_H__57100946_6DAA_4E4A_8371_E2534D3B8CA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "EditerBezier.h"

class CBezierUndoAction :public CUndoAction  
{
public:
	CBezierUndoAction();
	virtual ~CBezierUndoAction();
	
	//��ʼ�������ߣ�type�����߲���������
	void StartRecord();
	void RecordUpdate();

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

private:
	CEditerBezier *m_pBezier;
};

//��һ��Ϊ2��·�������⴦��
class CBezierUndoAction2 :public CUndoAction
{
public:
	CBezierUndoAction2();
	virtual ~CBezierUndoAction2();
	
	//��ʼ�������ߣ�type�����߲���������
	void StartRecord();
	void SetNewBezier(CEditerBezier *pb1,CEditerBezier *pb2);
	

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

private:
	void RecordUpdate();

	CEditerBezier *m_pBezierOld;
	CEditerBezier *m_pBezierNew[2];
};

#endif // !defined(AFX_BEZIERUNDOACTION_H__57100946_6DAA_4E4A_8371_E2534D3B8CA6__INCLUDED_)
