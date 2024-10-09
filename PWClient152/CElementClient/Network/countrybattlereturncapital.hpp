
#ifndef __GNET_COUNTRYBATTLERETURNCAPITAL_HPP
#define __GNET_COUNTRYBATTLERETURNCAPITAL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleReturnCapital : public GNET::Protocol
{
	#include "countrybattlereturncapital"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
