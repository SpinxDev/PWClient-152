
#ifndef __GNET_PLAYERPROFILEGETMATCHRESULT_RE_HPP
#define __GNET_PLAYERPROFILEGETMATCHRESULT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "profilematchresult"

namespace GNET
{

class PlayerProfileGetMatchResult_Re : public GNET::Protocol
{
	#include "playerprofilegetmatchresult_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
