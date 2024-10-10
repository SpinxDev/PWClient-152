// Filename	: EC_UIHelper.h
// Creator	: Xu Wenbin
// Date		: 2013/4/11

#pragma once

#include <ABaseDef.h>
#include <AAssist.h>
#include <A3DTypes.h>
#include "vector.h"
#include "chatdata.h"

class A3DVECTOR3;
class EditBoxItemBase;
class CECGame;
class CECGameRun;
class CECGameSession;
class CECUIManager;
class CECBaseUIMan;
class CECLoginUIMan;
class CECGameUIMan;
class CECHostPlayer;
class CECWorld;
struct Task_Region;
class CDlgBase;
class AUIObject;
class AUIDialog;
class CECIvtrItem;
class CDlgQShop;

struct _GSHOP_ITEM;
typedef struct _GSHOP_ITEM GSHOP_ITEM;

//	提供对游戏界面的全局操作，不放在 EC_GameUIMan.h 里以提高增量编译速度
namespace CECUIHelper
{
	CECGame *		GetGame();
	CECGameRun *	GetGameRun();
	CECGameSession *GetGameSession();
	CECBaseUIMan *	GetBaseUIMan();
	CECUIManager *	GetUIManager();
	CECLoginUIMan * GetLoginUIMan();
	CECGameUIMan*	GetGameUIMan();
	CECHostPlayer*	GetHostPlayer();
	CECWorld *		GetWorld();

	void AddTitle(unsigned short id, int time);
	void AddTitleChallenge(unsigned short id);
	void RemoveTitleChallenge(unsigned short id);
	void OnDeliverChallengeTask(unsigned short id);
	void OnQueryTitleRe();
	
	bool ShowCountryMap();
	void UpdateParallelWorld();
	void AddHeartBeatHint(const ACHAR* pszMsg);
	void AutoMoveStartComplex(const A3DVECTOR3 &dst, const int targetId = 0, int taskId = 0);	
	bool GetReportPluginResult(ACString &s, int retcode, int roleid, bool bAutoGenName);
	void SetFactionName(int idFaction, AUIObject *pObj);

	ACString FormatCoordText(const ACHAR *szText);
	
	void RemoveNameFlagFromNPCChat(const ACHAR *pszChat, ACHAR *pszConv);
	//策略喊话中原始发言信息中需进行替换的字符检测，特殊占位字符检测
	void ReplaceSubString(ACString &srcString, const ACString &oldSubString, const ACString &newSubString);
	bool FindSpecialCharInPolicyChat(const ACString &srcPolicyChat, ACString &subString, ACString &keyInSubString, ACString &variableInSubString);
	ACString PolicySpecialCharReplace(const ACHAR *szText, const CHAT_S2C::PolicyChatParameter* pPolicyChatPara);
	bool FollowCoord(EditBoxItemBase *pItem, int taskId = -1);

	void QShopBatchBuy();
	bool QShopShowCategory(int mainType, int subType);
	void QShopSetCash(AUIObject *pObj);
	bool CanShowShop(bool bQShop);

	void OnPushNewTasksCanDeliver(); // 有新的可接任务时推送给玩家
	A3DVECTOR3 GetTaskObjectCoordinates(int id, bool& in_table);
	A3DVECTOR3 GetTaskRegionCoordinates(int reach_world, const Task_Region* region);
	const char* GetTaskImage(bool has_task, bool can_finish, unsigned long type);
	A3DPOINT2 GetShowPos(float world_x, float world_z);
	bool ShowTaskPosInMap(int id, bool clear_other = false);
	bool ShowOneTaskInMap(int id);
	void ShowTasksInMap(const abase::vector<int>& tasks);
	void OnTaskProcessUpdated(int id);
	void OnTaskItemGained(int item_id);
	bool IsOnSameDay(time_t t0, time_t t1);
	void OnTaskCompleted(int id);

	void CheckBuyPokerShop(CDlgBase *pDlg, const char *szObjName);
	void ShowPokerShop(AUIObject *pObj);

	bool FactionPVPCanShowResourceInMap();
	bool ShowFashionShop(bool bQShop, AUIDialog * alignDialog = NULL, unsigned int ownerNPC = 0);
	ACString HintForGShopItem(const GSHOP_ITEM *pGShopItem);
	ACString NameForGShopItem(const GSHOP_ITEM *pGShopItem);
	ACString HintForShoppingItem(int tid, int expireDate, int count);

	void AlignByCenter(AUIDialog *p, AUIDialog *refDialog);
	ACString MarshalLinkedItem(CECIvtrItem *pItem, bool bNeedInfo); //通过物品获取在聊天内容中的可链接物品序列号内容
	bool CheckDialogName(AUIDialog *p, const char *szNameToMatch);
	ACString TrimColorPrefix(const ACHAR *szName);

	const char * GetShopDialogName(bool bQShop);
	CDlgQShop *  GetShopDialog(bool bQShop);
	CDlgQShop *  GetShopDialogCounterpart(bool bQShop);
	bool ShowShop(bool bQShop, unsigned int ownerNPCID, bool bEnableShopSwitch);
	bool ShowQShop(unsigned int ownerNPCID, bool bEnableBackShop);
	bool ShowBackShop(unsigned int ownerNPCID, bool bEnableQShop);
	bool ShowShop(bool bQShop);
	void EndNPCGoldShopService();
};
