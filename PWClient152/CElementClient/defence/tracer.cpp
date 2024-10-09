//#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <winsock.h>
#include <math.h>
#include <time.h>
#include <map>
#include <stdio.h>
#include "infocollection.h"
#include "stackinfocollection.h"
#include "replaceapi.h"
#include "defence.h"
//#include "log.h"

#define TABLESIZE 256
static DWORD table[TABLESIZE];
static DWORD init_tracer();



#ifdef USING_TRACER
	static DWORD rva = init_tracer();
#else
	static DWORD rva = 0;
#endif

#define PATTERNCOUNT 256

inline DWORD fold(DWORD x)
{
	return table[((x >> 24) ^ (x >> 16) ^ (x >> 8) ^ x) & 0xFF];
}

#ifndef BRIEF
class StackInfo
{
public:
	DWORD	caller;
	DWORD	count;
	DWORD	size;
	DWORD	pattern;
};
class TraceInfo
{
public:
	//DWORD	tid;
	//DWORD	frame_ptr;
	//DWORD	code_ptr;
	DWORD	count;
	//BYTE	__dummy[24];
	StackInfo si[PATTERNCOUNT];
};

__declspec(thread) DWORD lo = 0;
__declspec(thread) DWORD hi = 0;
__declspec(thread) TraceInfo ti;

void stack_range()
{
	//DWORD vesp = NULL;
	//__asm mov vesp, esp;
	if ( lo && hi ) return;
	ti.count = 0;
	DWORD vesp = (DWORD)&vesp & 0xfffff000;
	while( ! IsBadWritePtr((void*)vesp, 4096) ) vesp+=4096;
	hi = vesp;
	do {
		vesp -= 4096;
		IsBadWritePtr((void*)vesp, 4096);
	} while (! IsBadWritePtr((void*)vesp, 4096));
	lo = vesp+4096;
}
void stack_trace(LPCVOID sv0, LPCVOID sv1)
{
	stack_range();

	DWORD frame_ptr = hi;
	DWORD frame_cur, caller, pattern;
	__asm
	{
		mov frame_cur, ebp
	}
	if( frame_cur >= hi || frame_cur <lo ) return;

	if ( !IsBadReadPtr((LPVOID)(4+frame_cur),4) && !IsBadReadPtr( (LPVOID)(-4+*(DWORD*)(frame_cur+4)),8 ) )
	{
		__asm {
			mov eax, [ebp+4]
			mov caller, eax
			mov	eax, [eax-4]
			mov pattern, eax
		}
	}
	else
	{
		//Log::log("frame_cur %p is unreadable.\n",frame_cur);
		//::MessageBoxA(0,"frame_cur is unreadable.\n","err",0);
		return;
	}
	//Log::log("frame_cur %p is readable and is successfully read.\n",frame_cur);
	caller -= rva;
	pattern = caller;
	DWORD tc = 0;
	DWORD size = (frame_ptr - frame_cur) >> 2;

	if ( sv0 ) { pattern ^= ((frame_ptr - (DWORD)sv0) >> 2) * 3145739 ^ table[tc++]; }
	DWORD a = 0x67452301, b = 0xefcdab89, c = 0x98badcfe, d = 0x10325476;
	DWORD fc = fold(caller);
	DWORD fs = fold(size);
	while( frame_cur < frame_ptr )
	{
		DWORD fh = fold((frame_ptr - frame_cur) >> 2);
		DWORD ft = fold(tc++);
		DWORD e = (( ~a +  fh ) | ( ~b -  ft )) & ( (~c &  fc ) + ( ~d |  fs ) );
		DWORD f = ((  b - ~fh ) & (  c + ~ft )) + ( ( d | ~fc ) - (  a & ~fs ) );
		DWORD g = (( ~c +  fh ) | ( ~d -  ft )) | ( (~a &  fc ) + ( ~b |  fs ) );
		DWORD h = ((  d - ~fh ) & (  a + ~ft )) - ( ( b | ~fc ) - (  c & ~fs ) );
		DWORD r = tc & 0x1F;
		a = ( h >> (r   ) ) | ( h << (32-(r   )) );
		b = ( e >> (r+ 8) ) | ( e << (32-(r+ 8)) );
		c = ( f >> (r+16) ) | ( f << (32-(r+16)) );
		d = ( g >> (r+24) ) | ( g << (32-(r+24)) );
		pattern ^= a ^ b ^ c ^ d;
		DWORD next_ptr = *(LPDWORD)frame_cur;
		if( tc > 128 || next_ptr <= frame_cur )
		//	|| IsBadReadPtr((LPVOID)next_ptr,4) )
			break;
		frame_cur = next_ptr;
	}

	if ( sv1 ) { pattern ^= ((frame_ptr - (DWORD)sv1) >> 2) * 786433 ^ table[tc%TABLESIZE]; }
	StackInfoCollection &sic = StackInfoCollection::GetInstance();

	if ( sic.TryLock() )
	{
		sic.Append( caller, 1, size, pattern );
		for ( DWORD i = 0, c = ti.count; i < c; i++ )
		{
			StackInfo& si = ti.si[i];
			sic.Append( si.caller, si.count, si.size, si.pattern );
		}
		ti.count = 0;
		sic.Unlock();
	}
	else
	{
		DWORD i, c = ti.count;
		for( i = 0; i < c; i++ )
		{
			StackInfo& si = ti.si[i];
			if ( si.caller == caller && si.size == size && si.pattern == pattern )
			{
				si.count ++;
				break;
			}
		}
		if ( i == c && c < PATTERNCOUNT )
		{
			StackInfo& si = ti.si[c];
			si.caller     = caller;
			si.count      = 1;
			si.size       = size;
			si.pattern    = pattern;
			ti.count     = c + 1;
		}
	}
}

#else

class StackInfo
{
public:
	DWORD	count;
	DWORD	pattern;
};
class TraceInfo
{
public:
	//DWORD	tid;
	//DWORD	frame_ptr;
	//DWORD	code_ptr;
	DWORD	count;
	//BYTE	__dummy[24];
	StackInfo si[PATTERNCOUNT];
};
__declspec(thread) DWORD lo = 0;
__declspec(thread) DWORD hi = 0;
__declspec(thread) TraceInfo ti;

void stack_range()
{
	//DWORD vesp = NULL;
	//__asm mov vesp, esp;
	if ( lo && hi ) return;
	ti.count = 0;
	DWORD vesp = (DWORD)&vesp & 0xfffff000;
	while( ! IsBadWritePtr((void*)vesp, 4096) ) vesp+=4096;
	hi = vesp;
	do {
		vesp -= 4096;
		IsBadWritePtr((void*)vesp, 4096);
	} while (! IsBadWritePtr((void*)vesp, 4096));
	lo = vesp+4096;
}
void stack_trace(LPCVOID sv0, LPCVOID sv1)
{
	stack_range();
	DWORD frame_ptr = hi;
	DWORD frame_cur, caller, pattern;
	__asm
	{
		mov frame_cur, ebp
	}
	if( frame_cur >= hi || frame_cur <lo ) return;
	if ( !IsBadReadPtr((LPVOID)(4+frame_cur),4) && !IsBadReadPtr( (LPVOID)(-4+*(DWORD*)(frame_cur+4)),8 )
	{
		__asm {
			mov eax, [ebp+4]
			mov caller, eax
			mov	eax, [eax-4]
			mov pattern, eax
		}
	}
	else
	{
		Log::log("frame_cur %p is unreadable.\n",frame_cur);
		return;
	}
	caller -= rva;
	DWORD tc = 0;
	DWORD size = (frame_ptr - frame_cur) >> 2;

	if ( sv0 ) { pattern ^= ((frame_ptr - (DWORD)sv0) >> 2) * 3145739 ^ table[tc++]; }
	DWORD a = 0x67452301, b = 0xefcdab89, c = 0x98badcfe, d = 0x10325476;
	DWORD fc = fold(caller);
	DWORD fs = fold(size);

	while( frame_cur < frame_ptr )
	{
		DWORD fh = fold((frame_ptr - frame_cur) >> 2);
		DWORD ft = fold(tc++);
		DWORD e = (( ~a +  fh ) | ( ~b -  ft )) & ( (~c &  fc ) + ( ~d |  fs ) );
		DWORD f = ((  b - ~fh ) & (  c + ~ft )) + ( ( d | ~fc ) - (  a & ~fs ) );
		DWORD g = (( ~c +  fh ) | ( ~d -  ft )) | ( (~a &  fc ) + ( ~b |  fs ) );
		DWORD h = ((  d - ~fh ) & (  a + ~ft )) - ( ( b | ~fc ) - (  c & ~fs ) );
		DWORD r = tc & 0x1F;
		a = ( h >> (r   ) ) | ( h << (32-(r   )) );
		b = ( e >> (r+ 8) ) | ( e << (32-(r+ 8)) );
		c = ( f >> (r+16) ) | ( f << (32-(r+16)) );
		d = ( g >> (r+24) ) | ( g << (32-(r+24)) );
		pattern ^= a ^ b ^ c ^ d;
		DWORD next_ptr = *(LPDWORD)frame_cur;
		if( tc > 128 || next_ptr <= frame_cur )
		//	|| IsBadReadPtr((LPVOID)next_ptr,4) )
			break;
		frame_cur = next_ptr;
	}
	if ( sv1 ) { pattern ^= ((frame_ptr - (DWORD)sv1) >> 2) * 786433 ^ table[tc%TABLESIZE]; }

	StackInfoCollection &sic = StackInfoCollection::GetInstance();
	if ( sic.TryLock() )
	{
		sic.Append( 1, pattern );
		for ( DWORD i = 0, c = ti.count; i < c; i++ )
		{
			StackInfo& si = ti.si[i];
			sic.Append( si.count, si.pattern );
		}
		ti.count = 0;
		sic.Unlock();
	}
	else
	{
		DWORD i, c = ti.count;
		for( i = 0; i < c; i++ )
		{
			StackInfo& si = ti.si[i];
			if ( si.pattern == pattern )
			{
				si.count ++;
				break;
			}
		}
		if ( i == c && c < PATTERNCOUNT )
		{
			StackInfo& si = ti.si[c];
			si.count      = 1;
			si.pattern    = pattern;
			ti.count     = c + 1;
		}
	}
}
#endif

DWORD DumpCollect()
{
	StackInfoCollection::GetInstance().Dump();
	return StackInfoCollection::GetInstance().GetOctets().size();
}

typedef int (WINAPI * _tsend)(SOCKET s, const char FAR *buf, int len, int flags);
_tsend oldsend;

//#pragma optimize("y", off)

static int WINAPI newsend(SOCKET s, const char FAR *buf, int len, int flags)
{
	static time_t next_time;
	time_t now = time(NULL);

	//stack_trace(&now, &flags);
	
	return oldsend(s, buf, len, flags);
}

//#pragma optimize("",  on)

static DWORD init_tracer()
{
	Collector::Engine::GetInstance();

	srand(time(NULL));

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );
	if( 0 == WSAStartup( wVersionRequested, &wsaData )
		&& LOBYTE( wsaData.wVersion ) == 2 && HIBYTE( wsaData.wVersion ) == 2 )
	{
		/*
		send(0, "", 0, 0);
		oldsend = (_tsend)ReplaceAPI(TEXT("ws2_32.dll"), "send", (DWORD)newsend);	
		*/
	}

	for (int i = 0; i < TABLESIZE; i++)
	{
		double x = sin((double)i);
		table[i] = *(LPDWORD)&x;
	}
	return (DWORD)GetModuleHandle(NULL);
}
