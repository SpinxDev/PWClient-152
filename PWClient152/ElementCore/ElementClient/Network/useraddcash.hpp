
#ifndef __GNET_USERADDCASH_HPP
#define __GNET_USERADDCASH_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class UserAddCash : public GNET::Protocol
{
	#include "useraddcash"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
