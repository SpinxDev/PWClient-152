
#ifndef __GNET_FACTIONOPREQUEST_RE_HPP
#define __GNET_FACTIONOPREQUEST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionOPRequest_Re : public GNET::Protocol
{
	#include "factionoprequest_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO


	}
};

};

#endif
