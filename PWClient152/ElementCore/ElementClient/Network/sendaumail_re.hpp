
#ifndef __GNET_SENDAUMAIL_RE_HPP
#define __GNET_SENDAUMAIL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SendAUMail_Re : public GNET::Protocol
{
	#include "sendaumail_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
