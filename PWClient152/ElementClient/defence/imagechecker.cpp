//#include "stdafx.h"

#include "imagechecker.h"
#include "malloc.h"


namespace ImageChecker
{

struct ACImageCheckInfo
{
	struct {
		DWORD addr;
		DWORD data[4];
	} stdaddr[2]; // 上面两个地址用于证明是对应版本的客户端，应该填写正确的数据

	struct {
		DWORD ret;
		DWORD addr;
		DWORD data;
	} checkaddr[16]; 
};

static ACImageCheckInfo image_info;
static CRITICAL_SECTION ics_code;

void ACImageCheckerInit()
{
	memset(&image_info, 0, sizeof(image_info));
	InitializeCriticalSection(&ics_code);
}

void CheckImage() // 反外挂线程需要每两秒调用一次
{
	EnterCriticalSection(&ics_code);
	if( image_info.stdaddr[0].addr >= 0x400000 && image_info.stdaddr[1].addr >= 0x400000 )
	{
		bool bClient = true;
		for(int i=0; i<2; ++i)
		{
			PDWORD pstdaddr = (PDWORD)(image_info.stdaddr[i].addr);
			PDWORD pstddata = (PDWORD)(image_info.stdaddr[i].data);
			if( pstdaddr[0] != pstddata[0]
				|| pstdaddr[1] != pstddata[1]
				|| pstdaddr[2] != pstddata[2]
				|| pstdaddr[3] != pstddata[3] )
			{
				bClient = false;
				break;
			}
		}
		if( bClient )
		{
			for(int j=0; j<16;++j)
			{
				if( image_info.checkaddr[j].addr <0x400000 )
					break;
				if( *(PDWORD)(image_info.checkaddr[j].addr) != image_info.checkaddr[j].data )
					image_info.checkaddr[j].ret++;
			}
		}
	}
	LeaveCriticalSection(&ics_code);
}

DWORD WINAPI UpdateImageChecker(void* checkinfo)			// 需要注册到APILoader，由 gacd 动态调用
{
	if( checkinfo == NULL ) return 0;
	DWORD r = 0;
	EnterCriticalSection(&ics_code);
	if( image_info.stdaddr[0].addr >= 0x400000 && image_info.stdaddr[1].addr >= 0x400000 )
	{
		
		for(int j=0; j<16;++j)
		{
			if( image_info.checkaddr[j].addr <0x400000 )
				break;
			if( image_info.checkaddr[j].ret != 0 )
			{
				r = j+1;
				break;;
			}
		}
		
	}

	//
	memset(&image_info, 0, sizeof(image_info));
	//
	unsigned char *p = (unsigned char*)checkinfo;
	for(int k=0; k<2; ++k)
	{
		image_info.stdaddr[k].addr = *(PDWORD)p;
		p += sizeof(DWORD);
		memcpy(image_info.stdaddr[k].data, p, 4*sizeof(DWORD));
		p += 4*sizeof(DWORD);
	}
	int n = *(int*)p;
	p += sizeof(int);
	for(int i=0; i<n && i<16; ++i)
	{
		image_info.checkaddr[i].addr = *(PDWORD)p;
		p += sizeof(DWORD);
		image_info.checkaddr[i].data = *(PDWORD)p;
		p += sizeof(DWORD);
	}
	LeaveCriticalSection(&ics_code);
	return r;
}

};

