
#ifndef __GNET_COUNTRYBATTLEGETSCORE_HPP
#define __GNET_COUNTRYBATTLEGETSCORE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetScore : public GNET::Protocol
{
	#include "countrybattlegetscore"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
