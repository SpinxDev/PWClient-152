
#ifndef __GNET_PSHOPMANAGEFUND_RE_HPP
#define __GNET_PSHOPMANAGEFUND_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopManageFund_Re : public GNET::Protocol
{
	#include "pshopmanagefund_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
