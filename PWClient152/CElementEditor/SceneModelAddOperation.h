// SceneModelAddOperation.h: interface for the SceneModelAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEMODELADDOPERATION_H__95E20AF5_83C3_4873_877D_4A9BF53852AD__INCLUDED_)
#define AFX_SCENEMODELADDOPERATION_H__95E20AF5_83C3_4873_877D_4A9BF53852AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Operation.h"
#include "ModelContainer.h"
#include "SceneSkinModel.h"
#include "CritterGroup.h"
#include "StaticModelObject.h"

class A3DModel;

class SceneModelAddOperation :public COperation  
{
public:
	SceneModelAddOperation();
	virtual ~SceneModelAddOperation();
	
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);
	void SetStaticModel(AString strName);
private:
	//PESTATICMODEL m_pStaticModel;
	AString m_strModelName;
	CStaticModelObject m_Model;//temp
};

class CSceneSkinModelAddOperation :public COperation
{
public:
	CSceneSkinModelAddOperation();
	virtual ~CSceneSkinModelAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove( int x, int y, DWORD dwFlags);
	void SetSkinModel(AString strPathName);

private:
	CSceneSkinModel m_ECModel;
	AString m_strModelPath;
};

class CSceneCritterAddOperation :public COperation
{
public:
	CSceneCritterAddOperation();
	virtual ~CSceneCritterAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove( int x, int y, DWORD dwFlags);
	void         CreateCritter();

private:
	CCritterGroup m_CritterGroup;
};


#endif // !defined(AFX_SCENEMODELADDOPERATION_H__95E20AF5_83C3_4873_877D_4A9BF53852AD__INCLUDED_)
