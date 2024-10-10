// Filename	: EC_PlayerActionController.h
// Creator	: Xu Wenbin
// Date		: 2014/8/11

#ifndef _ELEMENTCLIENT_EC_PLAYERBODYCONTROLLER_H_
#define _ELEMENTCLIENT_EC_PLAYERBODYCONTROLLER_H_

#include <ABaseDef.h>
#include <A3DVector.h>

class CECPlayer;
class CECModel;
class A3DBoneBodyTurnController;
class CECObject;
class CECPlayerBodyController{
	CECPlayer	*				m_pPlayer;						//	角色指针（供动作相关其它状态查询）
	CECModel	*				m_pPlayerModel;					//	人物基本模型

	A3DBoneBodyTurnController*	m_pSpine1TurnController;		//	“移动施法”时上身骨骼转向控制器
	A3DBoneBodyTurnController*	m_pSpine2TurnController;		//	同上

	A3DVECTOR3					m_vMoveDirection;				//	角色位移朝向
	int							m_facingTargetID;				//	面向目标ID(NPC、玩家等）

	A3DVECTOR3					m_vModelTargetDirection;		//	模型目标朝向
	A3DVECTOR3					m_vModelTargetUp;				//	模型目标头部向上方向
	float						m_targetTurningAngle;			//	上身相对于下身的旋转角度目标值
	float						m_currentTurningAngle;			//	上身相对于下身的旋转角度当前值

private:
	bool  InitializeBoneControllers();
	void  ReleaseBoneControllers();
	bool  HasBone(const char *szBoneName)const;
	A3DBoneBodyTurnController * CreateBoneController(const char *szBoneName);
	void  SetTurnDegree(float degree);

	bool  HasMovingDirection()const;

	CECObject * GetFacingTarget()const;
	bool  GetTargetDirection(A3DVECTOR3 &vTargetDir)const;
	
	void  SetModelTargetDirection(const A3DVECTOR3 &vDir);
	void  SetModelTargetUp(const A3DVECTOR3 &vUp);
	void  SetMoveDirection(const A3DVECTOR3 &vDir);
	void  RecalculateModelTargetDirection();
	void  RecalculateTargetTurningAngle();
	void  RecalculateTurnInfo();
	void  UpdateModelDirection(DWORD dwDeltaTime);
	void  UpdateModelDirectionImmediately();
	void  UpdateTurningAngle(DWORD dwDeltaTime);
	bool  ShouldEnableBodyTurn()const;
	void  EnableBodyTurn(bool bEnable);

	static float HorizontalAntiClockWiseAngleInRadian(const A3DVECTOR3 &v);
	static float HorizontalClockWiseAngleInRadianBetween(const A3DVECTOR3 &vFrom, const A3DVECTOR3 &vTo);
	static float HorizontalClockWiseAngleInDegreeBetween(const A3DVECTOR3 &vFrom, const A3DVECTOR3 &vTo);

public:
	CECPlayerBodyController();
	~CECPlayerBodyController();

	bool Bind(CECPlayer *player, CECModel *playerModel);
	void Unbind();
	
	void Move(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp);
	void MoveImmediatelyTo(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp);
	void StopMove(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp);
	void StopMove();
	void ChangeMoveDirAndUp(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp);
	void ChangeModelTargetDirAndUp(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp);
	
	A3DVECTOR3 GetModelMoveDir()const;

	void TurnFaceTo(int targetID);

	void Tick(DWORD dwDeltaTime);
};

#endif	//	_ELEMENTCLIENT_EC_PLAYERBODYCONTROLLER_H_