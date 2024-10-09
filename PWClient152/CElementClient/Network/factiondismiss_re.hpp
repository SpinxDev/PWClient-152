
#ifndef __GNET_FACTIONDISMISS_RE_HPP
#define __GNET_FACTIONDISMISS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionDismiss_Re : public GNET::Protocol
{
	#include "factiondismiss_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{

	}
};

};

#endif
