// Filename	: EC_GetHostByName.cpp
// Creator	: XuWenbin
// Date		: 2012/4/10

#include "EC_GetHostByName.h"
#include <process.h>

unsigned WINAPI CECGetHostByName::ThreadGetHostName(void * pParam)
{
	CECGetHostByName::ServerEntry * pEntry = (CECGetHostByName::ServerEntry *)pParam;

	while (pEntry && !pEntry->hostname.empty())
	{		
		struct hostent * addr = gethostbyname(pEntry->hostname.c_str());
		if (!addr)	break;
		char ** p;
		int n = 0;
		for(p=addr->h_addr_list;*p!=NULL;p++) 
			n++;
		if(n<=0)	break;
		n = rand() % n;
		pEntry->addr = *(struct in_addr *)(addr->h_addr_list[n]);
		pEntry->addr_valid = true;
		break;
	}

	return 0;
}

CECGetHostByName & CECGetHostByName::Instance()
{
	static CECGetHostByName s_dummy;
	return s_dummy;
}

std::string CECGetHostByName::Trim(const char *str)
{
	using std::string;

	string ret;

	while (str)
	{
		string temp(str);
		if (temp.empty())	break;
		
		//	��ȥͷβ�Ŀհ��ַ�
		const char *szTrimChars = " \t\n";
		string::size_type start = temp.find_first_not_of(szTrimChars);
		if (start == string::npos)	break;
		string::size_type end = temp.find_last_not_of(szTrimChars);

		ret = temp.substr(start, end-start+1);
		break;
	}

	return ret;	
}

void CECGetHostByName::Add(const char *szHostName)
{
	using std::string;

	//	��鲢Ԥ�������
	string strTrimmed = Trim(szHostName);
	if (strTrimmed.empty())	return;

	//	��ѯ�Ƿ��Ѽ�¼�ڰ�
	if (m_mServers.find(strTrimmed) != m_mServers.end())	return;

	//	��ӽ���ѯ����
	ServerEntry *pEntry = new ServerEntry(strTrimmed);
	m_mServers[strTrimmed] = ServerEntryPtr(pEntry);

	//	������������ѯ�߳�
	unsigned threadID = 0;
	pEntry->thdl = ::_beginthreadex(NULL, 0, ThreadGetHostName, pEntry, CREATE_SUSPENDED, &threadID);
	::ResumeThread((HANDLE) pEntry->thdl);
}

bool CECGetHostByName::Get(const char *szHostName, in_addr &addr)const
{
	bool bGet(false);

	//	��鲢Ԥ�������
	using std::string;
	string strTrimmed = Trim(szHostName);

	while (!strTrimmed.empty())
	{
		//	��ѯ���
		ServerEntryMap::const_iterator it = m_mServers.find(strTrimmed);
		if (it == m_mServers.end())	break;

		//	��鵱ǰ��ѯ״̬
		const ServerEntry *pEntry = it->second.p;
		if (pEntry->addr_valid)
		{
			addr = pEntry->addr;
			bGet = true;
		}

		break;
	}	

	return bGet;
}

void CECGetHostByName::Clear()
{
	for (ServerEntryMap::iterator it = m_mServers.begin(); it != m_mServers.end(); ++ it)
	{
		ServerEntryPtr &sp = it->second;
		ServerEntry *pEntry = sp.p;
		if (pEntry->thdl)
		{
			::WaitForSingleObject((HANDLE) pEntry->thdl, INFINITE);
			::CloseHandle((HANDLE)pEntry->thdl);
		}
		delete pEntry;
		sp.p = NULL;
	}
	m_mServers.clear();
}
