// SceneDummyAddOperation.h: interface for the CSceneDummyAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEDUMMYADDOPERATION_H__8EB7D7D9_5630_463C_B3EA_99649220603B__INCLUDED_)
#define AFX_SCENEDUMMYADDOPERATION_H__8EB7D7D9_5630_463C_B3EA_99649220603B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneDummyObject.h"
#include "Operation.h"

class CSceneDummyAddOperation :public COperation     
{
public:
	CSceneDummyAddOperation();
	virtual ~CSceneDummyAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);
	
	void CreateDummy();
private:
	CSceneDummyObject m_tempDummy;
};

#endif // !defined(AFX_SCENEDUMMYADDOPERATION_H__8EB7D7D9_5630_463C_B3EA_99649220603B__INCLUDED_)
