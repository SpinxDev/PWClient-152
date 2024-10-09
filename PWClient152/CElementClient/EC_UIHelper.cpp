// Filename	: EC_UIHelper.cpp
// Creator	: Xu Wenbin
// Date		: 2013/4/11

#include "EC_UIHelper.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_MsgDef.h"
#include "EC_Manager.h"
#include "EC_AutoPolicy.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_FixedMsg.h"
#include "EC_TaskInterface.h"
#include "EC_IvtrItem.h"
#include "EC_World.h"
#include "EC_Instance.h"
#include "EC_AutoTeam.h"
#include "EC_UIConfigs.h"
#include "EC_Shop.h"
#include "EC_Faction.h"
#include "EC_FactionPVP.h"
#include "EC_ShoppingManager.h"
#include "EC_FashionShop.h"
#include "EC_Utility.h"
#include "EC_CrossServer.h"
#include "EC_Split.h"
#include "EC_MsgDataDef.h"

#include "DlgCountryMap.h"
#include "DlgTitleBubble.h"
#include "DlgTitleChallenge.h"
#include "DlgTitleList.h"
#include "DlgHost.h"
#include "DlgPopMsg.h"
#include "DlgQShopBuy.h"
#include "DlgTask.h"
#include "DlgQShop.h"
#include "DlgWorldMap.h"
#include "DlgTaskTrace.h"
#include "DlgAutoLock.h"
#include "DlgFashionShop.h"

#include "TaskTemplMan.h"

#include "elementdataman.h"

#include <AUICTranslate.h>

namespace CECUIHelper
{
	//	class CECQShopUIManager
	//	管理乾坤袋界面、鸿利商城界面、时装商城界面切换显示时，界面显示互斥及商城挂靠与不挂靠NPC状态间切换
	class CECQShopUIManager{
		CDlgFashionShop *	m_pDlgFashionShop;
		
		CDlgQShop *			m_pDlgQShop;
		unsigned int m_QShopNPCOwner;
		
		CDlgQShop *			m_pDlgBackShop;
		unsigned int m_BackShopNPCOwner;
		
		CDlgQShop * GetShopDialogCounterpart(bool bQShop)const{
			return bQShop ? m_pDlgBackShop : m_pDlgQShop;
		}
		void SyncNPCOwnerForAllShop(unsigned int ownerNPC){
			if (m_pDlgQShop->GetShopData().GetOwnerNPCID() != ownerNPC){
				m_pDlgQShop->GetShopData().SetOwnerNPCID(ownerNPC);		//	乾坤袋商城挂靠 NPC 保持一致
			}
			if (m_pDlgBackShop->GetShopData().GetOwnerNPCID() != ownerNPC){
				m_pDlgBackShop->GetShopData().SetOwnerNPCID(ownerNPC);	//	鸿利商城挂靠 NPC 保持一致
			}
		}
		void EndNPCGoldShopService(unsigned int ownerNPCID){
			if (!ownerNPCID && (m_QShopNPCOwner || m_BackShopNPCOwner)){
				CECUIHelper::EndNPCGoldShopService();					//	之前挂靠NPC，但当前不再挂靠NPC时，一定结束NPC服务状态
			}
		}
	public:
		CECQShopUIManager(){
			CECGameUIMan *pGameUIMan = GetGameUIMan();
			m_pDlgFashionShop = dynamic_cast<CDlgFashionShop *>(pGameUIMan->GetDialog("Win_FashionShop"));
			
			m_pDlgQShop = dynamic_cast<CDlgQShop*>(pGameUIMan->GetDialog("Win_QShop"));
			m_QShopNPCOwner = m_pDlgQShop->GetShopData().GetOwnerNPCID();
			
			m_pDlgBackShop = dynamic_cast<CDlgQShop*>(pGameUIMan->GetDialog("Win_BackShop"));
			m_BackShopNPCOwner = m_pDlgBackShop->GetShopData().GetOwnerNPCID();
		}
		void ArrangeForShowFashionShop(bool bQShopFashionShop, unsigned int ownerNPC){
			//	显示时装商城时，对乾坤袋、鸿利商城、NPC元宝商城服务的调整
			if (m_pDlgQShop->IsShow()){
				m_pDlgQShop->ShowFalse();		//	乾坤袋界面一定要隐藏
			}
			if (m_pDlgBackShop->IsShow()){
				m_pDlgBackShop->ShowFalse();	//	鸿利商城界面一定要隐藏
			}
			SyncNPCOwnerForAllShop(ownerNPC);
			EndNPCGoldShopService(ownerNPC);
		}
		void ArrangeForShowShop(bool bQShop, unsigned int ownerNPC){
			//	显示乾坤袋/鸿利商城时，对鸿利商城/乾坤袋、时装商城、NPC元宝商城服务的调整
			if (m_pDlgFashionShop->IsShow()){
				m_pDlgFashionShop->ShowFalse();	//	时装商城界面一定要隐藏
			}
			if (GetShopDialogCounterpart(bQShop)->IsShow()){
				GetShopDialogCounterpart(bQShop)->ShowFalse(true);			//	另一个商城界面一定要隐藏
			}
			SyncNPCOwnerForAllShop(ownerNPC);
			EndNPCGoldShopService(ownerNPC);
		}
	};

	CECGame * GetGame()
	{
		return g_pGame;
	}
	CECGameRun * GetGameRun()
	{
		return GetGame()->GetGameRun();
	}
	CECGameSession * GetGameSession()
	{
		return GetGame()->GetGameSession();
	}
	CECUIManager *	GetUIManager(){
		return GetGameRun()->GetUIManager();
	}
	CECBaseUIMan* GetBaseUIMan()
	{
		return GetUIManager()->GetBaseUIMan();
	}
	CECLoginUIMan * GetLoginUIMan(){
		return GetUIManager()->GetLoginUIMan();
	}
	CECGameUIMan* GetGameUIMan()
	{
		return GetUIManager()->GetInGameUIMan();
	}
	CECHostPlayer* GetHostPlayer()
	{
		return GetGameRun()->GetHostPlayer();
	}
	CECWorld * GetWorld()
	{
		return GetGameRun()->GetWorld();
	}

	bool ShowCountryMap()
	{
		//	根据当前状态判断是否适合显示国战数据并提示
		bool bShown = false;
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI){
			CDlgCountryMap *pDlgCountryMap = dynamic_cast<CDlgCountryMap*>(pGameUI->GetDialog("Win_CountryMap"));
			if (pDlgCountryMap->IsShow()){
				bShown = true;
			}else if (!g_pGame->GetGameSession()->IsConnected()){
			}
			else if (!pDlgCountryMap->IsConfigReady()){
				pDlgCountryMap->GetConfig();
				pGameUI->ShowErrorMsg(9999);
			}else if (!pDlgCountryMap->IsVersionOK()){
				pGameUI->ShowErrorMsg(9998);
			}else{
				pDlgCountryMap->Show(true);
				bShown = true;
			}
		}
		return bShown;
	}
	void AddTitle(unsigned short id, int time)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgTitleBubble* pDlg = dynamic_cast<CDlgTitleBubble*>(pGameUI->GetDialog("Win_TitleBubble"));
			if (pDlg) pDlg->AddTitle(id, time);
		}
	}
	void AddTitleChallenge(unsigned short id)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgTitleChallenge* pDlg = dynamic_cast<CDlgTitleChallenge*>(pGameUI->GetDialog("Win_Event"));
			if (pDlg) pDlg->AddChallenge(id);
		}
	}
	void RemoveTitleChallenge(unsigned short id)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgTitleChallenge* pDlg = dynamic_cast<CDlgTitleChallenge*>(pGameUI->GetDialog("Win_Event"));
			if (pDlg) pDlg->RemoveChallenge(id);
		}
	}
	
	void OnDeliverChallengeTask(unsigned short id)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgTitleChallenge* pDlg = dynamic_cast<CDlgTitleChallenge*>(pGameUI->GetDialog("Win_Event"));
			if (pDlg) pDlg->OnReceiveChallenge(id);
		}
	}
	void OnQueryTitleRe()
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgTitleList* pDlg =  dynamic_cast<CDlgTitleList*>(pGameUI->GetDialog("Win_TitleList"));
			if (pDlg) pDlg->Update();
		}
	}
	void UpdateParallelWorld()
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgHost* pDlg =  dynamic_cast<CDlgHost*>(pGameUI->GetDialog("Win_Hpmpxp"));
			if (pDlg) pDlg->UpdateParallelWorldUI();
		}
	}
	
	void AddHeartBeatHint(const ACHAR* pszMsg)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgPopMsg* pDlg =  dynamic_cast<CDlgPopMsg*>(pGameUI->GetDialog("Win_Popmsg2"));
			if (pDlg) pDlg->Add(pszMsg);
		}
	}
	
	void AutoMoveStartComplex(const A3DVECTOR3 &dst, const int targetId /* 0 */, const int taskId /* 0 */)
	{
		if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
			return;
		MsgDataAutoMove* msgDTTargetId = new MsgDataAutoMove(MSG_TARGETNPC, targetId, taskId);
		g_pGame->GetGameRun()->PostMessage(MSG_HST_AUTOMOVE, MAN_PLAYER, 0, 3, *(DWORD*)&dst.x, *(DWORD*)&dst.y, *(DWORD*)&dst.z, msgDTTargetId);
	}
	void QShopBatchBuy()
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgQShopBuy* pDlg = dynamic_cast<CDlgQShopBuy*>(pGameUI->GetDialog("Win_QshopBuy"));
			if (pDlg) pDlg->BatchBuy();
		}
	}
	ACString FormatCoordText(const ACHAR *szText)
	{
		//	转换文本中的 $id$、@id@ 为可点击的坐标
		int len = a_strlen(szText);
		ACString keyword;
		ACString result = _AL("");
		ACHAR text[3000];
		ACHAR flag;
		a_strcpy(text, szText);
		int i = 0;
		int j = 0;
		int k = 0;
		int m = 0;
		ACString map;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
		while( i < len )
		{
			k = i;
			while( i < len && text[i] != '@' && text[i] != '$')
				i++;
			if( i >= len ) break;
			text[i] = '\0';
			result += text + k;
			k = ++i;
			while (i < len && text[i] != '@' && text[i] != '$')
			{
				i++;
			}
			if (i >= len) break;
			flag = text[i];
			text[i] = '\0';
			keyword.Format(_AL("%s"), text + k);
			int id = keyword.ToInt();		
			ACString strName;
			if (flag == '$')
			{
				DATA_TYPE DataType;
				const void *p = pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
				if (DataType == DT_NPC_ESSENCE){
					strName = ((const NPC_ESSENCE *)p)->name;
				}else if (DataType == DT_MINE_ESSENCE){
					strName = ((const MINE_ESSENCE *)p)->name;
				}
			}else if (flag == '@'){
				DATA_TYPE DataType;
				const void *p = pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
				if( DataType == DT_MONSTER_ESSENCE ){
					strName = ((const MONSTER_ESSENCE *)p)->name;
				}
			}
			if (!strName.IsEmpty()){
				bool bFind = false;
				CECGame::ObjectCoords TargetTemp;
				A3DVECTOR3 vPos = g_pGame->GetGameRun()->GetHostPlayer()->GetObjectCoordinates(id, TargetTemp, bFind);
				if (bFind){
					EditBoxItemBase item(enumEICoord);
					item.SetName(strName);
					ACHAR szInfo[100];
					a_sprintf(szInfo, _AL("%f %f %f %d"), vPos.x, vPos.y, vPos.z, id);
					item.SetInfo(szInfo);
					item.SetColor(A3DCOLORRGB(0, 255, 0));
					strName = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize();
				}
				result += strName;
			}else{
				result += _AL("^00FF00?????^FFFFFF");
			}
			k = i++;
		}
		result += text + k;
		return result;
	}

	void RemoveNameFlagFromNPCChat(const ACHAR *pszChat, ACHAR *pszConv)
	{
		int i, nLen = 0;
		if (!pszChat || !pszConv)
			return;
		
		pszConv[0] = 0;
		for( i = 0; i < (int)a_strlen(pszChat); i++ ){
			if( pszChat[i] == '&' ){
				continue;
			}
			else{
				pszConv[nLen] = pszChat[i];
				nLen++;
			}
		}
		pszConv[nLen] = 0;
	}

	void ReplaceSubString(ACString &srcString, const ACString &oldSubString, const ACString &newSubString)
	{
		int nSub = srcString.Find(oldSubString);
		while (nSub != -1){
			srcString = srcString.Left(nSub) + newSubString + srcString.Right(srcString.GetLength() - nSub - oldSubString.GetLength());
			nSub = srcString.Find(oldSubString);
		}
	}

	bool FindSpecialCharInPolicyChat(const ACString &srcPolicyChat, ACString &subString, ACString &keyInSubString, ACString &variableInSubString)
	{
		bool result(false);
		int posStart = srcPolicyChat.Find(_AL("{"));
		while (posStart != -1){
			int posEnd = srcPolicyChat.Find(_AL("}"), posStart);
			if (posEnd != -1){
				subString = srcPolicyChat.Mid(posStart, posEnd-posStart+1);
				CECSplitHelperW splitter(subString.Mid(1, subString.GetLength()-2), _AL(":"));
				if (splitter.Count() == 2){
					keyInSubString = splitter.ToString(0);
					variableInSubString = splitter.ToString(1);
					result = true;
				}
			}
			break;
		}
		return result;
	}
	
	ACString GetGlobalVariableInPolicyChat(const ACString &variable)
	{
		ACString result;
		result.Format(_AL("%ld"),  g_pGame->GetGameRun()->GetCommonData(variable.ToInt()));
		return result;
	}
	
	ACString GetLocalVariableInPolicyChat(const ACString &variable, const CHAT_S2C::PolicyChatParameter* pPolicyChatPara)
	{
		ACString result;
		int localValue(0);
		if (pPolicyChatPara && pPolicyChatPara->GetLocalValue(variable.ToInt(), localValue)){
			result.Format(_AL("%d"), localValue);
		}else{
			result = _AL("???");
		}
		return result;
	}
	
	void ReplaceSpecialCharInPolicyChat(ACString &srcString, const ACString &oldSubString, const ACString &key, const ACString &variable, const CHAT_S2C::PolicyChatParameter* pPolicyChatPara)
	{
		ACString newSubString;
		if(key == _AL("global_var")){
			newSubString = GetGlobalVariableInPolicyChat(variable);
		}else if (key == _AL("local_var")){
			newSubString = GetLocalVariableInPolicyChat(variable, pPolicyChatPara);
		}else{
			newSubString = _AL("???");
		}
		ReplaceSubString(srcString, oldSubString, newSubString);
	}

	void ReplaceNameInPolicyChat(ACString &srcString, const CHAT_S2C::PolicyChatParameter* pPolicyChatPara)
	{
		const ACHAR *szNameTag = _AL("$name");
		if (srcString.Find(szNameTag) == -1){
			return;
		}
		ACString strName;
		if (pPolicyChatPara && pPolicyChatPara->HasRoleID()){
			strName = ACString(_AL("&")) + pPolicyChatPara->name + _AL("&");
		}else{
			strName = _AL("???");
		}
		ReplaceSubString(srcString, szNameTag, strName);
	}

	ACString PolicySpecialCharReplace(const ACHAR *szText, const CHAT_S2C::PolicyChatParameter* pPolicyChatPara)
	{
		ACString result = szText;
		ReplaceNameInPolicyChat(result, pPolicyChatPara);
		ACString subString, key, variable;
		while (FindSpecialCharInPolicyChat(result, subString, key, variable)){
			ReplaceSpecialCharInPolicyChat(result, subString, key, variable, pPolicyChatPara);
		}
		return result;
	}

	bool FollowCoord(EditBoxItemBase *pItem, int taskId /* taskId = - 1*/)
	{
		bool bOK(false);
		if (pItem && pItem->GetType() == enumEICoord){
			A3DVECTOR3 vPos;
			int id(0);
			if (a_sscanf(pItem->GetInfo(), _AL("%f%f%f%d"), &vPos.x, &vPos.y, &vPos.z, &id) == 4){
				if (id > 0){
					bool bInTable(false);
					CECGame::ObjectCoords targetCoords;
					A3DVECTOR3 vec = g_pGame->GetGameRun()->GetHostPlayer()->GetObjectCoordinates(id, targetCoords, bInTable);
					if (!bInTable && MAJOR_MAP == g_pGame->GetGameRun()->GetWorld()->GetInstanceID()){
						vec = vPos;
					}
					AutoMoveStartComplex(vec, id, taskId);
					ACString strTraceName = pItem->GetName() ? pItem->GetName() : _AL("  ");
					CDlgTask::SetTraceObjects(targetCoords, strTraceName);
					
					bOK = true;
				}else{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_OBJECT_NOT_IN_INSTANCE);
				}
			}
		}
		return bOK;
	}

	bool GetReportPluginResult(ACString &s, int retcode, int roleid, bool bAutoGenName)
	{
		bool bRet(false);
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (const ACHAR * pName = pGameRun->GetPlayerName(roleid, bAutoGenName)){
			if (CECGameUIMan *pGameUI = pGameRun->GetUIManager()->GetInGameUIMan()){
				ACString strFormat = pGameUI->GetStringFromTable(11200+retcode);
				if (strFormat.IsEmpty()){
					s.Format(_AL("unknown retcode=%d for player %d(name=%s)"), retcode, roleid, pName);
				}else{
					s.Format(strFormat, pName);
				}
				bRet = true;
			}
		}
		return bRet;
	}

	bool QShopShowCategory(int mainType, int subType)
	{
		bool bShow(false);
		if (CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()){
			CDlgQShop *pQShop = dynamic_cast<CDlgQShop *>(pGameUI->GetDialog("Win_QShop"));
			bShow = pQShop->ShowCategory(mainType, subType, 0);
		}
		return bShow;
	}

	void OnPushNewTasksCanDeliver()
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) {
			CDlgTask* pTask = dynamic_cast<CDlgTask*>(pGameUI->GetDialog("Win_Quest"));
			if (pTask) pTask->OnTaskPush();
			CDlgTheme* pDlg = dynamic_cast<CDlgTheme*>(pGameUI->GetDialog("Win_Main"));
			if (pDlg) pDlg->StartButtonGfx("Btn_Quest", "界面\\任务提示");
		}		
	}

	A3DVECTOR3 GetTaskObjectCoordinates(int id, bool& in_table)
	{
		A3DVECTOR3 ret(0.f);
		in_table = false;
		CECGame::ObjectCoords TargetTemp;
		ret = g_pGame->GetGameRun()->GetHostPlayer()->GetObjectCoordinates(
						id, TargetTemp, in_table);
		if (!in_table && MAJOR_MAP == g_pGame->GetGameRun()->GetWorld()->GetInstanceID()) {
			ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
			const NPC_INFO* pInfo = pMan->GetTaskNPCInfo(id);
			if(pInfo) {
				ret.Set(pInfo->x, pInfo->z, pInfo->y);
				in_table = true;
			}
		}
		return ret;
	}

	A3DVECTOR3 GetTaskRegionCoordinates(int reach_world, const Task_Region* region)
	{
		A3DVECTOR3 ret(0.f);
		int cur_instance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
		if (cur_instance != reach_world) {
			abase::vector<CECGame::OBJECT_COORD> tempCoord;
			CECInstance *pInstance = g_pGame->GetGameRun()->GetInstance(cur_instance);
			ACString strCurMap = AS2AC(pInstance->GetPath());
			pInstance = g_pGame->GetGameRun()->GetInstance(reach_world);
			ACString mapName = pInstance ? AS2AC(pInstance->GetPath()) : _AL("");
			g_pGame->GetObjectCoord(mapName, tempCoord);
			abase::vector<CECGame::OBJECT_COORD>::iterator iter = std::find(tempCoord.begin(), tempCoord.end(), strCurMap);
			if (iter != tempCoord.end()) ret = iter->vPos;
		} else {
			ret.x = (region->zvMax.x - region->zvMin.x) / 2 + region->zvMin.x;
			ret.y = (region->zvMax.y - region->zvMin.y) / 2 + region->zvMin.y;
			ret.z = (region->zvMax.z - region->zvMin.z) / 2 + region->zvMin.z;
		}
		return ret;
	}

	A3DPOINT2 GetShowPos(float world_x, float world_z)
	{
		A3DPOINT2 p(0, 0);
		p.x = ((int)world_x + 4000) / 10;
		p.y = ((int)world_z + 5500) / 10;
		return p;
	}

	static const char* IMG_TASK_NPC = "完美按钮\\任务.tga";
	static const char* IMG_TASK_NPC_GRAY = "完美按钮\\任务未完成.tga";
	static const char* IMG_TASK_MONSTER = "完美按钮\\怪物图标.tga";
	static const char* IMG_TASK_MINE = "完美按钮\\矿物图标.tga";
	static const char* IMG_TASK_REACH = "完美按钮\\任务地点.tga";
	static const char* IMG_TASK_DAILY = "完美按钮\\任务每日.png";
	static const char* IMG_TASK_BRANCH = "完美按钮\\任务支线.png";
	static const char* IMG_TASK_MAJOR = "完美按钮\\任务主线.png";
	static const char* IMG_TASK_LEVEL2 = "完美按钮\\任务修真.png";

	const char* GetTaskImage(bool has_task, bool can_finish, unsigned long type)
	{
		const char* ret = NULL;
		if (has_task) ret = can_finish ? IMG_TASK_NPC : IMG_TASK_NPC_GRAY;
		else {
			switch(type) {
			case enumTTDaily:
			case enumTTEvent:
				ret = IMG_TASK_DAILY;
				break;
			case enumTTBranch:
				ret = IMG_TASK_BRANCH;
				break;
			case enumTTMajor:
				ret = IMG_TASK_MAJOR;
				break;
			case enumTTLevel2:
				ret = IMG_TASK_LEVEL2;
				break;
			default:
				ret = IMG_TASK_BRANCH;
				break;
			}
		}
		return ret;
	}

	bool ShowTaskPosInMap(int id, bool clear_other)
	{
		ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
		elementdataman *pDataMan = GetGame()->GetElementDataMan();
		CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
		ATaskTempl* pTemp = pMan->GetTaskTemplByID(id);
		CDlgWorldMap* pMap = dynamic_cast<CDlgWorldMap*>(GetGameUIMan()->GetDialog("Win_WorldMap"));
		CDlgWorldMap* pDetail = dynamic_cast<CDlgWorldMap*>(GetGameUIMan()->GetDialog("Win_WorldMapDetail"));
		bool bNeedShowMap(false);
		if (pMap && pDetail && pTemp) {
			// clear 之前加入的控件
			if (clear_other) {
				pMap->ClearControls();
				pDetail->ClearControls();
			}
			// 显示NPC所在位置
			bool bActiveTask = pTask->HasTask(id);
			bool bCanFinish = pTask->CanFinishTask(id);
			int nANPC = bActiveTask ? pTemp->GetAwardNPC() : pTemp->GetDeliverNPC();
			A3DVECTOR3 vPos(0.f);
			if (nANPC > 0) {
				bool bFind = false;
				vPos = GetTaskObjectCoordinates(nANPC, bFind);
				if (bFind) {
					bNeedShowMap = true;
					const char* strImg = GetTaskImage(bActiveTask, bCanFinish, pTemp->m_ulType);
					pMap->AddAutoMoveButton(CDlgTask::GetTaskNameWithColor(pTemp), strImg, vPos, bActiveTask ? id : 0);
					pDetail->AddAutoMoveButton(CDlgTask::GetTaskNameWithColor(pTemp), strImg, vPos, bActiveTask ? id : 0);
				}
	
				if (bCanFinish || !bActiveTask) return bNeedShowMap;
			}
			Task_State_info tsi;
			pTask->GetTaskStateInfo(id, &tsi, bActiveTask);
			ACString strHint;
			AString strImg;
			// 显示掉落任务物品的怪物或矿物的位置
			int j = 0;
			for(j = 0; j < MAX_ITEM_WANTED; j++ ) {
				int itemid = tsi.m_ItemsWanted[j].m_ulItemId;
				if( itemid <= 0 )
					break;
				int search_id(0);
				if (tsi.m_ItemsWanted[j].m_ulItemsToGet > 0) {
					CECIvtrItem* pItem = CECIvtrItem::CreateItem(itemid, 0, 1);
					if (pItem) {
						int monid = tsi.m_ItemsWanted[j].m_ulMonsterId;
						
						if (pTemp->m_enumMethod != enumTMKillPlayer) {
							if(monid > 0) {
								search_id = monid;
								strImg = IMG_TASK_MONSTER;
							} else {
								const MINE_ESSENCE* pMine = CDlgTask::SearchTaskMine(id);
								if(pMine) {
									search_id = pMine->id;
									strImg = IMG_TASK_MINE;
								}
							}
							strHint = pItem->GetName();
						}		
						delete pItem;
					}
					if (search_id > 0) {
						bool bFind = false;
						A3DVECTOR3 vPos = GetTaskObjectCoordinates(search_id, bFind);
						if (bFind) {
							pMap->AddAutoMoveButton(strHint, strImg, vPos, id);
							pDetail->AddAutoMoveButton(strHint, strImg, vPos, id);
							bNeedShowMap = true;
						}
					}
				}
			}
			// 显示所需杀怪的位置
			for (j = 0; j < MAX_MONSTER_WANTED; j++) {
				int monid = tsi.m_MonsterWanted[j].m_ulMonsterId;			
				if (monid == 0)
					break;
				int search_id(0);
				int tokill = tsi.m_MonsterWanted[j].m_ulMonstersToKill;
				if(tokill > 0 ) {
					DATA_TYPE DataType = DT_INVALID;
					MONSTER_ESSENCE *pMonster = (MONSTER_ESSENCE *)pDataMan->get_data_ptr(monid, ID_SPACE_ESSENCE, DataType);
					
					if( DataType == DT_MONSTER_ESSENCE ) {
						strHint = pMonster->name;
						strImg = IMG_TASK_MONSTER;
						search_id = pMonster->id;
					}
					if (search_id > 0) {
						bool bFind = false;
						A3DVECTOR3 vPos = GetTaskObjectCoordinates(search_id, bFind);
						if (bFind) {
							pMap->AddAutoMoveButton(strHint, strImg, vPos, id);
							pDetail->AddAutoMoveButton(strHint, strImg, vPos, id);
							bNeedShowMap = true;
						}
					}
				}
			}
			// 显示任务达到地点的位置
			if(pTemp->m_ulReachSiteCnt > 0) {
				A3DVECTOR3 pos = GetTaskRegionCoordinates((int)pTemp->m_ulReachSiteId, &pTemp->m_pReachSite[0]);
				if (!pos.IsZero()) {
					A3DPOINT2 p = GetShowPos(pos.x, pos.z);
					strHint.Format(GetGameUIMan()->GetStringFromTable(9393), p.x, p.y);
					pMap->AddAutoMoveButton(strHint, IMG_TASK_REACH, pos, id);
					pDetail->AddAutoMoveButton(strHint, IMG_TASK_REACH, pos, id);	
					bNeedShowMap = true;
				}
			}
		}
		return bNeedShowMap;
	}
	
	bool ShowOneTaskInMap(int id)
	{
		CDlgWorldMap* pMap = dynamic_cast<CDlgWorldMap*>(GetGameUIMan()->GetDialog("Win_Bigmap"));
		if (pMap && ShowTaskPosInMap(id, true)) {
			pMap->SetTaskShowType(CDlgWorldMap::TASK_TYPE_ONE);
			return true;
		}
		return false;
	}
	
	void ShowTasksInMap(const abase::vector<int>& tasks)
	{
		size_t count = tasks.size();
		for (size_t i = 0; i < count; ++i)
			ShowTaskPosInMap(tasks[i], false);
	}

	void OnTaskProcessUpdated(int id)
	{
		CDlgTask* pTask = dynamic_cast<CDlgTask*>(GetGameUIMan()->GetDialog("Win_Quest"));
		if (pTask) pTask->OnTaskProcessUpdated(id);
	}

	void OnTaskItemGained(int item_id)
	{
		CDlgTask* pTask = dynamic_cast<CDlgTask*>(GetGameUIMan()->GetDialog("Win_Quest"));
		if (pTask) pTask->OnTaskItemGained(item_id);
	}

	bool IsOnSameDay(time_t t0, time_t t1)
	{
		bool bIs(false);
		if (t0 == t1){
			bIs = true;
		}else{
			CECGame *pGame = GetGame();
			tm g0 = pGame->GetServerLocalTime(t0);
			tm g1 = pGame->GetServerLocalTime(t1);
			if (g0.tm_year == g1.tm_year &&
				g0.tm_mon == g1.tm_mon &&
				g0.tm_mday == g1.tm_mday){
				bIs = true;
			}
		}
		return bIs;
	}

	void OnTaskCompleted(int id)
	{
		CECAutoTeam* pAutoTeam = g_pGame->GetGameRun()->GetHostPlayer()->GetAutoTeam();
		if( pAutoTeam->GetCurGoal() == id )
			pAutoTeam->Cancel(CECAutoTeam::TYPE_TASK);
		CDlgTaskTrace* pDlg = 
			dynamic_cast<CDlgTaskTrace*>(GetGameUIMan()->GetDialog("Win_QuestMinion"));
		if (pDlg) pDlg->OnTaskCompleted(id);
	}
	
	void CheckBuyPokerShop(CDlgBase *pDlg, const char *szObjName)
	{
		if (pDlg){
			if (PAUIOBJECT pObj = pDlg->GetDlgItem("Btn_CardBuy")){
				pObj->Show(CECUIConfig::Instance().GetGameUI().bEnableRandShop);
				pObj->Enable(GetHostPlayer()->GetMaxLevelSofar() >= CECUIConfig::Instance().GetGameUI().nPokerShopLevelLimit);
				pDlg->DisableForCrossServer("Btn_CardBuy");
				if (pObj->IsShow() && pObj->IsEnabled()){
					if (!pObj->GetData()){
						pObj->SetData(1);
						pObj->SetFlash(true);
					}
				}
			}
		}
	}

	void ShowPokerShop(AUIObject *pObj)
	{
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		if (PAUIDIALOG pDlg = pGameUIMan->GetDialog("Win_GeneralCardBuy")){
			if (!pDlg->IsShow()){
				CDlgAutoLock *pDlgAutoLock = dynamic_cast<CDlgAutoLock *>(pGameUIMan->GetDialog("Win_AutoLock"));
				if (pDlgAutoLock->IsLocked()){
					GetGameRun()->AddFixedChannelMsg(FIXMSG_INLOCK, GP_CHAT_MISC);
				}else{
					pDlg->Show(true);
					if (pObj){
						pObj->SetFlash(false);
					}
				}
			}else{
				pDlg->OnCommand("IDCANCEL");
				if (pObj){
					pObj->SetFlash(false);
				}
			}
		}
	}

	void QShopSetCash(AUIObject *pObj)
	{
		if (pObj){
			GetGameUIMan()->SetCashText(pObj, CECQShop::Instance().GetCash());
		}
	}

	void SetFactionName(int idFaction, AUIObject *pObj)
	{
		if (pObj){
			if (idFaction > 0){
				if (Faction_Info *pInfo = GetGame()->GetFactionMan()->GetFaction(idFaction)){
					pObj->SetText(pInfo->GetName());
				}else{
					pObj->SetText(_AL(""));
				}
			}else{
				pObj->SetText(GetGameUIMan()->GetStringFromTable(251));
			}
		}
	}
	
	bool FactionPVPCanShowResourceInMap()
	{
		return CECFactionPVPModel::Instance().CanShowResource() && GetWorld()->IsMajorMap();
	}

	ACString HintForGShopItem(const GSHOP_ITEM *pGShopItem){
		AUICTranslate trans;
		ACString strHint = trans.Translate(pGShopItem->desc);
		strHint.TrimLeft(_AL("\r"));
		strHint.TrimRight(_AL("\r"));
		ACString strID;
		if (GetGame()->GetConfigs()->GetShowIDFlag()){
			if (pGShopItem->idGift > 0 && pGShopItem->iGiftNum > 0){
				strID.Format(_AL("(%d,%d)"), pGShopItem->id, pGShopItem->idGift);
			}else{
				strID.Format(_AL("(%d)"), pGShopItem->id);
			}
		}
		strHint = ACString(pGShopItem->szName) + strID + _AL("\r\r") + strHint;
		return strHint;
	}
	
	ACString NameForGShopItem(const GSHOP_ITEM *pGShopItem){
		ACString strID;
		if (GetGame()->GetConfigs()->GetShowIDFlag()){
			if (pGShopItem->idGift > 0 && pGShopItem->iGiftNum > 0){
				strID.Format(_AL("(%d,%d)"), pGShopItem->id, pGShopItem->idGift);
			}else{
				strID.Format(_AL("(%d)"), pGShopItem->id);
			}
		}
		return ACString(pGShopItem->szName) + strID;
	}

	ACString HintForShoppingItem(int tid, int expireDate, int count){
		ACString strHint;
		if (CECIvtrItem *pItem = CECIvtrItem::CreateItem(tid, expireDate, count)){
			pItem->GetDetailDataFromLocal();
			pItem->SetPriceScale(CECIvtrItem::SCALE_BUY, 0.0f);
			strHint = pItem->GetDesc();
			if (!strHint.IsEmpty()){
				AUICTranslate trans;
				strHint = trans.Translate(strHint);
			}
			delete pItem;
		}
		return strHint;
	}

	void AlignByCenter(AUIDialog *p, AUIDialog *refDialog){
		if (!p || !refDialog || p == refDialog){
			return;
		}
		APointI center = p->GetRect().CenterPoint();
		APointI refCenter = refDialog->GetRect().CenterPoint();
		APointI delta = refCenter - center;
		POINT pos = p->GetPos();
		p->SetPosEx(pos.x+delta.x, pos.y+delta.y);
	}

	ACString MarshalLinkedItem(CECIvtrItem *pItem, bool bNeedInfo){
		ACString result;
		//构建EditBoxItemBase对象
		ACString strName;
		A3DCOLOR clrName;
		GetGameUIMan()->TransformNameColor(pItem, strName, clrName);
		
		EditBoxItemBase* pEditboxItem = new EditBoxItemBase(enumEIIvtrlItem);
		pEditboxItem->SetColor(clrName);
		pEditboxItem->SetName(strName);
		if(bNeedInfo){
			ACString strInfo;
			strInfo.Format(_AL("%s%s"), A3DCOLOR_TO_STRING(clrName), strName);
			pEditboxItem->SetInfo(strInfo);
		}
		result = (ACHAR)AUICOMMON_ITEM_CODE_START + pEditboxItem->Serialize();		
		
		delete pEditboxItem;
		return result;
	}

	bool CheckDialogName(AUIDialog *p, const char *szNameToMatch){
		return p != NULL && szNameToMatch != NULL && !stricmp(p->GetName(), szNameToMatch);
	}

	ACString TrimColorPrefix(const ACHAR *szName){
		ACString result = szName;
		const int COLOR_PREFIX_LENGTH = strlen("^ffffff");
		if (result.GetLength() >= COLOR_PREFIX_LENGTH && result[0] == '^') {
			bool bColorPreFix = true;
			for (int i = 1; i < COLOR_PREFIX_LENGTH; i++) {
				if (result[i] < 'a' && result[i] > 'f' && result[i] < 'A' && result[i] > 'F' &&
					result[i] < '0' && result[i] > '9') {
					bColorPreFix = false;
					break;
				}
			}
			if (bColorPreFix) {
				result = result.Right(result.GetLength() - COLOR_PREFIX_LENGTH);
			}
		}
		return result;
	}

	const char * GetShopDialogName(bool bQShop){
		return bQShop ? "Win_QShop" : "Win_BackShop";
	}

	CDlgQShop * GetShopDialog(bool bQShop){
		return dynamic_cast<CDlgQShop*>(GetGameUIMan()->GetDialog(GetShopDialogName(bQShop)));
	}

	CDlgQShop * GetShopDialogCounterpart(bool bQShop){
		return GetShopDialog(!bQShop);
	}
	
	bool CanShowShop(bool bQShop){
		const CECShopBase *pShop = &CECQShop::Instance();
		if (!bQShop){
			pShop = &CECBackShop::Instance();
		}
		if (pShop->GetCount() == 0){
			return false;
		}		
		if (GetGame()->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient()){
			return false;
		}
		if (CECCrossServer::Instance().IsOnSpecialServer()){
			return false;
		}
		if (!pShop->ValidateTimeStamp()){
			return false;
		}
		return true;
	}
	
	bool ShowFashionShop(bool bQShop, AUIDialog * alignDialog/* = NULL*/, unsigned int ownerNPC/* = 0*/){
		if (!CanShowShop(bQShop)){
			return false;
		}
		CECQShopUIManager qshopUIManager;
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		CECShoppingCart *pShoppingCart = NULL;
		CECFashionShop *pFashionShop = NULL;
		if (bQShop){
			pShoppingCart = CECShoppingManager::Instance().QShopFashionShoppingCart();
			pFashionShop = CECFashionShopManager::Instance().QShopFashionShop();
		}else{
			pShoppingCart = CECShoppingManager::Instance().BackShopFashionShoppingCart();
			pFashionShop = CECFashionShopManager::Instance().BackShopFashionShop();
		}
		CDlgFashionShop *pDlgFashionShop = dynamic_cast<CDlgFashionShop *>(pGameUIMan->GetDialog("Win_FashionShop"));
		if (!pDlgFashionShop->SetModel(pShoppingCart, pFashionShop)){
			return false;
		}
		pFashionShop->Shop()->SetOwnerNPCID(ownerNPC);
		if (!pDlgFashionShop->IsShow()){
			pDlgFashionShop->Show(true);
		}else{
			pGameUIMan->BringWindowToTop(pDlgFashionShop);
		}
		if (alignDialog){
			AlignByCenter(pDlgFashionShop, alignDialog);
		}
		qshopUIManager.ArrangeForShowFashionShop(bQShop, ownerNPC);
		return true;
	}

	bool ShowShop(bool bQShop, unsigned int ownerNPCID, bool bEnableShopSwitch){
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		if (GetGame()->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient()){
			pGameUIMan->ShowErrorMsg(10713);
			return false;
		}
		if (!pGameUIMan->GetDialog("Win_Map")->GetDlgItem("Btn_TreasureBag")->IsEnabled()){
			return false;	//	因为跨服等原因被禁止
		}
		CDlgQShop * pDlgShopToShow = GetShopDialog(bQShop);
		if (pDlgShopToShow->GetShopData().GetCount() == 0){
			return false;
		}
		if (!pDlgShopToShow->GetShopData().ValidateTimeStamp()){
			pGameUIMan->ShowErrorMsg(817);
			return false;
		}
		CECQShopUIManager qshopUIManager;
		pDlgShopToShow->GetShopData().SetOwnerNPCID(ownerNPCID);
		pDlgShopToShow->EnableShopSwitch(bEnableShopSwitch);
		CDlgQShop * pDlgShopCounterpart = GetShopDialogCounterpart(bQShop);
		if (pDlgShopCounterpart->IsShow()){
			POINT pos = pDlgShopCounterpart->GetPos();
			pDlgShopToShow->SetPosEx(pos.x, pos.y);
		}
		if (!pDlgShopToShow->IsShow()){
			pDlgShopToShow->Show(true);
			pDlgShopToShow->GetShopData().GetFromServer(0, 0);
		}else{
			pGameUIMan->BringWindowToTop(pDlgShopToShow);
		}
		qshopUIManager.ArrangeForShowShop(bQShop, ownerNPCID);
		return true;
	}
	
	bool ShowQShop(unsigned int ownerNPCID, bool bEnableBackShop){
		return ShowShop(true, ownerNPCID, bEnableBackShop);
	}

	bool ShowBackShop(unsigned int ownerNPCID, bool bEnableQShop){
		return ShowShop(false, ownerNPCID, bEnableQShop);
	}

	bool ShowShop(bool bQShop){
		CDlgQShop * pDlgShopToShow = GetShopDialog(bQShop);
		return ShowShop(bQShop, pDlgShopToShow->GetShopDataConst().GetOwnerNPCID(), pDlgShopToShow->IsShopSwitchEnabled());
	}

	void EndNPCGoldShopService(){
		GetGameUIMan()->EndNPCService();
		GetShopDialog(true)->ClearOwnerNPC();
		GetShopDialog(false)->ClearOwnerNPC();
	}
}
