
#ifndef __GNET_CASHPASSWORDSET_HPP
#define __GNET_CASHPASSWORDSET_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class CashPasswordSet : public GNET::Protocol
{
	#include "cashpasswordset"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
