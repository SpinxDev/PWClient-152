
#ifndef __GNET_GETROLEDESC_HPP
#define __GNET_GETROLEDESC_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "../ElementHintProcess.h"
#include "getroledesc_re.hpp"
#include "../EC_Global.h"


namespace GNET
{

class GetRoleDesc : public GNET::Protocol
{
	#include "getroledesc"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		GetRoleDesc_Re re;

		//	处理协议
		try
		{
			processOneDemand(*this, re);
		}
		catch (...)
		{			
			re.sn = this->sn;
			re.timestamp = this->timestamp;
			re.retcode = RC_UNKNOWN;
			Log_Info("Unknown exception occurred(retcode: %d)", re.retcode);
		}

		//	返回结果
		try
		{
			manager->Send(sid, &re);
		}
		catch (...)
		{
			Log_Info("Unknown exception occurred (retcode: -2)");
		}
	}
};

};

#endif
