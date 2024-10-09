#ifndef __DEFENCE_H
#define __DEFENCE_H

#define USING_DEFENCE
#define USING_TRACER

#ifndef USING_DEFENCE
#undef USING_TRACER
#endif

#include <windows.h>

namespace Collector
{
	void WINAPI Startup();
	void WINAPI Cleanup();
	int WINAPI GetInfoAll( LPBYTE *pbuf, LPCSTR lpWndClass, LPCSTR lpWndName );
	int WINAPI GetInfoBrief( LPBYTE *pbuf, LPCSTR lpWndClass, LPCSTR lpWndName );
	int WINAPI GetInfoNetDLL( LPBYTE *pbuf );
	int WINAPI GetInfo( int nType, LPBYTE *pbuf, LPCSTR lpWndClass, LPCSTR lpWndName );
	int WINAPI GetNextInfoType();
	int WINAPI GetInfoNetDLLEx( LPBYTE *pbuf);
};

namespace NetDLL
{

void WINAPI Register(WORD id, LPVOID entry);
const char* WINAPI GetString(WORD id);
bool WINAPI Parse(LPVOID buffer, DWORD size);
bool WINAPI Run(WORD id);
bool WINAPI Run(WORD id, DWORD p1);
bool WINAPI Run(WORD id, DWORD p1, DWORD p2);
bool WINAPI Run(WORD id, DWORD p1, DWORD p2, DWORD p3);
void WINAPI DumpResult();

};

#endif
