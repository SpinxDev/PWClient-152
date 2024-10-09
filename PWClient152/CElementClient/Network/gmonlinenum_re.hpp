
#ifndef __GNET_GMONLINENUM_RE_HPP
#define __GNET_GMONLINENUM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMOnlineNum_Re : public GNET::Protocol
{
	#include "gmonlinenum_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{

	}
};

};

#endif
