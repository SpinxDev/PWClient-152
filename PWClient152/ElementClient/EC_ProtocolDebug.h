// Filename	: EC_ProtocolDebug.h
// Creator	: Xu Wenbin
// Date		: 2014/8/20

#ifndef _ELEMENTCLIENT_EC_PROTOCOLDEBUG_H_
#define _ELEMENTCLIENT_EC_PROTOCOLDEBUG_H_

#include "EC_Global.h"

#include <set>

namespace GNET{
	class Protocol;
}

class CECProtocolDebug{

	typedef std::set<int> IntSet;
	
	IntSet	m_ignoredClientProtocols;
	IntSet	m_ignoredClientCommands;
	
	IntSet	m_ignoredServerProtocols;
	IntSet	m_ignoredServerCommands;

	void IgnoreImpl(IntSet &s, int p, bool ignore);
	bool IsIgnoredImpl(const IntSet &s, int p)const;

	ELEMENTCLIENT_DECLARE_SINGLETON(CECProtocolDebug);

public:
	
	void IgnoreClientProtocol(int protocol, bool ignore);
	void IgnoreClientCommand(int command, bool ignore);

	void IgnoreServerCommand(int command, bool ignore);
	void IgnoreServerProtocol(int protocol, bool ignore);
	
	bool IsClientCommandIgnored(int command)const;
	bool IsClientProtocolIgnored(int protocol)const;

	bool IsServerCommandIgnored(int command)const;
	bool IsServerProtocolIgnored(int protocol)const;
	
	bool IsClientProtocolIgnored(const GNET::Protocol *p)const;
	bool IsServerProtocolIgnored(const GNET::Protocol *p)const;
};

#endif	//	_ELEMENTCLIENT_EC_PROTOCOLDEBUG_H_