
#ifndef __GNET_PRESERVEMAIL_HPP
#define __GNET_PRESERVEMAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class PreserveMail : public GNET::Protocol
{
	#include "preservemail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
