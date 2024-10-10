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
	"界面\\战灵卡牌破裂效果_蓝色.gfx",
	"界面\\战灵卡牌破裂效果_紫色.gfx",
	"界面\\战灵卡牌破裂效果_金色.gfx",
	"界面\\战灵卡牌破裂效果_橙色.gfx",
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
	int index = GetCheckedRadioButton(2);	//	0:不播放，1~5 对应 rank 0~4
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
		//	界面初始化完成后再初始化动画控制，以获取正确的大小等内容
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
	//	未播放完成的特效，再次打开界面时，会由于 AUIImagePicture::Show 而再次播放，此处清除特效、隐藏控件
	ShowRollToCardGfx(false);
	CheckPayGfxAndToNextRoll(true);
	CDlgBase::OnHideDialog();
}

void CDlgGeneralCardBuy::OnTick()
{
	CDlgBase::OnTick();

	//	检查随机动画是否播放完成、并显示随机结果
	if (m_pUIRoller && m_pUIRoller->Tick()){
		ShowCard(true);
		ShowRollToCardGfx(true);
	}

	//	检查成功随机特效播放是否完成、并切换到可付款状态
	CheckRollToGfxAndToPay(false);

	//	检查付款特效播放是否完成、并切换到下次购买状态
	CheckPayGfxAndToNextRoll(false);

	//	批量购买进度推动
	TickBatchBuy();

	//	更新剩余元宝
	CECUIHelper::QShopSetCash(m_pTxt_Cash);
}

void CDlgGeneralCardBuy::TickBatchBuy()
{
	while (IsBatchBuy())
	{
		//	批量购买延迟停止处理
		if (IsWaitStopBatchBuy() && CanStopBatchBuyImmediately()){
			StopBatchBuyImmediately();
			break;
		}
		
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){			
			//	检查成功随机特效播放是否完成（未完成则在Tick中继续检查）、完成后再自动付款
			if (p->IsStateToPay() && IsRollToCardGfxShown()){
				break;
			}			
			//	检查启动下一次随机\购买行为
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

	//	根据用户输入等状态、实时更新是否可以批量购买等
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
		//	szCommand 不为空时，为玩家手动点击，此时停止批量购买
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
						//	批量购买过程中，如果玩家点击取消付款，则中止批量购买
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
			if (p->CanRoll()){	//	批量购买状态下、GotoNextRoll() 调用后、会导致 OnStateChange 并嵌套调用本函数（OnCommandBuy），则已经调用过 p->Roll
				p->Roll();		//	此处只能为单次购买服务
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
	//	清除已有战灵图片
	CDlgGeneralCard::SetCardImage(0, m_pImg_Card, m_pImg_CardFrame, NULL);

	//	设置新图片
	if (tid != 0){
		CDlgGeneralCard::SetCardImage(tid, m_pImg_Card, m_pImg_CardFrame, NULL);
	}
}

void CDlgGeneralCardBuy::ShowCard(bool bShow)
{
	if (bShow){
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
			//	商品名称、描述及图标
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
			
			//	战灵图片
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
	//	显示/隐藏随机的商品获得特效
	if (bShow && IsShow() && m_itemToShow){
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
			if (p->IsStateToPay()){
				//	获得商品时设置、显示特效
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
				//	购买过程中维持特效
				return;
			}
		}
	}
	//	其它情况下清除特效
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
	//	在随到战灵特效显示期间，检查播放是否结束并进入下一步
	if (m_pImg_CardGfx->IsShow()){
		if (bStopGfx || CheckGfxStopped(m_pImg_CardGfx)){
			if (IsBatchBuy()){	//	只在批量购买期间有此自动行为
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
	//	付款瞬间、显示特效，完成后，切换到下次购买
	if (bShow){
		if (RandMallShopping *p = RandMallShoppingManager::Instance().GetPokerShopping()){
			if (p->IsStateInPay()){
				if (IsShow()){
					m_pImg_NameGfx->Show(true);
					if (A3DGFXEx *pGfx = m_pImg_NameGfx->GetGfx()){
						if (pGfx->GetState() == ST_PLAY){
							//	成功切换到播放状态，直接返回
							return;
						}
					}
				}
				//	continue, 隐藏特效
			}
		}
	}
	if (m_pImg_NameGfx->IsShow()){
		m_pImg_NameGfx->Show(false);
		if (A3DGFXEx *pGfx = m_pImg_NameGfx->GetGfx()){
			pGfx->Stop(true);	//	重置播放状态
		}
	}
}

void CDlgGeneralCardBuy::CheckPayGfxAndToNextRoll(bool bStopGfx)
{
	//	在付款特效显示期间，检查播放状态并进入下一步
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
	//	当前是否可点击批量购买
	bool bCan(false);
	while (true)
	{
		//	正在批量购买
		if (IsBatchBuy()){
			break;
		}
		
		//	检查输入数量，值 > 1
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
		
		//	首次购买不允许批量
		if (p->IsFirstTime()){
			break;
		}
		//	当前为可随机状态、或可切换到可随机状态
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
	//	进入批量购买状态
	m_bBatchBuy = true;
	m_bWaitStopBatchBuy = false;
	m_bBatchBuyNextStep = false;

	//	显示“停止购买”
	m_pBtn_BatchBuy->SetText(GetStringFromTable(11263));

	//	记录需购买数量、并禁止再修改
	m_pEdt_Amount->Enable(false);
	SetBatchBuyCount(0, GetBatchToBuyCountFromUI());

	//	触发第一次购买行为
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
	//	更新界面：已购买数量
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
	//	是否能立即停止批量购买
	//	用途：为保证停止购买时显示的已购买数量准确、购买过程中不立即停止购买，而记录标记、等下次状态转换时根据此标志停止购买
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
	//	进入停止状态
	m_bBatchBuy = false;
	m_bWaitStopBatchBuy = false;
	m_bBatchBuyNextStep = false;

	//	显示“批量购买”
	m_pBtn_BatchBuy->SetText(GetStringFromTable(11262));

	//	允许修改购买数量
	m_pEdt_Amount->Enable(true);

	//	购买数量不够时，显示中止消息
	int buyed = GetBatchBuyedCount();
	int tobuy = GetBatchToBuyCount();
	ACString strText;
	if (buyed < tobuy){
		strText.Format(GetStringFromTable(11264), tobuy, buyed);
	}else{
		strText.Format(GetStringFromTable(11265), buyed);
	}
	GetGameUIMan()->AddChatMessage(strText, GP_CHAT_SYSTEM);

	//	清空已购买数量
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
	//	配置表未正确初始化
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

	//	随机\购买按钮
	if (p->IsStateToPay() || p->IsStateInPay()){
		m_pBtn_Buy->SetText(GetStringFromTable(11257));
	}else{
		m_pBtn_Buy->SetText(GetStringFromTable(11256));
	}
	m_pBtn_Buy->Enable(p->CanRoll() || p->CanPay() || p->CanGotoNextRoll());

	//	记录可显示的商品ID，在随机动画播放完成后显示
	if (p->IsStateToPay() || p->IsStateInPay()){
		m_itemToShow = p->GetItemToPay();
	}else if (p->IsStatePayed()){
		//	保留 m_itemToShow 不变（从付款前获得的参数）
	}else{
		m_itemToShow = 0;
	}

	//	随机动画（仅在单次购买期间播放）
	if (!IsBatchBuy()){
		if (p->IsStateInRoll()){
			m_pUIRoller->Start();	//	播放开始动画
		}else if (m_pUIRoller->IsMoving()){
			m_pUIRoller->Stop();	//	播放结束动画
		}
	}else{
		if (m_pUIRoller->IsMoving()){
			m_pUIRoller->Stop();	//	播放结束动画
		}
	}

	//	商品名称等
	if (!m_pUIRoller->IsMoving()){
		ShowCard(true);
		ShowRollToCardGfx(lastState == RMSS_IN_ROLL);
	}else{
		ShowCard(false);
		ShowRollToCardGfx(false);
	}

	//	付款特效
	if (p->IsStateInPay()){			//	成功进行付款动作
		ShowPayGfx(true);
	}else if (p->IsStatePayed()){	//	付款成功状态（此时特效可能没播放完）
	}else{
		ShowPayGfx(false);
	}

	//	批量购买更新
	if (!IsBatchBuy() && RMSS_NONE == lastState){
		//	首次注册回调、初始化控件状态
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
			
			//	增加已购买数量
			SetBatchBuyedCount(buyed+1);
			
			//	检查是否已经购买结束
			if (buyed+1 >= tobuy){
				StopBatchBuyImmediately();
				break;
			}
		}

		//	非法状态时、停止批量购买
		if (!p->IsStateToRoll() &&
			!p->IsStateInRoll() &&
			!p->IsStateToPay() &&
			!p->IsStateInPay() &&
			!p->IsStatePayed()){
			StopBatchBuyImmediately();
			break;
		}

		//	协议未响应、出错（元宝不足、包裹不足等）时，停止批量购买
		if (p->IsStateToRoll() && lastState != RMSS_PAYED ||
			p->IsStateToPay() && lastState != RMSS_IN_ROLL){
			StopBatchBuyImmediately();
			break;
		}

		//	检查成功随机特效播放是否完成（未完成则在Tick中继续检查）、完成后再自动付款
		if (p->IsStateToPay() && IsRollToCardGfxShown()){
			break;
		}

		//	记录并在 OnTick 中启动下一次随机\购买行为（原因：此处若进行下一步状态切换、将导致状态切换嵌套，难以理解和维护代码）
		m_bBatchBuyNextStep = true;
		break;
	}
}
