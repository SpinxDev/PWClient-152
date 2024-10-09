// Filename	: EC_ProtocolDebug.cpp
// Creator	: Xu Wenbin
// Date		: 2014/8/20

#include "EC_ProtocolDebug.h"
#include "EC_GPDataType.h"
#include "gnproto.h"
#include "gamedatasend.hpp"

ELEMENTCLIENT_DEFINE_SINGLETON(CECProtocolDebug);


void CECProtocolDebug::IgnoreImpl(IntSet &s, int p, bool ignore){
	if (ignore){
		s.insert(p);
	}else{
		s.erase(p);
	}
}

bool CECProtocolDebug::IsIgnoredImpl(const IntSet &s, int p)const{
	return s.find(p) != s.end();
}

void CECProtocolDebug::IgnoreClientProtocol(int protocol, bool ignore){
	IgnoreImpl(m_ignoredClientProtocols, protocol, ignore);
}
void CECProtocolDebug::IgnoreClientCommand(int command, bool ignore){
	IgnoreImpl(m_ignoredClientCommands, command, ignore);
}
void CECProtocolDebug::IgnoreServerProtocol(int protocol, bool ignore){
	IgnoreImpl(m_ignoredServerProtocols, protocol, ignore);
}
void CECProtocolDebug::IgnoreServerCommand(int command, bool ignore){
	IgnoreImpl(m_ignoredServerCommands, command, ignore);
}

bool CECProtocolDebug::IsClientProtocolIgnored(int protocol)const{
	return IsIgnoredImpl(m_ignoredClientProtocols, protocol);
}
bool CECProtocolDebug::IsClientCommandIgnored(int command)const{
	return IsIgnoredImpl(m_ignoredClientCommands, command);
}
bool CECProtocolDebug::IsServerProtocolIgnored(int protocol)const{
	return IsIgnoredImpl(m_ignoredServerProtocols, protocol);
}
bool CECProtocolDebug::IsServerCommandIgnored(int command)const{
	return IsIgnoredImpl(m_ignoredServerCommands, command);
}

bool CECProtocolDebug::IsClientProtocolIgnored(const GNET::Protocol *p)const{
	if (IsClientProtocolIgnored(p->GetType())){
		return true;
	}else if (p->GetType() == PROTOCOL_GAMEDATASEND){
		const C2S::cmd_header *pCmd = (const C2S::cmd_header *)((const GNET::GamedataSend *)p)->data.begin();
		if (IsClientCommandIgnored(pCmd->cmd)){
			return true;
		}
	}
	return false;
}

bool CECProtocolDebug::IsServerProtocolIgnored(const GNET::Protocol *p)const{
	if (IsServerProtocolIgnored(p->GetType())){
		return true;
	}else if (p->GetType() == PROTOCOL_GAMEDATASEND){
		const S2C::cmd_header *pCmd = (const S2C::cmd_header *)((const GNET::GamedataSend *)p)->data.begin();
		if (IsServerCommandIgnored(pCmd->cmd)){
			return true;
		}
	}
	return false;
}