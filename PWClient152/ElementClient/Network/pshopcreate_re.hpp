
#ifndef __GNET_PSHOPCREATE_RE_HPP
#define __GNET_PSHOPCREATE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopCreate_Re : public GNET::Protocol
{
	#include "pshopcreate_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
