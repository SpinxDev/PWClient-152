
#ifndef __GNET_ACQUESTION_HPP
#define __GNET_ACQUESTION_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ACQuestion : public GNET::Protocol
{
	#include "acquestion"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
