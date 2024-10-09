
#ifndef __GNET_ACCOUNTLOGINRECORD_HPP
#define __GNET_ACCOUNTLOGINRECORD_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class AccountLoginRecord : public GNET::Protocol
{
	#include "accountloginrecord"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
