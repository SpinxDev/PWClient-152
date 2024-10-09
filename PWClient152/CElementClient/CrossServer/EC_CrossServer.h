// File		: EC_CrossServer.h
// Creator	: Xu Wenbin
// Date		: 2012/12/6

#pragma once

#include <AAssist.h>
#include "gnoctets.h"

namespace GNET
{
	class Protocol;
	class PlayerChangeDS_Re;
}

class CECCrossServer
{
	CECCrossServer();
	CECCrossServer(const CECCrossServer &);
	CECCrossServer & operator=(const CECCrossServer &);

public:

	~CECCrossServer();
	static CECCrossServer & Instance();

	//	登录相关协议
	void OnPrtcChangeDS_Re(GNET::Protocol *pProtocol);
	void OnPrtcPlayerChangeDS_Re(GNET::Protocol *pProtocol);
	void OnPrtcChallenge();
	void OnPrtcOnlineAnnounce();
	
	//	登录辅助函数
	bool IsWaitLogin()const{ return m_bWaitLogin; }
	bool ApplyServerSetting();
	void SelectRole();
	void OnLogout();
	void OnLoginFail();
	void OnLoginSuccess();

	enum CrossType
	{
		CT_NONE,					//	非转服标记
		CT_GAME_TO_SPECIAL,			//	游戏中请求到中央服
		CT_GAME_TO_ORIGINAL,		//	游戏中请求回原服
		CT_LOGIN_TO_SPECIAL,		//	直接登录到中央服
	};
	CrossType GetType()const;
	bool IsLoginToSpecial()const { return GetType() == CT_LOGIN_TO_SPECIAL; }
	bool IsGameToSpecial()const { return GetType() == CT_GAME_TO_SPECIAL; }
	bool IsGameToOriginal()const { return GetType() == CT_GAME_TO_ORIGINAL; }
	bool IsToSpecial()const { return IsLoginToSpecial() || IsGameToSpecial(); }

	//	登录后状态判断
	bool IsOnSpecialServer()const;

	//	原服信息
	void SetOrgServerIndex(int iServer){ m_iOrgServerIndex = iServer; }
	int	 GetOrgServerIndex()const{ return m_iOrgServerIndex; }
	int	 GetOrgServerLine()const;

	//	公共信息
	void SetUser(const ACString &user) { m_strUser = user; }
	const ACString & GetUser()const{ return m_strUser; }

	void SetOSecurity(const GNET::Octets &o) { m_OSecurity = o; }
	const GNET::Octets & GetOSecurity()const{ return m_OSecurity; }
	
	void SetISecurity(const GNET::Octets &o) { m_ISecurity = o; }
	const GNET::Octets & GetISecurity()const{ return m_ISecurity; }

	void SetSrcZoneID(int zoneid){ m_srcZoneID = zoneid; }
	int GetSrcZoneID()const{ return m_srcZoneID; }

private:
	void ShowError(const ACString &);

private:
	bool	m_bWaitLogin;								//	是否等待转服
	bool	m_bOnSpecialServer;							//	是否在跨服中央服上
	GNET::PlayerChangeDS_Re	 * m_pPlayerChangeDS_Re;	//	转服参数

	int			m_iOrgServerIndex;	//	原服在服务器列表中的下标（固定值，初始化时决定）
	ACString	m_strUser;			//	账号名

	GNET::Octets m_OSecurity;	//	发出协议加密参数
	GNET::Octets m_ISecurity;	//	收取协议加密参数

	int		m_srcZoneID;		//	转服起点 zoneid

	int		m_iSaveBackImage;	//	连续加载时上次加载背景图
	int		m_iSaveTipIndex;	//	连续加载时上次提示下标
	float	m_fSaveLoadPos;		//	连续加载时上次加载进度条位置
};