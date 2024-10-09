#pragma once

#include <vector.h>
#include <ABaseDef.h>
#include <A3DTypes.h>
#include "SingletonHolder.h"

#ifdef UNICODE
	#define GM_HELP_MSG_TYPE_BASE 0xfff0
#else
	#define GM_HELP_MSG_TYPE_BASE 0xf0
#endif

//	GM 收到的消息类型
enum GM_HELP_MSG_TYPE
{
	GM_HELP_MSG_TYPE_STUCK = GM_HELP_MSG_TYPE_BASE,				//	被卡住了
	GM_HELP_MSG_TYPE_PLUGIN = GM_HELP_MSG_TYPE_BASE + 1,	//	举报外挂
	GM_HELP_MSG_TYPE_TALK = GM_HELP_MSG_TYPE_BASE + 2,			//	给GM留言
	GM_HELP_MSG_TYPE_SPEAK = GM_HELP_MSG_TYPE_BASE + 3,			//	举报发言
};

struct GMHelpMsg
{
	ACHAR type;
	A3DCOLOR color;
	int msg;
};

typedef abase::vector<GMHelpMsg> CGMHelpMsgArrayBase;
class CGMHelpMsgArray : public CGMHelpMsgArrayBase
{
public:
	CGMHelpMsgArray();
	bool FindByType(ACHAR type, GMHelpMsg * pMsg) const;
};
typedef SingletonHolder<const CGMHelpMsgArray> TheGMHelpMsgArray;
