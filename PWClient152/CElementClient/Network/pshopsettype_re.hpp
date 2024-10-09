
#ifndef __GNET_PSHOPSETTYPE_RE_HPP
#define __GNET_PSHOPSETTYPE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopSetType_Re : public GNET::Protocol
{
	#include "pshopsettype_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
