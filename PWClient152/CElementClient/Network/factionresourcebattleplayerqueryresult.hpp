
#ifndef __GNET_FACTIONRESOURCEBATTLEPLAYERQUERYRESULT_HPP
#define __GNET_FACTIONRESOURCEBATTLEPLAYERQUERYRESULT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionResourceBattlePlayerQueryResult : public GNET::Protocol
{
	#include "factionresourcebattleplayerqueryresult"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
