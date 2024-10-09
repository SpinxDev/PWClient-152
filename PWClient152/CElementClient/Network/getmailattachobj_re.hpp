
#ifndef __GNET_GETMAILATTACHOBJ_RE_HPP
#define __GNET_GETMAILATTACHOBJ_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gmailsyncdata"

namespace GNET
{

class GetMailAttachObj_Re : public GNET::Protocol
{
	#include "getmailattachobj_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
