// AIGeneratorOperation.h: interface for the CAIGeneratorOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIGENERATOROPERATION_H__0F349640_CE6C_4BAF_A479_69877614A5C3__INCLUDED_)
#define AFX_AIGENERATOROPERATION_H__0F349640_CE6C_4BAF_A479_69877614A5C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"
#include "SceneAIGenerator.h"
#include "ScenePrecinctObject.h"
#include "SceneGatherObject.h"
#include "CloudBox.h"
#include "A2DSprite.h"
#include "ObstructOperation.h"
#include "SceneDynamicObject.h"

class CAIGeneratorOperation  :public COperation      
{
public:
	CAIGeneratorOperation();
	virtual ~CAIGeneratorOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateAI();
	virtual void Render(A3DViewport* pA3DViewport);
private:
	CSceneAIGenerator m_AI;
	void DrawRect(A3DViewport *pViewport);
	bool m_bDrawRect;
	A3DVECTOR3 m_vStartPoint;
	A3DVECTOR3 m_vEndPoint;
};

class CSceneGatherOperation :public COperation      
{
	public:
	CSceneGatherOperation();
	virtual ~CSceneGatherOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateGather();
	virtual void Render(A3DViewport* pA3DViewport);
private:
	CSceneGatherObject m_Gather;
	void DrawRect(A3DViewport *pViewport);
	bool m_bDrawRect;
	A3DVECTOR3 m_vStartPoint;
	A3DVECTOR3 m_vEndPoint;
};

class CFixedNpcGeneratorOperation :public COperation
{
public:
	CFixedNpcGeneratorOperation();
	virtual ~CFixedNpcGeneratorOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);
	void CreateNpc();
	void TransObstructData();
private:
	CSceneFixedNpcGenerator     m_Npc;
	A2DSprite*					m_pSprite;
	BYTE*                       m_pData;
	bool                        m_bLoad;
	int							m_iObsMapWidth;
};

class CPrecinctOperation  :public COperation      
{
public:
	CPrecinctOperation();
	virtual ~CPrecinctOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreatePrecinct();
	virtual void Render(A3DViewport* pA3DViewport);
private:
	CScenePrecinctObject m_Precinct;
};

class CRangeOperation  :public COperation      
{
public:
	CRangeOperation();
	virtual ~CRangeOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateRange();
	virtual void Render(A3DViewport* pA3DViewport);
private:
	CSceneRangeObject m_Range;
};

class CCloudBoxOperation :public COperation
{
public:
	CCloudBoxOperation();
	virtual ~CCloudBoxOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateCloud();
	virtual void Render(A3DViewport* pA3DViewport);
private:
	CCloudBox m_Cloud;
	bool m_bDrawRect;
	bool m_bDrawHeight;
	A3DVECTOR3 m_vStartPoint;
	A3DVECTOR3 m_vEndPoint;
};

class CDynamicObjectOperation  :public COperation      
{
public:
	CDynamicObjectOperation();
	virtual ~CDynamicObjectOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateDynamic();
	virtual void Render(A3DViewport* pA3DViewport);
private:
	CSceneDynamicObject m_DynamicObject;
};

#endif // !defined(AFX_AIGENERATOROPERATION_H__0F349640_CE6C_4BAF_A479_69877614A5C3__INCLUDED_)
