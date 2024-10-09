// Filename	: EC_PolicyAction.cpp
// Creator	: Xu Wenbin
// Date		: 2011/04/02

#include "EC_Global.h"
#include "EC_PolicyAction.h"
#include "EC_GPDataType.h"

CECPolicyAction::CECPolicyAction()
{
	Reset();
}

void CECPolicyAction::Reset()
{
	//	Э�����
	m_timeLength = 0;
	m_actionTime = 0;
	m_intervalTime = 0;
	m_playTimes = 0;
	m_actionName.Empty();
	m_validPolicy = false;
	
	//	״̬����
	m_inPolicy = false;
	m_inPlay = false;
	m_policyTimer = 0;
	m_playedTimes = 0;
	m_playTimer = 0;
	m_internalTimer = 0;
}

void CECPolicyAction::Init(const S2C::cmd_object_start_play_action *pCmd)
{
	//	�����������
	Reset();

	//	����Э������
	m_playTimes = pCmd->play_times;
	m_actionTime = pCmd->action_last_time;
	m_intervalTime = pCmd->interval_time;
	if (pCmd->name_length && pCmd->action_name[0])
		m_actionName = AString(pCmd->action_name, pCmd->name_length);

	//	��֤Э������
	ASSERT(m_actionTime >= 0);		//	�����Ŷ���ʱ��Ϊ0
	ASSERT(m_intervalTime >= 0);	//	���������ż��ʱ��Ϊ0
	ASSERT(m_actionTime > 0 || m_intervalTime > 0);	//	������������ʱ��Ͷ������ż��ͬʱΪ0
	ASSERT(m_actionTime == 0 || m_actionTime > 0 && !m_actionName.IsEmpty());	//	�������Ŷ���ʱ����������Ϊ��
	ASSERT(m_playTimes > 0);		//	�������Ŵ�������

	if (m_playTimes > 0 && m_actionTime >= 0 && m_intervalTime >= 0)
		m_timeLength = m_playTimes*m_actionTime+(m_playTimes-1)*m_intervalTime;

	//	��ʼ��״̬����
	m_validPolicy = true;
	if (m_actionTime < 0 || m_intervalTime < 0 || m_actionTime == 0 && m_intervalTime == 0 ||
		m_playTimes <= 0)
	{
		m_validPolicy = false;
	}

	m_inPolicy = (m_validPolicy && m_timeLength > 0);
	m_inPlay = (m_inPolicy && m_playTimes > 0 && m_actionTime > 0);
	m_policyTimer = 0;
	m_playedTimes = 0;
	m_playTimer = 0;
	m_internalTimer = 0;
}

void CECPolicyAction::Tick(DWORD dwDeltaTime)
{
	if (!IsInPolicy())
		return;

	if (IsInPlay())
	{
		//	֮ǰ���ڲ���״̬
		m_playTimer += dwDeltaTime;

		//	��鲥��������
		if (m_playTimer >= (DWORD)m_actionTime)
		{
			m_playedTimes += m_playTimer / (m_actionTime + m_intervalTime);
			m_playTimer %= (m_actionTime + m_intervalTime);

			if (m_playTimer >= (DWORD)m_actionTime)
			{
				//	�������״̬
				m_playedTimes ++;
				m_internalTimer = m_playTimer - m_actionTime;
				m_playTimer = 0;
				m_inPlay = false;
			}
			//	else ��������״̬��ʱ
		}
		//	else ��������״̬��ʱ
	}
	else
	{
		//	֮ǰ���ڿ���״̬
		m_internalTimer += dwDeltaTime;

		//	������������
		if (m_internalTimer >= (DWORD)m_intervalTime)
		{
			m_playedTimes += m_internalTimer / (m_intervalTime + m_actionTime);
			m_internalTimer %= (m_intervalTime + m_actionTime);

			if (m_internalTimer >= (DWORD)m_intervalTime)
			{
				//	���벥��״̬
				m_playTimer = m_internalTimer - m_intervalTime;
				m_internalTimer = 0;
				m_inPlay = true;
			}
			//	else ��������״̬��ʱ
		}
		//	else ��������״̬��ʱ
	}

	//	���²��������ʱ��
	m_policyTimer += dwDeltaTime;
	if (m_policyTimer >= (DWORD)m_timeLength)
	{
		//	����ִ�����
		m_inPolicy = false;
	}
}