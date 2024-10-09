
#ifndef __GNET_SETUICONFIG_RE_HPP
#define __GNET_SETUICONFIG_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetUIConfig_Re : public GNET::Protocol
{
	#include "setuiconfig_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
#ifdef _TESTCODE
		if (result==ERR_SUCCESS)
			DEBUG_PRINT("Set uiconfig successfully.");
		else
			DEBUG_PRINT("Set custom data failed. retcode=%d\n",result);
#endif

	}
};

};

#endif
