
#ifndef __GNET_GETMAIL_HPP
#define __GNET_GETMAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetMail : public GNET::Protocol
{
	#include "getmail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
