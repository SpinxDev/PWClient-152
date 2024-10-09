
#ifndef __GNET_FACTIONLISTRELATION_RE_HPP
#define __GNET_FACTIONLISTRELATION_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gfactionalliance"
#include "gfactionhostile"
#include "gfactionrelationapply"

namespace GNET
{

class FactionListRelation_Re : public GNET::Protocol
{
	#include "factionlistrelation_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
