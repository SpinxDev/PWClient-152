// Filename	: EC_ArcAsia.h
// Creator	: Han Guanghui
// Date		: 2014/01/22

#pragma once

#include "ArcAsia_API.h"
#include <AAssist.h>

namespace GNET{
	class Octets;
	class SSOGetTicket_Re;
}

class CECArcAsia 
{
public:
	CECArcAsia();
	~CECArcAsia();

	static CECArcAsia& GetSingleton();

	bool IsArcInit(){ return m_hArcSDK != NULL; }
	void Init();
	void UnInit(); //��Ϸ�ͻ����˳�ʱ���á�ͨ��Arc����Ϸ���Զ���¼ʧ��ʱҲҪ���á�
	void GetToken(ACString& user, ACString& password, ACString& agent);
	bool NavigateURL(const ACString& url);
	
	enum {
		SSO_TOAID = 109,
	};
	bool CheckUser(const ACString& user); // ��鵱ǰ��¼���˺���Arc�˺��Ƿ�һ�£���һ�µĻ�ж��Arc
	GNET::Octets GetArcAsiaContext();
	bool OnSSOGetTicket_Re(const wchar_t* ticket);
protected:
	
	ARC_ASIA_SDK::CC_HANDLE m_hArcSDK;
	ACString m_strUser;
	ACString m_strUserChanging;
	DWORD	 m_dwGetTokenTime; // ��¼����CC_GetTokenEx��ʱ�䣬��ֹ���ε��ü�����̣������½ʧ��
};