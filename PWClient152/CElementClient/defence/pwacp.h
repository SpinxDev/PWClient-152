#ifndef __DEFENCE_PWACP
#define __DEFENCE_PWACP

#include <windows.h>

namespace PWACP
{

BOOL WINAPI ACPInit(VOID);		// ��������ʱ���ã��������FASLEֱ���˳��ͻ���
BOOL WINAPI ACPDestroy(VOID);   // �����˳�ʱ����
DWORD WINAPI ACPTalk(DWORD*);	// ��ʱ����
};

#endif