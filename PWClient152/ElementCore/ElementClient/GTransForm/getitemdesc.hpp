
#ifndef __GNET_GETITEMDESC_HPP
#define __GNET_GETITEMDESC_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "../ElementHintProcess.h"
#include "getitemdesc_re.hpp"
#include "../EC_Global.h"

namespace GNET
{

class GetItemDesc : public GNET::Protocol
{
	#include "getitemdesc"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		GetItemDesc_Re re;

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
