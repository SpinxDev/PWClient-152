
#ifndef __GNET_PSHOPLISTITEM_RE_HPP
#define __GNET_PSHOPLISTITEM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopitementry"

namespace GNET
{

class PShopListItem_Re : public GNET::Protocol
{
	#include "pshoplistitem_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
