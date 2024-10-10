
#ifndef __GNET_PSHOPCREATE_HPP
#define __GNET_PSHOPCREATE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopCreate : public GNET::Protocol
{
	#include "pshopcreate"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
