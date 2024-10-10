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
	CECPlayer	*				m_pPlayer;						//	��ɫָ�루�������������״̬��ѯ��
	CECModel	*				m_pPlayerModel;					//	�������ģ��

	A3DBoneBodyTurnController*	m_pSpine1TurnController;		//	���ƶ�ʩ����ʱ�������ת�������
	A3DBoneBodyTurnController*	m_pSpine2TurnController;		//	ͬ��

	A3DVECTOR3					m_vMoveDirection;				//	��ɫλ�Ƴ���
	int							m_facingTargetID;				//	����Ŀ��ID(NPC����ҵȣ�

	A3DVECTOR3					m_vModelTargetDirection;		//	ģ��Ŀ�곯��
	A3DVECTOR3					m_vModelTargetUp;				//	ģ��Ŀ��ͷ�����Ϸ���
	float						m_targetTurningAngle;			//	����������������ת�Ƕ�Ŀ��ֵ
	float						m_currentTurningAngle;			//	����������������ת�Ƕȵ�ǰֵ

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