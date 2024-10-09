// SceneLightObject.h: interface for the CSceneLightObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENELIGHTOBJECT_H__0811D061_D6A5_487E_BE26_8348E86CBCC5__INCLUDED_)
#define AFX_SCENELIGHTOBJECT_H__0811D061_D6A5_487E_BE26_8348E86CBCC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"
#define BOX_WIDTH	0.5f
EXTERN_DEFINESET(int, Light_Type);

struct LIGHTDATA
{
	AString     m_strName;
	A3DVECTOR3  m_vDir;
	int         m_nType;
	float       m_fRange;
	DWORD       m_dwColor;
	DWORD		m_dwAbient;
	float       m_fFalloff;
    float       m_fAttenuation0;
    float       m_fAttenuation1;
    float       m_fAttenuation2;
    float       m_fTheta;
    float       m_fPhi;
	float       m_fColorScale;
	int         m_nEffectType;
	//2
};

void Init_Light_Data( LIGHTDATA &data);

class CSceneLightObject : public CSceneObject
{
public:
	enum 
	{
		LIGHT_EFFECT_DAY = 0,
		LIGHT_EFFECT_NIGHT,
		LIGHT_EFFECT_BOTH,
	};
	
	//.data，这儿是灯光对象特有的数据
	LIGHTDATA m_Data;
	
	ASetTemplate<int> light_type;
	ASetTemplate<int> light_effect_type;
	CSceneLightObject();
	virtual ~CSceneLightObject();
	
	virtual AString     GetObjectName();
	virtual void        SetObjectName(const AString& name);
	
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void UpdateProperty(bool bGet);

	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	//  Copy object
	virtual CSceneObject*		CopyObject();

	//把灯光数据取出来，主要是用于属性表更改UNDO
	void GetLightData(LIGHTDATA &data);
	void SetLightData(const LIGHTDATA &data);
	void BuildLightProperty();
private:
	void	Draw();
	void    DrawSphere(DWORD clr);
	void    DrawCircle();
	void    DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);
	void    DrawTaper(bool bPhi);
	void    DrawDirection();
	

private:
	float m_fScale;
};

typedef CSceneLightObject* PSCENELIGHTOBJECT;

#endif // !defined(AFX_SCENELIGHTOBJECT_H__0811D061_D6A5_487E_BE26_8348E86CBCC5__INCLUDED_)
