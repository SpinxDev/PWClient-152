
#ifndef __GNET_SYSAUCTIONCASHTRANSFER_RE_HPP
#define __GNET_SYSAUCTIONCASHTRANSFER_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SysAuctionCashTransfer_Re : public GNET::Protocol
{
	#include "sysauctioncashtransfer_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
