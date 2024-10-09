#include "ExpTypes.h"

#include <windows.h>
#include "hardinfo.h"

#pragma comment(lib, "ws2_32.lib")

void MyGetComputerName(char * szBuffer, unsigned long * pLen)
{
	GetComputerNameA(szBuffer, pLen);
}

void MySleep()
{
	Sleep(500);
}

void MyGetHardInfo(unsigned char * buffer, unsigned long * len)
{
	GetHardInfo(buffer, len);
}
