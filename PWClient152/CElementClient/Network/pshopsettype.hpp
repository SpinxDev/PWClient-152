
#ifndef __GNET_PSHOPSETTYPE_HPP
#define __GNET_PSHOPSETTYPE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopSetType : public GNET::Protocol
{
	#include "pshopsettype"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
