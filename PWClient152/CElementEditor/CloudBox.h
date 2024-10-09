// CloudBox.h: interface for the CCloudBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOUDBOX_H__F360F4F7_D9CF_44EE_A775_4B53ED471AAE__INCLUDED_)
#define AFX_CLOUDBOX_H__F360F4F7_D9CF_44EE_A775_4B53ED471AAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

struct CLOUD_BOX_DATA
{
	float fEdgeLen;
	float fEdgeHei;
	float fEdgeWth;
	int   nGroup;
	int   nType;//0:base,1:stratus,2:solid,3:wispy,4: no change
	DWORD dwRandSeed;
};

struct CLOUD_RAND_SEED_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant	m_var;
	CString		m_strText;
};

void init_cloud_data(CLOUD_BOX_DATA &data);

class CCloudBox :public CSceneObject 
{
public:
	CCloudBox();
	virtual ~CCloudBox();

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void EntryScene();
	virtual void LeaveScene();
	virtual void RotateX(float fRad);
	virtual void RotateY(float fRad);
	virtual void RotateZ(float fRad);
	virtual void Rotate(A3DVECTOR3 axis,float fRad);
	//  Copy object
	virtual CSceneObject*		CopyObject();
	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	CLOUD_BOX_DATA &GetProperty(){ return m_Data; };
	void SetProperty( CLOUD_BOX_DATA data){ m_Data = data; };
	void GetAABB(A3DVECTOR3 &vMin, A3DVECTOR3 &vMax);
protected:
	void BuildProperty();

private:
	CLOUD_BOX_DATA m_Data;
	ASetTemplate<int> cloud_type;
    CLOUD_RAND_SEED_FUNCS *pRandSeedFunc;
	CString strSeed;

};

#endif // !defined(AFX_CLOUDBOX_H__F360F4F7_D9CF_44EE_A775_4B53ED471AAE__INCLUDED_)
