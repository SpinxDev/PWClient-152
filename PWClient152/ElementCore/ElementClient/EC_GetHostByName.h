// Filename	: EC_GetHostByName.h
// Creator	: XuWenbin
// Date		: 2012/4/10

#pragma once

#include <string>
#include <hashmap.h>
#include <winsock.h>

//	将字符类型的 hostname 通过多线程调用 gethostbyname 转换为 struct hostent 及 struct in_addr
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
		ServerEntry(const ServerEntry&);					//	未实现
		ServerEntry & operator=(const ServerEntry&);//	未实现
	};

	//	封装 ServerEntry 以便在 hash_map 中进行有意义的比较查找
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
	CECGetHostByName(const CECGetHostByName&);						//	未实现
	CECGetHostByName & operator=(const CECGetHostByName&);	 //	未实现

	static std::string	Trim(const char *str);
	static unsigned WINAPI ThreadGetHostName(void *pParam);
public:

	static CECGetHostByName & Instance();
	~CECGetHostByName(){ Clear(); }
	
	void Add(const char *szHostName);
	bool Get(const char *szHostName, in_addr &addr)const;
	void Clear();
};