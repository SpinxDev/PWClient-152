
#ifndef __GNET_FACTIONAPPLYJOIN_RE_HPP
#define __GNET_FACTIONAPPLYJOIN_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
namespace GNET
{

class FactionApplyJoin_Re : public GNET::Protocol
{
	#include "factionapplyjoin_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO

	}
};

};

#endif
