#ifndef __SCENE_DYNAMIC_OBJECT__
#define __SCENE_DYNAMIC_OBJECT__


#include "SceneObject.h"
class A3DGFXEx;
class CECModel;

struct DYNAMIC_SEL_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct CONTROLLER_DYNAMIC_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

class CSceneDynamicObject : public CSceneObject
{
public:
	CSceneDynamicObject();
	virtual ~CSceneDynamicObject();
	
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	void         SetDynamicObjectID( unsigned int id){ m_uDynamicObjectID = id; }
	unsigned int GetDynamicObjectID(){ return m_uDynamicObjectID; }


	void         SetControllerID( unsigned int id){ m_uControllerID = id; }
	unsigned int GetControllerID(){ return m_uControllerID;}
	
	//  Copy object
	virtual CSceneObject*		CopyObject();
	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);


	virtual void				Translate(const A3DVECTOR3& vDelta);
	virtual void				RotateX(float fRad);
	virtual void				RotateY(float fRad);
	virtual void				RotateZ(float fRad);
	virtual void				Rotate(const A3DVECTOR3& axis,float fRad);
	virtual void				Scale(float fScale);
	virtual void				SetPos(const A3DVECTOR3& vPos);
	virtual void                ResetRotateMat();
	virtual void SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up);
	virtual void SetPropertyPtr(DYNAMIC_SEL_FUNCS *pPtr){ m_pIDSelFuncs = pPtr; }
	virtual void SetControllerPtr(CONTROLLER_DYNAMIC_ID_FUNCS *pPtr){ m_pControllerSelFuncs = pPtr; }
	void SetAllPrtToNull()
	{
		m_pGfxEx = NULL;
		m_pECModel = NULL;
		m_pIDSelFuncs = NULL;
		m_pControllerSelFuncs = NULL;
	}
	void ReloadModels();

private:
	void BuildProperty();
	void LoadHulls(const char *szECMPath);
	void TransformHulls(const A3DMATRIX4 &mat);
	void ReleaseHulls();
	unsigned int m_uDynamicObjectID;
    unsigned int m_uControllerID;
	DYNAMIC_SEL_FUNCS* m_pIDSelFuncs;

	A3DGFXEx*      m_pGfxEx;
	CECModel*      m_pECModel;
	typedef abase::vector<CConvexHullData*> ConvexHullDataArray;
	ConvexHullDataArray	m_ECMHulls;
	CONTROLLER_DYNAMIC_ID_FUNCS* m_pControllerSelFuncs;
};

#endif