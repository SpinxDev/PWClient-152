
#ifndef __GNET_FACTIONRESOURCEBATTLENOTIFYPLAYEREVENT_HPP
#define __GNET_FACTIONRESOURCEBATTLENOTIFYPLAYEREVENT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionResourceBattleNotifyPlayerEvent : public GNET::Protocol
{
	#include "factionresourcebattlenotifyplayerevent"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
