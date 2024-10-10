
#ifndef __GNET_FACTIONRESOURCEBATTLEGETMAP_RE_HPP
#define __GNET_FACTIONRESOURCEBATTLEGETMAP_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionResourceBattleGetMap_Re : public GNET::Protocol
{
	#include "factionresourcebattlegetmap_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
