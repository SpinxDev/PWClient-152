#include "stdafx.h"
#include "Global.h"
#include "ElementData.h"
#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"
#include "ExpTypes.h"
#include "BaseDataIDMan.h"
#include "elementdataman.h"
#include "WinUser.h"
#include "TalkModifyDlg.h"
#include "EC_MD5Hash.h"
#include "Policy.h"
#include "ElementDataDoc.h"
#include "AVariant.h"
#include "VssOperation.h"
#include "DlgProgressNotify.h"
#include "BaseDataExp.h"
#include <AFileImage.h>

static const char* _replace = "!$";

bool HasMd5(const char* szPath)
{
	CStdioFile file(szPath, CStdioFile::modeRead);
	CString csLine;
	while (file.ReadString(csLine))
	{
		if (csLine.Find("Md:")!= -1)
		{
			file.Close();
			return true;
		}		
	}
	file.Close();
	return false;
}

void ConvertMultilineText(char* szText, bool bToMultiline)
{
	if (bToMultiline)
	{
		char* pNext = strstr(szText, _replace);

		while (pNext)
		{
			pNext[0] = 0xD;
			pNext[1] = 0xA;

			pNext += 2;
			pNext = strstr(szText, _replace);
		}
	}
	else
	{
		char* pNext = strstr(szText, "\r\n");

		while (pNext)
		{
			pNext[0] = _replace[0];
			pNext[1] = _replace[1];

			pNext += 2;
			pNext = strstr(szText, "\r\n");
		}
	}
}

static const int PATH_LEN			= 128;
static const int NAME_LEN			= 32;
static const int TEXT_LEN			= 256;
static const int SINGLE_TEXT_LEN	= 16;
static const int ANSI_TEXT_LEN		= 32;
static const int PAGE_TITLE_LEN		= 8;

#define FILL_BASE(tmpl, pStruct) \
{ \
	pStruct->id = tmpl.GetID(); \
	wcsncpy( \
		(wchar_t*)pStruct->name, \
		CSafeString(tmpl.GetName()), \
		sizeof(pStruct->name) / sizeof(namechar) - 1); \
}

const int _base_size = sizeof(unsigned int) + sizeof(namechar) * NAME_LEN;

inline bool Fill_Data(char* pBuf, const BaseDataTempl& tmpl, int nSize, int i = 0)
{
	char* pOrg = pBuf;

	for (; i < tmpl.GetItemNum(); i++)
	{
		if (tmpl.GetItemType(i) == "path")
		{
			strncpy(pBuf, AString(tmpl.GetItemValue(i)), PATH_LEN - 1);
			pBuf += PATH_LEN;
		}
		else if (tmpl.GetItemType(i) == "vector")
		{
			A3DVECTOR3 vec = tmpl.GetItemValue(i);
			*(float*)pBuf = vec.x;
			pBuf += sizeof(float);
			*(float*)pBuf = vec.y;
			pBuf += sizeof(float);
			*(float*)pBuf = vec.z;
			pBuf += sizeof(float);
		}
		else if (tmpl.GetItemType(i) == "text_type")
		{
			CSafeString str(AString(tmpl.GetItemValue(i)));
			ConvertMultilineText(const_cast<char*>(str.GetAnsi()), true);
			wcsncpy((wchar_t*)pBuf, str, TEXT_LEN - 1);
			pBuf += TEXT_LEN * sizeof(namechar);
		}
		else if (tmpl.GetItemType(i) == "single_text")
		{
			wcsncpy((wchar_t*)pBuf, CSafeString(AString(tmpl.GetItemValue(i))), SINGLE_TEXT_LEN - 1);
			pBuf += SINGLE_TEXT_LEN * sizeof(namechar);
		}
		else if (tmpl.GetItemType(i) == "ansi_text")
		{
			strncpy((char*)pBuf, AString(tmpl.GetItemValue(i)), ANSI_TEXT_LEN - 1);
			pBuf += ANSI_TEXT_LEN;
		}
		else if (tmpl.GetItemType(i) == "page_title")
		{
			wcsncpy((wchar_t*)pBuf, CSafeString(AString(tmpl.GetItemValue(i))), PAGE_TITLE_LEN - 1);
			pBuf += PAGE_TITLE_LEN * sizeof(namechar);
		}
		else if (tmpl.GetItemType(i) == "name_text")
		{
			wcsncpy((wchar_t*)pBuf, CSafeString(AString(tmpl.GetItemValue(i))), NAME_LEN - 1);
			pBuf += NAME_LEN * sizeof(namechar);
		}
		else if (tmpl.GetItemType(i) == "equip_fashion_mask")
		{
//			*(int*)pBuf = ((int)tmpl.GetItemValue(i)) << 13;
			int nMaskEdit = (int)tmpl.GetItemValue(i);
			int nMaskClient = 0;
			if (nMaskEdit & 0x01) nMaskClient |= 0x00002000;	// EQUIP_MASK_FASHION_BODY
			if (nMaskEdit & 0x02) nMaskClient |= 0x00004000;	// EQUIP_MASK_FASHION_LEG
			if (nMaskEdit & 0x04) nMaskClient |= 0x00008000;	// EQUIP_MASK_FASHION_FOOT
			if (nMaskEdit & 0x08) nMaskClient |= 0x00010000;	// EQUIP_MASK_FASHION_WRIST
			if (nMaskEdit & 0x10) nMaskClient |= 0x02000000;	// EQUIP_MASK_FASHION_HEAD
			if (nMaskEdit & 0x20) nMaskClient |= 0x20000000;	// EQUIP_MASK_FASHION_WEAPON
			*(int*)pBuf = nMaskClient;
			pBuf += sizeof(int);
		}
		else
		{
			*(int*)pBuf = tmpl.GetItemValue(i);
			pBuf += sizeof(int);
		}
	}

	if (pBuf - pOrg != nSize - _base_size)
	{
		char buf[1024];
		sprintf(buf, "填充数据大小出错！！！，Name = %s, Path = %s, TemplSize = %d, StructSize = %d",
			tmpl.GetName(),
			tmpl.GetFilePath(),
			pBuf - pOrg,
			nSize - _base_size);
		::MessageBox(NULL, buf, "错误", MB_ICONSTOP);
		return false;
	}
	return true;
}

bool Fill_EQUIPMENT_ADDON(const ExtendDataTempl& tmpl, EQUIPMENT_ADDON* pStruct)
{
	FILL_BASE(tmpl, pStruct)

#ifdef _DEBUG
	if (tmpl.GetItemNum() > 3)
	{
		char buf[256];
		sprintf(buf, "Export: addon num > 3, Path = %s\n", tmpl.GetFilePath());
		OutputDebugString(buf);
	}
#endif

	pStruct->num_params = tmpl.GetItemNum();
	for (int i = 0; i < tmpl.GetItemNum(); i++)
	{
		if (i == 0) pStruct->param1 = tmpl.GetItemValue(0);
		else if (i == 1) pStruct->param2 = tmpl.GetItemValue(1);
		else if (i == 2) pStruct->param3 = tmpl.GetItemValue(2);
	}

	return true;
}

/*
 *	Weapon
 */
bool Fill_WEAPON_MAJOR_TYPE(const BaseDataTempl& tmpl, WEAPON_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_WEAPON_SUB_TYPE(const BaseDataTempl& tmpl, WEAPON_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WEAPON_SUB_TYPE));
}

bool Fill_WEAPON_ESSENCE(const BaseDataTempl& tmpl, WEAPON_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WEAPON_ESSENCE) - 8);
}

/*
 *	Armor
 */
bool Fill_ARMOR_MAJOR_TYPE(const BaseDataTempl& tmpl, ARMOR_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_ARMOR_SUB_TYPE(const BaseDataTempl& tmpl, ARMOR_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ARMOR_SUB_TYPE));
}

bool Fill_ARMOR_ESSENCE(const BaseDataTempl& tmpl, ARMOR_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ARMOR_ESSENCE) - 8);
}

bool Fill_SUITE_ESSENCE(const BaseDataTempl& tmpl, SUITE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SUITE_ESSENCE));
}

bool Fill_POKER_SUITE_ESSENCE(const BaseDataTempl& tmpl, POKER_SUITE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(POKER_SUITE_ESSENCE));
}

bool Fill_FASHION_MAJOR_TYPE(const BaseDataTempl& tmpl, FASHION_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_FASHION_SUB_TYPE(const BaseDataTempl& tmpl, FASHION_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FASHION_SUB_TYPE));
}

bool Fill_FASHION_ESSENCE(const BaseDataTempl& tmpl, FASHION_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FASHION_ESSENCE) - 8);
}

bool Fill_POKER_SUB_TYPE(const BaseDataTempl& tmpl, POKER_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(POKER_SUB_TYPE));
}

bool Fill_POKER_ESSENCE(const BaseDataTempl& tmpl, POKER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
		
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(POKER_ESSENCE) - 4);
}

/*
 *	Decoration
 */
bool Fill_DECORATION_MAJOR_TYPE(const BaseDataTempl& tmpl, DECORATION_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_DECORATION_SUB_TYPE(const BaseDataTempl& tmpl, DECORATION_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DECORATION_SUB_TYPE));
}

bool Fill_DECORATION_ESSENCE(const BaseDataTempl& tmpl, DECORATION_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DECORATION_ESSENCE) - 8);
}

bool Fill_DESTROYING_ESSENCE(const BaseDataTempl& tmpl, DESTROYING_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DESTROYING_ESSENCE));
}

bool Fill_DOUBLE_EXP_ESSENCE(const BaseDataTempl& tmpl, DOUBLE_EXP_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DOUBLE_EXP_ESSENCE));
}

bool Fill_DYE_TICKET_ESSENCE(const BaseDataTempl& tmpl, DYE_TICKET_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DYE_TICKET_ESSENCE));
}

bool Fill_REFINE_TICKET_ESSENCE(const BaseDataTempl& tmpl, REFINE_TICKET_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(REFINE_TICKET_ESSENCE));
}

/*
 *	Medicine
 */
bool Fill_MEDICINE_MAJOR_TYPE(const BaseDataTempl& tmpl, MEDICINE_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_MEDICINE_SUB_TYPE(const BaseDataTempl& tmpl, MEDICINE_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_MEDICINE_ESSENCE(const BaseDataTempl& tmpl, MEDICINE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MEDICINE_ESSENCE) - 8);
}

/*
 *	Material
 */
bool Fill_MATERIAL_MAJOR_TYPE(const BaseDataTempl& tmpl, MATERIAL_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_MATERIAL_SUB_TYPE(const BaseDataTempl& tmpl, MATERIAL_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_MATERIAL_ESSENCE(const BaseDataTempl& tmpl, MATERIAL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MATERIAL_ESSENCE) - 8);
}

/*
 *	Damage_Rune
 */
bool Fill_DAMAGERUNE_SUB_TYPE(const BaseDataTempl& tmpl, DAMAGERUNE_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_DAMAGERUNE_ESSENCE(const BaseDataTempl& tmpl, DAMAGERUNE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DAMAGERUNE_ESSENCE) - 4);
}

/*
 *	Armor_Rune
 */
bool Fill_ARMORRUNE_SUB_TYPE(const BaseDataTempl& tmpl, ARMORRUNE_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_ARMORRUNE_ESSENCE(const BaseDataTempl& tmpl, ARMORRUNE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ARMORRUNE_ESSENCE) - 4);
}

/*
 *	Skill_Tome
 */
bool Fill_SKILLTOME_SUB_TYPE(const BaseDataTempl& tmpl, SKILLTOME_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_SKILLTOME_ESSENCE(const BaseDataTempl& tmpl, SKILLTOME_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SKILLTOME_ESSENCE) - 4);
}

bool Fill_SHOP_TOKEN_ESSENCE(const BaseDataTempl& tmpl, SHOP_TOKEN_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SHOP_TOKEN_ESSENCE));
}

bool Fill_UNIVERSAL_TOKEN_ESSENCE(const BaseDataTempl& tmpl, UNIVERSAL_TOKEN_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(UNIVERSAL_TOKEN_ESSENCE));
}

bool Fill_FASHION_SUITE_ESSENCE(const BaseDataTempl& tmpl, FASHION_SUITE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FASHION_SUITE_ESSENCE));
}

/*
 *	Faction_Building
 */
bool Fill_FACTION_BUILDING_SUB_TYPE(const BaseDataTempl& tmpl, FACTION_BUILDING_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_FACTION_BUILDING_ESSENCE(const BaseDataTempl& tmpl, FACTION_BUILDING_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACTION_BUILDING_ESSENCE) - 4);
}

/*
 *	Faction_Material
 */
bool Fill_FACTION_MATERIAL_ESSENCE(const BaseDataTempl& tmpl, FACTION_MATERIAL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACTION_MATERIAL_ESSENCE));
}

/*
 *	Fly_Sword
 */
bool Fill_FLYSWORD_ESSENCE(const BaseDataTempl& tmpl, FLYSWORD_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FLYSWORD_ESSENCE));
}

/*
 *	WINGMANWING
 */
bool Fill_WINGMANWING_ESSENCE(const BaseDataTempl& tmpl, WINGMANWING_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WINGMANWING_ESSENCE));
}

/*
 *	Town_Scroll
 */
bool Fill_TOWNSCROLL_ESSENCE(const BaseDataTempl& tmpl, TOWNSCROLL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TOWNSCROLL_ESSENCE));
}

/*
 *	Transmit_Scroll
 */
bool Fill_TRANSMITSCROLL_ESSENCE(const BaseDataTempl& tmpl, TRANSMITSCROLL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TRANSMITSCROLL_ESSENCE));
}

/*
 *	Union_Scroll
 */
bool Fill_UNIONSCROLL_ESSENCE(const BaseDataTempl& tmpl, UNIONSCROLL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(UNIONSCROLL_ESSENCE));
}

/*
 *	Revive_Scroll
 */
bool Fill_REVIVESCROLL_ESSENCE(const BaseDataTempl& tmpl, REVIVESCROLL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(REVIVESCROLL_ESSENCE));
}

/*
 *	Element
 */
bool Fill_ELEMENT_ESSENCE(const BaseDataTempl& tmpl, ELEMENT_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ELEMENT_ESSENCE));
}

/*
 *	Task Matter
 */
bool Fill_TASKMATTER_ESSENCE(const BaseDataTempl& tmpl, TASKMATTER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TASKMATTER_ESSENCE));
}

bool Fill_TASKNORMALMATTER_ESSENCE(const BaseDataTempl& tmpl, TASKNORMALMATTER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TASKNORMALMATTER_ESSENCE));
}

bool Fill_TASKDICE_ESSENCE(const BaseDataTempl& tmpl, TASKDICE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TASKDICE_ESSENCE));
}

bool Fill_POKER_DICE_ESSENCE(const BaseDataTempl& tmpl, POKER_DICE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(POKER_DICE_ESSENCE));
}

/*
 *	Toss Matter
 */
bool Fill_TOSSMATTER_ESSENCE(const BaseDataTempl& tmpl, TOSSMATTER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TOSSMATTER_ESSENCE));
}

/*
 *	Projectile
 */
bool Fill_PROJECTILE_TYPE(const BaseDataTempl& tmpl, PROJECTILE_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_PROJECTILE_ESSENCE(const BaseDataTempl& tmpl, PROJECTILE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	pStruct->type = tmpl.GetItemValue(0);
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PROJECTILE_ESSENCE) - 4, 1);
}

/*
 *	Stone
 */
bool Fill_STONE_SUB_TYPE(const BaseDataTempl& tmpl, STONE_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_STONE_ESSENCE(const BaseDataTempl& tmpl, STONE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(STONE_ESSENCE) - 4);
}

/*
 *	Face ticket
 */
bool Fill_FACETICKET_MAJOR_TYPE(const BaseDataTempl& tmpl, FACETICKET_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_FACETICKET_SUB_TYPE(const BaseDataTempl& tmpl, FACETICKET_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_FACETICKET_ESSENCE(const BaseDataTempl& tmpl, FACETICKET_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACETICKET_ESSENCE) - 8);
}

/*
 *	Face pill
 */
bool Fill_FACEPILL_MAJOR_TYPE(const BaseDataTempl& tmpl, FACEPILL_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_FACEPILL_SUB_TYPE(const BaseDataTempl& tmpl, FACEPILL_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_FACEPILL_ESSENCE(const BaseDataTempl& tmpl, FACEPILL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACEPILL_ESSENCE) - 8);
}

/*
 * GM generator
*/
bool  Fill_GM_GENERATOR_TYPE(const BaseDataTempl& tmpl, GM_GENERATOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_GM_GENERATOR_ESSENCE(const BaseDataTempl& tmpl, GM_GENERATOR_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GM_GENERATOR_ESSENCE) - 4);
}

/*
 * PET things.
 */
bool Fill_PET_EGG_ESSENCE(const BaseDataTempl& tmpl, PET_EGG_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_EGG_ESSENCE));
}

bool Fill_PET_FOOD_ESSENCE(const BaseDataTempl& tmpl, PET_FOOD_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_FOOD_ESSENCE));
}

bool Fill_PET_FACETICKET_ESSENCE(const BaseDataTempl& tmpl, PET_FACETICKET_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_FACETICKET_ESSENCE));
}

/*
 * Fire works
 */
bool Fill_FIREWORKS_ESSENCE(const BaseDataTempl& tmpl, FIREWORKS_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FIREWORKS_ESSENCE));
}

/*
 * War_tankcallin
 */
bool Fill_WAR_TANKCALLIN_ESSENCE(const BaseDataTempl& tmpl, WAR_TANKCALLIN_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WAR_TANKCALLIN_ESSENCE));
}

/*
 *	Quiver
 */
bool Fill_QUIVER_SUB_TYPE(const BaseDataTempl& tmpl, QUIVER_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_QUIVER_ESSENCE(const BaseDataTempl& tmpl, QUIVER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(QUIVER_ESSENCE) - 4);
}

/*
 *	Monster
 */
bool Fill_NPC_TYPE(const BaseDataTempl& tmpl, NPC_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_MONSTER_ADDON(const ExtendDataTempl& tmpl, MONSTER_ADDON* pStruct)
{
	FILL_BASE(tmpl, pStruct)

#ifdef _DEBUG
	if (tmpl.GetItemNum() > 3)
	{
		char buf[256];
		sprintf(buf, "Export: addon num > 3, Path = %s\n", tmpl.GetFilePath());
		OutputDebugString(buf);
	}
#endif

	pStruct->num_params = tmpl.GetItemNum();
	for (int i = 0; i < tmpl.GetItemNum(); i++)
	{
		if (i == 0) pStruct->param1 = tmpl.GetItemValue(0);
		else if (i == 1) pStruct->param2 = tmpl.GetItemValue(1);
		else if (i == 2) pStruct->param3 = tmpl.GetItemValue(2);
	}

	return true;
}

bool Fill_MONSTER_TYPE(const BaseDataTempl& tmpl, MONSTER_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MONSTER_TYPE));
}

bool Fill_MONSTER_ESSENCE(const BaseDataTempl& tmpl, MONSTER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MONSTER_ESSENCE) - 4);
}

bool Fill_PET_TYPE(const BaseDataTempl& tmpl, PET_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_PET_ESSENCE(const BaseDataTempl& tmpl, PET_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_ESSENCE) - 4);
}

bool Fill_GOBLIN_ESSENCE(const BaseDataTempl& tmpl, GOBLIN_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GOBLIN_ESSENCE));
}

bool Fill_GOBLIN_EQUIP_TYPE(const BaseDataTempl& tmpl, GOBLIN_EQUIP_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_GOBLIN_EQUIP_ESSENCE(const BaseDataTempl& tmpl, GOBLIN_EQUIP_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GOBLIN_EQUIP_ESSENCE) - 4);
}

bool Fill_GOBLIN_EXPPILL_ESSENCE(const BaseDataTempl& tmpl, GOBLIN_EXPPILL_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GOBLIN_EXPPILL_ESSENCE));
}

bool Fill_SELL_CERTIFICATE_ESSENCE(const BaseDataTempl& tmpl, SELL_CERTIFICATE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SELL_CERTIFICATE_ESSENCE));
}

bool Fill_TARGET_ITEM_ESSENCE(const BaseDataTempl& tmpl, TARGET_ITEM_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TARGET_ITEM_ESSENCE));
}

bool Fill_LOOK_INFO_ESSENCE(const BaseDataTempl& tmpl, LOOK_INFO_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(LOOK_INFO_ESSENCE));
}

/*
 *	NPC Talk Service
 */
bool Fill_NPC_TALK_SERVICE(const BaseDataTempl& tmpl, NPC_TALK_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_TALK_SERVICE));
}

/*
 *	NPC Sell Service
 */
bool Fill_NPC_SELL_SERVICE(const BaseDataTempl& tmpl, NPC_SELL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_SELL_SERVICE));
}

/*
 *	NPC Buy Service
 */
bool Fill_NPC_BUY_SERVICE(const BaseDataTempl& tmpl, NPC_BUY_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_BUY_SERVICE));
}

/*
 *	NPC Repair Service
 */
bool Fill_NPC_REPAIR_SERVICE(const BaseDataTempl& tmpl, NPC_REPAIR_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_REPAIR_SERVICE));
}

/*
 *	NPC Install Service
 */
bool Fill_NPC_INSTALL_SERVICE(const BaseDataTempl& tmpl, NPC_INSTALL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_INSTALL_SERVICE));
}

/*
 *	NPC Uninstall Service
 */
bool Fill_NPC_UNINSTALL_SERVICE(const BaseDataTempl& tmpl, NPC_UNINSTALL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_UNINSTALL_SERVICE));
}

/*
 *	NPC Task Service
 */
bool Fill_NPC_TASK_OUT_SERVICE(const BaseDataTempl& tmpl, NPC_TASK_OUT_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_TASK_OUT_SERVICE));
}

bool Fill_NPC_TASK_IN_SERVICE(const BaseDataTempl& tmpl, NPC_TASK_IN_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_TASK_IN_SERVICE));
}

bool Fill_NPC_TASK_MATTER_SERVICE(const BaseDataTempl& tmpl, NPC_TASK_MATTER_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_TASK_MATTER_SERVICE));
}

/*
 *	NPC Skill Service
 */
bool Fill_NPC_SKILL_SERVICE(const BaseDataTempl& tmpl, NPC_SKILL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_SKILL_SERVICE));
}

/*
 *	NPC Heal Service
 */
bool Fill_NPC_HEAL_SERVICE(const BaseDataTempl& tmpl, NPC_HEAL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_HEAL_SERVICE));
}

/*
 *	NPC Transmit Service
 */
bool Fill_NPC_TRANSMIT_SERVICE(const BaseDataTempl& tmpl, NPC_TRANSMIT_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_TRANSMIT_SERVICE));
}

/*
 *	NPC Transport Service
 */
bool Fill_NPC_TRANSPORT_SERVICE(const BaseDataTempl& tmpl, NPC_TRANSPORT_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_TRANSPORT_SERVICE));
}

/*
 *	NPC Proxy Service
 */
bool Fill_NPC_PROXY_SERVICE(const BaseDataTempl& tmpl, NPC_PROXY_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_PROXY_SERVICE));
}

/*
 *	NPC Storage Service
 */
bool Fill_NPC_STORAGE_SERVICE(const BaseDataTempl& tmpl, NPC_STORAGE_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_STORAGE_SERVICE));
}

/*
 *	NPC Make Service
 */
bool Fill_NPC_MAKE_SERVICE(const BaseDataTempl& tmpl, NPC_MAKE_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_MAKE_SERVICE));
}

/*
 *	NPC Decompose Service
 */
bool Fill_NPC_DECOMPOSE_SERVICE(const BaseDataTempl& tmpl, NPC_DECOMPOSE_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_DECOMPOSE_SERVICE));
}

bool Fill_NPC_IDENTIFY_SERVICE(const BaseDataTempl& tmpl, NPC_IDENTIFY_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_IDENTIFY_SERVICE));
}

/*
 *	NPC tower build Service
 */
bool Fill_NPC_WAR_TOWERBUILD_SERVICE(const BaseDataTempl& tmpl, NPC_WAR_TOWERBUILD_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_WAR_TOWERBUILD_SERVICE));
}

/*
 *	NPC reset prop Service
 */
bool Fill_NPC_RESETPROP_SERVICE(const BaseDataTempl& tmpl, NPC_RESETPROP_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_RESETPROP_SERVICE));
}

/*
 *	NPC pet name Service
 */
bool Fill_NPC_PETNAME_SERVICE(const BaseDataTempl& tmpl, NPC_PETNAME_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_PETNAME_SERVICE));
}

/*
 *	NPC pet learn skill Service
 */
bool Fill_NPC_PETLEARNSKILL_SERVICE(const BaseDataTempl& tmpl, NPC_PETLEARNSKILL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_PETLEARNSKILL_SERVICE));
}

/*
 *	NPC pet forget skill Service
 */
bool Fill_NPC_PETFORGETSKILL_SERVICE(const BaseDataTempl& tmpl, NPC_PETFORGETSKILL_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_PETFORGETSKILL_SERVICE));
}

/*
 *	NPC equipment bind Service
 */
bool Fill_NPC_EQUIPBIND_SERVICE(const BaseDataTempl& tmpl, NPC_EQUIPBIND_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_EQUIPBIND_SERVICE));
}

/*
 *	NPC equipment destroy Service
 */
bool Fill_NPC_EQUIPDESTROY_SERVICE(const BaseDataTempl& tmpl, NPC_EQUIPDESTROY_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_EQUIPDESTROY_SERVICE));
}

/*
 *	NPC equipment undestroy Service
 */
bool Fill_NPC_EQUIPUNDESTROY_SERVICE(const BaseDataTempl& tmpl, NPC_EQUIPUNDESTROY_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_EQUIPUNDESTROY_SERVICE));
}

/*
 *	NPC engrave Service
 */
bool Fill_NPC_ENGRAVE_SERVICE(const BaseDataTempl& tmpl, NPC_ENGRAVE_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_ENGRAVE_SERVICE));
}

/*
 *	NPC randprop Service
 */
bool Fill_NPC_RANDPROP_SERVICE(const BaseDataTempl& tmpl, NPC_RANDPROP_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_RANDPROP_SERVICE));
}

/*
 *	NPC Force Service
 */
bool Fill_NPC_FORCE_SERVICE(const BaseDataTempl& tmpl, NPC_FORCE_SERVICE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_FORCE_SERVICE));
}

/*
 *	NPC_Essence
 */
bool Fill_NPC_ESSENCE(const BaseDataTempl& tmpl, NPC_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(NPC_ESSENCE));
}

bool  Fill_MINE_TYPE(const BaseDataTempl& tmpl, MINE_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_MINE_ESSENCE(const BaseDataTempl& tmpl, MINE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MINE_ESSENCE) - 4);
}

bool Fill_FACE_TEXTURE_ESSENCE(const BaseDataTempl& tmpl, FACE_TEXTURE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_TEXTURE_ESSENCE));
}

bool Fill_FACE_SHAPE_ESSENCE(const BaseDataTempl& tmpl, FACE_SHAPE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_SHAPE_ESSENCE));
}

bool Fill_FACE_EMOTION_TYPE(const BaseDataTempl& tmpl, FACE_EMOTION_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_EMOTION_TYPE));
}

bool Fill_FACE_EXPRESSION_ESSENCE(const BaseDataTempl& tmpl, FACE_EXPRESSION_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_EXPRESSION_ESSENCE));
}

bool Fill_FACE_HAIR_ESSENCE(const BaseDataTempl& tmpl, FACE_HAIR_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_HAIR_ESSENCE));
}

bool Fill_FACE_MOUSTACHE_ESSENCE(const BaseDataTempl& tmpl, FACE_MOUSTACHE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_MOUSTACHE_ESSENCE));
}

bool Fill_FACE_FALING_ESSENCE(const BaseDataTempl& tmpl, FACE_FALING_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_FALING_ESSENCE));
}

bool Fill_FACE_THIRDEYE_ESSENCE(const BaseDataTempl& tmpl, FACE_THIRDEYE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_THIRDEYE_ESSENCE));
}

bool Fill_COLORPICKER_ESSENCE(const BaseDataTempl& tmpl, COLORPICKER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(COLORPICKER_ESSENCE));
}

bool Fill_CUSTOMIZEDATA_ESSENCE(const BaseDataTempl& tmpl, CUSTOMIZEDATA_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(CUSTOMIZEDATA_ESSENCE));
}

bool Fill_SKILLMATTER_ESSENCE(const BaseDataTempl& tmpl, SKILLMATTER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SKILLMATTER_ESSENCE));
}

bool Fill_DYNSKILLEQUIP_ESSENCE(const BaseDataTempl& tmpl, DYNSKILLEQUIP_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(DYNSKILLEQUIP_ESSENCE));
}

bool Fill_INC_SKILL_ABILITY_ESSENCE(const BaseDataTempl& tmpl, INC_SKILL_ABILITY_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(INC_SKILL_ABILITY_ESSENCE));
}

bool Fill_WEDDING_BOOKCARD_ESSENCE(const BaseDataTempl& tmpl, WEDDING_BOOKCARD_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WEDDING_BOOKCARD_ESSENCE));
}

bool Fill_WEDDING_INVITECARD_ESSENCE(const BaseDataTempl& tmpl, WEDDING_INVITECARD_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WEDDING_INVITECARD_ESSENCE));
}

bool Fill_SHARPENER_ESSENCE(const BaseDataTempl& tmpl, SHARPENER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SHARPENER_ESSENCE));
}

bool Fill_CONGREGATE_ESSENCE(const BaseDataTempl& tmpl, CONGREGATE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(CONGREGATE_ESSENCE));
}

bool Fill_MONSTER_SPIRIT_ESSENCE(const BaseDataTempl& tmpl, MONSTER_SPIRIT_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MONSTER_SPIRIT_ESSENCE));
}

bool Fill_RECIPE_MAJOR_TYPE(const BaseDataTempl& tmpl, RECIPE_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_RECIPE_SUB_TYPE(const BaseDataTempl& tmpl, RECIPE_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_RECIPE_ESSENCE(const BaseDataTempl& tmpl, RECIPE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)

	// 防止升级生产系数为0
	if (pStruct->upgrade_rate < 0.01f)
		pStruct->upgrade_rate = 1.0f;
	if (pStruct->engrave_upgrade_rate < 0.01f)
		pStruct->engrave_upgrade_rate = 1.0f;
	if (pStruct->addon_inherit_fee_rate < 0.01f)
		pStruct->addon_inherit_fee_rate = 1.0f;

	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(RECIPE_ESSENCE) - 8);
}

bool Fill_ENGRAVE_MAJOR_TYPE(const BaseDataTempl& tmpl, ENGRAVE_MAJOR_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_ENGRAVE_SUB_TYPE(const BaseDataTempl& tmpl, ENGRAVE_SUB_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_ENGRAVE_ESSENCE(const BaseDataTempl& tmpl, ENGRAVE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_sub_type = parent.GetID();
	if (!parent.LoadHeader(parent.ParseParentPath())) return false;
	pStruct->id_major_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ENGRAVE_ESSENCE) - 8);
}

bool Fill_RANDPROP_TYPE(const BaseDataTempl& tmpl, RANDPROP_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_RANDPROP_ESSENCE(const BaseDataTempl& tmpl, RANDPROP_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();

	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(RANDPROP_ESSENCE) - 4);
}

bool Fill_STONE_CHANGE_RECIPE_TYPE(const BaseDataTempl& tmpl, STONE_CHANGE_RECIPE_TYPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return true;
}

bool Fill_STONE_CHANGE_RECIPE(const BaseDataTempl& tmpl, STONE_CHANGE_RECIPE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
		
	BaseDataTempl parent;
	if (!parent.LoadHeader(tmpl.ParseParentPath())) return false;
	pStruct->id_type = parent.GetID();
	
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(STONE_CHANGE_RECIPE) - 4);
}

bool Fill_ENEMY_FACTION_CONFIG(const BaseDataTempl& tmpl, ENEMY_FACTION_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ENEMY_FACTION_CONFIG));
}

bool Fill_CHARRACTER_CLASS_CONFIG(const BaseDataTempl& tmpl, CHARRACTER_CLASS_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(CHARRACTER_CLASS_CONFIG));
}

bool Fill_PARAM_ADJUST_CONFIG(const BaseDataTempl& tmpl, PARAM_ADJUST_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PARAM_ADJUST_CONFIG));
}

bool Fill_PLAYER_ACTION_INFO_CONFIG(const BaseDataTempl& tmpl, PLAYER_ACTION_INFO_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_ACTION_INFO_CONFIG));
}

bool Fill_UPGRADE_PRODUCTION_CONFIG(const BaseDataTempl& tmpl, UPGRADE_PRODUCTION_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(UPGRADE_PRODUCTION_CONFIG));
}

bool Fill_ACC_STORAGE_BLACKLIST_CONFIG(const BaseDataTempl& tmpl, ACC_STORAGE_BLACKLIST_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ACC_STORAGE_BLACKLIST_CONFIG));
}

bool Fill_PLAYER_DEATH_DROP_CONFIG(const BaseDataTempl& tmpl, PLAYER_DEATH_DROP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_DEATH_DROP_CONFIG));
}

bool Fill_CONSUME_POINTS_CONFIG(const BaseDataTempl& tmpl, CONSUME_POINTS_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(CONSUME_POINTS_CONFIG));
}

bool Fill_ONLINE_AWARDS_CONFIG(const BaseDataTempl& tmpl, ONLINE_AWARDS_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(ONLINE_AWARDS_CONFIG));
}

bool Fill_FASHION_WEAPON_CONFIG(const BaseDataTempl& tmpl, FASHION_WEAPON_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FASHION_WEAPON_CONFIG));
}

bool Fill_PET_EVOLVE_CONFIG(const BaseDataTempl& tmpl, PET_EVOLVE_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_EVOLVE_CONFIG));
}

bool Fill_PET_EVOLVED_SKILL_CONFIG(const BaseDataTempl& tmpl, PET_EVOLVED_SKILL_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_EVOLVED_SKILL_CONFIG));
}

bool Fill_PET_EVOLVED_SKILL_RAND_CONFIG(const BaseDataTempl& tmpl, PET_EVOLVED_SKILL_RAND_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PET_EVOLVED_SKILL_RAND_CONFIG));
}

bool Fill_AUTOTASK_DISPLAY_CONFIG(const BaseDataTempl& tmpl, AUTOTASK_DISPLAY_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(AUTOTASK_DISPLAY_CONFIG));
}

bool Fill_PLAYER_SPIRIT_CONFIG(const BaseDataTempl& tmpl, PLAYER_SPIRIT_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_SPIRIT_CONFIG));
}

bool Fill_HISTORY_STAGE_CONFIG(const BaseDataTempl& tmpl, HISTORY_STAGE_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(HISTORY_STAGE_CONFIG));
}

bool Fill_HISTORY_ADVANCE_CONFIG(const BaseDataTempl& tmpl, HISTORY_ADVANCE_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(HISTORY_ADVANCE_CONFIG));
}

bool Fill_AUTOTEAM_CONFIG(const BaseDataTempl& tmpl, AUTOTEAM_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(AUTOTEAM_CONFIG));
}

bool Fill_CHARIOT_CONFIG(const BaseDataTempl& tmpl, CHARIOT_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(CHARIOT_CONFIG));
}

bool Fill_CHARIOT_WAR_CONFIG(const BaseDataTempl& tmpl, CHARIOT_WAR_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(CHARIOT_WAR_CONFIG));
}

bool Fill_POKER_LEVELEXP_CONFIG(const BaseDataTempl& tmpl, POKER_LEVELEXP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(POKER_LEVELEXP_CONFIG));
}

bool Fill_GT_CONFIG(const BaseDataTempl& tmpl, GT_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GT_CONFIG));
}

bool Fill_MERIDIAN_CONFIG(const BaseDataTempl& tmpl, MERIDIAN_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MERIDIAN_CONFIG));
}

bool Fill_MONEY_CONVERTIBLE_ESSENCE(const BaseDataTempl& tmpl, MONEY_CONVERTIBLE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MONEY_CONVERTIBLE_ESSENCE));
}

bool Fill_MULTI_EXP_CONFIG(const BaseDataTempl& tmpl, MULTI_EXP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(MULTI_EXP_CONFIG));
}

bool Fill_WEDDING_CONFIG(const BaseDataTempl& tmpl, WEDDING_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WEDDING_CONFIG));
}

bool Fill_PLAYER_LEVELEXP_CONFIG(const BaseDataTempl& tmpl, PLAYER_LEVELEXP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_LEVELEXP_CONFIG));
}

bool Fill_PLAYER_SECONDLEVEL_CONFIG(const BaseDataTempl& tmpl, PLAYER_SECONDLEVEL_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_SECONDLEVEL_CONFIG));
}

bool Fill_PLAYER_REINCARNATION_CONFIG(const BaseDataTempl& tmpl, PLAYER_REINCARNATION_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_REINCARNATION_CONFIG));
}

bool Fill_PLAYER_REALM_CONFIG(const BaseDataTempl& tmpl, PLAYER_REALM_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PLAYER_REALM_CONFIG));
}

bool Fill_FACTION_FORTRESS_CONFIG(const BaseDataTempl& tmpl, FACTION_FORTRESS_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACTION_FORTRESS_CONFIG));
}

bool Fill_FORCE_CONFIG(const BaseDataTempl& tmpl, FORCE_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FORCE_CONFIG));
}

bool Fill_COUNTRY_CONFIG(const BaseDataTempl& tmpl, COUNTRY_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(COUNTRY_CONFIG));
}

bool Fill_GM_ACTIVITY_CONFIG(const BaseDataTempl& tmpl, GM_ACTIVITY_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GM_ACTIVITY_CONFIG));
}

bool Fill_TOUCH_SHOP_CONFIG(const BaseDataTempl& tmpl, TOUCH_SHOP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TOUCH_SHOP_CONFIG));
}

bool Fill_TOKEN_SHOP_CONFIG(const BaseDataTempl& tmpl, TOKEN_SHOP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TOKEN_SHOP_CONFIG));
}

bool Fill_RAND_SHOP_CONFIG(const BaseDataTempl& tmpl, RAND_SHOP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(RAND_SHOP_CONFIG));
}

bool Fill_PROFIT_TIME_CONFIG(const BaseDataTempl& tmpl, PROFIT_TIME_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(PROFIT_TIME_CONFIG));
}

bool Fill_FACTION_PVP_CONFIG(const BaseDataTempl& tmpl, FACTION_PVP_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACTION_PVP_CONFIG));
}

bool Fill_TASK_LIST_CONFIG(const BaseDataTempl& tmpl, TASK_LIST_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TASK_LIST_CONFIG));
}

bool Fill_TASK_DICE_BY_WEIGHT_CONFIG(const BaseDataTempl& tmpl, TASK_DICE_BY_WEIGHT_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TASK_DICE_BY_WEIGHT_CONFIG));
}

bool Fill_FASHION_BEST_COLOR_CONFIG(const BaseDataTempl& tmpl, FASHION_BEST_COLOR_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FASHION_BEST_COLOR_CONFIG));
}

bool Fill_SIGN_AWARD_CONFIG(const BaseDataTempl& tmpl, SIGN_AWARD_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SIGN_AWARD_CONFIG));
}

bool Fill_GOD_EVIL_CONVERT_CONFIG(const BaseDataTempl& tmpl, GOD_EVIL_CONVERT_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(GOD_EVIL_CONVERT_CONFIG));
}

bool Fill_WIKI_TABOO_CONFIG(const BaseDataTempl& tmpl, WIKI_TABOO_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(WIKI_TABOO_CONFIG));
}

bool Fill_TITLE_CONFIG(const BaseDataTempl& tmpl, TITLE_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(TITLE_CONFIG));
}

bool Fill_COMPLEX_TITLE_CONFIG(const BaseDataTempl& tmpl, COMPLEX_TITLE_CONFIG* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(COMPLEX_TITLE_CONFIG));
}

/*
 *	Bible essence
 */
bool Fill_BIBLE_ESSENCE(const BaseDataTempl& tmpl, BIBLE_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(BIBLE_ESSENCE));
}

/*
 *	Speaker essence
 */
bool Fill_SPEAKER_ESSENCE(const BaseDataTempl& tmpl, SPEAKER_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(SPEAKER_ESSENCE));
}

/*
 *	AUTOHP essence
 */
bool Fill_AUTOHP_ESSENCE(const BaseDataTempl& tmpl, AUTOHP_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(AUTOHP_ESSENCE));
}

/*
 *	AUTOMP essence
 */
bool Fill_AUTOMP_ESSENCE(const BaseDataTempl& tmpl, AUTOMP_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(AUTOMP_ESSENCE));
}

bool Fill_FORCE_TOKEN_ESSENCE(const BaseDataTempl& tmpl, FORCE_TOKEN_ESSENCE* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FORCE_TOKEN_ESSENCE));
}

bool Fill_FACE_HAIR_TEXTURE_MAP(const BaseDataTempl& tmpl, FACE_HAIR_TEXTURE_MAP* pStruct)
{
	FILL_BASE(tmpl, pStruct)
	return Fill_Data((char*)(pStruct->name + NAME_LEN), tmpl, sizeof(FACE_HAIR_TEXTURE_MAP));
}


/*
 *	Export all data
 */

#define EXPORT_DATA(type) \
{ \
	type data; \
	memset(&data, 0, sizeof(data)); \
	if (!Fill_##type(tmpl, &data)) return false; \
	DataMan.add_structure(data.id, data); \
	if (bFileDigestOpen)	\
	{	\
		strDigestLine.Format("%d, %s, %s", tmpl.GetID(), tmpl.GetName(), strMD5);	\
		fileDigest.WriteLine(strDigestLine);	\
	}	\
}

//	Calculate a file's md5
static AString CalcFileMD5(const char * szFile)
{
	AString strMD5;

	AFileImage file;
	if (file.Open(szFile, AFILE_BINARY | AFILE_OPENEXIST))
	{
		PATCH::MD5Hash md;
		md.Update((const char *)file.GetFileBuffer(), file.GetFileLength());
		md.Final();
		
		char szBuffer[64];
		unsigned int nLength = 64; 
		md.GetString(szBuffer,nLength);
		strMD5 = szBuffer;
	}
	
	return strMD5;
}

bool CheckMd5CodeForTemplate(char *szPathName, AString *pMD5)
{
//	return true;
	
	if (pMD5)
		pMD5->Empty();

	AFile file;
	PATCH::MD5Hash md;
	
	if (!file.Open(szPathName, AFILE_OPENEXIST | AFILE_TEXT))
		return true;
	
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwRead = 0;
	DWORD	dwVersion = 0;
	int		nItemNum = 0;

#define CHECK_MD5_READLINE if (!file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead)) return false

	CHECK_MD5_READLINE;
	//Skip previous version
	sscanf(szLine, "Version: %u", &dwVersion);
	
	md.Update(szLine,dwRead);

	CHECK_MD5_READLINE;
	md.Update(szLine,dwRead);

	CHECK_MD5_READLINE;
	md.Update(szLine,dwRead);

	CHECK_MD5_READLINE;
	md.Update(szLine,dwRead);
	
	CHECK_MD5_READLINE;
	md.Update(szLine,dwRead);
	
	sscanf(szLine, "ItemNum: %d", &nItemNum);
	
	for (int i = 0; i < nItemNum; i++)
	{
		CHECK_MD5_READLINE;
		md.Update(szLine,dwRead);
		CHECK_MD5_READLINE;
		md.Update(szLine,dwRead);
		
		//----------------------------------------
		CHECK_MD5_READLINE;
		md.Update(szLine,dwRead);
		int iType;
		sscanf(szLine,"Type: %d", &iType);
		switch (iType)
		{
		case AVariant::AVT_INVALIDTYPE:
			break;
		case AVariant::AVT_BOOL:
			CHECK_MD5_READLINE;
			md.Update(szLine,dwRead);
			break;
		case AVariant::AVT_INT:
		case AVariant::AVT_LONG:
			CHECK_MD5_READLINE;
			md.Update(szLine,dwRead);
			break;
		case AVariant::AVT_UINT:
		case AVariant::AVT_ULONG:
			CHECK_MD5_READLINE;
			md.Update(szLine,dwRead);
			break;
		case AVariant::AVT_FLOAT:
		case AVariant::AVT_DOUBLE:
			CHECK_MD5_READLINE;
			md.Update(szLine,dwRead);
			break;
		case AVariant::AVT_STRING:
			CHECK_MD5_READLINE;
			md.Update(szLine,dwRead);
			break;
		case AVariant::AVT_A3DVECTOR3: 
			CHECK_MD5_READLINE;
			md.Update(szLine,dwRead);
			break;
		default:
			ASSERT(FALSE);
		}
	}
	//Md5 finished
	md.Final();

	//	Get md5 code
	char szBuffer[64];
	unsigned int nLenght = 64; 
	md.GetString(szBuffer,nLenght);
	if (pMD5)
		*pMD5 = szBuffer;

	//Campare md5 code 
	while(file.GetPos() < file.GetFileLength())
	{
		CHECK_MD5_READLINE;
		if(dwRead==0) break;
		char szCode[64];
		sscanf(szLine,"Md: %s",szCode);
		if(strcmp(szCode,szBuffer)==0) 
		{
			file.Close();
			return true;
		}
	}
	file.Close();
	return false;
}

bool ExportBaseData(const BaseDataExportConfig *sourcePaths)
{
	const char *szPath = sourcePaths->exportFullPath;

	elementdataman DataMan;
	abase::vector<AString> PathArray;

	AString strDigestLine;
	bool bFileDigestOpen(false);
	AFile fileDigest;
	AString strMD5;

#ifdef _MD5_CHECK
	time_t t1 = time(NULL);
	struct tm t2 = *localtime(&t1);
	AString strDigestPath;
	strDigestPath.Format("%s-%d年%02d月%02d日%02d时%02d分%02d秒.txt", szPath, t2.tm_year+1900, t2.tm_mon+1, t2.tm_mday, t2.tm_hour, t2.tm_min, t2.tm_sec);
	if (fileDigest.Open(strDigestPath, AFILE_TEXT | AFILE_CREATENEW))
	{
		fileDigest.WriteLine("");
		bFileDigestOpen = true;
	}
#endif

	if (bFileDigestOpen)
	{
		fileDigest.WriteLine("--------------------------------------------------------");		
		fileDigest.WriteLine("essence");
		fileDigest.WriteLine("--------------------------------------------------------");
	}

	size_t i(0);

	PathArray = sourcePaths->pathArray[BDET_BASE];
	for (i = 0; i < PathArray.size(); i++, CDlgProgressNotify::m_nCurProgress ++)
	{
		BaseDataTempl tmpl;
		
		//Checking template by md5 code
#ifdef _MD5_CHECK
		if(!CheckMd5CodeForTemplate(PathArray[i].GetBuffer(0), &strMD5))
		{
			CString str;
			str.Format("模板MD5校验错误：%s", PathArray[i]);
			AfxMessageBox(str);
			PathArray[i].ReleaseBuffer();
			PathArray.clear();
			if (bFileDigestOpen) fileDigest.Close();
			return false;
		}
#endif
		if (tmpl.Load(PathArray[i]) != 0)
		{
			/*
			CString str;
			str.Format("不能加载 %s", PathArray[i]);
			AfxMessageBox(str);
			*/
			continue;
		}

		if (tmpl.GetID() != g_BaseIDMan.GetIDByPath(PathArray[i]))
		{
			CString str;
			str.Format("ID对应错误 %s", PathArray[i]);
			AfxMessageBox(str);
		}

		if (PathArray[i].Find("BaseData\\装备\\武器\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(WEAPON_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(WEAPON_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(WEAPON_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\饰品\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(DECORATION_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(DECORATION_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(DECORATION_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\护具\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(ARMOR_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(ARMOR_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(ARMOR_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\套装\\") != -1)
		{
			EXPORT_DATA(SUITE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\卡套\\") != -1)
		{
			EXPORT_DATA(POKER_SUITE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\（时装）套装\\") != -1)
		{
			EXPORT_DATA(FASHION_SUITE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\时装\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(FASHION_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(FASHION_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(FASHION_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\卡牌\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(POKER_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(POKER_ESSENCE)
		}
	 	else if (PathArray[i].Find("BaseData\\装备\\弹药类型\\") != -1)
		{
			EXPORT_DATA(PROJECTILE_TYPE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\弹药\\") != -1)
		{
			EXPORT_DATA(PROJECTILE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\装备\\技能装备\\") != -1)
		{
			EXPORT_DATA(DYNSKILLEQUIP_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\正在被销毁的物品\\") != -1)
		{
			EXPORT_DATA(DESTROYING_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\双倍经验道具\\") != -1)
		{
			EXPORT_DATA(DOUBLE_EXP_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\染色道具\\") != -1)
		{
			EXPORT_DATA(DYE_TICKET_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\精炼概率调整道具\\") != -1)
		{
			EXPORT_DATA(REFINE_TICKET_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\药品\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(MEDICINE_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(MEDICINE_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(MEDICINE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\生产原料\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(MATERIAL_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(MATERIAL_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(MATERIAL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\防御优化符\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(ARMORRUNE_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(ARMORRUNE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\飞剑\\") != -1)
		{
			EXPORT_DATA(FLYSWORD_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\羽人翅膀\\") != -1)
		{
			EXPORT_DATA(WINGMANWING_ESSENCE)
		}		
		else if (PathArray[i].Find("BaseData\\物品\\天书\\") != -1)
		{
			EXPORT_DATA(BIBLE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\小喇叭\\") != -1)
		{
			EXPORT_DATA(SPEAKER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\护身符\\") != -1)
		{
			EXPORT_DATA(AUTOHP_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\守神符\\") != -1)
		{
			EXPORT_DATA(AUTOMP_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\势力令牌\\") != -1)
		{
			EXPORT_DATA(FORCE_TOKEN_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\攻击优化符\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(DAMAGERUNE_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(DAMAGERUNE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\技能书\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(SKILLTOME_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(SKILLTOME_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\商城代币\\") != -1)
		{
			EXPORT_DATA(SHOP_TOKEN_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\通用客户端功能物品\\") != -1)
		{
			EXPORT_DATA(UNIVERSAL_TOKEN_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\帮派回城卷轴\\") != -1)
		{
			EXPORT_DATA(UNIONSCROLL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\复活卷轴\\") != -1)
		{
			EXPORT_DATA(REVIVESCROLL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\回城卷轴\\") != -1)
		{
			EXPORT_DATA(TOWNSCROLL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\传送符\\") != -1)
		{
			EXPORT_DATA(TRANSMITSCROLL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\任务物品\\") != -1)
		{
			if (tmpl.GetLevel() == 3)
				EXPORT_DATA(TASKMATTER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\用于任务的普通物品\\") != -1)
		{
			if (tmpl.GetLevel() == 2)
				EXPORT_DATA(TASKNORMALMATTER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\任务随机发生器\\") != -1)
		{
			EXPORT_DATA(TASKDICE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\卡牌随机发生器\\") != -1)
		{
			EXPORT_DATA(POKER_DICE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\元石\\") != -1)
		{
			EXPORT_DATA(ELEMENT_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\金币兑换券\\") != -1)
		{
			EXPORT_DATA(MONEY_CONVERTIBLE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\暗器\\") != -1)
		{
			EXPORT_DATA(TOSSMATTER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\箭囊\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(QUIVER_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(QUIVER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\宝石\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(STONE_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(STONE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\整容卷\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(FACETICKET_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(FACETICKET_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(FACETICKET_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\变形丸\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(FACEPILL_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(FACEPILL_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(FACEPILL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\GM物品生成器\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(GM_GENERATOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(GM_GENERATOR_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\宠物蛋\\") != -1)
		{
			EXPORT_DATA(PET_EGG_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\宠物食品\\") != -1)
		{
			EXPORT_DATA(PET_FOOD_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\宠物美容卷\\") != -1)
		{
			EXPORT_DATA(PET_FACETICKET_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\烟花\\") != -1)
		{
			EXPORT_DATA(FIREWORKS_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\投石车召唤券\\") != -1)
		{
			EXPORT_DATA(WAR_TANKCALLIN_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\技能物品\\") != -1)
		{
			EXPORT_DATA(SKILLMATTER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\生产技能速成丹\\") != -1)
		{
			EXPORT_DATA(INC_SKILL_ABILITY_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\婚礼预约凭证\\") != -1)
		{
			EXPORT_DATA(WEDDING_BOOKCARD_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\婚礼请柬\\") != -1)
		{
			EXPORT_DATA(WEDDING_INVITECARD_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\磨刀石\\") != -1)
		{
			EXPORT_DATA(SHARPENER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\集结令\\") != -1)
		{
			EXPORT_DATA(CONGREGATE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\怪物元魂\\") != -1)
		{
			if (tmpl.GetLevel() == 2)
				EXPORT_DATA(MONSTER_SPIRIT_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\基地材料\\") != -1)
		{
			EXPORT_DATA(FACTION_MATERIAL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\基地设施\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(FACTION_BUILDING_SUB_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(FACTION_BUILDING_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\怪物\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(MONSTER_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(MONSTER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\宠物\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(PET_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(PET_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\小精灵\\小精灵本体\\") != -1)
		{
			EXPORT_DATA(GOBLIN_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\小精灵\\小精灵装备\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(GOBLIN_EQUIP_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(GOBLIN_EQUIP_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\小精灵经验丸\\") != -1)
		{
			EXPORT_DATA(GOBLIN_EXPPILL_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\售卖凭证\\") != -1)
		{
			EXPORT_DATA(SELL_CERTIFICATE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\对目标使用物品\\") != -1)
		{
			EXPORT_DATA(TARGET_ITEM_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\物品\\查看目标属性道具\\") != -1)
		{
			EXPORT_DATA(LOOK_INFO_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\NPC类型\\") != -1)
		{
			EXPORT_DATA(NPC_TYPE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\拆除\\") != -1)
		{
			EXPORT_DATA(NPC_UNINSTALL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\出售商品\\") != -1)
		{
			EXPORT_DATA(NPC_SELL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\传送\\") != -1)
		{
			EXPORT_DATA(NPC_TRANSMIT_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\存储\\") != -1)
		{
			EXPORT_DATA(NPC_STORAGE_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\代售\\") != -1)
		{
			EXPORT_DATA(NPC_PROXY_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\分解\\") != -1)
		{
			EXPORT_DATA(NPC_DECOMPOSE_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\鉴定\\") != -1)
		{
			EXPORT_DATA(NPC_IDENTIFY_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\交谈\\") != -1)
		{
			EXPORT_DATA(NPC_TALK_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\教授\\") != -1)
		{
			EXPORT_DATA(NPC_SKILL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\发放任务\\") != -1)
		{
			EXPORT_DATA(NPC_TASK_OUT_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\验证完成任务\\") != -1)
		{
			EXPORT_DATA(NPC_TASK_IN_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\发放任务物品\\") != -1)
		{
			EXPORT_DATA(NPC_TASK_MATTER_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\生产\\") != -1)
		{
			EXPORT_DATA(NPC_MAKE_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\收购商品\\") != -1)
		{
			EXPORT_DATA(NPC_BUY_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\镶嵌\\") != -1)
		{
			EXPORT_DATA(NPC_INSTALL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\修理商品\\") != -1)
		{
			EXPORT_DATA(NPC_REPAIR_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\运输\\") != -1)
		{
			EXPORT_DATA(NPC_TRANSPORT_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\治疗\\") != -1)
		{
			EXPORT_DATA(NPC_HEAL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\城战炮塔建造\\") != -1)
		{
			EXPORT_DATA(NPC_WAR_TOWERBUILD_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\洗点服务\\") != -1)
		{
			EXPORT_DATA(NPC_RESETPROP_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\宠物改名服务\\") != -1)
		{
			EXPORT_DATA(NPC_PETNAME_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\宠物学习技能服务\\") != -1)
		{
			EXPORT_DATA(NPC_PETLEARNSKILL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\宠物遗忘技能服务\\") != -1)
		{
			EXPORT_DATA(NPC_PETFORGETSKILL_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\装备绑定服务\\") != -1)
		{
			EXPORT_DATA(NPC_EQUIPBIND_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\装备销毁服务\\") != -1)
		{
			EXPORT_DATA(NPC_EQUIPDESTROY_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\装备解除销毁服务\\") != -1)
		{
			EXPORT_DATA(NPC_EQUIPUNDESTROY_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\镌刻\\") != -1)
		{
			EXPORT_DATA(NPC_ENGRAVE_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\属性值随机\\") != -1)
		{
			EXPORT_DATA(NPC_RANDPROP_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能\\势力服务\\") != -1)
		{
			EXPORT_DATA(NPC_FORCE_SERVICE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\功能NPC\\") != -1)
		{
			EXPORT_DATA(NPC_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\NPC\\矿\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(MINE_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(MINE_ESSENCE)
		}
		else
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Unknown Templ %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
		}

		tmpl.Release();
	}

	if (bFileDigestOpen)
	{
		fileDigest.WriteLine("--------------------------------------------------------");		
		fileDigest.WriteLine("addon");
		fileDigest.WriteLine("--------------------------------------------------------");
	}

	PathArray = sourcePaths->pathArray[BDET_EXT];
	for (i = 0; i < PathArray.size(); i++, CDlgProgressNotify::m_nCurProgress ++)
	{
		ExtendDataTempl tmpl;
		
		//Checking template by md5 code
#ifdef _MD5_CHECK
		if(!CheckMd5CodeForTemplate(PathArray[i].GetBuffer(0), &strMD5))
		{
			CString str;
			str.Format("模板MD5校验错误：%s", PathArray[i]);
			AfxMessageBox(str);
			PathArray[i].ReleaseBuffer();
			PathArray.clear();
			if (bFileDigestOpen) fileDigest.Close();
			return false;
		}
		PathArray[i].ReleaseBuffer();
#endif
		if (tmpl.Load(PathArray[i], false) != 0)
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Cant Load %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
			continue;
		}

		if (PathArray[i].Find("BaseData\\装备\\") != -1)
		{
			EXPORT_DATA(EQUIPMENT_ADDON)
		}
		else if (PathArray[i].Find("BaseData\\镌刻\\") != -1)
		{
			EXPORT_DATA(EQUIPMENT_ADDON)
		}
		else if (PathArray[i].Find("BaseData\\怪物\\") != -1)
		{
			EXPORT_DATA(MONSTER_ADDON)
		}
		else if (PathArray[i].Find("BaseData\\物品\\宝石") != -1)
		{
			EXPORT_DATA(EQUIPMENT_ADDON);
		}
		else if (PathArray[i].Find("BaseData\\物品\\天书") != -1)
		{
			EXPORT_DATA(EQUIPMENT_ADDON);
		}
		else if (PathArray[i].Find("BaseData\\物品\\磨刀石") != -1)
		{
			EXPORT_DATA(EQUIPMENT_ADDON);
		}
		else
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Unknown Templ %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
		}

		tmpl.Release();
	}

	PathArray = sourcePaths->pathArray[BDET_TALK];
	for (i = 0; i < PathArray.size(); i++, CDlgProgressNotify::m_nCurProgress ++)
	{
		CTalkModifyDlg talk;
		talk_proc* tp = new talk_proc;
		memset(tp, 0, sizeof(talk_proc));

		if (!talk.ExportData(tp, PathArray[i], g_TalkIDMan.GetIDByPath(PathArray[i])))
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Load Talk Data Error %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
			delete tp;
			continue;
		}

		DataMan.add_structure(tp->id_talk, tp);
	}

	if (bFileDigestOpen)
	{
		fileDigest.WriteLine("--------------------------------------------------------");		
		fileDigest.WriteLine("face");
		fileDigest.WriteLine("--------------------------------------------------------");
	}

	PathArray = sourcePaths->pathArray[BDET_FACE];
	for (i = 0; i < PathArray.size(); i++, CDlgProgressNotify::m_nCurProgress ++)
	{
		//Checking template by md5 code
#ifdef _MD5_CHECK
		if(!CheckMd5CodeForTemplate(PathArray[i].GetBuffer(0), &strMD5))
		{
			CString str;
			str.Format("模板MD5校验错误：%s", PathArray[i]);
			AfxMessageBox(str);
			PathArray[i].ReleaseBuffer();
			PathArray.clear();
			if (bFileDigestOpen) fileDigest.Close();
			return false;
		}
		PathArray[i].ReleaseBuffer();
#endif
		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Cant Load %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
			continue;
		}

		if (PathArray[i].Find("BaseData\\Face\\各部位贴图\\") != -1)
		{
			EXPORT_DATA(FACE_TEXTURE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\各部位形状\\") != -1)
		{
			EXPORT_DATA(FACE_SHAPE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\表情情绪属性\\") != -1)
		{
			EXPORT_DATA(FACE_EMOTION_TYPE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\表情\\") != -1)
		{
			EXPORT_DATA(FACE_EXPRESSION_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\头发\\") != -1)
		{
			EXPORT_DATA(FACE_HAIR_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\胡子\\") != -1)
		{
			EXPORT_DATA(FACE_MOUSTACHE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\法令\\") != -1)
		{
			EXPORT_DATA(FACE_FALING_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\附眼\\") != -1)
		{
			EXPORT_DATA(FACE_THIRDEYE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\颜色选择\\") != -1)
		{
			EXPORT_DATA(COLORPICKER_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\个性化数据\\") != -1)
		{
			EXPORT_DATA(CUSTOMIZEDATA_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\Face\\头发模型和纹理对应表\\") != -1)
		{
			EXPORT_DATA(FACE_HAIR_TEXTURE_MAP)
		}
		else
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Unknown Templ %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
		}

		tmpl.Release();
	}

	if (bFileDigestOpen)
	{
		fileDigest.WriteLine("--------------------------------------------------------");		
		fileDigest.WriteLine("recipe");
		fileDigest.WriteLine("--------------------------------------------------------");
	}

	PathArray = sourcePaths->pathArray[BDET_RECIPE];
	for (i = 0; i < PathArray.size(); i++, CDlgProgressNotify::m_nCurProgress ++)
	{
		//Checking template by md5 code
#ifdef _MD5_CHECK
		if(!CheckMd5CodeForTemplate(PathArray[i].GetBuffer(0), &strMD5))
		{
			CString str;
			str.Format("模板MD5校验错误：%s", PathArray[i]);
			AfxMessageBox(str);
			PathArray[i].ReleaseBuffer();
			PathArray.clear();
			if (bFileDigestOpen) fileDigest.Close();
			return false;
		}
		PathArray[i].ReleaseBuffer();
#endif
		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Cant Load %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
			continue;
		}

		if (PathArray[i].Find("BaseData\\配方\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(RECIPE_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(RECIPE_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(RECIPE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\镌刻\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(ENGRAVE_MAJOR_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(ENGRAVE_SUB_TYPE)
			else if (tmpl.GetLevel() == 3)
				EXPORT_DATA(ENGRAVE_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\属性值随机\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(RANDPROP_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(RANDPROP_ESSENCE)
		}
		else if (PathArray[i].Find("BaseData\\宝石转化\\") != -1)
		{
			if (tmpl.GetLevel() == 1)
				EXPORT_DATA(STONE_CHANGE_RECIPE_TYPE)
			else if (tmpl.GetLevel() == 2)
				EXPORT_DATA(STONE_CHANGE_RECIPE)
		}
		else
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Unknown Templ %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
		}

		tmpl.Release();
	}

	if (bFileDigestOpen)
	{
		fileDigest.WriteLine("--------------------------------------------------------");		
		fileDigest.WriteLine("config");
		fileDigest.WriteLine("--------------------------------------------------------");
	}

	PathArray = sourcePaths->pathArray[BDET_CONFIG];
	for (i = 0; i < PathArray.size(); i++, CDlgProgressNotify::m_nCurProgress ++)
	{
		//Checking template by md5 code
#ifdef _MD5_CHECK
		if(!CheckMd5CodeForTemplate(PathArray[i].GetBuffer(0), &strMD5))
		{
			CString str;
			str.Format("模板MD5校验错误：%s", PathArray[i]);
			AfxMessageBox(str);
			PathArray[i].ReleaseBuffer();
			PathArray.clear();
			if (bFileDigestOpen) fileDigest.Close();
			return false;
		}
		PathArray[i].ReleaseBuffer();
#endif	
		
		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Cant Load %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
			continue;
		}

		if (PathArray[i].Find("BaseData\\Config\\敌对阵营列表\\") != -1)
		{
			EXPORT_DATA(ENEMY_FACTION_CONFIG)
		}
		else if (PathArray[i].Find("BaseData\\Config\\职业属性列表\\") != -1)
		{
			EXPORT_DATA(CHARRACTER_CLASS_CONFIG)
		}
		else if (PathArray[i].Find("BaseData\\Config\\数据修正表\\") != -1)
		{
			EXPORT_DATA(PARAM_ADJUST_CONFIG)
		}
		else if (PathArray[i].Find("BaseData\\Config\\动作属性表\\") != -1)
		{
			if (tmpl.GetLevel() == 2)
				EXPORT_DATA(PLAYER_ACTION_INFO_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\玩家升级曲线表\\") != -1)
		{
			EXPORT_DATA(PLAYER_LEVELEXP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\玩家修真级别表\\") != -1)
		{
			EXPORT_DATA(PLAYER_SECONDLEVEL_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\玩家转升配置表\\") != -1)
		{
			EXPORT_DATA(PLAYER_REINCARNATION_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\玩家境界配置表\\") != -1)
		{
			EXPORT_DATA(PLAYER_REALM_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\基地配置表\\") != -1)
		{
			EXPORT_DATA(FACTION_FORTRESS_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\升级生产表\\") != -1)
		{
			EXPORT_DATA(UPGRADE_PRODUCTION_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\不可放入帐号仓库物品列表\\") != -1)
		{
			EXPORT_DATA(ACC_STORAGE_BLACKLIST_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\聚灵套餐表\\") != -1)
		{
			EXPORT_DATA(MULTI_EXP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\仙魔技能转换表\\") != -1)
		{
			EXPORT_DATA(GOD_EVIL_CONVERT_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\结婚配置表\\") != -1)
		{
			EXPORT_DATA(WEDDING_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\wiki禁忌表\\") != -1)
		{
			EXPORT_DATA(WIKI_TABOO_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\势力配置表\\") != -1)
		{
			EXPORT_DATA(FORCE_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\国战配置表\\") != -1)
		{
			EXPORT_DATA(COUNTRY_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\GM活动开关配置表\\") != -1)
		{
			EXPORT_DATA(GM_ACTIVITY_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\玩家死亡掉落表\\") != -1)
		{
			EXPORT_DATA(PLAYER_DEATH_DROP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\消费积分配置表\\") != -1)
		{
			EXPORT_DATA(CONSUME_POINTS_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\在线奖励配置表\\") != -1)
		{
			EXPORT_DATA(ONLINE_AWARDS_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\时装武器配置表\\") != -1)
		{
			EXPORT_DATA(FASHION_WEAPON_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\宠物进化配置表\\") != -1)
		{
			EXPORT_DATA(PET_EVOLVE_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\进化宠技能配置表\\") != -1)
		{
			EXPORT_DATA(PET_EVOLVED_SKILL_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\进化宠技能随机配置表\\") != -1)
		{
			EXPORT_DATA(PET_EVOLVED_SKILL_RAND_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\经脉配置表\\") != -1)
		{
			EXPORT_DATA(MERIDIAN_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\自动任务显示配置表\\") != -1)
		{
			EXPORT_DATA(AUTOTASK_DISPLAY_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\Touch商城\\") != -1)
		{
			EXPORT_DATA(TOUCH_SHOP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\包裹商城\\") != -1)
		{
			EXPORT_DATA(TOKEN_SHOP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\随机商城\\") != -1)
		{
			EXPORT_DATA(RAND_SHOP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\收益时间配置表\\") != -1)
		{
			EXPORT_DATA(PROFIT_TIME_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\称号\\简单称号\\") != -1)
		{
			if (tmpl.GetLevel() == 2)
				EXPORT_DATA(TITLE_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\称号\\复合称号\\") != -1)
		{
			if (tmpl.GetLevel() == 2)
				EXPORT_DATA(COMPLEX_TITLE_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\玩家命轮属性配置表\\") != -1)
		{
			EXPORT_DATA(PLAYER_SPIRIT_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\历史阶段\\") != -1)
		{
			EXPORT_DATA(HISTORY_STAGE_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\历史推进配置表\\") != -1)
		{
			EXPORT_DATA(HISTORY_ADVANCE_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\自动组队\\") != -1)
		{
			if (tmpl.GetLevel() == 2)
				EXPORT_DATA(AUTOTEAM_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\战车配置表\\") != -1)
		{
			EXPORT_DATA(CHARIOT_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\战车战场配置表\\") != -1)
		{
			EXPORT_DATA(CHARIOT_WAR_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\卡牌升级配置表\\") != -1)
		{
			EXPORT_DATA(POKER_LEVELEXP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\GT配置表\\") != -1)
		{
			EXPORT_DATA(GT_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\帮派掠夺战配置表\\") != -1)
		{
			EXPORT_DATA(FACTION_PVP_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\任务列表配置表\\") != -1)
		{
			EXPORT_DATA(TASK_LIST_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\权重式任务随机配置表\\") != -1)
		{
			EXPORT_DATA(TASK_DICE_BY_WEIGHT_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\时装最佳色配置表\\") != -1)
		{
			EXPORT_DATA(FASHION_BEST_COLOR_CONFIG);
		}
		else if (PathArray[i].Find("BaseData\\Config\\签到奖励配置表\\") != -1)
		{
			EXPORT_DATA(SIGN_AWARD_CONFIG);
		}
		else
		{
#ifdef _DEBUG
			char buf[256];
			sprintf(buf, "Export: Unknown Templ %s\n", PathArray[i]);
			OutputDebugString(buf);
#endif
		}

		tmpl.Release();
	}

	bool bRet = (DataMan.save_data(szPath) == 0);
	if (bFileDigestOpen)
	{
		if (bRet)
		{
			fileDigest.WriteLine("--------------------------------------------------------");
			fileDigest.WriteLine("elements.data");
			fileDigest.WriteLine("--------------------------------------------------------");

			//	写入 elements.data 的 md5
			strMD5 = CalcFileMD5(szPath);
			fileDigest.WriteLine(strMD5);

			//	写入 elements.data 生成时间
			time_t timeCurrent = time(NULL);
			AString strTime = ctime(&timeCurrent);
			strTime.TrimRight();
			fileDigest.WriteLine(strTime);

			fileDigest.Close();

			ShellExecute(NULL, "open", "notepad.exe", strDigestPath, NULL, SW_SHOW);
		}
		else
		{
			fileDigest.Close();
			::DeleteFile(strDigestPath);
		}
	}
	return bRet;
}

bool ExportPolicyData( const char* szPath)
{
	elementdataman DataMan;
	abase::vector<AString> PathArray;
	CString error_msg;
	g_PolicyIDMan.Release();
	if (g_PolicyIDMan.Load("PolicyData\\PolicyID.dat") != 0)
	{
		AfxMessageBox("ExportPolicyData(), 打开TPolicyData\\PolicyID.dat文件失败", MB_ICONSTOP);
		return false;
	}
	g_PolicyIDMan.GeneratePathArray(PathArray);
	
	FILE *pFile = fopen(szPath,"wb");
	if(pFile==NULL) return false;
	int num = 0;
	unsigned int uVersion = F_POLICY_EXP_VERSION;
	fwrite(&uVersion,sizeof(unsigned int),1,pFile);
	fwrite(&num,sizeof(int),1,pFile);
	for (size_t i = 0; i < PathArray.size(); i++)
	{
		CPolicyData dat;
		if(!dat.Load(PathArray[i].GetBuffer(0)))
		{
			error_msg.Format("警告: 不能打开 %s 策略,输出忽略该策略!",PathArray[i]);
			AfxMessageBox(error_msg);
			continue;
		}
		if(!dat.Save(pFile))
		{
			AfxMessageBox("保存数据失败!");
			goto faild;
		}
		dat.Release();
		++num;
	}
	if(0==fseek(pFile,sizeof(unsigned int),SEEK_SET))
		fwrite(&num,sizeof(int),1,pFile);
	else {
		AfxMessageBox("保存数据失败!");
		goto faild;
	}
	fclose(pFile);
	g_PolicyIDMan.Release();
	PathArray.clear();
	return true;
faild:
	fclose(pFile);
	g_PolicyIDMan.Release();
	PathArray.clear();
	return false;
}

//Use only for programer
void TempTransVersion()
{
#ifdef _MD5_CHECK
	int nCount = 0;

	abase::vector<AString> PathArray;
	size_t i;
	g_BaseIDMan.GeneratePathArray(PathArray);

	for (i = 0; i < PathArray.size(); i++)
	{
		CString csPath(PathArray[i].GetBuffer(0));
		if (HasMd5(csPath))
		{
			continue;
		}	
		int n = csPath.ReverseFind('\\');
		g_VSS.SetProjectPath(csPath.Left(n));
		g_VSS.GetFile(csPath);
		g_VSS.CheckOutFile(csPath);	

		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
			g_Log.Log("不能加载 %s", PathArray[i]);
			continue;
		}
		if(!tmpl.Save(PathArray[i]))
		{
			g_Log.Log("不能保存 %s", PathArray[i]);
			continue;
		}

		if(!WriteMd5CodeToTemplate((LPCSTR)PathArray[i]))
			g_Log.Log("WriteMd5CodeToTemplate() %s" ,PathArray[i]);		
		g_VSS.CheckInFile(csPath.Mid(n+1));
		++nCount;
	}

	PathArray.clear();
	
	g_ExtBaseIDMan.GeneratePathArray(PathArray);

	for (i = 0; i < PathArray.size(); i++)
	{
		CString csPath(PathArray[i].GetBuffer(0));
		if (HasMd5(csPath))
		{
			continue;
		}	
		int n = csPath.ReverseFind('\\');
		g_VSS.SetProjectPath(csPath.Left(n));
		g_VSS.GetFile(csPath);
		g_VSS.CheckOutFile(csPath);	

		ExtendDataTempl tmpl;
		if (tmpl.Load(PathArray[i], false) != 0)
		{
			g_Log.Log("不能加载 %s", PathArray[i]);
			continue;
		}
		if( tmpl.GetItemNum() == 0 )
		{
			ASSERT(false);
		}
		if(!tmpl.Save(PathArray[i]))
		{
			g_Log.Log("不能保存 %s", PathArray[i]);
			continue;
		}
		if(!WriteMd5CodeToTemplate((LPCSTR)PathArray[i]))
			g_Log.Log("WriteMd5CodeToTemplate() %s" ,PathArray[i]);
		g_VSS.CheckInFile(csPath.Mid(n+1));
		++nCount;
	}

	PathArray.clear();
	g_FaceIDMan.GeneratePathArray(PathArray);

	for (i = 0; i < PathArray.size(); i++)
	{
		CString csPath(PathArray[i].GetBuffer(0));
		if (HasMd5(csPath))
		{
			continue;
		}	
		int n = csPath.ReverseFind('\\');
		g_VSS.SetProjectPath(csPath.Left(n));
		g_VSS.GetFile(csPath);
		g_VSS.CheckOutFile(csPath);	

		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
			g_Log.Log("不能加载 %s", PathArray[i]);
			continue;
		}
		if(!tmpl.Save(PathArray[i]))
		{
			g_Log.Log("不能保存 %s", PathArray[i]);
			continue;
		}
		if(!WriteMd5CodeToTemplate((LPCSTR)PathArray[i]))
			g_Log.Log("WriteMd5CodeToTemplate() %s" ,PathArray[i]);
		g_VSS.CheckInFile(csPath.Mid(n+1));
		++nCount;
	}

	PathArray.clear();
	g_RecipeIDMan.GeneratePathArray(PathArray);

	for (i = 0; i < PathArray.size(); i++)
	{
		CString csPath(PathArray[i].GetBuffer(0));
		if (HasMd5(csPath))
		{
			continue;
		}	
		int n = csPath.ReverseFind('\\');
		g_VSS.SetProjectPath(csPath.Left(n));
		g_VSS.GetFile(csPath);
		g_VSS.CheckOutFile(csPath);	

		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
			g_Log.Log("不能加载 %s", PathArray[i]);
			continue;
		}
		if(!tmpl.Save(PathArray[i]))
		{
			g_Log.Log("不能保存 %s", PathArray[i]);
			continue;
		}
		if(!WriteMd5CodeToTemplate((LPCSTR)PathArray[i]))
			g_Log.Log("WriteMd5CodeToTemplate() %s" ,PathArray[i]);
		g_VSS.CheckInFile(csPath.Mid(n+1));
		++nCount;
	}

	PathArray.clear();
	g_ConfigIDMan.GeneratePathArray(PathArray);

	for (i = 0; i < PathArray.size(); i++)
	{
		CString csPath(PathArray[i].GetBuffer(0));
		if (HasMd5(csPath))
		{
			continue;
		}	
		int n = csPath.ReverseFind('\\');
		g_VSS.SetProjectPath(csPath.Left(n));
		g_VSS.GetFile(csPath);
		g_VSS.CheckOutFile(csPath);	

		BaseDataTempl tmpl;
		if (tmpl.Load(PathArray[i]) != 0)
		{
			g_Log.Log("不能加载 %s", PathArray[i]);
			continue;
		}
		if(!tmpl.Save(PathArray[i]))
		{
			g_Log.Log("不能保存 %s", PathArray[i]);
			continue;
		}
		if(!WriteMd5CodeToTemplate((LPCSTR)PathArray[i]))
			g_Log.Log("WriteMd5CodeToTemplate() %s" ,PathArray[i]);
		g_VSS.CheckInFile(csPath.Mid(n+1));
		++nCount;
	}
	CString csResult;
	csResult.Format("%d 个模板添加了Md5校验码", nCount);
	MessageBox(NULL, csResult, "提示", MB_ICONWARNING);
#endif
}

//Use only for programer
void TempExtendFullProfession()
{
	//	将之前全职业(character_combo_id == 255) 的部分数据，转换为新的全职业(character_combo_id == 1023)
	//	添加剑灵、魅灵
	//
#ifdef _MD5_CHECK
	abase::vector<AString> PathArray;
	size_t i;
	g_BaseIDMan.GeneratePathArray(PathArray);

	for (i = 0; i < PathArray.size(); i++)
	{
		//	必须是模板文件
		if (PathArray[i].Find(".tmpl") == -1)
			continue;

		//	必须是指定文件夹下的
		if (PathArray[i].Find("BaseData\\宠物\\观赏宠") != -1 ||
			PathArray[i].Find("BaseData\\宠物\\骑宠") != -1/* ||
			PathArray[i].Find("BaseData\\装备\\饰品\\") != -1 ||
			PathArray[i].Find("BaseData\\装备\\护具\\") != -1*/)
		{
			CString csPath(PathArray[i].GetBuffer(0));
			if (!HasMd5(csPath))
			{
				g_Log.Log("扩展全职业，没有md5 %s", PathArray[i]);
				continue;
			}
			int n = csPath.ReverseFind('\\');
			g_VSS.SetProjectPath(csPath.Left(n));
			g_VSS.GetFile(csPath);
			g_VSS.CheckOutFile(csPath);
			
			BaseDataTempl tmpl;
			if (tmpl.Load(PathArray[i]) != 0)
			{
				g_Log.Log("扩展全职业，不能加载 %s", PathArray[i]);
				continue;
			}

			//	检查是否有职业限制
			int k(0);
			bool bExtended(false);
			for (k = 0; k < tmpl.GetItemNum(); ++ k)
			{
				const AString &strType = tmpl.GetItemType(k);
				if (strType == "character_combo_id")
				{
					//	有职业限制，查找职业限制值
					AVariant var = tmpl.GetItemValue(k);
					if ((unsigned int)var == 1023)
					{
						var = (unsigned int)((1 << ELEMENTDATA_NUM_PROFESSION)-1);
						tmpl.SetItemValue(k, var);
						bExtended = true;
					}
				}
			}

			if (!bExtended)
			{
				g_VSS.UndoCheckOut(csPath);
				continue;
			}

			if(!tmpl.Save(PathArray[i]))
			{
				g_Log.Log("扩展全职业，不能保存 %s", PathArray[i]);
				continue;
			}
			
			if(!WriteMd5CodeToTemplate((LPCSTR)PathArray[i]))
				g_Log.Log("扩展全职业，WriteMd5CodeToTemplate() %s" ,PathArray[i]);		
			g_VSS.CheckInFile(csPath.Mid(n+1));

			g_Log.Log("扩展全职业 %s", PathArray[i]);
		}
	}

	PathArray.clear();
	#endif
}