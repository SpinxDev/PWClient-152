// Filename	: DlgGuildDiplomacyMan.h
// Creator	: Xu Wenbin
// Date		: 2010/11/19

#pragma once

#include "EC_Counter.h"
#include "Network/rpcdata/gfactionrelationapply"
#include <vector.h>

namespace GNET
{
	class FactionListRelation_Re;
}

class CECGuildDiplomacyReminder
{
public:

	CECGuildDiplomacyReminder();

	void OnPrtcFactionListRelation_Re(GNET::FactionListRelation_Re *pProtocol);
	void Tick(DWORD dwDeltaTime);
	
private:

	CECCounter	m_counter;

	typedef abase::vector<GNET::GFactionRelationApply>	VApplyToMe;
	VApplyToMe	m_apply;
};