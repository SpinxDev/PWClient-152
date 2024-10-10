
#ifndef __GNET_FACTIONRESOURCEBATTLEGETRECORD_HPP
#define __GNET_FACTIONRESOURCEBATTLEGETRECORD_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionResourceBattleGetRecord : public GNET::Protocol
{
	#include "factionresourcebattlegetrecord"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
