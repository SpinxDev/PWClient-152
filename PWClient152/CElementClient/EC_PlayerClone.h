/*
 * FILE: EC_ManPlayer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: 
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_PLAYERCLONE_H_
#define _EC_PLAYERCLONE_H_

#include "EC_Player.h"

class CECHostPlayer;
class CECElsePlayer;
class CECNavigateCtrl;

class CECClonePlayer : public CECPlayer
{
public:
	CECClonePlayer(CECPlayerMan* pPlayerMan);

public:
	//	Release object
	virtual void Release();	
	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0);	
	virtual CECModel* GetRenderModel();

	void RenderModel(CECViewport* pViewport, bool bHighlight);
	void AddShadower();
	void RenderPateContent(CECViewport* pViewport, bool bHighlight);
	void SetUseHintModelFlag(bool bUse) { m_bUseHintModel = bUse; }

	using CECPlayer::ClearComActFlagAllRankNodes;
	using CECPlayer::StopChannelAction;
	using CECPlayer::IsPlayingAction;
	using CECPlayer::PlayAction;
	using CECPlayer::PlayAttackAction;
	using CECPlayer::PlaySkillCastAction;
	using CECPlayer::PlaySkillAttackAction;

	void PlayActionByName(const char *szActName);
	bool HasComAct(const char *szActName);
	bool IsPlayingAction(const char *szActName);

	static bool CanClone(CECPlayer *player);
	bool Clone(CECPlayer *player, bool atOnce);

protected:
	bool Load(CECPlayer* player, bool atOnce);
	bool LoadFrom(CECHostPlayer* player, bool atOnce);
	bool LoadFrom(CECElsePlayer* player, bool atOnce);

	virtual bool ShouldLoadEquipment(int index)const;
	virtual void OnCloneSimpleProperty(){}

	bool IsFromHostPlayer()const;
	bool TickHostPlayer(DWORD dwDeltaTime);
	bool TickElsePlayer(DWORD dwDeltaTime);
	
protected:
	bool	m_bShowCustomize;	//	显示个性化
	bool	m_bUseHintModel;
};

//////////////////////////////////////////////////////////////////////////

class CECHostNavigatePlayer : public CECClonePlayer
{
public:
	CECHostNavigatePlayer(CECPlayerMan* pPlayerMan,CECHostPlayer* pHost);
	~CECHostNavigatePlayer(){}

protected:
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);	
	virtual bool ShouldLoadEquipment(int index)const;
	virtual void OnCloneSimpleProperty();

public:
	bool Init();
	void Release();
	bool LoadConfig();

	virtual bool Tick(DWORD dwDeltatime);
	virtual void OnAllResourceReady(){}			// 屏蔽CECPlayer::OnAllResourceReady行为
	virtual void SetPos(const A3DVECTOR3& vPos);

	virtual CECModel* GetRenderModel();

	bool ApplyNavigateModel();					// 挂载移动模型
	void SetNavigateModelFile(const char* szFile) {m_szNavigateModelFile = szFile;}

	bool PlayNavigateAction();
	A3DAABB GetShadowAABB();

	bool IsReadyNavigate();
	bool IsNavigateMoving();
	
	void OnNavigateEvent(int task,int e);

	CECNavigateCtrl* GetNavigateCtrl() { return m_pNavigateCtrl;}	

protected:
	CECHostPlayer*		m_pHostPlayer;

	const char*			m_szNavigateModelFile;
	CECModel*			m_pNavigateModel;

	CECNavigateCtrl*	m_pNavigateCtrl;		// 强制移动
	bool				m_bNavigateModelApplied;// 移动模型是否被挂载
};

class CECNPCClonedMaster : public CECClonePlayer{
public:
	CECNPCClonedMaster(CECPlayerMan* pPlayerMan);
	int	GetMoveAction()const;
	int	GetStandAction()const;
	virtual bool Tick(DWORD dwDeltaTime);
};

#endif