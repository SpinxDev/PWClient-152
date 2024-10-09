
#ifndef __GNET_COUNTRYBATTLEMOVE_RE_HPP
#define __GNET_COUNTRYBATTLEMOVE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleMove_Re : public GNET::Protocol
{
	#include "countrybattlemove_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
