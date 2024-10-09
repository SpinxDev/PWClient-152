
#ifndef __GNET_COMPLAIN2GM_HPP
#define __GNET_COMPLAIN2GM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class Complain2GM : public GNET::Protocol
{
	#include "complain2gm"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
