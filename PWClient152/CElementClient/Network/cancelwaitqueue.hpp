
#ifndef __GNET_CANCELWAITQUEUE_HPP
#define __GNET_CANCELWAITQUEUE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CancelWaitQueue : public GNET::Protocol
{
	#include "cancelwaitqueue"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
