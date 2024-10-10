
#ifndef __GNET_GETSAVEDMSG_RE_HPP
#define __GNET_GETSAVEDMSG_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "message"

namespace GNET
{

class GetSavedMsg_Re : public GNET::Protocol
{
	#include "getsavedmsg_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
