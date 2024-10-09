
#ifndef __GNET_GMSETTIMELESSAUTOLOCK_HPP
#define __GNET_GMSETTIMELESSAUTOLOCK_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GMSetTimelessAutoLock : public GNET::Protocol
{
	#include "gmsettimelessautolock"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
