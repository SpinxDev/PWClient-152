
#ifndef __GNET_TANKBATTLEPLAYERAPPLY_RE_HPP
#define __GNET_TANKBATTLEPLAYERAPPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class TankBattlePlayerApply_Re : public GNET::Protocol
{
	#include "tankbattleplayerapply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
