
#ifndef __GNET_FACTIONREMOVERELATIONREPLY_RE_HPP
#define __GNET_FACTIONREMOVERELATIONREPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionRemoveRelationReply_Re : public GNET::Protocol
{
	#include "factionremoverelationreply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
