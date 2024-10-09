
#ifndef __GNET_KEGETSTATUS_RE_HPP
#define __GNET_KEGETSTATUS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "keking"
#include "kecandidate"

namespace GNET
{

class KEGetStatus_Re : public GNET::Protocol
{
	#include "kegetstatus_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
