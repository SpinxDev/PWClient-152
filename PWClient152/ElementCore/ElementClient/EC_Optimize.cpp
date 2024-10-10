// Filename	: EC_Optimize.cpp
// Creator	: Xu Wenbin
// Date		: 2013/4/18

#include "EC_Optimize.h"
#include "EC_Global.h"
#include "EC_World.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "EC_ManPlayer.h"
#include "EC_Object.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "EC_NativeAPI.h"
#include <A3DVector.h>
#include <Psapi.h>

#define ISPLAYERID(id)	((id) && !((id) & 0x80000000))
#define ISNPCID(id)		(((id) & 0x80000000) && !((id) & 0x40000000))

static bool IsLoginPlayer(int cid)
{
	return cid == CECObject::OCID_LOGINPLAYER;
}
static bool IsHostPlayer(int cid)
{
	return cid == CECObject::OCID_HOSTPLAYER || cid == CECObject::OCID_HOST_NAVIGATER;
}
static bool IsElsePlayer(int cid)
{
	return cid == CECObject::OCID_ELSEPLAYER;
}

static int GetClassID(int idHost)
{
	int cid = CECObject::OCID_OBJECT;
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	if (pWorld){
		if (ISPLAYERID(idHost)){
			cid = CECObject::OCID_PLAYER;
			CECPlayerMan *pPlayerMan = pWorld->GetPlayerMan();
			if (pPlayerMan){
				CECPlayer *pPlayer = pPlayerMan->GetPlayer(idHost);
				if (pPlayer){
					cid = pPlayer->GetClassID();
				}
			}
		}else if (ISNPCID(idHost)){
			cid = CECObject::OCID_NPC;
			CECNPCMan *pNPCMan = pWorld->GetNPCMan();
			if (pNPCMan){
				CECNPC *pNPC = pNPCMan->GetNPCFromAll(idHost);
				if (pNPC){
					cid = pNPC->GetClassID();
				}
			}
		}
	}
	return cid;
}

// CECOptimize::GFX
CECOptimize::GFX::GFX()
: bExcludeHost(false)
, bHidePlayerCast(false)
, bHideNPCCast(false)
, bHidePlayerAttack(false)
, bHideNPCAttack(false)
, bHidePlayerFly(false)
, bHideNPCFly(false)
, bHidePlayerHit(false)
, bHideNPCHit(false)
, bHidePlayerState(false)
, bHideNPCState(false)
, bHideWeaponStone(false)
, bHideArmorStone(false)
, bHideSuite(false)
{}

bool CECOptimize::GFX::CanShowCast(clientid_t idHost, int cid)const
{
	if (ISPLAYERID(idHost))
		return !bHidePlayerCast
		|| bExcludeHost && IsHostPlayer(cid);
	if (ISNPCID(idHost))
		return !bHideNPCCast;
	return true;
}

bool CECOptimize::GFX::CanShowAttack(clientid_t idHost, int cid)const
{
	if (ISPLAYERID(idHost))
		return !bHidePlayerAttack
		|| bExcludeHost && IsHostPlayer(cid);
	if (ISNPCID(idHost))
		return !bHideNPCAttack;
	return true;
}

bool CECOptimize::GFX::CanShowFly(clientid_t idHost)const
{
	if (ISPLAYERID(idHost))
		return !bHidePlayerFly
		|| bExcludeHost && IsHostPlayer(GetClassID(static_cast<int>(idHost)));
	if (ISNPCID(idHost))
		return !bHideNPCFly;
	return true;
}

bool CECOptimize::GFX::CanShowHit(clientid_t idHost)const
{
	if (ISPLAYERID(idHost))
		return !bHidePlayerHit
		|| bExcludeHost && IsHostPlayer(GetClassID(static_cast<int>(idHost)));
	if (ISNPCID(idHost))
		return !bHideNPCHit;
	return true;
}

bool CECOptimize::GFX::CanShowState(clientid_t idHost, int cid)const
{
	if (ISPLAYERID(idHost))
		return !bHidePlayerState
		|| IsLoginPlayer(cid)
		|| bExcludeHost && IsHostPlayer(GetClassID(static_cast<int>(idHost)));
	if (ISNPCID(idHost))
		return !bHideNPCState;
	return true;
}

bool CECOptimize::GFX::CanShowWeaponStone(clientid_t idHost, int cid)const
{
	return !bHideWeaponStone
		|| IsLoginPlayer(cid)
		|| bExcludeHost && IsHostPlayer(GetClassID(static_cast<int>(idHost)));
}

bool CECOptimize::GFX::CanShowArmorStone(clientid_t idHost, int cid)const
{
	return !bHideArmorStone
		|| IsLoginPlayer(cid)
		|| bExcludeHost && IsHostPlayer(GetClassID(static_cast<int>(idHost)));
}

bool CECOptimize::GFX::CanShowSuite(clientid_t idHost, int cid)const
{
	return !bHideSuite
		|| IsLoginPlayer(cid)
		|| bExcludeHost && IsHostPlayer(GetClassID(static_cast<int>(idHost)));
}

// CECOptimize

void CECOptimize::OnEndGameState()
{
	m_gfx = GFX();
}

void CECOptimize::SetGFX(const GFX &gfx)
{
	GFX lastGfx = m_gfx;

    m_gfx = gfx;
	
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	if (!pWorld){
		return;
	}
	
	CECNPCMan* pNPCMan = pWorld->GetNPCMan();
	if (pNPCMan){
		if (lastGfx.bHideNPCState != gfx.bHideNPCState){
			pNPCMan->OptimizeShowExtendStates();
		}
		
		CECPlayerMan* pPlayerMan = pWorld->GetPlayerMan();
		if (pPlayerMan){
			CECNPC *pHostPet = pNPCMan->GetNPC(pPlayerMan->GetHostPlayer()->GetCurPetID());
			if (lastGfx.bHidePlayerState != m_gfx.bHidePlayerState){
				pNPCMan->OptimizeShowExtendStates();
			}else if (pHostPet && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
				pHostPet->OptimizeShowExtendStates();
			}
			if (lastGfx.bHideWeaponStone != m_gfx.bHideWeaponStone){
				pNPCMan->OptimizeWeaponStoneGfx();
			}else if (pHostPet && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
				pHostPet->OptimizeWeaponStoneGfx();
			}
			if (lastGfx.bHideArmorStone != m_gfx.bHideArmorStone){
				pNPCMan->OptimizeArmorStoneGfx();
			}else if (pHostPet && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
				pHostPet->OptimizeArmorStoneGfx();
			}
			if (lastGfx.bHideSuite != m_gfx.bHideSuite){
				pNPCMan->OptimizeSuiteGfx();
			}else if (pHostPet && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
				pHostPet->OptimizeSuiteGfx();
			}
		}
	}
	
	CECPlayerMan* pPlayerMan = pWorld->GetPlayerMan();
	if (pPlayerMan){
		CECHostPlayer *pHost = pPlayerMan->GetHostPlayer();
		if (lastGfx.bHidePlayerState != m_gfx.bHidePlayerState){
			pPlayerMan->OptimizeShowExtendStates();
		}else if (pHost && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
			pHost->OptimizeShowExtendStates();
		}
		if (lastGfx.bHideWeaponStone != m_gfx.bHideWeaponStone){
			pPlayerMan->OptimizeWeaponStoneGfx();
		}else if (pHost && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
			pHost->OptimizeWeaponStoneGfx();
		}
		if (lastGfx.bHideArmorStone != m_gfx.bHideArmorStone){
			pPlayerMan->OptimizeArmorStoneGfx();
		}else if (pHost && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
			pHost->OptimizeArmorStoneGfx();
		}
		if (lastGfx.bHideSuite != m_gfx.bHideSuite){
			pPlayerMan->OptimizeSuiteGfx();
		}else if (pHost && lastGfx.bExcludeHost != m_gfx.bExcludeHost){
			pHost->OptimizeSuiteGfx();
		}
	}
}

CECOptimize & CECOptimize::Instance()
{
    static CECOptimize s_instance;
    return s_instance;
}

CECOptimize::CECOptimize()
: m_hNTDLL(NULL)
{
}

CECOptimize::~CECOptimize()
{
	if (m_hNTDLL){
		FreeLibrary(m_hNTDLL);
		m_hNTDLL = NULL;
	}
}

static bool AlignedMalloc(ULONG size, PVOID &buffer, PVOID &bufferAligned)
{
	//	按字节对齐分配内存
	//	size 为原始大小，buffer为直接分配结果（用于释放内存），bufferAligned为对齐的结果
	const ULONG EC_ALIGNMENT_BYTES = 4L;	//	1,2,4,8
	if (EC_ALIGNMENT_BYTES > 1L){
		size += EC_ALIGNMENT_BYTES;
	}
	buffer = (BYTE *)a_malloctemp(size);
	bufferAligned = buffer;
	if (EC_ALIGNMENT_BYTES > 1L){
		const ULONG EC_ALIGNMENT_MASK = EC_ALIGNMENT_BYTES-1L;
		if (((ULONG)bufferAligned) & EC_ALIGNMENT_MASK){
			bufferAligned = (PVOID)((ULONG)bufferAligned + (EC_ALIGNMENT_MASK & ~(ULONG)bufferAligned)+1L);
		}
	}
	return buffer != NULL;
}

static void AlignedFree(PVOID &buffer)
{
	if (buffer){
		a_freetemp(buffer);
		buffer = NULL;
	}
}

bool CECOptimize::GetVirtualSize(SIZE_T &s)
{	
	bool bRet(false);

	//	尝试初始化 NtQuerySystemInformation
	static bool s_bInit = false;
	static PFNNtQuerySystemInformation NtQuerySystemInformation = NULL;
	if (!s_bInit){
		s_bInit = true;
		m_hNTDLL = LoadLibraryW(L"ntdll.dll");
		if (!m_hNTDLL){
			a_LogOutput(1, "CECOptimize::GetVirtualSize,load ntdll.dll failed(GetLastError()=%u)", GetLastError());
		}else{
			NtQuerySystemInformation = (PFNNtQuerySystemInformation)GetProcAddress(m_hNTDLL, "NtQuerySystemInformation");
			if (!NtQuerySystemInformation){
				a_LogOutput(1, "CECOptimize::GetVirtualSize,Get NtQuerySystemInformation failed(GetLastError()=%u)", GetLastError());
			}
		}
		if (NtQuerySystemInformation){
			a_LogOutput(1, "CECOptimize::GetVirtualSize, NtQuerySystemInformation found");
		}
	}

	//	使用 NtQuerySystemInformation 枚举并获取当前进程信息
	while (NtQuerySystemInformation)
	{
		static ULONG initialBufferSize = 0x4000;
		NTSTATUS status;
		PVOID buffer, bufferAligned;
		ULONG bufferSize;
		
		bufferSize = initialBufferSize;
		if (!AlignedMalloc(bufferSize, buffer, bufferAligned)){
			break;
		}
		
		while (TRUE)
		{
			status = NtQuerySystemInformation(
				SystemProcessesAndThreadsInformation,
				bufferAligned,
				bufferSize,
				&bufferSize
				);
			
			if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_INFO_LENGTH_MISMATCH){
				AlignedFree(buffer);
				if (!AlignedMalloc(bufferSize, buffer, bufferAligned)){
					break;
				}
			}else{
				break;
			}
		}
		if (!buffer){
			break;
		}		
		if (!NT_SUCCESS(status)){
			AlignedFree(buffer);
			break;
		}		
		if (bufferSize <= 0x20000){
			initialBufferSize = bufferSize;
		}

		DWORD currentProcess = GetCurrentProcessId();		
		PSYSTEM_PROCESSES pSysProcess = (PSYSTEM_PROCESSES)bufferAligned ;
		while (pSysProcess)
		{
			if (pSysProcess->ProcessId == currentProcess){
				s = pSysProcess->VmCounters.VirtualSize;
				bRet = true;
				break;
			}
			if (pSysProcess->NextEntryDelta == 0){
				break;
			}
			pSysProcess = (PSYSTEM_PROCESSES)( (DWORD)pSysProcess + pSysProcess->NextEntryDelta ) ;
		}
		
		AlignedFree(buffer);
		break;
	}

	return bRet;
}

bool CECOptimize::GetPrivateBytes(SIZE_T &s)
{
    PROCESS_MEMORY_COUNTERS pmc;
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))){
		s = pmc.PagefileUsage;
        return true;
	}
    return false;
}