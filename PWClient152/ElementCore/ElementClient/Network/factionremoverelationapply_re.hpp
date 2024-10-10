
#ifndef __GNET_FACTIONREMOVERELATIONAPPLY_RE_HPP
#define __GNET_FACTIONREMOVERELATIONAPPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionRemoveRelationApply_Re : public GNET::Protocol
{
	#include "factionremoverelationapply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
