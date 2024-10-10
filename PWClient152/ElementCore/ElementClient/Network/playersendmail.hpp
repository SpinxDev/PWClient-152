
#ifndef __GNET_PLAYERSENDMAIL_HPP
#define __GNET_PLAYERSENDMAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gmailsyncdata"

namespace GNET
{

class PlayerSendMail : public GNET::Protocol
{
	#include "playersendmail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
