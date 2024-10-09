// SceneSpeciallyObject.h: interface for the CSceneSpeciallyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENESPECIALLYOBJECT_H__18A73D8F_3E1E_4039_8960_774BDFFA6943__INCLUDED_)
#define AFX_SCENESPECIALLYOBJECT_H__18A73D8F_3E1E_4039_8960_774BDFFA6943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

class A3DGraphicsFX;
class A3DGFXEx;

class CSceneSpeciallyObject :public CSceneObject
{
public:
	enum
	{
		GFX_TYPE_DAY = 0,
		GFX_TYPE_NIGHT,
		GFX_TYPE_BOTH,
	};


	CSceneSpeciallyObject();
	virtual ~CSceneSpeciallyObject();

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void EntryScene();
	virtual void LeaveScene();
	//  Copy object
	virtual CSceneObject*		CopyObject();
	
	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags); 
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);
	void UpdateGfx();

	//	Get effect scale factor
	inline float GetScaleFactor() { return m_matScale._11; }
	inline float GetPlaySpeed(){ return m_fPlaySpeed;}
	inline int   GetGfxType(){ return m_nGfxType; };
	inline float GetTransparency(){ return m_fTransparency; };
	inline bool  IsAttenuation(){ return m_bAttenuation; };
	//	Get GFX object
	inline A3DGFXEx* GetGfxExObject() { return m_pGfxEx; }
	//	Get GFX path name
	inline const char* GetGfxExFileName() { return m_strGfxPathName; }
	
	void DisableLoad(){ m_bLoading = false; };
	void SetGfxNull(){ m_pGfxEx = NULL; };
	
private:
	
	void BuildProperty();
	ASetTemplate<int> gfx_type;
	//Gfx file path name
	AString m_strGfxPathName;
	float   m_fPlaySpeed;
	A3DGFXEx*      m_pGfxEx;
	int     m_nGfxType;
	bool    m_bLoading;
	bool    m_bAttenuation;//特效的距离衰减开关
	float   m_fTransparency;//特效的透明程度
	//3
	float m_fOldAlpha;
};

#endif // !defined(AFX_SCENESPECIALLYOBJECT_H__18A73D8F_3E1E_4039_8960_774BDFFA6943__INCLUDED_)
