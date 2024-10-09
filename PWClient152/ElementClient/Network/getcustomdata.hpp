
#ifndef __GNET_GETCUSTOMDATA_HPP
#define __GNET_GETCUSTOMDATA_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetCustomData : public GNET::Protocol
{
	#include "getcustomdata"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
