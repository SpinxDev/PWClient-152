// killother.cpp : Defines the entry point for the application.
//

//#include "stdafx.h"

#pragma warning( disable : 4786 )
#include "windows.h"
#include "EC_Utility.h"
#include "ALog.h"


#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/****** Shapshot function **********************************************/

HANDLE
WINAPI
CreateToolhelp32Snapshot(
    DWORD dwFlags,
    DWORD th32ProcessID
    );

//
// The th32ProcessID argument is only used if TH32CS_SNAPHEAPLIST or
// TH32CS_SNAPMODULE is specified. th32ProcessID == 0 means the current
// process.
//
// NOTE that all of the snapshots are global except for the heap and module
//      lists which are process specific. To enumerate the heap or module
//      state for all WIN32 processes call with TH32CS_SNAPALL and the
//      current process. Then for each process in the TH32CS_SNAPPROCESS
//      list that isn't the current process, do a call with just
//      TH32CS_SNAPHEAPLIST and/or TH32CS_SNAPMODULE.
//
// dwFlags
//
#define TH32CS_SNAPHEAPLIST 0x00000001
#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008
#define TH32CS_SNAPALL      (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT      0x80000000
//
// Use CloseHandle to destroy the snapshot
//

/***** Process walking *************************************************/

typedef struct tagPROCESSENTRY32
{
    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ProcessID;          // this process
    DWORD   th32DefaultHeapID;
    DWORD   th32ModuleID;           // associated exe
    DWORD   cntThreads;
    DWORD   th32ParentProcessID;    // this process's parent process
    LONG    pcPriClassBase;         // Base priority of process's threads
    DWORD   dwFlags;
    CHAR    szExeFile[MAX_PATH];    // Path
} PROCESSENTRY32;
typedef PROCESSENTRY32 *  PPROCESSENTRY32;
typedef PROCESSENTRY32 *  LPPROCESSENTRY32;

BOOL
WINAPI
Process32First(
    HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe
    );

BOOL
WINAPI
Process32Next(
    HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe
    );

#ifdef __cplusplus
}
#endif

#include <set>
#include <map>
#include <vector>


#include "gnoctets.h"

using GNET::Octets;
#include "cheaterkiller.h"
namespace CheaterKiller
{


struct MemoryInfo
{
	DWORD sizemin;			// 内存块大小
	DWORD sizemax;
	DWORD offsetmin;		// 在此块中搜索的具体范围
	DWORD offsetmax;
	Octets data;			// 特征数据
};

struct TitleAndClass
{
	Octets classname;
	Octets windowname;
	bool matched;
};

struct WindowInfo
{
	unsigned topclassname;
	std::vector<TitleAndClass> names;
	size_t minmatch;
	size_t match;
};

class Killer
{
	WORD tick;						// 干活的时间
	std::map<DWORD, WindowInfo> windowinfos;	// 组合窗口名作为特征
	std::map<DWORD, MemoryInfo> memoryinfos;			// 其他进程的内存特征

	unsigned char *buffer;
public:
	Killer();
	~Killer() { free(buffer); windowinfos.clear(); memoryinfos.clear(); }
	void TryKillCheaters();
	void AppendMemoryInfo(DWORD id, const MemoryInfo &info) { memoryinfos[id]=info; }
	void AppendWindowInfo(DWORD id, const WindowInfo &info) { windowinfos[id]=info; }
};

static Killer killer;

Killer::Killer() : tick(0)
{ 
	buffer = (unsigned char*)malloc(0x2000);

	/*
	MemoryInfo minfo;
	
	minfo.offsetmax = 0x100;
	minfo.offsetmin = 0x0;
	minfo.sizemax = minfo.sizemin = 0x8000;
	minfo.data.resize(16);
	unsigned *pu = (unsigned *)minfo.data.begin();
	pu[0] = 0x77da6fc8;
	pu[1] = 0x77da6bf0;
	pu[2] = 0x77dc8f7d;
	pu[3] = 0x77dcd5fd;

	AppendMemoryInfo(1,minfo);
	*/
	/*
	WindowInfo winfo;
	winfo.minmatch = 1;
	winfo.match = 0;
	winfo.topclassname = 0x65746f4e;
	TitleAndClass tac;
	tac.matched = false;
	tac.windowname.replace("无标题",6);
	winfo.names.push_back(tac);
	AppendWindowInfo(1,winfo);
	*/
	
	/*
	WindowInfo winfo;
	winfo.minmatch = 1;
	winfo.match = 0;
	winfo.topclassname = 1;
	TitleAndClass tac;
	tac.matched = false;
	tac.windowname.replace("按键精灵",8);
	winfo.names.push_back(tac);
	AppendWindowInfo(2,winfo);

	tac.windowname.replace("按键游侠",8);
	winfo.names[0] = tac;
	AppendWindowInfo(3,winfo);

	tac.windowname.replace("按键武林",8);
	winfo.names[0] = tac;
	AppendWindowInfo(4,winfo);

	tac.windowname.replace("蝴蝶按键",8);
	winfo.names[0] = tac;
	AppendWindowInfo(5,winfo);

	tac.windowname.replace("武林外挂",8);
	winfo.names[0] = tac;
	AppendWindowInfo(6,winfo);

	tac.windowname.replace("Cheat Engine",12);
	winfo.names[0] = tac;
	AppendWindowInfo(13,winfo);

	tac.windowname.replace("诛仙辅助",8);
	winfo.names[0] = tac;
	AppendWindowInfo(14,winfo);

	tac.windowname.replace("完美辅助",8);
	winfo.names[0] = tac;
	AppendWindowInfo(15,winfo);

	tac.windowname.replace("武林小精灵",10);
	winfo.names[0] = tac;
	AppendWindowInfo(16,winfo);

	tac.windowname.replace("按键小精灵",10);
	winfo.names[0] = tac;
	AppendWindowInfo(17,winfo);

	tac.windowname.replace("游侠小精灵",10);
	winfo.names[0] = tac;
	AppendWindowInfo(18,winfo);

	tac.windowname.replace("完美外挂",8);
	winfo.names[0] = tac;
	AppendWindowInfo(19,winfo);

	tac.windowname.replace("诛仙外挂",8);
	winfo.names[0] = tac;
	AppendWindowInfo(20,winfo);

	tac.windowname.replace("武林辅助",8);
	winfo.names[0] = tac;
	AppendWindowInfo(7,winfo);

	tac.windowname.replace("God's Hand",10);
	winfo.names[0] = tac;
	AppendWindowInfo(8,winfo);

	tac.windowname.replace("AutoIt",6);
	winfo.names[0] = tac;
	AppendWindowInfo(9,winfo);

	tac.windowname.replace("模拟精灵",8);
	winfo.names[0] = tac;
	tac.windowname.replace("精灵库", 6);
	winfo.names.push_back(tac);
	tac.windowname.replace("开始录制", 8);
	winfo.names.push_back(tac);
	AppendWindowInfo(10,winfo);
	*/
	
	/*
	winfo.topclassname = 0;
	winfo.names.clear();

	tac.windowname.replace("自动挂机",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("自动回城",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("辅助技能",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("自动打怪",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("自动加血",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("自动加蓝",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("攻击技能",8);
	winfo.names.push_back(tac);
	tac.windowname.replace("自动喊话",8);
	winfo.names.push_back(tac);

	winfo.minmatch = 3;
	AppendWindowInfo(11,winfo);
	*/
}
/*
class RTTimer
{
	unsigned long hdword;
	unsigned long ldword;
public:
	RTTimer()
	{
		register unsigned long ld, hd;
		__asm rdtsc;
		__asm mov ld, eax;
		__asm mov hd, edx;
		hdword = hd;
		ldword = ld;
				
	}
	~RTTimer()
	{
		register unsigned long ld = ldword, hd = hdword;
		__asm rdtsc;
		__asm sub edx, hd;
		__asm sbb eax, ld;
		__asm mov ld, eax;		
		char buffer[256];
		sprintf(buffer, "timer %d us", ld/3000);
		::MessageBoxA(0, buffer, "timer", 0);
	}
};
*/


struct Node
{
	HWND h;
	DWORD flag;
};

void Killer::TryKillCheaters() // 两秒钟call一次
{
	if( windowinfos.empty() && memoryinfos.empty() ) return;
	tick ++;
	if( tick >0 && tick <=30 && tick%5 == 0 && !windowinfos.empty() ) // 处理窗口
	{
		//RTTimer timer;
		char class_buffer[36];
		char text_buffer[36];
		std::set<DWORD> killingpids;
		for ( HWND hwnd = GetTopWindow(NULL); hwnd; hwnd = GetWindow( hwnd, GW_HWNDNEXT ) )
		{
			GetClassNameA( hwnd, class_buffer, 8 );
			std::vector<WindowInfo> matchtopwindows;
			for(std::map<DWORD, WindowInfo>::const_iterator it = windowinfos.begin(), ie = windowinfos.end();
				it != ie; ++it)
			{
					if( ( tick/5-1 == it->first % 6) && (
						it->second.topclassname == 0 || 
						it->second.topclassname == 1 && ( 0x72464549 != *(unsigned*)class_buffer ) ||
						it->second.topclassname == *(unsigned*)class_buffer ))
						matchtopwindows.push_back(it->second);
			}
			if( matchtopwindows.empty() ) continue;
			
			unsigned char lvl = 0;
			Node wins[6] = { {0,0} };
			wins[0].h = hwnd;
			wins[0].flag = 0;
			//HWND now = hwnd3;
			for(;;)
			{
				if( ((wins[lvl].flag) & 1) == 0 )
				{
					unsigned char c = lvl;
					GetWindowTextA( wins[lvl].h, text_buffer, 36 );
					GetClassNameA( wins[lvl].h, class_buffer, 36 );
					for(std::vector<WindowInfo>::iterator it2 = matchtopwindows.begin(),
						ie2 = matchtopwindows.end(); it2 != ie2; ++it2)
					{
						WindowInfo &winfo = *it2;
						if( winfo.match >= winfo.minmatch ) continue; // 已经够了
						for(std::vector<TitleAndClass>::iterator it3 = winfo.names.begin(),
							ie3 = winfo.names.end(); it3 != ie3; ++it3)
						{
							TitleAndClass &tac = *it3;
							if( tac.matched ) continue; // 这个有了
							if( ( tac.classname.size() == 0 ||
								memcmp(tac.classname.begin(), class_buffer, tac.classname.size()) == 0 )
								&&
								( tac.windowname.size() == 0 ||
								memcmp(tac.windowname.begin(), text_buffer, tac.windowname.size()) == 0 )
							)
							{
								tac.matched = true;
								winfo.match++;
								winfo.topclassname = (unsigned)(wins[lvl].h);
							}
						}
					}
					
					wins[lvl].flag |= 1;
				}
				
				HWND hc = GetWindow(wins[lvl].h, GW_CHILD);
				if( lvl < 5 && ( (2&(wins[lvl].flag)) == 0 ) && hc )
				{
					wins[lvl].flag |= 2;
					++lvl;
					wins[lvl].h = hc;
					wins[lvl].flag = 0;
					continue;
				}
				
				if( lvl == 0 )
					break;
				
				hc = GetWindow(wins[lvl].h, GW_HWNDNEXT);
				if( hc )
				{
					wins[lvl].h = hc;
					wins[lvl].flag = 0;
					continue;
				}
				
				--lvl;
				// TODO find next
			}
			for(std::vector<WindowInfo>::const_iterator it4 = matchtopwindows.begin(),
						ie4 = matchtopwindows.end(); it4 != ie4; ++it4)
			{
				if( it4->match >= it4->minmatch )
				{
					DWORD pid;
					GetWindowThreadProcessId((HWND)it4->topclassname, &pid);
					killingpids.insert(pid);
				}
			}
		}
		
		
		killingpids.erase(0);
		killingpids.erase(GetCurrentProcessId());
		for(std::set<DWORD>::const_iterator it5 = killingpids.begin(), ie5 = killingpids.end();
			it5 != ie5; ++it5)
		{
			HANDLE hp = OpenProcess(PROCESS_TERMINATE, FALSE, *it5);
			if( hp )
			{
				TerminateProcess(hp, 0);
				CloseHandle(hp);
			}
		}

	}
	else if( tick >30 && tick <=60 && tick%5 == 0/* 60 */&& !memoryinfos.empty() ) // 处理内存
	{
		
		std::vector<MemoryInfo> tmemoryinfos;
		for(std::map<DWORD, MemoryInfo>::const_iterator itm = memoryinfos.begin(), iem = memoryinfos.end();
			itm != iem; ++itm)
			{
				if( tick/5 - 7 == itm->first %6 )
					tmemoryinfos.push_back(itm->second);
			}
		if( !tmemoryinfos.empty() ) 
		{
		DWORD mypid = GetCurrentProcessId();
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		
		if( snap != INVALID_HANDLE_VALUE )
		{		
			PROCESSENTRY32 entry;
			entry.dwSize = sizeof(PROCESSENTRY32);
			BOOL b = Process32First(snap, &entry);
			
			while( b )
			{
				
				DWORD pid = entry.th32ProcessID;
				
				if( pid != mypid ) // 自己就算了
				{
					
					HANDLE hp = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION|PROCESS_TERMINATE, FALSE, pid);
					if( hp )
					{
						DWORD start = 0x100000;
						while( start < 0x76000000 )
							
						{

							MEMORY_BASIC_INFORMATION mbi;	
							if( VirtualQueryEx(hp, (LPVOID)start, &mbi, sizeof(mbi)) )
								
								
							{
								start = (DWORD)mbi.BaseAddress;
								if( mbi.Protect == PAGE_EXECUTE_READ
									||mbi.Protect == PAGE_EXECUTE_READWRITE
									||mbi.Protect == PAGE_READONLY
									||mbi.Protect == PAGE_READWRITE
									||mbi.Protect == PAGE_WRITECOPY
									||mbi.Protect == PAGE_EXECUTE_WRITECOPY
									)
								{
									for(std::vector<MemoryInfo>::const_iterator it = tmemoryinfos.begin(),
										ie = tmemoryinfos.end(); it != ie; ++it)
									{
										const MemoryInfo &minfo = *it;
										if( minfo.sizemin <= mbi.RegionSize && minfo.sizemax >= mbi.RegionSize )
										{
											SIZE_T readsize = 0;
											BOOL r = ReadProcessMemory(hp, (LPCVOID)(start+minfo.offsetmin),
												buffer, minfo.offsetmax-minfo.offsetmin, &readsize);
											if( r && readsize == minfo.offsetmax-minfo.offsetmin )
											{
												unsigned char *searchend = 
													buffer + minfo.offsetmax-minfo.offsetmin-minfo.data.size();
												for(unsigned char *p = buffer; p!=searchend;++p)
												{
													if( memcmp(p, minfo.data.begin(), minfo.data.size()) == 0 )
													{
														//MessageBox(0, entry.szExeFile, "kill?", 0);
														TerminateProcess(hp, 0);
														break;
													}
												}
											}
										}
									}
								}
								start += mbi.RegionSize;
							}
							else
							{
								start += 0x4000;
							}
							
						}
						CloseHandle(hp);
					}
				}
				b = Process32Next(snap, &entry);
			}
			CloseHandle(snap);
		}
		}
	}

	if( tick == 60 ) tick = 0;
}

void WINAPI AppendCheatInfo( LPVOID buf )
{
	if( buf == NULL ) return;
	unsigned char *p = (unsigned char*)buf;
	WORD i, i2, n;
	n= *(PWORD)p; p += sizeof(WORD);
	for(i=0; i <n; ++i)
	{
		WORD id = *(PWORD)p; p += sizeof(WORD);
		WindowInfo winfo;
		winfo.match = 0;
		winfo.topclassname = *(unsigned*)p; p += sizeof(unsigned);
		winfo.minmatch = *(PWORD)p; p += sizeof(WORD);
		WORD n2 = *(PWORD)p; p += sizeof(WORD);
		for(i2=0; i2<n2; ++i2)
		{
			TitleAndClass tac;
			tac.matched = false;
			WORD n3;
			n3= *(PWORD)p; p += sizeof(WORD);
			if( n3 ) { tac.classname.replace(p, n3); p += n3; }
			n3= *(PWORD)p; p += sizeof(WORD);
			if( n3 ) { tac.windowname.replace(p, n3); p += n3; }
			winfo.names.push_back(tac);
		}
		killer.AppendWindowInfo(id, winfo);
	}

	n= *(PWORD)p; p += sizeof(WORD);
	for(i=0; i <n; ++i)
	{
		WORD id = *(PWORD)p; p += sizeof(WORD);
		MemoryInfo minfo;
		minfo.sizemin = *(PDWORD)p; p += sizeof(DWORD);
		minfo.sizemax = *(PDWORD)p; p += sizeof(DWORD);
		minfo.offsetmin = *(PDWORD)p; p += sizeof(DWORD);
		minfo.offsetmax = *(PDWORD)p; p += sizeof(DWORD);
		
		WORD n3= *(PWORD)p; p += sizeof(WORD);
		if( n3 ) { minfo.data.replace(p, n3); p += n3; }
		
		killer.AppendMemoryInfo(id, minfo);
	}
}

void WINAPI TryKillCheaters()
{
	__try
	{
		killer.TryKillCheaters();
	}
	__except (glb_HandleException(GetExceptionInformation()))
	{
		a_LogOutput(1, "exception occured in defence thread (@CheaterKiller::TryKillCheaters)!");
	}
}

};

/*
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	unsigned char buffer[1024] = { 0 };
	unsigned char *p  = (unsigned char*)buffer;
	*(PWORD)p = 1; p += sizeof(WORD);
	*(PWORD)p = 100; p += sizeof(WORD);
	*(unsigned*)p = 0; p += sizeof(unsigned);
	*(PWORD)p = 1; p += sizeof(WORD);
	*(PWORD)p = 1; p += sizeof(WORD);
	*(PWORD)p = 0; p += sizeof(WORD);
	*(PWORD)p = 7; p += sizeof(WORD);
	memcpy(p, "Element", 7); p += 7;

	CheaterKiller::AppendCheatInfo(buffer);
	while( true )
	{
		Sleep(2000);
		CheaterKiller::TryKillCheaters();
	}
	return 0;
}

*/


