
#ifndef __GNET_QPGETACTIVATEDSERVICES_RE_HPP
#define __GNET_QPGETACTIVATEDSERVICES_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class QPGetActivatedServices_Re : public GNET::Protocol
{
	#include "qpgetactivatedservices_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
