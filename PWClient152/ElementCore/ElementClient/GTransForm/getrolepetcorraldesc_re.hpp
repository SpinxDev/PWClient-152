
#ifndef __GNET_GETROLEPETCORRALDESC_RE_HPP
#define __GNET_GETROLEPETCORRALDESC_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GetRolePetCorralDesc_Re : public GNET::Protocol
{
	#include "getrolepetcorraldesc_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
