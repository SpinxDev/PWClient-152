
#ifndef __GNET_TANKBATTLEPLAYERGETRANK_HPP
#define __GNET_TANKBATTLEPLAYERGETRANK_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class TankBattlePlayerGetRank : public GNET::Protocol
{
	#include "tankbattleplayergetrank"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
