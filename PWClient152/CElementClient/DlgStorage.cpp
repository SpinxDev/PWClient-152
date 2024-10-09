/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   19:38
	file name:	DlgStorage.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "AFI.h"
#include "AUIEditBox.h"
#include "AUIDef.h"
#include "AUIComboBox.h"
#include "AUICheckBox.h"
#include "DlgChangeSkirt.h"
#include "DlgStorage.h"
#include "DlgInputNO.h"
#include "DlgDragDrop.h"
#include "DlgChat.h"
#include "DlgGeneralCard.h"
#include "DlgItemDesc.h"
#include "EC_Global.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrFashion.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_IvtrEquipMatter.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgStorage, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommandIDCANCEL)
AUI_ON_COMMAND("choosemoney", OnCommandChooseMoney)
AUI_ON_COMMAND("storage", OnCommandStorage)
AUI_ON_COMMAND("storage1", OnCommandStorage1)
AUI_ON_COMMAND("storage2", OnCommandStorage2)
AUI_ON_COMMAND("changeskirt", OnCommandChangeSkirt)
AUI_ON_COMMAND("arrange", OnCommand_arrange)
AUI_ON_COMMAND("arrange_bypart", OnCommand_arrange_fashion_bypart)
AUI_ON_COMMAND("arrange_bysuite", OnCommand_arrange_fashion_bysuite)
AUI_ON_COMMAND("Btn_All", OnCommand_Btn_All)
AUI_ON_COMMAND("Btn_BatchSwallow", OnCommandBatchSwallow)
AUI_ON_COMMAND("Btn_AutoOpen", OnCommmandAutoDice)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgStorage, CDlgBase)

AUI_ON_EVENT("Item_*", WM_LBUTTONDOWN, OnLButtonDownItem)
AUI_ON_EVENT("Item_*", WM_LBUTTONDBLCLK, OnLButtonDbClickItem)
AUI_ON_EVENT("Item_*", WM_RBUTTONDOWN, OnRButtonDownItem)
AUI_ON_EVENT("*",	   WM_MOUSEWHEEL,  OnEventMouseWheel)
AUI_ON_EVENT(NULL,     WM_MOUSEWHEEL,  OnEventMouseWheel)
AUI_ON_EVENT("Img_Dice", WM_LBUTTONDOWN, OnLButtonDownCardDice)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgStorage
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgStorage> StorageClickShortcut;
//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Right-Click Shortcut for GeneralCard Storage
//------------------------------------------------------------------------
class GeneralCardClickShortcut : public CECShortcutMgr::ClickShortcut
{
public:
	GeneralCardClickShortcut(const char* pName, CDlgStorage* pDlg)
		:m_pDlg(pDlg)
		,m_GroupName(pName)
	{}
	
	virtual const char* GetGroupName() { return m_GroupName; }
	
	virtual bool CanTrigger(PAUIOBJECT pSrcObj) 
	{
		if(!m_pDlg) return false;
		if (stricmp(m_pDlg->GetName(), "Win_Storage4")) return false;
		
		PAUIMANAGER pUIMan = m_pDlg->GetAUIManager();
		if(!pUIMan->GetDialog("Win_GeneralCard")->IsShow())
			return false;
		
		return m_pDlg->IsShow() && pSrcObj && pSrcObj->GetDataPtr("ptr_CECIvtrItem");
	}
	
	virtual void Trigger(PAUIOBJECT pSrcObj, int num) 
	{ 
		CECIvtrItem* pItem = (CECIvtrItem*)pSrcObj->GetDataPtr("ptr_CECIvtrItem");
		
		PAUIOBJECT pObj = m_pDlg->GetEmptySlot(pItem);
		
		CDlgDragDrop* pDragDrop = dynamic_cast<CDlgDragDrop*>(m_pDlg->GetAUIManager()->GetDialog("DragDrop"));
		if (pDragDrop) pDragDrop->OnInventoryDragDrop(pSrcObj->GetParent(), pSrcObj, m_pDlg, pObj, pItem);
	}
	
protected:
	CDlgStorage* m_pDlg;
	AString m_GroupName;
};
//------------------------------------------------------------------------

enum {FASHOIN_STORAGE_PART_GROUP_ID = 2};

CDlgStorage::CDlgStorage():
m_bSwallowing(false),
m_bAutoDice(false),
m_iSwallowIndex(0)
{
}

CDlgStorage::~CDlgStorage()
{
}

bool CDlgStorage::OnInitDialog()
{
	int i;
	char szItem[20];
	PAUIIMAGEPICTURE pCell;

	for( i = 0; ; i++ )
	{
		sprintf(szItem, "Item_%02d", i + 1);
		pCell = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pCell ) break;
		m_vecImgCell.push_back(pCell);
		m_vecImgBg.push_back((PAUIIMAGEPICTURE)GetDlgItem(AString().Format("Img_Bg%02d", i + 1)));
	}

	for (i = 0; i < FASHION_COUNT; i++)
	{
		sprintf(szItem, "Txt_Fashion%d", i);
		m_pTxt_Fashion[i] = GetDlgItem(szItem);
	}
	m_pTxt_FashionAll = GetDlgItem("Txt_FashionAll");

	m_pScl_Right = (PAUISCROLL)GetDlgItem("Scl_Right");

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new StorageClickShortcut(this));
	if (IsGeneralCardStorage()) {
		GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new GeneralCardClickShortcut("Win_GeneralCard", this));
		m_BatchSwallowCounter.SetPeriod(100);
		PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
		if (pObj) pObj->SetData(-1);
		PAUICOMBOBOX pCombo = dynamic_cast<PAUICOMBOBOX>(GetDlgItem("Combo_Line"));
		if (pCombo) {
			for (int i = 10971;i < 10974; ++i)
				pCombo->AddString(GetStringFromTable(i));
			pCombo->SetCurSel(0);
		}
	}

	return true;
}

void CDlgStorage::OnShowDialog()
{
	if( stricmp(m_szName, "Win_Storage") == 0 )
		CheckRadioButton(1, 1);
	else if( stricmp(m_szName, "Win_Storage1") == 0 )
		CheckRadioButton(1, 2);
	else
		CheckRadioButton(1, 3);
	if (IsFashionStorage()){
		CheckRadioButton(FASHOIN_STORAGE_PART_GROUP_ID, -1);
	}
}

void CDlgStorage::OnCommandIDCANCEL(const char *szCommand)
{
	Show(false);

	if (IsGeneralCardStorage()) {
		EndBatchSwallow();
		EndAutoDice();
	} else if (IsFashionStorage()) {
		CDlgChangeSkirt *pChangeskirt = (CDlgChangeSkirt *)(GetGameUIMan()->GetDialog("Win_ChangeSkirt"));
		pChangeskirt->OnCommand("IDCANCEL");
	} else {
		GetGameSession()->c2s_CmdCancelAction();
	}
}

void CDlgStorage::OnCommandChooseMoney(const char *szCommand)
{
	CDlgInputNO *pDlg = GetGameUIMan()->m_pDlgInputNO;
	pDlg->Show(true, true);
	pDlg->SetType(CDlgInputNO::INPUTNO_STORAGE_TRASH_MONEY);
	
	ACHAR szText[40];
	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("DEFAULT_Txt_No."));
	pEdit->SetData(GetMoneyCnt());
	a_sprintf(szText, _AL("%d"), min(1, GetMoneyCnt()));
	pEdit->SetText(szText);
	pEdit->SetIsNumberOnly(true);
}

void CDlgStorage::OnCommandStorage(const char *szCommand)
{
	Show(false);
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Storage");
	pDlg->Show(true);
	pDlg->SetPosEx(GetPos().x, GetPos().y);
}

void CDlgStorage::OnCommandStorage1(const char *szCommand)
{
	Show(false);
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Storage1");
	pDlg->Show(true);
	pDlg->SetPosEx(GetPos().x, GetPos().y);
}

void CDlgStorage::OnCommandStorage2(const char *szCommand)
{
	Show(false);
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Storage2");
	pDlg->Show(true);
	pDlg->SetPosEx(GetPos().x, GetPos().y);
}

void CDlgStorage::OnCommandChangeSkirt(const char *szCommand)
{
	PAUIDIALOG pDlgChangeSkirt = GetGameUIMan()->GetDialog("Win_ChangeSkirt");
	pDlgChangeSkirt->Show(!pDlgChangeSkirt->IsShow());
}

class FashionOrderComponent
{
protected:
	bool IsFashionWeapon(CECIvtrFashion *pIvtrFashion)const{
		return pIvtrFashion->GetDBSubType()->equip_fashion_mask == EQUIP_MASK64_FASHION_WEAPON;
	}
public:
	virtual ~FashionOrderComponent(){}
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const=0;	//	优先者返回值更小
};

//	性别相同者优先
class FashionGenderOrderComponent : public FashionOrderComponent{
	int		m_iGenderPreferred;
public:
	FashionGenderOrderComponent(int iGender)
		: m_iGenderPreferred(iGender)
	{
	}
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		return pIvtrFashion->GetEssence().gender == m_iGenderPreferred ? 0 : 1;
	}
};

//	职业匹配者优先
class FashionProfessionOrderComponent : public FashionOrderComponent{
	int		m_iProfessionPreferred;
public:
	FashionProfessionOrderComponent(int iProfession)
		: m_iProfessionPreferred(iProfession)
	{
	}
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		if (!IsFashionWeapon(pIvtrFashion)){
			return 0;
		}
		if (pIvtrFashion->GetDBEssence()->character_combo_id & (1 << m_iProfessionPreferred)){
			return 0;
		}
		return 1;
	}
};

//	时装武器优先
class FashionWeaponOrderComponent : public FashionOrderComponent{
public:
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		return IsFashionWeapon(pIvtrFashion) ? 0 : 1;
	}
};

//	时装部位排序
class FashionPartOrderComponent : public FashionOrderComponent{
public:
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		enum {FASHION_COUNT = 6};
		static unsigned int s_mask[FASHION_COUNT] = {
			EQUIP_MASK64_FASHION_WEAPON,
				EQUIP_MASK64_FASHION_HEAD,
				EQUIP_MASK64_FASHION_BODY,
				EQUIP_MASK64_FASHION_LEG,
				EQUIP_MASK64_FASHION_WRIST,
				EQUIP_MASK64_FASHION_FOOT,
		};
		unsigned int mask = pIvtrFashion->GetDBSubType()->equip_fashion_mask;
		const unsigned int *begin = &s_mask[0];
		const unsigned int *end = begin + FASHION_COUNT;
		const unsigned int * it = std::find(begin, end, mask);
		return it - begin;
	}
};

//	时装武器按类型排序
class FashionWeaponTypeOrderComponent : public FashionOrderComponent{
public:
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		return IsFashionWeapon(pIvtrFashion) ? pIvtrFashion->GetDBEssence()->action_type : 0;
	}
};

//	装备部位排序
class FashionEquipLocationOrderComponent : public FashionOrderComponent{
public:
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		return pIvtrFashion->GetDBEssence()->equip_location;
	}
};

//	套装排序
class FashionSuiteOrderComponent : public FashionOrderComponent{
public:
	virtual int GetOrder(CECIvtrFashion *pIvtrFashion)const{
		int idSuite = pIvtrFashion->GetFashionSuiteID();
		return idSuite == 0 ? INT_MAX : idSuite;
	}
};

//	class FashionPackSorterFrameWork
class FashionPackSorterFrameWork : public CECHostPlayer::PackSorter{
	typedef std::vector<FashionOrderComponent *> Components;
	Components	m_components;
public:
	virtual ~FashionPackSorterFrameWork(){
		for (size_t i(0); i < m_components.size(); ++ i){
			delete m_components[i];
		}
		m_components.clear();
	}
	void AppendComponent(FashionOrderComponent *component){
		if (component){
			m_components.push_back(component);
		}
	}
	virtual bool operator()(int index1, int index2)const{
		// Return whether index1 should be put before index2
		CECIvtrFashion *pFashion1 = dynamic_cast<CECIvtrFashion *>(GetItem(index1));
		if (!pFashion1){
			// whether item2 is empty or not, need not exchange
			return false;
		}
		CECIvtrFashion *pFashion2 = dynamic_cast<CECIvtrFashion *>(GetItem(index2));
		if (!pFashion2){
			// item2 should be put after item1
			return true;
		}
		for (size_t i(0); i < m_components.size(); ++ i){
			FashionOrderComponent *p = m_components[i];
			int order1 = p->GetOrder(pFashion1);
			int order2 = p->GetOrder(pFashion2);
			if (order1 < order2){
				return true;
			}else if (order1 > order2){
				return false;
			}
		}
		return pFashion1->GetTemplateID() < pFashion2->GetTemplateID();		//	以 ID 结尾
	}
};

class FashionPackSorterPreferFashionPart : public FashionPackSorterFrameWork{
public:
	FashionPackSorterPreferFashionPart(int iGender, int iProfession){
		AppendComponent(new FashionGenderOrderComponent(iGender));			//	以性别开头
		AppendComponent(new FashionProfessionOrderComponent(iProfession));	//	再按职业切分
		AppendComponent(new FashionWeaponOrderComponent);					//	时装武器排在前
		AppendComponent(new FashionPartOrderComponent);						//	再接部位分
		AppendComponent(new FashionEquipLocationOrderComponent);			//	相同部位按装备位置分
		AppendComponent(new FashionWeaponTypeOrderComponent);				//	时装武器按类型细分
	}
};

class FashionPackSorterPreferSuite : public FashionPackSorterFrameWork{
public:
	FashionPackSorterPreferSuite(int iGender, int iProfession){
		AppendComponent(new FashionGenderOrderComponent(iGender));			//	以性别开头
		AppendComponent(new FashionProfessionOrderComponent(iProfession));	//	再按职业切分
		AppendComponent(new FashionWeaponOrderComponent);					//	时装武器排在前
		AppendComponent(new FashionSuiteOrderComponent);					//	先按套装分
		AppendComponent(new FashionPartOrderComponent);						//	同一套装按部位分
		AppendComponent(new FashionEquipLocationOrderComponent);			//	同一部位按装备位置分
		AppendComponent(new FashionWeaponTypeOrderComponent);				//	时装武器按类型细分
	}
};

void CDlgStorage::OnCommand_arrange_fashion_bypart(const char *szCommand){
	if (!IsFashionStorage()){
		return;
	}
	CECHostPlayer *pHostPlayer = GetHostPlayer();
	if (pHostPlayer->GetAutoFashion()){
		return;
	}
	if (pHostPlayer->GetCoolTime(GP_CT_MULTI_EXCHANGE_ITEM)){
		GetGame()->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return;
	}
	FashionPackSorterPreferFashionPart partSorter(pHostPlayer->GetGender(), pHostPlayer->GetProfession());
	pHostPlayer->SortPack(IVTRTYPE_TRASHBOX3, &partSorter);
}

void CDlgStorage::OnCommand_arrange_fashion_bysuite(const char *szCommand){
	if (!IsFashionStorage()){
		return;
	}
	CECHostPlayer *pHostPlayer = GetHostPlayer();
	if (pHostPlayer->GetAutoFashion()){
		return;
	}
	if (pHostPlayer->GetCoolTime(GP_CT_MULTI_EXCHANGE_ITEM)){
		GetGame()->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return;
	}
	FashionPackSorterPreferSuite suiteSorter(pHostPlayer->GetGender(), pHostPlayer->GetProfession());
	pHostPlayer->SortPack(IVTRTYPE_TRASHBOX3, &suiteSorter);
}

void CDlgStorage::OnCommand_arrange(const char *szCommand)
{
	int iPack = 0;
	if (m_szName == "Win_Storage") iPack = IVTRTYPE_TRASHBOX;
	else if (m_szName == "Win_Storage4") iPack = IVTRTYPE_GENERALCARD_BOX;
	else return;

	CECHostPlayer *pHostPlayer = GetHostPlayer();
	if (pHostPlayer->GetCoolTime(GP_CT_MULTI_EXCHANGE_ITEM))
	{
		GetGame()->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return;
	}
	pHostPlayer->SortPack(iPack);
}

void CDlgStorage::OnCommand_Btn_All(const char *szCommand){
	if (!IsFashionStorage()){
		return;
	}
	CheckRadioButton(FASHOIN_STORAGE_PART_GROUP_ID, -1);
}

void CDlgStorage::OnCommandBatchSwallow(const char *szCommand)
{
	if (m_bSwallowing) EndBatchSwallow();
	else StartBatchSwallow();
}
void CDlgStorage::StartBatchSwallow()
{
	m_bSwallowing = true;
	m_iSwallowIndex = 0;
	PAUIOBJECT pObj = GetDlgItem("Btn_BatchSwallow");
	if (pObj) pObj->SetText(GetStringFromTable(10976));
	pObj = GetDlgItem("Combo_Line");
	if (pObj) pObj->Enable(false);
	pObj = GetDlgItem("Btn_AutoOpen");
	if (pObj && !m_bAutoDice) pObj->Enable(false);
}
void CDlgStorage::EndBatchSwallow()
{
	m_bSwallowing = false;
	PAUIOBJECT pObj = GetDlgItem("Btn_BatchSwallow");
	if (pObj) pObj->SetText(GetStringFromTable(10975));
	pObj = GetDlgItem("Combo_Line");
	if (pObj && !m_bAutoDice) pObj->Enable(true);
	pObj = GetDlgItem("Btn_AutoOpen");
	if (pObj && !m_bAutoDice) pObj->Enable(true);
}
void CDlgStorage::BatchSwallow()
{
	if (m_bSwallowing && m_BatchSwallowCounter.IncCounter(GetGame()->GetRealTickTime())) {
		m_BatchSwallowCounter.Reset();
		CECInventory* pCardBox = GetHostPlayer()->GetGeneralCardBox();
		if (!pCardBox || m_iSwallowIndex >= pCardBox->GetSize()) {
			EndBatchSwallow();
			// 如果正在自动开包，吞噬完一遍仓库中的卡牌后回到开包过程
			if (m_bAutoDice) AutoDice();
			return;
		}
		int rank(0);
		PAUICOMBOBOX pCombo = dynamic_cast<PAUICOMBOBOX>(GetDlgItem("Combo_Line"));
		if (pCombo) rank = pCombo->GetCurSel();
		int index = m_iSwallowIndex;
		while (index < pCardBox->GetSize()) {
			CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pCardBox->GetItem(index++));
			if (pCard) {
				CDlgGeneralCard* pDlg = dynamic_cast<CDlgGeneralCard*>(GetAUIManager()->GetDialog("Win_GeneralCard"));
				if (pDlg && rank >= pCard->GetEssence().rank) {
					pDlg->SwallowCard(0, index - 1, 1, true);
					break;
				}
			}
		}
		m_iSwallowIndex = index;
	}
}
void CDlgStorage::OnRButtonDownItem(WPARAM, LPARAM, AUIObject *pObj)
{
	// try to trigger shortcuts
	GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts(this->GetName(), pObj);
}

void CDlgStorage::OnLButtonDownItem(WPARAM, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj || !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;

	if (IsFashionStorage())
	{
		// Forbidden drag drop when auto fashion started
		if (GetHostPlayer()->GetAutoFashion())
			return;
	}
	if (IsGeneralCardStorage())
	{
		CDlgGeneralCard* pCard = dynamic_cast<CDlgGeneralCard*>(GetAUIManager()->GetDialog("Win_GeneralCard"));
		int iDst = atoi(pObj->GetName() + strlen("Item_")) - 1;
		CECIvtrItem* pStored = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
		if (pCard && pCard->IsSwallowing() && pCard->SwallowCard(0, iDst, pStored->GetCount(), false))
			return;
	}
	
	if( ((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"))->IsFrozen() )
		return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	
	POINT pt = 
	{
		GET_X_LPARAM(lParam) - p->X,
		GET_Y_LPARAM(lParam) - p->Y
	};

	GetGameUIMan()->InvokeDragDrop(this, pObj, pt);
	GetGameUIMan()->PlayItemSound((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"), false);
}

void CDlgStorage::OnLButtonDbClickItem(WPARAM, LPARAM lParam, AUIObject *pObj)
{
	if (!pObj || !pObj->GetDataPtr("ptr_CECIvtrItem")){
		return;
	}	
	if (!IsFashionStorage()){
		return;
	}
	CECIvtrItem *pItem = (CECIvtrItem *)(pObj->GetDataPtr("ptr_CECIvtrItem"));
	int iEquippedSlot = pItem->GetEquippedSlot();
	if (iEquippedSlot < 0){
		return;
	}
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	CDlgDragDrop* pDragDrop = dynamic_cast<CDlgDragDrop*>(pGameUIMan->GetDialog("DragDrop"));
	PAUIDIALOG pDlgOver = pGameUIMan->GetDialog("Win_Inventory");
	PAUIOBJECT pObjOver = pDlgOver->GetDlgItem(AString().Format("Equip_%02d", iEquippedSlot));
	pDragDrop->OnStorageDragDrop(this, pObj, pDlgOver, pObjOver, pItem);
}

void CDlgStorage::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_pScl_Right!=NULL && m_pScl_Right->IsShow())
	{
		int zDelta = (short)HIWORD(wParam);
		zDelta /= 120;
		m_pScl_Right->SetBarLevel(m_pScl_Right->GetBarLevel() - zDelta);
	}
}

bool CDlgStorage::Tick(void)
{
	if (IsGeneralCardStorage())	BatchSwallow();
	UpdateStorage();
	return CDlgBase::Tick();
}

bool CDlgStorage::UpdateStorage()
{
	int i;
	AString strFile;
	ACHAR szText[40];
	CECIvtrItem *pItem;
	PAUIIMAGEPICTURE pObj, pLast = NULL;
	CECHostPlayer *pHost = GetHostPlayer();
	CECInventory *pInventory = GetInventory();
	PAUIOBJECT pObj1 = GetDlgItem("Rdo_Storage1");
	if (pObj1)
		pObj1->Enable(pHost->GetTrashBox2()->GetSize() > 0);
	/*GetDlgItem("Rdo_Storage2")->Enable(pHost->GetTrashBox3()->GetSize() > 0);*/
	a_sprintf(szText, _AL("%d"), GetMoneyCnt());
	PAUIOBJECT pGold = GetDlgItem("Txt_Gold");
	if (pGold) pGold->SetText(szText);
	
	int inventorySize = pInventory->GetSize();
	if (pInventory == pHost->GetAccountBox())
	{
		// Test if the latter part of the account box is empty
		//
		int i(0);
		for (i=inventorySize/2; i<inventorySize; ++i)
		{
			if (pInventory->GetItem(i))
				break;
		}
		if (i==inventorySize)
		{
			// The latter half part of the account box is empty
			// Show only the first half part as if the account box is only half-sized
			inventorySize /= 2;
		}
	}
	
	bool bFashionStorage = IsFashionStorage();
	bool bGeneralCardStorage = IsGeneralCardStorage();
	if (bFashionStorage || bGeneralCardStorage)
	{
		// 针对时装包裹，显示/隐藏滚动条
		//
		int nMaxDisplayItem = (int)m_vecImgCell.size();
		int nMaxDisplayLine = nMaxDisplayItem/FASHION_PER_LINE;
		int nLine = inventorySize/FASHION_PER_LINE;

		if (nLine <= nMaxDisplayLine)
		{
			m_pScl_Right->SetBarLevel(0);
			m_pScl_Right->Show(false);
		}
		else
		{
			m_pScl_Right->SetScrollRange(0, nLine-nMaxDisplayLine);
			m_pScl_Right->SetScrollStep(1);
			m_pScl_Right->SetBarLength(-1);
			m_pScl_Right->Show(true);
		}
	}

	int iSlotStart(0);
	if (bFashionStorage || bGeneralCardStorage)
	{
		// 时装可能带滚动条，显示
		iSlotStart = m_pScl_Right->GetBarLevel();
		iSlotStart*=FASHION_PER_LINE;
	}

	AString strName;
	char szName[20];
	for( i = 0; i < (int)m_vecImgCell.size(); i++ )
	{
		int iSlot = iSlotStart+i;

		pObj = m_vecImgCell[i];
		PAUIIMAGEPICTURE pBackImg = m_vecImgBg[i];

		// 根据起始位置重命名
		if (bFashionStorage || bGeneralCardStorage)
		{
			sprintf(szName, "Item_%02d", iSlot+1);
			pObj->SetName(szName);
		}

		if (iSlot < inventorySize)
		{
			pObj->Show(true);
			pBackImg->Show(true);
			pItem = pInventory->GetItem(iSlot);
			if( pItem )
			{
				pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
				pObj->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, reinterpret_cast<unsigned long>(pItem), NULL, NULL);
				if( pItem->GetCount() > 1 )
				{
					a_sprintf(szText, _AL("%d"), pItem->GetCount());
					pObj->SetText(szText);
				}
				else
					pObj->SetText(_AL(""));

				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

				if( pItem->IsFrozen() )
					pObj->SetColor(A3DCOLORRGB(128, 128, 128));
				else if( pItem->IsEquipment() && !pHost->CanUseEquipment((CECIvtrEquip *)pItem) )
					pObj->SetColor(A3DCOLORRGB(192, 0, 0));
				else if (pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
					pObj->SetColor(A3DCOLORRGB(128, 128, 128));
				else
					pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			}
			else
			{
				pObj->ClearCover();
				pObj->SetText(_AL(""));
				pObj->SetDataPtr(NULL);
				pObj->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, NULL, NULL, NULL);
				pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			}
		}
		else
		{
			pObj->Show(false);
			pBackImg->Show(false);
			if( !pLast && i>0) pLast = m_vecImgCell[i - 1];
		}
	}

	if (bFashionStorage)
	{
		// 计算各类时装的个数
		static unsigned int fashion_submask[FASHION_COUNT] =
		{
			EQUIP_MASK64_FASHION_HEAD,
				EQUIP_MASK64_FASHION_BODY,
				EQUIP_MASK64_FASHION_LEG,
				EQUIP_MASK64_FASHION_FOOT,
				EQUIP_MASK64_FASHION_WRIST,
				EQUIP_MASK64_FASHION_WEAPON,
		};
		int nCount[FASHION_COUNT] = {0};
		int nTotalCount(0);
		
		for (i=0; i<inventorySize; ++i)
		{
			pItem = pInventory->GetItem(i);
			if (!pItem)
				continue;
			
			for (int j = 0; j < FASHION_COUNT; ++j)
			{
				if (((CECIvtrFashion *)pItem)->GetDBSubType()->equip_fashion_mask == fashion_submask[j])
				{
					nCount[j]++;
					nTotalCount ++;
					break;
				}
			}
		}
		
		// 显示时装计数
		ACString strCount;
		for (i = 0; i < FASHION_COUNT; i++)
		{
			if (m_pTxt_Fashion[i])
			{
				strCount.Format(_AL("%d"), nCount[i]);
				m_pTxt_Fashion[i]->SetText(strCount);
			}
		}
		if (m_pTxt_FashionAll){
			strCount.Format(_AL("%d"), nTotalCount);
			m_pTxt_FashionAll->SetText(strCount);
		}
	}

	if( !pLast ) pLast = m_vecImgCell[m_vecImgCell.size() - 1];

	POINT pt = pLast->GetPos(true);	
	int nHeight = pt.y + m_pAUIFrameDlg->GetSizeLimit().cy / 2;
	if (inventorySize>0)
	{
		SIZE s = pLast->GetSize();
		nHeight += s.cy;
	}
	SetSize(GetSize().cx, nHeight);
	if (IsGeneralCardStorage()) UpdateDiceImg();
	return true;
}

int CDlgStorage::GetMoneyCnt()
{
	int money(0);
	if (m_szName == "Win_Storage3")
		money = GetHostPlayer()->GetAccountBoxMoneyCnt();
	else
		money = GetHostPlayer()->GetTrashBoxMoneyCnt();
	return money;
}

bool CDlgStorage::IsFashionStorage()
{
	return stricmp(m_szName, "Win_Storage2") == 0;
}
bool CDlgStorage::IsGeneralCardStorage()
{
	return stricmp(m_szName, "Win_Storage4") == 0;
}

bool CDlgStorage::OnChangeLayoutBegin()
{
	if(m_pScl_Right) m_pScl_Right->SetBarLevel(0);
	UpdateStorage();
	return true;
}

CECInventory* CDlgStorage::GetInventory()
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECInventory *pInventory = NULL;
	if( stricmp(m_szName, "Win_Storage") == 0 )
		pInventory = pHost->GetTrashBox();
	else if( stricmp(m_szName, "Win_Storage1") == 0 )
		pInventory = pHost->GetTrashBox2();
	else if (stricmp(m_szName, "Win_Storage3") == 0)
		pInventory = pHost->GetAccountBox();
	else if ( stricmp(m_szName, "Win_Storage2") == 0)
		pInventory = pHost->GetTrashBox3();
	else pInventory = pHost->GetGeneralCardBox();

	return pInventory;
}
PAUIOBJECT CDlgStorage::GetEmptySlot(CECIvtrItem* pIvtrSrc)
{
	CECInventory* pInventory = GetInventory();
	PAUIOBJECT pObjFound = NULL;
	bool bFind = false;
	int inv_size = pInventory->GetSize();
	CECIvtrItem* pStored = NULL;
	int i = 0;
	// 看有没有可以堆叠的物品
	for (i = 0; i < inv_size; ++i) {
		pStored = pInventory->GetItem(i);
		if (pStored && pStored->GetTemplateID() == pIvtrSrc->GetTemplateID() &&
			pStored->GetCount() + pIvtrSrc->GetCount() < pStored->GetPileLimit()) {
			bFind = true;
			break;
		}
	}
	// 找空位
	if (!bFind) {
		for (i = 0; i < inv_size; ++i) {
			pStored = pInventory->GetItem(i);
			if (pStored == NULL) {
				bFind = true;
				break;
			}
		}
	}
	if (bFind) {
		// 时装和卡牌背包是带滚动条的
		if (IsFashionStorage() || IsGeneralCardStorage()) {
			int nMaxDisplayItem = (int)m_vecImgCell.size();
			int nMaxDisplayLine = nMaxDisplayItem/FASHION_PER_LINE;
			int bar_level = 0;
			if (i >= nMaxDisplayItem)
				bar_level = (i + 1) / FASHION_PER_LINE - nMaxDisplayLine + 1;
			m_pScl_Right->SetBarLevel(bar_level);
			UpdateStorage();
		}
		AString strName;
		strName.Format("Item_%02d", i + 1);
		pObjFound = GetDlgItem(strName);
	}
	return pObjFound;
}

void CDlgStorage::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (IsGeneralCardStorage() && OnDiceDragDrop(pIvtrSrc, iSrc, pObjOver)) 
		return;
	// for right-click scene
	if(!pObjOver)
	{
		pObjOver = GetEmptySlot(pIvtrSrc);
	}

	// for drag-drop scene
	if( !pObjOver || !strstr(pObjOver->GetName(), "Item_") )
	{
		return;
	}

	CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem");
	int iDstTrashBox = IVTRTYPE_TRASHBOX + atoi(this->GetName() + strlen("Win_Storage"));

	if( iDstTrashBox == IVTRTYPE_TRASHBOX2 && 
		!(pIvtrSrc->GetClassID() == CECIvtrItem::ICID_MATERIAL || pIvtrSrc->GetClassID() == CECIvtrItem::ICID_SKILLTOME || pIvtrSrc->GetClassID() == CECIvtrItem::ICID_TASKNMMATTER )){
		return;
	}

	if (iDstTrashBox == IVTRTYPE_TRASHBOX3 &&
		(pIvtrSrc->GetClassID() != CECIvtrItem::ICID_FASHION ||	GetHostPlayer()->GetAutoFashion())){
		return;
	}
	
	if (iDstTrashBox == IVTRTYPE_GENERALCARD_BOX &&
		pIvtrSrc->GetClassID() != CECIvtrItem::ICID_GENERALCARD &&
		pIvtrSrc->GetClassID() != CECIvtrItem::ICID_GENERALCARD_DICE)
	{
		return;
	}
	
	if (iDstTrashBox == IVTRTYPE_ACCOUNT_BOX &&	!pIvtrSrc->CanPutIntoAccBox())
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(8081), GP_CHAT_MISC);
		return;
	}
	
	int iDst = atoi(pObjOver->GetName() + strlen("Item_")) - 1;
	if( pIvtrDst )
	{
		if( pIvtrSrc->GetPileLimit() > 1 &&
			pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
		{
			if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 )
			{
				GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
					CDlgInputNO::INPUTNO_STORAGE_PUT_ITEMS, pIvtrSrc->GetCount());
			}
			else
			{
				GetGameSession()->c2s_CmdMoveIvtrToTrashBox(iDstTrashBox, iDst, iSrc,
					min(pIvtrSrc->GetCount(), pIvtrDst->GetPileLimit() - pIvtrDst->GetCount()));
			}
		}
		else
			GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(iDstTrashBox, iDst, iSrc);
	}
	else
	{
		if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 && pIvtrSrc->GetPileLimit() > 1 )
		{
			GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
				CDlgInputNO::INPUTNO_STORAGE_PUT_ITEMS, pIvtrSrc->GetCount());
		}
		else
			GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(iDstTrashBox, iDst, iSrc);
	}
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
}

bool CDlgStorage::OnDiceDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjOver)
{
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
	if (pImg 
		&& stricmp(pImg->GetName(), "Img_Dice") == 0
		&& pIvtrSrc 
		&& pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GENERALCARD_DICE){
		PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
		if (pObj) pObj->SetData(iSrc);
		pImg->SetDataPtr(pIvtrSrc,"ptr_CECIvtrItem");
		UpdateDiceImg();
		return true;
	}
	return false;
}

void CDlgStorage::OnLButtonDownCardDice(WPARAM, LPARAM, AUIObject *pObj)
{
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if (pImg) {
		pImg->ClearCover();
		pImg->SetText(_AL(""));
		pImg->SetDataPtr(NULL);
		pImg->SetColor(A3DCOLORRGB(255, 255, 255));
		PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
		if (pObj) pObj->SetData(-1);
	}
}

void CDlgStorage::OnCommmandAutoDice(const char *szCommand)
{
	if (m_bAutoDice) EndAutoDice();
	else StartAutoDice();
}

void CDlgStorage::StartAutoDice()
{
	m_bAutoDice = true;
	PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
	if (pObj) pObj->SetText(GetStringFromTable(9632));
	pObj = GetDlgItem("Combo_Line");
	if (pObj) pObj->Enable(false);	
	pObj = GetDlgItem("Btn_BatchSwallow");
	if (pObj) pObj->Enable(false);
	pObj = GetDlgItem("Chk_BatchSwallow");
	if (pObj) pObj->Enable(false);
	AutoDice();
}
void CDlgStorage::EndAutoDice()
{
	m_bAutoDice = false;
	PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
	if (pObj) pObj->SetText(GetStringFromTable(10980));
	pObj = GetDlgItem("Btn_BatchSwallow");
	if (pObj) pObj->Enable(true);
	pObj = GetDlgItem("Chk_BatchSwallow");
	if (pObj) pObj->Enable(true);
	EndBatchSwallow();
}
void CDlgStorage::AutoDice()
{
	if (m_bAutoDice && !m_bSwallowing) {
		PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
		if (pObj) {
			int iSrc = pObj->GetData();
			if (iSrc != -1) {
				CECIvtrItem* pDice = GetHostPlayer()->GetPack()->GetItem(iSrc);
				pObj = GetDlgItem("Img_Dice");
				// 卡牌包耗尽时停止自动开包过程
				if (!pDice || pDice->GetCount() <= 0) {
					OnLButtonDownCardDice(0, 0, pObj);
					EndAutoDice();
					return;
				}
				CECInventory* pInventory = GetHostPlayer()->GetGeneralCardBox();
				if (pInventory->GetEmptySlotNum() == 0) {
					PAUICHECKBOX pCheck = dynamic_cast<PAUICHECKBOX>(GetDlgItem("Chk_BatchSwallow"));
					// 卡牌仓库已满时，并勾选了批量吞噬，则进入批量吞噬过程
					if (pCheck->IsChecked()) StartBatchSwallow();
					// 否则结束自动开包过程
					else EndAutoDice();
					return;
				}
				GetHostPlayer()->UseItemInPack(IVTRTYPE_PACK, iSrc);
			} else EndAutoDice();
		}
	}
}

void CDlgStorage::UpdateDiceImg()
{
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Dice"));
	PAUIOBJECT pObj = GetDlgItem("Btn_AutoOpen");
	if (pObj == NULL) return;
	int iSrc = pObj->GetData();
	if (pImg && iSrc != -1) {
		CECIvtrItem* pIvtrSrc = GetHostPlayer()->GetPack()->GetItem(iSrc);
		if (pIvtrSrc 
			&& pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GENERALCARD_DICE
			&& pIvtrSrc == pImg->GetDataPtr("ptr_CECIvtrItem")) {
			ACHAR szText[40];
			if( pIvtrSrc->GetCount() > 1 ) {
				a_sprintf(szText, _AL("%d"), pIvtrSrc->GetCount());
				pImg->SetText(szText);
			} else pImg->SetText(_AL(""));
			
			AString strFile;
			af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
			strFile.MakeLower();
			pImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			
			if( pIvtrSrc->IsFrozen() ) pImg->SetColor(A3DCOLORRGB(128, 128, 128));
			else pImg->SetColor(A3DCOLORRGB(255, 255, 255));
		} else OnLButtonDownCardDice(0, 0, pImg);
	}
}

int CDlgStorage::GetHighlightedFashionEquipMask(){
	int iButton = GetCheckedRadioButton(FASHOIN_STORAGE_PART_GROUP_ID);
	static unsigned int s_mask[FASHION_COUNT] = {
		EQUIP_MASK64_FASHION_HEAD,
			EQUIP_MASK64_FASHION_BODY,
			EQUIP_MASK64_FASHION_LEG,
			EQUIP_MASK64_FASHION_WRIST,
			EQUIP_MASK64_FASHION_FOOT,
			EQUIP_MASK64_FASHION_WEAPON,
	};
	if (iButton >= 1 && iButton <= FASHION_COUNT){
		return s_mask[iButton-1];
	}
	return 0;
}

bool CDlgStorage::Render(){
	if (!CDlgBase::Render()){
		return false;
	}
	if (IsFashionStorage()){
		unsigned int highlightedFashionEquipMask = GetHighlightedFashionEquipMask();
		if (highlightedFashionEquipMask != 0){
			CECGameUIMan *pGameUIMan = GetGameUIMan();
			for (int i = 0; i < (int)m_vecImgCell.size(); i++){
				PAUIOBJECT pObj = m_vecImgCell[i];
				if (!pObj->IsShow()){
					continue;
				}
				CECIvtrItem *pIvtrItem = (CECIvtrItem *)(pObj->GetDataPtr("ptr_CECIvtrItem"));
				if (!pIvtrItem){
					continue;
				}
				CECIvtrFashion * pIvtrFashion = dynamic_cast<CECIvtrFashion*>(pIvtrItem);
				if (!pIvtrFashion){
					continue;
				}
				if (pIvtrFashion->GetDBSubType()->equip_fashion_mask != highlightedFashionEquipMask){
					continue;
				}
				pGameUIMan->RenderCover(pObj);
			}
		}
	}
	return true;
}