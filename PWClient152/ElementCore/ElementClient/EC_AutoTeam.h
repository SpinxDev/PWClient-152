// Filename	: EC_AutoTeam.h
// Creator	: Shizhenhua
// Date		: 2013/8/13

#pragma once

#include <ABaseDef.h>
#include <A3DVector.h>
#include "EC_Counter.h"

class CECGameUIMan;
class CECGameSession;
class CECHostPlayer;

namespace GNET
{
	class Protocol;
}

///////////////////////////////////////////////////////////////////////////
//  
//  class CECAutoTeam
//  
///////////////////////////////////////////////////////////////////////////

class CECAutoTeam
{
public:
	
	enum
	{
		TYPE_UNKNOWN,
		TYPE_TASK,
		TYPE_ACTIVITY,
	};

public:
	CECAutoTeam();

	void Tick(DWORD dwDeltaTime);

	bool DoAutoTeam(int type, int iGoalID);
	void Cancel(int type);

	void OnPrtcAutoTeamSetGoalRe(GNET::Protocol* p);
	void OnPrtcPlayerLeave(GNET::Protocol* p);

	void OnWorldChanged();

	int GetType() const { return m_iType; }
	bool IsMatchingForActivity() const { return m_iType == TYPE_ACTIVITY && m_iCurGoal != 0; }
	int GetCurGoal() const { return m_iCurGoal; }

protected:
	int m_iType;			// �Զ��������
	int m_iCurGoal;			// ����������ӣ�����ID������Ϊ�ID
	A3DVECTOR3 m_vTaskPos;	// �������ʱ����ҵ�λ��
	CECCounter m_cntCancel;

private:
	CECGameUIMan* GetGameUIMan();
	CECGameSession* GetGameSession();
	CECHostPlayer* GetHostPlayer();
};