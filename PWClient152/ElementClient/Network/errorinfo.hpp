
#ifndef __GNET_ERRORINFO_HPP
#define __GNET_ERRORINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ErrorInfo : public GNET::Protocol
{
	#include "errorinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
