
#ifndef __GNET_FACTIONRENAMEANNOUNCE_HPP
#define __GNET_FACTIONRENAMEANNOUNCE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionRenameAnnounce : public GNET::Protocol
{
	#include "factionrenameannounce"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
