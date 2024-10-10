
#ifndef __GNET_PSHOPDRAWITEM_HPP
#define __GNET_PSHOPDRAWITEM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopDrawItem : public GNET::Protocol
{
	#include "pshopdrawitem"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
