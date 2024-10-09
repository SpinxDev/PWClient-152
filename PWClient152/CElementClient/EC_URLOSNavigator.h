// Filename	: EC_URLOSNavigator.h
// Creator	: Xu Wenbin
// Date		: 2014/9/26

#ifndef _ELEMENTCLIENT_EC_URLOSNAVIGATOR_H_
#define _ELEMENTCLIENT_EC_URLOSNAVIGATOR_H_

#include <AString.h>

namespace GNET{
	class Octets;
}

class CECURLOSNavigator{
public:
	virtual ~CECURLOSNavigator(){}
	virtual bool CanNavigateNow()const=0;
	virtual void OnClickNavigate()=0;
	virtual bool AutoRelease()const=0;
	static const GNET::Octets & GetNavigateInOSSuffix();
	static GNET::Octets TrimNavigateInOSSuffix(const GNET::Octets &ticketContext);
	static GNET::Octets AppendNavigateInOSSuffix(const GNET::Octets &ticketContext);
	static bool HasNavigateInOSSuffix(const GNET::Octets &ticketContext);
};

#endif	//	_ELEMENTCLIENT_EC_URLOSNAVIGATOR_H_