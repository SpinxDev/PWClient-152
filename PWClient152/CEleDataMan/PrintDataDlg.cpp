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

	m_cmbType.AddString(_T("ģ������"));
	m_cmbType.AddString(_T("��������"));
	m_cmbType.SetCurSel(0);
	m_eDataType = TEMPLATE_DATA;

	m_cmbGenerateStyle.AddString(_T("����ģʽ"));
	m_cmbGenerateStyle.AddString(_T("����ģʽ"));
	m_cmbGenerateStyle.AddString(_T("����ģʽ"));
	m_cmbGenerateStyle.AddString(_T("ָ��ģʽ"));
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
	print_data("��������Ӧģ����Ľ���Զ�̱�־", (short **)&item);
	print_data("�����������ӳ�ʱ��", (short **)&item);
	print_data("��������", (int **)&item);
	print_data("��������", (int **)&item);
	print_data("��Ҫ��ҩ������", (int **)&item);
	print_data("��������С��ֵ", (int **)&item);
	print_data("����������ֵ", (int **)&item);

	print_data("ħ���������ֵ", (int **)&item);
	print_data("ħ������", (int **)&item);	

	print_data("�����ٶ�", (int **)&item);
	print_data("������Χ", (float **)&item, 1);
	print_data("������С��Χ", (float **)&item, 1);
	
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
	print_data("��ҩ����", (int **)&item);
	print_data("���������Ĺ�����", (int **)&item);
	print_data("���ձ������ӹ�����", (int **)&item);
	
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
	print_data("����������addon ����", (int **)&item);
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
	print_data("���ڷ��ߵ�addon ����", (int **)&item);
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
	print_data("��Ʒ��ģ��ID", (unsigned int **)item);
	print_data("��Ʒ������", (size_t **)item);
	print_data("��Ʒ�Ķѵ�����", (size_t **)item);
	print_data("��Ʒ�Ŀ�װ����־", (int **)item);
	print_data("��Ʒ�Ĵ���ʽ", (int **)item);
	print_data("��Ʒ��Ӧ�����ID", (int **)item);
	print_data("��Ʒ��Ӧ�����ID GUID1", (int **)item);
	print_data("��Ʒ��Ӧ�����ID GUID2", (int **)item);
	print_data("��Ʒ�۸�", (int **)item);
	
	print_data("content_length", (int **)item);
	print_data("item_contentָ��", (int **)item);
	
}

void CPrintDataDlg::PrintEquipDataHeader(const void ** item)
{
	print_data("��Ʒ��ģ��ID", (unsigned int **)item);
	print_data("��Ʒ������", (size_t **)item);
	print_data("��Ʒ�Ķѵ�����", (size_t **)item);
	print_data("��Ʒ�Ŀ�װ����־", (int **)item);
	print_data("��Ʒ�Ĵ���ʽ", (int **)item);
	print_data("��Ʒ��Ӧ�����ID", (int **)item);
	print_data("��Ʒ��Ӧ�����ID GUID1", (int **)item);
	print_data("��Ʒ��Ӧ�����ID GUID2", (int **)item);
	print_data("��Ʒ�۸�", (int **)item);
	
	print_data("content_length", (int **)item);
	print_data("item_contentָ��", (int **)item);
	
	print_data("prerequisition level", (int **)item);
	print_data("prerequisition strength", (int **)item);
	print_data("prerequisition agility", (int **)item);
	print_data("prerequisition durability", (int **)item);
	print_data("prerequisition max_durability", (int **)item);

	print_data("װ�������С���ֽڣ�", (int **)item);
}



void CPrintDataDlg::PrintItemHoleAndAddon(const void ** item)
{
	size_t i=0;
	// hole
	size_t numhole = **(int**)item;
	print_data("�׶�����Ŀ", (int **)item);	
	for(i=0; i<numhole; i++)
	{
		print_data("�׶���Ƕ���������", (int **)item);
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

	__print_data(char, file_hitgfx, 128)			// ����Ч������
	__print_data(char, file_hitsfx, 128)			// ��������Ч������
	
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
	// ������Ϣ
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(unsigned int, id_major_type, 1)				// ���������ID
	__print_data(unsigned int, id_sub_type, 1)				// ����С���ID
	__print_data(namechar, name, 32)					// ��������, ���15������

	__print_data(unsigned int, require_projectile, 1)			// ��Ҫ�ĵ�ҩ����(�ӵ�ҩ������ѡȡ), ��������Ҫ�ĵ�ҩ���ͺ͵�ҩ�ϵ�����һ��ʱ����ʹ��

	// ��ʾ��Ϣ
	__print_data(char, file_model_right, 128)		// ����ģ��·����
	__print_data(char, file_model_left, 128)		// ����ģ��·����
	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����
	
	// װ�����
	__print_data(int, require_strength, 1)
	__print_data(int, require_agility, 1)			// ��������
	__print_data(int, require_level, 1)				// �ȼ�����

	// ��������
	__print_data(int, level, 1)						// ����ĵȼ�

	__print_data(int, damage_low, 1)				// ����������Сֵ
	__print_data(int, damage_high_min, 1)			// �����������ֵ��������Сֵ
	__print_data(int, damage_high_max, 1)			// �����������ֵ���������ֵ
				
	__print_data(int, magic_damage_low, 1)			// ������������Сֵ
	__print_data(int, magic_damage_high_min, 1)		// �������������ֵ��������Сֵ
	__print_data(int, magic_damage_high_max, 1)		// �������������ֵ���������ֵ
				
	__print_data(float, attack_range, 1)				// ��������
	__print_data(unsigned int, short_range_mode, 1)			// ��������Զ�̱�־, 1-����, 0-Զ��

	__print_data(int, durability_min, 1)				// �;ö���Сֵ
	__print_data(int, durability_max, 1)				// �;ö����ֵ

	// ��Ǯ
	__print_data(int, price, 1)						// ���������̵���ۼ�Ǯ

	// ��
	__print_data(float, probability_socket, 3)		// ����0���Ļ���
	
	// ��������
	__print_data(float, probability_addon_num, 4)		// �������Գ���0���Ļ���

	__print_data(addon, addons, 32)
	__print_data(addon, rands, 32)
	
	__print_data(int,durability_drop_min,1)
	__print_data(int,durability_drop_max,1)


	__print_data(int, decompose_price, 1)			// ��ּ۸�
	__print_data(int, decompose_time, 1)			// ���ʱ��
	__print_data(unsigned int, element_id, 1)		// ��ֳɵ�Ԫʯ������ID
	__print_data(int, element_num, 1)				// ��ֲ�����Ԫʯ��

	__print_data(int, pile_num_max, 1)				// �������Ķѵ�����
	__print_data(unsigned int, has_guid, 1)			// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	__print_data(unsigned int, proc_type, 1)		// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

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
	// ������Ϣ
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(unsigned int, id_major_type, 1)				// ���ߴ����ID
	__print_data(unsigned int, id_sub_type, 1)				// ����С���ID
	__print_data(namechar, name, 32)					// ��������, ���15������

	// ��ʾ��Ϣ
	// ��ģ���в��ٶ��廤�ߵ�ģ������Ϊ��Щ���߻�Ϊÿ��������һ����ͬ��ģ��,
	// ��ģ�͵����������ǹ̶���:������+װ����.ski
	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(unsigned int, equip_location, 1)				// �û���ģ�͸��ǵĲ�λ���ɲ�ͬ��λMask��ɣ����ܵĲ�λ�У�������������Ь�ĸ�����
	__print_data(int, level, 1)

	// װ�����
	__print_data(int, require_strength, 1)			// ��������
	__print_data(int, require_level, 1)				// �ȼ�����

	// ��������
	__print_data(int, defence_low, 1)				// �����������Сֵ
	__print_data(int, defence_high, 1)				// ������������ֵ
	
	__print_data(magic_defence, magic_defences, 5)		// ��5������Ϊ�ֽ�ľˮ������ϵ

	__print_data(int, mp_enhance_low, 1)				// ��MP����Сֵ
	__print_data(int, mp_enhance_high, 1)			// ��MP�����ֵ

	__print_data(int, hp_enhance_low, 1)				// ��HP����Сֵ
	__print_data(int, hp_enhance_high, 1)			// ��HP�����ֵ

	__print_data(int, armor_enhance_low, 1)			// �Ӷ����ȵ���Сֵ
	__print_data(int, armor_enhance_high, 1)			// �Ӷ����ȵ����ֵ

	__print_data(int, durability_min, 1)				// �;ö���Сֵ
	__print_data(int, durability_max, 1)				// �;ö����ֵ

	__print_data(int, price, 1)						// �û��ߵ��̵���ۼ�Ǯ

	__print_data(float, probability_socket, 5)
	
	// ��������
	__print_data(float, probability_addon_num, 4)
	
	// addon
	__print_data(addon, addons, 32)
	__print_data(addon, rands, 32)

	__print_data(int,durability_drop_min,1)
	__print_data(int,durability_drop_max,1)
	
	// �����Ϣ
	__print_data(int, decompose_price, 1)			// ��ּ۸�
	__print_data(int, decompose_time, 1)				// ���ʱ��
	__print_data(unsigned int, 	element_id, 1)					// ��ֳɵ�Ԫʯ������ID
	__print_data(int, element_num, 1)				// ��ֲ�����Ԫʯ��

	__print_data(int, pile_num_max, 1)				// �û��ߵĶѵ�����
	__print_data(unsigned int, has_guid, 1)			// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	__print_data(unsigned int, proc_type, 1)		// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

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
	// ������Ϣ
	__print_data(unsigned int, id, 1)							// ��Ʒ(����)ID
	__print_data(unsigned int, id_major_type, 1)				// ��Ʒ�����ID
	__print_data(unsigned int, id_sub_type, 1)				// ��ƷС���ID
	__print_data(namechar, name, 32)					// ��Ʒ����, ���15������

	// ��ʾ��Ϣ
	__print_data(char, file_model, 128)			// ģ��·����
	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int,level,1)

	// װ�����
	__print_data(int, require_level, 1)				// �ȼ�����

	// ��������
	__print_data(int, damage_low, 1)					// ����������Сֵ
	__print_data(int, damage_high, 1)				// �����������ֵ
				
	__print_data(int, magic_damage_low, 1)			// ������������Сֵ
	__print_data(int, magic_damage_high, 1)			// �������������ֵ

	__print_data(int, defence_low, 1)				// �����������Сֵ
	__print_data(int, defence_high, 1)				// ������������ֵ
				
	__print_data(magic_defence, magic_defences, 5)						// ��5������Ϊ�ֽ�ľˮ������ϵ

	__print_data(int, armor_enhance_low, 1)		//�����ȵ���Сֵ
	__print_data(int, armor_enhance_high, 1)	//�Ӷ����ȵ����ֵ

	__print_data(int, durability_min, 1)				// �;ö���Сֵ
	__print_data(int, durability_max, 1)				// �;ö����ֵ

	// ��Ǯ
	__print_data(int, price, 1)						// ����Ʒ���̵���ۼ�Ǯ

	// ��������
	__print_data(float,	probability_addon_num, 4)		// �������Գ���0���Ļ���

	__print_data(addon, addons, 32)
	__print_data(addon, rands, 32)

	__print_data(int,durability_drop_min,1)
	__print_data(int,durability_drop_max,1)

	// �����Ϣ
	__print_data(int, decompose_price, 1)			// ��ּ۸�
	__print_data(int, decompose_time, 1)				// ���ʱ��
	__print_data(unsigned int, element_id, 1)					// ��ֳɵ�Ԫʯ������ID
	__print_data(int, element_num, 1)				// ��ֲ�����Ԫʯ��

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// ����Ʒ�Ķѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
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
	__print_data(unsigned int, id, 1)			// ҩƷ(����)ID
	__print_data(unsigned int, id_major_type, 1)				// ҩƷ�����ID
	__print_data(unsigned int, id_sub_type, 1)				// ҩƷС���ID
	__print_data(namechar, name, 32)					// ҩƷ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon ,128)				// ͼ��·����

	__print_data(int, require_level, 1)
	__print_data(int, cool_time, 1)
	__print_data(int, hp_add_total, 1)				// ��HP����������
	__print_data(int, hp_add_time, 1)				// ��HP�����������ʱ��
	__print_data(int, mp_add_total, 1)				// ��MP����������
	__print_data(int, mp_add_time, 1)				// ��MP�����������ʱ��

	__print_data(int, price, 1)						// ��ҩƷ���̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// ��ҩƷ�Ķѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)			// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)		// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
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
	__print_data(unsigned int, id, 1)							// ����ԭ��(����)ID
	__print_data(unsigned int, id_major_type, 1)				// ����ԭ�ϴ����ID
	__print_data(unsigned int, id_sub_type, 1)				// ����ԭ��С���ID
	__print_data(namechar, name, 32)					// ����ԭ������, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// ��ԭ�ϵ��̵���ۼ�Ǯ

	// �����Ϣ
	__print_data(int, decompose_price, 1)			// ��ּ۸�
	__print_data(int, decompose_time, 1)				// ���ʱ��
	__print_data(unsigned int, element_id, 1)					// ��ֳɵ�Ԫʯ������ID
	__print_data(int, element_num, 1)				// ��ֲ�����Ԫʯ��

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// ��ԭ�ϵĶѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

}

void CPrintDataDlg::Print_DAMAGERUNE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_DAMAGERUNE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id_sub_type, 1)				// �����Ż���С���ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����
	__print_data(char, file_gfx ,128)
	__print_data(char, file_sfx ,128)

	__print_data(unsigned int, damage_type, 1)				// ����ѡ����������ѡ��(0 ��ʾ����, 1 ��ʾ����)

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	__print_data(int, require_weapon_level_min, 1)	// ʹ�������ļ������䣺��С
	__print_data(int, require_weapon_level_max, 1)	// ʹ�������ļ������䣺���
	
	__print_data(float, damage_enhance, 1)				// �����չ����������ӱ������������루float��

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ	
}

void CPrintDataDlg::Print_ARMORRUNE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_ARMORRUNE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(unsigned int, id_sub_type, 1)				// �����Ż���С���ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����
	__print_data(char, file_gfx ,128)
	__print_data(char, file_sfx ,128)

	__print_data(unsigned int, damage_type, 1)				// ����ѡ����������ѡ��(0 ��ʾ����, 1 ��ʾ����)

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	__print_data(int, require_player_level_min, 1)	// ʹ����Ҫ�����Ｖ�����䣺��С
	__print_data(int, require_player_level_max, 1)	// ʹ����Ҫ�����Ｖ�����䣺���
	
	__print_data(float, damage_reduce_percent, 1)		// �Թ������������Ľ��Ͱٷֱ�
	__print_data(int, damage_reduce_time, 1)			// ���������Ĵ�������������

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
}

void CPrintDataDlg::Print_SKILLTOME_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)
	__print_data(namechar, name, 32)
}

void CPrintDataDlg::Print_SKILLTOME_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(unsigned int, id_sub_type, 1)				// ������С���ID
	__print_data(namechar, name ,32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)			// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)				// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
}

void CPrintDataDlg::Print_FLYSWORD_ESSENCE(const void * data)
{
	__print_data(unsigned int, 	id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_model, 128)			// ���ڵ��ϵ�ģ��·����	
	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	__print_data(int, require_player_level_min, 1)	// ʹ����Ҫ��������Ｖ��
	__print_data(float, speed_increase, 1)				// ���ٶȵ���������

	__print_data(float, time_max, 1)					// ���ķ���ʱ��
	__print_data(float, time_increase_per_element, 1)	// ÿװ��һ��Ԫʯ���ӵķ���ʱ��

	__print_data(int, fly_mode, 1)				// �ѵ�����
	__print_data(int, character_combo_id, 1);	
	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
}

void CPrintDataDlg::Print_WINGMANWING_ESSENCE(const void * data)
{
	__print_data(unsigned int, 	id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_model, 128);			// ģ��·����
	__print_data(char, file_matter, 128);			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128);				// ͼ��·����

	__print_data(int, price, 1);						// �̵���ۼ�Ǯ

	__print_data(int, require_player_level_min, 1);	// ʹ����Ҫ��������Ｖ��
	__print_data(float, speed_increase, 1);				// ���ٶȵ���������

	__print_data(int, mp_launch, 1);					// ��ɺķѵ�MP
	__print_data(int, mp_per_second, 1);				// ÿ��ķѵ�MP

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1);				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1);					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1);					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
	
}

void CPrintDataDlg::Print_TOWNSCROLL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)			
}

void CPrintDataDlg::Print_UNIONSCROLL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)

}

void CPrintDataDlg::Print_REVIVESCROLL_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
}

void CPrintDataDlg::Print_ELEMENT_ESSENCE(const void * data)
{
	__print_data(unsigned int,	id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
}

void CPrintDataDlg::Print_TASKMATTER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_icon, 128)				// ͼ��·����

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

}

void CPrintDataDlg::Print_TOSSMATTER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_model, 128)				// ģ��·����
	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����
	__print_data(char, file_firegfx, 128)			// ����Ч��
	__print_data(char, file_hitgfx, 128)			// ����Ч������
	__print_data(char, file_hitsfx, 128)			// ��������Ч������

	// ʹ�ù���
	__print_data(int, require_strength, 1)			// ��������
	__print_data(int, require_agility, 1)			// ��������
	__print_data(int, require_level, 1)				// �ȼ�����

	__print_data(int, damage_low, 1)					// ����������Сֵ
	__print_data(int, damage_high_min, 1)			// �����������ֵ��������Сֵ
	__print_data(int, damage_high_max, 1)			// �����������ֵ���������ֵ

	__print_data(float, attack_range, 1)				// ��������

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

}

void CPrintDataDlg::Print_PROJECTILE_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ��ҩ����ID
	__print_data(namechar, name, 32)							// ����, ���15������
}

void CPrintDataDlg::Print_PROJECTILE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(unsigned int, type, 1)						// ��ҩ���� (�ӵ�ҩ������ѡȡ)
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_model, 128)				// ģ��·����
	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����
	__print_data(char, file_firegfx, 128)			// ����Ч��
	__print_data(char, file_hitgfx, 128)			// ����Ч������
	__print_data(char, file_hitsfx, 128)			// ��������Ч������

	__print_data(int, require_weapon_level_min, 1)	// ʹ����������Χ: ��Сֵ (װ��ʱ���������,������ֻ�Ǹ��߻�������)
	__print_data(int, require_weapon_level_max, 1)	// ʹ����������Χ: ���ֵ

	__print_data(int, damage_enhance, 1)				// ����������������
	__print_data(int, damage_scale_enhance, 1)		// ��������������������
	
	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// ��������
	__print_data(unsigned int, id_addon, 4)					// �������Ե�����ID
	

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

}

void CPrintDataDlg::Print_QUIVER_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)				// ��ҩ����ID
	__print_data(namechar, name, 32)				// ����
}

void CPrintDataDlg::Print_QUIVER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(unsigned int, id_sub_type, 1)				// ����С���ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(unsigned int, id_projectile, 1)	// ʢװ�ĵ�ҩ����
	__print_data(int, num_min, 1)					// ��Ŀ��
	__print_data(int, num_max, 1)					// ��Ŀ��

}

void CPrintDataDlg::Print_STONE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)				// ��ҩ����ID
	__print_data(namechar, name, 32)				// ����

}

void CPrintDataDlg::Print_STONE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(unsigned int, id_sub_type, 1)				// ��ʯС���ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �۸�

	__print_data(unsigned int, id_addon_damage, 1)			// ������������id
	__print_data(unsigned int, id_addon_defence, 1)			// ������������id

//	__print_data(int, damage_enhance, 1)				// ������������ֵ(����)
//	__print_data(int, magic_damage_enhance, 1)		// ��������������ֵ(����)
//	__print_data(int, attack_enhance, 1)				// ����player��������(����)
//	__print_data(int, maxlife_enhance, 1)			// ����player����ֵ����(����)
//	__print_data(int, maxmp_enhance, 1)				// ����player����ֵ����(����)
//	
//	__print_data(int, defence_enhance, 1)			// �������������ֵ(����)
//	__print_data(int, magic_define_enhance, 5)	// ��ľ��ˮ��������ϵ����������ֵ(����)
//	__print_data(int, armor_enahnce, 1)				// ����player�Ķ����(����)
//	__print_data(int, maxlife_enhance2, 1)			// ����player����ֵ����(����)
//	__print_data(int, maxmp_enhance2, 1)				// ����player����ֵ����(����)
}

		
void CPrintDataDlg::Print_MONSTER_ADDON(const void * data)
{
	__print_data(unsigned int, id, 1)							// �˸������Ե�ID����Ʒ�ĸ������Բ���һ���ռ�
												// �������Ե�ID����������ƷʱҪ���14��13λ��ʾ��
												// �������ԵĲ�����������12λ����ռ��
												// ���������ڲ���IDʱ������ʹ��12��13��14����λ

	__print_data(namechar, name, 32)					// �������Ե�����

	__print_data(int, num_params, 1)					// ���������ԵĲ�������
	__print_data(int, param1, 1)						// ��1�������������Ǹ�����
	__print_data(int, param2, 1)						// ��2�������������Ǹ�����
	__print_data(int, param3, 1)						// ��3�������������Ǹ�����

}

void CPrintDataDlg::Print_MONSTER_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(addon, addons, 16)
}

void CPrintDataDlg::Print_MONSTER_ESSENCE(const void * data)
{

	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(unsigned int, id_type, 1)					// ���ID
	__print_data(namechar, name, 32)					// ����, ���15������
	__print_data(namechar, prop, 16)
	__print_data(namechar, desc, 16)

	__print_data(unsigned int, faction, 1)					// ��Ӫ��ϴ���
	__print_data(unsigned int, monster_faction, 1)			// �������ϸ��ϵ����

	__print_data(char, file_model, 128)			// ģ��·����
	__print_data(char, file_weapon, 128)			// ������������

	__print_data(char, file_gfx_short, 128)		// ������Ч������
	__print_data(char, file_gfx_short_hit, 128)	// ����������Ч������
	
	__print_data(float, size, 1)						// ��С
	__print_data(float, damage_delay, 1)						// ��С
	
	__print_data(unsigned int, id_strategy, 1)				// ���﹥������ID (0, 1, 2, 3) 
												// 0 ��ʾ�����ⲫ�����
												// 1 ��ʾ�����������
												// 2 ��ʾħ�������
												// 3 ��ʾ�ⲫ��Զ�������

	__print_data(int, level, 1)						// ����ȼ�
	__print_data(int, life, 1)						// ����ֵ
	__print_data(int, defence, 1)					// ���	

	__print_data(int, magic_defences, 5)			// ��5������Ϊ�ֽ�ľˮ������ϵ

	__print_data(int, exp, 1)						// ����
	__print_data(int, skillpoint, 1)					// ���ܵ�
					
	__print_data(int, money_average, 1)				// Ǯ����׼ֵ
	__print_data(int, money_var, 1)					// Ǯ������ֵ

	__print_data(unsigned int, short_range_mode, 1)			// ��������Զ�̱�־, 1-����, 0-Զ��

	__print_data(int, sight_range, 1)				// ��Ұ

	__print_data(int, attack, 1)						// ������
	__print_data(int, armor, 1)						// �ر���

	__print_data(int, damage_min, 1)					// ������: ��С������
	__print_data(int, damage_max, 1)					// ������: ��󹥻���

	__print_data(magic_damages_ext, magic_damages_exts, 5)

	__print_data(float, attack_range, 1)				// ������: ��������
	__print_data(float, attack_speed, 1)				// ������: �������

	__print_data(int, magic_damage_min, 1)			// ��������: ��С������
	__print_data(int, magic_damage_max, 1)			// ��������: ��󹥻���
	__print_data(unsigned int, id_skill, 1)					// ���ܺ�
	__print_data(int, skill_level, 1)					// ���ܺ�

	__print_data(int, hp_regenerate, 1)				// ��Ѫ�ٶȣ�/�룩������

	__print_data(unsigned int, aggressive_mode, 1)			// ������ѡ��0 - ������1 - ����

	__print_data(unsigned int, monster_faction_ask_help, 1)	// ����Щ��Ӫ�������ӹ�����ϸ��ϵ��ѡ��
	__print_data(unsigned int, monster_faction_can_help, 1)	// ������Щ��Ӫ�������ӹ�����ϸ��ϵ��ѡ��

	__print_data(float,	aggro_range, 1)				// ��޾���
	__print_data(float, aggro_time, 1)					// ���ʱ��

	__print_data(unsigned int, inhabit_type, 1)				// ��Ϣ����𣬿��ܵ��У����桢ˮ�¡����С������ˮ�¡�����ӿ��С�ˮ�¼ӿ��С���½��
	__print_data(unsigned int, 	patroll_mode, 1)			// Ѳ�߷�ʽ�����ܵ��У�����߶���...

	__print_data(float, walk_speed, 1)					// �����ٶ�
	__print_data(float, run_speed, 1)					// �����ٶ�
	__print_data(float, fly_speed, 1)					// �����ٶ�
	__print_data(float, swim_speed, 1)					// ��Ӿ�ٶ�

	__print_data(aggro_strategy, aggro_strategy, 4)

	// ������������
	__print_data(skill_hp75, skill_hp75, 5)
	__print_data(skill_hp50, skill_hp50, 5)
	__print_data(skill_hp25, skill_hp25, 5)

	__print_data(unsigned int, after_death, 1)			// ������������-0���Ա�-0x1������-0x2
	
	__print_data(skill, skills, 32)							
	
	__print_data(float, probability_drop_num, 4) 		// �������Ʒ���ֵĻ���

	__print_data(drop_matter, drop_matters, 32)
}

void CPrintDataDlg::Print_NPC_TALK_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_dialog, 1)					// �Ի�ID
}

void CPrintDataDlg::Print_NPC_SELL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	for(int i=0; i<8; i++)
	{
		__print_data(namechar, page_title, 8);					// ҳ�����֣����7������
		__print_data(unsigned int, id_goods, 32);
	}

	__print_data(unsigned int, id_dialog, 1)				// �Ի�ID
}

void CPrintDataDlg::Print_NPC_BUY_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_dialog, 1)					// �Ի�ID
}

void CPrintDataDlg::Print_NPC_REPAIR_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_dialog, 1)					// �Ի�ID
}

void CPrintDataDlg::Print_NPC_INSTALL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)					// �Ի�ID
}

void CPrintDataDlg::Print_NPC_UNINSTALL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)							// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)					// �Ի�ID
}

void CPrintDataDlg::Print_NPC_TASK_OUT_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_tasks, 32)
}

void CPrintDataDlg::Print_NPC_TASK_IN_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_tasks, 32)
}

void CPrintDataDlg::Print_NPC_TASK_MATTER_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(task, tasks, 16)

}

void CPrintDataDlg::Print_NPC_SKILL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_skills, 32)

	__print_data(unsigned int, id_dialog, 1)			// �Ի�ID

}

void CPrintDataDlg::Print_NPC_HEAL_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_dialog, 1)			// �Ի�ID

}

void CPrintDataDlg::Print_NPC_TRANSMIT_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)				// ����(����)ID
	__print_data(namechar, name, 32)				// ����, ���15������	

	__print_data(int, num_targets, 1)				// Ŀ�����Ŀ
	__print_data(target, targets, 32)
	__print_data(unsigned int, id_dialog, 1)		// �Ի�ID

}

void CPrintDataDlg::Print_NPC_TRANSPORT_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(route, routes, 32)
	__print_data(unsigned int, id_dialog, 1)			// �Ի�ID

}

void CPrintDataDlg::Print_NPC_PROXY_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_dialog, 1)			// �Ի�ID

}

void CPrintDataDlg::Print_NPC_STORAGE_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	
}

void CPrintDataDlg::Print_NPC_MAKE_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)			// �Ի�ID

}

void CPrintDataDlg::Print_NPC_DECOMPOSE_SERVICE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	

	__print_data(unsigned int, id_goods, 32)
	__print_data(unsigned int, id_dialog, 1)			// �Ի�ID
}

void CPrintDataDlg::Print_NPC_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// ����(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������	
}

void CPrintDataDlg::Print_NPC_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)					// NPC(����)ID
	__print_data(namechar, name, 32)					// ����, ���15������
	__print_data(unsigned int, type, 1)					// NPC ����, ��NPC_TYPE��ѡȡ
	__print_data(float, refresh_time, 1)				// ˢ��ʱ��
	__print_data(unsigned int, attack_rule, 1)			// �Ƿ񱻹��� 0-���ɱ�������1-�ɱ����������������2-�ɱ��������������
	__print_data(char, file_model, 128)					// ģ��·����
	__print_data(float, tax_rate, 1)					// ˰��, Ĭ��ֵ0.05
	__print_data(unsigned int, id_src_monster, 1)		// �����������ԵĹ���ID������ʱ��ʾʹ�øù��������滻��NPC������
	__print_data(namechar, hello_msg, 256)				// �����ף���һ�������ı�

	// �����б�
	__print_data(unsigned int, id_talk_service, 1)			// ��̸�ķ���ID
	__print_data(unsigned int, id_sell_service, 1)			// ������Ʒ�ķ���ID
	__print_data(unsigned int, id_buy_service, 1)			// �չ�Ʒ�ķ���ID
	__print_data(unsigned int, id_repair_service, 1)		// ������Ʒ�ķ���ID
	__print_data(unsigned int, id_install_service, 1)		// ��װ��ǶƷ�ķ���ID
	__print_data(unsigned int, id_uninstall_service, 1)		// �����ǶƷ�ķ���ID
	__print_data(unsigned int, id_task_out_service, 1)		// ������صķ���ID: �����������
	__print_data(unsigned int, id_task_in_service, 1)		// ������صķ���ID: ��֤����������
	__print_data(unsigned int, id_task_matter_service, 1)	// ������صķ���ID: ����������Ʒ����
	__print_data(unsigned int, id_skill_service, 1)			// ���ڼ��ܵķ���ID
	__print_data(unsigned int, id_heal_service, 1)			// ���Ƶķ���ID
	__print_data(unsigned int, id_transmit_service, 1)		// ���͵ķ���ID
	__print_data(unsigned int, id_transport_service, 1)		// ����ķ���ID
	__print_data(unsigned int, id_proxy_service, 1)			// ���۵ķ���ID
	__print_data(unsigned int, id_storage_service, 1)		// �ֿ�ķ���ID
	__print_data(unsigned int, id_make_service, 1)			// �����ķ���ID
	__print_data(unsigned int, id_decompose_service, 1)		// �ֽ�ķ���ID
}

void CPrintDataDlg::Print_RECIPE_MAJOR_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// �䷽�����ID
	__print_data(namechar, name, 32)					// �䷽���������
}

void CPrintDataDlg::Print_RECIPE_SUB_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1)					// �䷽С���ID
	__print_data(namechar, name, 32)					// �䷽С�������
}

void CPrintDataDlg::Print_RECIPE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);					// �䷽����ID
	__print_data(unsigned int, id_major_type, 1);		// �䷽�����ID
	__print_data(unsigned int, id_sub_type, 1);			// �䷽С���ID
	__print_data(namechar, name, 32);					// �䷽��������

	__print_data(int, recipe_level, 1);
	__print_data(unsigned int, id_skill, 1);			// ��Ҫ�ļ��ܺ�
	__print_data(int, skill_level, 1);					// ��Ҫ�ļ��ܵȼ�
	__print_data(int, id_to_make,1);					// ���ɵĶ���ID	
	__print_data(int, num_to_make, 1);					// һ�������ĸ�����Ĭ��1
	__print_data(int, price, 1);						// һ�������ļ۸���������
	__print_data(float, duration, 1);					// һ��������ʱ�䣺��������float
	__print_data(int, exp, 1);
	__print_data(int, skillpoint, 1);

	__print_data(material, materials, 32);
}

void CPrintDataDlg::Print_FACE_TEXTURE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);					// ��ͼ����(����)ID
	__print_data(namechar, name, 32);					// ����, ���15������

	__print_data(char, file_base_tex, 128);				// �������ͼ�ļ�,���еĲ�λ��ͼ�����������
	__print_data(char, file_high_tex, 128);				// �ϲ��alpha����ͼ�ļ�,����Face��Nose�ⶼ���������
	__print_data(char, file_icon, 128);					// ͼ���ļ�

	__print_data(unsigned int, tex_part_id, 1);			// ��λID,��ʾ����ͼ���÷����������ĸ���λ��, ����:0-����1-��Ƥ��2-��Ӱ��3-ü��4-����5-�ǡ�6-���顢7-����
	__print_data(unsigned int, character_id, 1);		// ��������ͼ���õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int, gender_id, 1);			// ��������ͼ���õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů
	__print_data(unsigned int, visualize_id, 1);		// �������Թ���, ��mask��Ϸ�ʽ�����ܵ��У��ԸС��ǸС��ɰ������ޡ����㡢���ʡ����ġ�Ұ�ԡ�Ӳ�ʡ����ա����

}

void CPrintDataDlg::Print_FACE_SHAPE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ��״(����)ID
	__print_data(namechar, name, 32);						// ����, ���15������

	__print_data(char, file_shape, 128);					// ��״�����ļ�
	__print_data(char, file_icon, 128);						// ͼ���ļ�

	__print_data(unsigned int, shape_part_id, 1);			// ��λID,��ʾ����ͼ���÷����������ĸ���λ��, ����:0-���͡�1-���͡�2-ü�͡�3-��ͷ��4-������5-�ϴ��ߡ�6-������7-�´��ߡ�8-����
	__print_data(unsigned int, character_id, 1);			// ��������״���õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int, gender_id, 1);				// ��������״���õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů
	__print_data(unsigned int, visualize_id, 1);			// �������Թ���, ��mask��Ϸ�ʽ�����ܵ��У��ԸС��ǸС��ɰ������ޡ����㡢���ʡ����ġ�Ұ�ԡ�Ӳ�ʡ����ա����
	__print_data(unsigned int, user_data, 1);				// �û��Զ�������, Ŀǰ���ڶ�ͬ��λ�ĸ�����״���ݽ��н�һ���ķ���
}

void CPrintDataDlg::Print_FACE_EMOTION_TYPE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ����(����)ID
	__print_data(namechar, name, 32);						// ����, ���15������

	__print_data(char, file_icon, 128);						// ͼ���ļ�

}

void CPrintDataDlg::Print_FACE_EXPRESSION_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ����(����)ID
	__print_data(namechar, name, 32);						// ����, ���15������

	__print_data(char, file_expression, 128);				// ���������ļ�
	__print_data(char, file_icon, 128);						// ͼ���ļ�

	__print_data(unsigned int, character_id, 1);			// �����˱������õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int, gender_id, 1);				// �����˱������õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů
	__print_data(unsigned int, emotion_id, 1);				// �˱�������������, ��Ҫ��FACE_EMOTION_TYPE��ѡȡ

}

void CPrintDataDlg::Print_FACE_HAIR_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ͷ��(����)ID
	__print_data(namechar, name, 32);						// ����, ���15������

	__print_data(char, file_hair_skin, 128);				// ͷ������ͬͷ����ӵ�Skin�ļ�
	__print_data(char, file_hair_model, 128);				// ͷ���Ĵ����Ӳ��ֵ�ģ���ļ�
	
	__print_data(char, file_icon, 128);						// ͼ��·��

	__print_data(unsigned int,	character_id, 1);			// ������ͷ�����õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int,	gender_id, 1);				// ������ͷ�����õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů

	__print_data(unsigned int,	visualize_id, 1);			// �������Թ���, ��mask��Ϸ�ʽ�����ܵ��У��ԸС��ǸС��ɰ������ޡ����㡢���ʡ����ġ�Ұ�ԡ�Ӳ�ʡ����ա����

}

void CPrintDataDlg::Print_FACE_MOUSTACHE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ����(����)ID
	__print_data(namechar, name, 32);						// ����, ���15������

	__print_data(char, file_moustache_skin, 128);			// ���Ӻ�����ӵ�Skin�ļ�
	
	__print_data(char, file_icon, 128);						// ͼ��·��

	__print_data(unsigned int, character_id, 1);			// �����˺������õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int, gender_id, 1);				// �����˺������õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů

	__print_data(unsigned int, visualize_id, 1);			// �������Թ���, ��mask��Ϸ�ʽ�����ܵ��У��ԸС��ǸС��ɰ������ޡ����㡢���ʡ����ġ�Ұ�ԡ�Ӳ�ʡ����ա����
}

void CPrintDataDlg::Print_COLORPICKER_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ��ɫѡ��ͼ��ID
	__print_data(namechar, name, 32);						// ���ƣ����15������

	__print_data(char, file_colorpicker, 128);				// ��ɫͼ�ļ���

	__print_data(unsigned int, color_part_id, 1);			// ��λID,��ʾ����ɫѡ��ͼ�������ĸ���λ��, ����:0-����1-��Ӱ��2-üë��3-�촽��4-ͷ����5-���顢6-����
	__print_data(unsigned int, character_id, 1);			// �����˺������õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int, gender_id, 1);				// �����˺������õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů

}

void CPrintDataDlg::Print_CUSTOMIZEDATA_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1);						// ��ɫѡ��ͼ��ID
	__print_data(namechar, name, 32);						// ���ƣ����15������

	__print_data(char, file_data, 128);				// ��ɫͼ�ļ���

	__print_data(unsigned int, character_id, 1);			// �����˺������õ�ְҵ, ���ܵ���:0-��ʿ, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	__print_data(unsigned int, gender_id, 1);				// �����˺������õ��Ա���Ϣ,���ܵ���:0-��, 1-Ů

}

void CPrintDataDlg::Print_ENEMY_FACTION_CONFIG(const void * data)
{
	__print_data(unsigned int, id, 1);					// id
	__print_data(namechar, name, 32);				// ���ƣ����15������

	__print_data(unsigned int, enemy_factions, 32);
}

void CPrintDataDlg::Print_CHARRACTER_CLASS_CONFIG(const void * data)
{
	__print_data(unsigned int, id, 1);						// id
	__print_data(namechar, name, 32);				// ���ƣ����15������

	__print_data(unsigned int, character_class_id, 1);		// ְҵid, ���ܵ���:0-����, 1-��ʦ, 2-ɮ��, 3-����, 4-����, 5-����, 6-��â, 7-����
	
	__print_data(unsigned int, faction, 1);				// ��ְҵ��������Ӫ
	__print_data(unsigned int, enemy_faction, 1);			// ��ְҵ�ĵж���Ӫ

	__print_data(float, attack_speed, 1);			// ����ʱ�������룩
	__print_data(float, attack_range, 1);			// ��������
	__print_data(int, hp_gen, 1);					// hp �ָ��ٶ�
	__print_data(int, mp_gen, 1);					// mp �ָ��ٶ�

	__print_data(float, walk_speed, 1);				// �����ٶ�
	__print_data(float, run_speed, 1);				// �����ٶ�
	__print_data(float, swim_speed, 1);				// ��Ӿ�ٶ�
	__print_data(float, fly_speed, 1);				// �����ٶ�
	__print_data(int, crit_rate, 1);				// �ػ��ʣ�%��
	
	__print_data(int, vit_hp, 1);					// 1��������Ӧ��hp
	__print_data(int, eng_mp, 1);					// 1��������Ӧ��mp
	__print_data(int, agi_attack, 1);
	__print_data(int, agi_armor, 1);

	__print_data(int, lvlup_hp, 1);				// ÿ��һ����������hp
	__print_data(int, lvlup_mp, 1);				// ÿ��һ����������mp
	__print_data(float, lvlup_dmg, 1);				// ÿ��һ������������������
	__print_data(float, lvlup_magic, 1);			// ÿ��һ����������ħ��������
	__print_data(float, lvlup_defense, 1);			// ÿ��һ���������ķ�����
	__print_data(float, lvlup_magicdefence, 1);
}

void CPrintDataDlg::Print_PARAM_ADJUST_CONFIG(const void * data)
{

	__print_data(unsigned int, id, 1);						// id
	__print_data(namechar, name, 32);				// ���ƣ����15������

	__print_data(level_diff_adjust_t, level_diff_adjust, 16);

	__print_data(team_adjust_t, team_adjust, 11);

}

void CPrintDataDlg::Print_PLAYER_ACTION_INFO_CONFIG(const void * data)
{
	__print_data(unsigned int, id, 1);						// id
	__print_data(namechar, name, 32);				// �������ƣ����15�����֣���ʾ�ã�

	__print_data(char, action_name, 32);		// �߼��������ƣ����15�����֣����߼�����һһ��Ӧ
	
	__print_data(char, action_prefix, 32);		// �붯�����ƶ�Ӧ�Ķ���ǰ׺
	
	__print_data(action_weapon_suffix_t, action_weapon_suffix, 10)
	
	__print_data(unsigned int, hide_weapon, 1);			// �Ƿ���������
	
}

void CPrintDataDlg::Print_TASKDICE_ESSENCE(const void * data)
{
	__print_data(unsigned int, id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ����ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(tasklist, tasklists, 8);

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ

}

void CPrintDataDlg::Print_TASKNORMALMATTER_ESSENCE(const void * data)
{
	__print_data(unsigned int,	id, 1)							// (����)ID
	__print_data(namechar, name, 32)					// ����, ���15������

	__print_data(char, file_matter, 128)			// ���ڵ��ϵ�ģ��·����
	__print_data(char, file_icon, 128)				// ͼ��·����

	__print_data(int, price, 1)						// �̵���ۼ�Ǯ

	// �ѵ���Ϣ
	__print_data(int, pile_num_max, 1)				// �ѵ�����
	// GUID��Ϣ
	__print_data(unsigned int, has_guid, 1)					// �Ƿ�Ϊ�����ȫ��ΨһID��������Trace��0-��1-��
	// ����ʽ
	__print_data(unsigned int, proc_type, 1)					// �����¼��ַ�ʽ�����: ����ʱ�Ƿ���䣬�Ƿ�������ڵ��ϣ��Ƿ��������NPC����¼��ϸLog��Ϣ
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
