
#ifndef __GNET_PRESERVEMAIL_RE_HPP
#define __GNET_PRESERVEMAIL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class PreserveMail_Re : public GNET::Protocol
{
	#include "preservemail_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
