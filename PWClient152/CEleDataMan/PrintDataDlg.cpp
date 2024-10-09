   // TestGenerateItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "eledataman.h"
#include "PrintDataDlg.h"
#include "elementdataman.h"
#include "GenItemParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintDataDlg dialog


CPrintDataDlg::CPrintDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintDataDlg)
	m_nID = 0;
	m_eIDSpace = ID_SPACE_ESSENCE;
	m_eDataType = TEMPLATE_DATA;
	//}}AFX_DATA_INIT
}


void CPrintDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintDataDlg)
	DDX_Control(pDX, IDC_COMBO_GENERATE_STYLE, m_cmbGenerateStyle);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cmbType);
	DDX_Control(pDX, IDC_COMBO_IDSPACE, m_cmbIDSpace);
	DDX_Control(pDX, IDC_SPIN_ID, m_spnID);
	DDX_Text(pDX, IDC_EDIT_ID, m_nID);
	DDX_Text(pDX, IDC_EDIT_ITEM_INFO, m_strItemInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintDataDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintDataDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_IDSPACE, OnSelchangeComboIdspace)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ID, OnDeltaposSpinId)
	ON_EN_CHANGE(IDC_EDIT_ID, OnChangeEditId)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_TEST, OnButtonBatchTest)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_GENERATE_STYLE, OnSelchangeComboGenerateStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintDataDlg message handlers

BOOL CPrintDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_cmbType.AddString(_T("模板数据"));
	m_cmbType.AddString(_T("生成数据"));
	m_cmbType.SetCurSel(0);
	m_eDataType = TEMPLATE_DATA;

	m_cmbGenerateStyle.AddString(_T("掉落模式"));
	m_cmbGenerateStyle.AddString(_T("出售模式"));
	m_cmbGenerateStyle.AddString(_T("生产模式"));
	m_cmbGenerateStyle.AddString(_T("指定模式"));
	m_cmbGenerateStyle.SetCurSel(0);
	m_eGenerateStyle = RANDOM;
	CWnd* pWnd = GetDlgItem(IDC_COMBO_GENERATE_STYLE);
	pWnd->EnableWindow(m_eDataType == GENERATE_DATA);

	
	m_cmbIDSpace.AddString(_T("ID_SPACE_ESSENCE"));
	m_cmbIDSpace.AddString(_T("ID_SPACE_ADDON"));
	m_cmbIDSpace.AddString(_T("ID_SPACE_TALK"));
	m_cmbIDSpace.AddString(_T("ID_SPACE_FACE"));	
	m_cmbIDSpace.AddString(_T("ID_SPACE_RECIPE"));
	m_cmbIDSpace.AddString(_T("ID_SPACE_CONFIG"));
	m_cmbIDSpace.SetCurSel(0);
	m_eIDSpace = ID_SPACE_ESSENCE;

	m_nID = 0;
	m_spnID.SetPos(m_nID);
	m_spnID.SetBuddy(GetDlgItem(IDC_EDIT_ID));

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintDataDlg::OnSelchangeComboIdspace() 
{
	// TODO: Add your control notification handler code here
	m_eIDSpace = (ID_SPACE)m_cmbIDSpace.GetCurSel();
}

void CPrintDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	m_strItemInfo = "";

	DATA_TYPE datatype;
	size_t sz;

	if(m_eDataType == GENERATE_DATA)
	{
		item_data * item;// = NULL;
		datatype = g_ItemDataMan.get_data_type(m_nID, m_eIDSpace);
	
		if(datatype == DT_INVALID)
			m_strItemInfo = "Invalid data type!";	
		else	
		{
			if(m_eIDSpace == ID_SPACE_ESSENCE)
			{
				if(m_eGenerateStyle == RANDOM)
				{
					element_data::item_tag_t tag = {element_data::IMT_CREATE,0};
					if(g_ItemDataMan.generate_item(m_nID, &item, sz, element_data::NORMAL(0),element_data::ADDON_LIST_DROP,&tag,sizeof(tag)) == 0)
					{
						PrintItem(item, datatype);
						abase::fastfree(item, item->content_length + sizeof(*item));
					}
					else
					{
						m_strItemInfo += "\r\n\r\nCan not generate item!";
					}
				}
				else
				if(m_eGenerateStyle == FORSALE)
				{
					const void * sell_item = g_ItemDataMan.get_item_for_sell(m_nID);
					datatype = g_ItemDataMan.get_data_type(m_nID,ID_SPACE_ESSENCE);

					PrintItem(sell_item, datatype);
				}
				else 
				if(m_eGenerateStyle == PRODUCE)
				{
					element_data::item_tag_t tag = {element_data::IMT_CREATE,0};
					if(item_data * data = g_ItemDataMan.generate_item_from_player(m_nID,&tag,sizeof(tag)))
					{
						PrintItem(data, datatype);
						abase::fastfree(data, sizeof(item_data) + data->content_length);
					}
					else
					{
						m_strItemInfo += "\r\n\r\nCan not generate item!";
					}
				}
				else
				if(m_eGenerateStyle == SPEC_GEN)
				{
					static float rlist[32]= {0};
					static int ridxlist[32]= {0};
					static int addonlist[32] = {0};

					CGenItemParam dlg;
					memcpy(dlg.m_randList,rlist,sizeof(dlg.m_randList));
					memcpy(dlg.m_indexList,ridxlist,sizeof(dlg.m_indexList));
					memcpy(dlg.m_addonList,addonlist,sizeof(dlg.m_addonList));
					if(dlg.DoModal() == IDOK)
					{
						memcpy(rlist,dlg.m_randList,sizeof(rlist));
						memcpy(ridxlist,dlg.m_indexList,sizeof(ridxlist));
						memcpy(addonlist,dlg.m_addonList,sizeof(addonlist));
					}
					if(item_data * data= g_ItemDataMan.generate_equipment(m_nID,rlist,ridxlist,addonlist))
					{
						PrintItem(data, datatype);
						abase::fastfree(data, sizeof(item_data) + data->content_length);
					}
					else
					{
						m_strItemInfo += "\r\n\r\nCan not generate item!";
					}
				}

			}
			if(m_eIDSpace == ID_SPACE_ADDON)
			{
				m_strItemInfo = DataTypeName[datatype];
				m_strItemInfo += "\r\n\r\nCan not get item from ID_SPACE_ADDON!";
			}
			if(m_eIDSpace == ID_SPACE_RECIPE)
			{
//				m_strItemInfo = DataTypeName[datatype];

//				m_strItemInfo += "\r\n\r\nCan not get item from ID_SPACE_RECIPE!";
			}
			if(m_eIDSpace == ID_SPACE_FACE)
			{
				m_strItemInfo = DataTypeName[datatype];
				m_strItemInfo += "\r\n\r\nCan not get item from ID_SPACE_FACE!";
			}			
			if(m_eIDSpace == ID_SPACE_CONFIG)
			{
				m_strItemInfo = DataTypeName[datatype];
				m_strItemInfo += "\r\n\r\nCan not get item from ID_SPACE_CONFIG!";
			}
		}
	}
	if(m_eDataType == TEMPLATE_DATA)
	{
		const void * dataptr = g_ItemDataMan.get_data_ptr(m_nID, m_eIDSpace, datatype);
		if(datatype == DT_INVALID || dataptr == NULL)
			m_strItemInfo = "Invalid data type!";
		else
		{
			PrintTemplateData(dataptr, datatype);
		}
	}


	UpdateData(FALSE);	
}

void CPrintDataDlg::PrintWeapon(const void * item)
{
	size_t i=0;

	// item_data header
	PrintEquipDataHeader((const void **)&item);

	// essence
	print_data("武器类别对应模板里的进程远程标志", (short **)&item);
	print_data("武器攻击的延迟时间", (short **)&item);
	print_data("武器子类", (int **)&item);
	print_data("武器级别", (int **)&item);
	print_data("需要弹药的类型", (int **)&item);
	print_data("物理攻击最小加值", (int **)&item);
	print_data("物理攻击最大加值", (int **)&item);

	print_data("魔法攻击最低值", (int **)&item);
	print_data("魔法攻击", (int **)&item);	

	print_data("攻击速度", (int **)&item);
	print_data("攻击范围", (float **)&item, 1);
	print_data("攻击最小范围", (float **)&item, 1);
	
	//hole and add on
	PrintItemHoleAndAddon((const void **)&item);	
}

void CPrintDataDlg::PrintArmor(const void * item)
{
	size_t i=0;
	// item_data header
	PrintEquipDataHeader((const void **)&item);

	// essence
	print_data("defence", (int **)&item);
	print_data("armor_enhance", (int **)&item);
	print_data("mp_enhance", (int **)&item);
	print_data("hp_enhance", (int **)&item);
	
	for(i=0; i<5; i++)
		print_data("Magic", (int **)&item);
		
	//hole and add on
	PrintItemHoleAndAddon((const void **)&item);
}

void CPrintDataDlg::PrintProjectile(const void * item)
{
	// item_data header
	PrintEquipDataHeader((const void **)&item);

	// essence
	print_data("弹药类型", (int **)&item);
	print_data("增加武器的攻击力", (int **)&item);
	print_data("按照比例增加攻击力", (int **)&item);
	
	//hole and add on
	PrintItemHoleAndAddon((const void **)&item);
}

void CPrintDataDlg::PrintDecoration(const void * item)
{
	size_t i=0;
	// item_data header
	PrintEquipDataHeader((const void **)&item);

	// essence
	print_data("damage", (int **)&item);
	print_data("magic_damage", (int **)&item);
	print_data("defence", (int **)&item);
	print_data("armor_enhance", (int **)&item);

	for(i=0; i<5; i++)
		print_data("magic_defences", (int **)&item);

	//hole and add on
	PrintItemHoleAndAddon((const void **)&item);
}

void CPrintDataDlg::PrintStone(const void * item)
{
	PrintItemDataHeader((const void **)&item);
	size_t numaddon = **(int**)&item;
	print_data("用于武器的addon 数量", (int **)&item);
	int i;
	for(i=0; i<numaddon; i++)
	{
		int id = **(int **)&item;
		print_data("addon id", (int **)&item);
		size_t numpara = (id & 0x6000) >>13;
		for(size_t j=0; j<numpara; j++)
			print_data("addon param", (int **)&item);
	}

	numaddon = **(int**)&item;
	print_data("用于防具的addon 数量", (int **)&item);
	for(i=0; i<numaddon; i++)
	{
		int id = **(int **)&item;
		print_data("addon id", (int **)&item);
		size_t numpara = (id & 0x6000) >>13;
		for(size_t j=0; j<numpara; j++)
			print_data("addon param", (int **)&item);
	}
}

void CPrintDataDlg::PrintItemDataHeader(const void ** item)
{
	print_data("物品的模板ID", (unsigned int **)item);
	print_data("物品的数量", (size_t **)item);
	print_data("物品的堆叠上限", (size_t **)item);
	print_data("物品的可装备标志", (int **)item);
	print_data("物品的处理方式", (int **)item);
	print_data("物品对应的类别ID", (int **)item);
	print_data("物品对应的类别ID GUID1", (int **)item);
	print_data("物品对应的类别ID GUID2", (int **)item);
	print_data("物品价格", (int **)item);
	
	print_data("content_length", (int **)item);
	print_data("item_content指针", (int **)item);
	
}

void CPrintDataDlg::PrintEquipDataHeader(const void ** item)
{
	print_data("物品的模板ID", (unsigned int **)item);
	print_data("物品的数量", (size_t **)item);
	print_data("物品的堆叠上限", (size_t **)item);
	print_data("物品的可装备标志", (int **)item);
	print_data("物品的处理方式", (int **)item);
	print_data("物品对应的类别ID", (int **)item);
	print_data("物品对应的类别ID GUID1", (int **)item);
	print_data("物品对应的类别ID GUID2", (int **)item);
	print_data("物品价格", (int **)item);
	
	print_data("content_length", (int **)item);
	print_data("item_content指针", (int **)item);
	
	print_data("prerequisition level", (int **)item);
	print_data("prerequisition strength", (int **)item);
	print_data("prerequisition agility", (int **)item);
	print_data("prerequisition durability", (int **)item);
	print_data("prerequisition max_durability", (int **)item);

	print_data("装备本体大小（字节）", (int **)item);
}



void CPrintDataDlg::PrintItemHoleAndAddon(const void ** item)
{
	size_t i=0;
	// hole
	size_t numhole = **(int**)item;
	print_data("孔洞的数目", (int **)item);	
	for(i=0; i<numhole; i++)
	{
		print_data("孔洞里嵌入物的类型", (int **)item);
	}

	size_t numaddon = **(int**)item;
	print_data("addon num", (int **)item);
	for(i=0; i<numaddon; i++)
	{
		int id = **(int **)item;
		print_data("addon id", (int **)item);
		size_t numpara = (id & 0x6000) >>13;
		for(size_t j=0; j<numpara; j++)
			print_data("addon param", (int **)item);
	}
}

void CPrintDataDlg::PrintItem(const void* item, DATA_TYPE datatype)
{
	m_strItemInfo = DataTypeName[datatype];
	m_strItemInfo += "\r\n\r\n";
	if(item == NULL) 
	{
		m_strItemInfo += "not a valid item!";
		return;
	}

	switch(datatype)
	{
	case DT_WEAPON_ESSENCE:
		PrintWeapon(item);
		break;

	case DT_ARMOR_ESSENCE:
		PrintArmor(item);
		break;

	case DT_QUIVER_ESSENCE:
	case DT_PROJECTILE_ESSENCE:
		PrintProjectile(item);
		break;

	case DT_DECORATION_ESSENCE:
		PrintDecoration(item);
		break;
		
	case DT_STONE_ESSENCE:
		PrintStone(item);
		break;
	
	case DT_MEDICINE_ESSENCE:

	case DT_MATERIAL_ESSENCE:
	
	case DT_DAMAGERUNE_ESSENCE:

	case DT_ARMORRUNE_ESSENCE:

	case DT_SKILLTOME_ESSENCE:

	case DT_FLYSWORD_ESSENCE:

	case DT_TOWNSCROLL_ESSENCE:

	case DT_UNIONSCROLL_ESSENCE:
	
	case DT_REVIVESCROLL_ESSENCE:

	case DT_ELEMENT_ESSENCE:

	case DT_TASKMATTER_ESSENCE:

	case DT_TOSSMATTER_ESSENCE:
		break;
	}
}

void CPrintDataDlg::PrintTemplateData(const void * data, DATA_TYPE datatype)
{
#define __Print(type) case DT_##type: \
	Print_##type##(data); \
	break;

	m_strItemInfo = DataTypeName[datatype];
	m_strItemInfo += "\r\n\r\n";
	if(data == NULL) 
	{
		m_strItemInfo += "not a valid data!";
		return;
	}

	switch(datatype) {

	__Print(EQUIPMENT_ADDON)
	__Print(WEAPON_MAJOR_TYPE)
	__Print(WEAPON_SUB_TYPE)
	__Print(WEAPON_ESSENCE)
	__Print(ARMOR_MAJOR_TYPE)

	__Print(ARMOR_SUB_TYPE)
	__Print(ARMOR_ESSENCE)
	__Print(DECORATION_MAJOR_TYPE)
	__Print(DECORATION_SUB_TYPE)
	__Print(DECORATION_ESSENCE)

	__Print(MEDICINE_MAJOR_TYPE)
	__Print(MEDICINE_SUB_TYPE)
	__Print(MEDICINE_ESSENCE)
	__Print(MATERIAL_MAJOR_TYPE)
	__Print(MATERIAL_SUB_TYPE)

	__Print(MATERIAL_ESSENCE)
	__Print(DAMAGERUNE_SUB_TYPE)
	__Print(DAMAGERUNE_ESSENCE)
	__Print(ARMORRUNE_SUB_TYPE)
	__Print(ARMORRUNE_ESSENCE)

	__Print(SKILLTOME_SUB_TYPE)
	__Print(SKILLTOME_ESSENCE)
	__Print(FLYSWORD_ESSENCE)
	__Print(WINGMANWING_ESSENCE)
	__Print(TOWNSCROLL_ESSENCE)
	__Print(UNIONSCROLL_ESSENCE)

	__Print(REVIVESCROLL_ESSENCE)
	__Print(ELEMENT_ESSENCE)
	__Print(TASKMATTER_ESSENCE)
	__Print(TOSSMATTER_ESSENCE)
	__Print(PROJECTILE_TYPE)

	__Print(PROJECTILE_ESSENCE)
	__Print(QUIVER_SUB_TYPE)
	__Print(QUIVER_ESSENCE)
	__Print(STONE_SUB_TYPE)
	__Print(STONE_ESSENCE)
	
	__Print(MONSTER_ADDON)
	__Print(MONSTER_TYPE)
	__Print(MONSTER_ESSENCE)
	__Print(NPC_TALK_SERVICE)
	__Print(NPC_SELL_SERVICE)

	__Print(NPC_BUY_SERVICE)
	__Print(NPC_REPAIR_SERVICE)
	__Print(NPC_INSTALL_SERVICE)
	__Print(NPC_UNINSTALL_SERVICE)
	__Print(NPC_TASK_OUT_SERVICE)

	__Print(NPC_TASK_IN_SERVICE)
	__Print(NPC_TASK_MATTER_SERVICE)
	__Print(NPC_SKILL_SERVICE)
	__Print(NPC_HEAL_SERVICE)
	__Print(NPC_TRANSMIT_SERVICE)

	__Print(NPC_TRANSPORT_SERVICE)
	__Print(NPC_PROXY_SERVICE)
	__Print(NPC_STORAGE_SERVICE)
	__Print(NPC_MAKE_SERVICE)
	__Print(NPC_DECOMPOSE_SERVICE)

	__Print(NPC_TYPE)
	__Print(NPC_ESSENCE)
	
	__Print(RECIPE_MAJOR_TYPE)
	__Print(RECIPE_SUB_TYPE)
	__Print(RECIPE_ESSENCE)

	__Print(FACE_TEXTURE_ESSENCE)
	__Print(FACE_SHAPE_ESSENCE)
	__Print(FACE_EMOTION_TYPE)
	__Print(FACE_EXPRESSION_ESSENCE)
	__Print(FACE_HAIR_ESSENCE)
	__Print(FACE_MOUSTACHE_ESSENCE)
	__Print(COLORPICKER_ESSENCE)
	__Print(CUSTOMIZEDATA_ESSENCE)
	__Print(ENEMY_FACTION_CONFIG)
	__Print(CHARRACTER_CLASS_CONFIG)
	__Print(PARAM_ADJUST_CONFIG)
	__Print(PLAYER_ACTION_INFO_CONFIG)
	__Print(TASKDICE_ESSENCE)
	__Print(TASKNORMALMATTER_ESSENCE);
	
	case DT_PLAYER_LEVELEXP_CONFIG:
		break;

//	__Print(talk_proc);

	default:
		m_strItemInfo += "can not print data properly!";
		break;
	}
#undef __Print

}

#define __print_data(T, name, count) print_data(#name, (T**)&data, count);
 
void CPrintDataDlg::Print_EQUIPMENT_ADDON(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
	__print_data(int, num_params, 1)
	__print_data(int, param1, 1)
	__print_data(int, param2, 1)
	__print_data(int, param3, 1)
}

void CPrintDataDlg::Print_WEAPON_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_WEAPON_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)

	__print_data(char, file_hitgfx, 128)			// 击中效果名字
	__print_data(char, file_hitsfx, 128)			// 击中声音效果名字
	
	__print_data(float, probability_fastest, 1)
	__print_data(float, probability_fast, 1)
	__print_data(float, probability_normal, 1)
	__print_data(float, probability_slow, 1)
	__print_data(float, probability_slowest, 1)

	__print_data(float, attack_speed, 1)
	__print_data(float, attack_short_range, 1)
	__print_data(unsigned int, action_type, 1)	
}

void CPrintDataDlg::Print_WEAPON_ESSENCE(const void * data)
{
	// 基本信息
	__print_data(unsigned int, id, 1)							// 武器(类型)ID
	__print_data(unsigned int, id_major_type, 1)				// 武器大类别ID
	__print_data(unsigned int, id_sub_type, 1)				// 武器小类别ID
	__print_data(namechar, name, 32)					// 武器名称, 最多15个汉字

	__print_data(unsigned int, require_projectile, 1)			// 需要的弹药类型(从弹药类型中选取), 当武器需要的弹药类型和弹药上的类型一致时可以使用

	// 显示信息
	__print_data(char, file_model_right, 128)		// 右手模型路径名
	__print_data(char, file_model_left, 128)		// 左手模型路径名
	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名
	
	// 装配规则
	__print_data(int, require_strength, 1)
	__print_data(int, require_agility, 1)			// 敏捷限制
	__print_data(int, require_level, 1)				// 等级限制

	// 本体属性
	__print_data(int, level, 1)						// 自身的等级

	__print_data(int, damage_low, 1)				// 物理攻击力最小值
	__print_data(int, damage_high_min, 1)			// 物理攻击力最大值的区间最小值
	__print_data(int, damage_high_max, 1)			// 物理攻击力最大值的区间最大值
				
	__print_data(int, magic_damage_low, 1)			// 法术攻击力最小值
	__print_data(int, magic_damage_high_min, 1)		// 法术攻击力最大值的区间最小值
	__print_data(int, magic_damage_high_max, 1)		// 法术攻击力最大值的区间最大值
				
	__print_data(float, attack_range, 1)				// 攻击距离
	__print_data(unsigned int, short_range_mode, 1)			// 物理攻击近远程标志, 1-近程, 0-远程

	__print_data(int, durability_min, 1)				// 耐久度最小值
	__print_data(int, durability_max, 1)				// 耐久度最大值

	// 价钱
	__print_data(int, price, 1)						// 该武器的商店出售价钱

	// 眼
	__print_data(float, probability_socket, 3)		// 出现0洞的机会
	
	// 附加属性
	__print_data(float, probability_addon_num, 4)		// 附加属性出现0个的机会

	__print_data(addon, addons, 32)
	__print_data(addon, rands, 32)
	
	__print_data(int,durability_drop_min,1)
	__print_data(int,durability_drop_max,1)


	__print_data(int, decompose_price, 1)			// 拆分价格
	__print_data(int, decompose_time, 1)			// 拆分时间
	__print_data(unsigned int, element_id, 1)		// 拆分成的元石的类型ID
	__print_data(int, element_num, 1)				// 拆分产生的元石数

	__print_data(int, pile_num_max, 1)				// 该武器的堆叠上限
	__print_data(unsigned int, has_guid, 1)			// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	__print_data(unsigned int, proc_type, 1)		// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_ARMOR_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_ARMOR_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
	__print_data(unsigned int, equip_mask, 1)
}

void CPrintDataDlg::Print_ARMOR_ESSENCE(const void * data)
{
	// 基本信息
	__print_data(unsigned int, id, 1)							// 护具(类型)ID
	__print_data(unsigned int, id_major_type, 1)				// 护具大类别ID
	__print_data(unsigned int, id_sub_type, 1)				// 护具小类别ID
	__print_data(namechar, name, 32)					// 护具名称, 最多15个汉字

	// 显示信息
	// 在模板中不再定义护具的模型是因为这些护具会为每个形象都作一个不同的模型,
	// 该模型的命名规则是固定的:形象名+装备名.ski
	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(unsigned int, equip_location, 1)				// 该护具模型覆盖的部位，由不同部位Mask组成，可能的部位有：上身、护腕、下身、鞋四个部分
	__print_data(int, level, 1)

	// 装配规则
	__print_data(int, require_strength, 1)			// 力量限制
	__print_data(int, require_level, 1)				// 等级限制

	// 本体属性
	__print_data(int, defence_low, 1)				// 物理防御力最小值
	__print_data(int, defence_high, 1)				// 物理防御力最大值
	
	__print_data(magic_defence, magic_defences, 5)		// 共5个是因为分金木水火土五系

	__print_data(int, mp_enhance_low, 1)				// 加MP的最小值
	__print_data(int, mp_enhance_high, 1)			// 加MP的最大值

	__print_data(int, hp_enhance_low, 1)				// 加HP的最小值
	__print_data(int, hp_enhance_high, 1)			// 加HP的最大值

	__print_data(int, armor_enhance_low, 1)			// 加躲闪度的最小值
	__print_data(int, armor_enhance_high, 1)			// 加躲闪度的最大值

	__print_data(int, durability_min, 1)				// 耐久度最小值
	__print_data(int, durability_max, 1)				// 耐久度最大值

	__print_data(int, price, 1)						// 该护具的商店出售价钱

	__print_data(float, probability_socket, 5)
	
	// 附加属性
	__print_data(float, probability_addon_num, 4)
	
	// addon
	__print_data(addon, addons, 32)
	__print_data(addon, rands, 32)

	__print_data(int,durability_drop_min,1)
	__print_data(int,durability_drop_max,1)
	
	// 拆分信息
	__print_data(int, decompose_price, 1)			// 拆分价格
	__print_data(int, decompose_time, 1)				// 拆分时间
	__print_data(unsigned int, 	element_id, 1)					// 拆分成的元石的类型ID
	__print_data(int, element_num, 1)				// 拆分产生的元石数

	__print_data(int, pile_num_max, 1)				// 该护具的堆叠上限
	__print_data(unsigned int, has_guid, 1)			// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	__print_data(unsigned int, proc_type, 1)		// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_DECORATION_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_DECORATION_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
	__print_data(unsigned int, equip_mask, 1)
}


void CPrintDataDlg::Print_DECORATION_ESSENCE(const void * data)
{
	// 基本信息
	__print_data(unsigned int, id, 1)							// 饰品(类型)ID
	__print_data(unsigned int, id_major_type, 1)				// 饰品大类别ID
	__print_data(unsigned int, id_sub_type, 1)				// 饰品小类别ID
	__print_data(namechar, name, 32)					// 饰品名称, 最多15个汉字

	// 显示信息
	__print_data(char, file_model, 128)			// 模型路径名
	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int,level,1)

	// 装配规则
	__print_data(int, require_level, 1)				// 等级限制

	// 本体属性
	__print_data(int, damage_low, 1)					// 物理攻击力最小值
	__print_data(int, damage_high, 1)				// 物理攻击力最大值
				
	__print_data(int, magic_damage_low, 1)			// 法术攻击力最小值
	__print_data(int, magic_damage_high, 1)			// 法术攻击力最大值

	__print_data(int, defence_low, 1)				// 物理防御力最小值
	__print_data(int, defence_high, 1)				// 物理防御力最大值
				
	__print_data(magic_defence, magic_defences, 5)						// 共5个是因为分金木水火土五系

	__print_data(int, armor_enhance_low, 1)		//躲闪度的最小值
	__print_data(int, armor_enhance_high, 1)	//加躲闪度的最大值

	__print_data(int, durability_min, 1)				// 耐久度最小值
	__print_data(int, durability_max, 1)				// 耐久度最大值

	// 价钱
	__print_data(int, price, 1)						// 该饰品的商店出售价钱

	// 附加属性
	__print_data(float,	probability_addon_num, 4)		// 附加属性出现0个的机会

	__print_data(addon, addons, 32)
	__print_data(addon, rands, 32)

	__print_data(int,durability_drop_min,1)
	__print_data(int,durability_drop_max,1)

	// 拆分信息
	__print_data(int, decompose_price, 1)			// 拆分价格
	__print_data(int, decompose_time, 1)				// 拆分时间
	__print_data(unsigned int, element_id, 1)					// 拆分成的元石的类型ID
	__print_data(int, element_num, 1)				// 拆分产生的元石数

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 该饰品的堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_MEDICINE_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)	
}

void CPrintDataDlg::Print_MEDICINE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_MEDICINE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)			// 药品(类型)ID
	__print_data(unsigned int, id_major_type, 1)				// 药品大类别ID
	__print_data(unsigned int, id_sub_type, 1)				// 药品小类别ID
	__print_data(namechar, name, 32)					// 药品名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon ,128)				// 图标路径名

	__print_data(int, require_level, 1)
	__print_data(int, cool_time, 1)
	__print_data(int, hp_add_total, 1)				// 补HP：补充总量
	__print_data(int, hp_add_time, 1)				// 补HP：补充所需的时间
	__print_data(int, mp_add_total, 1)				// 补MP：补充总量
	__print_data(int, mp_add_time, 1)				// 补MP：补充所需的时间

	__print_data(int, price, 1)						// 该药品的商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 该药品的堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)			// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)		// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_MATERIAL_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_MATERIAL_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_MATERIAL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 生产原料(类型)ID
	__print_data(unsigned int, id_major_type, 1)				// 生产原料大类别ID
	__print_data(unsigned int, id_sub_type, 1)				// 生产原料小类别ID
	__print_data(namechar, name, 32)					// 生产原料名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 该原料的商店出售价钱

	// 拆分信息
	__print_data(int, decompose_price, 1)			// 拆分价格
	__print_data(int, decompose_time, 1)				// 拆分时间
	__print_data(unsigned int, element_id, 1)					// 拆分成的元石的类型ID
	__print_data(int, element_num, 1)				// 拆分产生的元石数

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 该原料的堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_DAMAGERUNE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_DAMAGERUNE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id_sub_type, 1)				// 攻击优化符小类别ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名
	__print_data(char, file_gfx ,128)
	__print_data(char, file_sfx ,128)

	__print_data(unsigned int, damage_type, 1)				// 种类选择：物理、法术选择(0 表示物理, 1 表示法术)

	__print_data(int, price, 1)						// 商店出售价钱

	__print_data(int, require_weapon_level_min, 1)	// 使用武器的级别区间：最小
	__print_data(int, require_weapon_level_max, 1)	// 使用武器的级别区间：最大
	
	__print_data(float, damage_enhance, 1)				// 对最终攻击力的增加比例：数字输入（float）

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息	
}

void CPrintDataDlg::Print_ARMORRUNE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_ARMORRUNE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(unsigned int, id_sub_type, 1)				// 防御优化符小类别ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名
	__print_data(char, file_gfx ,128)
	__print_data(char, file_sfx ,128)

	__print_data(unsigned int, damage_type, 1)				// 种类选择：物理、法术选择(0 表示物理, 1 表示法术)

	__print_data(int, price, 1)						// 商店出售价钱

	__print_data(int, require_player_level_min, 1)	// 使用需要的人物级别区间：最小
	__print_data(int, require_player_level_max, 1)	// 使用需要的人物级别区间：最大
	
	__print_data(float, damage_reduce_percent, 1)		// 对攻击方攻击力的降低百分比
	__print_data(int, damage_reduce_time, 1)			// 抵御攻击的次数：数字输入

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_SKILLTOME_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_SKILLTOME_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(unsigned int, id_sub_type, 1)				// 技能书小类别ID
	__print_data(namechar, name ,32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)			// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)				// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_FLYSWORD_ESSENCE(const void * data)
{
	__print_data(unsigned int, 	id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_model, 128)			// 掉在地上的模型路径名	
	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	__print_data(int, require_player_level_min, 1)	// 使用需要的最低人物级别
	__print_data(float, speed_increase, 1)				// 对速度的增加数字

	__print_data(float, time_max, 1)					// 最大的飞行时间
	__print_data(float, time_increase_per_element, 1)	// 每装入一个元石增加的飞行时间

	__print_data(int, fly_mode, 1)				// 堆叠上限
	__print_data(int, character_combo_id, 1);	
	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_WINGMANWING_ESSENCE(const void * data)
{
	__print_data(unsigned int, 	id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_model, 128);			// 模型路径名
	__print_data(char, file_matter, 128);			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128);				// 图标路径名

	__print_data(int, price, 1);						// 商店出售价钱

	__print_data(int, require_player_level_min, 1);	// 使用需要的最低人物级别
	__print_data(float, speed_increase, 1);				// 对速度的增加数字

	__print_data(int, mp_launch, 1);					// 起飞耗费的MP
	__print_data(int, mp_per_second, 1);				// 每秒耗费的MP

	// 堆叠信息
	__print_data(int, pile_num_max, 1);				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1);					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1);					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
	
}

void CPrintDataDlg::Print_TOWNSCROLL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)			
}

void CPrintDataDlg::Print_UNIONSCROLL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)

}

void CPrintDataDlg::Print_REVIVESCROLL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_ELEMENT_ESSENCE(const void * data)
{
	__print_data(unsigned int,	id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

void CPrintDataDlg::Print_TASKMATTER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_icon, 128)				// 图标路径名

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_TOSSMATTER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_model, 128)				// 模型路径名
	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名
	__print_data(char, file_firegfx, 128)			// 飞行效果
	__print_data(char, file_hitgfx, 128)			// 击中效果名字
	__print_data(char, file_hitsfx, 128)			// 击中声音效果名字

	// 使用规则
	__print_data(int, require_strength, 1)			// 力量限制
	__print_data(int, require_agility, 1)			// 敏捷限制
	__print_data(int, require_level, 1)				// 等级限制

	__print_data(int, damage_low, 1)					// 物理攻击力最小值
	__print_data(int, damage_high_min, 1)			// 物理攻击力最大值的区间最小值
	__print_data(int, damage_high_max, 1)			// 物理攻击力最大值的区间最大值

	__print_data(float, attack_range, 1)				// 攻击距离

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_PROJECTILE_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 弹药类型ID
	__print_data(namechar, name, 32)							// 名称, 最多15个汉字
}

void CPrintDataDlg::Print_PROJECTILE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(unsigned int, type, 1)						// 弹药类型 (从弹药类型中选取)
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_model, 128)				// 模型路径名
	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名
	__print_data(char, file_firegfx, 128)			// 飞行效果
	__print_data(char, file_hitgfx, 128)			// 击中效果名字
	__print_data(char, file_hitsfx, 128)			// 击中声音效果名字

	__print_data(int, require_weapon_level_min, 1)	// 使用武器级别范围: 最小值 (装配时并不作检查,放在这只是给策划备忘用)
	__print_data(int, require_weapon_level_max, 1)	// 使用武器级别范围: 最大值

	__print_data(int, damage_enhance, 1)				// 附加武器物理攻击力
	__print_data(int, damage_scale_enhance, 1)		// 附加武器物理攻击力比例
	
	__print_data(int, price, 1)						// 商店出售价钱

	// 附加属性
	__print_data(unsigned int, id_addon, 4)					// 附加属性的类型ID
	

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_QUIVER_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)				// 弹药类型ID
	__print_data(namechar, name, 32)				// 名称
}

void CPrintDataDlg::Print_QUIVER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(unsigned int, id_sub_type, 1)				// 箭囊小类别ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(unsigned int, id_projectile, 1)	// 盛装的弹药个体
	__print_data(int, num_min, 1)					// 数目低
	__print_data(int, num_max, 1)					// 数目高

}

void CPrintDataDlg::Print_STONE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)				// 弹药类型ID
	__print_data(namechar, name, 32)				// 名称

}

void CPrintDataDlg::Print_STONE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(unsigned int, id_sub_type, 1)				// 宝石小类别ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 价格

	__print_data(unsigned int, id_addon_damage, 1)			// 攻击属性描述id
	__print_data(unsigned int, id_addon_defence, 1)			// 防御属性描述id

//	__print_data(int, damage_enhance, 1)				// 物理攻击力增加值(武器)
//	__print_data(int, magic_damage_enhance, 1)		// 法术攻击力增加值(武器)
//	__print_data(int, attack_enhance, 1)				// 提升player的命中率(武器)
//	__print_data(int, maxlife_enhance, 1)			// 提升player生命值上限(武器)
//	__print_data(int, maxmp_enhance, 1)				// 提升player神力值上限(武器)
//	
//	__print_data(int, defence_enhance, 1)			// 物理防御力增加值(防具)
//	__print_data(int, magic_define_enhance, 5)	// 金、木、水、火、土五系防御力增加值(防具)
//	__print_data(int, armor_enahnce, 1)				// 提升player的躲避率(防具)
//	__print_data(int, maxlife_enhance2, 1)			// 提升player生命值上限(防具)
//	__print_data(int, maxmp_enhance2, 1)				// 提升player神力值上限(防具)
}

		
void CPrintDataDlg::Print_MONSTER_ADDON(const void * data)
{
	__print_data(unsigned int, id, 1)							// 此附加属性的ID与物品的附加属性不在一个空间
												// 附加属性的ID，在生成物品时要求第14、13位表示本
												// 附加属性的参数个数，第12位不能占用
												// 所以我们在产生ID时，不能使用12，13，14这三位

	__print_data(namechar, name, 32)					// 附加属性的名字

	__print_data(int, num_params, 1)					// 本附加属性的参数个数
	__print_data(int, param1, 1)						// 第1个参数，可以是浮点数
	__print_data(int, param2, 1)						// 第2个参数，可以是浮点数
	__print_data(int, param3, 1)						// 第3个参数，可以是浮点数

}

void CPrintDataDlg::Print_MONSTER_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(addon, addons, 16)
}

void CPrintDataDlg::Print_MONSTER_ESSENCE(const void * data)
{

	__print_data(unsigned int, id, 1)							// 怪物(类型)ID
	__print_data(unsigned int, id_type, 1)					// 类别ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字
	__print_data(namechar, prop, 16)
	__print_data(namechar, desc, 16)

	__print_data(unsigned int, faction, 1)					// 阵营组合代码
	__print_data(unsigned int, monster_faction, 1)			// 怪物的详细派系划分

	__print_data(char, file_model, 128)			// 模型路径名
	__print_data(char, file_weapon, 128)			// 悬挂武器名称

	__print_data(char, file_gfx_short, 128)		// 物理攻击效果名称
	__print_data(char, file_gfx_short_hit, 128)	// 物理攻击击中效果名称
	
	__print_data(float, size, 1)						// 大小
	__print_data(float, damage_delay, 1)						// 大小
	
	__print_data(unsigned int, id_strategy, 1)				// 怪物攻击策略ID (0, 1, 2, 3) 
												// 0 表示物理肉搏类怪物
												// 1 表示物理弓箭类怪物
												// 2 表示魔法类怪物
												// 3 表示肉搏＋远程类怪物

	__print_data(int, level, 1)						// 怪物等级
	__print_data(int, life, 1)						// 生命值
	__print_data(int, defence, 1)					// 物防	

	__print_data(int, magic_defences, 5)			// 共5个是因为分金木水火土五系

	__print_data(int, exp, 1)						// 经验
	__print_data(int, skillpoint, 1)					// 技能点
					
	__print_data(int, money_average, 1)				// 钱：标准值
	__print_data(int, money_var, 1)					// 钱：浮动值

	__print_data(unsigned int, short_range_mode, 1)			// 物理攻击近远程标志, 1-近程, 0-远程

	__print_data(int, sight_range, 1)				// 视野

	__print_data(int, attack, 1)						// 命中率
	__print_data(int, armor, 1)						// 回避率

	__print_data(int, damage_min, 1)					// 物理攻击: 最小攻击力
	__print_data(int, damage_max, 1)					// 物理攻击: 最大攻击力

	__print_data(magic_damages_ext, magic_damages_exts, 5)

	__print_data(float, attack_range, 1)				// 物理攻击: 攻击距离
	__print_data(float, attack_speed, 1)				// 物理攻击: 攻击间隔

	__print_data(int, magic_damage_min, 1)			// 法术攻击: 最小攻击力
	__print_data(int, magic_damage_max, 1)			// 法术攻击: 最大攻击力
	__print_data(unsigned int, id_skill, 1)					// 技能号
	__print_data(int, skill_level, 1)					// 技能号

	__print_data(int, hp_regenerate, 1)				// 回血速度（/秒）：慢速

	__print_data(unsigned int, aggressive_mode, 1)			// 主被动选择：0 - 被动；1 - 主动

	__print_data(unsigned int, monster_faction_ask_help, 1)	// 向哪些阵营求助，从怪物详细派系中选择
	__print_data(unsigned int, monster_faction_can_help, 1)	// 接受哪些阵营求助，从怪物详细派系中选择

	__print_data(float,	aggro_range, 1)				// 仇恨距离
	__print_data(float, aggro_time, 1)					// 仇恨时间

	__print_data(unsigned int, inhabit_type, 1)				// 栖息地类别，可能的有：地面、水下、空中、地面加水下、地面加空中、水下加空中、海陆空
	__print_data(unsigned int, 	patroll_mode, 1)			// 巡逻方式，可能的有：随机走动，...

	__print_data(float, walk_speed, 1)					// 行走速度
	__print_data(float, run_speed, 1)					// 奔跑速度
	__print_data(float, fly_speed, 1)					// 飞行速度
	__print_data(float, swim_speed, 1)					// 游泳速度

	__print_data(aggro_strategy, aggro_strategy, 4)

	// 三个条件技能
	__print_data(skill_hp75, skill_hp75, 5)
	__print_data(skill_hp50, skill_hp50, 5)
	__print_data(skill_hp25, skill_hp25, 5)

	__print_data(unsigned int, after_death, 1)			// 死亡后续：无-0、自爆-0x1、重生-0x2
	
	__print_data(skill, skills, 32)							
	
	__print_data(float, probability_drop_num, 4) 		// 掉落的物品出现的机会

	__print_data(drop_matter, drop_matters, 32)
}

void CPrintDataDlg::Print_NPC_TALK_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_dialog, 1)					// 对话ID
}

void CPrintDataDlg::Print_NPC_SELL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	for(int i=0; i<8; i++)
	{
		__print_data(namechar, page_title, 8);					// 页的名字，最多7个汉字
		__print_data(unsigned int, id_goods, 32);
	}

	__print_data(unsigned int, id_dialog, 1)				// 对话ID
}

void CPrintDataDlg::Print_NPC_BUY_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_dialog, 1)					// 对话ID
}

void CPrintDataDlg::Print_NPC_REPAIR_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_dialog, 1)					// 对话ID
}

void CPrintDataDlg::Print_NPC_INSTALL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)					// 对话ID
}

void CPrintDataDlg::Print_NPC_UNINSTALL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)					// 对话ID
}

void CPrintDataDlg::Print_NPC_TASK_OUT_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_tasks, 32)
}

void CPrintDataDlg::Print_NPC_TASK_IN_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_tasks, 32)
}

void CPrintDataDlg::Print_NPC_TASK_MATTER_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(task, tasks, 16)

}

void CPrintDataDlg::Print_NPC_SKILL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_skills, 32)

	__print_data(unsigned int, id_dialog, 1)			// 对话ID

}

void CPrintDataDlg::Print_NPC_HEAL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_dialog, 1)			// 对话ID

}

void CPrintDataDlg::Print_NPC_TRANSMIT_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)				// 服务(类型)ID
	__print_data(namechar, name, 32)				// 名称, 最多15个汉字	

	__print_data(int, num_targets, 1)				// 目标点数目
	__print_data(target, targets, 32)
	__print_data(unsigned int, id_dialog, 1)		// 对话ID

}

void CPrintDataDlg::Print_NPC_TRANSPORT_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(route, routes, 32)
	__print_data(unsigned int, id_dialog, 1)			// 对话ID

}

void CPrintDataDlg::Print_NPC_PROXY_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_dialog, 1)			// 对话ID

}

void CPrintDataDlg::Print_NPC_STORAGE_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	
}

void CPrintDataDlg::Print_NPC_MAKE_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)			// 对话ID

}

void CPrintDataDlg::Print_NPC_DECOMPOSE_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)			// 对话ID
}

void CPrintDataDlg::Print_NPC_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 服务(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字	
}

void CPrintDataDlg::Print_NPC_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)					// NPC(类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字
	__print_data(unsigned int, type, 1)					// NPC 类型, 从NPC_TYPE中选取
	__print_data(float, refresh_time, 1)				// 刷新时间
	__print_data(unsigned int, attack_rule, 1)			// 是否被攻击 0-不可被攻击，1-可被攻击攻击变粉名，2-可被攻击攻击变红名
	__print_data(char, file_model, 128)					// 模型路径名
	__print_data(float, tax_rate, 1)					// 税率, 默认值0.05
	__print_data(unsigned int, id_src_monster, 1)		// 基本属性来自的怪物ID，非零时表示使用该怪物属性替换本NPC的属性
	__print_data(namechar, hello_msg, 256)				// 开场白，是一个多行文本

	// 服务列表
	__print_data(unsigned int, id_talk_service, 1)			// 交谈的服务ID
	__print_data(unsigned int, id_sell_service, 1)			// 出售商品的服务ID
	__print_data(unsigned int, id_buy_service, 1)			// 收购品的服务ID
	__print_data(unsigned int, id_repair_service, 1)		// 修理商品的服务ID
	__print_data(unsigned int, id_install_service, 1)		// 安装镶嵌品的服务ID
	__print_data(unsigned int, id_uninstall_service, 1)		// 拆除镶嵌品的服务ID
	__print_data(unsigned int, id_task_out_service, 1)		// 任务相关的服务ID: 发放任务服务
	__print_data(unsigned int, id_task_in_service, 1)		// 任务相关的服务ID: 验证完成任务服务
	__print_data(unsigned int, id_task_matter_service, 1)	// 任务相关的服务ID: 发放任务物品服务
	__print_data(unsigned int, id_skill_service, 1)			// 教授技能的服务ID
	__print_data(unsigned int, id_heal_service, 1)			// 治疗的服务ID
	__print_data(unsigned int, id_transmit_service, 1)		// 传送的服务ID
	__print_data(unsigned int, id_transport_service, 1)		// 运输的服务ID
	__print_data(unsigned int, id_proxy_service, 1)			// 代售的服务ID
	__print_data(unsigned int, id_storage_service, 1)		// 仓库的服务ID
	__print_data(unsigned int, id_make_service, 1)			// 生产的服务ID
	__print_data(unsigned int, id_decompose_service, 1)		// 分解的服务ID
}

void CPrintDataDlg::Print_RECIPE_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 配方大类别ID
	__print_data(namechar, name, 32)					// 配方大类别名称
}

void CPrintDataDlg::Print_RECIPE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// 配方小类别ID
	__print_data(namechar, name, 32)					// 配方小类别名称
}

void CPrintDataDlg::Print_RECIPE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);					// 配方本体ID
	__print_data(unsigned int, id_major_type, 1);		// 配方大类别ID
	__print_data(unsigned int, id_sub_type, 1);			// 配方小类别ID
	__print_data(namechar, name, 32);					// 配方本体名称

	__print_data(int, recipe_level, 1);
	__print_data(unsigned int, id_skill, 1);			// 需要的技能号
	__print_data(int, skill_level, 1);					// 需要的技能等级
	__print_data(int, id_to_make,1);					// 生成的东西ID	
	__print_data(int, num_to_make, 1);					// 一次生产的个数：默认1
	__print_data(int, price, 1);						// 一次生产的价格：数字输入
	__print_data(float, duration, 1);					// 一次生产的时间：数字输入float
	__print_data(int, exp, 1);
	__print_data(int, skillpoint, 1);

	__print_data(material, materials, 32);
}

void CPrintDataDlg::Print_FACE_TEXTURE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);					// 贴图方案(类型)ID
	__print_data(namechar, name, 32);					// 名称, 最多15个汉字

	__print_data(char, file_base_tex, 128);				// 基层的贴图文件,所有的部位贴图都有这个属性
	__print_data(char, file_high_tex, 128);				// 上层带alpha的贴图文件,除了Face和Nose外都有这个属性
	__print_data(char, file_icon, 128);					// 图标文件

	__print_data(unsigned int, tex_part_id, 1);			// 部位ID,表示此贴图配置方案是用于哪个部位的, 包括:0-脸、1-眼皮、2-眼影、3-眉、4-唇、5-鼻、6-眼珠、7-胡子
	__print_data(unsigned int, character_id, 1);		// 表明此贴图适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int, gender_id, 1);			// 表明此贴图适用的性别信息,可能的有:0-男, 1-女
	__print_data(unsigned int, visualize_id, 1);		// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐

}

void CPrintDataDlg::Print_FACE_SHAPE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 形状(类型)ID
	__print_data(namechar, name, 32);						// 名称, 最多15个汉字

	__print_data(char, file_shape, 128);					// 形状数据文件
	__print_data(char, file_icon, 128);						// 图标文件

	__print_data(unsigned int, shape_part_id, 1);			// 部位ID,表示此贴图配置方案是用于哪个部位的, 包括:0-脸型、1-眼型、2-眉型、3-鼻头、4-鼻梁、5-上唇线、6-唇沟、7-下唇线、8-耳型
	__print_data(unsigned int, character_id, 1);			// 表明此形状适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int, gender_id, 1);				// 表明此形状适用的性别信息,可能的有:0-男, 1-女
	__print_data(unsigned int, visualize_id, 1);			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
	__print_data(unsigned int, user_data, 1);				// 用户自定义数据, 目前用于对同部位的各种形状数据进行进一步的分类
}

void CPrintDataDlg::Print_FACE_EMOTION_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 情绪(类型)ID
	__print_data(namechar, name, 32);						// 名称, 最多15个汉字

	__print_data(char, file_icon, 128);						// 图标文件

}

void CPrintDataDlg::Print_FACE_EXPRESSION_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 表情(类型)ID
	__print_data(namechar, name, 32);						// 名称, 最多15个汉字

	__print_data(char, file_expression, 128);				// 表情数据文件
	__print_data(char, file_icon, 128);						// 图标文件

	__print_data(unsigned int, character_id, 1);			// 表明此表情适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int, gender_id, 1);				// 表明此表情适用的性别信息,可能的有:0-男, 1-女
	__print_data(unsigned int, emotion_id, 1);				// 此表情所表达的情绪, 需要从FACE_EMOTION_TYPE中选取

}

void CPrintDataDlg::Print_FACE_HAIR_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 头发(类型)ID
	__print_data(namechar, name, 32);						// 名称, 最多15个汉字

	__print_data(char, file_hair_skin, 128);				// 头发用于同头顶相接的Skin文件
	__print_data(char, file_hair_model, 128);				// 头发的带辫子部分的模型文件
	
	__print_data(char, file_icon, 128);						// 图标路径

	__print_data(unsigned int,	character_id, 1);			// 表明此头发适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int,	gender_id, 1);				// 表明此头发适用的性别信息,可能的有:0-男, 1-女

	__print_data(unsigned int,	visualize_id, 1);			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐

}

void CPrintDataDlg::Print_FACE_MOUSTACHE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 胡子(类型)ID
	__print_data(namechar, name, 32);						// 名称, 最多15个汉字

	__print_data(char, file_moustache_skin, 128);			// 胡子和脸相接的Skin文件
	
	__print_data(char, file_icon, 128);						// 图标路径

	__print_data(unsigned int, character_id, 1);			// 表明此胡子适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int, gender_id, 1);				// 表明此胡子适用的性别信息,可能的有:0-男, 1-女

	__print_data(unsigned int, visualize_id, 1);			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
}

void CPrintDataDlg::Print_COLORPICKER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 颜色选择图像ID
	__print_data(namechar, name, 32);						// 名称，最多15个汉字

	__print_data(char, file_colorpicker, 128);				// 颜色图文件名

	__print_data(unsigned int, color_part_id, 1);			// 部位ID,表示此颜色选择图是用于哪个部位的, 包括:0-脸、1-眼影、2-眉毛、3-嘴唇、4-头发、5-眼珠、6-胡子
	__print_data(unsigned int, character_id, 1);			// 表明此胡子适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int, gender_id, 1);				// 表明此胡子适用的性别信息,可能的有:0-男, 1-女

}

void CPrintDataDlg::Print_CUSTOMIZEDATA_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// 颜色选择图像ID
	__print_data(namechar, name, 32);						// 名称，最多15个汉字

	__print_data(char, file_data, 128);				// 颜色图文件名

	__print_data(unsigned int, character_id, 1);			// 表明此胡子适用的职业, 可能的有:0-武士, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	__print_data(unsigned int, gender_id, 1);				// 表明此胡子适用的性别信息,可能的有:0-男, 1-女

}

void CPrintDataDlg::Print_ENEMY_FACTION_CONFIG(const void * data)
{
	__print_data(unsigned int, id, 1);					// id
	__print_data(namechar, name, 32);				// 名称，最多15个汉字

	__print_data(unsigned int, enemy_factions, 32);
}

void CPrintDataDlg::Print_CHARRACTER_CLASS_CONFIG(const void * data)
{
	__print_data(unsigned int, id, 1);						// id
	__print_data(namechar, name, 32);				// 名称，最多15个汉字

	__print_data(unsigned int, character_class_id, 1);		// 职业id, 可能的有:0-武侠, 1-法师, 2-僧侣, 3-妖精, 4-妖兽, 5-魅灵, 6-羽芒, 7-羽灵
	
	__print_data(unsigned int, faction, 1);				// 该职业的所属阵营
	__print_data(unsigned int, enemy_faction, 1);			// 该职业的敌对阵营

	__print_data(float, attack_speed, 1);			// 攻击时间间隔（秒）
	__print_data(float, attack_range, 1);			// 攻击距离
	__print_data(int, hp_gen, 1);					// hp 恢复速度
	__print_data(int, mp_gen, 1);					// mp 恢复速度

	__print_data(float, walk_speed, 1);				// 行走速度
	__print_data(float, run_speed, 1);				// 奔跑速度
	__print_data(float, swim_speed, 1);				// 游泳速度
	__print_data(float, fly_speed, 1);				// 飞行速度
	__print_data(int, crit_rate, 1);				// 重击率（%）
	
	__print_data(int, vit_hp, 1);					// 1点生命对应的hp
	__print_data(int, eng_mp, 1);					// 1点真气对应的mp
	__print_data(int, agi_attack, 1);
	__print_data(int, agi_armor, 1);

	__print_data(int, lvlup_hp, 1);				// 每升一级所增长的hp
	__print_data(int, lvlup_mp, 1);				// 每升一级所增长的mp
	__print_data(float, lvlup_dmg, 1);				// 每升一级所增长的物理攻击力
	__print_data(float, lvlup_magic, 1);			// 每升一级所增长的魔法攻击力
	__print_data(float, lvlup_defense, 1);			// 每升一级所增长的防御力
	__print_data(float, lvlup_magicdefence, 1);
}

void CPrintDataDlg::Print_PARAM_ADJUST_CONFIG(const void * data)
{

	__print_data(unsigned int, id, 1);						// id
	__print_data(namechar, name, 32);				// 名称，最多15个汉字

	__print_data(level_diff_adjust_t, level_diff_adjust, 16);

	__print_data(team_adjust_t, team_adjust, 11);

}

void CPrintDataDlg::Print_PLAYER_ACTION_INFO_CONFIG(const void * data)
{
	__print_data(unsigned int, id, 1);						// id
	__print_data(namechar, name, 32);				// 动作名称，最多15个汉字（显示用）

	__print_data(char, action_name, 32);		// 逻辑动作名称，最多15个汉字，与逻辑动作一一对应
	
	__print_data(char, action_prefix, 32);		// 与动作名称对应的动作前缀
	
	__print_data(action_weapon_suffix_t, action_weapon_suffix, 10)
	
	__print_data(unsigned int, hide_weapon, 1);			// 是否隐藏武器
	
}

void CPrintDataDlg::Print_TASKDICE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉落模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(tasklist, tasklists, 8);

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息

}

void CPrintDataDlg::Print_TASKNORMALMATTER_ESSENCE(const void * data)
{
	__print_data(unsigned int,	id, 1)							// (类型)ID
	__print_data(namechar, name, 32)					// 名称, 最多15个汉字

	__print_data(char, file_matter, 128)			// 掉在地上的模型路径名
	__print_data(char, file_icon, 128)				// 图标路径名

	__print_data(int, price, 1)						// 商店出售价钱

	// 堆叠信息
	__print_data(int, pile_num_max, 1)				// 堆叠上限
	// GUID信息
	__print_data(unsigned int, has_guid, 1)					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	__print_data(unsigned int, proc_type, 1)					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，记录详细Log信息
}

#undef __print_data

void CPrintDataDlg::OnDeltaposSpinId(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_nID -= pNMUpDown->iDelta;
	UpdateData(FALSE);
	OnOK();
	
	*pResult = 0;
}

void CPrintDataDlg::OnChangeEditId() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_spnID.SetPos(m_nID);
	UpdateData(FALSE);	
}

void CPrintDataDlg::OnButtonBatchTest() 
{
	// TODO: Add your control notification handler code here

	unsigned int i=0;
	for(i=0; i<5000; i++)
	{
		m_nID = i;
		UpdateData(FALSE);
		OnOK();
	}	
}

void CPrintDataDlg::OnSelchangeComboType() 
{
	// TODO: Add your control notification handler code here
	m_eDataType = (Type) m_cmbType.GetCurSel();
	CWnd* pWnd = GetDlgItem(IDC_COMBO_GENERATE_STYLE);

	pWnd->EnableWindow(m_eDataType == GENERATE_DATA);	
}

void CPrintDataDlg::OnSelchangeComboGenerateStyle() 
{
	// TODO: Add your control notification handler code here
	m_eGenerateStyle = (GenerateStyle) m_cmbGenerateStyle.GetCurSel();
	
}
