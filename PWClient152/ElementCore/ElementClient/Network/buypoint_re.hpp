
#ifndef __GNET_BUYPOINT_RE_HPP
#define __GNET_BUYPOINT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BuyPoint_Re : public GNET::Protocol
{
	#include "buypoint_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
