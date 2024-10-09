
#ifndef __GNET_REFGETREFERENCECODE_HPP
#define __GNET_REFGETREFERENCECODE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class RefGetReferenceCode : public GNET::Protocol
{
	#include "refgetreferencecode"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
