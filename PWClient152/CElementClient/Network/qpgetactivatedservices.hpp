
#ifndef __GNET_QPGETACTIVATEDSERVICES_HPP
#define __GNET_QPGETACTIVATEDSERVICES_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class QPGetActivatedServices : public GNET::Protocol
{
	#include "qpgetactivatedservices"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
