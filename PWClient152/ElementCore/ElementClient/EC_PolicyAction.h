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

//	策略动作
class CECPolicyAction 
{
private:
	//	协议变量
	int			m_timeLength;			//	策略动作持续时间（毫秒）=play_times*actionTime+(play_times-1)*interval_time
	int			m_actionTime;			//	服务器端传来的动作条持续时间（毫秒）
	int			m_intervalTime;		//	播放动作间隔时间（毫秒）
	int			m_playTimes;			//	动作总播放次数
	AString		m_actionName;	//	ecm 模型中动作条名称
	
	//	状态变量
	bool		m_validPolicy;		//	协议变量是否合法
	bool		m_inPolicy;			//	策略动作是否位于启用状态
	bool		m_inPlay;			//	为 true 时表示正在播放动作（为 false 处于动作播放 interval）
	DWORD	m_policyTimer;	//	策略动作总体定时器
	int				m_playedTimes;	//	已播放次数
	DWORD	m_playTimer;			//	play 定时器
	DWORD	m_internalTimer;		//	 interval 定时器
	
public:
	CECPolicyAction();
	
	//	初始化
	void	Init(const S2C::cmd_object_start_play_action *pCmd);
	void	Reset();
	
	//	获取执行状态
	bool	IsValid()const { return m_validPolicy; }
	bool	IsInPolicy()const { return m_inPolicy; }
	bool	IsInPlay()const { return IsInPolicy() && m_inPlay; }
	bool	IsInInterval()const { return IsInPolicy() && !m_inPlay; }
	
	//	获取协议变量
	const AString &GetActionName()const { return m_actionName; }
	int			GetIntervalTime()const { return m_intervalTime; }

	//	查询策略类型

	bool	HasAction()const
	{
		//	动作播放有效（时间、播放次数均不为0）
		return m_actionTime > 0 && m_playTimes > 0;
	}
	bool	HasInterval()const
	{
		//	间隔时间有效（间隔不为0）
		return GetIntervalTime() > 0;
	}
	bool	HasNone()const
	{
		//	动作播放、间隔时间均无效
		return !HasAction() && !HasInterval();
	}
	bool	IsNormal()const
	{
		//	动作播放、间隔时间均有效
		return HasAction() && HasInterval();
	}
	
	//	其它
	void	Tick(DWORD dwDeltaTime);
};
