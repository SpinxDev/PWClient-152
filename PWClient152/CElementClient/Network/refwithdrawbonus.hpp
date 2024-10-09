
#ifndef __GNET_REFWITHDRAWBONUS_HPP
#define __GNET_REFWITHDRAWBONUS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class RefWithdrawBonus : public GNET::Protocol
{
	#include "refwithdrawbonus"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
