
#ifndef __GNET_PLAYERACCUSE_RE_HPP
#define __GNET_PLAYERACCUSE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerAccuse_Re : public GNET::Protocol
{
	#include "playeraccuse_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
