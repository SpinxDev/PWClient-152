
#ifndef __GNET_MATRIXCHALLENGE_HPP
#define __GNET_MATRIXCHALLENGE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class MatrixChallenge : public GNET::Protocol
{
	#include "matrixchallenge"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
