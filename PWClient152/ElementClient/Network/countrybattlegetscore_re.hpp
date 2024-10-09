
#ifndef __GNET_COUNTRYBATTLEGETSCORE_RE_HPP
#define __GNET_COUNTRYBATTLEGETSCORE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetScore_Re : public GNET::Protocol
{
	#include "countrybattlegetscore_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
