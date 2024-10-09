
#ifndef __GNET_COUNTRYBATTLEGETPLAYERLOCATION_HPP
#define __GNET_COUNTRYBATTLEGETPLAYERLOCATION_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetPlayerLocation : public GNET::Protocol
{
	#include "countrybattlegetplayerlocation"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
