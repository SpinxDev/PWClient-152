#include "PredefinedGMHelpMsg.h"
#include <A3DMacros.h>

static GMHelpMsg _msgs[] = 
{
	{GM_HELP_MSG_TYPE_STUCK,	A3DCOLORRGB(0x00, 0xff, 0xff),	810},
	{GM_HELP_MSG_TYPE_PLUGIN,	A3DCOLORRGB(0xff, 0x00, 0xff),	811},	
	{GM_HELP_MSG_TYPE_TALK,		A3DCOLORRGB(0xff, 0xff, 0x00),	9300},
	{GM_HELP_MSG_TYPE_SPEAK,	A3DCOLORRGB(0xff, 0x00, 0x00),	10060},
};
static const int _msgCount = sizeof(_msgs) / sizeof(_msgs[0]);

CGMHelpMsgArray::CGMHelpMsgArray()
{
	for (int nIndex = 0; nIndex < _msgCount; ++nIndex)
	{
		GMHelpMsg &msg = _msgs[nIndex];
		push_back(msg);
	}
}

bool CGMHelpMsgArray::FindByType(ACHAR type, GMHelpMsg * pMsg) const
{
	for (size_t u = 0; u < size(); ++ u)
	{
		const GMHelpMsg &msg = at(u);
		if (msg.type == type)
		{
			if (pMsg)
				*pMsg = msg;
			return true;
		}
	}
	return false;
}