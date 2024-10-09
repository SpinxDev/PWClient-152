
#ifndef __GNET_SSOGETTICKET_RE_HPP
#define __GNET_SSOGETTICKET_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SSOGetTicket_Re : public GNET::Protocol
{
	#include "ssogetticket_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
