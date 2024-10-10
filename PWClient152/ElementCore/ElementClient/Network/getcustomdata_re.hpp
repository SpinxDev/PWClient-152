
#ifndef __GNET_GETCUSTOMDATA_RE_HPP
#define __GNET_GETCUSTOMDATA_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetCustomData_Re : public GNET::Protocol
{
	#include "getcustomdata_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
