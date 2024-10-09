
#ifndef __GNET_FACTIONRELATIONRECVREPLY_HPP
#define __GNET_FACTIONRELATIONRECVREPLY_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionRelationRecvReply : public GNET::Protocol
{
	#include "factionrelationrecvreply"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
