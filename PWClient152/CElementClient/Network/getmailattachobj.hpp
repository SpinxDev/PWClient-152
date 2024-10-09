
#ifndef __GNET_GETMAILATTACHOBJ_HPP
#define __GNET_GETMAILATTACHOBJ_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gmailsyncdata"

namespace GNET
{

class GetMailAttachObj : public GNET::Protocol
{
	#include "getmailattachobj"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
