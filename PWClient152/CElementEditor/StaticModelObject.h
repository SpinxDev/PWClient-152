// StaticModelObject.h: interface for the CStaticModelObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATICMODELOBJECT_H__59519995_09C2_479E_8E2E_D44AF89064B0__INCLUDED_)
#define AFX_STATICMODELOBJECT_H__59519995_09C2_479E_8E2E_D44AF89064B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SceneObject.h"
#include "A3DModel.h"

#include "ModelContainer.h"
#include "ElementResMan.h"


class CStaticModelObject : public CSceneObject
{
public:
	CStaticModelObject();
	virtual ~CStaticModelObject();


	
	virtual A3DVECTOR3	GetMin();
	virtual A3DVECTOR3	GetMax();

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void RenderForLightMapShadow(A3DViewport* pA3DViewport);   
	virtual void RenderForLight(A3DViewport* pA3DViewport, float fOffsetX, float fOffsetZ);
	virtual void Translate(const A3DVECTOR3& vDelta);
	virtual void RotateX(float fRad);
	virtual void RotateY(float fRad);
	virtual void RotateZ(float fRad);
	virtual void Rotate(const A3DVECTOR3& axis,float fRad);
	virtual void Scale(float fScale);
	virtual void SetPos(const A3DVECTOR3& vPos);
	virtual void SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up);
	virtual void SetScale(const A3DVECTOR3& vs);		
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void ResetRotateMat();
	
	//  Copy object
	virtual CSceneObject*		CopyObject();
	

	inline void SetA3dModel(PESTATICMODEL pModel)	{ 	m_pModel = pModel;	}
	PESTATICMODEL GetA3DModel()	{ return m_pModel; }

	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	bool IsShawdow() { return m_bShawdow; };
	void SetShawdow(bool bShawdow) { m_bShawdow = bShawdow; };

	//	Get reflect flag
	bool GetReflectFlag() { return m_bReflect; }
	//	Get refract flag
	bool GetRefractFlag() { return m_bRefraction; }
	
	bool RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd,A3DVECTOR3& inter);
	void BuildProperty();

	//判断区域是否跟该区域相交
	bool IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);

	//查询自己模型引用是否已经加入到资源表中，如果已经加入就不再加入了，否则就写到资源表中
	void QueryRef(ElementResMan *pResMan);

	//把静态模型设置成只有碰撞没有显示的特殊模型
	void SetCollideOnly(bool bCollide);

	bool GetCollideFlags();
	bool IsTree(){ return m_bIsTree; }
	void SetTreeFlag( bool bIsTree){ m_bIsTree = bIsTree; }
	virtual int GetTriangleNum();

	void UpdateModel();
	
	AString GetModelPath(){ return m_strPathName; }
	void SetModelPath(const AString& path){ m_strPathName = path; }
	void SetHullPath(const AString& path){ m_strHullPathName = path; }

private:
	// Property data
	PESTATICMODEL m_pModel;
	bool m_bShawdow;
	bool m_bReflect;
	bool m_bRefraction;
	bool m_bCollideOnly;
	A3DVECTOR3 m_vMin;
	A3DVECTOR3 m_vMax;
	bool m_bIsTree;//是否是树，因为可能在做某些渲染操作时可能不需要显示树
	AString m_strPathName;
	AString m_strHullPathName;

public:
	inline bool GetReflect()		{ return m_bReflect; }
	inline bool GetRefract()		{ return m_bRefraction; }
	inline bool GetShadow()			{ return m_bShawdow; }

	void SetReflect(bool bReflect){ m_bReflect = bReflect; };
	void SetReflact(bool bRefract){ m_bRefraction = bRefract; };

private:	
	void ReBuiludAABBBox();
	void ReloadModel();
	// Only for render selected,don't save to file;
	DWORD m_dwLastTime;
	bool  m_bCircle;
};

typedef CStaticModelObject* PSTATICMODELOBJECT;

#endif // !defined(AFX_STATICMODELOBJECT_H__59519995_09C2_479E_8E2E_D44AF89064B0__INCLUDED_)
