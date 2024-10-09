
#ifndef __GNET_CASHLOCK_HPP
#define __GNET_CASHLOCK_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class CashLock : public GNET::Protocol
{
	#include "cashlock"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
