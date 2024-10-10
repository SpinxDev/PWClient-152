
#ifndef __GNET_PSHOPSELFGET_RE_HPP
#define __GNET_PSHOPSELFGET_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopdetail"

namespace GNET
{

class PShopSelfGet_Re : public GNET::Protocol
{
	#include "pshopselfget_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
