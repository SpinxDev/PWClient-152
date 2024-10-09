#ifndef __CHEATERKILLER_H
#define __CHEATERKILLER_H

#include <windows.h>

namespace CheaterKiller
{

void WINAPI AppendCheatInfo( LPVOID buf);	// 注册到APILoader 
void WINAPI TryKillCheaters();			// 反外挂线程每两秒调用一次

};

#endif
