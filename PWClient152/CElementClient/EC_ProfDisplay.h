// File		: EC_ProfDisplay.h
// Creator	: Xu Wenbin
// Date		: 2013/9/27

#pragma once

#include "EC_RoleTypes.h"

#include <A3DVector.h>
#include <ABaseDef.h>

//	RotateAnimator : 负责获取旋转展示过程中各种族位置
class RotateAnimator
{
	A3DVECTOR3	m_center;		//	旋转中心位置
	float		m_radius;		//	目标与 m_center 距离
	int			m_count;		//	围绕中心的目标个数
	float		m_deltaDist;	//	同一种族位置不同族内职业相对种族中心位置间距
	float		m_selectMove;	//	选中职业向相机移动距离（相对未选中时位置）
	A3DVECTOR3	m_dir;			//	旋转角度为0时对应的轴向
	A3DVECTOR3	m_dirTangent;	//	m_dir 的切向方向，用于计算位置偏移，达到一个位置附近获取多个位置的能力
	double		m_angleStart;	//	转向目标的开始位置
	double		m_angle;		//	当前角度
	double		m_angleTarget;		//	目标角度
	DWORD		m_timeCost;			//	转往目标花费总时间
	DWORD		m_timeElapsed;		//	转往目标位置已花费的时间（从m_angleStart位置更新时算起）
	bool		m_rotating;			//	是否正在旋转中
	int			m_firstIndicator;	//	未旋转时转盘箭头指向的位置下标
	int			m_targetIndicator;	//	转盘箭头最终将指向的位置下标

	enum RotateDirection
	{
		ROTATE_CLOCKWISE,		//	顺时针旋转
		ROTATE_ANTICLOCKWISE,	//	逆时针旋转
	};
	RotateDirection	m_rotateDir;
	
	RotateAnimator(int count);
	RotateAnimator(const RotateAnimator &);
	RotateAnimator & operator=(const RotateAnimator &);

public:
	static const float RA_DEFAULT_RADIUS;
	static const float RA_DEFAULT_DELTA_DIST;
	static const float RA_DEFAULT_SELECT_MOVE;

	static RotateAnimator & Instance();

	double GetAngleBetween()const;
	void SetCenter(const A3DVECTOR3 &center);
	void SetDir(const A3DVECTOR3 &dir);
	const A3DVECTOR3 & GetDir()const;
	void SetRadius(float radius);
	void SetDeltaDist(float dist);
	void SetSelectMove(float dist);
	float GetSelectMove()const;
	void SetFirstIndicator(int indicator);
	double GetBestAngleRepresentation(double angle);
	void RotateToIndicator(int indicator);
	bool Tick(DWORD dwDeltaTime);
	void RotateFinished();
	bool IsRotating()const{ return m_rotating; }
	A3DVECTOR3 GetCenteredIndicatorPos(int indicator);
	A3DVECTOR3 GetIndicatorPos(int indicator);
	A3DVECTOR3 GetIndicatorDeltaPos(int deltaIndex, float fMoveFrontRatio);
};

//	倒计时一次性 Trigger
class ActionTrigger
{
	bool	m_bTriggered;
	DWORD	m_triggerTime;
	DWORD	m_triggerLength;
public:
	ActionTrigger();
	void Reset();
	void StartCounting(DWORD dwTime);
	void ReverseCounting(DWORD dwTime);
	void Tick(DWORD dwDeltaTime);
	bool GetTriggered();
	float GetLeftPercent()const;
};

class ProfessionTrigger;
//	职业行为控制器用到的状态
class ProfessionTriggerState
{
	ProfessionTrigger*	m_parent;
public:
	virtual ~ProfessionTriggerState(){}
	ProfessionTriggerState() : m_parent(NULL){
	}
	void Init(ProfessionTrigger *parent){
		m_parent = parent;
	}
	ProfessionTrigger * GetParent(){
		return m_parent;
	}
	virtual void TriggerMoveFront(){}
	virtual void TriggerGoback(){}
	virtual void Enter(ProfessionTriggerState *prev){}
	virtual void Tick(DWORD dwDeltaTime){}
	virtual void Rest(){}
	virtual float GetBackToFrontRatio(){ return 0.0f; }
};
class PTStateBack : public ProfessionTriggerState
{
	//	默认状态：站在默认位置静默
public:
	virtual void TriggerMoveFront();
	virtual void Enter(ProfessionTriggerState *prev);
};
class PTStateWaitRotator : public ProfessionTriggerState
{
	//	默认位置、已选中，等待旋转完成开启后续状态
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
};
class PTStateWaitMoveFront : public ProfessionTriggerState
{
	//	等待开始移动
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
};
class PTStateMoveFront : public ProfessionTriggerState
{
	//	走近移动中
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
	virtual float GetBackToFrontRatio();
};
class PTStateWaitDisplay : public ProfessionTriggerState
{
	//	等待播放展示动作
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual float GetBackToFrontRatio(){ return 1.0f; }
};
class PTStateDisplay : public ProfessionTriggerState
{
	//	播放展示动作中...
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Rest();
	virtual float GetBackToFrontRatio(){ return 1.0f; }
};
class PTStateGoback : public ProfessionTriggerState
{
	//	走回默认位置移动中...
public:
	virtual void TriggerMoveFront();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
	virtual float GetBackToFrontRatio();
};

//	职业行为控制器
class ProfessionTrigger
{	
	ProfessionTriggerState*	m_curState;

	PTStateBack				m_stateBack;
	PTStateWaitRotator		m_stateWaitRotator;
	PTStateWaitMoveFront	m_stateWaitMoveFront;
	PTStateMoveFront		m_stateMoveFront;
	PTStateWaitDisplay		m_stateWaitDisplay;
	PTStateDisplay			m_stateDisplay;
	PTStateGoback			m_stateGoback;
	
	//	各状态共用的对象
	ActionTrigger			m_counter;
	bool					m_moveFrontActionTriggered;
	bool					m_gobackActionTriggered;
	bool					m_displayActionTriggered;
	bool					m_standActionTriggered;

public:
	ProfessionTrigger();
	void Reset();

	//	转发给各 State 的方法
	void TriggerMoveFront();
	void TriggerGoback();
	void Tick(DWORD dwDeltaTime);
	void Rest();
	float GetBackToFrontRatio();
	
	//	被各 State 对象使用的方法
	void ChangeState(ProfessionTriggerState *state);
	ProfessionTriggerState * GetStateBack(){ return &m_stateBack; }
	ProfessionTriggerState * GetStateWaitRotator(){ return &m_stateWaitRotator; }
	ProfessionTriggerState * GetStateWaitMoveFront(){ return &m_stateWaitMoveFront; }
	ProfessionTriggerState * GetStateMoveFront(){ return &m_stateMoveFront; }
	ProfessionTriggerState * GetStateWaitDisplay(){ return &m_stateWaitDisplay; }
	ProfessionTriggerState * GetStateDisplay(){ return &m_stateDisplay; }
	ProfessionTriggerState * GetStateGoback(){ return &m_stateGoback; }
	ActionTrigger & GetTrigger(){ return m_counter; }
	void TriggerMoveFrontAction();
	void TriggerGobackAction();
	void TriggerDisplayAction();
	void TriggerStandAction();
	void ClearTriggerAction();

	//	外界查询方法
	bool MoveFrontActionTriggered();
	bool GobackActionTriggered();
	bool DisplayActionTriggered();
	bool StandActionTriggered();
	bool IsGoingback();
};

//	职业展示动作播放控制
class DisplayActionTrigger
{
	ProfessionTrigger	m_trigger[NUM_PROFESSION];

	DisplayActionTrigger();
	DisplayActionTrigger(const DisplayActionTrigger &);
	DisplayActionTrigger & operator=(const DisplayActionTrigger &);

	bool Validate(int prof);

public:
	static DisplayActionTrigger & Instance();

	void Reset();
	void Rest();
	void Trigger(int prof);
	void Tick(DWORD dwDeltaTime);
	bool MoveFrontActionTriggered(int prof);
	bool GoBackActionTriggered(int prof);
	bool DisplayActionTriggered(int prof);
	bool StandActionTriggered(int prof);
	void GetPosDir(int prof, A3DVECTOR3& vPos, A3DVECTOR3& dir);
};
