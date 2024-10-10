// Filename	: EC_URLOSNavigator.cpp
// Creator	: Xu Wenbin
// Date		: 2014/9/26

#include "EC_URLOSNavigator.h"
#include "gnoctets.h"

using namespace GNET;
const Octets & CECURLOSNavigator::GetNavigateInOSSuffix(){
	const char *szSuffix = "_os";
	static Octets s_suffix((const void *)szSuffix, strlen(szSuffix));
	return s_suffix;
}

Octets CECURLOSNavigator::TrimNavigateInOSSuffix(const Octets &ticketContext){
	Octets result;
	if (HasNavigateInOSSuffix(ticketContext)){
		result = ticketContext;
		void *suffixBegin = (char *)result.begin()+(result.size() - GetNavigateInOSSuffix().size());
		void *suffixEnd = result.end();
		result.erase(suffixBegin, suffixEnd);
	}
	return result;
}

Octets CECURLOSNavigator::AppendNavigateInOSSuffix(const Octets &ticketContext){
	Octets result = ticketContext;
	result.insert(result.end(), GetNavigateInOSSuffix().begin(), GetNavigateInOSSuffix().end());
	return result;
}

bool CECURLOSNavigator::HasNavigateInOSSuffix(const Octets &ticketContext){
	bool result(false);
	if (ticketContext.size() >= GetNavigateInOSSuffix().size()){
		const void *suffixBegin = (const char *)ticketContext.begin()+(ticketContext.size() - GetNavigateInOSSuffix().size());
		const void *suffixEnd = ticketContext.end();
		if (Octets(suffixBegin, suffixEnd) == GetNavigateInOSSuffix()){
			result = true;
		}
	}
	return result;
}