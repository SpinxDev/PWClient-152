
#ifndef __GNET_FACTIONALLIANCEREPLY_RE_HPP
#define __GNET_FACTIONALLIANCEREPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionAllianceReply_Re : public GNET::Protocol
{
	#include "factionalliancereply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
