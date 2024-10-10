
#ifndef __GNET_KECANDIDATEAPPLY_HPP
#define __GNET_KECANDIDATEAPPLY_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class KECandidateApply : public GNET::Protocol
{
	#include "kecandidateapply"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
