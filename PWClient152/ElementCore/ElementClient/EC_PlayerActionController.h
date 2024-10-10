// Filename	: EC_PlayerActionController.h
// Creator	: Xu Wenbin
// Date		: 2014/7/19

#ifndef _ELEMENTCLIENT_EC_PLAYERACTIONCONTROLLER_H_
#define _ELEMENTCLIENT_EC_PLAYERACTIONCONTROLLER_H_

#include <vector>

#include <ABaseDef.h>

#include <EC_Model.h>

class CECPlayer;
class CECModel;
class CECPlayerActionPlayPolicy{
	CECPlayer				*	m_pPlayer;
	CECModel				*	m_pPlayerModel;

protected:
	CECModel *  GetModel()const;
	CECPlayer*	GetPlayer()const;

	bool IsMoving()const;

public:
	CECPlayerActionPlayPolicy(CECPlayer *pPlayer, CECModel *pPlayerModel);
	virtual ~CECPlayerActionPlayPolicy(){}

	virtual bool CanCombineWithMoveForSkill(int idSkill=-1)const;
	static  bool IsMoveAction(DWORD dwUserData);

	virtual bool PlayNonSkillActionWithName(int iAction, const char* szActName, bool bRestart, int nTransTime, bool bNoFx, bool *pActFlag, DWORD dwFlagMode)=0;
	virtual bool QueueNonSkillActionWithName(int iAction, const char* szActName, int nTransTime, bool bForceStopPrevAct, bool bNoFx, bool bResetSpeed, bool bResetActFlag, bool *pNewActFlag, DWORD dwNewFlagMode)=0;
	
	virtual bool PlaySkillCastActionWithName(int idSkill, const char *szActName, bool bNoFX)=0;
	virtual bool PlaySkillAttackActionWithName(int idSkill, const char *szActName, bool bNoFX, bool *pActFlag, DWORD dwFlagMode)=0;
	virtual bool QueueSkillAttackActionWithName(int idSkill, const char *szActName, int nTransTime, bool bNoFX, bool bResetSpeed, bool bResetActFlag, bool *pNewActFlag, DWORD dwNewFlagMode)=0;

	virtual bool PlayWoundActionWithName(const char* szActName)=0;

	virtual void ClearComActFlagAllRankNodes(bool bSignalCurrent)=0;
	virtual void StopChannelAction()=0;
	virtual void StopSkillAction()=0;
	
	virtual bool IsPlayingAction(int iAction)const=0;
	virtual bool IsPlayingMoveAction()const=0;
	bool IsPlayingCastingSkillAction()const;
	virtual int  GetLowerBodyAction()const=0;
};

class CECPlayer;
class CECModel;
class CECPlayerActionController{
	CECPlayer				*	m_pPlayer;						//	角色指针（供动作相关其它状态查询）
	CECModel				*	m_pPlayerModel;					//	人物基本模型
	bool						m_bSupportCastSkillWhenMove;	//	当前模型是否支持“移动施法”
	CECPlayerActionPlayPolicy*	m_actionPlayPolicy;				//	播放逻辑
	bool						m_bSkillAttackActionPlayed;		//	当前技能的攻击动作播放过（用于判断是否可以停止技能吟唱动作）

public:
	enum{
		ACT_CHANNEL_UPPERBODY	= 1,
		ACT_CHANNEL_LOWERBODY,
		ACT_CHANNEL_WOUND,
		ACT_CHANNEL_COUNT,
	};

private:
	//	action channel
	typedef std::vector<int> Bones;
	Bones GetUpperBodyBones()const;
	
	Bones GetBonesIDFromName(int bonesNameCount, const char ** szBonesName) const;
	Bones GetLowerBodyBones()const;
	Bones GetWoundChannelBones()const;
	bool  BuildChannelForCastSkillWhenMove();

	//	action play policy
	void InitializeActionPlayPolicy();
	void ReleaseActionPlayPolicy();

public:
	CECPlayerActionController();
	~CECPlayerActionController();

	void Bind(CECPlayer *player, CECModel *playerModel);

	bool SupportCastSkillWhenMove()const;
	bool CanCombineWithMoveForSkill(int idSkill=-1)const;
	
	bool PlayNonSkillActionWithName(int iAction, const char* szActName, bool bRestart=true, int nTransTime=200, bool bNoFx=false, bool *pActFlag=NULL, DWORD dwFlagMode=COMACT_FLAG_MODE_NONE);
	bool QueueNonSkillActionWithName(int iAction, const char* szActName, int nTransTime=200, bool bForceStopPrevAct=false, bool bNoFx=false, bool bResetSpeed=false, bool bResetActFlag=false, bool *pNewActFlag=NULL, DWORD dwNewFlagMode=COMACT_FLAG_MODE_NONE);
	
	bool PlaySkillCastActionWithName(int idSkill, const char *szActName, bool bNoFX=false);
	bool PlaySkillAttackActionWithName(int idSkill, const char *szActName, bool bNoFX=false, bool *pActFlag=NULL, DWORD dwFlagMode=COMACT_FLAG_MODE_NONE);
	bool QueueSkillAttackActionWithName(int idSkill, const char *szActName, int nTransTime=200, bool bNoFX=false, bool bResetSpeed=false, bool bResetActFlag=false, bool *pNewActFlag=NULL, DWORD dwNewFlagMode=COMACT_FLAG_MODE_NONE);

	bool PlayWoundActionWithName(const char* szActName);

	void ClearComActFlagAllRankNodes(bool bSignalCurrent);
	void StopChannelAction();
	void StopSkillCastAction();
	void StopSkillAttackAction();

	bool IsPlayingAction(int iAction)const;
	bool IsPlayingCastingSkillAction()const;
	bool IsPlayingMoveAction()const;
	int  GetLowerBodyAction()const;
};

#endif	//	_ELEMENTCLIENT_EC_PLAYERACTIONCONTROLLER_H_