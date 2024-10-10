
#ifndef __GNET_REPORTCHAT_HPP
#define __GNET_REPORTCHAT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class ReportChat : public GNET::Protocol
{
	#include "reportchat"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
