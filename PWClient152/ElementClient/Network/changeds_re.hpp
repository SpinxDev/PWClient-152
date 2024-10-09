
#ifndef __GNET_CHANGEDS_RE_HPP
#define __GNET_CHANGEDS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class ChangeDS_Re : public GNET::Protocol
{
	#include "changeds_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
