
#ifndef __GNET_FACTIONRESOURCEBATTLEGETMAP_HPP
#define __GNET_FACTIONRESOURCEBATTLEGETMAP_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionResourceBattleGetMap : public GNET::Protocol
{
	#include "factionresourcebattlegetmap"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
