
#ifndef __GNET_GETROLEEQUIPMENTDESC_RE_HPP
#define __GNET_GETROLEEQUIPMENTDESC_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GetRoleEquipmentDesc_Re : public GNET::Protocol
{
	#include "getroleequipmentdesc_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
