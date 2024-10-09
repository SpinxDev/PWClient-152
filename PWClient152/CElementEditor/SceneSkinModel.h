// SceneSkinModel.h: interface for the CSceneSkinModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENESKINMODEL_H__AF9E2B62_35AF_4DE8_82AE_C7E230356264__INCLUDED_)
#define AFX_SCENESKINMODEL_H__AF9E2B62_35AF_4DE8_82AE_C7E230356264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"
class CECModel;

struct EC_SKINMODEL_PROPERTY
{
	AString strPathName;
	AString strActionName;
	DWORD   dwActionID;
	float   fAlphaFlags;
};

class CSceneSkinModel :public  CSceneObject
{
public:
								CSceneSkinModel();
	virtual						~CSceneSkinModel();
	virtual void				Release();


	virtual A3DVECTOR3			GetMin();
	virtual A3DVECTOR3			GetMax();

	virtual void				Render(A3DViewport* pA3DViewport);
	virtual void                RenderForLight(A3DViewport* pA3DViewport, float fOffsetX, float fOffsetZ);
	virtual void				RenderForLightMapShadow(A3DViewport* pA3DViewport);    

	virtual void				Tick(DWORD timeDelta);
	
	virtual void				Translate(const A3DVECTOR3& vDelta);
	virtual void				RotateX(float fRad);
	virtual void				RotateY(float fRad);
	virtual void				RotateZ(float fRad);
	virtual void				Rotate(const A3DVECTOR3& axis,float fRad);
	virtual void				Scale(float fScale);
	virtual void				SetPos(const A3DVECTOR3& vPos);
	virtual void				UpdateProperty(bool bGet);
	virtual void                ResetRotateMat();
	virtual void SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up);
	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	virtual A3DVECTOR3			GetPos();
	virtual A3DMATRIX4			GetTransMatrix();
	virtual A3DMATRIX4			GetRotateMatrix();
	virtual A3DMATRIX4			GetScaleMatrix();

	bool						LoadSkinModel(AString strPathName);
	void                        BuildPropery();
	void                        BuildPropertyEx();

	EC_SKINMODEL_PROPERTY&      GetProperty(){ return m_Property; };
	void                        SetProperty(EC_SKINMODEL_PROPERTY prop){ m_Property = prop; };
	AString                     GetActNameByIndex(int index);
	
	//	Get EC model object
	inline CECModel* GetECModel() { return m_pECModel; }
	inline AString  GetECModelPath(){ return m_Property.strPathName; };

private:
	CECModel *m_pECModel;
	EC_SKINMODEL_PROPERTY m_Property;
	ASetTemplate<int> act_type;
	bool m_bInitedEx;
};

#endif // !defined(AFX_SCENESKINMODEL_H__AF9E2B62_35AF_4DE8_82AE_C7E230356264__INCLUDED_)
