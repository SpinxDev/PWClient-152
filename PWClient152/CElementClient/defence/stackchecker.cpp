//#include "stdafx.h"

#include "stackchecker.h"
#include "malloc.h"


namespace StackChecker
{

// tstack.cpp : Defines the entry point for the console application.
//

struct ACTraceInfo
{
	void* code;			// ��̬У����룬 �ɼ���ջ���ϲ���õ�ַ��
	int data[7];		// У����������汾��
};

static ACTraceInfo trace_info[16];
static CRITICAL_SECTION cs_code;

void ACStackCheckInit()
{
	memset(trace_info, 0, sizeof(trace_info));
	InitializeCriticalSection(&cs_code);
}

void ACTrace(int id) // ���� trace ����
{
	EnterCriticalSection(&cs_code);
	ACTraceInfo &tinfo = trace_info[id];
	if( tinfo.code != NULL )
	{
		int *pi = (int*)&tinfo;
		__asm mov eax, pi;	// �������
		__asm call [eax];		// ����У�麯��
	}
	LeaveCriticalSection(&cs_code);
}


VOID WINAPI GetStackCheckData(void* buf)				// ��Ҫע�ᵽAPILoader���� gacd ��̬����
{
	EnterCriticalSection(&cs_code);
	memcpy(buf, &trace_info[0], sizeof(trace_info));
	LeaveCriticalSection(&cs_code);
}

VOID WINAPI UpdateStackCheckCode(void *codes)		// ��Ҫע�ᵽAPILoader���� gacd ��̬����
{
	EnterCriticalSection(&cs_code);
	// �����޸�trace����
	unsigned char *p = (unsigned char*)codes;
	int n = *(int*)p;
	p += sizeof(int);
	for(int i=0; i<n; ++i)
	{
		int id = *(int*)p;
		p += sizeof(int);
		int size = *(int*)p;
		p += sizeof(int);
		void *code = (void*)p;
		p += size;

		ACTraceInfo &tinfo = trace_info[id];
		free(tinfo.code);
		memset(&tinfo, 0, sizeof(tinfo));
		if( size > 0 )
		{
			//printf("before set code, n=%d,i=%d, id=%d,size=%d\n", n, i, id, size);
			if( tinfo.code = malloc(size) )
			{
				memcpy(tinfo.code , code, size);
				DWORD oldprotect;
				VirtualProtect(tinfo.code, size, PAGE_EXECUTE_READWRITE, &oldprotect);
				FlushInstructionCache(GetCurrentProcess(), tinfo.code, size);
			}
			//printf("after set code, id=%d,size=%d\n", id, size);
		}
	}
	LeaveCriticalSection(&cs_code);
	//printf("set code ok\n");
}

};
