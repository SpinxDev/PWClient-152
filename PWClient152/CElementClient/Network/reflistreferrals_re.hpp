
#ifndef __GNET_REFLISTREFERRALS_RE_HPP
#define __GNET_REFLISTREFERRALS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "referralbrief"

namespace GNET
{

class RefListReferrals_Re : public GNET::Protocol
{
	#include "reflistreferrals_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
