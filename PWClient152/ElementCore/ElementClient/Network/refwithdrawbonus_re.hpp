
#ifndef __GNET_REFWITHDRAWBONUS_RE_HPP
#define __GNET_REFWITHDRAWBONUS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class RefWithdrawBonus_Re : public GNET::Protocol
{
	#include "refwithdrawbonus_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
