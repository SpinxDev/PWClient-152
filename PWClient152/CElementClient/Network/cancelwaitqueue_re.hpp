
#ifndef __GNET_CANCELWAITQUEUE_RE_HPP
#define __GNET_CANCELWAITQUEUE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CancelWaitQueue_Re : public GNET::Protocol
{
	#include "cancelwaitqueue_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
