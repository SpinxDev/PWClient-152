#include "EC_Global.h"
#include "EC_IvtrDestroyingEssence.h"
#include "EC_Game.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "../CCommon/elementdataman.h"
#include "EC_RTDebug.h"

#define new A_DEBUG_NEW

CECIvtrDestroyingEssence::CECIvtrDestroyingEssence(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_DESTROYINGESSENCE;

	memset(&m_Essence, 0, sizeof (m_Essence));

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (DESTROYING_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bEmbeddable	= true;
}

CECIvtrDestroyingEssence::CECIvtrDestroyingEssence(const CECIvtrDestroyingEssence& s) : CECIvtrItem(s)
{
	m_Essence		= s.m_Essence;
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrDestroyingEssence::~CECIvtrDestroyingEssence()
{
}

//	Set item detail information
bool CECIvtrDestroyingEssence::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);

	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		m_Essence = *(IVTR_ESSENCE_DESTROYING*)dr.Read_Data(sizeof (IVTR_ESSENCE_DESTROYING));
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrDestroyingEssence::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrDestroyingEssence::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrDestroyingEssence::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrDestroyingEssence::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(m_Essence.tid, 0, 1);

	if (pItem)
	{
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_DESTROYINGNAME), pItem->GetName());
		delete pItem;
	}

	AddIDDescText();

	AddExpireTimeDesc();

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrDestroyingEssence::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
