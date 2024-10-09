
#ifndef __GNET_GMGETPLAYERCONSUMEINFO_RE_HPP
#define __GNET_GMGETPLAYERCONSUMEINFO_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "playerconsumeinfo"

namespace GNET
{

class GMGetPlayerConsumeInfo_Re : public GNET::Protocol
{
	#include "gmgetplayerconsumeinfo_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
