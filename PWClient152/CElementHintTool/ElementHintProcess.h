#pragma once

#include <AString.h>   // For AString
#include <AWString.h>  // For AWString
#include <ABaseDef.h>     // For CECIvtrItem
#include "EC_IvtrItem.h"  // For CECIvtrItem
#include "../CElementClient/EC_IvtrTypes.h" // For PLAYER_PRICE_SCALE
#include "EC_Game.h"

namespace GNET
{
	class GetItemDesc;
	class GetItemDesc_Re;
	class GetRoleDesc;
	class GetRoleDesc_Re;
	class GetRoleEquipmentDesc;
	class GetRoleEquipmentDesc_Re;
	class GetRolePetCorralDesc;
	class GetRolePetCorralDesc_Re;
	class GetRoleSkillDesc;
	class GetRoleSkillDesc_Re;
	class GetEncryptedItemDesc;
	class GetEncryptedItemDesc_Re;
	class Octets;
}

enum ReturnCode
{
	RC_SUCCESS = 0,										//	成功
	RC_PARSE_PROTOCOL_STREAM,	//	分析协议流时发生错误
	RC_ITEM_NOT_EXIST,							//	待查询物品在本地数据库中不存在
	RC_INVALID_PROFESSION ,					//	职业id非法
	RC_INVALID_ROLEEXTPROP,				//	角色属性大小错误
	
	RC_PARSE_PET_STREAM,				//	分析宠物数据流时发生错误
	RC_INVALID_PET_DATA,					//	宠物数据有错
	RC_INVALID_PET_TYPE,					//	宠物类型错误

	RC_INVALID_ROLEFORCE,				//	角色势力数据错误
	RC_FORCE_NOT_EXIST,					//	待查询势力在本地数据库中不存在
	RC_INVALID_REINCARNATIONDATA,		//	角色转生数据错误
	RC_INVALID_REALMDATA,				//	角色境界数据错误
	RC_INVALID_ROLEBRIEFEXTEND,			//	角色扩展数据错误
	RC_INVALID_ROLEMERIDIAN,			//	角色灵脉数据解析错误
	RC_INVALID_ROLEMERIDIAN_LAYER,		//	角色灵脉层数错误
	RC_INVALID_ROLEMERIDIAN_CONFIG,		//	无法从本地查找角色灵脉配置
	RC_INVALID_ROLE_CARD_LEADERSHIP,	//	角色战灵统御度数据错误
	RC_INVALID_ROLE_FATE_RING,			//	角色元魂数据错误
	RC_INVALID_PLAYER_SPIRIT_CONFIG,	//	从本地获取角色元魂配置失败
	RC_INVALID_ROLE_TITLE,				//	角色称号数据错误
	RC_INVALID_ROLE_TITLE_CONFIG,		//	从本地获取称号配置失败

	RC_UNKNOWN = 0xFFFFFFFF,		//	未知错误
};

extern void initMultiThreadAccess();
extern void destroyMultiThreadAccess();
extern void getProcessedCount(int &nCountAll, int &nCountError);
extern void processOneDemand(const GNET::GetItemDesc &in, GNET::GetItemDesc_Re &out);
extern void processOneDemand(const GNET::GetRoleDesc &in, GNET::GetRoleDesc_Re &out);
extern void processOneDemand(const GNET::GetRoleEquipmentDesc &in, GNET::GetRoleEquipmentDesc_Re &out);
extern void processOneDemand(const GNET::GetRolePetCorralDesc &in, GNET::GetRolePetCorralDesc_Re &out);
extern void processOneDemand(const GNET::GetRoleSkillDesc &in, GNET::GetRoleSkillDesc_Re &out);
extern void processOneDemand(const GNET::GetEncryptedItemDesc &in, GNET::GetEncryptedItemDesc_Re &out);

extern AString UnicodeToUTF8(const wchar_t *str);
extern AWString UTF8ToUnicode(const char *str);
extern void	outputToFile(const char *szProtocolDesc, char outputReason, __int64 id, const GNET::Octets &details);