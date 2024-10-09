
#ifndef __GNET_PLAYERPROFILEGETMATCHRESULT_HPP
#define __GNET_PLAYERPROFILEGETMATCHRESULT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerProfileGetMatchResult : public GNET::Protocol
{
	#include "playerprofilegetmatchresult"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
