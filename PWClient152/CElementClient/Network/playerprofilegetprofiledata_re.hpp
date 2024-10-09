
#ifndef __GNET_PLAYERPROFILEGETPROFILEDATA_RE_HPP
#define __GNET_PLAYERPROFILEGETPROFILEDATA_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "playerprofiledata"

namespace GNET
{

class PlayerProfileGetProfileData_Re : public GNET::Protocol
{
	#include "playerprofilegetprofiledata_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
