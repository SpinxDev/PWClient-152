
#ifndef __GNET_DELETEMAIL_RE_HPP
#define __GNET_DELETEMAIL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class DeleteMail_Re : public GNET::Protocol
{
	#include "deletemail_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
