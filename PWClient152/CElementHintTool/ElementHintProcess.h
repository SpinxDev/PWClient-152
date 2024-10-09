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
	RC_SUCCESS = 0,										//	�ɹ�
	RC_PARSE_PROTOCOL_STREAM,	//	����Э����ʱ��������
	RC_ITEM_NOT_EXIST,							//	����ѯ��Ʒ�ڱ������ݿ��в�����
	RC_INVALID_PROFESSION ,					//	ְҵid�Ƿ�
	RC_INVALID_ROLEEXTPROP,				//	��ɫ���Դ�С����
	
	RC_PARSE_PET_STREAM,				//	��������������ʱ��������
	RC_INVALID_PET_DATA,					//	���������д�
	RC_INVALID_PET_TYPE,					//	�������ʹ���

	RC_INVALID_ROLEFORCE,				//	��ɫ�������ݴ���
	RC_FORCE_NOT_EXIST,					//	����ѯ�����ڱ������ݿ��в�����
	RC_INVALID_REINCARNATIONDATA,		//	��ɫת�����ݴ���
	RC_INVALID_REALMDATA,				//	��ɫ�������ݴ���
	RC_INVALID_ROLEBRIEFEXTEND,			//	��ɫ��չ���ݴ���
	RC_INVALID_ROLEMERIDIAN,			//	��ɫ�������ݽ�������
	RC_INVALID_ROLEMERIDIAN_LAYER,		//	��ɫ������������
	RC_INVALID_ROLEMERIDIAN_CONFIG,		//	�޷��ӱ��ز��ҽ�ɫ��������
	RC_INVALID_ROLE_CARD_LEADERSHIP,	//	��ɫս��ͳ�������ݴ���
	RC_INVALID_ROLE_FATE_RING,			//	��ɫԪ�����ݴ���
	RC_INVALID_PLAYER_SPIRIT_CONFIG,	//	�ӱ��ػ�ȡ��ɫԪ������ʧ��
	RC_INVALID_ROLE_TITLE,				//	��ɫ�ƺ����ݴ���
	RC_INVALID_ROLE_TITLE_CONFIG,		//	�ӱ��ػ�ȡ�ƺ�����ʧ��

	RC_UNKNOWN = 0xFFFFFFFF,		//	δ֪����
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