
#ifndef __GNET_GETROLESKILLDESC_RE_HPP
#define __GNET_GETROLESKILLDESC_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GetRoleSkillDesc_Re : public GNET::Protocol
{
	#include "getroleskilldesc_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
