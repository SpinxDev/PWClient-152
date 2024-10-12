
#ifndef __GNET_GETITEMDESC_RE_HPP
#define __GNET_GETITEMDESC_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GetItemDesc_Re : public GNET::Protocol
{
	#include "getitemdesc_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
