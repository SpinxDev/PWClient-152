
#ifndef __GNET_SSOGETTICKET_HPP
#define __GNET_SSOGETTICKET_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "ssouser"

namespace GNET
{

class SSOGetTicket : public GNET::Protocol
{
	#include "ssogetticket"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
