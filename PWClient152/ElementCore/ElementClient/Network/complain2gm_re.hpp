
#ifndef __GNET_COMPLAIN2GM_RE_HPP
#define __GNET_COMPLAIN2GM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class Complain2GM_Re : public GNET::Protocol
{
	#include "complain2gm_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
