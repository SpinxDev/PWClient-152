// File		: EC_ProfDisplay.cpp
// Creator	: Xu Wenbin
// Date		: 2013/9/27

#include "EC_ProfDisplay.h"
#include "EC_ProfConfigs.h"
#include "EC_Global.h"
#include "EC_Ease.h"

#include <A3DMacros.h>
#include <A3DFuncs.h>
#include <AAssist.h>

//	class RotateAnimator
const float RotateAnimator::RA_DEFAULT_RADIUS = 2.0f;
const float RotateAnimator::RA_DEFAULT_DELTA_DIST = 0.4f;
const float RotateAnimator::RA_DEFAULT_SELECT_MOVE = 1.6f;

RotateAnimator & RotateAnimator::Instance()
{
	static RotateAnimator s_dummy(NUM_RACE);
	return s_dummy;
}

RotateAnimator::RotateAnimator(int count)
: m_center(0.0f), m_radius(RA_DEFAULT_RADIUS), m_count(count)
, m_deltaDist(RA_DEFAULT_DELTA_DIST), m_selectMove(RA_DEFAULT_SELECT_MOVE)
, m_dir(0.0f, 0.0f, -1.0f), m_dirTangent(-1.0f, 0.0f, 0.0f)		
, m_angleStart(0.0f), m_angle(0.0f), m_angleTarget(0.0f)
, m_timeElapsed(0), m_timeCost(1000), m_rotating(false)
, m_firstIndicator(0), m_targetIndicator(0)
, m_rotateDir(ROTATE_ANTICLOCKWISE)
{}
double RotateAnimator::GetAngleBetween()const
{
	return A3D_2PI / m_count;
}
void RotateAnimator::SetCenter(const A3DVECTOR3 &center)
{
	m_center = center;
}
void RotateAnimator::SetDir(const A3DVECTOR3 &dir)
{
	m_dir = dir;
	m_dir.y = 0;
	m_dir.Normalize();
	m_dirTangent = a3d_RotatePosAroundY(m_dir, A3D_PI/2);
}
const A3DVECTOR3 & RotateAnimator::GetDir()const
{
	return m_dir;
}
void RotateAnimator::SetRadius(float radius)
{
	m_radius = radius;
}
void RotateAnimator::SetDeltaDist(float dist)
{
	m_deltaDist = dist;
}
void RotateAnimator::SetSelectMove(float dist)
{
	m_selectMove = dist;
}
float RotateAnimator::GetSelectMove()const
{
	return m_selectMove;
}
void RotateAnimator::SetFirstIndicator(int indicator)
{
	if (indicator < 0 || indicator >= m_count){
		assert(false);
	}else{			
		m_firstIndicator = indicator;
		m_targetIndicator = indicator;
		m_angleStart = m_angle = m_angleTarget = 0.0f;
		m_timeElapsed = 0;
		m_rotating = false;
		m_rotateDir = ROTATE_ANTICLOCKWISE;
		RotateFinished();
	}
}
double RotateAnimator::GetBestAngleRepresentation(double angle)
{
	//	将 angle 转化到 [0, A3D_2PI] 区间
	angle = fmod(angle, A3D_2PI);
	if (angle < 0){
		angle += A3D_2PI;
	}
	return angle;
}
void RotateAnimator::RotateToIndicator(int indicator)
{
	if (indicator == m_targetIndicator){
		return;
	}
	int lastIndicator = m_targetIndicator;
	m_targetIndicator = indicator;
	
	double newRotateAngle = -(indicator - lastIndicator) * GetAngleBetween();	//	由于 indicator 按顺时针朝向摆放，所以取负值
	double newTargetAngle = GetBestAngleRepresentation(m_angleTarget + newRotateAngle);
	double curAngle = GetBestAngleRepresentation(m_angle);
	double angleDiff = newTargetAngle - curAngle;
	m_angleTarget = newTargetAngle;
	if (GetBestAngleRepresentation(angleDiff) > A3D_PI){
		//	如果顺时针，需要大半个圈，因此需用减少的方式
		m_rotateDir = ROTATE_ANTICLOCKWISE;
		m_angle = curAngle;
		if (m_angle < newTargetAngle){
			m_angle += A3D_2PI;	//	保证 m_angle 大于 m_angleTarget 并逐渐减少
		}
	}else{
		m_rotateDir = ROTATE_CLOCKWISE;
		m_angle = curAngle;
		if (m_angle > newTargetAngle){
			m_angle -= A3D_2PI;	//	保证 m_angle 小于 m_angleTarget 并逐渐增加
		}
	}
	m_angleStart = m_angle;
	m_timeCost = 3000 * fabs(m_angle - m_angleTarget) / A3D_2PI;
	if (m_timeCost < 50){
		m_timeCost = 50;
	}
	m_timeElapsed = 0;
	m_rotating = true;
}

bool RotateAnimator::Tick(DWORD dwDeltaTime)
{
	if (!IsRotating()){
		return false;
	}
	m_timeElapsed += dwDeltaTime;
	float t = m_timeElapsed / (float)m_timeCost;
	a_ClampRoof(t, 1.0f);
	t = Ease::ExponentialOut(t);
	m_angle = m_angleStart + t * (m_angleTarget-m_angleStart);
	if (m_timeElapsed >= m_timeCost){
		m_angle = m_angleTarget;
		m_rotating = false;
		RotateFinished();
	}
	return true;
}
void RotateAnimator::RotateFinished()
{
}

A3DVECTOR3 RotateAnimator::GetCenteredIndicatorPos(int indicator)
{
	//	与 SetFirstIndicator 配合，保证不论如何旋转，同一参数 indicator 对应同一位置对象
	if (indicator < 0 || indicator >= m_count){
		assert(false);
		return A3DVECTOR3(0.0f);
	}
	int realIndex = (indicator - m_firstIndicator + m_count) % m_count;
	A3DVECTOR3 vZeroPos = m_dir * m_radius;
	return a3d_RotatePosAroundY(vZeroPos, (float)(m_angle + realIndex * GetAngleBetween()));	//	此函数决定了左手坐标系，indicator 顺时针旋转依次显示
}
A3DVECTOR3 RotateAnimator::GetIndicatorPos(int indicator)
{
	if (indicator < 0 || indicator >= m_count){
		assert(false);
		return A3DVECTOR3(0.0f);
	}
	return m_center + GetCenteredIndicatorPos(indicator);
}
A3DVECTOR3 RotateAnimator::GetIndicatorDeltaPos(int deltaIndex, float fMoveFrontRatio)
{
	A3DVECTOR3 delta = m_dirTangent * m_deltaDist; //	距离 indicator 位置偏移
	if (deltaIndex == 1){
		delta = -delta;
	}
	delta += m_dir * m_selectMove * fMoveFrontRatio;
	return delta;
}

//	class ActionTrigger
ActionTrigger::ActionTrigger()
{
	Reset();
}
void ActionTrigger::Reset()
{
	m_bTriggered = false;
	m_triggerTime = 0;
	m_triggerLength = 0;
}
void ActionTrigger::StartCounting(DWORD dwTime)
{
	m_bTriggered = false;
	m_triggerTime = dwTime;
	m_triggerLength = dwTime;
}
void ActionTrigger::ReverseCounting(DWORD dwTime)
{
	if (m_triggerLength > 0){
		m_triggerTime = m_triggerLength - m_triggerTime;
		float ratio = m_triggerTime/(float)m_triggerLength;
		m_triggerLength = dwTime;
		m_triggerTime = (DWORD)(int)(m_triggerLength * ratio);
	}
}
void ActionTrigger::Tick(DWORD dwDeltaTime)
{
	if (m_bTriggered || m_triggerTime == 0){
		return;
	}
	if (m_triggerTime <= dwDeltaTime){
		m_bTriggered = true;
		m_triggerTime = 0;
		m_triggerLength = 0;
		return;
	}
	m_triggerTime -= dwDeltaTime;
}
bool ActionTrigger::GetTriggered()
{		
	if (m_bTriggered){
		Reset();
		return true;
	}
	return false;
}
float ActionTrigger::GetLeftPercent()const
{
	return (m_triggerLength == 0) ? 0.0f : (float)m_triggerTime/(float)m_triggerLength;
}

//	class ProfessionTriggerState category
static const DWORD WAIT_MOVEFRONT_TIME = 100;
static const DWORD WAIT_DISPLAY_TIME = 300;
static const float PLAYER_WALK_SPEED = 2.0f;	//	角色走路的速度、用于计算移动时间
static DWORD GetMoveFrontTimeCost()
{
	//	从默认位置走到最前端所需时间
	float s = RotateAnimator::Instance().GetSelectMove() / PLAYER_WALK_SPEED;
	return (DWORD)(s*1000);
}
static DWORD GetGobackTimeCost()
{
	//	从最前端走回默认位置所需时间
	return GetMoveFrontTimeCost();
}

//	class PTStateBack
void PTStateBack::TriggerMoveFront()
{
	GetParent()->ChangeState(GetParent()->GetStateWaitRotator());
}
void PTStateBack::Enter(ProfessionTriggerState *prev)
{
	GetParent()->GetTrigger().Reset();
	GetParent()->TriggerStandAction();
}
//	class PTStateWaitRotator
void PTStateWaitRotator::TriggerGoback()
{
	GetParent()->ChangeState(GetParent()->GetStateBack());
}
void PTStateWaitRotator::Enter(ProfessionTriggerState *prev)
{
	GetParent()->GetTrigger().Reset();
	GetParent()->TriggerStandAction();
}
void PTStateWaitRotator::Tick(DWORD dwDeltaTime)
{
	if (!RotateAnimator::Instance().IsRotating()){
		GetParent()->ChangeState(GetParent()->GetStateWaitMoveFront());
	}
}
void PTStateWaitRotator::Rest()
{
	GetParent()->ChangeState(GetParent()->GetStateWaitDisplay());
}
//	class PTStateWaitMoveFront
void PTStateWaitMoveFront::TriggerGoback()
{
	GetParent()->ChangeState(GetParent()->GetStateBack());
}
void PTStateWaitMoveFront::Enter(ProfessionTriggerState *prev)
{
	GetParent()->GetTrigger().StartCounting(WAIT_MOVEFRONT_TIME);
}
void PTStateWaitMoveFront::Tick(DWORD dwDeltaTime)
{
	GetParent()->GetTrigger().Tick(dwDeltaTime);
	if (GetParent()->GetTrigger().GetTriggered()){
		GetParent()->ChangeState(GetParent()->GetStateMoveFront());
	}
}
void PTStateWaitMoveFront::Rest()
{
	GetParent()->ChangeState(GetParent()->GetStateWaitDisplay());
}
//	class PTStateMoveFront
void PTStateMoveFront::TriggerGoback()
{
	GetParent()->ChangeState(GetParent()->GetStateGoback());
}
void PTStateMoveFront::Enter(ProfessionTriggerState *prev)
{
	GetParent()->GetTrigger().StartCounting(GetMoveFrontTimeCost());
	GetParent()->TriggerMoveFrontAction();
}
void PTStateMoveFront::Tick(DWORD dwDeltaTime)
{
	GetParent()->GetTrigger().Tick(dwDeltaTime);
	if (GetParent()->GetTrigger().GetTriggered()){
		GetParent()->ChangeState(GetParent()->GetStateWaitDisplay());
	}
}
void PTStateMoveFront::Rest()
{
	GetParent()->ChangeState(GetParent()->GetStateWaitDisplay());
}
float PTStateMoveFront::GetBackToFrontRatio()
{
	return 1.0f - GetParent()->GetTrigger().GetLeftPercent();
}
//	class PTStateWaitDisplay
void PTStateWaitDisplay::TriggerGoback()
{
	GetParent()->ChangeState(GetParent()->GetStateGoback());
}
void PTStateWaitDisplay::Enter(ProfessionTriggerState *prev)
{
	GetParent()->GetTrigger().StartCounting(WAIT_DISPLAY_TIME);
	GetParent()->TriggerStandAction();
}
void PTStateWaitDisplay::Tick(DWORD dwDeltaTime)
{
	GetParent()->GetTrigger().Tick(dwDeltaTime);
	if (GetParent()->GetTrigger().GetTriggered()){
		GetParent()->ChangeState(GetParent()->GetStateDisplay());
	}
}
//	class PTStateDisplay
void PTStateDisplay::TriggerGoback()
{
	GetParent()->ChangeState(GetParent()->GetStateGoback());
}
void PTStateDisplay::Enter(ProfessionTriggerState *prev)
{
	GetParent()->TriggerDisplayAction();
}
void PTStateDisplay::Rest()
{
	GetParent()->ChangeState(GetParent()->GetStateWaitDisplay());
}
//	class PTStateGoback
void PTStateGoback::TriggerMoveFront()
{
	GetParent()->ChangeState(GetParent()->GetStateWaitRotator());
}
void PTStateGoback::Enter(ProfessionTriggerState *prev)
{
	if (prev == GetParent()->GetStateMoveFront()){
		GetParent()->GetTrigger().ReverseCounting(GetGobackTimeCost());
	}else{
		GetParent()->GetTrigger().StartCounting(GetGobackTimeCost());
	}
	GetParent()->TriggerGobackAction();
}
void PTStateGoback::Tick(DWORD dwDeltaTime)
{
	GetParent()->GetTrigger().Tick(dwDeltaTime);
	if (GetParent()->GetTrigger().GetTriggered()){
		GetParent()->ChangeState(GetParent()->GetStateBack());
	}
}
void PTStateGoback::Rest()
{
	GetParent()->ChangeState(GetParent()->GetStateBack());
}
float PTStateGoback::GetBackToFrontRatio()
{
	return GetParent()->GetTrigger().GetLeftPercent();
}

//	class ProfessionTrigger
ProfessionTrigger::ProfessionTrigger()
{
	GetStateBack()->Init(this);
	GetStateWaitRotator()->Init(this);
	GetStateWaitMoveFront()->Init(this);
	GetStateMoveFront()->Init(this);
	GetStateWaitDisplay()->Init(this);
	GetStateDisplay()->Init(this);
	GetStateGoback()->Init(this);
	Reset();
}
void ProfessionTrigger::TriggerMoveFrontAction()
{
	m_moveFrontActionTriggered = true;
	m_gobackActionTriggered = false;
	m_displayActionTriggered = false;
	m_standActionTriggered = false;
}
void ProfessionTrigger::TriggerGobackAction()
{
	m_moveFrontActionTriggered = false;
	m_gobackActionTriggered = true;
	m_displayActionTriggered = false;
	m_standActionTriggered = false;
}
void ProfessionTrigger::TriggerDisplayAction()
{
	m_moveFrontActionTriggered = false;
	m_gobackActionTriggered = false;
	m_displayActionTriggered = true;
	m_standActionTriggered = false;
}
void ProfessionTrigger::TriggerStandAction()
{
	m_moveFrontActionTriggered = false;
	m_gobackActionTriggered = false;
	m_displayActionTriggered = false;
	m_standActionTriggered = true;
}
void ProfessionTrigger::ClearTriggerAction()
{
	m_moveFrontActionTriggered = false;
	m_gobackActionTriggered = false;
	m_displayActionTriggered = false;
	m_standActionTriggered = false;
}
bool ProfessionTrigger::MoveFrontActionTriggered()
{
	if (m_moveFrontActionTriggered){
		m_moveFrontActionTriggered = false;
		return true;
	}
	return false;
}
bool ProfessionTrigger::GobackActionTriggered()
{
	if (m_gobackActionTriggered){
		m_gobackActionTriggered = false;
		return true;
	}
	return false;
}
bool ProfessionTrigger::DisplayActionTriggered()
{
	if (m_displayActionTriggered){
		m_displayActionTriggered = false;
		return true;
	}
	return false;
}
bool ProfessionTrigger::StandActionTriggered()
{
	if (m_standActionTriggered){
		m_standActionTriggered = false;
		return true;
	}
	return false;
}
bool ProfessionTrigger::IsGoingback()
{
	return m_curState == GetStateGoback();
}
void ProfessionTrigger::Reset()
{
	ClearTriggerAction();
	GetTrigger().Reset();
	ChangeState(GetStateBack());
}
void ProfessionTrigger::TriggerMoveFront()
{
	m_curState->TriggerMoveFront();
}
void ProfessionTrigger::TriggerGoback()
{
	m_curState->TriggerGoback();
}
void ProfessionTrigger::Tick(DWORD dwDeltaTime)
{
	m_curState->Tick(dwDeltaTime);
}
void ProfessionTrigger::Rest()
{
	m_curState->Rest();
}
float ProfessionTrigger::GetBackToFrontRatio()
{
	return m_curState->GetBackToFrontRatio();
}
void ProfessionTrigger::ChangeState(ProfessionTriggerState *state)
{
	if (state == m_curState){
		return;
	}
	ProfessionTriggerState *prev = m_curState;
	m_curState = state;
	m_curState->Enter(prev);
}

//	class DisplayActionTrigger
DisplayActionTrigger & DisplayActionTrigger::Instance()
{
	static DisplayActionTrigger s_dummy;
	return s_dummy;
}

DisplayActionTrigger::DisplayActionTrigger()
{
	Reset();
}
void DisplayActionTrigger::Reset()
{
	for (int i(0); i < ARRAY_SIZE(m_trigger); ++ i)
	{
		m_trigger[i].Reset();
	}
}
void DisplayActionTrigger::Rest()
{
	//	从当前状态直接到达最重要的状态（站到默认位置、或播放演武动画）
	for (int i(0); i < ARRAY_SIZE(m_trigger); ++ i)
	{
		m_trigger[i].Rest();
	}
}
bool DisplayActionTrigger::Validate(int prof)
{
	if (!CECProfConfig::Instance().IsProfession(prof)){
		assert(false);
		return false;
	}
	return true;
}
void DisplayActionTrigger::Trigger(int prof)
{
	if (Validate(prof)){
		for (int i(0); i < ARRAY_SIZE(m_trigger); ++ i)
		{
			if (i == prof){
				m_trigger[i].TriggerMoveFront();
			}else{
				m_trigger[i].TriggerGoback();
			}
		}
	}
}
void DisplayActionTrigger::Tick(DWORD dwDeltaTime)
{
	for (int i(0); i < ARRAY_SIZE(m_trigger); ++ i)
	{
		m_trigger[i].Tick(dwDeltaTime);
	}
}
bool DisplayActionTrigger::MoveFrontActionTriggered(int prof)
{
	if (Validate(prof)){
		return m_trigger[prof].MoveFrontActionTriggered();
	}
	return false;
}
bool DisplayActionTrigger::GoBackActionTriggered(int prof)
{
	if (Validate(prof)){
		return m_trigger[prof].GobackActionTriggered();
	}
	return false;
}
bool DisplayActionTrigger::DisplayActionTriggered(int prof)
{
	if (Validate(prof)){
		return m_trigger[prof].DisplayActionTriggered();
	}
	return false;
}
bool DisplayActionTrigger::StandActionTriggered(int prof)
{
	if (Validate(prof)){
		return m_trigger[prof].StandActionTriggered();
	}
	return false;
}
void DisplayActionTrigger::GetPosDir(int prof, A3DVECTOR3& vPos, A3DVECTOR3& dir)
{
	if (Validate(prof)){
		int race = CECProfConfig::Instance().GetRaceByProfession(prof);
		int showOrder = CECProfConfig::Instance().GetRaceShowOrder(race);
		A3DVECTOR3 racePos = RotateAnimator::Instance().GetIndicatorPos(showOrder);
		int showOrderInRace = CECProfConfig::Instance().GetProfessionShowOrderInRace(prof);
		vPos = racePos + RotateAnimator::Instance().GetIndicatorDeltaPos(showOrderInRace, m_trigger[prof].GetBackToFrontRatio());
		if (m_trigger[prof].IsGoingback()){
			dir = -RotateAnimator::Instance().GetDir();
		}else{
			dir = RotateAnimator::Instance().GetDir();
		}
	}else{
		vPos.Clear();
		dir = g_vAxisY;
	}
}