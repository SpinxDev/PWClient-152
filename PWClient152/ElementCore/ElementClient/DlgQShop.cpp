// Filename	: DlgQShop.cpp
// Creator	: Tom Zhou
// Date		: 2006/5/22

#include "DlgQShop.h"
#include "DlgQShopItem.h"
#include "DlgQuickPay.h"
#include "EC_GameUIMan.h"
#include "AUIDef.h"
#include "AUICTranslate.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_HostPlayer.h"
#include "EC_LoginPlayer.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_Game.h"
#include "EC_World.h"
#include "roleinfo"
#include "EC_Model.h"
#include "EC_SendC2SCmds.h"
#include "EC_GameSession.h"
#include "EC_IvtrFashion.h"
#include "EC_UIConfigs.h"
#include "AIniFile.h"
#include "DlgFittingRoom.h"
#include "DlgViewModel.h"
#include "EC_ManPlayer.h"
#include "EC_CrossServer.h"
#include "EC_NPC.h"
#include "ExpTypes.h"
#include "EC_IvtrFlySword.h"
#include "EC_Shop.h"
#include "EC_UIHelper.h"
#include "DlgFashionShop.h"
#include "EC_FashionShop.h"
#include "EC_ShoppingManager.h"

#include "elementdataman.h"

#include "AWStringWithWildcard.h"
#include "Network/useraddcash_re.hpp"
#include <AWIniFile.h>
#include <CSplit.h>

#define new A_DEBUG_NEW

//	在所有商品中搜索目标
class GShopSearcher
{
	CDlgQShop	*_pDlgShop;
	CECShopBase *_pShop;

	typedef std::vector<ACString> PatternArray;
	PatternArray _patterns;				//	字符匹配模式数组（要求数组中各模式在搜索对象中都能找到）
	int			_id;					//	id 匹配模式下ID对象
	bool		_byPattern;				//	当前搜索模式

	int			_it;					//	下一个搜索

	ACStringWithWildcard	_matcher;	//	字符匹配辅助对象
	ACString				_strTemp;	//	临时匹配对象

	elementdataman	*		_pDataMan;	//	elements.data

	bool IsMatch(const GSHOP_ITEM &item)
	{
		bool bMatched=false;
		if (!_pDlgShop->ShouldIgnore(item)){
			if (_byPattern){
				//	将目标转换成小写，以实现忽略大小写进行匹配
				_cp_str(_strTemp, item.szName, a_strlen(item.szName)*sizeof(ACHAR));
				_strTemp.MakeLower();
				for (PatternArray::const_iterator cit = _patterns.begin(); cit != _patterns.end(); ++ cit){
					if (!(bMatched = _matcher.IsMatch(_strTemp, *cit))){
						break;
					}
				}
			}else{
				bMatched = (item.id == _id);
			}
		}
		return bMatched;
	}

public:
	GShopSearcher(CDlgQShop *pDlgShop, const ACString &searchPattern)
		: _pDlgShop(pDlgShop)
		, _pShop(&_pDlgShop->GetShopData())
		, _id(0)					//	默认不能切换使用ID搜索
		, _byPattern(true)			//	默认使用字符搜索
		, _it(0)					//	下一个从第0个位置搜索
	{
		_pDataMan = g_pGame->GetElementDataMan();

		CSplit patternSplitter(searchPattern);
		CSplit::VectorAWString vec = patternSplitter.Split(_AL(" "));	//	以空格为分隔符
		if (!vec.empty())
		{
			for (size_t i = 0; i < vec.size(); ++ i)
			{
				AWString &str = vec[i];
				str.TrimLeft();
				str.TrimRight();
				
				// 首尾删除通配符，以匹配有实质意义的内容
				str.TrimRight(_AL("?*"));
				str.TrimLeft(_AL("?*"));

				if (!str.IsEmpty())
				{
					str.MakeLower();	//	转换成小写，用于匹配时不区分大小写
					_patterns.push_back(_AL("*") + str +  _AL("*"));
				}
			}
		}

		//	查看是否可转换成ID，用于按ID搜索
		int testID = const_cast<ACString &>(searchPattern).ToInt();
		if (testID > 0 &&
			ACString().Format(_AL("%d"), testID) == searchPattern)
			_id = testID;

		ResetSearch();
	}

	void ResetSearch()
	{
		_it = 0;
	}

	bool Next(int &matchedItemIndex)
	{
		while (_it < (int)_pShop->GetCount())
		{
			const GSHOP_ITEM *pItem = _pShop->GetItem(_it);
			if (IsMatch(*pItem))
			{
				matchedItemIndex = _it++;
				return true;
			}
			++_it;
		}
		return false;
	}

	bool SearchByID()
	{
		if (!_byPattern)
		{
			//	已经是搜索ID模式
			ASSERT(false);
			return false;
		}

		if (_id > 0)
		{
			//	切换到按ID搜索模式
			_byPattern = false;
			ResetSearch();
			return true;
		}
		return false;
	}
};

//	将商品 status 分类排序输出
class GShopCollector
{
	CDlgQShop::GShopItemsPtr _pItems;	//	商品列表

	typedef CDlgQShop::QShopItemIndexArray QShopItemIndexArray;

	enum {CATEGORY_COUNT = 5};
	QShopItemIndexArray _category[CATEGORY_COUNT];	//	分类列表，对应商品 status 依次为 13闪购、3推荐、2促销、1新品、其它

	int _count;	//	总数

	int GetCategory(int itemIndex)const
	{
		int category = -1;
		if (itemIndex >= 0 && itemIndex < _pItems->size())
		{
			const GSHOP_ITEM &item = (*_pItems)[itemIndex];
			unsigned int status = 0;
			if (!CDlgQShop::GetFirstBuyStatus(item, &status)){
				ASSERT(false);
			}else{
				switch (status)
				{
				case 13: category = 0; break;
				case 3: category = 1; break;
				case 2: category = 2; break;
				case 1: category = 3; break;
				default: category = 4; break;
				}
			}
		}
		else{
			ASSERT(false);
		}
		return category;
	}

public:

	GShopCollector(CDlgQShop::GShopItemsPtr pItems)
		: _pItems(pItems)
		, _count(0)
	{}

	bool PushBack(int itemIndex)
	{
		int c = GetCategory(itemIndex);
		if (c >= 0)
		{
			_category[c].push_back(itemIndex);
			++ _count;
		}
		return c >= 0;
	}

	int GetSorted(QShopItemIndexArray &rhs)const
	{
		rhs.clear();
		rhs.reserve(_count);

		//	依次拷贝
		for (int i = 0; i < CATEGORY_COUNT; ++ i)
			rhs.insert(rhs.end(), _category[i].begin(), _category[i].end());

		return _count;
	}

	int GetCount()const {return _count; }
};

AUI_BEGIN_COMMAND_MAP(CDlgQShop, CDlgBase)

AUI_ON_COMMAND("maintype*",		OnCommand_MainType)
AUI_ON_COMMAND("subtype*",		OnCommand_SubType)
AUI_ON_COMMAND("payment",		OnCommand_Payment)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("backshop",      OnCommand_BackShop)
AUI_ON_COMMAND("qshop",         OnCommand_QShop)
AUI_ON_COMMAND("backothers",    OnCommand_BackOthers)
AUI_ON_COMMAND("backself",      OnCommand_BackSelf)
AUI_ON_COMMAND("bshop",         OnCommand_BShop)
AUI_ON_COMMAND("fit",           OnCommand_Fit)
AUI_ON_COMMAND("Btn_Coupon",	OnCommand_Coupon)
AUI_ON_COMMAND("Btn_AddCash",	OnCommand_UserAddCash)
AUI_ON_COMMAND("Btn_QuickPay",  OnCommand_QuickPay)
AUI_ON_COMMAND("Btn_Search",	OnCommand_Search)
AUI_ON_COMMAND("Txt_Search",	OnCommand_Search)
AUI_ON_COMMAND("Btn_CardBuy",	OnCommand_BuyCard)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQShop, CDlgBase)

AUI_ON_EVENT("*",		WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT("Txt_Search",	WM_KEYDOWN,		OnEventKeyDown_Txt_Search)
AUI_ON_EVENT("Lst_Search",	WM_LBUTTONDBLCLK, OnEventDBClk_Lst_Search)

AUI_END_EVENT_MAP()

CDlgQShop::CDlgQShop()
{
	m_nMainType = 0;
	m_nSubType = 0;
	m_bFirstFlash = true;
	m_playerLevelOnShow = -1;
	m_pImgGfx = NULL;
}

CDlgQShop::~CDlgQShop()
{
}

bool CDlgQShop::OnInitDialog()
{
	m_pTxt_ItemName = (PAUILABEL)GetDlgItem("Txt_ItemName");
	m_pTxt_ItemDesc = (PAUILABEL)GetDlgItem("Txt_ItemDesc");
	m_pScl_Item = (PAUISCROLL)GetDlgItem("Scl_Item");
	m_pTxt_Cash = (PAUILABEL)GetDlgItem("Txt_Cash");
	m_pTxt_CashMoneyRate = (PAUILABEL)GetDlgItem("Txt_Gold");
	m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Img_Item");
	m_pImg_Bg01 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg01");
	m_pImg_Bg02 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg02");
	m_pImg_Bg03 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg03");
	m_pImg_Bg04 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg04");
	m_pImgGfx   = (PAUIIMAGEPICTURE)GetDlgItem("ImageGfx");
	if (!CECUIConfig::Instance().GetGameUI().bEnableQuickPay)
	{
		PAUIOBJECT pQuickPay = GetDlgItem("Btn_QuickPay");
		if (pQuickPay) pQuickPay->Show(false);
	}
	int i(0);
	for(i = 0; i < CDLGQSHOP_BUTTONMAX; i++)
	{
		char szName[20];
		sprintf(szName, "Btn_MainType%d", i);
		m_pBtn_MainType[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);
		sprintf(szName, "Btn_SubType%d", i);
		m_pBtn_SubType[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);
	}
	SetData(CECGAMEUIMAN_MAX_QSHOPITEMS);
	SetDataPtr((void*)-1);
	m_bFirstOpen = true;	
	
	m_pBtn_BackShop = static_cast<PAUIRADIOBUTTON>(GetDlgItem("Btn_BackShop"));
	m_pBtn_QShop = static_cast<PAUIRADIOBUTTON>(GetDlgItem("Btn_QShop"));

	// QShop controls
	m_pTxt_Notify = (PAUILABEL)GetDlgItem("Txt_Notify");
	if (IsQShop())
	{
		CECGame::CONSUME_REWARD &reward = GetGame()->GetConsumeReward();
		if (reward.bOpen)
		{
			ACString strText;
			strText.Format(GetStringFromTable(8000),
				reward.iEndTime[0],
				reward.iEndTime[1],
				reward.iEndTime[2],
				reward.iEndTime[3]);
			m_pTxt_Notify->SetText(strText);
			m_pTxt_Notify->Show(true);
		}
		else
		{
			m_pTxt_Notify->SetText(_AL(""));
			m_pTxt_Notify->Show(false);
		}
	}

	// Back shop controls
	m_pBtn_BackOthers = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_BackOthers");
	m_pBtn_BackSelf = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_BackSelf");
	if (IsBackShop())
	{
		CECGame::CONSUME_REWARD &reward = GetGame()->GetConsumeReward();
		m_pBtn_BackSelf->Enable(reward.bOpen);
	}

	m_pBtn_TrySkirt = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_TrySkirt");
	m_pBtn_TrySkirt->Enable(false);
	
	m_pImg_Highlight = (PAUIIMAGEPICTURE)GetDlgItem("Img_Highlight");
	m_pImg_Normal = (PAUIIMAGEPICTURE)GetDlgItem("Img_Normal");
	m_pImg_Highlight->Show(false);
	m_pImg_Normal->Show(false);

	m_pTxt_Search = GetDlgItem("Txt_Search");
	m_pLst_Search = dynamic_cast<PAUILISTBOX>(GetDlgItem("Lst_Search"));

	DisableForCrossServer("Btn_BShop");

	return true;
}

void CDlgQShop::OnShowDialog()
{
	//	为获取统一的显示，在商城显示时获取玩家等级，用于在本次显示中等级过滤
	m_playerLevelOnShow = GetHostPlayer()->GetMaxLevelSofar();

	if (IsQShop())
	{
		if(m_pImgGfx)
			m_pImgGfx->SetGfx("");

		CheckRadioButton(1, 1);
		m_pBtn_BackShop->SetFlash(false);
		CECGame::CONSUME_REWARD &reward = GetGame()->GetConsumeReward();
		if (reward.bOpen && m_bFirstFlash)
		{
			m_pBtn_BackShop->SetFlash(true);
			m_bFirstFlash = false;
		}

		// 取元宝金币兑换率
		::c2s_SendCmdGetCashMoneyRate();
	}
	else
	{
		CheckRadioButton(1, 2);
		m_pBtn_BackShop->SetFlash(false);
	}

	if( m_bFirstOpen )
	{
		if (IsQShop())
			c2s_SendCmdQueryCashInfo();

		m_bFirstOpen = false;
	}

	// refresh buttons
	m_nMainType = 0;
	OnChangeLayoutEnd(true);

	m_pTxt_ItemName->SetText(GetStringFromTable(821));
	m_pTxt_ItemName->SetData(0);
	
	// 清空搜索记录
	if (m_pTxt_Search){
		m_pTxt_Search->SetText(_AL(""));
	}

	CECUIHelper::CheckBuyPokerShop(this, "Btn_CardBuy");
}

void CDlgQShop::OnCommand_CANCEL(const char * szCommand){
	//	手动关闭调这里
	if (!IsShow()){
		return;
	}
	ShowFalse();
	if (GetShopDataConst().HasOwnerNPC()){
		CECUIHelper::EndNPCGoldShopService();		//	当处理挂靠NPC商城时，关闭界面将导致结束NPC服务
	}
}

void CDlgQShop::ShowFalse(bool bIgnoreShopItemDialog/*=false*/)
{
	Show(false);
	if (!bIgnoreShopItemDialog){
		for(int i = 0; i < CECGAMEUIMAN_MAX_QSHOPITEMS; i++){
			GetGameUIMan()->m_pDlgQShopDefault[i]->Show(false);
		}
	}
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_QshopBuy");
	if (pDlg) pDlg->Show(false);
}

bool CDlgQShop::CanFitCurrentItem()
{
	bool result(false);	
	int tid = m_pTxt_ItemName->GetData();
	if (tid)
	{
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(tid, 0, 1);
		pItem->GetDetailDataFromLocal();

		if (pItem->GetClassID() == CECIvtrItem::ICID_FASHION){
			//	时装预览通过试衣间界面单独判断
			while (true)
			{
				CDlgFittingRoom * pDlgFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
				PAUIOBJECT pImage = pDlgFittingRoom->GetEquipIcon(pItem);
				if(!pImage)
					break;
				
				CECIvtrFashion *pFashion = dynamic_cast<CECIvtrFashion *>(pItem);
				const FASHION_ESSENCE *pEssence = pFashion->GetDBEssence();
				if (pEssence){
					if (pEssence->gender != GetHostPlayer()->GetGender())
						break;
					CECHostPlayer *pHost = GetHostPlayer();
					if (pEssence->require_level > pHost->GetMaxLevelSofar())
						break;
					if (pEssence->equip_location == enumSkinShowHand && !(pEssence->character_combo_id & (1 << pHost->GetProfession())))
						break;
					result = true;
				}
				break;
			}
		}
		else{
			PAUIDIALOG pDlgViewModel = GetGameUIMan()->GetDialog("Win_ViewModel");
			if (pDlgViewModel){
				//	通过允许 Win_ViewModel 为空实现禁用此功能
				if (CECViewModelCondition::Meet(pItem) &&
					CECViewModelExtractor::CanExtract(pItem)){
					result = true;
				}
			}
		}

		delete pItem;
	}
	return result;
}

void CDlgQShop::OnTick()
{
	m_pTxt_Cash->SetText(GetCashText(GetCash()));

	if (IsQShop())
	{
		if (!m_pTxt_Notify->IsMarquee())
			m_pTxt_Notify->SetMarquee(true, 3, 40);

		ACString strText;
		float fCashRate = GetHostPlayer()->GetCashMoneyRate() / 10000.0f;
		strText.Format(GetGameUIMan()->GetStringFromTable(10014), fCashRate);

		if( GetHostPlayer()->IsCashMoneyOpen() )
		{
			m_pTxt_CashMoneyRate->SetText(strText);
			m_pTxt_CashMoneyRate->SetHint(_AL(""));
		}
		else
		{
			m_pTxt_CashMoneyRate->SetText(GetGameUIMan()->GetStringFromTable(10017));
			m_pTxt_CashMoneyRate->SetHint(strText);
		}
	}

	UpdateScrollPos();
	UpdateSearchList();

	m_pBtn_TrySkirt->Enable(CanFitCurrentItem());
}

void CDlgQShop::OnCommand_Payment(const char * szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("Payment", "URL");
	GetBaseUIMan()->NavigateURL(strText, "Win_Explorer", true);
}

void CDlgQShop::OnCommand_MainType(const char * szCommand){
	ShowMainType(atoi(szCommand + strlen("maintype")));
}

int  CDlgQShop::AdjustMainTypeWithOwnerNPC(int nMainType){
	//	根据当前挂靠NPC调整修改，以显示出有效分类
	int result = nMainType;
	if (GetShopDataConst().HasOwnerNPC()){
		if (IsMainTypeEmpty(nMainType)){
			result = SearchFirstNotEmptyMainType();
			if (result < 0){
				result = nMainType;
			}
		}
	}
	return nMainType;
}

int  CDlgQShop::AdjustSubTypeWithOwnerNPC(int nMainType, int nSubType){
	//	根据当前挂靠NPC调整修改，以显示出有效分类
	int result = nSubType;
	if (GetShopDataConst().HasOwnerNPC()){
		if (nSubType >= 0 && IsSubTypeEmpty(nMainType, nSubType)){
			result = SearchFirstNotEmptySubType(nMainType);
			if (result < 0){
				result = nSubType;
			}
		}
	}
	return result;
}

void CDlgQShop::EnableEmptyMainType(){
	bool bEnable(true);
	if (GetShopDataConst().HasOwnerNPC()){
		if (!IsMainTypeEmpty(m_nMainType)){
			bEnable = false;
		}
	}
	EnableEmptyMainType(bEnable);
}

void CDlgQShop::EnableEmptySubType(){
	bool bEnable(true);
	if (GetShopDataConst().HasOwnerNPC()){
		if (!IsMainTypeEmpty(m_nMainType)){
			bEnable = false;
		}
	}
	EnableEmptySubType(bEnable);
}

void CDlgQShop::ShowMainType(int nCommandMainType){
	int nMainType = AdjustMainTypeWithOwnerNPC(nCommandMainType);
	EnableEmptyMainType();

	abase::vector<GSHOP_MAIN_TYPE> *maintype = GetShopMainTypes();

	//	检查是否是时装分类、是则显示时装商城，同时当前界面显示其它分类
	bool bShowFashionShop(false);
	if (IsFashionShopCategory(nMainType)){
		bShowFashionShop = true;
		if (!IsFashionShopCategory(m_nMainType)){
			//	当前主分类合法，仍旧使用当前分类并更新数据
			nMainType = m_nMainType;
		}else{
			//	检查其它非时装分类
			for (int i(0); i < (int)maintype->size(); ++ i){
				if (!IsFashionShopCategory(i)){
					nMainType = i;
				}
			}
		}
	}
	if (nMainType < (int)maintype->size()){
		m_nMainType = nMainType;
	}else{
		//	找不到非时装分类，维持 m_nMainType 不变
		ASSERT(false);
	}
	DWORD i;
	for(i = 0; i < CDLGQSHOP_BUTTONMAX; i++ ){
		if( m_pBtn_MainType[i] ){
			if( m_nMainType == (int)i ){
				m_pBtn_MainType[i]->SetPushed(true);
				m_pBtn_MainType[i]->SetColor(A3DCOLORRGB(255, 203, 74));
			}else{
				m_pBtn_MainType[i]->SetPushed(false);
				m_pBtn_MainType[i]->SetColor(A3DCOLORRGB(255, 255, 255));
			}
		}
	}
	for(i = 0; i < (*maintype)[m_nMainType].sub_types.size(); i++ ){
		if( m_pBtn_SubType[i] ){
			m_pBtn_SubType[i]->SetText((*maintype)[m_nMainType].sub_types[i]);
			m_pBtn_SubType[i]->Show(true);
		}
	}
	for(; i < CDLGQSHOP_BUTTONMAX; i++ ){
		if( m_pBtn_SubType[i] ){
			m_pBtn_SubType[i]->Show(false);
		}
	}
	int nSubType = GetSubTypeCount(m_nMainType) > 0 ? 0 : -1;
	ShowSubType(nSubType);

	//	时装商城放到最后显示
	if (bShowFashionShop){
		ShowFashionShop(GetShopDataConst().GetOwnerNPCID());
	}
}

void CDlgQShop::EnableEmptyMainType(bool bEnable){
	int mainTypeCount = GetMainTypeCount();
	for (int i(0); i < mainTypeCount && i < ARRAY_SIZE(m_pBtn_MainType); ++ i){
		PAUISTILLIMAGEBUTTON pBtn = m_pBtn_MainType[i];
		if (pBtn){
			pBtn->Enable(bEnable || !IsMainTypeEmpty(i));
		}
	}
}

void CDlgQShop::EnableEmptySubType(bool bEnable){
	int subTypeCount = GetSubTypeCount(m_nMainType);
	for (int i(0); i < subTypeCount && i < ARRAY_SIZE(m_pBtn_SubType); ++ i){
		PAUISTILLIMAGEBUTTON pBtn = m_pBtn_SubType[i];
		if (pBtn){
			pBtn->Enable(bEnable || !IsSubTypeEmpty(m_nMainType, i));
		}
	}
}

bool CDlgQShop::IsMainTypeEmpty(int nMainType){
	if (!IsMainTypeValid(nMainType)){
		return true;
	}
	if (!IsFashionShopCategory(nMainType)){
		GShopItemsPtr items = GetShopItems();
		for (int i = 0; i < (int)items->size(); i++){
			const GSHOP_ITEM &item = (*items)[i];
			if (item.main_type == nMainType && !ShouldIgnore(item)){
				return false;
			}
		}
	}else{
		GShopItemsPtr items = GetShopItems();
		for (int i = 0; i < (int)items->size(); i++){
			const GSHOP_ITEM &item = (*items)[i];
			if (item.main_type == nMainType && !ShouldIgnoreByNPCID(item)){
				return false;
			}
		}
	}
	return true;
}

bool CDlgQShop::IsSubTypeEmpty(int nMainType, int nSubType){
	if (!IsSubTypeValid(nMainType, nSubType)){
		return true;
	}
	GShopItemsPtr items = GetShopItems();
	for (int i = 0; i < (int)items->size(); i++)
	{
		const GSHOP_ITEM &item = (*items)[i];
		if (item.main_type == nMainType && item.sub_type == nSubType && !ShouldIgnore(item)){
			return false;
		}
	}
	return true;
}

int  CDlgQShop::GetMainTypeCount(){
	return (int)(GetShopMainTypes()->size());
}

int	 CDlgQShop::GetSubTypeCount(int nMainType){
	if (!IsMainTypeValid(nMainType)){
		return 0;
	}
	const GSHOP_MAIN_TYPE &mainType = (*GetShopMainTypes())[nMainType];
	return (int)mainType.sub_types.size();
}

bool CDlgQShop::IsMainTypeValid(int nMainType){
	return nMainType >= 0 && nMainType < GetMainTypeCount();
}

bool CDlgQShop::IsSubTypeValid(int nMainType, int nSubType){
	return nSubType >= 0 && nSubType < GetSubTypeCount(nMainType);
}

int  CDlgQShop::SearchFirstNotEmptyMainType(){	
	int mainTypeCount = GetMainTypeCount();
	for (int i(0); i < mainTypeCount; ++ i){
		if (!IsMainTypeEmpty(i)){
			return i;
		}
	}
	return -1;
}

int	 CDlgQShop::SearchFirstNotEmptySubType(int nMainType){
	int subTypeCount = GetSubTypeCount(nMainType);
	for (int i(0); i < subTypeCount; ++ i){
		if (!IsSubTypeEmpty(nMainType, i)){
			return i;
		}
	}
	return -1;
}

void CDlgQShop::OnCommand_SubType(const char * szCommand){
	ShowSubType(atoi(szCommand + strlen("subtype")));
}

void CDlgQShop::ShowSubType(int nSubType){
	m_nSubType = AdjustSubTypeWithOwnerNPC(m_nMainType, nSubType);
	EnableEmptySubType();
	int i;
	for(i = 0; i < CDLGQSHOP_BUTTONMAX; i++ )
		if( m_pBtn_SubType[i] )
		if( m_nSubType == i )
		{
			m_pBtn_SubType[i]->SetPushed(true);
			m_pBtn_SubType[i]->SetColor(A3DCOLORRGB(255, 203, 74));
		}
		else
		{
			m_pBtn_SubType[i]->SetPushed(false);
			m_pBtn_SubType[i]->SetColor(A3DCOLORRGB(255, 255, 255));
		}
	if( m_nSubType == -1 )
	{
		((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SubType-1"))->SetPushed(true);
		((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SubType-1"))->SetColor(A3DCOLORRGB(255, 203, 74));
	}
	else
	{
		((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SubType-1"))->SetPushed(false);
		((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SubType-1"))->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	
	GShopItemsPtr items = GetShopItems();
	GShopCollector collector(items);
	for (i = 0; i < (int)items->size(); i++)
	{
		const GSHOP_ITEM &item = (*items)[i];
		if (item.main_type == m_nMainType && (m_nSubType == -1 || item.sub_type == m_nSubType) &&
			!ShouldIgnore(item)){
			collector.PushBack(i);
		}
	}
	collector.GetSorted(m_vecIdItem);
	UpdateView();

	// 切换分页时关闭购买界面
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_QshopBuy");
	if (pDlg) pDlg->Show(false);
}

bool CDlgQShop::ShouldIgnoreByCategory(const GSHOP_ITEM &item)const{
	return GetShopDataConst().GetFashionShopCategory().MatchItem(item);
}

bool CDlgQShop::ShouldIgnoreByNPCID(const GSHOP_ITEM &item)const{
	return !GetShopDataConst().MatchOwnerNPC(item);
}

bool CDlgQShop::ShouldIgnoreWithPlayerLevel(const GSHOP_ITEM &item)const{
	return IsIgnored(item, m_playerLevelOnShow);
}

bool CDlgQShop::ShouldIgnore(const GSHOP_ITEM &item)const{
	return ShouldIgnoreByCategory(item)
		|| ShouldIgnoreByNPCID(item)
		|| ShouldIgnoreWithPlayerLevel(item);
}

void CDlgQShop::UpdateView()
{
	//	m_vecIdItem 变化时调用
	
	if( m_vecIdItem.size() > CECGAMEUIMAN_MAX_QSHOPITEMS )
	{
		m_pScl_Item->SetScrollRange(0, (m_vecIdItem.size() - 1) / 2 - 3);
		m_pScl_Item->Show(true);
	}
	else
	{
		m_pScl_Item->SetScrollRange(0, 0);
		m_pScl_Item->Show(false);
	}
	m_pScl_Item->SetBarLevel(0);
	m_pScl_Item->SetBarLength(-1);
	m_pScl_Item->SetScrollStep(1);
	m_nStartLine = -1;
	
	UpdateScrollPos();
	
	SetSelectItem(999999999, -1);
}

void CDlgQShop::UpdateScrollPos()
{
	//	滚动条位置变化时，更新商品显示

	if (!IsShow())
	{
		//	当前对话框没显示时，留到 OnTick 中更新
		//	否则，将导致 CDlgQShopItem 对话框在 CDlgQShop 没显示时出现（更换界面皮肤时）
		return;
	}

	GShopItemsPtr items = GetShopItems();
	if( m_nStartLine != m_pScl_Item->GetBarLevel() )
	{
		int w0 = 0;
		m_nStartLine = m_pScl_Item->GetBarLevel();
		int nStart = m_nStartLine * 2;
		int nEnd = nStart + CECGAMEUIMAN_MAX_QSHOPITEMS;
		if( nEnd > (int)m_vecIdItem.size() )
			nEnd = m_vecIdItem.size();
		int i;
		for(i = nStart; i < nEnd; i++)
			GetGameUIMan()->m_pDlgQShopDefault[w0++]->SetItem(i - nStart,m_vecIdItem[i]);
		for(i = w0; i < CECGAMEUIMAN_MAX_QSHOPITEMS; i++)
			GetGameUIMan()->m_pDlgQShopDefault[i]->SetItem();
		SetSelectItem(GetData(), (int)GetDataPtr());
	}
}

bool CDlgQShop::GetFirstBuyStatus(const GSHOP_ITEM &item, unsigned int *status/* =NULL */)
{
	//	搜索商品第一个 price 合法时对应的 status
	//	无任何 price 合法时认为商品失效
	//	price 合法时对应 status 可用于排序显示
	for(int j = 0; j < ARRAY_SIZE(item.buy); j++)
	{
		if (item.buy[j].price > 0)
		{
			if (status)
				*status = item.buy[j].status;
			return true;
		}
	}
	return false;
}

bool CDlgQShop::IsIgnored(const GSHOP_ITEM &item, int playerLevel)
{
	bool bIgnored = false;

	//	检查是否被配置文件过滤
	if (GetFirstBuyStatus(item)){
		bIgnored = CECQShopConfig::Instance().IsFilteredByLevel((int)item.id, playerLevel);
	}else{
		bIgnored = true;
	}

	return bIgnored;
}

PAUIIMAGEPICTURE CDlgQShop::GetImageGfx(){
	return m_pImgGfx;
}

void CDlgQShop::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (pObj == m_pLst_Search)
	{
		//	商品搜索列表只滚动自身
		return;
	}

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 )
		zDelta = 1;
	else
		zDelta = -1;
	if( m_pScl_Item->IsShow() )
		m_pScl_Item->SetBarLevel(m_pScl_Item->GetBarLevel() - zDelta);
}

void CDlgQShop::SetSelectItem(int nIndex, int nBuyIndex)
{
	AUIOBJECT_SETPROPERTY p1, p2;
	m_pImg_Normal->GetProperty("Image File", &p1);
	m_pImg_Highlight->GetProperty("Image File", &p2);
	for(int i = 0; i < CECGAMEUIMAN_MAX_QSHOPITEMS; i++ )
	{
		CDlgQShopItem *pDlg = GetGameUIMan()->m_pDlgQShopDefault[i];
		bool bSelect = pDlg->GetItemIndex() == nIndex;
		ScopedDialogSpriteModify _dummy(pDlg);
		pDlg->SetProperty("Frame Image", &(bSelect ? p2 : p1));
	}

	SetData(nIndex);
	SetDataPtr((void *)nBuyIndex);
	GShopItemsPtr items = GetShopItems();
	m_pTxt_ItemName->SetText(GetStringFromTable(821));

	AUIOBJECT_SETPROPERTY p;
	if( nIndex < (int)items->size() )
	{
		strcpy(p.fn, (*items)[nIndex].icon + strlen("Surfaces\\"));
		m_pImg_Item->Show(true);
		{
			ScopedAUIControlSpriteModify _dummy(m_pImg_Item);
			m_pImg_Item->SetProperty("Image File", &p);
		}
		AUICTranslate trans;
		m_pTxt_ItemDesc->Show(true);
		m_pTxt_ItemDesc->SetText(trans.Translate((*items)[nIndex].desc));
		CECIvtrItem *pItem = CECIvtrItem::CreateItem((*items)[nIndex].id, 0, 1);
		if( pItem )
		{
			m_pTxt_ItemName->SetText((*items)[nIndex].szName);
			
			int oldItem = (int)m_pTxt_ItemName->GetData();
			if (oldItem != pItem->GetTemplateID())
			{
				m_pTxt_ItemName->SetData(pItem->GetTemplateID());

				PAUIDIALOG pDlgViewModel = GetGameUIMan()->GetDialog("Win_ViewModel");
				if (GetGameUIMan()->m_pDlgFittingRoom->IsShow() ||
					pDlgViewModel && pDlgViewModel->IsShow())
					OnCommand_Fit("");
			}

			if (nBuyIndex>=0 && nBuyIndex<4)
			{
				// 打折及赠品相关
				//
				const GSHOP_ITEM &curItem = (*items)[nIndex];
				unsigned int status = curItem.buy[nBuyIndex].status;
				bool discount = (status>=4 && status <=12);

				// 打折底层图标
				m_pImg_Bg01->Show(discount);
				
				// 打折角标
				m_pImg_Bg02->Show(discount);
				if (discount)
					m_pImg_Bg02->FixFrame(status-4);    // status=4 对应打1折
				
				// 赠品
				m_pImg_Bg03->Show(curItem.idGift>0 && curItem.iGiftNum>0);

				//	闪购
				m_pImg_Bg04->Show(status == 13);
			}
			else
			{
				m_pImg_Bg01->Show(false);
				m_pImg_Bg02->Show(false);
				m_pImg_Bg03->Show(false);
				m_pImg_Bg04->Show(false);
			}
			
			delete pItem;
		}
	}
	else
	{
		m_pImg_Item->Show(false);
		m_pTxt_ItemDesc->Show(false);
		m_pImg_Bg01->Show(false);
		m_pImg_Bg02->Show(false);
		m_pImg_Bg03->Show(false);
		m_pImg_Bg04->Show(false);
	}
	if (IsShow()){
		GetGameUIMan()->BringWindowToTop(this);
	}
}

ACString CDlgQShop::GetCashText(int nCash, bool bFullText)
{
	return GetGameUIMan()->GetCashText(nCash, bFullText);
}

ACString CDlgQShop::GetFormatTime(int nIndex, int nBuyIndex)
{
	abase::vector<GSHOP_ITEM> *items = GetShopItems();
	
	if(nIndex >= 0 && nIndex < (int)(*items).size() 
		&& nBuyIndex >= 0 && nBuyIndex < 4)
		return CDlgQShopItem::GetFormatTime((*items)[nIndex].buy[nBuyIndex].time);

	return _AL("");
}

void CDlgQShop::UpdateQshopData()
{
	//	商品发生变化（有效性、内容）
	ShowMainType(m_nMainType);
	
	//	清空搜索列表残留内容
	if (m_pTxt_Search)
		m_pTxt_Search->SetText(_AL(""));
}

bool CDlgQShop::IsQShop()const
{
	return m_szName == "Win_QShop";
}

bool CDlgQShop::IsBackShop()const
{
	return m_szName == "Win_BackShop";
}

CDlgQShop::GShopItemsPtr CDlgQShop::GetShopItems()
{
	return IsQShop() ? globaldata_getgshopitems() : globaldata_getgshopitems2();
}

CECShopBase & CDlgQShop::GetShopData(){
	return const_cast<CECShopBase &>(GetShopDataConst());
}

const CECShopBase & CDlgQShop::GetShopDataConst()const{
	if (IsQShop()){
		return CECQShop::Instance();
	}
	return CECBackShop::Instance();
}

abase::vector<GSHOP_MAIN_TYPE> * CDlgQShop::GetShopMainTypes()
{
	return IsQShop() ? globaldata_getgshopmaintypes() : globaldata_getgshopmaintypes2();
}

int CDlgQShop::GetCash()
{
	return IsQShop() ? GetHostPlayer()->GetCash() : GetHostPlayer()->GetDividend();
}

void CDlgQShop::OnCommand_QShop(const char *szCommand)
{
	CECUIHelper::ShowQShop(GetShopDataConst().GetOwnerNPCID(), true);
}

void CDlgQShop::OnCommand_BShop(const char *szCommand)
{
	if (CECCrossServer::Instance().IsOnSpecialServer())
		return;

	CDlgBase *pShop = (CDlgBase*)GetGameUIMan()->m_pDlgBShop;
	
	POINT pos = GetPos();
	pShop->SetPosEx(pos.x, pos.y);
	
	OnCommand_CANCEL("");
	pShop->Show(true);
}

void CDlgQShop::OnCommand_BackShop(const char *szCommand)
{
	CECUIHelper::ShowBackShop(GetShopDataConst().GetOwnerNPCID(), true);
}

void CDlgQShop::OnCommand_BackOthers(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_BackOthers");
	if (pDlg)
	{
		pDlg->Show(!pDlg->IsShow());
		if (pDlg->IsShow()){
			OnCommand_CANCEL("");
		}
	}
}

void CDlgQShop::OnCommand_BackSelf(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_BackSelf");
	if (pDlg)
	{
		pDlg->Show(!pDlg->IsShow());
		if (pDlg->IsShow()){
			OnCommand_CANCEL("");
		}
	}
}

void CDlgQShop::OnCommand_Fit(const char *szCommand)
{
	if (!CanFitCurrentItem())
		return;
	
	int tid = m_pTxt_ItemName->GetData();
	if (tid)
	{
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(tid, 0, 1);
		pItem->GetDetailDataFromLocal();

		CDlgFittingRoom * pDlgFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
		PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ViewModel");
		CDlgViewModel *pDlgViewModel = NULL;
		if (pDlg) pDlgViewModel = dynamic_cast<CDlgViewModel *>(pDlg);

		if (pItem->GetClassID() == CECIvtrItem::ICID_FASHION)
		{
			PAUIOBJECT pImage = pDlgFittingRoom->GetEquipIcon(pItem);
			if(pImage){
				int i = atoi(pImage->GetName() + strlen("Equip_"));
				
				// 随机服装的颜色（除去时装头饰）
				if (i != EQUIPIVTR_FASHION_HEAD)
				{
					CECIvtrFashion *pFashion = static_cast<CECIvtrFashion *>(pItem);
					IVTR_ESSENCE_FASHION essence = pFashion->GetEssence();
					A3DCOLORVALUE colorValue;
					colorValue.Set((float)rand()/(RAND_MAX), (float)rand()/(RAND_MAX), (float)rand()/(RAND_MAX), 1.0f);
					A3DCOLOR color = colorValue.ToRGBAColor();
					essence.color = (unsigned short)FASHION_A3DCOLOR_TO_WORDCOLOR(color);
					pFashion->SetItemInfo((BYTE *)&essence, sizeof(essence));
				}
				
				// Now everything is ready
				
				// Show the FittingRoom first
				if (!pDlgFittingRoom->IsShow())
					pDlgFittingRoom->Show(true);
				if (pDlgViewModel && pDlgViewModel->IsShow()){
					POINT pos = pDlgViewModel->GetPos();
					pDlgFittingRoom->SetPosEx(pos.x, pos.y);
					pDlgViewModel->Show(false);
				}
				
				// Bring the FittingRoom to the top
				GetGameUIMan()->BringWindowToTop(pDlgFittingRoom);
				
				// Use(Override) the item in the FittingRoom later
				pDlgFittingRoom->SetEquipIcon(pItem, pImage);
			}
		}
		else if (pDlgViewModel)
		{
			CECModel *pModel = CECViewModelExtractor::Extract(pItem);
			if (pModel){
				if (!pDlgViewModel->IsShow())
					pDlgViewModel->Show(true);
				if (pDlgFittingRoom->IsShow()){
					POINT pos = pDlgFittingRoom->GetPos();
					pDlgViewModel->SetPosEx(pos.x, pos.y);
					pDlgFittingRoom->Show(false);
				}
				GetGameUIMan()->BringWindowToTop(pDlgViewModel);

				const char *szAct = NULL;
				switch (pItem->GetClassID())
				{
				case CECIvtrItem::ICID_PETEGG:
					szAct = CECNPC::GetBaseActionName(CECNPC::ACT_STAND);
					break;
				case CECIvtrItem::ICID_FLYSWORD:
					{						
						CECIvtrFlySword *pFlySword = dynamic_cast<CECIvtrFlySword *>(pItem);
						switch (CECPlayer::FlyMode2WingType(pFlySword->GetDBEssence()->fly_mode)){
						case WINGTYPE_WING:
							szAct = "飞行悬停";
							break;
						case WINGTYPE_FLYSWORD:
							szAct = "飞剑悬停";
							break;
						case WINGTYPE_DOUBLEWHEEL:
							szAct = "悬停";
							break;
						}
					}
					break;
				}
				pDlgViewModel->SetModel(pModel, szAct);
			}
		}

		delete pItem;
	}
	
}

// for notify user the layout changing is over
void CDlgQShop::OnChangeLayoutEnd(bool bAllDone)
{
	abase::vector<GSHOP_MAIN_TYPE> *maintype = GetShopMainTypes();
	DWORD i;
	for(i = 0; i < maintype->size(); i++ )
	{
		if( m_pBtn_MainType[i] )
		{
			m_pBtn_MainType[i]->SetText((*maintype)[i].szName);
			m_pBtn_MainType[i]->Show(true);
		}
	}

	for(; i < CDLGQSHOP_BUTTONMAX; i++ )
	{
		if( m_pBtn_MainType[i] )
		m_pBtn_MainType[i]->Show(false);
	}

	ShowMainType(m_nMainType);
}

void CDlgQShop::OnCommand_Coupon(const char *szCommand)
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_Coupon");
	if(pDlg) pDlg->Show(true);
}

void CDlgQShop::OnCommand_UserAddCash(const char *szCommand)
{
	if (GetBaseUIMan()->NavigateTWRechargeURL()){
		return;
	}

	//	快速点卡充值功能

	PAUIOBJECT pTxt_CardID = GetDlgItem("Txt_CardID");
	PAUIOBJECT pTxt_CardPasswd = GetDlgItem("Txt_CardPasswd");
	if (pTxt_CardID && pTxt_CardPasswd)
	{
		ACString strID = pTxt_CardID->GetText();
		ACString strPasswd = pTxt_CardPasswd->GetText();

		//	除去首尾空格，验证合法性
		strID.TrimLeft();
		strID.TrimRight();
		if (strID.IsEmpty() || strPasswd.IsEmpty())
		{
			if (strID.IsEmpty() && strPasswd.IsEmpty()){
				GetBaseUIMan()->NavigateDefaultRechargeURL();
			}
			return;
		}

		//	转换为UTF8格式
		char utf8[256];
		WideCharToMultiByte(CP_UTF8, 0, strID, -1, utf8, 256, NULL, NULL);
		AString strUTF8ID = utf8;
		
		WideCharToMultiByte(CP_UTF8, 0, strPasswd, -1, utf8, 256, NULL, NULL);
		AString strUTF8Passwd = utf8;

		//	发送往服务器
		GetGameSession()->usercash_InstantAdd(GNET::Octets(strUTF8ID, strUTF8ID.GetLength()), GNET::Octets(strUTF8Passwd, strUTF8Passwd.GetLength()));

		//	清除输入内容
		pTxt_CardID->SetText(_AL(""));
		pTxt_CardPasswd->SetText(_AL(""));
	}
}

void CDlgQShop::OnCommand_QuickPay(const char *szCommand)
{
	if (CECUIConfig::Instance().GetGameUI().bEnableQuickPay)
		GetGameUIMan()->m_pDlgQuickPay->Show(true);
}
void CDlgQShop::OnUserCashAction(void *pProtocol)
{
	GNET::Protocol *p = (GNET::Protocol *)pProtocol;
	switch(p->GetType())
	{
	case PROTOCOL_USERADDCASH_RE:
		{
			GNET::UserAddCash_Re *p1 = (GNET::UserAddCash_Re *)p;
			if (p1->retcode != ERR_SUCCESS)
			{
				//	充值失败，显示失败原因
				ACString errMsg;
				if (p1->retcode == -1)
					errMsg = GetStringFromTable(9372);
				else
					GetGameSession()->GetServerError(p1->retcode, errMsg);

				ACString strTemp;
				strTemp.Format(GetStringFromTable(9371), errMsg);
				GetGameUIMan()->ShowErrorMsg(strTemp);
			}
			else
			{
				//	充值成功
				GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9370));
			}
		}
		break;
	}
}

void CDlgQShop::OnEventKeyDown_Txt_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_ESCAPE)
		ChangeFocus(NULL);	//	释放焦点，便于二次ESC关闭商城
	else if (m_pLst_Search && m_pLst_Search->GetCount() > 1)
	{
		//	使用上下箭头从列表中挑选不同商品
		int nCurSel = m_pLst_Search->GetCurSel();
		int nNewSel = nCurSel;
		if (nNewSel < 0 || nNewSel >= m_pLst_Search->GetCount())
			nNewSel = 0;
		if (wParam == VK_UP)
		{
			if (nNewSel > 0) nNewSel --;
		}
		else if (wParam == VK_DOWN)
		{
			if (nNewSel+1 < m_pLst_Search->GetCount())
				nNewSel ++;
		}
		if (nNewSel != nCurSel)
		{
			m_pLst_Search->SetCurSel(nNewSel);
			m_pLst_Search->EnsureVisible(nNewSel);
		}
	}
}

void CDlgQShop::OnEventDBClk_Lst_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//	有搜索列表框控件时，搜索控件中被选中（或第一项）结果
	if (!m_pLst_Search ||m_pLst_Search->GetCount() <= 0) return;
	int nCurSel = m_pLst_Search->GetCurSel();
	if (nCurSel < 0 || nCurSel >= m_pLst_Search->GetCount())
		nCurSel = 0;
	int itemIndex = m_pLst_Search->GetItemData(nCurSel);
	ShowSearchItem(itemIndex);
	ChangeFocus(NULL);	// 通过修改焦点来隐藏搜索框
}

bool CDlgQShop::SearchByPattern(const ACString &strPattern, QShopItemIndexArray &result, int maxSearchCount)
{
	GShopItemsPtr pItems = GetShopItems();
	GShopSearcher searcher(this, strPattern);
	GShopCollector collector(pItems);
	
	int matched = -1;
	
	//	先使用字符匹配模式搜索
	while (searcher.Next(matched))
	{
		collector.PushBack(matched);
		if (maxSearchCount > 0 && collector.GetCount() >= maxSearchCount)
			break;
	}
	
	//	无法搜索到目标时，将搜索框输入解释成ID再搜索，内部使用，便于确定对象
	if (collector.GetSorted(result) <= 0 && searcher.SearchByID())
	{
		while (searcher.Next(matched))
		{
			collector.PushBack(matched);
			if (maxSearchCount > 0 && collector.GetCount() >= maxSearchCount)
				break;
		}
	}
	
	collector.GetSorted(result);

	return !result.empty();
}

void CDlgQShop::OnCommand_Search(const char *szCommand)
{	
	ACString strPattern = GetSearchPattern();
	if (!strPattern.IsEmpty())
	{
		QShopItemIndexArray result;
		bool itemFound = SearchByPattern(strPattern, result);
		ShowSubType(-2);					//	释放所有子分类按钮的显示
		m_vecIdItem.swap(result);
		UpdateView();
		if (itemFound) ChangeFocus(NULL);	// 通过修改焦点来隐藏搜索框
	}
}

ACString CDlgQShop::GetSearchPattern()
{
	// 获取当前搜索框的有效搜索模式
	ACString strPattern;
	if (m_pTxt_Search)
	{
		strPattern = m_pTxt_Search->GetText();
		strPattern.TrimLeft();  
		strPattern.TrimRight();
		
		// 首尾删除通配符，以匹配有实质意义的内容
		strPattern.TrimRight(_AL("?*"));
		strPattern.TrimLeft(_AL("?*"));
	}
	return strPattern;
}

void CDlgQShop::UpdateSearchList()
{
	// 根据当前搜索框内容，更新搜索结果列表（有最多显示个数限制）
	if (!m_pLst_Search) return;
	
	if (!m_pTxt_Search)
	{
		//	没有搜索内容输入框
		m_pLst_Search->Show(false);
		return;
	}
	
	if (GetFocus() == m_pLst_Search)
	{
		//	正在搜索列表中选择搜索商品
		return;
	}

	if (GetFocus() != m_pTxt_Search)
	{
		//	焦点在其它界面，隐藏搜索列表，避免遮挡影响商城操作
		m_pLst_Search->Show(false);
		return;
	}
	
	ACString strPattern = GetSearchPattern();
	if (strPattern != m_strLastPattern)
	{
		//	搜索内容有变化
		m_pLst_Search->ResetContent();
		m_strLastPattern = strPattern;
		if (!strPattern.IsEmpty())
		{			
			QShopItemIndexArray result;
			if (SearchByPattern(strPattern, result, 20))
			{
				GShopItemsPtr pItems = GetShopItems();
				for (size_t i = 0; i < result.size(); ++ i)
				{
					int matched = result[i];
					const GSHOP_ITEM &item = (*pItems)[matched];
					m_pLst_Search->AddString(item.szName);
					m_pLst_Search->SetItemData(m_pLst_Search->GetCount()-1, matched);
					m_pLst_Search->SetItemHint(m_pLst_Search->GetCount()-1, item.szName);
				}
			}
		}
	}	

	m_pLst_Search->Show(m_pLst_Search->GetCount() > 0);
}

void CDlgQShop::ShowSearchItem(int itemIndex)
{
	// 根据所有商品中的下标 itemIndex，自动切换对应主分类和子分类，并翻到对应页（CECGAMEUIMAN_MAX_QSHOPITEMS），选中对应商品

	GShopItemsPtr items = GetShopItems();
	if (itemIndex < 0 || itemIndex >= (int)items->size())
	{
		ASSERT(false);
		return;
	}
	
	const GSHOP_ITEM &item = (*items)[itemIndex];
	
	// 选中主分类
	bool bMainTypeSwitched = false;
	if (m_nMainType != item.main_type)
	{
		ShowMainType(item.main_type);
		bMainTypeSwitched = true;
	}
	
	// 选中子分类
	bool bSubTypeSwitched = false;
	if (bMainTypeSwitched || m_nSubType != item.sub_type)
	{
		ShowSubType(item.sub_type);
		bSubTypeSwitched = true;
	}
	
	// 计算物品在当前子分类中的位置
	int pos = -1;
	for (int j = 0; j < (int)m_vecIdItem.size(); ++ j)
	{
		if (m_vecIdItem[j] == itemIndex)
		{
			pos = j;
			break;
		}
	}
	if (pos < 0)
	{
		ASSERT(false);
		return;
	}
	
	// 如果被查找物品在当前分类下，则先在当前页面查找物品，减少切换
	if (!bSubTypeSwitched )
	{
		for (int k = 0; k < CECGAMEUIMAN_MAX_QSHOPITEMS; ++ k)
		{
			CDlgQShopItem *pDlgItem = GetGameUIMan()->m_pDlgQShopDefault[k];
			if (pDlgItem->GetItemIndex() != itemIndex) continue;
			pDlgItem->OnEventLButtonDown(0, 0, NULL);
			return;
		}
	}
	
	// 其它情况，自动滚动到pos位置并选中
	if (m_vecIdItem.size() > CECGAMEUIMAN_MAX_QSHOPITEMS)
	{
		int iBarLevel = pos/2-3;
		a_ClampFloor(iBarLevel, 0); //	第一页的物品，iBarLevel 都为0
		m_pScl_Item->SetBarLevel(iBarLevel);
	}

	UpdateScrollPos();
	
	for (int k = 0; k < CECGAMEUIMAN_MAX_QSHOPITEMS; ++ k)
	{
		CDlgQShopItem *pDlgItem = GetGameUIMan()->m_pDlgQShopDefault[k];
		if (pDlgItem->GetItemIndex() != itemIndex) continue;
		pDlgItem->OnEventLButtonDown(0, 0, NULL);
		return;
	}
	
	// 错误，检查程序代码
	ASSERT(false);
}

int CDlgQShop::GetItemPrice(int nIndex, int nBuyIndex)
{
	abase::vector<GSHOP_ITEM> *items = GetShopItems();
	
	if(nIndex >= 0 && nIndex < (int)(*items).size() 
		&& nBuyIndex >= 0 && nBuyIndex < 4)
		return (*items)[nIndex].buy[nBuyIndex].price;
	
	return 0;
}

bool CDlgQShop::IsFashionShopCategory(int mainType){
	return GetShopData().GetFashionShopCategory().MatchMainType(mainType);
}

bool CDlgQShop::ShowCategory(int mainType, int subType, unsigned int ownerNPCID)
{
	bool bShown(false);
	abase::vector<GSHOP_MAIN_TYPE> *pMaintypes = GetShopMainTypes();
	while (true)
	{
		if (mainType < 0 || mainType >= (int)pMaintypes->size()){
			break;
		}
		if (subType < 0 && subType != -1){
			break;
		}
		if (subType >= 0){
			if (subType >= ARRAY_SIZE(m_pBtn_SubType) ||
				m_pBtn_SubType[subType] == NULL){
				break;
			}
		}
		if (IsFashionShopCategory(mainType)){
			bShown = ShowFashionShop(ownerNPCID);
			break;
		}
		CECUIHelper::ShowShop(IsQShop(), ownerNPCID, true);
		if (m_nMainType != mainType){
			ShowMainType(mainType);
			ShowSubType(subType);
		}else if (m_nSubType != subType){
			ShowSubType(subType);
		}
		bShown = true;
		break;
	}
	return bShown;
}

void CDlgQShop::OnCommand_BuyCard(const char* szCommand)
{
	CECUIHelper::ShowPokerShop(GetDlgItem("Btn_CardBuy"));
}

bool CDlgQShop::ShowFashionShop(unsigned int ownerNPCID)
{
	return CECUIHelper::ShowFashionShop(IsQShop(), this, ownerNPCID);
}

AUIObject * CDlgQShop::GetShopSwitchButton(){
	return IsQShop() ? m_pBtn_BackShop : m_pBtn_QShop;
}

void CDlgQShop::EnableShopSwitch(bool bEnable){
	GetShopSwitchButton()->Enable(bEnable);
}

bool CDlgQShop::IsShopSwitchEnabled(){
	return GetShopSwitchButton()->IsEnabled();
}

void CDlgQShop::ClearOwnerNPC(){	
	if (GetShopDataConst().GetOwnerNPCID()){
		GetShopData().SetOwnerNPCID(0);
	}
	EnableShopSwitch(true);
}