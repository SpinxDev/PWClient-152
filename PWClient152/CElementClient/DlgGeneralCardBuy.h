// Filename	: DlgGeneralCardBuy.h
// Creator	: Xu Wenbin
// Date		: 2014/2/26

#pragma once

#include "DlgBase.h"
#include "EC_RandMallShopping.h"
#include "EC_ControlLoopAnimation.h"

class AUICheckBox;
class CDlgGeneralCardBuySetting : public CDlgBase
{	
public:
	CDlgGeneralCardBuySetting();	
	
	bool GetSettingNeedConfirmToBuy();
	bool GetSettingShowRollToCardGfx(int rank);
	
protected:
	
	//	从 CDlgBase 继承
	virtual bool		OnInitDialog();
	
private:
	AUICheckBox *		m_pChk_NoConfirm;
};

class AUIImagePicture;
class CECControlLoopAnimation;
class CDlgGeneralCardBuy : public CDlgBase, public RandMallShoppingObserver
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgGeneralCardBuy();
	virtual ~CDlgGeneralCardBuy();

	void				OnCommandBuy(const char* szCommand);
	void				OnCommandBatchBuy(const char* szCommand);
	void				OnCommandSetting(const char* szCommand);
	void				OnCommandCancel(const char* szCommand);

	//	从 RandMallShoppingObserver 继承
	virtual void		OnStateChange(const RandMallShopping *p, enumRandMallShoppingState lastState);

	//	外部状态查询
	bool IsBatchBuy()const{ return m_bBatchBuy; }
	bool IsWaitStopBatchBuy()const{ return m_bWaitStopBatchBuy; }

protected:

	//	从 CDlgBase 继承
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	virtual void		OnHideDialog();
	virtual void		OnTick();
	virtual bool		Release();

	//	从 CDlgTheme 继承
	virtual bool		OnChangeLayoutBegin();
	virtual void		OnChangeLayoutEnd(bool bAllDone);

	//	界面更新相关
	void OnNoConfig();
	void SetCardImage(int tid);
	void ShowCard(bool bShow);
	void ShowRollToCardGfx(bool bShow);
	void CheckRollToGfxAndToPay(bool bStopGfx);
	bool IsRollToCardGfxShown();
	void ShowPayGfx(bool bShow);
	void CheckPayGfxAndToNextRoll(bool bStopGfx);
	bool CheckGfxStopped(AUIImagePicture *pImg);

	//	批量购买
	int	 GetBatchToBuyCountFromUI();
	bool CanBatchBuy();
	void StartBatchBuy();
	bool CanStopBatchBuyImmediately(const RandMallShopping *p=NULL);
	void StopBatchBuy();
	void StopBatchBuyImmediately();
	void UpdateBatchBuyCountUI();
	int  GetBatchBuyedCount();
	int  GetBatchToBuyCount();
	void SetBatchBuyedCount(int buyed);
	void SetBatchToBuyCount(int tobuy);
	void SetBatchBuyCount(int buyed, int tobuy);
	void TickBatchBuy();

	//	控制选项
	bool GetSettingNeedConfirmToBuy();
	bool GetSettingShowRollToCardGfx(int rank);

private:
	AUIObject *			m_pBtn_Buy;			//	单次随机\购买
	AUIObject *			m_pTxt_Name;		//	商品名称
	AUIObject *			m_pTxt_NameHint;	//	商品名称含义（已随机、已购买）
	AUIImagePicture *	m_pImg_CardIcon;	//	商品图标
	AUIImagePicture *	m_pImg_CardGfx;		//	随机成功后显示特效（跟战灵级别挂钩）
	AUIImagePicture *	m_pImg_NameGfx;		//	成功购买后显示特效
	AUIImagePicture *	m_pImg_CardFrame;	//	战灵边框图片
	AUIImagePicture *	m_pImg_Card;		//	战灵图片
	AUIObject *			m_pTxt_Cash;		//	剩余元宝数

	AUIObject *			m_pBtn_BatchBuy;	//	批量购买\停止
	AUIObject *			m_pEdt_Amount;		//	批量购买数量输入
	AUIObject *			m_pTxt_Amount;		//	批量已购买数量

	CECControlLoopAnimation	*	m_pUIRoller;//	随机动画控制
	int					m_itemToShow;		//	待显示的物品ID

	bool				m_bBatchBuy;		//	当前是否批量购买
	bool				m_bWaitStopBatchBuy;//	需要停止批量购买（购买过程中不能立即停止、等下次状态改变时再处理）
	bool				m_bBatchBuyNextStep;//	是否切换到下一步（见 OnStateChange 中说明）
};