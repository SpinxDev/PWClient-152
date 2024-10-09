
#ifndef __GNET_PLAYERBASEINFOCRC_RE_HPP
#define __GNET_PLAYERBASEINFOCRC_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class PlayerBaseInfoCRC_Re : public GNET::Protocol
{
	#include "playerbaseinfocrc_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
