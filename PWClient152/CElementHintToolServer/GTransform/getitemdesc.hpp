
#ifndef __GNET_GETITEMDESC_HPP
#define __GNET_GETITEMDESC_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "getitemdesc_re.hpp"

namespace GNET
{

class GetItemDesc : public GNET::Protocol
{
	#include "getitemdesc"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
