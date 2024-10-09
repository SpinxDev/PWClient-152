// Filename	: DlgEquipUp.cpp
// Creator	: Xu Wenbin
// Date		: 2009/10/16

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgEquipUp.h"
#include "DlgEquipUpEnsure.h"
#include "DlgProduce.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrStone.h"
#include "EC_Game.h"
#include "EC_IvtrMaterial.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_UIHelper.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipUp, CDlgBase)
AUI_ON_COMMAND("confirm",  OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

CDlgEquipUp::CDlgEquipUp()
{
	m_pTxt_Name = NULL;
	m_pChk_InheritAll = NULL;
	m_bInheritAll = false;
	m_pBtn_Confirm = NULL;
	m_pTxt_Num = NULL;
	m_pOldEquip = NULL;
	
	m_addonsNum = 0;
	m_pProperties.clear();
}
CDlgEquipUp::~CDlgEquipUp()
{

}

bool CDlgEquipUp::Release()
{
	for (int i = 0; i < (int)m_pProperties.size();i++){
		if (m_pProperties[i]){
			delete m_pProperties[i];
		}
	}
	return CDlgBase::Release();
}	

bool CDlgEquipUp::OnInitDialog()
{
	CDlgBase::OnInitDialog();
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Chk_ChooseAll", m_pChk_InheritAll);
	CECEquipUpInheritRefine* m_pInheritRefine = new CECEquipUpInheritRefine(this);
	
	DDX_Control("Txt_EquipRefine", m_pInheritRefine->m_pTxt_EquipRefine);
	DDX_Control("Txt_Num1", m_pInheritRefine->m_pTxt_Num);
	DDX_Control("Rdo_Yes1", m_pInheritRefine->m_pRdo_Inherit);	
	DDX_Control("Rdo_No1", m_pInheritRefine->m_pRdo_NoInherit);
	m_pProperties.push_back(m_pInheritRefine);
	
	CECEquipUpInheritStoneHole* m_pInheritStoneHole = new CECEquipUpInheritStoneHole(this);
	
	DDX_Control("Txt_EquipHole", m_pInheritStoneHole->m_pInheritHole->m_pTxt_EquipHole);
	DDX_Control("Txt_Num2", m_pInheritStoneHole->m_pInheritHole->m_pTxt_Num);
	DDX_Control("Rdo_Yes2", m_pInheritStoneHole->m_pInheritHole->m_pRdo_Inherit);	
	DDX_Control("Rdo_No2", m_pInheritStoneHole->m_pInheritHole->m_pRdo_NoInherit);
	
	DDX_Control("Txt_Stone", m_pInheritStoneHole->m_pInheritStone->m_pTxt_Stone);
	DDX_Control("Txt_Num3", m_pInheritStoneHole->m_pInheritStone->m_pTxt_Num);
	DDX_Control("Rdo_Yes3", m_pInheritStoneHole->m_pInheritStone->m_pRdo_Inherit);	
	DDX_Control("Rdo_No3", m_pInheritStoneHole->m_pInheritStone->m_pRdo_NoInherit);
	m_pProperties.push_back(m_pInheritStoneHole);
	
	CECEquipUpInheritAddons* m_pInheritAddons = new CECEquipUpInheritAddons(this);
	
	DDX_Control("Txt_Nature", m_pInheritAddons->m_pTxt_Addons);
	DDX_Control("Txt_NatureCost", m_pInheritAddons->m_pTxt_Num);
	DDX_Control("Rdo_Yes5", m_pInheritAddons->m_pRdo_Inherit);
	DDX_Control("Rdo_No5", m_pInheritAddons->m_pRdo_NoInherit);
	m_pProperties.push_back(m_pInheritAddons);
	AString ItemName[] = {"Txt_Nature","Txt_NatureCost","Rdo_Yes5","Rdo_No5","Lbl_NatureAtt","Img_BG06"};
	m_pInheritAddons->InitialDialogItem(ItemName, 6);
	
	CECEquipUpInheritEngrave* m_pInheritEngrave = new CECEquipUpInheritEngrave(this);
	
	DDX_Control("Txt_Engrave", m_pInheritEngrave->m_pTex_Engrave);
	DDX_Control("Txt_Num4", m_pInheritEngrave->m_pTxt_Num);
	DDX_Control("Rdo_Yes4", m_pInheritEngrave->m_pRdo_Inherit);	
	DDX_Control("Rdo_No4", m_pInheritEngrave->m_pRdo_NoInherit);
	m_pProperties.push_back(m_pInheritEngrave);
	//添加相关控件，需要隐藏的
	AString ItemName2[] = {"Txt_Engrave","Txt_Num4","Rdo_Yes4","Rdo_No4","Lbl_EName","Img_CutOff03","Img_BG08"};
	m_pInheritEngrave->InitialDialogItem(ItemName2, 7);
	
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Txt_Num", m_pTxt_Num);
	
	AString ItemName3[] = {"Txt_Num","Img_BG07","Btn_Confirm","Btn_Cancel", "Img_Picture"};
	AddMovableItem(ItemName3, 5);
	AString ItemName4[] = {"Edi_BGBig","Edi_Bg","Img_Stand01","Img_Stand02","Img_Stand03"};
	AddResizableItem(ItemName4, 5);
	return true;
	
}

void CDlgEquipUp::showDialog()
{
	// Set default value of all
	m_pTxt_Name->SetText(_AL(""));
	
	m_pChk_InheritAll->Check(false);
	m_pChk_InheritAll->Enable(false);
	
	for (int i = 0; i < (int)m_pProperties.size();i++){
		m_pProperties[i]->OnInitial();
	}
	
	m_pBtn_Confirm->Enable(false);
	m_pTxt_Num->SetText(_AL(""));
	
	// Initialize label
	CECIvtrItem *pItem = m_pOldEquip;
	if (pItem && pItem->IsEquipment()){
		CECIvtrEquip *pEquip = static_cast<CECIvtrEquip *>(pItem);
		
		m_pTxt_Name->SetText(pItem->GetName());
		PrepareShowData(pEquip);
	}
    // 设置“继承所有属性”按钮是否可选
	if( CanInheritAll( GetWealth() ) )
	{
		m_pChk_InheritAll->Enable(true);
	}
	
	int deltaY(0);
	for (i = 0; i < (int)m_pProperties.size();i++){
		m_pProperties[i]->MoveAllItem(deltaY);
		if (!m_pProperties[i]->m_bActive && !m_pProperties[i]->m_bHided){			
			m_pProperties[i]->m_bHided = true;
			deltaY -= m_pProperties[i]->m_iItemTotalHeight;
		}
		if (m_pProperties[i]->m_bActive && m_pProperties[i]->m_bHided){
			m_pProperties[i]->m_bHided = false;
			deltaY += m_pProperties[i]->m_iItemTotalHeight;
		}
		m_pProperties[i]->HideAllItem(m_pProperties[i]->m_bHided);		
	}
	for (i= 0; i< (int)m_pMovableItem.size();i++){
		POINT Pos = m_pMovableItem[i]->GetDefaultPos();
		m_pMovableItem[i]->SetDefaultPos(Pos.x, Pos.y + deltaY);
	}
	for (i= 0; i< (int)m_pResizableItem.size();i++){
		SIZE objSize = m_pResizableItem[i]->GetDefaultSize();
		m_pResizableItem[i]->SetDefaultSize(objSize.cx, objSize.cy+deltaY);
	}
}

void CDlgEquipUp::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	showDialog();
}

int CDlgEquipUp::GetWealth()
{
	// 获取人物背包中用于“装备升级”的材料（乾坤石）数量
	return GetHostPlayer()->GetPack()->GetItemTotalNum(FEE_ITEM_ID);
}

bool CDlgEquipUp::Tick()
{
	if( m_pChk_InheritAll->IsChecked() && !m_bInheritAll ){
		for(int i = 0; i < m_pProperties.size(); ++i){
			if( m_pProperties[i]->m_bActive ){
				m_pProperties[i]->CheckYes();
			}
		}
		m_bInheritAll = true;
	}
	else if( !m_pChk_InheritAll->IsChecked() && m_bInheritAll )
	{
		showDialog();
		m_bInheritAll = false;
	}
	else{
		// 更新各个继承项按钮,判断是否有足够的乾坤石使其可选，若不可选则默认选中“不继承（no）”按钮
		int wealthLeft = GetWealthLeft();
		UpdateInheritInput(wealthLeft);
		// 更新“继承所有属性”按钮的状态
		UpdateInheritAllCheckBox();
	}
	// Update confirm button enable state
	m_pBtn_Confirm->Enable(CanConfirm());
	
	// Update fee needed for all
	if (!m_pBtn_Confirm->IsEnabled()){
		// Show none when input invalid
		m_pTxt_Num->SetText(_AL(""));
	}
	else{
		// Get fee needed
		int fee = GetFee();
		// Generate string for display
		ACString strNum;
		if ( fee > 0 )
			strNum.Format(GetStringFromTable(8063), fee);
		else
			strNum = GetStringFromTable(8064);
		
		m_pTxt_Num->SetText(strNum);
		m_pTxt_Num->SetData(fee);
	}
	
	return CDlgBase::Tick();
}

void CDlgEquipUp::SetEquipUp(CECIvtrItem *pOldEquip, CECIvtrRecipe *pRecipe, bool bAdvanced)
{
	// Set new equipment before shown
	if (IsShow())
		return;

	ASSERT(pOldEquip !=	NULL);
	ASSERT(pRecipe != NULL);

	if (pOldEquip && pRecipe){		
		m_pOldEquip = pOldEquip;
		m_pRecipe = pRecipe;
	}
	for (int i = 0; i < (int)m_pProperties.size();i++){
		m_pProperties[i]->CheckAdvancedshow(bAdvanced);
	}

}

void CDlgEquipUp::OnCommand_Confirm(const char *szCommand)
{
	char inherit_type(0);
	char inherit_active(0);
	if (!CanConfirm(&inherit_type, &inherit_active))
		return;
		
	CECIvtrEquip *pEquip = NULL;
	if (m_pOldEquip && m_pOldEquip->IsEquipment())
		pEquip = static_cast<CECIvtrEquip *>(m_pOldEquip);
	
	for (int i = 0; i < (int)m_pProperties.size();i++){
		if (!m_pProperties[i]->CheckForcedInherit(pEquip)){
			GetGameUIMan()->MessageBox("", GetStringFromTable(8065), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}
	
	CDlgEquipUpEnsure * pEquipUpEnsure = GetGameUIMan()->m_pDlgEquipUpEnsure;
	pEquipUpEnsure->SetInheritStatus(inherit_type, inherit_active, (int)m_pTxt_Num->GetData());
	pEquipUpEnsure->Show(true, true);
}

void CDlgEquipUp::OnCommand_CANCEL(const char * szCommand)
{
	m_pOldEquip = NULL;
	m_pRecipe = NULL;
	m_addonsNum = 0;
	Show(false);
}

bool CDlgEquipUp::CanInheritAll(int wealthLeft)
{
	bool result(true);
	for(int i = 0; i < (int)m_pProperties.size(); ++i){
		if( !m_pProperties[i]->CanCheckYes(wealthLeft) ){
			result = false;
			break;
		}
		wealthLeft -= m_pProperties[i]->m_iInheritFee;
	}
	return result;
}

bool CDlgEquipUp::IsAllPropertiesCheckedYes()
{
	bool result(true);
	for(int i = 0; i < m_pProperties.size(); ++i){
		if( m_pProperties[i]->m_bActive ){
			if( !m_pProperties[i]->IsYesChecked() ){
				result = false;
				break;
			}
		}
	}
	return result;
}

bool CDlgEquipUp::IsAnyPropertiesCheckedNo()
{
	bool result(false);
	for(int i = 0; i < m_pProperties.size(); ++i){
		if( m_pProperties[i]->m_bActive ){
			if( m_pProperties[i]->IsNoChecked() )
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

void CDlgEquipUp::UpdateInheritAllCheckBox()
{
	if( IsAllPropertiesCheckedYes() ){
		m_pChk_InheritAll->Check(true);
		m_bInheritAll = true;
	}
	else if( IsAnyPropertiesCheckedNo() ){
		m_pChk_InheritAll->Check(false);
		m_bInheritAll = false;
	}	
}

bool CDlgEquipUp::CanConfirm( char* inheritState , char* activestatus)
{
	bool result(true);
	for (int i = 0; i < (int)m_pProperties.size(); ++i){
		if( !m_pProperties[i]->CanInherit(inheritState, activestatus) )
			result = false;
	}
	return result;
}

void CDlgEquipUp::PrepareShowData( CECIvtrEquip *pEquip )
{
	for (int i = 0; i < (int)m_pProperties.size();i++){
		m_pProperties[i]->SetInheritData(pEquip);
		m_pProperties[i]->CalculateInheritFee(pEquip);
		m_pProperties[i]->OnShow();
	}
}

void CDlgEquipUp::UpdateInheritInput( int wealthLeft )
{
	for (int i = 0; i < (int)m_pProperties.size();i++){
		m_pProperties[i]->UpdateInput(wealthLeft);
	}
}
int CDlgEquipUp::GetFee()
{
	// 计算当前合法输入下需要的费用（需要的“乾坤石”的数目）
	int fee(0);

	for(int i = 0; i < (int)m_pProperties.size(); ++i){
		if( m_pProperties[i]->CanInherit() )
			fee += m_pProperties[i]->GetInheritFee();
	}
	return fee;
}

int CDlgEquipUp::GetWealthLeft()
{
	// Get wealth left for valid input
	// Used to update input disable/enable
	// 
	int wealthLeft = GetWealth()- GetFee();
	return wealthLeft;
}

void CDlgEquipUp::AddMovableItem( const AString* itemName, int num )
{
	for (int i = 0; i<num; i++){
		PAUIOBJECT pObj = GetDlgItem(itemName[i]);
		if (pObj)
			m_pMovableItem.push_back(pObj);
	}
}

void CDlgEquipUp::AddResizableItem( const AString* itemName, int num )
{
	for (int i = 0; i<num; i++){
		PAUIOBJECT pObj = GetDlgItem(itemName[i]);
		if (pObj)
			m_pResizableItem.push_back(pObj);
	}
}

void CDlgEquipUp::SetInheritableAddons( int num, int* addons )
{
	if (num <= 0){
		return;
	}
	m_addonsNum = num;
	for (int i = 0 ; i< num; i++){
		m_addonsId[i] = addons[i];
	}
}

void CECEquipUpInheritProperty::OnInitial()
{
	
}

CECEquipUpInheritProperty::CECEquipUpInheritProperty(CDlgEquipUp* pDlg)
{
	m_bActive			= true;
	m_iInheritFee		= 0;
	m_pRdo_Inherit		= NULL;
	m_pRdo_NoInherit	= NULL;
	m_pTxt_Num			= NULL;
	m_bHided			= false;
	m_iItemTotalHeight	= 0;	
	this->pDlg			= pDlg;
}

int CECEquipUpInheritProperty::GetInheritFee()
{
	return m_pRdo_Inherit->IsChecked() ? m_iInheritFee : 0;
}

bool CECEquipUpInheritProperty::IsYesChecked()
{
	return m_pRdo_Inherit->IsChecked();
}

bool CECEquipUpInheritProperty::IsNoChecked()
{
	return m_pRdo_NoInherit->IsChecked();
}

void CECEquipUpInheritProperty::InvalidateButton(bool bEnable)
{
	m_pRdo_Inherit->Enable(bEnable);
	m_pRdo_NoInherit->Enable(bEnable);
}

bool CECEquipUpInheritProperty::IsWealthEnough(int wealthLeft)
{
	if (CanInherit())
		wealthLeft += GetInheritFee();
	
	// Enable/disable refine yes input
	if (wealthLeft >= m_iInheritFee){
		// Enable input yes
		m_pRdo_Inherit->Enable(true);
		m_pRdo_NoInherit->Enable(true);
		return true;
	}else{
		// Disable input yes (accept no only)
		m_pRdo_Inherit->Enable(false);
		m_pRdo_NoInherit->Enable(false);
		return false;
	}
}

void CECEquipUpInheritProperty::InitialDialogItem(const AString* itemName, int num)
{
	int mLow(2000), mHigh(0);
	for (int i = 0; i<num; i++){
		PAUIOBJECT pObj = pDlg->GetDlgItem(itemName[i]);
		if (pObj){
			m_pRelativeControl.push_back(pObj);
			POINT pos = pObj->GetDefaultPos();
			SIZE size = pObj->GetDefaultSize();
			if (pos.y < mLow){
				mLow = pos.y;
			}
			if (pos.y + size.cy > mHigh){
				mHigh = pos.y + size.cy;
			}
		}
	}
	if (mHigh - mLow > 0)
		m_iItemTotalHeight = mHigh - mLow;
}

void CECEquipUpInheritProperty::HideAllItem(bool bHide)
{
	for (int i = 0 ; i < m_pRelativeControl.size(); i++){
		m_pRelativeControl[i]->Show(!bHide);
	}
}

void CECEquipUpInheritProperty::MoveAllItem(int y /*= 0*/ )
{
	for (int i = 0 ; i < m_pRelativeControl.size(); i++){
		POINT Pos = m_pRelativeControl[i]->GetDefaultPos();
		m_pRelativeControl[i]->SetDefaultPos(Pos.x, Pos.y + y);
	}
}

int CECEquipUpInheritProperty::GetRefineFee(int level)
{
	elementdataman *pDataMan = CECUIHelper::GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	UPGRADE_PRODUCTION_CONFIG * pUpgradeData = (UPGRADE_PRODUCTION_CONFIG *)pDataMan->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, DataType);
	int fee(0);
	if (pUpgradeData){
		int nMaxLevel = sizeof(pUpgradeData->num_refine)/sizeof(pUpgradeData->num_refine[0]);
		if (level >= 1 && level <= nMaxLevel)
		{
			fee = pUpgradeData->num_refine[level-1];
			fee = GetScaledFee(fee);
		}
	}
	return fee;
}

int CECEquipUpInheritProperty::GetWeaponFee(int level, int nHole)
{
	elementdataman *pDataMan = CECUIHelper::GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	UPGRADE_PRODUCTION_CONFIG * pUpgradeData = (UPGRADE_PRODUCTION_CONFIG *)pDataMan->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, DataType);
	int fee(0);
	if (pUpgradeData){
		int nMaxHole = sizeof(pUpgradeData->num_weapon)/sizeof(pUpgradeData->num_weapon[0]);
		int nMaxLevel = sizeof(pUpgradeData->num_weapon[0])/sizeof(pUpgradeData->num_weapon[0][0]);
		if (level >= 1 && level <= nMaxLevel &&
			nHole >= 1 && nHole <= nMaxHole){
			fee = pUpgradeData->num_weapon[nHole-1][level-1];
			fee = GetScaledFee(fee);
		}
	}
	return fee;
}

int CECEquipUpInheritProperty::GetArmorFee(int level, int nHole)
{
	elementdataman *pDataMan = CECUIHelper::GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	UPGRADE_PRODUCTION_CONFIG * pUpgradeData = (UPGRADE_PRODUCTION_CONFIG *)pDataMan->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, DataType);
	int fee(0);
	if (pUpgradeData){
		int nMaxHole = sizeof(pUpgradeData->num_armor)/sizeof(pUpgradeData->num_armor[0]);
		int nMaxLevel = sizeof(pUpgradeData->num_armor[0])/sizeof(pUpgradeData->num_armor[0][0]);
		if (level >= 1 && level <= nMaxLevel &&
			nHole >= 1 && nHole <= nMaxHole){
			fee = pUpgradeData->num_armor[nHole-1][level-1];
			fee = GetScaledFee(fee);
		}
	}
	return fee;
}

int CECEquipUpInheritProperty::GetStoneFee(int level)
{
	elementdataman *pDataMan = CECUIHelper::GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	UPGRADE_PRODUCTION_CONFIG * pUpgradeData = (UPGRADE_PRODUCTION_CONFIG *)pDataMan->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, DataType);
	int fee(0);
	if (pUpgradeData){
		int nMaxLevel = sizeof(pUpgradeData->num_stone)/sizeof(pUpgradeData->num_stone[0]);
		if (level >= 1 && level <= nMaxLevel){
			fee = pUpgradeData->num_stone[level-1];
			fee = GetScaledFee(fee);
		}
	}
	return fee;
}

int CECEquipUpInheritProperty::GetEngraveFee(int propNum)
{
	elementdataman *pDataMan = CECUIHelper::GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	UPGRADE_PRODUCTION_CONFIG * pUpgradeData = (UPGRADE_PRODUCTION_CONFIG *)pDataMan->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, DataType);
	int fee(0);
	if (pUpgradeData){
		int nMaxPropNum = sizeof(pUpgradeData->num_engrave)/sizeof(pUpgradeData->num_engrave[0]);
		if (propNum >= 1 && propNum <= nMaxPropNum){
			fee = pUpgradeData->num_engrave[propNum-1];
			fee = GetEngraveScaledFee(fee);
		}
	}
	return fee;
}

int CECEquipUpInheritProperty::GetScaledFee(int fee)
{
	float rate = GetFeeRate();
	return (int)(fee * rate + 0.5f);
}
int CECEquipUpInheritProperty::GetEngraveScaledFee(int fee)
{
	float rate = GetEngraveFeeRate();
	return (int)(fee * rate + 0.5f);
}

int CECEquipUpInheritProperty::GetAddonsFee( int propNum )
{
	elementdataman *pDataMan = CECUIHelper::GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	UPGRADE_PRODUCTION_CONFIG * pUpgradeData = (UPGRADE_PRODUCTION_CONFIG *)pDataMan->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, DataType);
	int fee(0);
	if (pUpgradeData){
		int nMaxPropNum = sizeof(pUpgradeData->num_addon)/sizeof(pUpgradeData->num_addon[0]);
		if (propNum >= 1 && propNum <= nMaxPropNum){
			fee = pUpgradeData->num_addon[propNum-1];
			fee = GetAddonsScaledFee(fee);
		}
	}
	return fee;
}

float CECEquipUpInheritProperty::GetFeeRate()
{
	float ratio(0);
	CECIvtrRecipe* pRecipe = pDlg->GetRecipe();
	if (pRecipe){
		const RECIPE_ESSENCE *pDBRecipe = (const RECIPE_ESSENCE *)(pRecipe->GetDBEssence());
		ratio = pDBRecipe->upgrade_rate;
	}
	return ratio;
}

float CECEquipUpInheritProperty::GetEngraveFeeRate()
{
	float ratio(0);
	CECIvtrRecipe* pRecipe = pDlg->GetRecipe();
	if (pRecipe){
		const RECIPE_ESSENCE *pDBRecipe = (const RECIPE_ESSENCE *)(pRecipe->GetDBEssence());
		ratio = pDBRecipe->engrave_upgrade_rate;
	}
	return ratio;
}

float CECEquipUpInheritProperty::GetAddonsFeeRate()
{
	float ratio(0);
	CECIvtrRecipe* pRecipe = pDlg->GetRecipe();
	if (pRecipe){
		const RECIPE_ESSENCE *pDBRecipe = (const RECIPE_ESSENCE *)(pRecipe->GetDBEssence());
		ratio = pDBRecipe->addon_inherit_fee_rate;
	}
	return ratio;
}

int CECEquipUpInheritProperty::GetAddonsScaledFee( int fee )
{
	float rate = GetAddonsFeeRate();
	return (int)(fee * rate + 0.5f);
}

void CECEquipUpInheritProperty::CheckAdvancedshow(bool bIsAdvanced)
{
	m_bActive = true;
}
void CECEquipUpInheritRefine::OnShow( )
{
	ACString strText;
	strText.Format(_AL("+%d"), m_iRefineLevel);
	m_pTxt_EquipRefine->SetText(strText);
	
	strText.Format(_AL("%d"), m_iInheritFee);
	m_pTxt_Num->SetText(strText);
}
void CECEquipUpInheritRefine::OnInitial()
{
	m_pTxt_EquipRefine->SetText(_AL(""));
	m_pTxt_Num->SetText(_AL(""));
	pDlg->CheckRadioButton(1, -1);			//	默认没有选择
	m_pRdo_Inherit->Enable(false);		//	默认不允许选择
	m_pRdo_NoInherit->Enable(false);
}

bool CECEquipUpInheritRefine::CheckForcedInherit(CECIvtrEquip * pEquip)
{
	float rate = GetFeeRate();
	if (fabs(rate) <= 1e-6f){
		// 继承缩放比例为0，强制继承所有属性 false表示有未强制继承的属性
		if (m_pRdo_NoInherit->IsChecked()){
			return false;
		}
	}
	return true;
}

void CECEquipUpInheritRefine::SetInheritData( CECIvtrEquip * pEquip )
{
	m_iRefineLevel = pEquip->GetRefineLevel();
}

CECEquipUpInheritRefine::CECEquipUpInheritRefine(CDlgEquipUp* pDlg):
CECEquipUpInheritProperty(pDlg)
{
	m_iRefineLevel = 0;
	m_pTxt_EquipRefine = NULL;
}

bool CECEquipUpInheritRefine::CanCheckYes( int wealthLeft)
{
	if(!m_bActive){
		return true;
	}
	if( wealthLeft >= m_iInheritFee ){
		return true;
	}
	return false;
}

void CECEquipUpInheritRefine::CheckYes()
{
	pDlg->CheckRadioButton(1,0);
}

bool CECEquipUpInheritRefine::CanInherit( char *inheritState /*= NULL*/, char *activestatus /*= NULL*/ )
{
	if(!m_bActive){
		return true;
	}
	if(activestatus)
		*activestatus += INHERIT_REFINE;
	if (m_pRdo_Inherit->IsChecked() || m_pRdo_NoInherit->IsChecked()){
		if (inheritState)
			*inheritState += m_pRdo_Inherit->IsChecked()? INHERIT_REFINE : 0;
		return true;
	}
	return false;
}

void CECEquipUpInheritRefine::UpdateInput( int wealthLeft )
{
	if (!IsWealthEnough(wealthLeft)){
		pDlg->CheckRadioButton(1, 1);
	}
}

void CECEquipUpInheritRefine::CalculateInheritFee( CECIvtrEquip* pEquip )
{
	m_iInheritFee = GetRefineFee(pEquip->GetRefineLevel());
}
CECEquipUpInheritHole::CECEquipUpInheritHole(CDlgEquipUp* pDlg):
CECEquipUpInheritProperty(pDlg)
{
	m_pTxt_EquipHole = NULL;
	m_iHoleNum = 0;
}

void CECEquipUpInheritHole::OnInitial()
{
	m_pTxt_EquipHole->SetText(_AL(""));
	m_pTxt_Num->SetText(_AL(""));
	pDlg->CheckRadioButton(2, -1);			//	默认没有选择
	m_pRdo_Inherit->Enable(false);		//	默认不允许选择
	m_pRdo_NoInherit->Enable(false);
}

void CECEquipUpInheritHole::OnShow()
{
	ACString strText;
	strText.Format(_AL("+%d"), m_iHoleNum);
	m_pTxt_EquipHole->SetText(strText);
	
	strText.Format(_AL("%d"), m_iInheritFee);
	m_pTxt_Num->SetText(strText);
}

void CECEquipUpInheritHole::SetInheritData( CECIvtrEquip * pEquip )
{
	m_iHoleNum = pEquip->GetHoleNum();
}

bool CECEquipUpInheritHole::CheckForcedInherit(CECIvtrEquip * pEquip)
{
	if (m_pRdo_NoInherit->IsChecked())	{
		// 继承缩放比例为0，强制继承所有属性 false表示有未强制继承的属性
		return false;
	}
	return true;
}

bool CECEquipUpInheritHole::CanCheckYes( int wealthLeft)
{
	if(!m_bActive){
		return true;
	}
	if( wealthLeft >= m_iInheritFee ){
		return true;
	}
	return false;
}

void CECEquipUpInheritHole::CheckYes()
{
	pDlg->CheckRadioButton(2,0);
}

bool CECEquipUpInheritHole::CanInherit( char *inheritState /*= NULL*/, char *activestatus /*= NULL*/ )
{
	if(!m_bActive){
		return true;
	}
	if(activestatus)
		*activestatus += INHERIT_HOLE;
	if (m_pRdo_Inherit->IsChecked() || m_pRdo_NoInherit->IsChecked()){
		if (inheritState)
			*inheritState += m_pRdo_Inherit->IsChecked()? INHERIT_HOLE : 0;
		return true;
	}
	return false;
}

void CECEquipUpInheritHole::UpdateInput( int wealthLeft )
{
	if (!IsWealthEnough(wealthLeft)){
		pDlg->CheckRadioButton(2, 1);
	}
}

void CECEquipUpInheritHole::CalculateInheritFee( CECIvtrEquip* pEquip )
{
	m_iInheritFee = 0;
	int nHole = pEquip->GetHoleNum();
	const RECIPE_ESSENCE *pRecipe = (const RECIPE_ESSENCE *)pDlg->GetRecipe()->GetDBEssence();
	if (pRecipe){
		CECIvtrItem *pProduceItem = CECIvtrItem::CreateItem((int)pRecipe->targets[0].id_to_make, 0, 1);
		if (pProduceItem){
			CECIvtrEquip *pEquip = static_cast<CECIvtrEquip *>(pProduceItem);
			if (pEquip->IsWeapon()){
				CECIvtrWeapon *pWeapon = static_cast<CECIvtrWeapon *>(pEquip);
				const WEAPON_ESSENCE *pEssence = pWeapon->GetDBEssence();
				m_iInheritFee = GetWeaponFee(pEssence->level, nHole);
			}
			else if (pEquip->IsArmor()){
				CECIvtrArmor *pArmor = static_cast<CECIvtrArmor *>(pEquip);
				const ARMOR_ESSENCE *pEssence = pArmor->GetDBEssence();
				m_iInheritFee = GetArmorFee(pEssence->level, nHole);
			}
			delete pProduceItem;
			pProduceItem = NULL;
		}
	}
}
CECEquipUpInheritStone::CECEquipUpInheritStone(CDlgEquipUp* pDlg):
CECEquipUpInheritProperty(pDlg)
{
	m_pTxt_Stone = NULL;
	m_iHoleNum = 0;
}

void CECEquipUpInheritStone::OnInitial()
{
	/*
		for (int i = 0; i < STONE_COUNT; ++ i)
				m_pTxt_Stone[i]->SetText(_AL(""));*/
	m_pTxt_Stone->SetText(_AL(""));	
	m_pTxt_Num->SetText(_AL(""));
	pDlg->CheckRadioButton(3, -1);			//	默认不选择
	m_pRdo_Inherit->Enable(false);		//	默认不允许选择
	m_pRdo_NoInherit->Enable(false);
}

void CECEquipUpInheritStone::SetInheritData( CECIvtrEquip * pEquip )
{
	m_iHoleNum = pEquip->GetHoleNum();
	int stoneFee(0);
	for (int i = 0; i < m_iHoleNum; ++ i){
		int idItem = pEquip->GetHoleItem(i);
		if (idItem <= 0)
			continue;
		
		CECIvtrStone *pStone = (CECIvtrStone *)CECIvtrItem::CreateItem(idItem, 0, 1);
		if (!pStone){
			ASSERT(false);
			continue;
		}
		const STONE_ESSENCE *pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();
		ACString strText;
		strText.Format(pDlg->GetStringFromTable(8060), pEssence->level);
		ACString strName;
		CECStringTab* pDescTab = CECUIHelper::GetGame()->GetItemDesc();
		strName += pDescTab->GetWideString(ITEMDESC_COL_CYANINE);
		strName += pEssence->name;
		if (strName.Find(strText) < 0)
			strName += strText;
		strName += _AL('\r');
		m_pTxt_Stone->AppendText(strName);
		delete pStone;
	}
}

void CECEquipUpInheritStone::OnShow()
{
	ACString strText;
	//m_pTxt_Stone 在设置数据时已经更新
	strText.Format(_AL("%d"), m_iInheritFee);
	m_pTxt_Num->SetText(strText);
}

bool CECEquipUpInheritStone::CheckForcedInherit(CECIvtrEquip * pEquip)
{
	if (m_pRdo_NoInherit->IsChecked()){			
		int i(0);
		for (i = 0; i < m_iHoleNum; ++ i){
			int idItem = pEquip->GetHoleItem(i);
			if (idItem > 0)	{
				// 有石头
				return false;
			}
		}
	}
	return true;
}

bool CECEquipUpInheritStone::CanCheckYes( int wealthLeft)
{
	if(!m_bActive){
		return true;
	}
	if( wealthLeft >= m_iInheritFee ){
		return true;
	}
	return false;
}

void CECEquipUpInheritStone::CheckYes()
{
	pDlg->CheckRadioButton(3,0);
}

bool CECEquipUpInheritStone::CanInherit( char *inheritState /*= NULL*/, char *activestatus /*= NULL*/ )
{
	if(!m_bActive){
		return true;
	}
	if(activestatus)
		*activestatus += INHERIT_STONE;
	if (m_pRdo_Inherit->IsChecked() || m_pRdo_NoInherit->IsChecked()){
		if (inheritState)
			*inheritState += m_pRdo_Inherit->IsChecked()? INHERIT_STONE : 0;
		return true;
	}
	return false;
}

void CECEquipUpInheritStone::UpdateInput( int wealthLeft )
{
	if (!IsWealthEnough(wealthLeft)){
		pDlg->CheckRadioButton(3, 1);
	}
}

void CECEquipUpInheritStone::CalculateInheritFee( CECIvtrEquip* pEquip )
{
	m_iInheritFee = 0;
	int nHole = pEquip->GetHoleNum();
	for (int i = 0; i < nHole; ++ i){
		int idItem = pEquip->GetHoleItem(i);
		if (idItem <= 0)
			continue;
		
		CECIvtrStone *pStone = (CECIvtrStone *)CECIvtrItem::CreateItem(idItem, 0, 1);
		if (!pStone){
			ASSERT(false);
			continue;
		}
		const STONE_ESSENCE *pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();				
		m_iInheritFee += GetStoneFee(pEssence->level);
		delete pStone;
	}
}
CECEquipUpInheritStoneHole::CECEquipUpInheritStoneHole(CDlgEquipUp* pDlg):
CECEquipUpInheritProperty(pDlg)
{
	m_pInheritHole = new CECEquipUpInheritHole(pDlg);
	m_pInheritStone = new CECEquipUpInheritStone(pDlg);
	m_bResetStoneButton = true;
}
CECEquipUpInheritStoneHole::~CECEquipUpInheritStoneHole()
{
	delete m_pInheritHole;
	delete m_pInheritStone;
}
void CECEquipUpInheritStoneHole::OnInitial()
{
	m_pInheritHole->OnInitial();
	m_pInheritStone->OnInitial();
	m_bResetStoneButton = true;
}

void CECEquipUpInheritStoneHole::SetInheritData( CECIvtrEquip * pEquip )
{
	m_pInheritHole->SetInheritData(pEquip);
	m_pInheritStone->SetInheritData(pEquip);
}

void CECEquipUpInheritStoneHole::OnShow()
{
	m_pInheritHole->OnShow();
	m_pInheritStone->OnShow();
}

bool CECEquipUpInheritStoneHole::CheckForcedInherit( CECIvtrEquip * pEquip )
{
	float rate = GetFeeRate();
	if (fabs(rate) <= 1e-6f){
		if(!m_pInheritHole->CheckForcedInherit(pEquip) || !m_pInheritStone->CheckForcedInherit(pEquip)){
			return false;
		}
	}
	return true;
}

bool CECEquipUpInheritStoneHole::CanCheckYes( int wealthLeft)
{
	if(!m_bActive){
		return true;
	}
	return m_pInheritHole->CanCheckYes(wealthLeft) && m_pInheritStone->CanCheckYes(wealthLeft - m_pInheritHole->m_iInheritFee);
}

void CECEquipUpInheritStoneHole::CheckYes()
{
	m_bResetStoneButton = false;
	m_pInheritHole->CheckYes();
	m_pInheritStone->CheckYes();
}

bool CECEquipUpInheritStoneHole::CanInherit(char *inheritState /*= NULL*/, char *activestatus /*= NULL*/)
{
	if(!m_bActive){
		return true;
	}
	return m_pInheritHole->CanInherit(inheritState, activestatus) && m_pInheritStone->CanInherit(inheritState, activestatus);
}

void CECEquipUpInheritStoneHole::UpdateInput( int wealthLeft )
{
	m_pInheritHole->UpdateInput(wealthLeft);
	if (m_pInheritHole->m_pRdo_Inherit->IsChecked()){
		m_pInheritStone->UpdateInput(wealthLeft);
		if (m_bResetStoneButton && m_pInheritStone->IsWealthEnough(wealthLeft) ){
			pDlg->CheckRadioButton(3, -1);
			m_bResetStoneButton = false;
		}
	}else{
		m_pInheritStone->InvalidateButton(false);
		pDlg->CheckRadioButton(3,1);
		m_bResetStoneButton = true;
	}
}

int CECEquipUpInheritStoneHole::GetInheritFee()
{
	int Cost(0);
	if (m_pInheritHole->CanInherit() && m_pInheritHole->m_pRdo_Inherit->IsChecked()){
		Cost += m_pInheritHole->GetInheritFee();
	}
	if (m_pInheritStone->CanInherit() && m_pInheritStone->m_pRdo_Inherit->IsChecked()){
		Cost += m_pInheritStone->GetInheritFee();
	}
	return Cost;
}

bool CECEquipUpInheritStoneHole::IsYesChecked()
{
	return m_pInheritHole->IsYesChecked() && m_pInheritStone->IsYesChecked();
}
bool CECEquipUpInheritStoneHole::IsNoChecked()
{
	return m_pInheritHole->IsNoChecked() || m_pInheritStone->IsNoChecked();
}

void CECEquipUpInheritStoneHole::CalculateInheritFee( CECIvtrEquip* pEquip )
{
	m_pInheritHole->CalculateInheritFee(pEquip);
	m_pInheritStone->CalculateInheritFee(pEquip);
	m_iInheritFee = m_pInheritHole->m_iInheritFee + m_pInheritStone->m_iInheritFee;
}
CECEquipUpInheritEngrave::CECEquipUpInheritEngrave(CDlgEquipUp* pDlg):
CECEquipUpInheritProperty(pDlg)
{
	m_iEngraveNum = 0;
	m_pTex_Engrave = NULL;
}

void CECEquipUpInheritEngrave::OnInitial()
{
	m_pTxt_Num->SetText(_AL(""));
	m_pTex_Engrave->SetText(_AL(""));
	pDlg->CheckRadioButton(4, -1);			//	默认没有选择
	m_pRdo_Inherit->Enable(false);		//	默认不允许选择
	m_pRdo_NoInherit->Enable(false);
}

void CECEquipUpInheritEngrave::SetInheritData( CECIvtrEquip * pEquip )
{
	//  镌刻
	m_iEngraveNum = pEquip->GetEngravedPropertyNum();
	if (m_iEngraveNum <= 0){
		m_bActive = false;
	}else{
		m_bActive = true;
	}
	ACString strTex = pEquip->GetEngraveDesc();
	AUICTranslate trans;
	m_pTex_Engrave->SetText(trans.Translate(strTex));
}

void CECEquipUpInheritEngrave::OnShow()
{
	ACString strText;
	strText.Format(_AL("%d"), m_iInheritFee);
	m_pTxt_Num->SetText(strText);
}

bool CECEquipUpInheritEngrave::CheckForcedInherit( CECIvtrEquip * pEquip )
{	
	float engrageRate = GetEngraveFeeRate();
	if (fabs(engrageRate) <= 1e-6f){
		if (m_pRdo_NoInherit->IsChecked()){
			// 有镌刻未继承
			return false;
		}
	}
	return true;
}

void CECEquipUpInheritEngrave::UpdateInput( int wealthLeft )
{
	// Get wealth left except refine	
	if (!IsWealthEnough(wealthLeft)){
		pDlg->CheckRadioButton(4,1);
	}
}

bool CECEquipUpInheritEngrave::CanCheckYes( int wealthLeft)
{
	if(!m_bActive){
		return true;
	}
	if( wealthLeft >= m_iInheritFee ){
		return true;
	}
	return false;
}

void CECEquipUpInheritEngrave::CheckYes()
{
	pDlg->CheckRadioButton(4,0);
}

bool CECEquipUpInheritEngrave::CanInherit( char *inheritState /*= NULL*/, char *activestatus /*= NULL*/ )
{
	if(!m_bActive){
		return true;
	}
	if(activestatus)
		*activestatus += INHERIT_ENGRAVE;
	if (m_pRdo_Inherit->IsChecked() || m_pRdo_NoInherit->IsChecked()){
		if (inheritState)
			*inheritState += m_pRdo_Inherit->IsChecked()? INHERIT_ENGRAVE : 0;
		return true;
	}
	return false;
}

void CECEquipUpInheritEngrave::CalculateInheritFee( CECIvtrEquip* pEquip )
{
	int iEngravedPropNum = pEquip->GetEngravedPropertyNum();
	m_iInheritFee = GetEngraveFee(iEngravedPropNum);
}
CECEquipUpInheritAddons::CECEquipUpInheritAddons( CDlgEquipUp* pDlg ):
CECEquipUpInheritProperty(pDlg)
{
	m_iAddonsNum = 0;
	m_pTxt_Addons = NULL;
}

void CECEquipUpInheritAddons::OnInitial()
{
	m_pTxt_Addons->SetText(_AL(""));
	m_pTxt_Num->SetText(_AL(""));
	pDlg->CheckRadioButton(5,-1);				//	默认没有选择
	m_pRdo_Inherit->Enable(false);
	m_pRdo_NoInherit->Enable(false);
}

void CECEquipUpInheritAddons::OnShow()
{
	AUICTranslate trans;
	m_pTxt_Addons->SetText(trans.Translate(m_sAddonsDesc));
	ACString strText;
	strText.Format(_AL("%d"), m_iInheritFee);
	m_pTxt_Num->SetText(strText);
}

void CECEquipUpInheritAddons::SetInheritData( CECIvtrEquip * pEquip )
{
	m_sAddonsDesc = pEquip->GetAddOnPropDesc(pDlg->m_addonsNum, pDlg->m_addonsId);
	m_iAddonsNum = pDlg->m_addonsNum;
}

bool CECEquipUpInheritAddons::CheckForcedInherit( CECIvtrEquip * pEquip )
{
	float addonsRate = GetAddonsFeeRate();
	if (fabs(addonsRate) <= 1e-6f){
		if (m_pRdo_NoInherit->IsChecked())
		{
			// 有附加属性未继承
			return false;
		}
	}
	return true;
}

void CECEquipUpInheritAddons::UpdateInput( int wealthLeft )
{
	// Get wealth left except refine
	if (!IsWealthEnough(wealthLeft)){
		pDlg->CheckRadioButton(5, 1);
	}
}

bool CECEquipUpInheritAddons::CanCheckYes( int wealthLeft)
{
	if(!m_bActive){
		return true;
	}
	if( wealthLeft >= m_iInheritFee ){
		return true;
	}
	return false;
}

void CECEquipUpInheritAddons::CheckYes()
{
	pDlg->CheckRadioButton(5,0);
}

bool CECEquipUpInheritAddons::CanInherit( char *inheritState /*= NULL*/, char *activestatus /*= NULL*/ )
{
	if(!m_bActive){
		return true;
	}
	if(activestatus)
		*activestatus += INHERIT_ADDONS;
	if (m_pRdo_Inherit->IsChecked() || m_pRdo_NoInherit->IsChecked()){
		if (inheritState)
			*inheritState += m_pRdo_Inherit->IsChecked()? INHERIT_ADDONS : 0;
		return true;
	}
	return false;
}

void CECEquipUpInheritAddons::CalculateInheritFee( CECIvtrEquip* pEquip )
{
	m_iInheritFee = GetAddonsFee(m_iAddonsNum);
}

void CECEquipUpInheritAddons::CheckAdvancedshow( bool bIsAdvanced)
{
	m_bActive = bIsAdvanced;
}