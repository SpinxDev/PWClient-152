
#ifndef __GNET_ADDFRIEND_RE_HPP
#define __GNET_ADDFRIEND_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gfriendinfo"

namespace GNET
{

class AddFriend_Re : public GNET::Protocol
{
	#include "addfriend_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{

	}
};

};

#endif
