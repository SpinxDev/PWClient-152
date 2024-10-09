
#ifndef __GNET_ACREPORT_HPP
#define __GNET_ACREPORT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ACReport : public GNET::Protocol
{
	#include "acreport"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
