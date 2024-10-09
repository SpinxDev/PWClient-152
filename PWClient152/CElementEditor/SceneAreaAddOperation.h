// SceneAreaAddOperation.h: interface for the CSceneAreaAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEAREAADDOPERATION_H__B8A602FA_106C_471F_B8DF_C72E2644F5C2__INCLUDED_)
#define AFX_SCENEAREAADDOPERATION_H__B8A602FA_106C_471F_B8DF_C72E2644F5C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Operation.h"
#include "SceneBoxArea.h"
#include "SceneInstanceBox.h"

class CSceneAreaAddOperation :public COperation    
{
public:
	CSceneAreaAddOperation();
	virtual ~CSceneAreaAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);

	void CreateArea();
private:
	CSceneBoxArea m_Area;
};

class CSceneInstanceBoxAddOperation :public COperation    
{
public:
	CSceneInstanceBoxAddOperation();
	virtual ~CSceneInstanceBoxAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);

	void CreateInstanceBox();
private:
	SceneInstanceBox m_InstanceBox;
};

#endif // !defined(AFX_SCENEAREAADDOPERATION_H__B8A602FA_106C_471F_B8DF_C72E2644F5C2__INCLUDED_)
