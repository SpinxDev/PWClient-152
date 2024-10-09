//#include "stdafx.h"
#include <windows.h>
#include "infocollection.h"
#include <vector>
#include "process.h"
#include <stdio.h>
using namespace Collector;

DWORD ReplaceAPI(LPCTSTR dllName, LPCSTR apiName, DWORD newEntry)
{
	PIMAGE_DOS_HEADER mz_header;
	PIMAGE_NT_HEADERS32 pe_header;
	LPDWORD entry;
	DWORD rva_base, entry_count, i, oldProtect;
	HMODULE hDll = GetModuleHandle(dllName);
	DWORD oldEntry = (DWORD)GetProcAddress(hDll, apiName);

	if( NULL == hDll || NULL == oldEntry )
		return oldEntry;

	if( 0 )//Engine::GetInstance().IsNT() )
	{
		rva_base  = (DWORD)hDll;
		mz_header = (PIMAGE_DOS_HEADER)hDll;
		pe_header = (PIMAGE_NT_HEADERS32)(rva_base + mz_header->e_lfanew);
		PIMAGE_EXPORT_DIRECTORY exp_dir = (PIMAGE_EXPORT_DIRECTORY)(rva_base + pe_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		entry = (LPDWORD)(rva_base + exp_dir->AddressOfFunctions);
		entry_count = exp_dir->NumberOfFunctions;
		for ( i = 0; i < entry_count; i++ )
		{
			if ( oldEntry - rva_base == entry[i] )
			{
				VirtualProtect( entry + i, 4, PAGE_READWRITE, &oldProtect);
				entry[i] = newEntry - rva_base;
				VirtualProtect( entry + i, 4, oldProtect, &oldProtect);
				break;
			}
		}
		if ( i == entry_count )	return 0;
	}

	rva_base = (DWORD)GetModuleHandle( NULL );
	mz_header = (PIMAGE_DOS_HEADER)rva_base;
	pe_header = (PIMAGE_NT_HEADERS32)(rva_base + mz_header->e_lfanew);
	entry = (LPDWORD)(rva_base + pe_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress);
	entry_count = pe_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size / sizeof(DWORD);
	for ( i = 0; i < entry_count; i++ )
	{
		if ( oldEntry == entry[i] )
		{
			VirtualProtect( entry + i, 4, PAGE_READWRITE, &oldProtect);
			entry[i] = newEntry;
			VirtualProtect( entry + i, 4, oldProtect, &oldProtect);
			break;
		}
	}
	return oldEntry;
}

