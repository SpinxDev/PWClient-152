#ifndef __ONLINEGAME_COMMON_CHAT_DATA_H__
#define __ONLINEGAME_COMMON_CHAT_DATA_H__

#include "../EC_IvtrItem.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"

#pragma pack(1)

//	外置声明
extern CECGame *g_pGame;

namespace CHAT_S2C
{
	enum 
	{
		CHAT_EQUIP_ITEM,
		CHAT_GENERALCARD_COLLECTION,
		CHAT_POLICYCHAT_PARAMETER
	};

	enum
	{
		CHAT_PARAMETER_ROLEID		= 0x00000001,
		CHAT_PARAMETER_LOCALVAL0	= 0x00000002,
		CHAT_PARAMETER_LOCALVAL1	= 0x00000004,
		CHAT_PARAMETER_LOCALVAL2	= 0x00000008,
	};

	struct chat_item_base
	{
		short cmd_id;
	};
	struct chat_equip_item
	{
		short cmd_id;
		int type;
		int expire_date;
		int proc_type;
		unsigned short content_length;
		char content[1];
	};
	struct chat_generalcard_collection
	{
		short cmd_id;
		int card_id;
	};
	struct chat_policy_parameter
	{
		short cmd_id;
		int parameter_mask;
		//if(parameter_mask & CHAT_PARAMETER_ROLEID) 1 int insert here
		//if(parameter_mask & CHAT_PARAMETER_LOCALVAL0) 1 int insert here
		//if(parameter_mask & CHAT_PARAMETER_LOCALVAL1) 1 int insert here
		//if(parameter_mask & CHAT_PARAMETER_LOCALVAL2) 1 int insert here
	};

	//存储策略喊话特殊参数的本地数据
	struct PolicyChatParameter
	{
		int role_id;
		ACString name;
		int parameter_mask;
		int localval_0;
		int localval_1;
		int localval_2;

	public:
		PolicyChatParameter()
		{
			role_id			= -1;
			name			= _AL("");
			parameter_mask	= -1;
			localval_0		= -1;
			localval_1		= -1;
			localval_2		= -1;
		}
		bool IsNameReady() const
		{
			return HasRoleID()?!name.IsEmpty():true;
		}

		void SetRoleId( int id)
		{
			role_id = id;
			name = g_pGame->GetGameRun()->GetPlayerName(role_id, false);
		}

		void SetLocaValue( int localValue, int id)
		{
			switch( id )
			{
			case 0:
				localval_0 = localValue;
				break;
			case 1:
				localval_1 = localValue;
				break;
			case 2:
				localval_2 = localValue;
				break;
			}
		}

		void GetNameFromServer()
		{
			if(role_id != -1 && HasRoleID()){
				g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(1, &role_id, 1);
			}
		}

		bool HasRoleID()const
		{
			return parameter_mask & CHAT_PARAMETER_ROLEID;
		}

		bool HasLocalValue(int ValueNum)const
		{
			bool bHasValue = false;
			switch (ValueNum)
			{
			case 0:
				if (parameter_mask & CHAT_PARAMETER_LOCALVAL0)
					bHasValue = true;
				break;
			case 1:
				if (parameter_mask & CHAT_PARAMETER_LOCALVAL1)
					bHasValue = true;
				break;
			case 2:
				if (parameter_mask & CHAT_PARAMETER_LOCALVAL2)
					bHasValue = true;
				break;
			}
			return bHasValue;
		}

		bool GetLocalValue(int localVariableID, int &localVariable)const{
			if (!HasLocalValue(localVariableID)){
				return false;
			}
			switch (localVariableID){
			case 0:
				localVariable = localval_0;
				break;
			case 1:
				localVariable = localval_1;
				break;
			case 2:
				localVariable = localval_2;
				break;
			}
			return true;
		}
	};
	
	// 从服务器传来的聊天协议数据中创建 Item 
	//
	inline CECIvtrItem * CreateChatItem(const Octets &data)
	{
		CECIvtrItem* pIvtrItem = NULL;				
		if (data.size() > 0){
			chat_item_base* pInfo = (chat_item_base*)data.begin();
			if (pInfo->cmd_id == CHAT_EQUIP_ITEM){
				chat_equip_item* pItemInfo = (chat_equip_item*)data.begin();
				size_t sz = sizeof(chat_equip_item) - sizeof(pItemInfo->content);					
				if (data.size() >= sz && sz + pItemInfo->content_length == data.size()){
					if (pItemInfo->cmd_id == CHAT_EQUIP_ITEM){
						pIvtrItem = CECIvtrItem::CreateItem(pItemInfo->type, pItemInfo->expire_date, 1);
						if (pIvtrItem){
							pIvtrItem->SetProcType(pItemInfo->proc_type);
							pIvtrItem->SetItemInfo((BYTE*) &pItemInfo->content, pItemInfo->content_length);
						}
					}
				}
			}
			else if (pInfo->cmd_id == CHAT_GENERALCARD_COLLECTION){
				chat_generalcard_collection* pItemInfo = (chat_generalcard_collection*)data.begin();
				if (data.size() >= sizeof(chat_generalcard_collection)){
					if (pItemInfo->cmd_id == CHAT_GENERALCARD_COLLECTION){
						pIvtrItem = CECIvtrItem::CreateItem(pItemInfo->card_id, 0, 1);						
						if (pIvtrItem){
							pIvtrItem->GetDetailDataFromLocal();
						}
					}
				}
			}			
		}
		return pIvtrItem;
	}
	
	inline PolicyChatParameter* CreatPolicyChatParameter(const Octets &data)
	{
		PolicyChatParameter* result = NULL;
		if (data.size() >= sizeof(chat_item_base)){
			chat_item_base* pInfo = (chat_item_base*)data.begin();
			if (pInfo->cmd_id == CHAT_POLICYCHAT_PARAMETER){
				if (data.size() >= sizeof(chat_policy_parameter)){
					BYTE* pData = (BYTE*) data.begin() + sizeof (chat_policy_parameter);
					result = new PolicyChatParameter();
					result->parameter_mask = ((chat_policy_parameter*)data.begin())->parameter_mask;
					int paraMask = result->parameter_mask;
					if (paraMask & CHAT_PARAMETER_ROLEID){
						if (pData + sizeof(int) > data.end()){
							a_LogOutput(1, "CHAT_S2C::CreatPolicyChatParameter, fail to get roleid data, para mask is %d, data size is %d", paraMask, data.size());
							delete result;
							return NULL;
						}
						result->SetRoleId(*(int*)pData);
						pData += sizeof(int);						
					}
					if (paraMask & CHAT_PARAMETER_LOCALVAL0){
						if (pData + sizeof(int) > data.end()){
							a_LogOutput(1, "CHAT_S2C::CreatPolicyChatParameter, fail to get local variable data, para mask is %d, data size is %d", paraMask, data.size());
							delete result;
							return NULL;
						}
						result->SetLocaValue(*(int*)pData, 0);
						pData += sizeof(int);
					}
					if (paraMask & CHAT_PARAMETER_LOCALVAL1){
						if (pData + sizeof(int) > data.end()){
							a_LogOutput(1, "CHAT_S2C::CreatPolicyChatParameter, fail to get local variable data, para mask is %d, data size is %d", paraMask, data.size());
							delete result;
							return NULL;
						}
						result->SetLocaValue(*(int*)pData, 1);
						pData += sizeof(int);
					}
					if (paraMask & CHAT_PARAMETER_LOCALVAL2){
						if (pData + sizeof(int) > data.end()){
							a_LogOutput(1, "CHAT_S2C::CreatPolicyChatParameter, fail to get local variable data, para mask is %d, data size is %d", paraMask, data.size());
							delete result;
							return NULL;
						}
						result->SetLocaValue(*(int*)pData, 2);
						pData += sizeof(int);
					}
				}
			}
		}
		return result;
	}
}

namespace CHAT_C2S
{

	enum		//CHAT_CMD
	{
		CHAT_EQUIP_ITEM,
		CHAT_GENERALCARD_COLLECTION,
	};

	struct chat_equip_item
	{
		short cmd_id;
		char where;
		short index;
	};
	struct chat_generalcard_collection
	{
		short cmd_id;
		int card_id;
	};
}

#pragma pack()
#endif

