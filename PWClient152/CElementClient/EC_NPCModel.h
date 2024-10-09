// Filename	: EC_NPCModel.h
// Creator	: Xu Wenbin
// Date		: 2014/8/20

#ifndef _ELEMENTCLIENT_EC_NPCMODEL_H_
#define _ELEMENTCLIENT_EC_NPCMODEL_H_

#include <ABaseDef.h>
#include <A3DGeometry.h>

namespace CHBasedCD{
	class BrushTraceInfo;
}

//	class CECNPCModelPolicy
class CECViewport;
class A3DVECTOR3;
class A3DAABB;
struct ECRAYTRACE;
class A3DSkinModel;
class A3DSkeletonHook;
struct ECMODEL_GFX_PROPERTY;
class CECNPCModelPolicy{
public:
	virtual ~CECNPCModelPolicy(){}
	virtual void Release()=0;
	virtual void RebuildTraceBrush(){}
	virtual void ReleaseTraceBrush(){}
	virtual bool HasAction(int iAction)const=0;
	virtual void Tick(DWORD dwDeltaTime)=0;
	virtual void Render(CECViewport* pViewport, bool bHighlight)=0;
	virtual void AddShadower()=0;
	virtual void RenderPateContent(CECViewport* pViewport, bool bHighlight)=0;
	virtual void SetPos(const A3DVECTOR3& vPos)=0;
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)=0;
	
	virtual bool HasModelCHAABB()const=0;
	virtual bool GetModelCHAABB(A3DAABB &)const=0;
	virtual bool HasCHAABB()const=0;
	virtual bool GetCHAABB(A3DAABB &)const=0;
	virtual const A3DAABB & GetPickAABB()=0;
	virtual const A3DAABB & GetDefaultPickAABB()const=0;
	virtual void SetDefaultPickAABBExt(const A3DVECTOR3 &)=0;

	virtual bool RayTraceModelConvexHull(ECRAYTRACE* pTraceInfo)const=0;	
	virtual bool TraceWithBrush(CHBasedCD::BrushTraceInfo * pInfo)const=0;

	virtual bool IsModelLoaded()const=0;
	virtual bool GetTransparent(float &fTransparent)const=0;
	virtual void SetTransparent(float fTransparent)=0;
	virtual void RemoveGfx(const char* szPath, const char* szHook)=0;
	virtual void PlayGfx(const char* szPath, const char* szHook)=0;

	virtual void ClearComActFlag(bool bSignalCurrent)=0;
	virtual void StopChannelAction()=0;
	virtual void PlayActionByName(const char *szActName)=0;
	virtual bool HasComAct(const char *szActName)=0;
	virtual bool IsPlayingAction()=0;
	virtual bool IsPlayingAction(int iAction)=0;
	virtual bool IsPlayingAction(const char *szActName)=0;
	virtual bool PlayModelAction(int iAction, bool bRestart)=0;
	virtual bool PlayAttackAction(int nAttackSpeed, bool *pActFlag)=0;
	virtual void PlaySkillCastAction(int idSkill)=0;
	virtual bool PlaySkillAttackAction(int idSkill, int nAttackSpeed, int nSection, bool *pActFlag)=0;
	
	virtual bool GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const=0;
	virtual A3DSkinModel * GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook)=0;
	virtual A3DSkeletonHook * GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook)=0;

	virtual const ACHAR *GetNameToShow()const=0;

	virtual void OptimizeShowExtendStates()=0;
	virtual void OptimizeWeaponStoneGfx()=0;
	virtual void OptimizeArmorStoneGfx()=0;
	virtual void OptimizeSuiteGfx()=0;
};

class CECNPC;
class CECModel;
namespace CHBasedCD{ class CCDBrush; }
class A3DSkin;
struct EC_NPCLOADRESULT;
class CECNPCModelDefaultPolicy : public CECNPCModelPolicy{	
	CECNPC	*		m_pNPC;

	CECModel*		m_pNPCModel;		//	NPC model
	A3DSkin*		m_pNPCSkin;			//	NPC skin of normal lod
	A3DSkin*		m_pNPCSkinLow;		//	NPC skin of low lod
	A3DSkin*		m_pCurSkin;			//	NPC skin current shown	
	
	A3DAABB			m_aabbDefPick;		//	Default aabb used to pick
	
	int						m_nBrushes;			//	number of brush object used in collision
	CHBasedCD::CCDBrush**	m_ppBrushes;		//	Brush object used in collision
	A3DAABB					m_CHAABB;			//	AABB Updated with m_ppBrushes
	
	void ReleaseModel();	
	CECModel * GetSgcModel(const char *szHanger, bool bChildHook, const char *szHook);
	
	const char * GetActionName(int iAct, bool bAttackStart = false)const;
	const char * GetSkillCastActionName(int idSkill)const;
	const char * GetSkillAttackActionName(int idSkill, int nSection,bool bStart)const;
	const char * GetActionEnvString()const;

public:
	CECNPCModelDefaultPolicy(CECNPC *pNPC);
	virtual ~CECNPCModelDefaultPolicy();
	virtual void Release();
	virtual void RebuildTraceBrush();
	virtual void ReleaseTraceBrush();
	void SetNPCLoadedResult(const EC_NPCLOADRESULT& Ret);
	virtual bool HasAction(int iAction)const;
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Render(CECViewport* pViewport, bool bHighlight);
	virtual void AddShadower();
	virtual void RenderPateContent(CECViewport* pViewport, bool bHighlight);
	virtual void SetPos(const A3DVECTOR3& vPos);
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	
	virtual bool HasModelCHAABB()const;
	virtual bool GetModelCHAABB(A3DAABB &)const;
	virtual bool HasCHAABB()const;
	virtual bool GetCHAABB(A3DAABB &)const;
	virtual const A3DAABB & GetPickAABB();
	virtual const A3DAABB & GetDefaultPickAABB()const;
	virtual void SetDefaultPickAABBExt(const A3DVECTOR3 &vExt);

	virtual bool RayTraceModelConvexHull(ECRAYTRACE* pTraceInfo)const;
	virtual bool TraceWithBrush(CHBasedCD::BrushTraceInfo * pInfo)const;

	virtual bool IsModelLoaded()const;
	virtual bool GetTransparent(float &fTransparent)const;
	virtual void SetTransparent(float fTransparent);
	virtual void RemoveGfx(const char* szPath, const char* szHook);
	virtual void PlayGfx(const char* szPath, const char* szHook);

	virtual void ClearComActFlag(bool bSignalCurrent);
	virtual void StopChannelAction();
	virtual void PlayActionByName(const char *szActName);
	virtual bool HasComAct(const char *szActName);
	virtual bool IsPlayingAction();
	virtual bool IsPlayingAction(int iAction);
	virtual bool IsPlayingAction(const char *szActName);
	virtual bool PlayModelAction(int iAction, bool bRestart);
	virtual bool PlayAttackAction(int nAttackSpeed, bool *pActFlag);
	virtual void PlaySkillCastAction(int idSkill);
	virtual bool PlaySkillAttackAction(int idSkill, int nAttackSpeed,int nSection, bool *pActFlag);
	
	virtual bool GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const;
	virtual A3DSkinModel * GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook);
	virtual A3DSkeletonHook * GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook);

	virtual const ACHAR *GetNameToShow()const;
	
	virtual void OptimizeShowExtendStates();
	virtual void OptimizeWeaponStoneGfx();
	virtual void OptimizeArmorStoneGfx();
	virtual void OptimizeSuiteGfx();
};

class CECNPCClonedMaster;
class CECPlayer;
class CECNPCModelClonePlayerPolicy : public CECNPCModelPolicy{
	friend class CECNPC;

	CECNPC	*			m_pNPC;
	CECNPCClonedMaster *m_pPlayer;
	A3DAABB				m_aabbDefPick;		//	Default aabb used to pick
	
	void	ReleaseModel();
	static bool	NPCAction2PlayerAction(int iMoveEnv, int iNPCAction, int &iPlayerAction);
	bool	NPCAction2PlayerAction(int iNPCAction, int &iPlayerAction)const;
	CECModel * GetSgcModel(const char *szHanger, bool bChildHook, const char *szHook);

public:
	CECNPCModelClonePlayerPolicy(CECNPC *pNPC);
	virtual ~CECNPCModelClonePlayerPolicy();
	virtual void Release();
	bool SetPlayerToClone(CECPlayer *pPlayer);
	void InheritFromPreviousPolicy(const CECNPCModelPolicy *pOtherPolicy);
	virtual bool HasAction(int iAction)const;
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Render(CECViewport* pViewport, bool bHighlight);
	virtual void AddShadower();
	virtual void RenderPateContent(CECViewport* pViewport, bool bHighlight);
	virtual void SetPos(const A3DVECTOR3& vPos);
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	
	virtual bool HasModelCHAABB()const;
	virtual bool GetModelCHAABB(A3DAABB &)const;
	virtual bool HasCHAABB()const;
	virtual bool GetCHAABB(A3DAABB &)const;
	virtual const A3DAABB & GetPickAABB();
	virtual const A3DAABB & GetDefaultPickAABB()const;
	virtual void SetDefaultPickAABBExt(const A3DVECTOR3 &vExt);
	
	virtual bool RayTraceModelConvexHull(ECRAYTRACE* pTraceInfo)const;
	virtual bool TraceWithBrush(CHBasedCD::BrushTraceInfo * pInfo)const;
	
	virtual bool IsModelLoaded()const;
	virtual bool GetTransparent(float &fTransparent)const;
	virtual void SetTransparent(float fTransparent);
	virtual void RemoveGfx(const char* szPath, const char* szHook);
	virtual void PlayGfx(const char* szPath, const char* szHook);
	
	virtual void ClearComActFlag(bool bSignalCurrent);
	virtual void StopChannelAction();
	virtual void PlayActionByName(const char *szActName);
	virtual bool HasComAct(const char *szActName);
	virtual bool IsPlayingAction();
	virtual bool IsPlayingAction(int iAction);
	virtual bool IsPlayingAction(const char *szActName);
	virtual bool PlayModelAction(int iAction, bool bRestart);
	virtual bool PlayAttackAction(int nAttackSpeed, bool *pActFlag);
	virtual void PlaySkillCastAction(int idSkill);
	virtual bool PlaySkillAttackAction(int idSkill, int nAttackSpeed,int nSection, bool *pActFlag);
	
	virtual bool GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const;
	virtual A3DSkinModel * GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook);
	virtual A3DSkeletonHook * GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook);

	virtual const ACHAR *GetNameToShow()const;

	virtual void OptimizeShowExtendStates();
	virtual void OptimizeWeaponStoneGfx();
	virtual void OptimizeArmorStoneGfx();
	virtual void OptimizeSuiteGfx();
};

#endif	//	_ELEMENTCLIENT_EC_NPCMODEL_H_