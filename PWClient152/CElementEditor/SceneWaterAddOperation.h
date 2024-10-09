// SceneWaterAddOperation.h: interface for the CSceneWaterAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEWATERADDOPERATION_H__79D8BE4D_A6AB_41CE_88FC_301832FB29EC__INCLUDED_)
#define AFX_SCENEWATERADDOPERATION_H__79D8BE4D_A6AB_41CE_88FC_301832FB29EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Operation.h"
#include "SceneWaterObject.h"

class CSceneWaterObject;

class CSceneWaterAddOperation :public COperation   
{
public:
	CSceneWaterAddOperation();
	virtual ~CSceneWaterAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);

	void CreateWater();
private:
	CSceneWaterObject m_Water;
};

#endif // !defined(AFX_SCENEWATERADDOPERATION_H__79D8BE4D_A6AB_41CE_88FC_301832FB29EC__INCLUDED_)
