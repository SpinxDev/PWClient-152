
#ifndef __GNET_GMGETPLAYERCONSUMEINFO_HPP
#define __GNET_GMGETPLAYERCONSUMEINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GMGetPlayerConsumeInfo : public GNET::Protocol
{
	#include "gmgetplayerconsumeinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
