
#ifndef __GNET_QQADDFRIEND_RE_HPP
#define __GNET_QQADDFRIEND_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class QQAddFriend_Re : public GNET::Protocol
{
	#include "qqaddfriend_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
