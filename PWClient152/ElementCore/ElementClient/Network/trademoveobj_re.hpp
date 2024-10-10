
#ifndef __GNET_TRADEMOVEOBJ_RE_HPP
#define __GNET_TRADEMOVEOBJ_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"

namespace GNET
{

class TradeMoveObj_Re : public GNET::Protocol
{
	#include "trademoveobj_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
