// File		: EC_ProfDisplay.h
// Creator	: Xu Wenbin
// Date		: 2013/9/27

#pragma once

#include "EC_RoleTypes.h"

#include <A3DVector.h>
#include <ABaseDef.h>

//	RotateAnimator : �����ȡ��תչʾ�����и�����λ��
class RotateAnimator
{
	A3DVECTOR3	m_center;		//	��ת����λ��
	float		m_radius;		//	Ŀ���� m_center ����
	int			m_count;		//	Χ�����ĵ�Ŀ�����
	float		m_deltaDist;	//	ͬһ����λ�ò�ͬ����ְҵ�����������λ�ü��
	float		m_selectMove;	//	ѡ��ְҵ������ƶ����루���δѡ��ʱλ�ã�
	A3DVECTOR3	m_dir;			//	��ת�Ƕ�Ϊ0ʱ��Ӧ������
	A3DVECTOR3	m_dirTangent;	//	m_dir �����������ڼ���λ��ƫ�ƣ��ﵽһ��λ�ø�����ȡ���λ�õ�����
	double		m_angleStart;	//	ת��Ŀ��Ŀ�ʼλ��
	double		m_angle;		//	��ǰ�Ƕ�
	double		m_angleTarget;		//	Ŀ��Ƕ�
	DWORD		m_timeCost;			//	ת��Ŀ�껨����ʱ��
	DWORD		m_timeElapsed;		//	ת��Ŀ��λ���ѻ��ѵ�ʱ�䣨��m_angleStartλ�ø���ʱ����
	bool		m_rotating;			//	�Ƿ�������ת��
	int			m_firstIndicator;	//	δ��תʱת�̼�ͷָ���λ���±�
	int			m_targetIndicator;	//	ת�̼�ͷ���ս�ָ���λ���±�

	enum RotateDirection
	{
		ROTATE_CLOCKWISE,		//	˳ʱ����ת
		ROTATE_ANTICLOCKWISE,	//	��ʱ����ת
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

//	����ʱһ���� Trigger
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
//	ְҵ��Ϊ�������õ���״̬
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
	//	Ĭ��״̬��վ��Ĭ��λ�þ�Ĭ
public:
	virtual void TriggerMoveFront();
	virtual void Enter(ProfessionTriggerState *prev);
};
class PTStateWaitRotator : public ProfessionTriggerState
{
	//	Ĭ��λ�á���ѡ�У��ȴ���ת��ɿ�������״̬
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
};
class PTStateWaitMoveFront : public ProfessionTriggerState
{
	//	�ȴ���ʼ�ƶ�
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
};
class PTStateMoveFront : public ProfessionTriggerState
{
	//	�߽��ƶ���
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
	virtual float GetBackToFrontRatio();
};
class PTStateWaitDisplay : public ProfessionTriggerState
{
	//	�ȴ�����չʾ����
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual float GetBackToFrontRatio(){ return 1.0f; }
};
class PTStateDisplay : public ProfessionTriggerState
{
	//	����չʾ������...
public:
	virtual void TriggerGoback();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Rest();
	virtual float GetBackToFrontRatio(){ return 1.0f; }
};
class PTStateGoback : public ProfessionTriggerState
{
	//	�߻�Ĭ��λ���ƶ���...
public:
	virtual void TriggerMoveFront();
	virtual void Enter(ProfessionTriggerState *prev);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Rest();
	virtual float GetBackToFrontRatio();
};

//	ְҵ��Ϊ������
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
	
	//	��״̬���õĶ���
	ActionTrigger			m_counter;
	bool					m_moveFrontActionTriggered;
	bool					m_gobackActionTriggered;
	bool					m_displayActionTriggered;
	bool					m_standActionTriggered;

public:
	ProfessionTrigger();
	void Reset();

	//	ת������ State �ķ���
	void TriggerMoveFront();
	void TriggerGoback();
	void Tick(DWORD dwDeltaTime);
	void Rest();
	float GetBackToFrontRatio();
	
	//	���� State ����ʹ�õķ���
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

	//	����ѯ����
	bool MoveFrontActionTriggered();
	bool GobackActionTriggered();
	bool DisplayActionTriggered();
	bool StandActionTriggered();
	bool IsGoingback();
};

//	ְҵչʾ�������ſ���
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
