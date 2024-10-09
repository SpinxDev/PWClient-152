
#ifndef __GNET_MATRIXRESPONSE_HPP
#define __GNET_MATRIXRESPONSE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class MatrixResponse : public GNET::Protocol
{
	#include "matrixresponse"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
