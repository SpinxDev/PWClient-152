
#ifndef __GNET_SELLCANCEL_RE_HPP
#define __GNET_SELLCANCEL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SellCancel_Re : public GNET::Protocol
{
	#include "sellcancel_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO

	}
};

};

#endif
