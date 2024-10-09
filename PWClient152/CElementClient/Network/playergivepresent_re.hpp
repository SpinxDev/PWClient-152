
#ifndef __GNET_PLAYERGIVEPRESENT_RE_HPP
#define __GNET_PLAYERGIVEPRESENT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerGivePresent_Re : public GNET::Protocol
{
	#include "playergivepresent_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
