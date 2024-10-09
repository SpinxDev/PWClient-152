
#ifndef __GNET_GMLISTONLINEUSER_RE_HPP
#define __GNET_GMLISTONLINEUSER_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gmplayerinfo"

namespace GNET
{

class GMListOnlineUser_Re : public GNET::Protocol
{
	#include "gmlistonlineuser_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
