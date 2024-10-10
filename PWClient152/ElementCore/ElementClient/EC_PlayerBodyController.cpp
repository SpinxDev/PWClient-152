// Filename	: EC_PlayerBodyController.cpp
// Creator	: Xu Wenbin
// Date		: 2014/8/11

#include "EC_PlayerBodyController.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Player.h"

#include <EC_Model.h>
#include <A3DSkeleton.h>
#include <A3DBoneController.h>
#include <A3DFuncs.h>

//	Local Function
static A3DVECTOR3 MakeOrthogonalUp(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp){
	//	根据 vDir 修改 vUp，使两者正交。返回修改后的 vUp
	A3DVECTOR3 vDirNormalized = Normalize(vDir);
	A3DVECTOR3 vRight = Normalize(CrossProduct(Normalize(vUp), vDirNormalized));
	return Normalize(CrossProduct(vDirNormalized, vRight));
}

//	CECPlayerBodyController
CECPlayerBodyController::CECPlayerBodyController()
: m_pPlayer(NULL)
, m_pPlayerModel(NULL)
, m_pSpine1TurnController(NULL)
, m_pSpine2TurnController(NULL)
, m_vMoveDirection(0.0f)
, m_vModelTargetUp(g_vAxisY)
, m_vModelTargetDirection(0.0f)
, m_facingTargetID(0)
, m_targetTurningAngle(0.0f)
, m_currentTurningAngle(0.0f)
{
}

CECPlayerBodyController::~CECPlayerBodyController(){
	Unbind();
}

bool CECPlayerBodyController::Bind(CECPlayer *player, CECModel *playerModel){
	if ((player != NULL) != (playerModel != NULL)){
		return false;	//	输入参数需同时为空或不为空
	}
	if (player == m_pPlayer && playerModel == m_pPlayerModel){
		return true;
	}
	Unbind();
	m_pPlayer = player;
	m_pPlayerModel = playerModel;
	SetModelTargetDirection(m_pPlayer->GetDir());
	SetModelTargetUp(m_pPlayer->GetUp());
	if (!InitializeBoneControllers()){
		Unbind();
		return false;
	}
	return true;
}

void CECPlayerBodyController::Unbind(){
	if (!m_pPlayer){
		return;
	}
	ReleaseBoneControllers();
	m_pPlayer = NULL;
	m_pPlayerModel = NULL;
	m_vMoveDirection.Clear();
	m_vModelTargetUp = g_vAxisY;
	m_vModelTargetDirection.Clear();
	m_facingTargetID = 0;
	m_targetTurningAngle = m_currentTurningAngle = 0;
}

bool CECPlayerBodyController::HasBone(const char *szBoneName)const{
	if (m_pPlayerModel != NULL){
		int iBoneIdx = -1;
		return m_pPlayerModel->GetA3DSkinModel()->GetSkeleton()->GetBone(szBoneName, &iBoneIdx) != NULL;
	}
	return false;
}

bool CECPlayerBodyController::InitializeBoneControllers(){
	bool result(false);
	while (true){
		const char *szSpline1BoneName = "Bip01 Spine1";
		const char *szSpline2BoneName = "Bip01 Spine2";
		if (!HasBone(szSpline1BoneName) ||
			!HasBone(szSpline2BoneName)){
			break;
		}
		if (!(m_pSpine1TurnController = CreateBoneController(szSpline1BoneName)) ||
			!(m_pSpine2TurnController = CreateBoneController(szSpline2BoneName))){
			break;
		}
		result = true;
		break;
	}
	if (!result){
		ReleaseBoneControllers();
	}
	return result;
}

A3DBoneBodyTurnController * CECPlayerBodyController::CreateBoneController(const char *szBoneName){
	A3DBoneBodyTurnController *result = NULL;
	if (m_pPlayerModel && szBoneName){
		int iBoneIdx = -1;
		A3DBone* pBone = m_pPlayerModel->GetA3DSkinModel()->GetSkeleton()->GetBone(szBoneName, &iBoneIdx);
		if (!pBone){
			a_LogOutput(1, "CECPlayerBodyController::CreateBoneController, Failed to get bone %s", szBoneName);
			return false;
		}		
		result = new A3DBoneBodyTurnController(pBone);
		pBone->SetFirstController(result);
		result->SetTurnDeg(0.0f);
	}
	return result;
}

void CECPlayerBodyController::ReleaseBoneControllers(){
	if (m_pSpine1TurnController){
		delete m_pSpine1TurnController;
		m_pSpine1TurnController = NULL;
	}
	if (m_pSpine2TurnController){
		delete m_pSpine2TurnController;
		m_pSpine2TurnController = NULL;
	}
}

void CECPlayerBodyController::SetTurnDegree(float degree){
	if (!m_pSpine1TurnController){
		return;
	}
	m_pSpine1TurnController->SetTurnDeg(0.4f*degree);
	m_pSpine2TurnController->SetTurnDeg(0.6f*degree);
}

bool CECPlayerBodyController::HasMovingDirection()const{
	return !m_vMoveDirection.IsZero();
}

CECObject * CECPlayerBodyController::GetFacingTarget()const{
	return m_facingTargetID != 0 && (m_pPlayer != NULL && m_facingTargetID != m_pPlayer->GetCharacterID())
		? g_pGame->GetGameRun()->GetWorld()->GetObject(m_facingTargetID, 0)
		: NULL;
}

bool CECPlayerBodyController::GetTargetDirection(A3DVECTOR3 &vTargetDir)const{
	if (CECObject *pFacingTarget = GetFacingTarget()){
		vTargetDir = pFacingTarget->GetPos() - m_pPlayer->GetPos();
		vTargetDir.y = 0.0f;
		vTargetDir.Normalize();
		return true;
	}
	return false;
}

void CECPlayerBodyController::Move(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp){
	if (!m_pPlayer){
		return;
	}
	SetMoveDirection(vDir);
	RecalculateTurnInfo();
}

void CECPlayerBodyController::MoveImmediatelyTo(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp){
	if (!m_pPlayer){
		return;
	}
	SetMoveDirection(vDir);
	SetModelTargetDirection(vDir);
	SetModelTargetUp(vUp);
	RecalculateTurnInfo();
	UpdateModelDirectionImmediately();
}

void CECPlayerBodyController::StopMove(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp){
	if (!m_pPlayer){
		return;
	}
	m_vMoveDirection.Clear();
	SetModelTargetDirection(vDir);
	SetModelTargetUp(vUp);
	RecalculateTurnInfo();
	UpdateModelDirectionImmediately();
}

void CECPlayerBodyController::StopMove(){
	if (!m_pPlayer){
		return;
	}
	m_vMoveDirection.Clear();
	RecalculateTurnInfo();
	UpdateModelDirectionImmediately();
}

void CECPlayerBodyController::ChangeMoveDirAndUp(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp){
	if (!m_pPlayer){
		return;
	}
	if (HasMovingDirection()){
		Move(vDir, vUp);
	}else{
		StopMove(vDir, vUp);
	}
}

void CECPlayerBodyController::ChangeModelTargetDirAndUp(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp){	
	if (!m_pPlayer){
		return;
	}
	if (m_pPlayer->IsHangerOn()){	//	相依相偎时，朝向由其它玩家控制
		return;
	}
	A3DVECTOR3 vTargetDirection;
	if (GetTargetDirection(vTargetDirection)){
		return;
	}
	SetModelTargetDirection(vDir);
	SetModelTargetUp(vUp);
}

A3DVECTOR3 CECPlayerBodyController::GetModelMoveDir()const{
	if (!m_pPlayer){
		return A3DVECTOR3(0.0f);
	}
	if (HasMovingDirection()){
		return m_vMoveDirection;
	}else{
		return m_pPlayer->GetDir();
	}
}

void CECPlayerBodyController::TurnFaceTo(int targetID){
	if (targetID == m_facingTargetID){
		return;
	}
	m_facingTargetID = targetID;
	RecalculateTurnInfo();
}

float CECPlayerBodyController::HorizontalAntiClockWiseAngleInRadian(const A3DVECTOR3 &v){
	A3DVECTOR3 va(v.x, 0.0f, v.z);
	if (va.z > 0.0f){
		va.Normalize();
		return acosf(va.x);
	}else if (va.z < 0.0f){
		va.Normalize();
		return A3D_2PI - acosf(va.x);
	}else if (va.x > 0.0f){
		return 0.0f;
	}else if (va.x < 0.0f){
		return A3D_PI;
	}else{
		return 0.0f;
	}
}

float CECPlayerBodyController::HorizontalClockWiseAngleInRadianBetween(const A3DVECTOR3 &vFrom, const A3DVECTOR3 &vTo){
	float result = HorizontalAntiClockWiseAngleInRadian(vFrom) - HorizontalAntiClockWiseAngleInRadian(vTo);
	if (result > A3D_PI){
		result -= A3D_2PI;
	}else if (result < -A3D_PI){
		result += A3D_2PI;
	}
	return result;
}

float CECPlayerBodyController::HorizontalClockWiseAngleInDegreeBetween(const A3DVECTOR3 &vFrom, const A3DVECTOR3 &vTo){
	return HorizontalClockWiseAngleInRadianBetween(vFrom, vTo) / A3D_PI * 180.0f;
}

void CECPlayerBodyController::SetMoveDirection(const A3DVECTOR3 &vDir){
	if (!m_pPlayer || vDir.IsZero()){
		return;
	}
	m_vMoveDirection = vDir;
	m_vMoveDirection.Normalize();
}

void CECPlayerBodyController::SetModelTargetDirection(const A3DVECTOR3 &vDir){
	if (!m_pPlayer || vDir.IsZero() || vDir == m_vModelTargetDirection){
		return;
	}
	m_vModelTargetDirection = vDir;
	if (!m_vModelTargetDirection.Normalize()){
		m_vModelTargetDirection = m_pPlayer->GetDir();
	}
}

void CECPlayerBodyController::SetModelTargetUp(const A3DVECTOR3 &vUp){
	if (!m_pPlayer || vUp.IsZero()){
		return;
	}
	m_vModelTargetUp = vUp;
	m_vModelTargetUp.Normalize();
}

void CECPlayerBodyController::RecalculateModelTargetDirection(){
	if (!m_pPlayer){
		return;
	}
	if (m_pPlayer->IsHangerOn()){	//	相依相偎时，朝向由其它玩家控制
		return;
	}
	A3DVECTOR3 vTargetDirection(0.0f);
	if (!GetTargetDirection(vTargetDirection)){			//	无目标
		m_facingTargetID = 0;								//	目标ID设置为0以免误操作
		return;
	}//	else											//	有目标
	A3DVECTOR3 newModelTargetDirection = m_vModelTargetDirection;
	if (HasMovingDirection()){							//	移动
		if (m_pPlayer->IsPlayingCastingSkillAndMoveActions()){	//	正在同时播放移动、技能动作
			if (DotProduct(m_vMoveDirection, vTargetDirection) < 0.0f){
				newModelTargetDirection = - m_vMoveDirection;		//	远离目标移动，以后退方式，模型朝向与移动朝向相反
			}else{
				newModelTargetDirection = m_vMoveDirection;			//	接近目标或保持距离，模型朝向与移动朝向一致
			}
		}else{													//	未同时播放移动、技能攻击动作
			newModelTargetDirection = m_vMoveDirection;				//	模型朝向与移动朝向一致
		}
	}else{												//	静止
		newModelTargetDirection = vTargetDirection;			//	朝向目标
	}
	newModelTargetDirection.y = 0.0f;
	newModelTargetDirection.Normalize();
	SetModelTargetDirection(newModelTargetDirection);
	SetModelTargetUp(MakeOrthogonalUp(m_vModelTargetDirection, m_vModelTargetUp));	//	移动攻击时需随时保证 m_vModelTargetDirection 与 m_vModelTargetUp 正交
}

void CECPlayerBodyController::RecalculateTargetTurningAngle(){
	if (!m_pPlayer){
		return;
	}
	if (m_pPlayer->IsHangerOn()){									//	相依相偎时，不应有转向
		m_targetTurningAngle = 0.0f;
		return;
	}
	A3DVECTOR3 vTargetDirection(0.0f);
	if (!GetTargetDirection(vTargetDirection)){
		m_targetTurningAngle = 0.0f;								//	无目标时，上身转向与模型朝向一致
	}else{
		if (HasMovingDirection()){
			if (m_pPlayer->IsPlayingCastingSkillAndMoveActions()){
				m_targetTurningAngle = HorizontalClockWiseAngleInDegreeBetween(m_vModelTargetDirection, vTargetDirection);	//	正在同时播放移动、技能动作，上身转向目标
			}else{
				m_targetTurningAngle = 0.0f;						//	未同时播放移动、技能攻击动作，上身转向与模型朝向一致
			}
		}else{
			m_targetTurningAngle = 0.0f;							//	静止时，上身转向与模型朝向一致
		}
	}
}

void CECPlayerBodyController::RecalculateTurnInfo(){
	RecalculateModelTargetDirection();
	RecalculateTargetTurningAngle();
}

void CECPlayerBodyController::UpdateModelDirection(DWORD dwDeltaTime){
	if (!m_pPlayer){
		return;
	}
	if (m_pPlayer->IsHangerOn()){					//	相依相偎时，朝向应由其它玩家控制
		return;
	}
	float horizontalAngleInDegree = HorizontalClockWiseAngleInDegreeBetween(m_pPlayer->GetDir(), m_vModelTargetDirection);
	if (fabs(horizontalAngleInDegree) <= 0.0001f){	//	不需要再改变朝向
		return;
	}
	if (fabs(horizontalAngleInDegree) >= 90.0f){	//	转角太大时，直接转向，避免大幅度转圈的不自然结果
		UpdateModelDirectionImmediately();
		return;
	}
	float turningRatio = fabsf(horizontalAngleInDegree) / 180.0f;
	DWORD dwTurningTime = static_cast<DWORD>(200 * turningRatio);
	m_pPlayer->SetDestDirAndUp(m_vModelTargetDirection, m_vModelTargetUp, dwTurningTime);
}

void CECPlayerBodyController::UpdateModelDirectionImmediately(){
	if (!m_pPlayer){
		return;
	}
	m_pPlayer->SetDirAndUp(m_vModelTargetDirection, m_vModelTargetUp);
}

void CECPlayerBodyController::UpdateTurningAngle(DWORD dwDeltaTime){
	if (m_currentTurningAngle != m_targetTurningAngle){
		if (m_currentTurningAngle < m_targetTurningAngle){
			float delta = 720.0f * dwDeltaTime * 0.001f;
			m_currentTurningAngle += delta;
			if (m_currentTurningAngle > m_targetTurningAngle){
				m_currentTurningAngle = m_targetTurningAngle;
			}
		}else{
			float delta = -720.0f * dwDeltaTime * 0.001f;
			m_currentTurningAngle += delta;
			if (m_currentTurningAngle < m_targetTurningAngle){
				m_currentTurningAngle = m_targetTurningAngle;
			}
		}
		SetTurnDegree(m_currentTurningAngle);
	}
	EnableBodyTurn(ShouldEnableBodyTurn());
}

bool CECPlayerBodyController::ShouldEnableBodyTurn()const{
	return fabs(m_currentTurningAngle) >= 0.001f || fabs(m_targetTurningAngle) >= 0.001f;
}

void CECPlayerBodyController::EnableBodyTurn(bool bEnable){
	if (m_pSpine1TurnController->IsEnabled() != bEnable){
		m_pSpine1TurnController->Enable(bEnable);
		m_pSpine2TurnController->Enable(bEnable);
	}
}

void CECPlayerBodyController::Tick(DWORD dwDeltaTime){
	if (!m_pPlayer){
		return;
	}
	RecalculateTurnInfo();
	UpdateModelDirection(dwDeltaTime);
	UpdateTurningAngle(dwDeltaTime);
}