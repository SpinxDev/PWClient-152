
#ifndef __GNET_REFLISTREFERRALS_HPP
#define __GNET_REFLISTREFERRALS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class RefListReferrals : public GNET::Protocol
{
	#include "reflistreferrals"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
