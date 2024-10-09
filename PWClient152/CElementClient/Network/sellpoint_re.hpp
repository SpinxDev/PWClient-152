
#ifndef __GNET_SELLPOINT_RE_HPP
#define __GNET_SELLPOINT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "sellpointinfo"
namespace GNET
{

class SellPoint_Re : public GNET::Protocol
{
	#include "sellpoint_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO

	}
};

};

#endif
