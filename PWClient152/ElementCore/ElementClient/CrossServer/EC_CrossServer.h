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

	//	��¼���Э��
	void OnPrtcChangeDS_Re(GNET::Protocol *pProtocol);
	void OnPrtcPlayerChangeDS_Re(GNET::Protocol *pProtocol);
	void OnPrtcChallenge();
	void OnPrtcOnlineAnnounce();
	
	//	��¼��������
	bool IsWaitLogin()const{ return m_bWaitLogin; }
	bool ApplyServerSetting();
	void SelectRole();
	void OnLogout();
	void OnLoginFail();
	void OnLoginSuccess();

	enum CrossType
	{
		CT_NONE,					//	��ת�����
		CT_GAME_TO_SPECIAL,			//	��Ϸ�����������
		CT_GAME_TO_ORIGINAL,		//	��Ϸ�������ԭ��
		CT_LOGIN_TO_SPECIAL,		//	ֱ�ӵ�¼�������
	};
	CrossType GetType()const;
	bool IsLoginToSpecial()const { return GetType() == CT_LOGIN_TO_SPECIAL; }
	bool IsGameToSpecial()const { return GetType() == CT_GAME_TO_SPECIAL; }
	bool IsGameToOriginal()const { return GetType() == CT_GAME_TO_ORIGINAL; }
	bool IsToSpecial()const { return IsLoginToSpecial() || IsGameToSpecial(); }

	//	��¼��״̬�ж�
	bool IsOnSpecialServer()const;

	//	ԭ����Ϣ
	void SetOrgServerIndex(int iServer){ m_iOrgServerIndex = iServer; }
	int	 GetOrgServerIndex()const{ return m_iOrgServerIndex; }
	int	 GetOrgServerLine()const;

	//	������Ϣ
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
	bool	m_bWaitLogin;								//	�Ƿ�ȴ�ת��
	bool	m_bOnSpecialServer;							//	�Ƿ��ڿ���������
	GNET::PlayerChangeDS_Re	 * m_pPlayerChangeDS_Re;	//	ת������

	int			m_iOrgServerIndex;	//	ԭ���ڷ������б��е��±꣨�̶�ֵ����ʼ��ʱ������
	ACString	m_strUser;			//	�˺���

	GNET::Octets m_OSecurity;	//	����Э����ܲ���
	GNET::Octets m_ISecurity;	//	��ȡЭ����ܲ���

	int		m_srcZoneID;		//	ת����� zoneid

	int		m_iSaveBackImage;	//	��������ʱ�ϴμ��ر���ͼ
	int		m_iSaveTipIndex;	//	��������ʱ�ϴ���ʾ�±�
	float	m_fSaveLoadPos;		//	��������ʱ�ϴμ��ؽ�����λ��
};