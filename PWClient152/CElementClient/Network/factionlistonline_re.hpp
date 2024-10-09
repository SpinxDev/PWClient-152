
#ifndef __GNET_FACTIONLISTONLINE_RE_HPP
#define __GNET_FACTIONLISTONLINE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionListOnline_Re : public GNET::Protocol
{
	#include "factionlistonline_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
