
#ifndef __GNET_UNDODELETEROLE_HPP
#define __GNET_UNDODELETEROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class UndoDeleteRole : public GNET::Protocol
{
	#include "undodeleterole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
