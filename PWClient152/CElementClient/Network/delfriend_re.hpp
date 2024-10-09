
#ifndef __GNET_DELFRIEND_RE_HPP
#define __GNET_DELFRIEND_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class DelFriend_Re : public GNET::Protocol
{
	#include "delfriend_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
