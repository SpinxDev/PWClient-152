
#ifndef __GNET_FACTIONALLIANCEAPPLY_RE_HPP
#define __GNET_FACTIONALLIANCEAPPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionAllianceApply_Re : public GNET::Protocol
{
	#include "factionallianceapply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
