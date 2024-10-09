
#ifndef __GNET_RESPONSE_HPP
#define __GNET_RESPONSE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "netclient.h"
#include "gnsecure.h"

namespace GNET
{

class Response : public GNET::Protocol
{
	#include "response"
	
};

};

#endif
