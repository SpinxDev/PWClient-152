
#ifndef __GNET_PSHOPCLEARGOODS_HPP
#define __GNET_PSHOPCLEARGOODS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopClearGoods : public GNET::Protocol
{
	#include "pshopcleargoods"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
