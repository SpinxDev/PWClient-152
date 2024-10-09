
#ifndef __GNET_COUNTRYBATTLEMOVE_HPP
#define __GNET_COUNTRYBATTLEMOVE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleMove : public GNET::Protocol
{
	#include "countrybattlemove"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
