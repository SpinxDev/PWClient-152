
#ifndef __GNET_STOCKCANCEL_RE_HPP
#define __GNET_STOCKCANCEL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StockCancel_Re : public GNET::Protocol
{
	#include "stockcancel_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
