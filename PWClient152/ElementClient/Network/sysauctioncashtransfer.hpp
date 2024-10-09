
#ifndef __GNET_SYSAUCTIONCASHTRANSFER_HPP
#define __GNET_SYSAUCTIONCASHTRANSFER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gmailsyncdata"

namespace GNET
{

class SysAuctionCashTransfer : public GNET::Protocol
{
	#include "sysauctioncashtransfer"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
