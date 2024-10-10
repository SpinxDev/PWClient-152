#ifndef __DEFENCE_PWACP
#define __DEFENCE_PWACP

#include <windows.h>

namespace PWACP
{

BOOL WINAPI ACPInit(VOID);		// 程序启动时调用，如果返回FASLE直接退出客户端
BOOL WINAPI ACPDestroy(VOID);   // 程序退出时调用
DWORD WINAPI ACPTalk(DWORD*);	// 暂时不用
};

#endif