// CritterGroup.h: interface for the CCritterGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITTERGROUP_H__1EC3F622_8CD6_47A8_94AE_9060FCC0C90F__INCLUDED_)
#define AFX_CRITTERGROUP_H__1EC3F622_8CD6_47A8_94AE_9060FCC0C90F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AObject.h"

class CECBezier;
class CSceneObject;
class A3DAutonomousGroup;
class A3DAutonomousBehaviour;
class CECBezierWalker;

struct CRITTER_DATA
{
	float		fRadius;				//半径
	DWORD		dwNumCritter;			//生物的个数
	DWORD		dwBehaviourType;		//行为类型
	DWORD       dwCritterType;          //生物类型
	int         nBezierID;				//曲线的ID,如果没有B曲线，那么该选项为-1
	AString     strBezierName;			//B曲线的名字，导出时不用，只是为了编辑器显示用
	float       fSpeed;                 //移动速度
	AString		strModelPathName;		//模型的名字
};

void InitCritterData( CRITTER_DATA &data);

struct BEZIER_SEL_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

class CCritterGroup :public CSceneObject  
{

public:
	enum
	{
		BEHAVIOUR_WANDER = 0,
		BEHAVIOUR_HOVER,//
		BEHAVIOUR_ARRIVAL,
		BEHAVIOUR_BOID,//
		BEHAVIOUR_FLEE,
		BEHAVIOUR_FLEE_AND_BACK,
	};

	enum
	{
		CRITTER_BUTTERFLY = 0,
		CRITTER_NORMAL_FISH,
		CRITTER_DOLTISH_FISH,
		CRITTER_BRISK_FISH,
		CRITTER_EAGLE,
		CRITTER_FIREFLY,
		CRITTER_CROW,
	};

public:
	CCritterGroup();
	virtual ~CCritterGroup();

	virtual void				Release();

	virtual void				Translate(A3DVECTOR3 vDelta);
	virtual void				SetPos(A3DVECTOR3 vPos);

	virtual void				Render(A3DViewport* pA3DViewport);
	virtual void				Tick(DWORD timeDelta);
	virtual void				UpdateProperty(bool bGet);
	//  Copy object
	virtual CSceneObject*		CopyObject();

	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	//判断区域是否跟该区域相交
	bool IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);

	void BuildProperty();
	inline void SetProperty( CRITTER_DATA & data){ m_Data = data; };
	inline CRITTER_DATA& GetProperty(){ return m_Data; };
	void BindBezier();
	inline void ResetGroup(){ m_pGroup = NULL; m_pBehaviour = NULL; };
	inline A3DAutonomousGroup* GetA3DAutonomousGroup(){ return m_pGroup; };

private:
	void UpdateGroup();
	void DrawCircle();
	//应付后来行为减少后，与以前的文件版本兼容
	void Repair_Behaviour(DWORD& dwBehaviour);
	void DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);

	//Test CECBezier
	CECBezier * m_pECBezier;
	CECBezierWalker* m_pBezierWalker;

private:
	float m_fScale;
	CRITTER_DATA m_Data;
	ASetTemplate<int> critter_type;
	ASetTemplate<int> behaviour_type;
	BEZIER_SEL_ID_FUNCS *m_pIDSelFuncs;
	AString m_strBezierName;


	A3DAutonomousGroup* m_pGroup;
	A3DAutonomousBehaviour* m_pBehaviour;

};

#endif // !defined(AFX_CRITTERGROUP_H__1EC3F622_8CD6_47A8_94AE_9060FCC0C90F__INCLUDED_)
