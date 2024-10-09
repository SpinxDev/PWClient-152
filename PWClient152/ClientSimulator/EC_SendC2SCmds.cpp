 /*
 * FILE: EC_SendC2SCmds.cpp
 *
 * DESCRIPTION: Functions used to C2S net commands
 *
 * CREATED BY: duyuxin, 2004/11/15
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Common.h"
#include "GameSession.h"
#include "SessionManager.h"
#include "EC_GPDataType.h"
#define IVTRTYPE_EQUIPPACK 29
#define EQUIPIVTR_GOBLIN 23
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Fixed length game command structure
#define FIXLEN_GAMECMD(cmd_id, content_type) \
	using namespace C2S; \
	struct \
	{ \
		cmd_header hd; \
		char content[sizeof (content_type)]; \
	} cmd; \
	cmd.hd.cmd = cmd_id; \
    content_type* pCmd = (content_type*)(&cmd.content); \
	int iSize = sizeof (cmd_header) + sizeof (content_type);

//	Fixed length NPC service command structure
#define FIXLEN_NPCSEV(sev_id, content_type) \
	using namespace C2S; \
	struct \
	{ \
		cmd_header hd; \
		cmd_sevnpc_serve sev; \
		char content[sizeof (content_type)]; \
	} cmd; \
	cmd.hd.cmd = SEVNPC_SERVE; \
	cmd.sev.service_type = sev_id; \
	cmd.sev.len = sizeof (content_type); \
    content_type* pContent = (content_type*)(&cmd.content); \
	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (content_type);

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Send a nake command (commands that have id but havn't data)
static inline void _SendNakeCommand(int iCmd)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = iCmd;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send a pure data-buffer npc service
static void _SendDataBufNPCSev(int iSevice, int iAction, void* pDataBuf, int iDataSize)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iAction;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) + iDataSize;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = iSevice;
	pCmd->len = sizeof (CONTENT) + iDataSize;

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iAction = iAction;
	pData += sizeof (CONTENT);

	if (pDataBuf)
		memcpy(pData, pDataBuf, iDataSize);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send a empty data npc service
static void _SendEmptyDataNPCSev(int iSevice)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = iSevice;
	pCmd->len = 0;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement
//	
///////////////////////////////////////////////////////////////////////////

void c2s_SendDebugCmd(int iCmd, int iNumParams, ...)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + iNumParams * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = iCmd;

	if (iNumParams)
	{
		va_list vaList;
		va_start(vaList, iNumParams);

		int* pData = (int*)(pBuf + sizeof (cmd_header));
		for (int i=0; i < iNumParams; i++)
			*pData++ = va_arg(vaList, int);

		va_end(vaList);
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::PLAYER_MOVE command
void c2s_SendCmdPlayerMove(const A3DVECTOR3& vCurPos, const A3DVECTOR3& vDest, 
						   int iTime, float fSpeed, int iMoveMode, WORD wStamp)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_player_move);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = PLAYER_MOVE;

	cmd_player_move* pCmd = (cmd_player_move*)(pBuf + sizeof (cmd_header));
	pCmd->move_mode	= (BYTE)iMoveMode;
	pCmd->use_time	= (WORD)iTime;
	pCmd->vCurPos	= vCurPos;
	pCmd->vNextPos	= vDest;
	pCmd->sSpeed	= (short)(fSpeed);
	pCmd->stamp		= wStamp;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::STOP_MOVE command
void c2s_SendCmdStopMove(const A3DVECTOR3& vDest, float fSpeed, int iMoveMode,
						 BYTE byDir, WORD wStamp, int iTime)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_stop_move);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = STOP_MOVE;

	cmd_stop_move* pCmd = (cmd_stop_move*)(pBuf + sizeof (cmd_header));
	pCmd->move_mode	= (BYTE)iMoveMode;
	pCmd->sSpeed	= (short)(fSpeed);
	pCmd->dir		= byDir;
	pCmd->vCurPos	= vDest;
	pCmd->stamp		= wStamp;
	pCmd->use_time	= (WORD)iTime;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::SELECT_TARGET command
void c2s_SendCmdSelectTarget(int id)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_select_target);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = SELECT_TARGET;

	cmd_select_target* pCmd = (cmd_select_target*)(pBuf + sizeof (cmd_header));
	pCmd->id = id;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::NORMAL_ATTACK command
void c2s_SendCmdNormalAttack(BYTE byPVPMask)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_normal_attack);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = NORMAL_ATTACK;

	cmd_normal_attack* pCmd = (cmd_normal_attack*)(pBuf + sizeof (cmd_header));
	pCmd->pvp_mask = byPVPMask;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::PICKUP command
void c2s_SendCmdPickup(int idItem, int tid)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_pickup);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = PICKUP;

	cmd_pickup* pCmd = (cmd_pickup*)(pBuf + sizeof (cmd_header));
	pCmd->idItem	= idItem;
	pCmd->tid		= tid;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdUseItem(BYTE byPackage, BYTE bySlot, int tid, BYTE byCount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_use_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = USE_ITEM;

	cmd_use_item* pCmd = (cmd_use_item*)(pBuf + sizeof (cmd_header));
	pCmd->where		= byPackage;
	pCmd->index		= bySlot;
	pCmd->item_id	= tid;
	pCmd->byCount	= byCount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdUseItemWithTarget(BYTE byPackage, BYTE bySlot, int tid, BYTE byPVPMask)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_use_item_t);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = USE_ITEM_T;

	cmd_use_item_t* pCmd = (cmd_use_item_t*)(pBuf + sizeof (cmd_header));
	pCmd->where		= byPackage;
	pCmd->index		= bySlot;
	pCmd->item_id	= tid;
	pCmd->pvp_mask	= byPVPMask;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdThrowMoney(DWORD dwAmount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_throw_money);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = THROW_MONEY;

	cmd_throw_money* pCmd = (cmd_throw_money*)(pBuf + sizeof (cmd_header));
	pCmd->amount = dwAmount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetItemInfoList(int iPackage, int iCount, BYTE* aIndices)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_item_info_list) + iCount * sizeof (BYTE);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = GET_ITEM_INFO_LIST;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_get_item_info_list* pCmd = (cmd_get_item_info_list*)pTemp;
	pCmd->where	= (BYTE)iPackage;
	pCmd->count	= (BYTE)iCount;
	
	pTemp += sizeof (cmd_get_item_info_list);
	memcpy(pTemp, aIndices, iCount * sizeof (BYTE));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetTrashBoxData(bool bDetail, int iAccountBox)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_trashbox_info);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = GET_TRASHBOX_INFO;

	cmd_get_trashbox_info* pCmd = (cmd_get_trashbox_info*)(pBuf + sizeof (cmd_header));
	pCmd->detail = bDetail;
	pCmd->is_accountbox = iAccountBox;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdExgTrashBoxItem(int where, int iIndex1, int iIndex2)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_exg_trashbox_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = EXG_TRASHBOX_ITEM;

	cmd_exg_trashbox_item* pCmd = (cmd_exg_trashbox_item*)(pBuf + sizeof (cmd_header));
	pCmd->where	 = (BYTE)where;
	pCmd->index1 = (BYTE)iIndex1;
	pCmd->index2 = (BYTE)iIndex2;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdMoveTrashBoxItem(int where, int iSrc, int iDst, int iAmount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_move_trashbox_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = MOVE_TRASHBOX_ITEM;

	cmd_move_trashbox_item* pCmd = (cmd_move_trashbox_item*)(pBuf + sizeof (cmd_header));
	pCmd->where		= (BYTE)where;
	pCmd->src		= (BYTE)iSrc;
	pCmd->dest		= (BYTE)iDst;
	pCmd->amount	= iAmount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdExgTrashBoxIvtrItem(int where, int iTrashIdx, int iIvtrIdx)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_exg_trashbox_ivtr);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = EXG_TRASHBOX_IVTR;

	cmd_exg_trashbox_ivtr* pCmd = (cmd_exg_trashbox_ivtr*)(pBuf + sizeof (cmd_header));
	pCmd->where		= (BYTE)where;
	pCmd->idx_inv	= (BYTE)iIvtrIdx;
	pCmd->idx_tra	= (BYTE)iTrashIdx;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdMoveTrashBoxToIvtr(int where, int iTrashIdx, int iIvtrIdx, int iAmount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_trashbox_item_to_ivtr);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = TRASHBOX_ITEM_TO_IVTR;

	cmd_trashbox_item_to_ivtr* pCmd = (cmd_trashbox_item_to_ivtr*)(pBuf + sizeof (cmd_header));
	pCmd->where		= (BYTE)where;
	pCmd->idx_tra	= (BYTE)iTrashIdx;
	pCmd->idx_inv	= (BYTE)iIvtrIdx;
	pCmd->amount	= iAmount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdMoveIvtrToTrashBox(int where, int iTrashIdx, int iIvtrIdx, int iAmount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_ivtr_item_to_trashbox);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = IVTR_ITEM_TO_TRASHBOX;

	cmd_ivtr_item_to_trashbox* pCmd = (cmd_ivtr_item_to_trashbox*)(pBuf + sizeof (cmd_header));
	pCmd->where		= (BYTE)where;
	pCmd->idx_tra	= (BYTE)iTrashIdx;
	pCmd->idx_inv	= (BYTE)iIvtrIdx;
	pCmd->amount	= iAmount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdExgTrashBoxMoney(int iTrashMoney, int iIvtrMoney, int iAccountBox)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_exg_trashbox_money);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = EXG_TRASHBOX_MONEY;

	cmd_exg_trashbox_money* pCmd = (cmd_exg_trashbox_money*)(pBuf + sizeof (cmd_header));
	pCmd->trashbox_money	= iTrashMoney;
	pCmd->inv_money			= iIvtrMoney;
	pCmd->is_accountbox		= iAccountBox;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetOtherEquipDetail(int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_other_equip_detail);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = GET_OTHER_EQUIP_DETAIL;

	cmd_get_other_equip_detail* pCmd = (cmd_get_other_equip_detail*)(pBuf + sizeof (cmd_header));
	pCmd->target = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}
void c2s_SendCmdOpenFashionTrash(const char* szPsw)
{
	using namespace C2S;

	int iLen = 0;
	if (szPsw && szPsw[0])
		iLen = strlen(szPsw);

	int iSize = sizeof (cmd_header) + sizeof (cmd_check_security_passwd) + iLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CHECK_SECURITY_PASSWD;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_check_security_passwd* pCmd = (cmd_check_security_passwd*)pTemp;
	pCmd->passwd_size = iLen;
	pTemp += sizeof (cmd_check_security_passwd);

	if (iLen)
	{
		memcpy(pTemp, szPsw, iLen);
		pTemp += iLen;
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}
//	Send C2S::REVIVE_VILLAGE command
void c2s_SendCmdReviveVillage()
{
	_SendNakeCommand(C2S::REVIVE_VILLAGE);
}

//	Send C2S::REVIVE_ITEM command
void c2s_SendCmdReviveItem()
{
	_SendNakeCommand(C2S::REVIVE_ITEM);
}

//	Send C2S::GOTO command data
void c2s_SendCmdGoto(float x, float y, float z)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_goto);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GOTO;

	cmd_goto* pCmd = (cmd_goto*)(pBuf + sizeof (cmd_header));
	pCmd->vDest.Set(x, y, z);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::GET_ITEM_INFO commadn data
void c2s_SendCmdGetItemInfo(BYTE byPackage, BYTE bySlot)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_item_info);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GET_ITEM_INFO;

	cmd_get_item_info* pCmd = (cmd_get_item_info*)(pBuf + sizeof (cmd_header));
	pCmd->byPackage	= byPackage;
	pCmd->bySlot	= bySlot;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::GET_IVTR commadn data
void c2s_SendCmdGetIvtrData(int iPackage)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_ivtr);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GET_IVTR;

	cmd_get_ivtr* pCmd = (cmd_get_ivtr*)(pBuf + sizeof (cmd_header));
	pCmd->byPackage	= (BYTE)iPackage;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::GET_IVTR_DETAIL commadn data
void c2s_SendCmdGetIvtrDetailData(int iPackage)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_ivtr_detail);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GET_IVTR_DETAIL;

	cmd_get_ivtr_detail* pCmd = (cmd_get_ivtr_detail*)(pBuf + sizeof (cmd_header));
	pCmd->byPackage	= (BYTE)iPackage;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//	Send C2S::GET_EXT_PROP commadn data
void c2s_SendCmdGetExtProps()
{
	_SendNakeCommand(C2S::GET_EXT_PROP);
}

void c2s_SendCmdGetPartExtProps(int iPropIndex)
{
	using namespace C2S;

	int iCmd = 0;

	switch (iPropIndex)
	{
	case EXTPROPIDX_BASE:	iCmd = GET_EXT_PROP_BASE;	break;
	case EXTPROPIDX_MOVE:	iCmd = GET_EXT_PROP_MOVE;	break;
	case EXTPROPIDX_ATTACK:	iCmd = GET_EXT_PROP_ATK;	break;
	case EXTPROPIDX_DEF:	iCmd = GET_EXT_PROP_DEF;	break;
	default:
		ASSERT(0);
		return;
	}

	_SendNakeCommand(iCmd);
}

//	Send C2S::SET_STATUS_POINT commadn data
void c2s_SendCmdSetStatusPts(int v, int e, int s, int a)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_set_status_pt);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SET_STATUS_POINT;

	cmd_set_status_pt* pCmd = (cmd_set_status_pt*)(pBuf + sizeof (cmd_header));
	pCmd->vitality	= v;
	pCmd->energy	= e;
	pCmd->strength	= s;
	pCmd->agility	= a;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdLogout(int iOutType)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_player_logout);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::LOGOUT;

	cmd_player_logout* pCmd = (cmd_player_logout*)(pBuf + sizeof (cmd_header));
	pCmd->iOutType = iOutType;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetAllData(bool bpd, bool bed, bool btd)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_all_data);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GET_ALL_DATA;

	cmd_get_all_data* pCmd = (cmd_get_all_data*)(pBuf + sizeof (cmd_header));
	pCmd->byPack	= (BYTE)bpd;
	pCmd->byEquip	= (BYTE)bed;
	pCmd->byTask	= (BYTE)btd;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdChargeEquipFlySword(int iEleIdx, int iCount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_charge_equipped_flysword);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CHARGE_E_FLYSWORD;

	cmd_charge_equipped_flysword* pCmd = (cmd_charge_equipped_flysword*)(pBuf + sizeof (cmd_header));
	pCmd->element_index	= iEleIdx;
	pCmd->count			= iCount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdChargeFlySword(int iEleIdx, int iFSIdx, int iCount, int idFlySword)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_charge_flysword);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CHARGE_FLYSWORD;

	cmd_charge_flysword* pCmd = (cmd_charge_flysword*)(pBuf + sizeof (cmd_header));
	pCmd->element_index		= iEleIdx;
	pCmd->flysword_index	= iFSIdx;
	pCmd->count				= iCount;
	pCmd->flysword_id		= idFlySword;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCancelAction()
{
	_SendNakeCommand(C2S::CANCEL_ACTION);
}

void c2s_SendCmdSitDown()
{
	_SendNakeCommand(C2S::SIT_DOWN);
}

void c2s_SendCmdStandUp()
{
	_SendNakeCommand(C2S::STAND_UP);
}

void c2s_SendCmdEmoteAction(WORD wPose)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_emote_action);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::EMOTE_ACTION;

	cmd_emote_action* pCmd = (cmd_emote_action*)(pBuf + sizeof (cmd_header));
	pCmd->action = wPose;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTaskNotify(const void* pData, DWORD dwDataSize)
{
	using namespace C2S;

	if (!pData || !dwDataSize)
		return;

	int iSize = sizeof (cmd_header) + sizeof (DWORD) + dwDataSize;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TASK_NOTIFY;

	cmd_task_notify* pCmd = (cmd_task_notify*)(pBuf + sizeof (cmd_header));
	pCmd->size = dwDataSize;

	memcpy(&pCmd->placeholder, pData, dwDataSize);
		
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdUnselect()
{
	_SendNakeCommand(C2S::UNSELECT);
}

void c2s_SendCmdContinueAction()
{
	_SendNakeCommand(C2S::CONTINUE_ACTION);
}

void c2s_SendCmdStopFall()
{
	_SendNakeCommand(C2S::STOP_FALL);
}

void c2s_SendCmdGatherMaterial(int idMatter, int iToolPack, int iToolIdx, int idTool, int idTask)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_gather_material);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GATHER_MATERIAL;

	cmd_gather_material* pCmd = (cmd_gather_material*)(pBuf + sizeof (cmd_header));
	pCmd->mid			= idMatter;
	pCmd->tool_pack		= (unsigned short)iToolPack;
	pCmd->tool_index	= (unsigned short)iToolIdx;
	pCmd->tool_type		= idTool;
	pCmd->id_task		= idTask;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCastSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_cast_skill) - sizeof (int) + 
				iNumTarget * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CAST_SKILL;

	cmd_cast_skill* pCmd = (cmd_cast_skill*)(pBuf + sizeof (cmd_header));
	pCmd->skill_id		= idSkill;
	pCmd->pvp_mask		= byPVPMask;
	pCmd->target_count	= iNumTarget;

	if (iNumTarget)
		memcpy(pCmd->targets, aTargets, iNumTarget * sizeof (int));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCastInstantSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_cast_skill) - sizeof (int) + 
				iNumTarget * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CAST_INSTANT_SKILL;

	cmd_cast_skill* pCmd = (cmd_cast_skill*)(pBuf + sizeof (cmd_header));
	pCmd->skill_id		= idSkill;
	pCmd->pvp_mask		= byPVPMask;
	pCmd->target_count	= iNumTarget;

	if (iNumTarget)
		memcpy(pCmd->targets, aTargets, iNumTarget * sizeof (int));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCastPosSkill(int idSkill, const A3DVECTOR3& vPos, BYTE byPVPMask, int iNumTarget, int* aTargets)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_cast_pos_skill) - sizeof (int) +
				iNumTarget * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CAST_POS_SKILL;

	cmd_cast_pos_skill* pCmd = (cmd_cast_pos_skill*)(pBuf + sizeof (cmd_header));
	pCmd->skill_id	= idSkill;
	pCmd->pos		= vPos;
	pCmd->force_attack = byPVPMask;
	pCmd->target_count = iNumTarget;
	
	if(iNumTarget)
		memcpy(pCmd->targets, aTargets, iNumTarget*sizeof(int)); 

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTrickAction(int iAction)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_trick_action);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TRICK_ACTION;

	cmd_trick_action* pCmd = (cmd_trick_action*)(pBuf + sizeof (cmd_header));
	pCmd->trick	= (BYTE)iAction;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdSetAdvData(int iData1, int iData2)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_set_adv_data);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SET_ADV_DATA;

	cmd_set_adv_data* pCmd = (cmd_set_adv_data*)(pBuf + sizeof (cmd_header));
	pCmd->data1	= iData1;
	pCmd->data2	= iData2;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdClearAdvData()
{
	_SendNakeCommand(C2S::CLR_ADV_DATA);
}

void c2s_SendCmdQueryPlayerInfo1(int iNum, int* aIDs)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_query_player_info_1) - sizeof (int) + 
				iNum * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::QUERY_PLAYER_INFO_1;

	cmd_query_player_info_1* pCmd = (cmd_query_player_info_1*)(pBuf + sizeof (cmd_header));
	pCmd->count	= (WORD)iNum;

	if (iNum)
		memcpy(pCmd->id, aIDs, iNum * sizeof (int));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdQueryNPCInfo1(int iNum, int* aIDs)
{
	if (!iNum || !aIDs)
		return;

	using namespace C2S;

	static const int iNumLimit = 200;
	int iCount = 0;
	
	while (iCount < iNum)
	{
		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iNum)
			iNumSend = iNum - iCount;

		int iSize = sizeof (cmd_header) + sizeof (cmd_query_npc_info_1) - sizeof (int) + 
					iNumSend * sizeof (int);
		BYTE* pBuf = new BYTE [iSize];
		if (!pBuf)
			return;
		
		((cmd_header*)pBuf)->cmd = C2S::QUERY_NPC_INFO_1;

		cmd_query_npc_info_1* pCmd = (cmd_query_npc_info_1*)(pBuf + sizeof (cmd_header));
		pCmd->count	= (WORD)iNumSend;

		memcpy(pCmd->id, &aIDs[iCount], iNumSend * sizeof (int));
		iCount += iNumSend;

		SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

		delete [] pBuf;
	}
}

void c2s_SendCmdSessionEmote(int iAction)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_session_emote);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SESSION_EMOTE;

	cmd_session_emote* pCmd = (cmd_session_emote*)(pBuf + sizeof (cmd_header));
	pCmd->action = (BYTE)iAction;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdConEmoteRequest(int iAction, int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_con_emote_request);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CON_EMOTE_REQUEST;

	cmd_con_emote_request* pCmd = (cmd_con_emote_request*)(pBuf + sizeof (cmd_header));
	pCmd->action = (WORD)iAction;
	pCmd->target = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdConEmoteReply(int iResult, int iAction, int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_con_emote_reply);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CON_EMOTE_REPLY;

	cmd_con_emote_reply* pCmd = (cmd_con_emote_reply*)(pBuf + sizeof (cmd_header));
	pCmd->result = (WORD)iResult;
	pCmd->action = (WORD)iAction;
	pCmd->target = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDeadMove(float y, int iTime, float fSpeed, int iMoveMode)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_dead_move);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DEAD_MOVE;

	cmd_dead_move* pCmd = (cmd_dead_move*)(pBuf + sizeof (cmd_header));
	pCmd->y			= y;
	pCmd->move_mode	= (BYTE)iMoveMode;
	pCmd->use_time	= (WORD)iTime;
	pCmd->speed		= (short)(fSpeed);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDeadStopMove(float y, float fSpeed, BYTE byDir, int iMoveMode)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_dead_stop_move);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DEAD_STOP_MOVE;

	cmd_dead_stop_move* pCmd = (cmd_dead_stop_move*)(pBuf + sizeof (cmd_header));
	pCmd->y			= y;
	pCmd->dir		= byDir;
	pCmd->move_mode	= (BYTE)iMoveMode;
	pCmd->speed		=  (short)(fSpeed);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdEnterSanctuary(int id)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_enter_sanctuary);
	BYTE* pBuf = new BYTE[iSize];
	if( !pBuf ) return;

	((cmd_header*)pBuf)->cmd = C2S::ENTER_SANCTUARY;

	cmd_enter_sanctuary* pCmd = (cmd_enter_sanctuary*)(pBuf + sizeof(cmd_header));
	pCmd->id = id;

	SessionManager::GetSingleton().GetTheSession()->SendGameData( pBuf,iSize );

	delete[] pBuf;
}

void c2s_SendCmdOpenBooth(int iNumEntry, const void* szName, void* aEntries)
{
	using namespace C2S;

	enum
	{
		SIZE_NAMEBUF = 62
	};

	int iSize = sizeof (cmd_header) + sizeof (WORD) + SIZE_NAMEBUF + iNumEntry * sizeof (cmd_open_booth::entry_t);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::OPEN_BOOTH;

	cmd_open_booth* pCmd = (cmd_open_booth*)(pBuf + sizeof (cmd_header));
	pCmd->count	= iNumEntry;

	//	Copy name
	int iNameSize = (wcslen((wchar_t*)szName) + 1) * sizeof (wchar_t);
	if (iNameSize > SIZE_NAMEBUF)
	{
		ASSERT(iNameSize <= SIZE_NAMEBUF);
		return;
	}

	memcpy(pCmd->name, szName, iNameSize);

	//	Copy entry data
	if (iNumEntry)
		memcpy(pCmd->list, aEntries, iNumEntry * sizeof (cmd_open_booth::entry_t));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCloseBooth()
{
	_SendNakeCommand(C2S::CLOSE_BOOTH);
}

void c2s_SendCmdQueryBoothName(int iNum, int* aIDs)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (WORD) + iNum * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::QUERY_BOOTH_NAME;

	cmd_query_booth_name* pCmd = (cmd_query_booth_name*)(pBuf + sizeof (cmd_header));
	pCmd->count	= (WORD)iNum;

	if (iNum)
		memcpy(pCmd->list, aIDs, iNum * sizeof (int));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCompleteTravel()
{
	_SendNakeCommand(C2S::COMPLETE_TRAVEL);
}

void c2s_SendCmdEnalbePVP()
{
	_SendNakeCommand(C2S::ENABLE_PVP_STATE);
}

void c2s_SendCmdDisablePVP()
{
	_SendNakeCommand(C2S::DISABLE_PVP_STATE);
}

void c2s_SendCmdOpenBoothTest()
{
	_SendNakeCommand(C2S::OPEN_BOOTH_TEST);
}

void c2s_SendCmdFashionSwitch()
{
	_SendNakeCommand(C2S::SWITCH_FASHION_MODE);
}

void c2s_SendCmdEnterInstance(int iTransIdx, int idInst)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_enter_instance);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::ENTER_INSTANCE;

	cmd_enter_instance* pCmd = (cmd_enter_instance*)(pBuf + sizeof (cmd_header));
	pCmd->iTransIndex	= iTransIdx;
	pCmd->idInst		= idInst;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdRevivalAgree()
{
	_SendNakeCommand(C2S::REVIVAL_AGREE);
}

void c2s_SendCmdActiveRushFly(bool bActive)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_active_rush_fly);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::ACTIVE_RUSH_FLY;

	cmd_active_rush_fly* pCmd = (cmd_active_rush_fly*)(pBuf + sizeof (cmd_header));
	pCmd->is_active	= bActive ? 1 : 0;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdQueryDoubleExp()
{
	_SendNakeCommand(C2S::QUERY_DOUBLE_EXP);
}

void c2s_SendCmdDuelRequest(int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_duel_request);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DUEL_REQUEST;

	cmd_duel_request* pCmd = (cmd_duel_request*)(pBuf + sizeof (cmd_header));
	pCmd->target = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDuelReply(int idWho, int iReply)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_duel_reply);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DUEL_REPLY;

	cmd_duel_reply* pCmd = (cmd_duel_reply*)(pBuf + sizeof (cmd_header));
	pCmd->who = idWho;
	pCmd->param = iReply;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdBindPlayerRequest(int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_bind_player_request);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::BIND_PLAYER_REQUEST;

	cmd_bind_player_request* pCmd = (cmd_bind_player_request*)(pBuf + sizeof (cmd_header));
	pCmd->target = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdBindPlayerInvite(int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_bind_player_invite);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::BIND_PLAYER_INVITE;

	cmd_bind_player_invite* pCmd = (cmd_bind_player_invite*)(pBuf + sizeof (cmd_header));
	pCmd->target = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdBindPlayerRequestRe(int idWho, int iReply)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_bind_player_request_reply);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::BIND_PLAYER_REQUEST_REPLY;

	cmd_bind_player_request_reply* pCmd = (cmd_bind_player_request_reply*)(pBuf + sizeof (cmd_header));
	pCmd->who = idWho;
	pCmd->param = iReply;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdBindPlayerInviteRe(int idWho, int iReply)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_bind_player_invite_reply);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::BIND_PLAYER_INVITE_REPLY;

	cmd_bind_player_invite_reply* pCmd = (cmd_bind_player_invite_reply*)(pBuf + sizeof (cmd_header));
	pCmd->who = idWho;
	pCmd->param = iReply;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCancelBindPlayer()
{
	_SendNakeCommand(C2S::CANCEL_BIND_PLAYER);
}

void c2s_SendCmdMallShopping(int iCount, const void* pGoods)
{
	using namespace C2S;

	int iSizeGoods = iCount * sizeof(mall_shopping::goods);
	int iSize = sizeof (cmd_header) + sizeof (int) + iSizeGoods;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::MALL_SHOPPING;

	mall_shopping* pCmd = (mall_shopping*)(pBuf + sizeof (cmd_header));
	pCmd->count	= iCount;
	memcpy(pCmd->list, pGoods, iSizeGoods);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetWallowInfo()
{
	_SendNakeCommand(C2S::GET_WALLOW_INFO);
}

void c2s_SendCmdTeamDismissParty()
{
	_SendNakeCommand(C2S::TEAM_DISMISS_PARTY);
}

void c2s_SendCmdUseItemWithArg(unsigned char where, unsigned char count, unsigned short index, int item_id, const void* arg, size_t size)
{
	using namespace C2S;

	const int iSize = sizeof (cmd_header) + sizeof (use_item_with_arg) + size;
	BYTE* pBuf = new BYTE[iSize];

	((cmd_header*)pBuf)->cmd = C2S::USE_ITEM_WITH_ARG;

	use_item_with_arg* pCmd = (use_item_with_arg*)(pBuf + sizeof (cmd_header));
	pCmd->where = where;
	pCmd->count = count;
	pCmd->index = index;
	pCmd->item_id = item_id;
	memcpy(pCmd + 1, arg, size);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;
}

void c2s_SendCmdQueryCashInfo()
{
	_SendNakeCommand(C2S::QUERY_CASH_INFO);
}

void c2s_SendCmdGetMallItemPrice(int start_index, int end_index)
{
	using namespace C2S;

	const int iSize = sizeof (cmd_header) + sizeof (cmd_get_mall_item_price);
	BYTE* pBuf = new BYTE[iSize];

	((cmd_header*)pBuf)->cmd = C2S::GET_MALL_ITEM_PRICE;

	cmd_get_mall_item_price* pCmd = (cmd_get_mall_item_price*)(pBuf + sizeof (cmd_header));
	pCmd->start_index = start_index;
	pCmd->end_index = end_index;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;
}

void c2s_SendCmdEquipFashionItem(int iBody, int iLeg, int iFoot, int iWrist, int iHead)
{
	using namespace C2S;

	const int iSize = sizeof (cmd_header) + sizeof (cmd_equip_fashion_item);
	BYTE* pBuf = new BYTE[iSize];

	((cmd_header*)pBuf)->cmd = C2S::EQUIP_FASHION_ITEM;

	cmd_equip_fashion_item* pCmd = (cmd_equip_fashion_item*)(pBuf + sizeof (cmd_header));
	pCmd->idx_body = iBody;
	pCmd->idx_leg = iLeg;
	pCmd->idx_foot = iFoot;
	pCmd->idx_wrist = iWrist;	
	pCmd->idx_head	= iHead;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;

}

void c2s_SendCmdNotifyForceAttack(int iForceAttack, BYTE refuseBless)
{
	using namespace C2S;

	const int iSize = sizeof (cmd_header) + sizeof (cmd_notify_force_attack);
	BYTE* pBuf = new BYTE[iSize];

	((cmd_header*)pBuf)->cmd = C2S::NOTIFY_FORCE_ATTACK;

	cmd_notify_force_attack* pCmd = (cmd_notify_force_attack*)(pBuf + sizeof (cmd_header));
	pCmd->force_attack = iForceAttack;
	pCmd->refuse_bless = refuseBless;
		
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;
}

void c2s_SendCmdDividendMallShopping(int iCount, const void* pGoods)
{
	using namespace C2S;

	int iSizeGoods = iCount * sizeof(cmd_dividend_mall_shopping::goods);
	int iSize = sizeof (cmd_header) + sizeof (int) + iSizeGoods;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::DIVIDEND_MALL_SHOPPING;

	cmd_dividend_mall_shopping* pCmd = (cmd_dividend_mall_shopping*)(pBuf + sizeof (cmd_header));
	pCmd->count	= iCount;
	memcpy(pCmd->list, pGoods, iSizeGoods);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetDividendMallItemPrice(int start_index, int end_index)
{
	using namespace C2S;

	const int iSize = sizeof (cmd_header) + sizeof (cmd_get_dividend_mall_item_price);
	BYTE* pBuf = new BYTE[iSize];

	((cmd_header*)pBuf)->cmd = C2S::GET_DIVIDEND_MALL_ITEM_PRICE;

	cmd_get_dividend_mall_item_price* pCmd = (cmd_get_dividend_mall_item_price*)(pBuf + sizeof (cmd_header));
	pCmd->start_index = start_index;
	pCmd->end_index = end_index;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;
}

void c2s_SendCmdChooseMultiExp(int iIndex)
{
	using namespace C2S;
	
	const int iSize = sizeof (cmd_header) + sizeof (cmd_choose_multi_exp);
	BYTE* pBuf = new BYTE[iSize];
	
	((cmd_header*)pBuf)->cmd = C2S::CHOOSE_MULTI_EXP;
	
	cmd_choose_multi_exp* pCmd = (cmd_choose_multi_exp*)(pBuf + sizeof (cmd_header));
	pCmd->index = iIndex;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;
}

void c2s_SendCmdToggleMultiExp(bool isActivate)
{
	using namespace C2S;
	
	const int iSize = sizeof (cmd_header) + sizeof (cmd_toggle_muti_exp);
	BYTE* pBuf = new BYTE[iSize];
	
	((cmd_header*)pBuf)->cmd = C2S::TOGGLE_MULTI_EXP;
	
	cmd_toggle_muti_exp* pCmd = (cmd_toggle_muti_exp*)(pBuf + sizeof (cmd_header));
	pCmd->is_activate = isActivate ? 1 : 0;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	delete[] pBuf;
}

void c2s_SendCmdMultiExchangeItem(int iPackage, int iCount, const int* aIndexPairs)
{
	if (iCount < 1)	return;

	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_multi_exchange_item) + (iCount-1) * sizeof (cmd_multi_exchange_item::operation);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = MULTI_EXCHANGE_ITEM;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_multi_exchange_item* pCmd = (cmd_multi_exchange_item*)pTemp;
	pCmd->location = (BYTE)iPackage;
	pCmd->count	= (BYTE)iCount;

	for (int i=0; i<iCount; ++i)
	{
		pCmd->operations[i].index1 = (BYTE)aIndexPairs[i+i];
		pCmd->operations[i].index2 = (BYTE)aIndexPairs[i+i+1];
	}
		
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdSysAuction(int type, const void* pDataBuf, int iDataSize)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(int) + iDataSize;
	BYTE * pBuf = new BYTE[iSize];
	if (!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = SYSAUCTION_OP;
	BYTE* pTemp = pBuf + sizeof(cmd_header);

	*((int *)pTemp) = type;
	pTemp += sizeof(int);

	if (pDataBuf)
		memcpy(pTemp, pDataBuf, iDataSize);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDebugDeliverCmd(WORD type, const char* szCmd)
{
	using namespace C2S;

	if (!szCmd || !szCmd[0])
		return;

	int iCmdLen = strlen(szCmd);

	int iSize = sizeof (cmd_header) + sizeof (cmd_debug_deliver_cmd) + iCmdLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DEBUG_DELIVER_CMD;

	cmd_debug_deliver_cmd* pCmd = (cmd_debug_deliver_cmd*)(pBuf + sizeof (cmd_header));
	pCmd->type = type;

	memcpy(pCmd+1, szCmd, iCmdLen);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDebugGSCmd(const char* szCmd)
{
	using namespace C2S;

	if (!szCmd || !szCmd[0])
		return;

	int iCmdLen = strlen(szCmd);

	int iSize = sizeof (cmd_header) + iCmdLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DEBUG_GS_CMD;

	BYTE* pDst = pBuf + sizeof (cmd_header);
	memcpy(pDst, szCmd, iCmdLen);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdCongregateReply(int type, bool agree, int sponsor_id)
{
	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (cmd_congregate_reply);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CONGREGATE_REPLY;
	
	cmd_congregate_reply* pCmd = (cmd_congregate_reply*)(pBuf + sizeof (cmd_header));
	pCmd->type = static_cast<char>(type);
	pCmd->agree = agree;
	pCmd->sponsor = sponsor_id;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdPreviewProduceReply(bool bSelectOld)
{
	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (cmd_preview_choose);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::PRODUCE4_CHOOSE;
	
	cmd_preview_choose* pCmd = (cmd_preview_choose*)(pBuf + sizeof (cmd_header));
	pCmd->remain = bSelectOld ? 1 : 0;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdGetForceGlobalData()
{
	_SendNakeCommand(C2S::GET_FORCE_GLOBAL_DATA);
}

void c2s_SendCmdQueryProfitTime()
{
	_SendNakeCommand(C2S::QUERY_PROFIT_TIME);
}
void c2s_SendCmdEnterPKPrecinct()
{
	_SendNakeCommand(C2S::ENTER_PK_PROTECTED);
}

void c2s_SendCmdCountryGetPersonalScore()
{
	_SendNakeCommand(C2S::COUNTRYBATTLE_GET_PERSONAL_SCORE);
}

void c2s_SendCmdGetServerTime()
{
	_SendNakeCommand(C2S::GET_SERVER_TIMESTAMP);
}

void c2s_SendCmdTeamInvite(int idPlayer)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_invite);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_INVITE;

	cmd_team_invite* pCmd = (cmd_team_invite*)(pBuf + sizeof (cmd_header));
	pCmd->idPlayer	= idPlayer;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamAgreeInvite(int idLeader, int iTeamSeq)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_agree_invite);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_AGREE_INVITE;

	cmd_team_agree_invite* pCmd = (cmd_team_agree_invite*)(pBuf + sizeof (cmd_header));
	pCmd->idLeader	= idLeader;
	pCmd->team_seq	= iTeamSeq;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamRejectInvite(int idLeader)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_reject_invite);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_REJECT_INVITE;

	cmd_team_reject_invite* pCmd = (cmd_team_reject_invite*)(pBuf + sizeof (cmd_header));
	pCmd->idLeader = idLeader;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamLeaveParty()
{
	_SendNakeCommand(C2S::TEAM_LEAVE_PARTY);
}

void c2s_SendCmdTeamKickMember(int idMember)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_kick_member);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_KICK_MEMBER;

	cmd_team_kick_member* pCmd = (cmd_team_kick_member*)(pBuf + sizeof (cmd_header));
	pCmd->idMember = idMember;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamSetPickupFlag(short sFlag)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_set_pickup);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_SET_PICKUP;

	cmd_team_set_pickup* pCmd = (cmd_team_set_pickup*)(pBuf + sizeof (cmd_header));
	pCmd->pickup_flag = sFlag;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamMemberPos(int iNumMem, int* aMemIDs)
{
	if (!iNumMem || !aMemIDs)
		return;

	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (unsigned short) + iNumMem * sizeof (int);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_MEMBER_POS;

	cmd_team_member_pos* pCmd = (cmd_team_member_pos*)(pBuf + sizeof (cmd_header));
	pCmd->wMemCnt = (unsigned short)iNumMem;

	memcpy(pCmd->aMemIDs, aMemIDs, iNumMem * sizeof (int));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamAssistSel(int idTeamMember)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_assist_sel);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::ASSIST_SELECT;

	cmd_assist_sel* pCmd = (cmd_assist_sel*)(pBuf + sizeof (cmd_header));
	pCmd->idTeamMember = idTeamMember;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamAskJoin(int idTarget)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_ask_join);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_ASK_TO_JOIN;

	cmd_team_ask_join* pCmd = (cmd_team_ask_join*)(pBuf + sizeof (cmd_header));
	pCmd->idTarget = idTarget;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamReplyJoinAsk(int idAsker, bool bAgree)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_team_reply_join_ask);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TEAM_REPLY_JOIN_ASK;

	cmd_team_reply_join_ask* pCmd = (cmd_team_reply_join_ask*)(pBuf + sizeof (cmd_header));
	pCmd->idAsker	= idAsker;
	pCmd->result	= bAgree ? 1 : 0;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamChangeLeader(int idLeader)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_change_team_leader);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CHANGE_TEAM_LEADER;

	cmd_change_team_leader* pCmd = (cmd_change_team_leader*)(pBuf + sizeof (cmd_header));
	pCmd->idLeader	= idLeader;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdTeamNotifyOwnPos()
{
	_SendNakeCommand(C2S::NOTIFY_POS_IN_TEAM);
}

void c2s_SendCmdMoveIvtrItem(int iSrc, int iDest, int iAmount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_move_ivtr_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::MOVE_IVTR_ITEM;

	cmd_move_ivtr_item* pCmd = (cmd_move_ivtr_item*)(pBuf + sizeof (cmd_header));
	pCmd->src	 = iSrc;
	pCmd->dest	 = iDest;
	pCmd->amount = iAmount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdExgIvtrItem(int iIndex1, int iIndex2)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_exg_ivtr_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::EXG_IVTR_ITEM;

	cmd_exg_ivtr_item* pCmd = (cmd_exg_ivtr_item*)(pBuf + sizeof (cmd_header));
	pCmd->index1 = iIndex1;
	pCmd->index2 = iIndex2;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDropIvtrItem(int iIndex, int iAmount)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_drop_ivtr_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DROP_IVTR_ITEM;

	cmd_drop_ivtr_item* pCmd = (cmd_drop_ivtr_item*)(pBuf + sizeof (cmd_header));
	pCmd->index	 = iIndex;
	pCmd->amount = iAmount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdDropEquipItem(int iIndex)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_drop_equip_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::DROP_EQUIP_ITEM;

	cmd_drop_equip_item* pCmd = (cmd_drop_equip_item*)(pBuf + sizeof (cmd_header));
	pCmd->index = iIndex;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdExgEquipItem(int iIndex1, int iIndex2)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_exg_equip_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::EXG_EQUIP_ITEM;

	cmd_exg_equip_item* pCmd = (cmd_exg_equip_item*)(pBuf + sizeof (cmd_header));
	pCmd->idx1 = iIndex1;
	pCmd->idx2 = iIndex2;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdEquipItem(int iIvtrIdx, int iEquipIdx)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_equip_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::EQUIP_ITEM;

	cmd_equip_item* pCmd = (cmd_equip_item*)(pBuf + sizeof (cmd_header));
	pCmd->idx_inv	= iIvtrIdx;
	pCmd->idx_eq	= iEquipIdx;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdMoveItemToEquip(int iIvtrIdx, int iEquipIdx)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_move_item_to_equip);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::MOVE_ITEM_TO_EQUIP;

	cmd_move_item_to_equip* pCmd = (cmd_move_item_to_equip*)(pBuf + sizeof (cmd_header));
	pCmd->idx_inv	= iIvtrIdx;
	pCmd->idx_eq	= iEquipIdx;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetOtherEquip(int iNumID, int* aIDs)
{
	using namespace C2S;

	static const int iNumLimit = 250;
	int iCount = 0;

	while (iCount < iNumID)
	{
		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iNumID)
			iNumSend = iNumID - iCount;

		int iSize = sizeof (cmd_header) + sizeof (WORD) + iNumSend * sizeof (int);
		BYTE* pBuf = new BYTE [iSize];
		if (!pBuf)
			return;
		
		((cmd_header*)pBuf)->cmd = C2S::GET_OTHER_EQUIP;

		cmd_get_other_equip* pCmd = (cmd_get_other_equip*)(pBuf + sizeof (cmd_header));
		pCmd->size = (WORD)iNumSend;

		for (int i=0; i < iNumSend; i++)
			pCmd->idlist[i] = aIDs[iCount+i];

		SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

		delete [] pBuf;

		iCount += iNumSend;
	}
}

void c2s_SendCmdGetWealth(bool bpd, bool bed, bool btd)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_get_own_wealth);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GET_OWN_WEALTH;

	cmd_get_own_wealth* pCmd = (cmd_get_own_wealth*)(pBuf + sizeof (cmd_header));
	pCmd->byPack	= (BYTE)bpd;
	pCmd->byEquip	= (BYTE)bed;
	pCmd->byTask	= (BYTE)btd;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdPetSummon(int iPetIdx)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_summon_pet);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SUMMON_PET;

	cmd_summon_pet* pCmd = (cmd_summon_pet*)(pBuf + sizeof (cmd_header));
	pCmd->pet_index	= iPetIdx;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdPetRecall()
{
	_SendNakeCommand(C2S::RECALL_PET);
}

void c2s_SendCmdPetBanish(int iPetIdx)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_banish_pet);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::BANISH_PET;

	cmd_banish_pet* pCmd = (cmd_banish_pet*)(pBuf + sizeof (cmd_header));
	pCmd->pet_index	= iPetIdx;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdPetCtrl(int idTarget, int cmd, void* pParamBuf, int iParamLen)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_pet_ctrl) + iParamLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::PET_CTRL;

	cmd_pet_ctrl* pCmd = (cmd_pet_ctrl*)(pBuf + sizeof (cmd_header));
	pCmd->target	= idTarget;
	pCmd->pet_cmd	= cmd;

	if (iParamLen)
	{
		if (1 == cmd)
		{
			ASSERT(iParamLen == sizeof (BYTE));
		}
		else if (cmd == 2 || cmd == 3 || cmd == 5)
		{
			ASSERT(iParamLen == sizeof (int));
		}
		else if (cmd == 4)
		{
			ASSERT(iParamLen == sizeof (int) + sizeof (BYTE));
		}
	
		memcpy(pCmd+1, pParamBuf, iParamLen);
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

//  Goblin comomands...
void c2s_SendCmdGoblinAddAttribute(int str, int agi, int vit, int eng)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_elf_add_attribute);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::ELF_ADD_ATTRIBUTE;

	cmd_elf_add_attribute* pCmd = (cmd_elf_add_attribute*)(pBuf + sizeof(cmd_header));
	pCmd->str = str;
	pCmd->agi = agi;
	pCmd->vit = vit;
	pCmd->eng = eng;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGoblinAddGenius(int g1, int g2, int g3, int g4, int g5)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_elf_add_genius);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::ELF_ADD_GENIUS;
	
	cmd_elf_add_genius* pCmd = (cmd_elf_add_genius*)(pBuf + sizeof(cmd_header));
	pCmd->genius[0] = g1;
	pCmd->genius[1] = g2;
	pCmd->genius[2] = g3;
	pCmd->genius[3] = g4;
	pCmd->genius[4] = g5;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGoblinPlayerInsertExp(unsigned int exp, int use_sp)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_elf_player_insert_exp);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::ELF_PLAYER_INSERT_EXP;
	
	cmd_elf_player_insert_exp* pCmd = (cmd_elf_player_insert_exp*)(pBuf + sizeof(cmd_header));
	pCmd->exp = exp;
	pCmd->use_sp = use_sp;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGoblinEquipItem(int index_inv)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_elf_equip_item);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::ELF_EQUIP_ITEM;
	
	cmd_elf_equip_item* pCmd = (cmd_elf_equip_item*)(pBuf + sizeof(cmd_header));
	pCmd->index_inv = index_inv;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGoblinChangeSecureStatus(unsigned char status)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_elf_change_secure_status);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::ELF_CHANGE_SECURE_STATUS;
	
	cmd_elf_change_secure_status* pCmd = (cmd_elf_change_secure_status*)(pBuf + sizeof(cmd_header));
	pCmd->status = status;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;	
}
void c2s_SendCmdGoblinCastSkill(int idSkill, unsigned char force_attack, int iNumTarget, int* aTargets)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_cast_elf_skill) - sizeof(int) + iNumTarget*sizeof(int);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::CAST_ELF_SKILL;
	
	cmd_cast_elf_skill* pCmd = (cmd_cast_elf_skill*)(pBuf + sizeof(cmd_header));
	pCmd->skill_id = idSkill;
	pCmd->force_attack = force_attack;
	pCmd->target_count = iNumTarget;

	if (iNumTarget)
		memcpy(pCmd->targets, aTargets, iNumTarget * sizeof (int));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;	
}
void c2s_SendCmdGoblinChargeEquip(int iEleIdx, int iCount)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(recharge_equipped_elf);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::RECHARGE_EQUIPPED_ELF;
	
	recharge_equipped_elf* pCmd = (recharge_equipped_elf*)(pBuf + sizeof(cmd_header));
	pCmd->element_index = iEleIdx;
	pCmd->count = iCount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;	
}
void c2s_SendCmdGoblinCharge(int iEleIdx, int iGoblinIdx, int iCount, int idGoblin)
{
	using namespace C2S;

	//  Since the struct of charging a goblin and charging a fly sword is same, 
	//  here we use the protocol CHARGE_FLYSWORD 
	int iSize = sizeof(cmd_header) + sizeof(cmd_charge_flysword);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::CHARGE_FLYSWORD;
	
	cmd_charge_flysword* pCmd = (cmd_charge_flysword*)(pBuf + sizeof(cmd_header));
	pCmd->element_index = iEleIdx;
	pCmd->count = iCount;
	pCmd->flysword_index = iGoblinIdx;
	pCmd->flysword_id = idGoblin;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;	
}
void c2s_SendCmdGoblinRefineActive(int item_id)
{
	using namespace C2S;
	
	//  Use protocol USE_ITEM
	int iSize = sizeof(cmd_header) + sizeof(cmd_use_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::USE_ITEM;

	cmd_use_item* pCmd = (cmd_use_item*)(pBuf + sizeof(cmd_header));	
	pCmd->where = IVTRTYPE_EQUIPPACK;
	pCmd->byCount = 1;
	pCmd->index = EQUIPIVTR_GOBLIN;
	pCmd->item_id = item_id;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;	
}

void c2s_SendCmdRechargeOnlineAward(int type,int item_count,C2S::cmd_recharge_online_award::entry* pEntry)
{
	using namespace C2S;

	if(item_count==0 || !pEntry) return;

	int iSize = sizeof(cmd_header) + sizeof(int) * 2 + sizeof(cmd_recharge_online_award::entry) * item_count;
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::RECHARGE_ONLINE_AWARD;
	
	cmd_recharge_online_award* pCmd = (cmd_recharge_online_award*)(pBuf + sizeof(cmd_header));
	pCmd->type = type;
	pCmd->count = item_count;
	int i(0);
	for (;i<item_count;i++)
	{
		pCmd->entry_list[i] = pEntry[i];
	}
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;	
}
void c2s_SendCmdToggleOnlineAward(int type, int active)
{
	using namespace C2S;

	int iSize = sizeof(cmd_header) + sizeof(cmd_toggle_online_award);
	BYTE* pBuf = new BYTE[iSize];
	if(!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::TOGGLE_ONLINE_AWARD;
	
	cmd_toggle_online_award* pCmd = (cmd_toggle_online_award*)(pBuf + sizeof(cmd_header));
	pCmd->type = type;
	pCmd->activate = active;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdNPCSevHello(int nid)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_hello);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_HELLO;

	cmd_sevnpc_hello* pCmd = (cmd_sevnpc_hello*)(pBuf + sizeof (cmd_header));
	pCmd->id = nid;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevBuy(int iItemNum, C2S::npc_trade_item* aItems)
{
	if (!iItemNum || !aItems)
		return;

	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         size_t money;
		 int	consume_contrib;
		 int	cumulate_contrib;
		 int force_id;
		 int force_repu;
		 int force_contrib;
         size_t item_count;
	};

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) + 
				iItemNum * sizeof (npc_trade_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_SELL;
	pCmd->len = sizeof (CONTENT) + iItemNum * sizeof (npc_trade_item);

	pTemp += sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->money	= 0;	//	Not use now
	pContent->consume_contrib = 0;
	pContent->cumulate_contrib = 0;
	pContent->force_id = 0;
	pContent->force_repu = 0;
	pContent->force_contrib = 0;
	pContent->item_count = iItemNum;

	//	Copy item data ...
	pTemp += sizeof (CONTENT);
	memcpy(pTemp, aItems, iItemNum * sizeof (npc_trade_item));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevSell(int iItemNum, C2S::npc_trade_item* aItems)
{
	if (!iItemNum || !aItems)
		return;

	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
        size_t item_count;
	};

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) +
				iItemNum * sizeof (npc_trade_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_BUY;
	pCmd->len = sizeof (CONTENT) + iItemNum * sizeof (npc_trade_item);

	pTemp += sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->item_count = iItemNum;

	//	Copy item data ...
	pTemp += sizeof (CONTENT);
	memcpy(pTemp, aItems, iItemNum * sizeof (npc_trade_item));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevRepair(BYTE byPackage, BYTE bySlot, int idItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int tid;
		BYTE byPackage;
		BYTE bySlot;
	};

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_REPAIR;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->tid		= idItem;
	pContent->byPackage = byPackage;
	pContent->bySlot	= bySlot;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevRepairAll()
{
	c2s_SendCmdNPCSevRepair(0, 0, -1);
}

void c2s_SendCmdNPCSevHeal()
{
	_SendEmptyDataNPCSev(GP_NPCSEV_HEAL);
}

void c2s_SendCmdNPCSevTransmit(int iIndex)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int index;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_TRANSMIT;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->index	= iIndex;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevTransmitDirect(int iNum, int *aWayPoints)
{
	using namespace C2S;

	if (iNum < 2 || !aWayPoints)
		return;

#pragma pack(1)

	struct CONTENT
	{
		size_t count;
		int		waypoint_list[1];
	};   

#pragma pack()

	int iLen = sizeof (CONTENT) + (iNum-1)*sizeof(int);
	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + iLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_TRANSMIT_DIRECT;
	pCmd->len = iLen;

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->count	= iNum;
	memcpy(pContent->waypoint_list, aWayPoints, sizeof(int)*iNum);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevCountryJoinLeave(bool bJoin)
{
	using namespace C2S;
	
#pragma pack(1)
	
	struct CONTENT
	{
		int	type;
	};   
	
#pragma pack()
	
	int iLen = sizeof (CONTENT);
	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + iLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;
	
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_COUNTRY_JOINLEAVE;
	pCmd->len = iLen;
	
	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->type	= bJoin ? 1 : 2;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdNPCSevCountryLeaveWar()
{
	_SendEmptyDataNPCSev(GP_NPCSEV_COUNTRY_LEAVEWAR);
}

void c2s_SendCmdNPCSevEmbed(WORD wStoneIdx, WORD wEquipIdx, int tidStone, int tidEquip)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         WORD wStoneIdx;
         WORD wEquipIdx;
         int tidStone;
         int tidEquip;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_EMBED;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->wStoneIdx	= wStoneIdx;
	pContent->wEquipIdx	= wEquipIdx;
	pContent->tidStone	= tidStone;
	pContent->tidEquip	= tidEquip;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevClearEmbeddedChip(int iEquipIdx, int tidEquip)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int iEquipIdx;
         int tidEquip;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_CLEAR_TESSERA;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->iEquipIdx	= iEquipIdx;
	pContent->tidEquip	= tidEquip;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevLearnSkill(int idSkill)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int idSkill;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_LEARN;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idSkill = idSkill;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevMakeItem(int idSkill, int idItem, DWORD dwCount)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int idSkill;
		 int idItem;
		 size_t dwCount;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_MAKEITEM;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idSkill	= idSkill;
	pContent->idItem	= idItem;
	pContent->dwCount	= dwCount;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevBreakItem(int idSkill, int iIvtrIdx, int idItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int idSkill;
		 int iIvtrIdx;
		 int idItem;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_BREAKITEM;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idSkill	= idSkill;
	pContent->iIvtrIdx	= iIvtrIdx;
	pContent->idItem	= idItem;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevAcceptTask(int idTask,int idStorage,int idRefreshItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int idTask;
		 int idStorage;
		 int idRefreshItem;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_TASK_ACCEPT;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idTask = idTask;
	pContent->idStorage = idStorage;
	pContent->idRefreshItem = idRefreshItem;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevReturnTask(int idTask, int iChoice)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int idTask;
		 int iChoice;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_TASK_RETURN;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idTask = idTask;
	pContent->iChoice = iChoice;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevTaskMatter(int idTask)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
         int idTask;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_TASK_MATTER;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idTask = idTask;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevChgTrashPsw(const char* szOldPsw, const char* szNewPsw)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		unsigned short origin_size;
		unsigned short new_size;
	};   

#pragma pack()

	int iOldLen = 0;
	if (szOldPsw && szOldPsw[0])
		iOldLen = strlen(szOldPsw);

	int iNewLen = 0;
	if (szNewPsw && szNewPsw[0])
		iNewLen = strlen(szNewPsw);

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) +
				iOldLen + iNewLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_TRASHPSW;
	pCmd->len = sizeof (CONTENT) + iOldLen + iNewLen;
	pTemp += sizeof (cmd_sevnpc_serve);

	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->origin_size	= (WORD)iOldLen;
	pContent->new_size		= (WORD)iNewLen;
	pTemp += sizeof (CONTENT);

	if (iOldLen)
	{
		memcpy(pTemp, szOldPsw, iOldLen);
		pTemp += iOldLen;
	}

	if (iNewLen)
	{
		memcpy(pTemp, szNewPsw, iNewLen);
		pTemp += iNewLen;
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevOpenTrash(const char* szPsw)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		unsigned int psw_size;
	};   

#pragma pack()

	int iLen = 0;
	if (szPsw && szPsw[0])
		iLen = strlen(szPsw);

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) + iLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_OPENTRASH;
	pCmd->len = sizeof (CONTENT) + iLen;
	pTemp += sizeof (cmd_sevnpc_serve);

	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->psw_size = iLen;
	pTemp += sizeof (CONTENT);

	if (iLen)
	{
		memcpy(pTemp, szPsw, iLen);
		pTemp += iLen;
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevViewTrash()
{
	_SendEmptyDataNPCSev(GP_NPCSEV_VIEW_TRASHBOX);
}

void c2s_SendCmdNPCSevIdentify(int iSlot, int tidItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iSlot;
		int tidItem;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_IDENTIFY;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->iSlot	= iSlot;
	pContent->tidItem = tidItem;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevFaction(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_FACTION, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevTravel(int iIndex, int idLine)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
        int iIndex;		//	Index in travel service
        int idLine;		//	Line's ID
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_TRAVEL;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->iIndex = iIndex;
	pContent->idLine = idLine;	

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevGetContent(int idSevice)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_get_content);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_GET_CONTENT;

	cmd_sevnpc_get_content* pCmd = (cmd_sevnpc_get_content*)(pBuf + sizeof (cmd_header));
	pCmd->service_id = idSevice;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevBoothBuy(int idBooth, int iItemNum, C2S::npc_booth_item* aItems, int iYinpiao)
{
	if (!iItemNum || !aItems)
		return;

	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int booth_id;
        size_t money;
		size_t yinpiao;
        size_t item_count;
	};

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) + 
				iItemNum * sizeof (npc_booth_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_BOOTHSELL;
	pCmd->len = sizeof (CONTENT) + iItemNum * sizeof (npc_booth_item);

	pTemp += sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->booth_id = idBooth;
	pContent->money	= 0;	//	Not use now
	pContent->yinpiao = iYinpiao;
	pContent->item_count = iItemNum;

	//	Copy item data ...
	pTemp += sizeof (CONTENT);
	memcpy(pTemp, aItems, iItemNum * sizeof (npc_booth_item));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevBoothSell(int idBooth, int iItemNum, C2S::npc_booth_item* aItems)
{
	if (!iItemNum || !aItems)
		return;

	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int booth_id;
        size_t money;
		size_t yinpiao;
        size_t item_count;
	};

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) + 
				iItemNum * sizeof (npc_booth_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_BOOTHBUY;
	pCmd->len = sizeof (CONTENT) + iItemNum * sizeof (npc_booth_item);

	pTemp += sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->booth_id = idBooth;
	pContent->money	= 0;	//	Not use now
	pContent->item_count = iItemNum;

	//	Copy item data ...
	pTemp += sizeof (CONTENT);
	memcpy(pTemp, aItems, iItemNum * sizeof (npc_booth_item));

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevWaypoint()
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_WAYPOINT;
	pCmd->len = 0;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevForgetSkill(int idSkill)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
        int idSkill;	//	Index in travel service
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_FORGETSKILL;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->idSkill = idSkill;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevFaceChange(int iIvtrIdx, int idItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
        int iIvtrIdx;	//	Index in pack
		int idItem;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_FACECHANGE;
	pCmd->len = sizeof (CONTENT);

	CONTENT* pContent = (CONTENT*)(pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve));
	pContent->iIvtrIdx	= iIvtrIdx;
	pContent->idItem	= idItem;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevMail(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_MAIL, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevVendue(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_VENDUE, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevDblExpTime(int iIndex)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iIndex;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_DBLEXPTIME;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iIndex = iIndex;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevHatchPet(int iIvtrIdx, int idEgg)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iIvtrIdx;
		int idEgg;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_HATCHPET;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iIvtrIdx = iIvtrIdx;
	pContent->idEgg = idEgg;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevRestorePet(int iPetIdx)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iPetIdx;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_RESTOREPET;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iPetIdx = iPetIdx;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevBattle(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_BATTLE, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevBuildTower(int iTowerIdx, int idItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iTowerIdx;
		int idItem;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_BUILDTOWER;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iTowerIdx = iTowerIdx;
	pContent->idItem = idItem;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevLeaveBattle()
{
	_SendEmptyDataNPCSev(GP_NPCSEV_LEAVEBATTLE);
}

void c2s_SendCmdNPCSevReturnStatusPt(int iIndex, int idItem)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iIndex;
		int idItem;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_RETURNSTATUSPT;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iIndex = iIndex;
	pContent->idItem = idItem;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevAccountPoint(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_ACCOUNTPOINT, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevRefine(int iIndex, int idItem, int iComponentIndex)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iIndex;
		int idItem;
		int iComponentIndex;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_REFINE;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iIndex = iIndex;
	pContent->idItem = idItem;
	pContent->iComponentIndex = iComponentIndex;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevPetName(int iPetIdx, void* szNameBuf, int iBufLen)
{
	if (!szNameBuf || !((wchar_t*)szNameBuf)[0] || !iBufLen)
		return;

	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		WORD wPetIdx;
        WORD wNameLen;
	};

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT) + iBufLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	BYTE* pTemp = pBuf + sizeof (cmd_header);
	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)pTemp;
	pCmd->service_type = GP_NPCSEV_PETNAME;
	pCmd->len = sizeof (CONTENT) + iBufLen;

	pTemp += sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pTemp;
	pContent->wPetIdx = (WORD)iPetIdx;
	pContent->wNameLen = (WORD)iBufLen;

	//	Copy item data ...
	pTemp += sizeof (CONTENT);
	memcpy(pTemp, szNameBuf, iBufLen);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevPetSkillDel(int idSkill)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int idSkill;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_PETSKILL_DEL;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->idSkill = idSkill;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevPetSkillLearn(int idSkill)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int idSkill;
	};   

#pragma pack()

	int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_PETSKILL_LEARN;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->idSkill = idSkill;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdNPCSevBindItem(int iIndex, int iItemId, int iItemNeed)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		int item_index;        //需要绑定物品在包裹栏的位置
		int item_id;           //需要绑定物品的ID，用于校验
		int	item_need;		//	绑定消耗物品ID
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_BIND_ITEM;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->item_index = iIndex;
	pContent->item_id = iItemId;
	pContent->item_need = iItemNeed;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCSevDestroyBind(int iIndex, int iItemId)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		int item_index;        //需要绑定物品在包裹栏的位置
		int item_id;           //需要绑定物品的ID，用于校验
	};   

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_DESTROY_BIND;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->item_index = iIndex;
	pContent->item_id = iItemId;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

}

void c2s_SendCmdNPCSevCancelDestroy(int iIndex, int iItemId)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		int item_index;        //需要绑定物品在包裹栏的位置
		int item_id;           //需要绑定物品的ID，用于校验
	};   

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = CP_NPCSEV_CANCEL_DESTROY;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->item_index = iIndex;
	pContent->item_id = iItemId;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCSevStockTransaction(int nWithdraw, int nCash, int nMoney)
{
	using namespace C2S;

#pragma pack(1)
	
	struct CONTENT
	{
		int iWithdraw;
		int iCash;
		int iMoney;
	};   

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_STOCK_TRANSACTION;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->iWithdraw = nWithdraw;
	pContent->iCash = nCash;
	pContent->iMoney = nMoney;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCSevStockOperation(int nType, void* pData, int nLen)
{
	_SendDataBufNPCSev(GP_NPCSEV_STOCK_OPERATION, nType, pData, nLen);
}

void c2s_SendCmdNPCSevDye(int inv_index, int item_type, int dye_index, int dye_type)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
        int inv_index;          //要染色的时装在包裹中的位置 
        int item_type;          //时装的id
        int dye_index;          //染色材料的位置 
        int dye_type;           //染色剂的id
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_DYE;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_index = inv_index;
	pContent->item_type = item_type;
	pContent->dye_index = dye_index;
	pContent->dye_type  = dye_type;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCSevPetDye(int pet_index, int pet_id, int dye_index, int dye_id)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
        int pet_index;         //要染色的宠物在宠物包裹中的位置 
        int pet_id;				 //宠物的id
        int dye_index;       //染色材料的位置 
        int dye_id;				//染色剂的id
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_PET_DYE;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->pet_index = pet_index;
	pContent->pet_id = pet_id;
	pContent->dye_index = dye_index;
	pContent->dye_id  = dye_id;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCSevRefineTrans(int iSrcIdx, int idSrc, int iDstIdx, int idDst)
{
#pragma pack(1)
	
	struct CONTENT
	{
		int iSrcIdx;
		int idSrc;
		int iDstIdx;
		int idDst;
	};

#pragma pack()

	FIXLEN_NPCSEV(GP_NPCSEV_REFINE_TRANS, CONTENT)

	pContent->iSrcIdx	= iSrcIdx;
	pContent->idSrc		= idSrc;
	pContent->iDstIdx	= iDstIdx;
	pContent->idDst		= idDst;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevCompose(int idSkill, int idRecipe, int aMaterialIDs[16], int aMaterialIdx[16])
{
#pragma pack(1)

	struct CONTENT
	{
		int skill;
		int recipe_id;
		int materials_id[16];   //指定位置的材料ID
		int idxs[16];               //指定位置的材料在包裹内的索引
	};

#pragma pack()

	FIXLEN_NPCSEV(GP_NPCSEV_COMPOSE, CONTENT)

	pContent->skill		= idSkill;
	pContent->recipe_id	= idRecipe;

	for (int i=0; i < 16; i++)
	{
		pContent->materials_id[i]	= aMaterialIDs[i];
		pContent->idxs[i]			= aMaterialIdx[i];
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevLevelUpProduce(int idSkill, int idRecipe, int aMaterialIDs[16], int aMaterialIdx[16], int iEquipId, int iEquipInvId, int iInheritType, bool bPreview)
{
#pragma pack(1)

	struct CONTENT
	{
		int skill;				// 生产技能ID
		int recipe_id;			// 配方ID
		int materials_id[16];	// 指定位置的材料ID
		int idxs[16];		    // 指定位置的材料在包裹内的索引
		int equip_id;			// 待升级装备id
		int equip_inv_idx;		// 待升级装备在包裹栏中的位置
		char inhert_type;		// bit0:是否继承精炼；bit1：是否继承孔数；bit2:是否继承宝石
								// 如果继承宝石，必须继承孔数
	};

#pragma pack()

	FIXLEN_NPCSEV(bPreview ? GP_NPCSEV_PREVIEW_PRODUCE : GP_NPCSEV_LEVELUP_PRODUCE, CONTENT)

	pContent->skill		= idSkill;
	pContent->recipe_id	= idRecipe;
	pContent->equip_id	= iEquipId;
	pContent->equip_inv_idx = iEquipInvId;
	pContent->inhert_type = iInheritType;
	
	for (int i=0; i < 16; i++)
	{
		pContent->materials_id[i]	= aMaterialIDs[i];
		pContent->idxs[i]			= aMaterialIdx[i];
	}

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevOpenAccountBox()
{
	_SendEmptyDataNPCSev(GP_NPCSEV_OPEN_ACCOUNT_BOX);
}

void c2s_SendCmdNPCSevGodEvilConvert()
{
	_SendEmptyDataNPCSev(GP_NPCSEV_GODEVILCONVERT);
}

void c2s_SendCmdNPCSevWebTrade(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_WEBTRADE, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevFactionFortressService1(int iAction, void* pDataBuf, int iDataSize)
{
	_SendDataBufNPCSev(GP_NPCSEV_FACTION_FORTRESS_SERVICE_1, iAction, pDataBuf, iDataSize);
}

void c2s_SendCmdNPCSevFactionFortressService2(int iAction, int param0, int param1, int param2)
{
#pragma pack(1)

	struct CONTENT 
	{
		int	type;
		int	param[3];
	};

#pragma pack()

	FIXLEN_NPCSEV(GP_NPCSEV_FACTION_FORTRESS_SERVICE_2, CONTENT)

	pContent->type = iAction;
	pContent->param[0] = param0;
	pContent->param[1] = param1;
	pContent->param[2] = param2;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevFactionFortressService3(int param0, int param1, int param2)
{
#pragma pack(1)

	struct CONTENT 
	{
		unsigned int src_index;	//	转换原材料
		unsigned int dst_index;	//	转换成材料
		int	count;				//	转换数目
	};

#pragma pack()

	FIXLEN_NPCSEV(GP_NPCSEV_FACTION_FORTRESS_SERVICE_3, CONTENT)

	pContent->src_index = (unsigned int)param0;
	pContent->dst_index = (unsigned int)param1;
	pContent->count = param2;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevWeddingBook(int type, int start_time, int end_time, int scene, int bookcard_index)
{
#pragma pack(1)

	struct CONTENT
	{
		int type;	//	1:预约，2:取消预约
		int wedding_start_time;
		int wedding_end_time;
		int wedding_scene;
		int bookcard_index;
	};

#pragma pack()

	FIXLEN_NPCSEV(GP_NPCSEV_WEDDING_BOOK, CONTENT)

	pContent->type = type;
	pContent->wedding_start_time = start_time;
	pContent->wedding_end_time = end_time;
	pContent->wedding_scene = scene;
	pContent->bookcard_index = bookcard_index;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevWeddingInvite(int invitecard_index, int invitee)
{
#pragma pack(1)
	
	struct CONTENT
	{
		int invitecard_index;
		int invitee;
	};
	
#pragma pack()
	
	FIXLEN_NPCSEV(GP_NPCSEV_WEDDING_INVITE, CONTENT)
		
	pContent->invitecard_index = invitecard_index;
	pContent->invitee = invitee;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevEngrave(int engrave_id, int ivtr_index)
{
#pragma pack(1)
	
	struct CONTENT
	{
		int engrave_id;
		int ivtr_index;
	};
	
#pragma pack()
	
	FIXLEN_NPCSEV(GP_NPCSEV_ENGRAVE, CONTENT)
		
	pContent->engrave_id = engrave_id;
	pContent->ivtr_index = ivtr_index;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevGetDPSDPHRank(int rank_mask)
{
#pragma pack(1)
	
	struct CONTENT
	{
		int	type;
        int rank_mask;
	};
	
#pragma pack()
	
	FIXLEN_NPCSEV(GP_NCPSEV_DPS_DPH_RANK, CONTENT)
		
	pContent->type = 0;
    pContent->rank_mask = rank_mask;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevAddonRegen(int recipe_id, int ivtr_index)
{
#pragma pack(1)
	
	struct CONTENT
	{
		int recipe_id;
		int ivtr_index;
	};
	
#pragma pack()
	
	FIXLEN_NPCSEV(GP_NPCSEV_ADDONREGEN, CONTENT)
		
		pContent->recipe_id = recipe_id;
	pContent->ivtr_index = ivtr_index;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevForce(int type, int param)
{
#pragma pack(1)

	struct CONTENT
	{
		int type;
		int param;	// type为RT_JOIN时，作为force_template_id
	};

#pragma pack()

	enum REQ_TYPE
	{
		RT_JOIN = 0,
		RT_LEAVE,

		RT_MAX,
	};

	FIXLEN_NPCSEV(GP_NPCSEV_FORCE, CONTENT)

	pContent->type = type;
	pContent->param = param;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdQueryNetworkDelay(int timestamp)
{
	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (cmd_network_delay);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::CALC_NETWORK_DELAY;

	cmd_network_delay* pCmd = (cmd_network_delay*)(pBuf + sizeof (cmd_header));
	pCmd->timestamp	= timestamp;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGetFactionFortressInfo()
{
	using namespace C2S;

	cmd_header buf;
	buf.cmd = C2S::GET_FACTION_FORTRESS_INFO;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(&buf, sizeof(buf));
}

void c2s_SendCmdNPCSevMakeSlot(int src_index, int src_id)
{
#pragma pack(1)

	struct CONTENT
	{
		int src_index;          //要打孔装备的索引
        int src_id;             //要打孔装备的id
	};

#pragma pack()

	FIXLEN_NPCSEV(GP_NPCSEV_MAKE_SLOT, CONTENT)

	pContent->src_index		= src_index;
	pContent->src_id		= src_id;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(&cmd, iSize);
}

void c2s_SendCmdNPCSevGoblinReturnStatusPt(int iIndex, int idItem,  int str, int agi, int vit, int eng)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_idx_elf;		//小精灵在包裹中索引
		unsigned char inv_idx_ticket;	//洗点道具在包裹中索引
		short str;
		short agi;
		short vit;
		short eng;
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLIN_RETURNSTATUSPT;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_idx_elf = iIndex;
	pContent->inv_idx_ticket = idItem;
	pContent->str = str;
	pContent->agi = agi;
	pContent->vit = vit;
	pContent->eng = eng;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinReturnGeniusPt(int iIndex, int idItem)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_idx_elf;  //小精灵在包裹中索引
		unsigned char inv_idx_ticket; //洗点道具在包裹中索引
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLIN_RETURNGENIUSPT;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_idx_elf = iIndex;
	pContent->inv_idx_ticket = idItem;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinLearnSkill(int iIndex, int idSkill)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_idx_elf;	// 小精灵在包裹中索引
		unsigned short skill_id;
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLINSKILL_LEARN;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_idx_elf = iIndex;
	pContent->skill_id = idSkill;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinForgetSkill(int iIndex, int idSkill, int forget_level)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_index;	// 小精灵在包裹中索引
		unsigned short skill_id;
		short	forget_level;
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLINSKILL_DEL;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_index = iIndex;
	pContent->skill_id = idSkill;
	pContent->forget_level = forget_level;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinRefine(int iIndex, int idItem, int iTicketCnt)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_idx_elf;		//小精灵在包裹中索引
		unsigned char inv_idx_ticket;	//精炼道具在包裹中索引 ，255为不使用道具，
		short ticket_cnt;				//使用道具数，除梦幻丹可放置多个，其他道具只能为1 
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLIN_REFINE;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_idx_elf = iIndex;
	pContent->inv_idx_ticket = idItem;
	pContent->ticket_cnt = iTicketCnt;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinTransferRefine(int iIndexSrc, int iIndexDest)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char src_index;        //小精灵在包裹中索引
		unsigned char dest_index;       //小精灵在包裹中索引
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLIN_REFINETRANSFER;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->src_index = iIndexSrc;
	pContent->dest_index = iIndexDest;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinDestroy(int iIndex)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_index;        //小精灵在包裹中索引
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLIN_DESTROY;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_index = iIndex;
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdNPCSevGoblinEquipDestroy(int iIndex, int iSlot, int iEquip_index)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_index;    //小精灵在包裹中索引
		unsigned char mask;         //装备mask 有效的是1， 2， 4， 8
		unsigned char equip_index;	//新增项 要替换的装备在包裹中的索引
									//如果为255 则表示只拆装备 如果>=0则表示替换装备
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_GOBLINEQUIP_DESTROY;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_index = iIndex;

	if(iSlot == 0)
		pContent->mask = 1;
	else if(iSlot == 1)
		pContent->mask = 2;
	else if(iSlot == 2)
		pContent->mask = 4;
	else if(iSlot == 3)
		pContent->mask = 8;

	pContent->equip_index = iEquip_index;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCSevDyeBySuit(int iBody, int iLeg, int iFoot, int iWrist, int iDye)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_idx_body;	//要染色的衣服在包裹中的位置,255表示衣服不染色
		unsigned char inv_idx_leg;	//要染色的裤子在包裹中的位置 255表示裤子不染色
		unsigned char inv_idx_foot;	//要染色的鞋在包裹中的位置 255表示鞋不染色
		unsigned char inv_idx_wrist;	//要染色的手套在包裹中的位置 255表示手套不染色

		unsigned char inv_idx_dye;	//染色材料的位置 
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_DYE_BY_SUIT;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_idx_body = iBody;
	pContent->inv_idx_leg = iLeg;
	pContent->inv_idx_foot = iFoot;
	pContent->inv_idx_wrist = iWrist;

	pContent->inv_idx_dye = iDye;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}

void c2s_SendCmdNPCRepairDestroyingItem(int iIndex)
{
	using namespace C2S;

#pragma pack(1)

	struct CONTENT
	{
		unsigned char inv_idx;	//要修复的装备在包裹中的位置 
	};

#pragma pack()

	const int iSize = sizeof (cmd_header) + sizeof (cmd_sevnpc_serve) + sizeof (CONTENT);
	BYTE pBuf[iSize];

	((cmd_header*)pBuf)->cmd = C2S::SEVNPC_SERVE;

	cmd_sevnpc_serve* pCmd = (cmd_sevnpc_serve*)(pBuf + sizeof (cmd_header));
	pCmd->service_type = GP_NPCSEV_REPAIR_DESTROYING_ITEM;
	pCmd->len = sizeof (CONTENT);

	BYTE* pData = pBuf + sizeof (cmd_header) + sizeof (cmd_sevnpc_serve);
	CONTENT* pContent = (CONTENT*)pData;
	pContent->inv_idx = iIndex;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
}
void c2s_SendCmdGMMoveToPlayer(int idPlayer)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (gm_cmd_moveto_player);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_MOVETO_PLAYER;

	gm_cmd_moveto_player* pCmd = (gm_cmd_moveto_player*)(pBuf + sizeof (cmd_header));
	pCmd->pid = idPlayer;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGMCallInPlayer(int idPlayer)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (gm_cmd_callin_player);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_CALLIN_PLAYER;

	gm_cmd_callin_player* pCmd = (gm_cmd_callin_player*)(pBuf + sizeof (cmd_header));
	pCmd->pid = idPlayer;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGMGenerate(int tid)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (gm_cmd_generate);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_GENERATE;

	gm_cmd_generate* pCmd = (gm_cmd_generate*)(pBuf + sizeof (cmd_header));
	pCmd->tid = tid;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGMActiveSpawner(bool bActive, int id_spawner)
{
	using namespace C2S;

	int iSize = sizeof (cmd_header) + sizeof (gm_cmd_active_spawner);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_ACTIVE_SPAWNER;

	gm_cmd_active_spawner* pCmd = (gm_cmd_active_spawner*)(pBuf + sizeof (cmd_header));
	pCmd->is_active = bActive ? 1 : 0;
	pCmd->sp_id = id_spawner;

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGMGenerateMob(int idMob, int idVis, short sCount, short sLifeTime, 
						const ACHAR* szName)
{
	using namespace C2S;

	int iNameLen = 0;
// 	if (szName || szName[0])
// 	{
// 		iNameLen = a_strlen(szName) * sizeof (ACHAR);
// 		a_ClampRoof(iNameLen, 18);
// 	}

	int iSize = sizeof (cmd_header) + sizeof (gm_cmd_generate_mob) + iNameLen;
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_GENERATE_MOB;

	gm_cmd_generate_mob* pCmd = (gm_cmd_generate_mob*)(pBuf + sizeof (cmd_header));
	pCmd->mob_id = idMob;
	pCmd->vis_id = idVis;
	pCmd->count = sCount >= 1 ? sCount : 1;
	pCmd->life = sLifeTime;
	pCmd->name_len = iNameLen;

	//	Copy name
	memcpy(pCmd+1, szName, iNameLen);

	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	delete [] pBuf;
}

void c2s_SendCmdGMTriggerInvisible()
{
	_SendNakeCommand(C2S::GM_INVISIBLE);
}

void c2s_SendCmdGMTriggerInvincible()
{
	_SendNakeCommand(C2S::GM_INVINCIBLE);
}

void c2s_SendCmdGMQuerySpecItem(int role_id, int item_id)
{
	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (gmcmd_query_spec_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_QUERY_SPEC_ITEM;
	
	gmcmd_query_spec_item* pCmd = (gmcmd_query_spec_item*)(pBuf + sizeof (cmd_header));
	pCmd->roleid = role_id;
	pCmd->type = item_id;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdGMRemoveSpecItem(int role_id, int item_id, unsigned char where, unsigned char index, unsigned int count)
{
	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (gm_cmd_remove_spec_item);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_REMOVE_SPEC_ITEM;
	
	gm_cmd_remove_spec_item* pCmd = (gm_cmd_remove_spec_item*)(pBuf + sizeof (cmd_header));
	pCmd->roleid = role_id;
	pCmd->type = item_id;
	pCmd->where = where;
	pCmd->index = index;
	pCmd->count = count;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

void c2s_SendCmdAutoTeamSetGoal(int goal_type, int goal_id, int op)
{
	using namespace C2S;
	int iSize = sizeof(cmd_header) + sizeof(cmd_auto_team_set_goal);
	BYTE* pBuf = (BYTE*)a_malloctemp(iSize);
	if(!pBuf) return;
	
	((cmd_header*)pBuf)->cmd = C2S::AUTO_TEAM_SET_GOAL;
	
	cmd_auto_team_set_goal* pCmd = (cmd_auto_team_set_goal*)(pBuf + sizeof(cmd_header));
	pCmd->goal_id = goal_id;
	pCmd->op = op;
	pCmd->goal_type = goal_type;
	
	SessionManager::GetSingleton().GetTheSession()->SendGameData(pBuf, iSize);

	a_freetemp(pBuf);
}