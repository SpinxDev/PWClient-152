
#ifndef __GNET_PLAYERSENDMASSMAIL_HPP
#define __GNET_PLAYERSENDMASSMAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerSendMassMail : public GNET::Protocol
{
	#include "playersendmassmail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
