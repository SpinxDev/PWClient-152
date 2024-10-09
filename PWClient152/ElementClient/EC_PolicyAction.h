// Filename	: EC_PolicyAction.h
// Creator	: Xu Wenbin
// Date		: 2011/04/02

#pragma once

#include <AString.h>
#include <ABaseDef.h>

namespace S2C
{
	struct cmd_object_start_play_action;
}

//	���Զ���
class CECPolicyAction 
{
private:
	//	Э�����
	int			m_timeLength;			//	���Զ�������ʱ�䣨���룩=play_times*actionTime+(play_times-1)*interval_time
	int			m_actionTime;			//	�������˴����Ķ���������ʱ�䣨���룩
	int			m_intervalTime;		//	���Ŷ������ʱ�䣨���룩
	int			m_playTimes;			//	�����ܲ��Ŵ���
	AString		m_actionName;	//	ecm ģ���ж���������
	
	//	״̬����
	bool		m_validPolicy;		//	Э������Ƿ�Ϸ�
	bool		m_inPolicy;			//	���Զ����Ƿ�λ������״̬
	bool		m_inPlay;			//	Ϊ true ʱ��ʾ���ڲ��Ŷ�����Ϊ false ���ڶ������� interval��
	DWORD	m_policyTimer;	//	���Զ������嶨ʱ��
	int				m_playedTimes;	//	�Ѳ��Ŵ���
	DWORD	m_playTimer;			//	play ��ʱ��
	DWORD	m_internalTimer;		//	 interval ��ʱ��
	
public:
	CECPolicyAction();
	
	//	��ʼ��
	void	Init(const S2C::cmd_object_start_play_action *pCmd);
	void	Reset();
	
	//	��ȡִ��״̬
	bool	IsValid()const { return m_validPolicy; }
	bool	IsInPolicy()const { return m_inPolicy; }
	bool	IsInPlay()const { return IsInPolicy() && m_inPlay; }
	bool	IsInInterval()const { return IsInPolicy() && !m_inPlay; }
	
	//	��ȡЭ�����
	const AString &GetActionName()const { return m_actionName; }
	int			GetIntervalTime()const { return m_intervalTime; }

	//	��ѯ��������

	bool	HasAction()const
	{
		//	����������Ч��ʱ�䡢���Ŵ�������Ϊ0��
		return m_actionTime > 0 && m_playTimes > 0;
	}
	bool	HasInterval()const
	{
		//	���ʱ����Ч�������Ϊ0��
		return GetIntervalTime() > 0;
	}
	bool	HasNone()const
	{
		//	�������š����ʱ�����Ч
		return !HasAction() && !HasInterval();
	}
	bool	IsNormal()const
	{
		//	�������š����ʱ�����Ч
		return HasAction() && HasInterval();
	}
	
	//	����
	void	Tick(DWORD dwDeltaTime);
};
