
#ifndef __GNET_KECANDIDATEAPPLY_RE_HPP
#define __GNET_KECANDIDATEAPPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class KECandidateApply_Re : public GNET::Protocol
{
	#include "kecandidateapply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
