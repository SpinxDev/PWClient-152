
#ifndef __GNET_GETENCRYPTEDITEMDESC_HPP
#define __GNET_GETENCRYPTEDITEMDESC_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "../ElementHintProcess.h"
#include "getencrypteditemdesc_re.hpp"
#include "../EC_Global.h"

namespace GNET
{

class GetEncryptedItemDesc : public GNET::Protocol
{
	#include "getencrypteditemdesc"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		GetEncryptedItemDesc_Re re;

		//	处理协议
		try
		{
			processOneDemand(*this, re);
		}
		catch (...)
		{			
			re.tid = this->tid;
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
