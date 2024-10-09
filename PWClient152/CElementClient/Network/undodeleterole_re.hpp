
#ifndef __GNET_UNDODELETEROLE_RE_HPP
#define __GNET_UNDODELETEROLE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class UndoDeleteRole_Re : public GNET::Protocol
{
	#include "undodeleterole_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
