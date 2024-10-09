
#ifndef __GNET_PSHOPDRAWITEM_RE_HPP
#define __GNET_PSHOPDRAWITEM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopDrawItem_Re : public GNET::Protocol
{
	#include "pshopdrawitem_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
