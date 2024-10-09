// File		: EC_QuickBuyPopPolicy.h
// Creator	: Xu Wenbin
// Date		: 2013/12/19

#pragma once

#include <ABaseDef.h>

//	��Ʒ������ԣ����������Ʒ����ʾʲô��Ϣ�������Ƿ���Ҫȷ�ϡ�����ɹ�����
class CECShopBase;
class CECShopSearchPolicyBase;
class CECQuickBuyPopBuyItemPolicy
{
	int		m_recommendMessage;	//	�Ƽ�����ʱ��ʾ����Ϣ
	int		m_recommendItem;	//	�Ƽ��������Ʒ
	bool	m_needConfirm;		//	����ʱ�Ƿ���Ҫ�ٴ�ȷ��
	bool	m_needFlash;		//	�����Ƿ�����˸��ʾ

public:
	CECQuickBuyPopBuyItemPolicy(int recommendMessage, int recommendItem, bool needConfirm, bool needFlash);
	virtual ~CECQuickBuyPopBuyItemPolicy();

	int  GetItemID()const;
	void GetItem(int &recommendMessage, int &recommendItem, bool &needConfirm, bool &needFlash)const;
	
	virtual CECShopBase * GetShopData();
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID);
};

//	�������ԣ���ʱ����
class AIniFile;
class CECQuickBuyPopTriggerPolicy
{
	bool		m_bTriggered;
	bool		m_bItemGained;
	int			m_chkHideTime;				//	�ϴε�� Chk_Hide �ķ�����ʱ��
	DWORD		m_dwNextTriggerTime;		//	������ȴ���ƣ��� Tick ʱֻ��ʹ��ʱ�䣩
	char *		m_szHideReason;				//	��������ԭ�����ں������Կ���

protected:
	CECQuickBuyPopBuyItemPolicy *m_pBuyItemPolicy;

protected:
	virtual bool CanTriggerCheckMore()const=0;	//	���������ӵĴ����������
	virtual const char *GetConfigName()const=0;	//	��������
	virtual void RestartMore(){}
	virtual void OnShowMore(){}
	virtual void OnHideMore(){}
	virtual void OnNoItemMore(){}
	virtual void OnNoPopDialogMore(){}
	virtual bool CanHideMore(const char *&szHideReason)const{ return false; }
	virtual void OnItemAppearInPackMore(){}
	virtual void SaveConfigMore(AIniFile &){}	//	�������ã�����ҵ������Ҫ����첻�ٵ�����
	virtual void LoadConfigMore(AIniFile &){}	//	������������

	const char * GetHideReason()const{ return m_szHideReason; }

public:
	CECQuickBuyPopTriggerPolicy(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy);
	virtual ~CECQuickBuyPopTriggerPolicy(){}

	void Restart();							//	���ã����¿�ʼ
	void OnShow();							//	����
	void OnHide();							//	���أ�����ʧ�ܡ�����ɹ������ȡ������
	void OnNoItem();						//	����ʱ�Ҳ�������Ҫ����Ʒ���̳�����Ʒ����Ʒ��ǰ״̬�����õȣ�
	void OnNoPopDialog();					//	����ʱ�Ҳ������õ����׽���

	bool CanTrigger()const;					//	�ܷ񴥷�����

	bool CanHide(const char *&szHideReason)const;//	�������أ�����ҵ������Ҫ����첻�ٵ�������Ʒ�Ѿ��õ���
	virtual bool NeedCheckHide()const=0;	//	�Ƿ������Ƶ���ѡ��
	void OnCheckHide();						//	��ҵ�����Ƶ����ؼ�

	void OnItemAppearInPack(int itemID, int iSlot);	//	����Ʒ�ڰ����г���

	void SetHideReason(const char *szReason);

	void SaveConfig(AIniFile &);			//	�������ã�����ҵ������Ҫ����첻�ٵ�����
	void LoadConfig(AIniFile &);			//	������������
};

//	�Ի���δ����ʱ���ز��ԣ�����ʱ���ء��������������ϲ�����������
class AUIDialog;
class CECQuickBuyPopHidePolicy
{
public:
	virtual ~CECQuickBuyPopHidePolicy(){}
	
	virtual void Restart()=0;
	virtual void TickOnShow(AUIDialog *pDlg)=0;
	virtual bool CanHide(const char *&szHideReason)=0;
};

//	�Ի�����ʾ������ԣ����С����ĸ��Ի���ʲôҪ�ض���
class AUIDialog;
class CECQuickBuyPopAlignPolicy
{
public:
	virtual ~CECQuickBuyPopAlignPolicy(){}	
	virtual void OnShow(AUIDialog *pDlg){}
};

//	�ṩĳ���������ԵĶ���ӿ�
class AUIDialog;
class AString;
enum QuickBuyPopConfigID
{
	QUICK_BUY_POP_INVALID,		//	��Ч
	QUICK_BUY_POP_SAFE_LIFE,	//	������
	QUICK_BUY_POP_ACTIVITY,		//	����ǰ����ص�
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