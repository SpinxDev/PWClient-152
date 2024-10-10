
#ifndef __GNET_PSHOPPLAYERGET_RE_HPP
#define __GNET_PSHOPPLAYERGET_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopbase"

namespace GNET
{

class PShopPlayerGet_Re : public GNET::Protocol
{
	#include "pshopplayerget_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
