/*
 * FILE: EC_IvtrItem.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AWString.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrItem
//	
///////////////////////////////////////////////////////////////////////////

class CECInventory;

class CECIvtrItem
{
public:		//	Types

	//	Inventory item class ID
	enum
	{
		ICID_ITEM	= -100,
		ICID_EQUIP	= -101,
		ICID_ARMOR	= 0,
		ICID_ARMORRUNE,
		ICID_ARROW,
		ICID_DECORATION,
		ICID_DMGRUNE,
		ICID_ELEMENT,
		ICID_FASHION,
		ICID_FLYSWORD,
		ICID_MATERIAL,
		ICID_MEDICINE,
		ICID_REVSCROLL,
		ICID_SKILLTOME,
		ICID_TOSSMAT,
		ICID_TOWNSCROLL,
		ICID_UNIONSCROLL,
		ICID_WEAPON,
		ICID_TASKITEM,
		ICID_STONE,
		ICID_WING,
		ICID_TASKDICE,
		ICID_TASKNMMATTER,
		ICID_ERRORITEM,
		ICID_FACETICKET,
		ICID_FACEPILL,
		ICID_GM_GENERATOR,
		ICID_RECIPE,
		ICID_PETEGG,
		ICID_PETFOOD,
		ICID_PETFACETICKET,
		ICID_FIREWORK,
		ICID_TANKCALLIN,
		ICID_SKILLMATTER,
		ICID_REFINETICKET,
		ICID_DESTROYINGESSENCE,
		ICID_BIBLE,
		ICID_SPEAKER,
		ICID_AUTOHP,
		ICID_AUTOMP,
		ICID_DOUBLEEXP,
		ICID_TRANSMITSCROLL,
		ICID_DYETICKET,
		ICID_GOBLIN,
		ICID_GOBLIN_EQUIP,
		ICID_GOBLIN_EXPPILL,
		ICID_CERTIFICATE,
		ICID_TARGETITEM,
		ICID_LOOKINFOITEM,
		ICID_INCSKILLABILITY,
		ICID_WEDDINGBOOKCARD,
		ICID_WEDDINGINVITECARD,
		ICID_SHARPENER,
		ICID_FACTIONMATERIAL,
		ICID_CONGREGATE,
		ICID_FORCETOKEN,
		ICID_DYNSKILLEQUIP,
		ICID_MONEYCONVERTIBLE,
		ICID_MONSTERSPIRIT,
		ICID_GENERALCARD,
		ICID_GENERALCARD_DICE,
		ICID_SHOPTOKEN,
		ICID_UNIVERSAL_TOKEN,
	};

	//	Item price scale type
	enum
	{
		SCALE_BUY = 1,	//	Buy from NPC
		SCALE_SELL,		//	Sell to NPC
		SCALE_BOOTH,	//	Booth item
		SCALE_MAKE,		//	Make item
		SCALE_OFFLINESHOP, // offline shop
	};
	
	//	Description type
	enum
	{
		DESC_NORMAL = 0,
		DESC_BOOTHBUY,
		DESC_REPAIR,
		DESC_REWARD,
		DESC_PRODUCE,
	};
	
	//	Item use conditions
	enum
	{
		USE_ATKTARGET	= 0x0001,	//	Attack target
		USE_PERSIST		= 0x0002,	//	Persist some time
		USE_TARGET		= 0x0004,	//	Normal target
	};

	//	Proc-type
	enum
	{
		PROC_DROPWHENDIE	= 0x0001,	//	死亡时是否掉落
		PROC_DROPPABLE		= 0x0002,	//	是否可以扔在地上
		PROC_SELLABLE		= 0x0004,	//	是否可以卖给NPC
		PROC_LOG			= 0x0008,	//	记录详细Log信息		——不显示
		
		PROC_TRADEABLE		= 0x0010,	//	是否可以玩家间交易
		PROC_TASK			= 0x0020,	//	是否任务相关物品	——不显示
		PROC_BIND			= 0x0040,	//	装备后绑定			——不显示
		PROC_UNBINDABLE		= 0x0080,	//  可解绑				——不显示

		PROC_DISAPEAR		= 0x0100,	//  离开场景消失
		PROC_USE			= 0x0200,	//  拾取使用
		PROC_DEADDROP		= 0x0400,	//  死亡必定掉落
		PROC_OFFLINE		= 0x0800,	//  下线掉落
		PROC_UNREPAIRABLE	= 0x1000,	//  不可修理
		PROC_DESTROYING		= 0x2000,	//  正在损毁
		PROC_NO_USER_TRASH	= 0x4000,	//  无法放入账号仓库
		PROC_BINDING		= 0x8000,	//  天人合一
		PROC_CAN_WEBTRADE = 0x10000,	//	寻宝网可交易
	};

public:		//	Constructor and Destructor

	CECIvtrItem(int tid, int expire_date);
	CECIvtrItem(const CECIvtrItem& s);
	virtual ~CECIvtrItem() {}

public:		//	Attributes

public:		//	Operations

	//	Create an inventory item
	static CECIvtrItem* CreateItem(int tid, int expire_date, int iCount, int idSpace=0);
	//	Check whether item2 is item1's candidate
	static bool IsCandidate(int tid1, int tid2);
	static bool IsCandidate(int tid1, CECIvtrItem* pItem2);
	//	Get scaled price of specified count item
	static int GetScaledPrice(int iUnitPrice, int iCount, int iScaleType, float fScale);
	static bool IsSharpenerProperty(BYTE propertyType);

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item default information from database
	virtual void DefaultInfo() {}
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Use item
	virtual bool Use() { return true; }
	//	Get scaled price of one item
	virtual int GetScaledPrice();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrItem(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL) { return 0; }
	//	Check item use condition
// 	virtual bool CheckUseCondition() { return IsUseable(); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	// Get Item Level
	virtual int GetItemLevel() const {return -1;}

	//	Get item description text
	const wchar_t* GetDesc(int iDescType=DESC_NORMAL, CECInventory* pInventory=NULL)
	{
		m_pDescIvtr = pInventory;

		if (iDescType == DESC_BOOTHBUY)
			return GetBoothBuyDesc();
		else if (iDescType == DESC_REWARD)
			return GetRewardDesc();
		else
			return GetNormalDesc(iDescType == DESC_REPAIR);
	}

	//	Merge item amount with another same kind item
	int MergeItem(int tid, int iAmount);
	//	Add item amount
	int AddAmount(int iAmount);
	//	Set amount
	void SetAmount(int iAmount) { m_iCount = iAmount; }
	//	Set expire date
	void SetExpireDate(int iExpireDate) { m_expire_date = iExpireDate; }

	//	Can this item be equipped to specified position ?
	bool CanEquippedTo(int iSlot) const { return (m_i64EquipMask & (1 << (__int64)iSlot)) ? true : false; }
	
	//  Can this item be put to account box ?
	bool CanPutIntoAccBox() const;

	//	Set / Get item properties
	int GetClassID() const { return m_iCID; }
	int GetTemplateID() const { return m_tid; }
	int GetExpireDate() const { return m_expire_date; }
	int GetCount() const { return m_iCount; }
	void SetCount(int iCount) { m_iCount = iCount; }
	int GetPileLimit() const { return m_iPileLimit; }
	int GetUnitPrice() const { return m_iPrice; }
	void SetUnitPrice(int iPrice) { m_iPrice = iPrice; }
	int GetShopPrice() const { return m_iShopPrice; }
	__int64 GetEquipMask() const { return m_i64EquipMask; }
	void SetPriceScale(int iType, float fScale) { m_iScaleType = iType; m_fPriceScale = fScale; }

	bool IsInNPCPack() const { return m_bIsInNPCPack; }
	//  Set item location: whether in npc pack?
	void SetInNPCPack(bool bInNPCPack) { m_bIsInNPCPack = bInNPCPack; }

	int GetProcType() { return m_iProcType; }
	void SetProcType(int iType) { m_iProcType = iType; }
	bool IsEmbeddable() const { return m_bEmbeddable; }
	bool IsUseable() const { return m_bUseable; }
	bool IsEquipment() const { return m_i64EquipMask ? true : false; }
	bool IsFrozen() const { return m_bFrozen || m_bNetFrozen; }
	virtual bool IsTradeable() const {
		return ((m_iProcType & PROC_TRADEABLE) || 
			(m_iProcType & PROC_BINDING))  ? false : true; }
	virtual bool IsWebTradeable()const{ return IsTradeable() || (m_iProcType & PROC_CAN_WEBTRADE); }
	bool IsSellable() const { return (m_iProcType & PROC_SELLABLE) ? false : true; }
	bool IsRepairable() const { return (m_iProcType & PROC_UNREPAIRABLE) ? false : true; }

	//	Does item data needs to be updated ?
	bool NeedUpdate() { return m_bNeedUpdate; }
	//	Get item's detail data from server
	void GetDetailDataFromSev(int iPack, int iSlot);
	//	Get item's detail data from local database
	void GetDetailDataFromLocal();
	bool IsDataFromLocal()const{ return m_bLocalDetailData; }
	//	Set local frozen flag
	void Freeze(bool bFreeze) { m_bFrozen = bFreeze; }
	//	Set net frozen flag
	void NetFreeze(bool bFreeze) { m_bNetFrozen = bFreeze; }

	//	Get use condition flags
	DWORD GetUseFlags() { return m_dwUseFlags; }
	bool Use_AtkTarget() { return (m_dwUseFlags & USE_ATKTARGET) ? true : false; }
	bool Use_Persist() { return (m_dwUseFlags & USE_PERSIST) ? true : false; }
	bool Use_Target() { return (m_dwUseFlags & USE_TARGET) ? true : false; }

protected:	//	Attributes

	int			m_iCID;			//	Class ID
	int			m_tid;			//	Template id
	int			m_expire_date;	//	Expiration date
	int			m_iCount;		//	Item count
	int			m_iPileLimit;	//	Pile limit number
	int			m_iPrice;		//	Item unit price
	int			m_iShopPrice;	//	Shop price
	__int64		m_i64EquipMask;	//	Equip mask
	bool		m_bEmbeddable;	//	true, embeddable item
	bool		m_bUseable;		//	true, item can be use
	bool		m_bFrozen;		//	Frozen flag set by local reason (cannot be moved, equipped and used)
	bool		m_bNetFrozen;	//	Frozen flag set by net reason (cannot be moved, equipped and used)
	DWORD		m_dwUseFlags;	//	Use condition flags
	int			m_iProcType;	//	proc-type flag

	int			m_iScaleType;	//	Item price scale type
	float		m_fPriceScale;	//	Price scale

	bool		m_bNeedUpdate;	//	true, detail data needs to updated
	bool		m_bUpdating;	//	true, being updating detail data
	DWORD		m_dwUptTime;	//	Time when updating request was sent
	AWString	m_strDesc;		//	Item description
	bool		m_bIsInNPCPack;	//  true, this item is in NPC package, used for item description
	bool		m_bLocalDetailData;	//	true, data from GetDetailDataFromLocal

	CECInventory*	m_pDescIvtr;	//	Inventory only used to get item description

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair) { return m_strDesc.GetLength() ? m_strDesc : NULL; }
	//	Get item description text for booth buying
	virtual const wchar_t* GetBoothBuyDesc() { return GetNormalDesc(false); }
	//	Get item description text for rewarding
	virtual const wchar_t* GetRewardDesc();
	//	Add price description
	virtual void AddPriceDesc(int col, bool bRepair);
	//	Add profession requirment description
	virtual void AddProfReqDesc(int iProfReq);
	//	Get item name color
	virtual int DecideNameCol();
	
	//  Set properties to local
	virtual void SetLocalProps() {};

	//	Add content to description string
	void AddDescText(int iCol, bool bRet, const ACHAR* szText, ...);
	//	Add extent description to description string
	void AddExtDescText();
	//	Add expire time desc
	void AddExpireTimeDesc();
	void AddExpireTimeDesc(int expire_date);
	//  Add item id desc
	void AddIDDescText();
	//	Add binding desc
	void AddBindDescText();
	//	Add action type desc
	void AddActionTypeDescText(int action_type);
	//	Trim the last '\r' in description string
	void TrimLastReturn();
	//	Build price number string
	void BuildPriceNumberStr(int iPrice, AWString& str);
	void BuildPriceNumberStr(unsigned int iPrice, AWString& str);

	//	Get item's color string id
	int GetColorStrID(int tid);
	
	//	Convert float percent property parameter to int value
	int VisualizeFloatPercent(int p) { return (int)(*(float*)&p * 100.0f + 0.5f); }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrUnknown
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrUnknown : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrUnknown(int tid);
	CECIvtrUnknown(const CECIvtrUnknown& s);
	virtual ~CECIvtrUnknown() {}

public:		//	Attributes

public:		//	Operations

	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrUnknown(*this); }

protected:	//	Attributes

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


