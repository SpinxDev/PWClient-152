
#ifndef __GNET_GETSAVEDMSG_HPP
#define __GNET_GETSAVEDMSG_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetSavedMsg : public GNET::Protocol
{
	#include "getsavedmsg"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
