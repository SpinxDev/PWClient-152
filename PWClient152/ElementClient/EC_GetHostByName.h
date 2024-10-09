// Filename	: EC_GetHostByName.h
// Creator	: XuWenbin
// Date		: 2012/4/10

#pragma once

#include <string>
#include <hashmap.h>
#include <winsock.h>

//	���ַ����͵� hostname ͨ�����̵߳��� gethostbyname ת��Ϊ struct hostent �� struct in_addr
class CECGetHostByName
{
	struct ServerEntry
	{
		std::string	hostname;
		in_addr		addr;
		bool		addr_valid;
		unsigned long thdl;

		ServerEntry() : addr_valid(false), thdl(0) {}
		ServerEntry(const std::string &rhs)
			: hostname(rhs)
			, addr_valid(false)
			, thdl(0)
		{}

	private:
		ServerEntry(const ServerEntry&);					//	δʵ��
		ServerEntry & operator=(const ServerEntry&);//	δʵ��
	};

	//	��װ ServerEntry �Ա��� hash_map �н���������ıȽϲ���
	struct ServerEntryPtr
	{
		ServerEntry *p;

		ServerEntryPtr() : p(NULL) {}
		ServerEntryPtr(ServerEntry *rhs) : p(rhs){}

		bool operator==(const ServerEntryPtr &rhs) const
		{
			return (!p && !rhs.p) || (p && rhs.p && p->hostname==rhs.p->hostname);
		}
	};

	struct ServerEntryHashFunc : public abase::_hash_function
	{
		unsigned long operator()(const std::string &rhs)const
		{
			return rhs.empty() ? 0 : _hash_function::operator()(rhs.c_str());
		}
	};

	typedef abase::hash_map<std::string, ServerEntryPtr, ServerEntryHashFunc> ServerEntryMap;
	
	ServerEntryMap			m_mServers;

	CECGetHostByName(){}
	CECGetHostByName(const CECGetHostByName&);						//	δʵ��
	CECGetHostByName & operator=(const CECGetHostByName&);	 //	δʵ��

	static std::string	Trim(const char *str);
	static unsigned WINAPI ThreadGetHostName(void *pParam);
public:

	static CECGetHostByName & Instance();
	~CECGetHostByName(){ Clear(); }
	
	void Add(const char *szHostName);
	bool Get(const char *szHostName, in_addr &addr)const;
	void Clear();
};