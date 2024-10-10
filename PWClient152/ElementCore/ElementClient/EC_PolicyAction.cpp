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
	//	协议变量
	m_timeLength = 0;
	m_actionTime = 0;
	m_intervalTime = 0;
	m_playTimes = 0;
	m_actionName.Empty();
	m_validPolicy = false;
	
	//	状态变量
	m_inPolicy = false;
	m_inPlay = false;
	m_policyTimer = 0;
	m_playedTimes = 0;
	m_playTimer = 0;
	m_internalTimer = 0;
}

void CECPolicyAction::Init(const S2C::cmd_object_start_play_action *pCmd)
{
	//	清除已有数据
	Reset();

	//	设置协议数据
	m_playTimes = pCmd->play_times;
	m_actionTime = pCmd->action_last_time;
	m_intervalTime = pCmd->interval_time;
	if (pCmd->name_length && pCmd->action_name[0])
		m_actionName = AString(pCmd->action_name, pCmd->name_length);

	//	验证协议内容
	ASSERT(m_actionTime >= 0);		//	允许播放动作时间为0
	ASSERT(m_intervalTime >= 0);	//	允许动作播放间隔时间为0
	ASSERT(m_actionTime > 0 || m_intervalTime > 0);	//	不允许动作播放时间和动作播放间隔同时为0
	ASSERT(m_actionTime == 0 || m_actionTime > 0 && !m_actionName.IsEmpty());	//	不允许播放动作时但动作名称为空
	ASSERT(m_playTimes > 0);		//	不允许播放次数非正

	if (m_playTimes > 0 && m_actionTime >= 0 && m_intervalTime >= 0)
		m_timeLength = m_playTimes*m_actionTime+(m_playTimes-1)*m_intervalTime;

	//	初始化状态变量
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
		//	之前正在播放状态
		m_playTimer += dwDeltaTime;

		//	检查播放完成情况
		if (m_playTimer >= (DWORD)m_actionTime)
		{
			m_playedTimes += m_playTimer / (m_actionTime + m_intervalTime);
			m_playTimer %= (m_actionTime + m_intervalTime);

			if (m_playTimer >= (DWORD)m_actionTime)
			{
				//	进入空闲状态
				m_playedTimes ++;
				m_internalTimer = m_playTimer - m_actionTime;
				m_playTimer = 0;
				m_inPlay = false;
			}
			//	else 继续播放状态计时
		}
		//	else 继续播放状态计时
	}
	else
	{
		//	之前正在空闲状态
		m_internalTimer += dwDeltaTime;

		//	检查空闲完成情况
		if (m_internalTimer >= (DWORD)m_intervalTime)
		{
			m_playedTimes += m_internalTimer / (m_intervalTime + m_actionTime);
			m_internalTimer %= (m_intervalTime + m_actionTime);

			if (m_internalTimer >= (DWORD)m_intervalTime)
			{
				//	进入播放状态
				m_playTimer = m_internalTimer - m_intervalTime;
				m_internalTimer = 0;
				m_inPlay = true;
			}
			//	else 继续空闲状态计时
		}
		//	else 继续空闲状态计时
	}

	//	更新策略总体计时器
	m_policyTimer += dwDeltaTime;
	if (m_policyTimer >= (DWORD)m_timeLength)
	{
		//	策略执行完毕
		m_inPolicy = false;
	}
}