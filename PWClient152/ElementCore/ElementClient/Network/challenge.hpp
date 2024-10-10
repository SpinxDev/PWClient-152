
#ifndef __GNET_CHALLENGE_HPP
#define __GNET_CHALLENGE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class Challenge : public GNET::Protocol
{
	#include "challenge"

	void Process(Manager *manager, Manager::Session::ID sid){}
};

};

#endif
