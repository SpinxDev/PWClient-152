// File		: EC_QuickBuyPopPolicy.h
// Creator	: Xu Wenbin
// Date		: 2013/12/19

#pragma once

#include <ABaseDef.h>

//	物品购买策略：购买何种物品、显示什么消息、购买是否需要确认、购买成功处理
class CECShopBase;
class CECShopSearchPolicyBase;
class CECQuickBuyPopBuyItemPolicy
{
	int		m_recommendMessage;	//	推荐购买时显示的消息
	int		m_recommendItem;	//	推荐购买的物品
	bool	m_needConfirm;		//	购买时是否需要再次确认
	bool	m_needFlash;		//	弹出是否需闪烁提示

public:
	CECQuickBuyPopBuyItemPolicy(int recommendMessage, int recommendItem, bool needConfirm, bool needFlash);
	virtual ~CECQuickBuyPopBuyItemPolicy();

	int  GetItemID()const;
	void GetItem(int &recommendMessage, int &recommendItem, bool &needConfirm, bool &needFlash)const;
	
	virtual CECShopBase * GetShopData();
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID);
};

//	弹出策略：何时弹出
class AIniFile;
class CECQuickBuyPopTriggerPolicy
{
	bool		m_bTriggered;
	bool		m_bItemGained;
	int			m_chkHideTime;				//	上次点击 Chk_Hide 的服务器时刻
	DWORD		m_dwNextTriggerTime;		//	用于冷却控制（无 Tick 时只有使用时间）
	char *		m_szHideReason;				//	本次隐藏原因，用于后续策略控制

protected:
	CECQuickBuyPopBuyItemPolicy *m_pBuyItemPolicy;

protected:
	virtual bool CanTriggerCheckMore()const=0;	//	子类中增加的触发条件检查
	virtual const char *GetConfigName()const=0;	//	配置名称
	virtual void RestartMore(){}
	virtual void OnShowMore(){}
	virtual void OnHideMore(){}
	virtual void OnNoItemMore(){}
	virtual void OnNoPopDialogMore(){}
	virtual bool CanHideMore(const char *&szHideReason)const{ return false; }
	virtual void OnItemAppearInPackMore(){}
	virtual void SaveConfigMore(AIniFile &){}	//	保存配置（如玩家点击控制要求今天不再弹出）
	virtual void LoadConfigMore(AIniFile &){}	//	加载上述配置

	const char * GetHideReason()const{ return m_szHideReason; }

public:
	CECQuickBuyPopTriggerPolicy(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy);
	virtual ~CECQuickBuyPopTriggerPolicy(){}

	void Restart();							//	重置：重新开始
	void OnShow();							//	弹出
	void OnHide();							//	隐藏（弹出失败、购买成功、玩家取消购买）
	void OnNoItem();						//	弹出时找不到符合要求物品（商城无物品、物品当前状态不可用等）
	void OnNoPopDialog();					//	弹出时找不到可用的配套界面

	bool CanTrigger()const;					//	能否触发弹出

	bool CanHide(const char *&szHideReason)const;//	触发隐藏（如玩家点击控制要求今天不再弹出、物品已经得到）
	virtual bool NeedCheckHide()const=0;	//	是否有限制弹出选项
	void OnCheckHide();						//	玩家点击限制弹出控件

	void OnItemAppearInPack(int itemID, int iSlot);	//	有物品在包裹中出现

	void SetHideReason(const char *szReason);

	void SaveConfig(AIniFile &);			//	保存配置（如玩家点击控制要求今天不再弹出）
	void LoadConfig(AIniFile &);			//	加载上述配置
};

//	对话框未购买时隐藏策略：倒计时隐藏、还是其它界面上操作触发隐藏
class AUIDialog;
class CECQuickBuyPopHidePolicy
{
public:
	virtual ~CECQuickBuyPopHidePolicy(){}
	
	virtual void Restart()=0;
	virtual void TickOnShow(AUIDialog *pDlg)=0;
	virtual bool CanHide(const char *&szHideReason)=0;
};

//	对话框显示对齐策略：居中、与哪个对话框什么要素对齐
class AUIDialog;
class CECQuickBuyPopAlignPolicy
{
public:
	virtual ~CECQuickBuyPopAlignPolicy(){}	
	virtual void OnShow(AUIDialog *pDlg){}
};

//	提供某个弹出策略的对外接口
class AUIDialog;
class AString;
enum QuickBuyPopConfigID
{
	QUICK_BUY_POP_INVALID,		//	无效
	QUICK_BUY_POP_SAFE_LIFE,	//	保命丸
	QUICK_BUY_POP_ACTIVITY,		//	即刻前往活动地点
};
class CECQuickBuyPopPolicies 
{
	QuickBuyPopConfigID			id;
	CECQuickBuyPopBuyItemPolicy	*pBuyItemPolicy;
	CECQuickBuyPopTriggerPolicy *pTriggerPolicy;
	CECQuickBuyPopHidePolicy	*pHidePolicy;
	CECQuickBuyPopAlignPolicy	*pAlignPolicy;
	
	const char * GetConfigFileDir();
	AString GetConfigFilePath(int playerID);
	void SaveConfigs();
	void LoadConfigs();

public:
	CECQuickBuyPopPolicies()
		: id(QUICK_BUY_POP_INVALID)
		, pBuyItemPolicy(0)
		, pTriggerPolicy(0)
		, pHidePolicy(0)
		, pAlignPolicy(0){
	}
	CECQuickBuyPopPolicies(QuickBuyPopConfigID ID, CECQuickBuyPopBuyItemPolicy *buy, CECQuickBuyPopTriggerPolicy *trigger, CECQuickBuyPopHidePolicy *hide, CECQuickBuyPopAlignPolicy *align)
		: id(ID)
		, pBuyItemPolicy(buy)
		, pTriggerPolicy(trigger)
		, pHidePolicy(hide)
		, pAlignPolicy(align){
		LoadConfigs();
	}

	bool IsValid()const{
		return id != QUICK_BUY_POP_INVALID
			&& pBuyItemPolicy != 0
			&& pTriggerPolicy != 0
			&& pHidePolicy != 0
			&& pAlignPolicy != 0;
	}

	void Release(){
		delete pBuyItemPolicy;
		pBuyItemPolicy = 0;

		delete pTriggerPolicy;
		pTriggerPolicy = 0;

		delete pHidePolicy;
		pHidePolicy = 0;

		delete pAlignPolicy;
		pAlignPolicy = 0;

		id = QUICK_BUY_POP_INVALID;
	}
	
	QuickBuyPopConfigID GetID()const{ return id; }
	bool GetItem(int &recommendMessage, int &recommendItem, bool &needConfirm, bool &needFlash){
		bool bGet(false);
		if (pBuyItemPolicy){
			pBuyItemPolicy->GetItem(recommendMessage, recommendItem, needConfirm, needFlash);
			bGet = true;
		}
		return bGet;
	}
	CECShopBase * GetShopData(){
		return pBuyItemPolicy ? pBuyItemPolicy->GetShopData() : 0;
	}
	CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID){
		return pBuyItemPolicy ? pBuyItemPolicy->GetShopSearchPolicy(itemID) : 0;
	}
	bool NeedCheckHide()const{
		return pTriggerPolicy ? pTriggerPolicy->NeedCheckHide() : false;
	}
	bool CanTrigger()const{
		return pTriggerPolicy ? pTriggerPolicy->CanTrigger() : false;
	}
	bool CanHide(const char *&szHideReason)const{
		return pHidePolicy && pHidePolicy->CanHide(szHideReason)
			|| pTriggerPolicy && pTriggerPolicy->CanHide(szHideReason);
	}
	void SetHideReason(const char *szHideReason){
		if (pTriggerPolicy) pTriggerPolicy->SetHideReason(szHideReason);
	}
	void Restart(){
		if (pTriggerPolicy) pTriggerPolicy->Restart();
		if (pHidePolicy)	pHidePolicy->Restart();
	}
	void OnShow(AUIDialog *pDlg){
		if (pTriggerPolicy) pTriggerPolicy->OnShow();
		if (pAlignPolicy)	pAlignPolicy->OnShow(pDlg);
	}
	void OnHide(){
		if (pTriggerPolicy) pTriggerPolicy->OnHide();
		SaveConfigs();
	}
	void OnNoItem(){
		if (pTriggerPolicy) pTriggerPolicy->OnNoItem();
	}
	void OnNoPopDialog(){
		if (pTriggerPolicy) pTriggerPolicy->OnNoPopDialog();
	}
	void TickOnShow(AUIDialog *pDlg){
		if (pHidePolicy)	pHidePolicy->TickOnShow(pDlg);
	}
	void OnCheckHide(){
		if (pTriggerPolicy) pTriggerPolicy->OnCheckHide();
	}
	void OnItemAppearInPack(int itemID, int iSlot){
		if (pTriggerPolicy) pTriggerPolicy->OnItemAppearInPack(itemID, iSlot);
	}
};