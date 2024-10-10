
#ifndef __GNET_QPADDCASH_RE_HPP
#define __GNET_QPADDCASH_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class QPAddCash_Re : public GNET::Protocol
{
	#include "qpaddcash_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
