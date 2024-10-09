#include <winsock2.h>
#include "gameclient.h"
#include "challenge.hpp"
#include "UsbHidKey.h"


//extern void VerifyDefenceThread();

namespace GNET
{
	GameClient GameClient::instance;
	const char *GameClient::GetLastError()
	{
		int n = strlen(errormsg);
		DWORD now = GetTickCount();
		if(n<ERRSIZE-20)
			_snprintf(errormsg+n, ERRSIZE-n, " %d:%d", now-m_lastsend,now-m_lastrecv);
		return errormsg;
	}

	void GameClient::OnRecvProtocol(Session::ID sid, Protocol* pdata) 
	{
		if(pdata->GetType()!=34)
		if(m_state==STATE_PINGING)
		{
			if(pdata->GetType()==PROTOCOL_CHALLENGE)
			{
				Challenge* p = (Challenge*)pdata;
				ServerStatus status;
				status.attr = *(Attr*)(p->nonce.begin());
				{
					DWORD last = pingmap[sid];
					Thread::Mutex::Scoped l(locker_ping);
					status.ping = (GetTickCount()-pingmap[sid]);
				}
				if(status.ping<=5)
					status.ping = 5;
				status.ping = status.ping>9999?9999:status.ping;
				status.createtime = *((unsigned int*)p->nonce.begin()+1);
				status.exp_rate = p->exp_rate;
				m_callback(m_pGameSession, &status, sid, EVENT_PINGRETURN);
				Close(sid);
				p->Destroy();
			}
			return;
		}
		if(sid==m_sid)
		{
			if(pdata->GetType()==PROTOCOL_KEEPALIVE)
			{
				m_lastrecv = GetTickCount();
				m_ping = m_lastrecv - m_lastsend;
				pdata->Destroy();
			}else{
				m_callback(m_pGameSession, pdata, sid, WM_IOPROTOCOL);
			}
		}
	} 

	void GameClient::Update()
	{
		if(m_state==STATE_KEEPING && (m_counter%15)==0){
			m_lastsend = GetTickCount();
			Send(m_sid, KeepAlive((unsigned int)PROTOCOL_KEEPALIVE));
		}
// 		if ((m_counter % 10) == 0)
// 			VerifyDefenceThread();
		m_counter++;
	}

	int GameClient::ConnectTo(struct in_addr *host, unsigned short port)
	{
		std::string hostaddr = inet_ntoa(*(struct in_addr *)(host));
		m_sid = Protocol::Client(this, hostaddr.c_str(), port);
		return m_sid;
	}
	int GameClient::ConnectTo(const char* hostname, unsigned short port)
	{
		if(!hostname)
		{
			m_sid = Protocol::Client(this);
			return m_sid;
		}

		struct hostent * addr = gethostbyname(hostname);
	
		if(!addr)
		{
			m_sid = Protocol::Client(this, hostname, port);
			return m_sid; 
		}
		
		char ** p;
		int n = 0;
		for(p=addr->h_addr_list;*p!=NULL;p++) 
			n++;

		if(n<=0)
			return 0;
		n = rand() % n;
		std::string hostaddr = inet_ntoa(*(struct in_addr *)(addr->h_addr_list[n]));
	
		m_sid = Protocol::Client(this, hostaddr.c_str(), port);
		return m_sid;
	}
	void GameClient::SetupResponse(Response& data, LoginParameter &para)
	{
		if (para.use_token)
		{
			if (para.use_token == 2)
			{
				data.response.replace(para.token, para.token.GetLength());
			}
			else
			{
				MD5Hash md5;
				md5.Update(Octets(para.token, para.token.GetLength()));
				md5.Update(para.nonce);
				md5.Final(data.response);
			}
		}
		else
		{
			if(!para.algo)
			{
				m_bIsUseUsbKey = HIDUSBKEY::TryDigestByUsbKey(para.passwd, (const BYTE*)(para.nonce.begin()), para.nonce.size(), (BYTE*)data.response.resize(16).begin());
				if(!m_bIsUseUsbKey)
				{
					HMAC_MD5Hash hash;
					MD5Hash md5;
					Octets digest;
					md5.Update(Octets(para.name, para.name.GetLength()));
					md5.Update(Octets(para.passwd, para.passwd.GetLength()));
					md5.Final(digest);
					hash.SetParameter(digest);
					hash.Update(para.nonce);
					hash.Final(data.response);
				}
			}
			else
			{
				SHA256Hash hash;
				Octets passdata(para.passwd, para.passwd.GetLength());
				hash.Update(passdata);
				hash.Final(data.response);
			}
		}
		data.identity.replace(para.name, para.name.GetLength());
		data.use_token = para.use_token;
		nonce = data.response;
	}
	bool GameClient::IsUsbKeyExisting()
	{
		return HIDUSBKEY::IsHasUsbKey();
	}

}

