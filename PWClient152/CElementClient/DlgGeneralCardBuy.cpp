// Filename	: DlgGeneralCardBuy.cpp
// Creator	: Xu Wenbin
// Date		: 2014/2/26

#include "DlgGeneralCardBuy.h"
#include "DlgBuyConfirm.h"
#include "DlgGeneralCard.h"
#include "EC_RandMallShopping.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_UIHelper.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_GPDataType.h"
#include "auto_delete.h"

#include "ExpTypes.h"

#include <AUICTranslate.h>
#include <AUICheckbox.h>
#include <AUIImagePicture.h>

#include <A3DGFXEx.h>
#include <A3DGFXElement.h>

#define new A_DEBUG_NEW

static const char* ROLL_TO_CARD_GFX[ELEMENTDATA_NUM_POKER_RANK] = {
	"����\\ս�鿨������Ч��_��ɫ.gfx",
	"����\\ս�鿨������Ч��_��ɫ.gfx",
	"����\\ս�鿨������Ч��_��ɫ.gfx",
	"����\\ս�鿨������Ч��_��ɫ.gfx",
	"",
};

//	class CDlgGeneralCardBuySetting
CDlgGeneralCardBuySetting::CDlgGeneralCardBuySetting()
: m_pChk_NoConfirm(NULL)
{
}

bool CDlgGeneralCardBuySetting::OnInitDialog()
{
	DDX_Control("Chk_NoConfirm",	m_pChk_NoConfirm);
	CheckRadioButton(2, 3);
	return CDlgBase::OnInitDialog();
}

bool CDlgGeneralCardBuySetting::GetSettingNeedConfirmToBuy()
{
	bool bNeed(true);
	if (m_pChk_NoConfirm && m_pChk_NoConfirm->IsChecked()){
		bNeed = false;
	}
	return bNeed;
}

bool CDlgGeneralCardBuySetting::GetSettingShowRollToCardGfx(int rank)
{
	int index = GetCheckedRadioButton(2);	//	0:�����ţ�1~5 ��Ӧ rank 0~4
	if (index >= 1 && index <= ELEMENTDATA_NUM_POKER_RANK){
		return index-1 <= rank;
	}
	return false;
}

//	class CDlgGeneralCardBuy
AUI_BEGIN_COMMAND_MAP(CDlgGeneralCardBuy, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCancel)
AUI_ON_COMMAND("Btn_Buy",		OnCommandBuy)
AUI_ON_COMMAND("Btn_BatchBuy",	OnCommandBatchBuy)
AUI_ON_COMMAND("Btn_Setting",	OnCommandSetting)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGeneralCardBuy, CDlgBase)
AUI_END_EVENT_MAP()

CDlgGeneralCardBuy::CDlgGeneralCardBuy()
: m_pBtn_Buy(NULL)
, m_pTxt_Name(NULL)
, m_pTxt_NameHint(NULL)
, m_pImg_CardIcon(NULL)
, m_pImg_CardGfx(NULL)
, m_pImg_NameGfx(NULL)
, m_pImg_CardFrame(NULL)
, m_pImg_Card(NULL)
, m_pTxt_Cash(NULL)
, m_pBtn_BatchBuy(NULL)
, m_pEdt_Amount(NULL)
, m_pTxt_Amount(NULL)
, m_pUIRoller(NULL)
, m_itemToShow(0)
, m_bBatchBuy(false)
, m_bWaitStopBatchBuy(false)
, m_bBatchBuyNextStep(false)
{
}

CDlgGeneralCardBuy::~CDlgGeneralCardBuy()
{
}

bool CDlgGeneralCardBuy::Release()
{
	if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
		p->UnregisterObserver(this);
	}
	delete m_pUIRoller;
	m_pUIRoller = NULL;
	SetCardImage(0);
	return CDlgBase::Release();
}

bool CDlgGeneralCardBuy::OnInitDialog()
{
	DDX_Control("Btn_Buy",		m_pBtn_Buy);
	DDX_Control("Txt_Name",		m_pTxt_Name);
	DDX_Control("Txt_NameHint",	m_pTxt_NameHint);
	DDX_Control("Img_CardIcon",	m_pImg_CardIcon);
	DDX_Control("Img_CardGfx",	m_pImg_CardGfx);
	DDX_Control("Img_NameGfx",	m_pImg_NameGfx);
	DDX_Control("Img_GeneralCardFrame",	m_pImg_CardFrame);
	DDX_Control("Img_GeneralCard",		m_pImg_Card);
	DDX_Control("Txt_Cash",		m_pTxt_Cash);
	DDX_Control("Btn_BatchBuy",	m_pBtn_BatchBuy);
	DDX_Control("Edt_Amount",	m_pEdt_Amount);
	DDX_Control("Txt_Amount",	m_pTxt_Amount);
	return CDlgBase::OnInitDialog();
}

void CDlgGeneralCardBuy::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	if (!m_pUIRoller){
		//	�����ʼ����ɺ��ٳ�ʼ���������ƣ��Ի�ȡ��ȷ�Ĵ�С������
		CECControlLoopAnimation::AnimateCtrls aCtrls;
		const char *szCtrlNames[] = {
			"Img_LeftCard2", "Img_LeftCard1",
			"Img_MidCard",
			"Img_RightCard1", "Img_RightCard2"
		};
		for (int i(0); i < sizeof(szCtrlNames)/sizeof(szCtrlNames[0]); ++ i)
		{
			if (PAUIOBJECT pCtrl = GetDlgItem(szCtrlNames[i])){
				aCtrls.push_back(pCtrl);
			}
		}
		m_pUIRoller = new CECControlLoopAnimation(this, aCtrls);
	}

	if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
		if (!p->IsObserverRegistered(this)){
			p->RegisterObserver(this);
		}
		if (p->CanQuery()){
			p->Query();
		}
	}else{
		OnNoConfig();
	}
}

void CDlgGeneralCardBuy::OnHideDialog()
{
	if (m_pUIRoller){
		m_pUIRoller->Reset();
	}	
	//	δ������ɵ���Ч���ٴδ򿪽���ʱ�������� AUIImagePicture::Show ���ٴβ��ţ��˴������Ч�����ؿؼ�
	ShowRollToCardGfx(false);
	CheckPayGfxAndToNextRoll(true);
	CDlgBase::OnHideDialog();
}

void CDlgGeneralCardBuy::OnTick()
{
	CDlgBase::OnTick();

	//	�����������Ƿ񲥷���ɡ�����ʾ������
	if (m_pUIRoller && m_pUIRoller->Tick()){
		ShowCard(true);
		ShowRollToCardGfx(true);
	}

	//	���ɹ������Ч�����Ƿ���ɡ����л����ɸ���״̬
	CheckRollToGfxAndToPay(false);

	//	��鸶����Ч�����Ƿ���ɡ����л����´ι���״̬
	CheckPayGfxAndToNextRoll(false);

	//	������������ƶ�
	TickBatchBuy();

	//	����ʣ��Ԫ��
	CECUIHelper::QShopSetCash(m_pTxt_Cash);
}

void CDlgGeneralCardBuy::TickBatchBuy()
{
	while (IsBatchBuy())
	{
		//	���������ӳ�ֹͣ����
		if (IsWaitStopBatchBuy() && CanStopBatchBuyImmediately()){
			StopBatchBuyImmediately();
			break;
		}
		
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){			
			//	���ɹ������Ч�����Ƿ���ɣ�δ�������Tick�м�����飩����ɺ����Զ�����
			if (p->IsStateToPay() && IsRollToCardGfxShown()){
				break;
			}			
			//	���������һ�����\������Ϊ
			if (m_bBatchBuyNextStep){
				m_bBatchBuyNextStep = false;
				if (p->IsStateToRoll() || p->IsStateToPay() || p->IsStatePayed()){
					OnCommandBuy("");
				}
			}
		}else{
			StopBatchBuyImmediately();
		}
		break;
	}

	//	�����û������״̬��ʵʱ�����Ƿ�������������
	if (IsBatchBuy() || CanBatchBuy()){
		m_pBtn_BatchBuy->Enable(true);
		m_pBtn_BatchBuy->SetHint(_AL(""));
	}else{
		m_pBtn_BatchBuy->Enable(false);
		while (true)
		{
			if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
				if (p->IsStateToPay()){
					m_pBtn_BatchBuy->SetHint(GetStringFromTable(11266));
					break;
				}
			}
			m_pBtn_BatchBuy->SetHint(_AL(""));
			break;
		}
	}
}

bool CDlgGeneralCardBuy::OnChangeLayoutBegin()
{
	if (m_pUIRoller && m_pUIRoller->IsInited()){
		m_pUIRoller->OnChangeLayoutBegin();
	}
	return true;
}

void CDlgGeneralCardBuy::OnChangeLayoutEnd(bool bAllDone)
{
	if (m_pUIRoller && m_pUIRoller->IsInited()){
		m_pUIRoller->OnChangeLayoutEnd();
	}
}

void CDlgGeneralCardBuy::OnCommandBuy(const char* szCommand)
{
	if (IsBatchBuy() && szCommand && strlen(szCommand)){
		//	szCommand ��Ϊ��ʱ��Ϊ����ֶ��������ʱֹͣ��������
		StopBatchBuy();
		return;
	}
	if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
		if (p->CanRoll()){
			p->Roll();
		}else if (p->CanPay()){
			if (!p->IsFirstTime() && !GetSettingNeedConfirmToBuy()){
				p->Pay();
			}else{
				class CECPokerShopBuyCommand : public CECRandShopBuyCommand
				{
					CDlgGeneralCardBuy * m_pDlg;
				protected:
					virtual void OnCancel(){
						//	������������У������ҵ��ȡ���������ֹ��������
						if (m_pDlg->IsBatchBuy()){
							m_pDlg->OnCommandBatchBuy("");
						}
					}
				public:
					CECPokerShopBuyCommand(RandMallShopping *p, CDlgGeneralCardBuy *pDlg)
						: CECRandShopBuyCommand(p), m_pDlg(pDlg) {}
				};
				GetGameUIMan()->m_pDlgBuyConfirm->Buy(new CECPokerShopBuyCommand(p, this));
			}
		}else if (p->CanGotoNextRoll()){
			p->GotoNextRoll();
			if (p->CanRoll()){	//	��������״̬�¡�GotoNextRoll() ���ú󡢻ᵼ�� OnStateChange ��Ƕ�׵��ñ�������OnCommandBuy�������Ѿ����ù� p->Roll
				p->Roll();		//	�˴�ֻ��Ϊ���ι������
			}
		}
	}
}

void CDlgGeneralCardBuy::OnCommandBatchBuy(const char* szCommand)
{
	if (CanBatchBuy()){
		StartBatchBuy();
	}else if (IsBatchBuy()){
		StopBatchBuy();
	}
}

void CDlgGeneralCardBuy::OnCommandSetting(const char* szCommand)
{
	GetGameUIMan()->m_pDlgGeneralCardBuySetting->SwitchShow();
}

void CDlgGeneralCardBuy::OnCommandCancel(const char* szCommand)
{
	if (IsBatchBuy()){
		StopBatchBuy();
		return;
	}
	if (m_pUIRoller && m_pUIRoller->IsMoving()){
		return;
	}
	Show(false);
}

void CDlgGeneralCardBuy::SetCardImage(int tid)
{
	//	�������ս��ͼƬ
	CDlgGeneralCard::SetCardImage(0, m_pImg_Card, m_pImg_CardFrame, NULL);

	//	������ͼƬ
	if (tid != 0){
		CDlgGeneralCard::SetCardImage(tid, m_pImg_Card, m_pImg_CardFrame, NULL);
	}
}

void CDlgGeneralCardBuy::ShowCard(bool bShow)
{
	if (bShow){
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
			//	��Ʒ���ơ�������ͼ��
			if (p->IsStateToPay() || p->IsStateInPay()){
				m_pTxt_NameHint->SetText(GetStringFromTable(11258));
			}else if (p->IsStatePayed()){
				m_pTxt_NameHint->SetText(GetStringFromTable(11259));
			}else{
				m_pTxt_NameHint->SetText(_AL(""));
			}
			if (m_itemToShow){
				CECIvtrItem *pItem = CECIvtrItem::CreateItem(m_itemToShow, 0, 1);
				pItem->GetDetailDataFromLocal();
				m_pTxt_Name->SetText(pItem->GetName());
				m_pImg_CardIcon->Show(true);
				if (const wchar_t *wszHint = pItem->GetDesc()){
					//AUICTranslate trans;
					//m_pImg_CardIcon->SetHint(trans.Translate(wszHint));
					m_pImg_CardIcon->SetHint(GetGameUIMan()->GetItemDescHint(pItem));
				}else{
					m_pImg_CardIcon->SetHint(_AL(""));
				}
				GetGameUIMan()->SetCover(m_pImg_CardIcon, pItem->GetIconFile());
				delete pItem;
			}else{
				m_pTxt_Name->SetText(_AL(""));
				m_pImg_CardIcon->Show(false);
				m_pImg_CardIcon->SetHint(_AL(""));
				m_pImg_CardIcon->ClearCover();
			}
			
			//	ս��ͼƬ
			SetCardImage(m_itemToShow);
			return;
		}
	}
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_NameHint->SetText(_AL(""));
	m_pImg_CardIcon->SetHint(_AL(""));
	m_pImg_CardIcon->ClearCover();
	m_pImg_CardIcon->Show(false);
	SetCardImage(0);
}

void CDlgGeneralCardBuy::ShowRollToCardGfx(bool bShow)
{
	//	��ʾ/�����������Ʒ�����Ч
	if (bShow && IsShow() && m_itemToShow){
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
			if (p->IsStateToPay()){
				//	�����Ʒʱ���á���ʾ��Ч
				CECIvtrItem *pItem = CECIvtrItem::CreateItem(m_itemToShow, 0, 1);
				auto_delete<CECIvtrItem> tmp(pItem);
				if (CECIvtrGeneralCard *pCard = dynamic_cast<CECIvtrGeneralCard *>(pItem)){
					int rank = pCard->GetDBEssence()->rank;
					if (GetSettingShowRollToCardGfx(rank)){
						m_pImg_CardGfx->SetGfx(ROLL_TO_CARD_GFX[rank]);
						m_pImg_CardGfx->Show(true);
						return;
					}
				}
			}else if (p->IsStateInPay()){
				//	���������ά����Ч
				return;
			}
		}
	}
	//	��������������Ч
	if (m_pImg_CardGfx->IsShow()){
		m_pImg_CardGfx->SetGfx("");
		m_pImg_CardGfx->Show(false);
	}
}

bool CDlgGeneralCardBuy::CheckGfxStopped(AUIImagePicture *pImg)
{
	bool bStopped(false);
	if (pImg->IsShow()){
		if (A3DGFXEx *pGfx = pImg->GetGfx()){
			if (pGfx->GetState() != ST_PLAY){
				bStopped = true;
			}
		}
	}
	return bStopped;
}

void CDlgGeneralCardBuy::CheckRollToGfxAndToPay(bool bStopGfx)
{
	//	���浽ս����Ч��ʾ�ڼ䣬��鲥���Ƿ������������һ��
	if (m_pImg_CardGfx->IsShow()){
		if (bStopGfx || CheckGfxStopped(m_pImg_CardGfx)){
			if (IsBatchBuy()){	//	ֻ�����������ڼ��д��Զ���Ϊ
				if (!IsWaitStopBatchBuy()){
					if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
						if (p->CanPay()){
							OnCommandBuy("");
						}
					}
				}
			}
			ShowRollToCardGfx(false);
		}
	}
}

bool CDlgGeneralCardBuy::IsRollToCardGfxShown()
{
	return m_pImg_CardGfx->IsShow();
}

void CDlgGeneralCardBuy::ShowPayGfx(bool bShow)
{
	//	����˲�䡢��ʾ��Ч����ɺ��л����´ι���
	if (bShow){
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
			if (p->IsStateInPay()){
				if (IsShow()){
					m_pImg_NameGfx->Show(true);
					if (A3DGFXEx *pGfx = m_pImg_NameGfx->GetGfx()){
						if (pGfx->GetState() == ST_PLAY){
							//	�ɹ��л�������״̬��ֱ�ӷ���
							return;
						}
					}
				}
				//	continue, ������Ч
			}
		}
	}
	if (m_pImg_NameGfx->IsShow()){
		m_pImg_NameGfx->Show(false);
		if (A3DGFXEx *pGfx = m_pImg_NameGfx->GetGfx()){
			pGfx->Stop(true);	//	���ò���״̬
		}
	}
}

void CDlgGeneralCardBuy::CheckPayGfxAndToNextRoll(bool bStopGfx)
{
	//	�ڸ�����Ч��ʾ�ڼ䣬��鲥��״̬��������һ��
	if (m_pImg_NameGfx->IsShow()){
		if (bStopGfx || CheckGfxStopped(m_pImg_NameGfx)){
			if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
				if (p->CanGotoNextRoll()){
					p->GotoNextRoll();
				}
			}
			ShowPayGfx(false);
		}
	}
}

int CDlgGeneralCardBuy::GetBatchToBuyCountFromUI()
{
	int count(0);
	ACString strCount = m_pEdt_Amount->GetText();
	if (!strCount.IsEmpty()){
		count = strCount.ToInt();
	}
	return count;
}

bool CDlgGeneralCardBuy::CanBatchBuy()
{
	//	��ǰ�Ƿ�ɵ����������
	bool bCan(false);
	while (true)
	{
		//	������������
		if (IsBatchBuy()){
			break;
		}
		
		//	�������������ֵ > 1
		ACString strCount = m_pEdt_Amount->GetText();
		if (strCount.IsEmpty()){
			break;
		}
		int count = strCount.ToInt();
		if (count <= 1){
			break;
		}

		RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping();
		if (!p){
			break;
		}
		
		//	�״ι�����������
		if (p->IsFirstTime()){
			break;
		}
		//	��ǰΪ�����״̬������л��������״̬
		if (!p->IsStateToRoll() && !p->CanGotoNextRoll()){
			break;
		}
		
		bCan = true;
		break;
	}
	return bCan;
}

void CDlgGeneralCardBuy::StartBatchBuy()
{
	if (!CanBatchBuy()){
		return;
	}
	//	������������״̬
	m_bBatchBuy = true;
	m_bWaitStopBatchBuy = false;
	m_bBatchBuyNextStep = false;

	//	��ʾ��ֹͣ����
	m_pBtn_BatchBuy->SetText(GetStringFromTable(11263));

	//	��¼�蹺������������ֹ���޸�
	m_pEdt_Amount->Enable(false);
	SetBatchBuyCount(0, GetBatchToBuyCountFromUI());

	//	������һ�ι�����Ϊ
	OnCommandBuy("");
}

int CDlgGeneralCardBuy::GetBatchBuyedCount()
{
	return m_pTxt_Amount->GetData();
}

int CDlgGeneralCardBuy::GetBatchToBuyCount()
{
	return (int)m_pTxt_Amount->GetDataPtr();
}

void CDlgGeneralCardBuy::SetBatchBuyedCount(int buyed)
{
	m_pTxt_Amount->SetData(buyed);
	UpdateBatchBuyCountUI();
}

void CDlgGeneralCardBuy::SetBatchToBuyCount(int tobuy)
{
	m_pTxt_Amount->SetDataPtr((void *)tobuy);
	UpdateBatchBuyCountUI();
}

void CDlgGeneralCardBuy::SetBatchBuyCount(int buyed, int tobuy)
{
	m_pTxt_Amount->SetData(buyed);
	m_pTxt_Amount->SetDataPtr((void *)tobuy);
	UpdateBatchBuyCountUI();
}

void CDlgGeneralCardBuy::UpdateBatchBuyCountUI()
{
	//	���½��棺�ѹ�������
	int tobuy = (int)m_pTxt_Amount->GetDataPtr();
	if (tobuy > 0){
		int buyed = m_pTxt_Amount->GetData();
		ACString strText;
		strText.Format(_AL("%d/%d"), buyed, tobuy);
		m_pTxt_Amount->SetText(strText);
	}else{
		m_pTxt_Amount->SetText(_AL("0"));
	}
}

bool CDlgGeneralCardBuy::CanStopBatchBuyImmediately(const RandMallShopping *p)
{
	//	�Ƿ�������ֹͣ��������
	//	��;��Ϊ��ֹ֤ͣ����ʱ��ʾ���ѹ�������׼ȷ����������в�����ֹͣ���򣬶���¼��ǡ����´�״̬ת��ʱ���ݴ˱�־ֹͣ����
	bool bCan(false);
	if (IsBatchBuy()){
		if (!p){
			p = RandMallShoppingManager::Instance().GetPokerShopping();
		}
		if (p && (p->IsStateToRoll() || p->IsStateToPay() || p->IsStatePayed())){
			bCan = true;
		}
	}
	return bCan;
}

void CDlgGeneralCardBuy::StopBatchBuyImmediately()
{
	if (!IsBatchBuy()){
		return;
	}
	//	����ֹͣ״̬
	m_bBatchBuy = false;
	m_bWaitStopBatchBuy = false;
	m_bBatchBuyNextStep = false;

	//	��ʾ����������
	m_pBtn_BatchBuy->SetText(GetStringFromTable(11262));

	//	�����޸Ĺ�������
	m_pEdt_Amount->Enable(true);

	//	������������ʱ����ʾ��ֹ��Ϣ
	int buyed = GetBatchBuyedCount();
	int tobuy = GetBatchToBuyCount();
	ACString strText;
	if (buyed < tobuy){
		strText.Format(GetStringFromTable(11264), tobuy, buyed);
	}else{
		strText.Format(GetStringFromTable(11265), buyed);
	}
	GetGameUIMan()->AddChatMessage(strText, GP_CHAT_SYSTEM);

	//	����ѹ�������
	SetBatchBuyCount(0, 0);
}

void CDlgGeneralCardBuy::StopBatchBuy()
{
	if (!IsBatchBuy()){
		return;
	}
	if (!CanStopBatchBuyImmediately()){
		m_bWaitStopBatchBuy = true;
		return;
	}
	StopBatchBuyImmediately();
}

bool CDlgGeneralCardBuy::GetSettingNeedConfirmToBuy()
{
	return GetGameUIMan()->m_pDlgGeneralCardBuySetting->GetSettingNeedConfirmToBuy();
}

bool CDlgGeneralCardBuy::GetSettingShowRollToCardGfx(int rank)
{
	return !IsBatchBuy() || GetGameUIMan()->m_pDlgGeneralCardBuySetting->GetSettingShowRollToCardGfx(rank);
}

void CDlgGeneralCardBuy::OnNoConfig()
{
	//	���ñ�δ��ȷ��ʼ��
	m_pBtn_Buy->SetText(GetStringFromTable(11256));
	m_pBtn_Buy->Enable(false);
	m_itemToShow = 0;
	ShowCard(false);
	ShowRollToCardGfx(false);
	ShowPayGfx(false);
	m_pUIRoller->Reset();
	
	m_bBatchBuy = false;
	m_bWaitStopBatchBuy = false;
	m_bBatchBuyNextStep = false;
	m_pBtn_BatchBuy->SetText(GetStringFromTable(11262));
	m_pEdt_Amount->Enable(true);
	m_pEdt_Amount->SetText(_AL(""));
	SetBatchBuyCount(0, 0);
}

void CDlgGeneralCardBuy::OnStateChange(const RandMallShopping *p, enumRandMallShoppingState lastState)
{
	if (!p->GetConfig()){
		OnNoConfig();
		return;
	}

	//	���\����ť
	if (p->IsStateToPay() || p->IsStateInPay()){
		m_pBtn_Buy->SetText(GetStringFromTable(11257));
	}else{
		m_pBtn_Buy->SetText(GetStringFromTable(11256));
	}
	m_pBtn_Buy->Enable(p->CanRoll() || p->CanPay() || p->CanGotoNextRoll());

	//	��¼����ʾ����ƷID�����������������ɺ���ʾ
	if (p->IsStateToPay() || p->IsStateInPay()){
		m_itemToShow = p->GetItemToPay();
	}else if (p->IsStatePayed()){
		//	���� m_itemToShow ���䣨�Ӹ���ǰ��õĲ�����
	}else{
		m_itemToShow = 0;
	}

	//	������������ڵ��ι����ڼ䲥�ţ�
	if (!IsBatchBuy()){
		if (p->IsStateInRoll()){
			m_pUIRoller->Start();	//	���ſ�ʼ����
		}else if (m_pUIRoller->IsMoving()){
			m_pUIRoller->Stop();	//	���Ž�������
		}
	}else{
		if (m_pUIRoller->IsMoving()){
			m_pUIRoller->Stop();	//	���Ž�������
		}
	}

	//	��Ʒ���Ƶ�
	if (!m_pUIRoller->IsMoving()){
		ShowCard(true);
		ShowRollToCardGfx(lastState == RMSS_IN_ROLL);
	}else{
		ShowCard(false);
		ShowRollToCardGfx(false);
	}

	//	������Ч
	if (p->IsStateInPay()){			//	�ɹ����и����
		ShowPayGfx(true);
	}else if (p->IsStatePayed()){	//	����ɹ�״̬����ʱ��Ч����û�����꣩
	}else{
		ShowPayGfx(false);
	}

	//	�����������
	if (!IsBatchBuy() && RMSS_NONE == lastState){
		//	�״�ע��ص�����ʼ���ؼ�״̬
		m_pBtn_BatchBuy->SetText(GetStringFromTable(11262));
		m_pEdt_Amount->Enable(true);
		m_pEdt_Amount->SetText(_AL(""));
		SetBatchBuyCount(0, 0);
	}
	while (IsBatchBuy())
	{
		if (p->IsStatePayed()){
			int buyed = GetBatchBuyedCount();
			int tobuy = GetBatchToBuyCount();
			
			//	�����ѹ�������
			SetBatchBuyedCount(buyed+1);
			
			//	����Ƿ��Ѿ��������
			if (buyed+1 >= tobuy){
				StopBatchBuyImmediately();
				break;
			}
		}

		//	�Ƿ�״̬ʱ��ֹͣ��������
		if (!p->IsStateToRoll() &&
			!p->IsStateInRoll() &&
			!p->IsStateToPay() &&
			!p->IsStateInPay() &&
			!p->IsStatePayed()){
			StopBatchBuyImmediately();
			break;
		}

		//	Э��δ��Ӧ������Ԫ�����㡢��������ȣ�ʱ��ֹͣ��������
		if (p->IsStateToRoll() && lastState != RMSS_PAYED ||
			p->IsStateToPay() && lastState != RMSS_IN_ROLL){
			StopBatchBuyImmediately();
			break;
		}

		//	���ɹ������Ч�����Ƿ���ɣ�δ�������Tick�м�����飩����ɺ����Զ�����
		if (p->IsStateToPay() && IsRollToCardGfxShown()){
			break;
		}

		//	��¼���� OnTick ��������һ�����\������Ϊ��ԭ�򣺴˴���������һ��״̬�л���������״̬�л�Ƕ�ף���������ά�����룩
		m_bBatchBuyNextStep = true;
		break;
	}
}
