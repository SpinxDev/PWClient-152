
#ifndef __GNET_USERCOUPON_RE_HPP
#define __GNET_USERCOUPON_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class UserCoupon_Re : public GNET::Protocol
{
	#include "usercoupon_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
