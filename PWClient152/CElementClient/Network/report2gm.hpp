
#ifndef __GNET_REPORT2GM_HPP
#define __GNET_REPORT2GM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class Report2GM : public GNET::Protocol
{
	#include "report2gm"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
