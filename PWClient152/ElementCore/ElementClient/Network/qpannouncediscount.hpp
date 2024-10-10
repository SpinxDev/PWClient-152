
#ifndef __GNET_QPANNOUNCEDISCOUNT_HPP
#define __GNET_QPANNOUNCEDISCOUNT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "qpdiscountinfo"

namespace GNET
{

class QPAnnounceDiscount : public GNET::Protocol
{
	#include "qpannouncediscount"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
