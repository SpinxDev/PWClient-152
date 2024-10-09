
#ifndef __GNET_STOCKCANCEL_HPP
#define __GNET_STOCKCANCEL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StockCancel : public GNET::Protocol
{
	#include "stockcancel"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
