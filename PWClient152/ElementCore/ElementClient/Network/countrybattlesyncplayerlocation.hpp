
#ifndef __GNET_COUNTRYBATTLESYNCPLAYERLOCATION_HPP
#define __GNET_COUNTRYBATTLESYNCPLAYERLOCATION_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleSyncPlayerLocation : public GNET::Protocol
{
	#include "countrybattlesyncplayerlocation"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
