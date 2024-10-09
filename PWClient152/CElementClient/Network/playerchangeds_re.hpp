
#ifndef __GNET_PLAYERCHANGEDS_RE_HPP
#define __GNET_PLAYERCHANGEDS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerChangeDS_Re : public GNET::Protocol
{
	#include "playerchangeds_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
