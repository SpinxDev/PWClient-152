
#ifndef __GNET_PLAYERPROFILEGETPROFILEDATA_HPP
#define __GNET_PLAYERPROFILEGETPROFILEDATA_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerProfileGetProfileData : public GNET::Protocol
{
	#include "playerprofilegetprofiledata"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
