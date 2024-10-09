
#ifndef __GNET_DELETEMAIL_HPP
#define __GNET_DELETEMAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class DeleteMail : public GNET::Protocol
{
	#include "deletemail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
