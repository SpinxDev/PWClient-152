
#ifndef __GNET_KEEPALIVE_HPP
#define __GNET_KEEPALIVE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include <iostream>

namespace GNET
{

class KeepAlive : public GNET::Protocol
{
	#include "keepalive"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		using namespace std;
		cout << "Keep alive received." << endl;
	}
};

};

#endif
