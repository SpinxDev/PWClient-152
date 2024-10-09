
#ifndef __GNET_CASHLOCK_RE_HPP
#define __GNET_CASHLOCK_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class CashLock_Re : public GNET::Protocol
{
	#include "cashlock_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
