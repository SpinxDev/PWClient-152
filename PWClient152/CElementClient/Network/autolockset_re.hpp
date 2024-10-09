
#ifndef __GNET_AUTOLOCKSET_RE_HPP
#define __GNET_AUTOLOCKSET_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class AutolockSet_Re : public GNET::Protocol
{
	#include "autolockset_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
