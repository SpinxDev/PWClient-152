
#ifndef __GNET_COUNTRYBATTLERESULT_HPP
#define __GNET_COUNTRYBATTLERESULT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleResult : public GNET::Protocol
{
	#include "countrybattleresult"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
