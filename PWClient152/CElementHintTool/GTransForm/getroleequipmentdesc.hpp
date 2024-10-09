
#ifndef __GNET_GETROLEEQUIPMENTDESC_HPP
#define __GNET_GETROLEEQUIPMENTDESC_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "../ElementHintProcess.h"
#include "getroleequipmentdesc_re.hpp"
#include "../EC_Global.h"


namespace GNET
{

class GetRoleEquipmentDesc : public GNET::Protocol
{
	#include "getroleequipmentdesc"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		GetRoleEquipmentDesc_Re re;

		//	处理协议
		try
		{
			processOneDemand(*this, re);
		}
		catch (...)
		{			
			re.id = this->id;
			re.zoneid = this->zoneid;
			re.retcode = RC_UNKNOWN;
			Log_Info("Unknown exception occurred(retcode: %d)", re.retcode);
			outputToFile("sns_equip", 1, this->id, this->detail);
		}

		//	返回结果
		try
		{
			manager->Send(sid, &re);
		}
		catch (...)
		{
			Log_Info("Unknown exception occurred (retcode: -2)");
			outputToFile("sns_equip", 2, this->id, this->detail);
		}
	}
};

};

#endif
