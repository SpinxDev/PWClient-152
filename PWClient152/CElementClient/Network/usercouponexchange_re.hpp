
#ifndef __GNET_USERCOUPONEXCHANGE_RE_HPP
#define __GNET_USERCOUPONEXCHANGE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class UserCouponExchange_Re : public GNET::Protocol
{
	#include "usercouponexchange_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
