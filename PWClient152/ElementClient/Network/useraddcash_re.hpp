
#ifndef __GNET_USERADDCASH_RE_HPP
#define __GNET_USERADDCASH_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class UserAddCash_Re : public GNET::Protocol
{
	#include "useraddcash_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
