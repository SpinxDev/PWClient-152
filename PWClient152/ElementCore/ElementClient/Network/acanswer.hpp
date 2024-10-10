
#ifndef __GNET_ACANSWER_HPP
#define __GNET_ACANSWER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ACAnswer : public GNET::Protocol
{
	#include "acanswer"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
