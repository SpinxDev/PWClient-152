#ifndef __APILOADER_H
#define __APILOADER_H

#include <windows.h>
#include "gnmarshal.h"

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
void WINAPI DumpResult( Marshal::OctetsStream & os );
bool WINAPI DumpResultEx( Marshal::OctetsStream & os );
void WINAPI AppendSpyInfo( DWORD id, LPVOID buf, size_t size);

};

#endif
