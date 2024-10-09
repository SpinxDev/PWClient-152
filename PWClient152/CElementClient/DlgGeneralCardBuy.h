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
	
	//	�� CDlgBase �̳�
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

	//	�� RandMallShoppingObserver �̳�
	virtual void		OnStateChange(const RandMallShopping *p, enumRandMallShoppingState lastState);

	//	�ⲿ״̬��ѯ
	bool IsBatchBuy()const{ return m_bBatchBuy; }
	bool IsWaitStopBatchBuy()const{ return m_bWaitStopBatchBuy; }

protected:

	//	�� CDlgBase �̳�
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	virtual void		OnHideDialog();
	virtual void		OnTick();
	virtual bool		Release();

	//	�� CDlgTheme �̳�
	virtual bool		OnChangeLayoutBegin();
	virtual void		OnChangeLayoutEnd(bool bAllDone);

	//	����������
	void OnNoConfig();
	void SetCardImage(int tid);
	void ShowCard(bool bShow);
	void ShowRollToCardGfx(bool bShow);
	void CheckRollToGfxAndToPay(bool bStopGfx);
	bool IsRollToCardGfxShown();
	void ShowPayGfx(bool bShow);
	void CheckPayGfxAndToNextRoll(bool bStopGfx);
	bool CheckGfxStopped(AUIImagePicture *pImg);

	//	��������
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

	//	����ѡ��
	bool GetSettingNeedConfirmToBuy();
	bool GetSettingShowRollToCardGfx(int rank);

private:
	AUIObject *			m_pBtn_Buy;			//	�������\����
	AUIObject *			m_pTxt_Name;		//	��Ʒ����
	AUIObject *			m_pTxt_NameHint;	//	��Ʒ���ƺ��壨��������ѹ���
	AUIImagePicture *	m_pImg_CardIcon;	//	��Ʒͼ��
	AUIImagePicture *	m_pImg_CardGfx;		//	����ɹ�����ʾ��Ч����ս�鼶��ҹ���
	AUIImagePicture *	m_pImg_NameGfx;		//	�ɹ��������ʾ��Ч
	AUIImagePicture *	m_pImg_CardFrame;	//	ս��߿�ͼƬ
	AUIImagePicture *	m_pImg_Card;		//	ս��ͼƬ
	AUIObject *			m_pTxt_Cash;		//	ʣ��Ԫ����

	AUIObject *			m_pBtn_BatchBuy;	//	��������\ֹͣ
	AUIObject *			m_pEdt_Amount;		//	����������������
	AUIObject *			m_pTxt_Amount;		//	�����ѹ�������

	CECControlLoopAnimation	*	m_pUIRoller;//	�����������
	int					m_itemToShow;		//	����ʾ����ƷID

	bool				m_bBatchBuy;		//	��ǰ�Ƿ���������
	bool				m_bWaitStopBatchBuy;//	��Ҫֹͣ�������򣨹�������в�������ֹͣ�����´�״̬�ı�ʱ�ٴ���
	bool				m_bBatchBuyNextStep;//	�Ƿ��л�����һ������ OnStateChange ��˵����
};