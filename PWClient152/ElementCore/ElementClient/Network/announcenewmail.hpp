
#ifndef __GNET_ANNOUNCENEWMAIL_HPP
#define __GNET_ANNOUNCENEWMAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AnnounceNewMail : public GNET::Protocol
{
	#include "announcenewmail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
