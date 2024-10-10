
#ifndef __GNET_SETCUSTOMDATA_RE_HPP
#define __GNET_SETCUSTOMDATA_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetCustomData_Re : public GNET::Protocol
{
	#include "setcustomdata_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
#ifdef _TESTCODE
		if (result==ERR_SUCCESS)
			DEBUG_PRINT("Set custom data successfully. new CRC is %d\n",CRC);
		else
			DEBUG_PRINT("Set custom data failed. retcode=%d\n",result);
#endif
	}
};

};

#endif
