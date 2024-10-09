
#ifndef __GNET_PLAYERASKFORPRESENT_RE_HPP
#define __GNET_PLAYERASKFORPRESENT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerAskForPresent_Re : public GNET::Protocol
{
	#include "playeraskforpresent_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
