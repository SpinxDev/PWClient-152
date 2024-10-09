
#ifndef __GNET_GMSETTIMELESSAUTOLOCK_RE_HPP
#define __GNET_GMSETTIMELESSAUTOLOCK_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GMSetTimelessAutoLock_Re : public GNET::Protocol
{
	#include "gmsettimelessautolock_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
