/*
 * FILE: EC_Goblin.h
 *
 * DESCRIPTION: Goblin follow a player
 *
 * CREATED BY: Sunxuewei, 2008/11/06
 *
 */
#ifndef _EC_GOBLIN_H_
#define _EC_GOBLIN_H_

#include "A3DTypes.h"
#include "EC_Counter.h"
#include "EC_MsgDef.h"
#include "EC_IvtrTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECModel;
class CECPlayer;
class CECViewport;
class CECInventory;
class CECSkill;
class CECIvtrGoblin;

namespace S2C
{
	struct IVTR_ESSENCE_GOBLIN;
};
struct GOBLIN_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGoblin
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 
//
///////////////////////////////////////////////////////////////////////////
class CECGoblin
{
public:	
	// 小精灵的状态
	enum GOBLIN_STATE
	{
		GOBLIN_STATE_NULL = 0,		// 无
		GOBLIN_STATE_IDLE,			// 歇着
		GOBLIN_STATE_ATTACK,		// 攻击
		GOBLIN_STATE_CHASE,			// 追主人
		GOBLIN_STATE_TRICK1,		// 第一种花样
		GOBLIN_STATE_TRICK2,		// 第二种花样
		GOBLIN_STATE_TRICK3,		// 第三种花样
		GOBLIN_STATE_TRICK4,		// 第四种花样
		GOBLIN_STATE_TRICK5,		// 第五种花样
		GOBLIN_STATE_TRICK6,		// 第六种花样
	};

	// 小精灵的模型调用
	enum GOBLIN_MODEL_ID
	{
		GOBLIN_MODEL1 = 1,				
		GOBLIN_MODEL2,				
		GOBLIN_MODEL3,				
		GOBLIN_MODEL4,				
	};

protected:	
	GOBLIN_MODEL_ID	m_idModel;			// 小精灵的模型ID
	int				m_tid;				// 小精灵的模板ID
	CECPlayer *		m_pPlayer;			// player that this Goblin is following

	int				m_iRefineLvl;		// 当前精炼等级
	bool			m_bRefineActive;	// 精炼效果是否激活

	CECModel*		m_pModel;			// 小精灵的模型
	
 	CECSkill*		m_pCurSkill;		// 当前释放的技能
 	CECCounter		m_SkillCnt;			// 技能释放时间
	int				m_idCurSkillTarget;	// 技能释放目标

	GOBLIN_STATE	m_curState;			// current state;
	DWORD			m_dwStateTicks;		// current state ticks.
	DWORD			m_dwIdleAllowed;	// idle ticks that can be allowed

	A3DVECTOR3		m_vecPos;			// current position
	A3DVECTOR3		m_vecDir;			// current direction

	float			m_vSpeed;			// current moving speed;
	float			m_vSpeedMax;		// max speed that can use
	float			m_vAcceleration;	// acceleration
	float			m_vDeceleration;	// deceleration
	
	GOBLIN_ESSENCE* m_pDBEssence;		//  小精灵本体模板
	
public:		//	Constructor and Destructor
	
	CECGoblin();
	virtual ~CECGoblin();
	
	GOBLIN_MODEL_ID GetModelID() const		{ return m_idModel; }
	int	GetTemplateID() const				{ return m_tid; }	
	CECPlayer* GetPlayer()					{ return m_pPlayer; }
	
	CECModel* GetModel()					{ return m_pModel; }

	//  释放技能
	CECSkill* GetCurrSkill()				{ return m_pCurSkill; }
	CECCounter& GetSkillCnt() 				{ return m_SkillCnt; }
	int GetCurrSkillTarget()				{ return m_idCurSkillTarget; }

	const A3DVECTOR3& GetPos()				{ return m_vecPos; }
	const A3DVECTOR3& GetDir()				{ return m_vecDir; }

	//	精练等级
	int GetRefineLevel() const				{ return m_iRefineLvl; }	
	//  精炼效果是否激活
	bool IsRefineActive() const 			{ return m_bRefineActive; }
	void SetRefineActive(bool bActive)		{ m_bRefineActive = bActive; }

	//  小精灵的名字
	const wchar_t* GetName();

	//  本体模板
	const GOBLIN_ESSENCE* GetDBEssence()	{ return m_pDBEssence; }

public:		//	Operations
	//	Initialize object
	bool Init(int tid, int idModel, int iRefineLvl, CECPlayer* pPlayer);
	virtual bool Init(int tid, CECIvtrGoblin* pIvtrGoblin, CECPlayer* pPlayer);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	Set target position
	bool SetPos(const A3DVECTOR3& vecPos);	
	//  Set transparent
	void SetTransparent(float fTransparent);

	// Play skill action
	bool PlaySkillAction(int idSkill, float fWeight, bool bRestart=true, int iTransTime=200, bool bQueue=false);
	// Play refine active effect
	void PlayRefineActiveEffect();
	// Goblin level up
	void LevelUp();
	
	//  消息处理
	void OnMsgGoblinRefineActive(const ECMSG &Msg);
	void OnMsgGoblinCastSkill(const ECMSG &Msg);
};
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif//_EC_GOBLIN_H_