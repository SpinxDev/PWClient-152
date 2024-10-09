
#ifndef __GNET_PSHOPSELL_RE_HPP
#define __GNET_PSHOPSELL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopiteminfo"
#include "pshopitem"

namespace GNET
{

class PShopSell_Re : public GNET::Protocol
{
	#include "pshopsell_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
