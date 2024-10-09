
#ifndef __GNET_PSHOPCANCELGOODS_RE_HPP
#define __GNET_PSHOPCANCELGOODS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "groleinventory"

namespace GNET
{

class PShopCancelGoods_Re : public GNET::Protocol
{
	#include "pshopcancelgoods_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
