
#ifndef __GNET_KEYREESTABLISH_HPP
#define __GNET_KEYREESTABLISH_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class KeyReestablish : public GNET::Protocol
{
	#include "keyreestablish"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
