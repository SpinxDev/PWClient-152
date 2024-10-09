
#ifndef __GNET_PLAYERNAMEUPDATE_HPP
#define __GNET_PLAYERNAMEUPDATE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerNameUpdate : public GNET::Protocol
{
	#include "playernameupdate"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
