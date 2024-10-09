
#ifndef __GNET_PSHOPMANAGEFUND_HPP
#define __GNET_PSHOPMANAGEFUND_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopManageFund : public GNET::Protocol
{
	#include "pshopmanagefund"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
