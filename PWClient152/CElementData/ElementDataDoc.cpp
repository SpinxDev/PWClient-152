// ElementDataDoc.cpp : implementation of the CElementDataDoc class
//

#include "stdafx.h"
#include "A3D.h"
#include "ElementData.h"
#include "EnumTypes.h"
#include "Global.h"
#include "ElementDataDoc.h"
#include "TemplIDSelDlg.h"
#include "FactionsSel.h"
#include "MonsterFaction.h"
#include "EquipMaskDlg.h"
#include "TaskIDSelDlg.h"
#include "ProcTypeMaskDlg.h"
#include "ElementDataView.h"
#include "TalkModifyDlg.h"
#include "TextInputDlg.h"
#include "SingleTextDlg.h"
#include "AVariant.h"
#include "EC_MD5Hash.h"
#include "ExtTemplateIDSelDlg.h"
#include "DlgSetTime.h"

static LPCTSTR _mask_equip_name[23] = {
	_T("武器"),
	_T("头盔，头饰"),
	_T("项链"),
	_T("披风、飘带"),
	_T("上衣"),
	_T("腰饰"),
	_T("裤子"),
	_T("靴子"),
	_T("护腕"),
	_T("戒指1"),
	_T("戒指2"),
	_T("箭囊"),
	_T("飞剑"),
	_T("时装上衣"),
	_T("时装裤子"),
	_T("时装鞋子"),
	_T("时装护腕"),
	_T("神符"),
	_T("天书"),
	_T("喇叭"),
	_T("自动回血"),
	_T("自动回魔"),
	_T("妙计锦囊"),
};

static LPCTSTR _mask_equip_name2[6] = {
	_T("卡牌1"),
	_T("卡牌2"),
	_T("卡牌3"),
	_T("卡牌4"),
	_T("卡牌5"),
	_T("卡牌6"),
};

static LPCTSTR _mask_visualize_id[11] =  {
	_T("性感"),
	_T("骨感"),
	_T("可爱"),
	_T("冷艳"),
	_T("清秀"),
	_T("俊朗"),
	_T("妖媚"),
	_T("野性"),
	_T("硬朗"),
	_T("阴险"),
	_T("猥琐")
};

static LPCTSTR _mask_patroll_mode[1] = {
	_T("随机走动")
};

static LPCTSTR _mask_character_combo_id[ELEMENTDATA_NUM_PROFESSION] =
{
	_T("武侠"),
	_T("法师"),
	_T("巫师"),
	_T("妖精"),
	_T("妖兽"),
	_T("刺客"),
	_T("羽芒"),
	_T("羽灵"),
	_T("剑灵"),
	_T("魅灵"),
	_T("夜影"),
	_T("月仙"),
};

static LPCTSTR _mask_pet_food[] =
{
	_T("草料"),
	_T("肉类"),
	_T("野菜"),
	_T("水果"),
	_T("清水"),
};

static LPCTSTR _equip_fashion_mask[] =
{
	_T("上衣"),
	_T("裤子"),
	_T("靴子"),
	_T("护腕"),
	_T("头饰"),
	_T("武器"),
};

static LPCTSTR _weapon_action_mask[NUM_WEAPON_TYPE] = 
{
	_T("单手短"),
	_T("双手短"),
	_T("双手长"),
	_T("双手短重"),
	_T("双手长重"),
	_T("软鞭"),
	_T("弓"),
	_T("弩"),
	_T("拳套"),
	_T("枪"),
	_T("空手"),
	_T("匕首"),
	_T("法宝"),
	_T("胧刀"),
	_T("月镰"),
};

static LPCTSTR _combined_services[] =
{
	_T("遗忘生产技能"),		//	0
	_T("打孔服务"),
	_T("空"),
	_T("驿站发现服务"),
	_T("帮派服务"),
	_T("整容服务"),			//	5
	_T("邮寄服务"),
	_T("拍卖服务"),
	_T("双倍经验打卡服务"),
	_T("孵化宠物蛋服务"),
	_T("还原宠物蛋服务"),	//	10
	_T("城战管理服务"),
	_T("离开战场服务"),
	_T("点卡寄售服务"),
	_T("装备升级服务"),
	_T("染色服务"),			//	15
	_T("扭转乾坤服务"),
	_T("小精灵洗属性点"),
	_T("小精灵洗技能点"),
	_T("小精灵遗忘技能"),
	_T("小精灵精炼服务"),	//	20
	_T("小精灵拆分服务"),
	_T("小精灵装备拆卸"),
	_T("修复损毁装备"),
	_T("交易平台服务"),
	_T("仙魔转换服务"),		//	25
	_T("结婚相关服务"),
	_T("基地开启服务"),
	_T("基地升级及上缴"),
	_T("基地材料兑换"),
	_T("无密码查看仓库"),	//	30
	_T("查看沙包排行榜"),
};

static LPCTSTR _combined_services2[] =
{
	_T("国战服务"),			//	0
	_T("离开国战战场"),
	_T("装备签名"),
	_T("加入跨服"),
	_T("返回原服"),
	_T("改名服务"),	
	_T("魂石转化和替换"),
	_T("国王选举"),
	_T("寄卖服务"),
	_T("时装拆分"),
	_T("转生服务"),
	_T("礼品卡"),	
	_T("战车战场报名"),
	_T("卡牌转生"),
	_T("飞剑强化"),
	_T("开启帮派掠夺"),
	_T("开启专用乾坤袋商城"),
	_T("开启专用鸿利商城"),
	_T("变性服务"),
};

static LPCTSTR _immune_type[] =
{
	_T("免疫物理攻击"),
	_T("免疫金系"),
	_T("免疫木系"),
	_T("免疫水系"),
	_T("免疫火系"),
	_T("免疫土系"),
	_T("免疫昏迷"),
	_T("免疫睡眠"),
	_T("免疫减速"),
	_T("免疫定身"),
	_T("免疫封印"),
	_T("免疫伤害加倍"),
	_T("免疫击退"),
	_T("免疫HP上限下降，按比例减HP"),
	_T("免疫打断技能"),
	_T("免疫无视防御伤害"),
	_T("免疫清除Filter(不要勾)"),
	_T("免疫流血"),
	_T("免疫所有负面状态"),	
	_T("免疫掉落物品或金币(不要勾)"),
};

static LPCTSTR _combined_switch[] = 
{
	_T("只接受召唤者攻击"),
	_T("玩家不可交互选中"),
	_T("头顶不显示免疫"),
	_T("死亡记录DPS排行榜"),
};

static LPCTSTR _mine_combined_switch[] = 
{
	_T("矿物归属某玩家"),
	_T("成功采集后广播"),
};

static LPCTSTR _npc_combined_switch[] = 
{
	_T("服务无距离限制"),
};

static LPCTSTR _pet_combined_switch[] = 
{
	_T("使用召唤者模型"),
	_T("玩家不可交互选中"),
	_T("头顶不显示免疫"),
	_T("头顶不显示名字"),
};

static LPCTSTR _autoteam_combined_switch[] = 
{
	_T("禁用"),
	_T("推荐"),
};

static LPCTSTR _targetitem_combined_switch[] = 
{
	_T("在屏幕上提示使用"),
	_T("只对本帮目标使用"),
};

static LPCTSTR _universal_token_combined_switch[] = 
{
	_T("申请token并访问网址"),
};

static LPCTSTR _fashion_combined_switch[] = 
{
	_T("指定范围内随机颜色"),
};

static LPCTSTR _week_combo[] = 
{
	_T("周一"),
	_T("周二"),
	_T("周三"),
	_T("周四"),
	_T("周五"),
	_T("周六"),
	_T("周日"),
};

struct  IncontinuousEnumType
{
	const char *	name;		//	枚举名称
	int					value;		//	对应值
};

static IncontinuousEnumType _level2_type[] = 
{
	{"筑基", 0},
	{"灵虚", 1},
	{"和合", 2},
	{"元婴", 3},
	{"空冥", 4},
	{"履霜", 5},
	{"渡劫", 6},
	{"寂灭", 7},
	{"大乘", 8},
	{"上仙", 20},
	{"真仙", 21},
	{"天仙", 22},
	{"狂魔", 30},
	{"魔圣", 31},
	{"魔尊", 32},
};

static ASetTemplate<int> * CreateIncontinuousEnumType(const IncontinuousEnumType *types, int size)
{
	ASetTemplate<int> *pNew_type = new ASetTemplate<int>;
	for(DWORD i=0; i<size; i++)
		pNew_type->AddElement(types[i].name, types[i].value);
	return pNew_type;
}

/////////////////////////////////////////////////////////////////////////////
// CElementDataDoc

IMPLEMENT_DYNCREATE(CElementDataDoc, CDocument)

BEGIN_MESSAGE_MAP(CElementDataDoc, CDocument)
	//{{AFX_MSG_MAP(CElementDataDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_WINDOW_NEW, OnWindowNew)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementDataDoc construction/destruction

CElementDataDoc::CElementDataDoc()
{
	// TODO: add one-time construction code here
	m_pProperty = NULL;
	m_bExt = false;
	bInited = false;
}

CElementDataDoc::~CElementDataDoc()
{
	if(m_pProperty) delete m_pProperty;
	POSITION pos = m_listTypes.GetHeadPosition();
	while(pos)
	{
		ASetTemplate<int>*ptemp = (ASetTemplate<int>*)m_listTypes.GetNext(pos);
		delete ptemp;
	}

	pos = m_listCustoms.GetHeadPosition();
	while(pos) delete (CUSTOM_FUNCS*)m_listCustoms.GetNext(pos);

	m_Tmpl.Release();
	m_ExtTmpl.Release();
}

BOOL CElementDataDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CElementDataDoc serialization

void CElementDataDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CElementDataDoc diagnostics

#ifdef _DEBUG
void CElementDataDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CElementDataDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CElementDataDoc commands

void CElementDataDoc::OnFileOpen() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("doc");
}

void CElementDataDoc::OnWindowNew() 
{
	// TODO: Add your command handler code here
	
}

BOOL CElementDataDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
    m_pProperty = new ADynPropertyObject;
	m_strDocPath = lpszPathName;
	CString str(lpszPathName);
	str.Replace(g_szWorkDir,"");
	if(!FileIsReadOnly(m_strDocPath))
	{
		//SetModifiedFlag(true);
		nFlag = WAY_DEFAULT;
	}else
	{
		nFlag = WAY_READONLY;	
	}
	m_Tmpl.Release();
	m_ExtTmpl.Release();
	if(str.Find(".tmpl") != -1)
	{
		if(m_Tmpl.Load(str)==0)
		{//读一个模板
			m_bExt = false;
			int id = m_Tmpl.GetID();
			AString tname = m_Tmpl.GetName();
			m_pProperty->DynAddProperty(AVariant(id), "模板ID",NULL,NULL,WAY_READONLY);
			m_pProperty->DynAddProperty(AVariant(tname), "模板名字",NULL,NULL,nFlag);
			int itemNum = m_Tmpl.GetItemNum();
			for(int i=0; i<itemNum; i++)
			{
				AString itemType = m_Tmpl.GetItemType(i);
				AVariant var = m_Tmpl.GetItemValue(i);
				if(AVariant::AVT_INVALIDTYPE == var.GetType())
				{
					if(stricmp(itemType,"int")==0)
					{
						int x = 0;
						m_Tmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					if(stricmp(itemType,"float")==0)
					{
						float x = 0.0f;
						m_Tmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					if(stricmp(itemType,"path")==0)
					{
						AString x("");
						m_Tmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					if(stricmp(itemType,"vector")==0)
					{
						var = A3DVECTOR3(0);
						m_Tmpl.SetItemValue(i,var);
					}else
					if(stricmp(itemType,"color")==0)
					{
						var = 0xff000000;
						m_Tmpl.SetItemValue(i,var);
					}else if (!stricmp(itemType, "poker_show_order")){
						int x = -1;
						m_Tmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}
					else if(g_EnumTypes.GetType(AString(itemType)))
					{//enum
						int x = 0;
						m_Tmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					{//uint,and other user template
						unsigned int x = 0;
						m_Tmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}
								
				}
				Explain(CString(itemType),var,CString(m_Tmpl.GetItemNameToShow(i)));
				
			}
		}
	}else
	if(str.Find(".ext") != -1)
	{
		if(m_ExtTmpl.Load(str, false)==0)
		{//读一个模板
			m_bExt = true;
			int id = m_ExtTmpl.GetID();
			AString tname = m_ExtTmpl.GetName();
			m_pProperty->DynAddProperty(AVariant(id), "模板ID",NULL,NULL,WAY_READONLY);
			m_pProperty->DynAddProperty(AVariant(tname), "模板名字",NULL,NULL,nFlag);
			int itemNum = m_ExtTmpl.GetItemNum();
			for(int i=0; i<itemNum; i++)
			{
				AString itemType = m_ExtTmpl.GetItemType(i);
				AVariant var = m_ExtTmpl.GetItemValue(i);
				if(AVariant::AVT_INVALIDTYPE == var.GetType())
				{
					if(stricmp(itemType,"int")==0)
					{
						int x = 0;
						m_ExtTmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					if(stricmp(itemType,"float")==0)
					{
						float x = 0.0f;
						m_ExtTmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					if(stricmp(itemType,"path")==0)
					{
						AString x("");
						m_ExtTmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else
					if(stricmp(itemType,"vector")==0)
					{
						var = A3DVECTOR3(0);
						m_ExtTmpl.SetItemValue(i,var);
					}else
					if(stricmp(itemType,"color")==0)
					{
						var = 0xff000000;
						m_Tmpl.SetItemValue(i,var);
					}else
					if(g_EnumTypes.GetType(AString(itemType)))
					{//enum
						int x = 0;
						m_ExtTmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}else if(stricmp(itemType,"uint")==0)
					{
						unsigned int x = 0;
						m_ExtTmpl.SetItemValue(i,AVariant(x));
						var = AVariant(x);
					}
				}
				Explain(CString(itemType),var,CString(m_ExtTmpl.GetItemNameToShow(i)));
			}
		}
	}
	return TRUE;
}

BOOL CElementDataDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(!FileIsReadOnly(m_strDocPath))
	{
		POSITION pos = GetFirstViewPosition();
		while (pos != NULL)
		{
			CElementDataView* pView = (CElementDataView*)GetNextView(pos);
			pView->UpdateData();
		}  
		
		AVariant var = m_pProperty->GetPropVal(0);
		var = m_pProperty->GetPropVal(1);
		if(!m_bExt)
		{
			m_Tmpl.SetName((AString)var);
			int itemNum = m_Tmpl.GetItemNum();
			for(int i=2; i<itemNum + 2; i++)
			{
				AVariant var = m_pProperty->GetPropVal(i);
				m_Tmpl.SetItemValue(i-2,var);
			}
			m_Tmpl.Save(m_strDocPath);
#ifdef _MD5_CHECK
			if(!WriteMd5CodeToTemplate(m_strDocPath.GetBuffer(0)))
			{
				AfxMessageBox("CElementDataDoc::OnSaveDocument(), failed to write md5 code to template!");
				return false;
			}
#endif 
			m_strDocPath.ReleaseBuffer();
		}else
		{
			m_ExtTmpl.SetName((AString)var);
			int itemNum = m_ExtTmpl.GetItemNum();
			for(int i=2; i<itemNum + 2; i++)
			{
				AVariant var = m_pProperty->GetPropVal(i);
				m_ExtTmpl.SetItemValue(i-2,var);
			}
			m_ExtTmpl.Save(m_strDocPath);
#ifdef _MD5_CHECK
			if(!WriteMd5CodeToTemplate(m_strDocPath.GetBuffer(0)))
			{
				AfxMessageBox("CElementDataDoc::OnSaveDocument(), failed to write md5 code to template!");
				return false;
			}
#endif
			m_strDocPath.ReleaseBuffer();
		}
	}
	return true;
}

bool CElementDataDoc::IsModifed()
{
	if(FileIsReadOnly(m_strDocPath)) return false;
	
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CElementDataView* pView = (CElementDataView*)GetNextView(pos);
		pView->UpdateData();
	}  
	
	AVariant var = m_pProperty->GetPropVal(0);
	var = m_pProperty->GetPropVal(1);
	if(!m_bExt)
	{
		if(0!=strcmp(m_Tmpl.GetName(),((AString)var)))
			return true;
		
		int itemNum = m_Tmpl.GetItemNum();
		for(int i=2; i<itemNum + 2; i++)
		{
			AVariant var = m_pProperty->GetPropVal(i);
			AVariant old = m_Tmpl.GetItemValue(i-2);
			AString type = m_Tmpl.GetItemType(i-2);

			if(!IsEqual(type,var,old)) return true;
		}
	}else
	{
		if(0!=strcmp(m_ExtTmpl.GetName(),((AString)var)))
			return true;

		int itemNum = m_ExtTmpl.GetItemNum();
		for(int i=2; i<itemNum + 2; i++)
		{
			AVariant var = m_pProperty->GetPropVal(i);
			AVariant old = m_ExtTmpl.GetItemValue(i-2);
			AString type = m_ExtTmpl.GetItemType(i-2);
			if(!IsEqual(type,var,old)) return true;
		}
	}
	
	return false;
}

bool CElementDataDoc::IsEqual(AString type,AVariant var1,AVariant var2)
{
	if(stricmp(type,"int")==0)
	{
		int value1 = (int)var1;
		int value2 = (int)var2;
		if(value1 == value2) return true;
	}else
	if(stricmp(type,"float")==0)
	{
		float value1 = (float)var1;
		float value2 = (float)var2;
		if(value1 == value2) return true;
	}else
	if(stricmp(type,"uint")==0)
	{
		unsigned int value1 = (unsigned int)var1;
		unsigned int value2 = (unsigned int)var2;
		if(value1 == value2) return true;
	}else
	if(stricmp(type,"path")==0)
	{
		AString value1 = (AString)var1;
		AString value2 = (AString)var2;
		if(value1 == value2) return true;	
	}else
	if(stricmp(type,"vector")==0)
	{
		A3DVECTOR3 value1 = (A3DVECTOR3)var1;
		A3DVECTOR3 value2 = (A3DVECTOR3)var2;
		if(value1 == value2) return true;	
	}else
	if(stricmp(type,"color")==0)
	{
		A3DCOLOR value1 = (A3DCOLOR)var1;
		A3DCOLOR value2 = (A3DCOLOR)var2;
		if(value1 == value2) return true;	
	}else
	{
		unsigned int value1 = (unsigned int)var1;
		unsigned int value2 = (unsigned int)var2;
		if(value1 == value2) return true;	
	}
	return false;
}

void CElementDataDoc::Explain(CString type,AVariant var,CString name)
{
	if(stricmp(type,"int")==0)
	{
		m_pProperty->DynAddProperty(AVariant((int)var), name,NULL,NULL,nFlag);
	}else
	if(stricmp(type,"float")==0)
	{
		m_pProperty->DynAddProperty(AVariant((float)var), name,NULL,NULL,nFlag);
	}else
	if(stricmp(type,"uint")==0)
	{
		m_pProperty->DynAddProperty(AVariant((unsigned int)var), name,NULL,NULL,nFlag);
	}else
	if(stricmp(type,"path")==0)
	{
		m_pProperty->DynAddProperty(AVariant((AString)var),name,NULL,NULL,WAY_FILENAME|nFlag,"");
	}else
	if(stricmp(type,"vector")==0)
	{
		m_pProperty->DynAddProperty(AVariant((A3DVECTOR3)var),name,NULL,NULL,WAY_VECTOR|nFlag,"");
	}else
	if(stricmp(type,"color")==0)
	{
		m_pProperty->DynAddProperty(AVariant((A3DCOLOR)var),name,NULL,NULL,WAY_COLOR|nFlag,"");
	}else if (!stricmp(type, "poker_show_order")){
		m_pProperty->DynAddProperty(AVariant((int)var), name,NULL,NULL,nFlag);
	}else
	{//enum
		ENUM_TYPE* pEType = g_EnumTypes.GetType(AString(type));
		if(pEType)
		{//看是否在枚举类型中有
			ASetTemplate<int> *pNew_type = new ASetTemplate<int>;
			m_listTypes.AddTail(pNew_type);
			for(DWORD i=0; i<pEType->dwItemNum; i++)
			{
				pNew_type->AddElement(pEType->listItemName[i],i);
			}
			m_pProperty->DynAddProperty(AVariant(var), name, pNew_type,NULL,nFlag);
			return;
		}else
		{//看生成的模板中有没有
			ExplainEx(type,var,name);
		}
	}
}

void CElementDataDoc::ExplainEx(CString type,AVariant var,CString name)
{
	ASetTemplate<int> *pNew_type = NULL;
	if(stricmp(type,"ext_weapon_type")==0)
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_armor_type")==0)//BaseData\\装备\\护具\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_ornament_type")==0)//BaseData\\装备\\饰品\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_suite_type")==0)//BaseData\\装备\\套装\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_projectile_type")==0)//BaseData\\装备\\弹药\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_npc_type")==0)//"BaseData\\怪物\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_stone_type")==0)//BaseData\\物品\\宝石\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else		
	if(stricmp(type,"ext_bible_type")==0)//BaseData\\物品\\天书\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_sharpener_type")==0)//BaseData\\物品\\磨刀石\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_engrave_type")==0)//BaseData\\镌刻\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else		
	if(stricmp(type,"ext_poker_type")==0)//BaseData\\装备\\卡牌\\附加属性
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"all_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else 
	if(stricmp(type,"building_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\基地设施",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else 
	if(stricmp(type,"equipment_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\装备",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"weapon_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\装备\\武器",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"fashion_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\装备\\时装",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"matter_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\物品",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"material_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else		
	if(stricmp(type,"npc_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\NPC类型",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"mine_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\矿",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"monster_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\怪物",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else		
	if(stricmp(type,"pet_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\宠物",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"pet_evolved_skill_rand_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\进化宠技能随机配置表",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"pet_egg_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\物品\\宠物蛋",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"stone_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\物品\\宝石",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else		
	if(stricmp(type,"skill_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\技能",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"projectile_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\装备\\弹药类型",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"id_projectile")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\装备\\弹药",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"visualize_id")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mask_visualize_id;
		fs->m_nNameCount = sizeof(_mask_visualize_id) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"emotion_id")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Face\\表情情绪属性",
			".tmpl",
			var,
			enumFaceID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}else
	if(stricmp(type,"factions_mask")==0)
	{
		FactionSel* fs = new FactionSel;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"monster_faction")==0)
	{
		MonsterFaction* fs = new MonsterFaction;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"equip_mask")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mask_equip_name;
		fs->m_nNameCount = sizeof(_mask_equip_name) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"equip_mask2")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mask_equip_name2;
		fs->m_nNameCount = sizeof(_mask_equip_name2) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"equip_fashion_mask")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _equip_fashion_mask;
		fs->m_nNameCount = sizeof(_equip_fashion_mask) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"weapon_action_mask")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _weapon_action_mask;
		fs->m_nNameCount = sizeof(_weapon_action_mask) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"combined_services")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _combined_services;
		fs->m_nNameCount = sizeof(_combined_services) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"combined_services2")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _combined_services2;
		fs->m_nNameCount = sizeof(_combined_services2) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"combined_switch")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _combined_switch;
		fs->m_nNameCount = sizeof(_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"mine_combined_switch")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mine_combined_switch;
		fs->m_nNameCount = sizeof(_mine_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}else if(stricmp(type,"npc_combined_switch")==0){
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _npc_combined_switch;
		fs->m_nNameCount = sizeof(_npc_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}else if(stricmp(type,"pet_combined_switch")==0){
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _pet_combined_switch;
		fs->m_nNameCount = sizeof(_pet_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}else if(stricmp(type,"autoteam_combined_switch")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _autoteam_combined_switch;
		fs->m_nNameCount = sizeof(_autoteam_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"targetitem_combined_switch")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _targetitem_combined_switch;
		fs->m_nNameCount = sizeof(_targetitem_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"universal_token_combined_switch")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _universal_token_combined_switch;
		fs->m_nNameCount = sizeof(_universal_token_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"fashion_combined_switch")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _fashion_combined_switch;
		fs->m_nNameCount = sizeof(_fashion_combined_switch) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"week_combo")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _week_combo;
		fs->m_nNameCount = sizeof(_week_combo) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"immune_type")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _immune_type;
		fs->m_nNameCount = sizeof(_immune_type) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"patroll_mode")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mask_patroll_mode;
		fs->m_nNameCount = sizeof(_mask_patroll_mode) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"character_combo_id")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mask_character_combo_id;
		fs->m_nNameCount = sizeof(_mask_character_combo_id) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"food_type")==0)
	{
		EquipMask* fs = new EquipMask;
		m_listCustoms.AddTail(fs);
		fs->m_var = var;
		fs->m_pMaskNames = _mask_pet_food;
		fs->m_nNameCount = sizeof(_mask_pet_food) / sizeof(LPCTSTR);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"task_type")==0)
	{
		TASK_ID_FUNCS* fs = new TASK_ID_FUNCS;
		m_listCustoms.AddTail(fs);
		fs->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"proc_type")==0)
	{
		ProcTypeMask* fs = new ProcTypeMask;
		m_listCustoms.AddTail(fs);
		fs->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"talk_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\交谈",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"sell_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\出售商品",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"buy_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\收购商品",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"repair_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\修理商品",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"install_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\镶嵌",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"uninstall_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\拆除",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"task_out_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\发放任务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"task_in_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\验证完成任务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"task_item_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\发放任务物品",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"skill_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\教授",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"heal_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\治疗",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"transmit_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\传送",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"transport_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\运输",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"proxy_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\代售",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"storage_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\存储",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"make_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\生产",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"decompose_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\分解",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"identify_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\鉴定",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"war_towerbuild_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\城战炮塔建造",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"resetprop_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\洗点服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"petname_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\宠物改名服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"petlearnskill_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\宠物学习技能服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"petforgetskill_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\宠物遗忘技能服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"equipbind_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\装备绑定服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"equipdestroy_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\装备销毁服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"equipundestroy_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\装备解除销毁服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"engrave_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\镌刻",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"randprop_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\属性值随机",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"force_service")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\NPC\\功能\\势力服务",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"recipe_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\配方",
			".tmpl",
			var,
			enumRecipeID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"engrave_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\镌刻",
			".tmpl",
			var,
			enumRecipeID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"randprop_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\属性值随机",
			".tmpl",
			var,
			enumRecipeID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"talk_type")==0)
	{
		TALK_ID_FUNCS* ts = new TALK_ID_FUNCS;
		m_listCustoms.AddTail(ts);
		ts->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)ts, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"text_type")==0)
	{
		TextInputFuncs* ts = new TextInputFuncs;
		m_listCustoms.AddTail(ts);
		ts->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)ts, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"single_text")==0)
	{
		SingleTextFuncs* ts = new SingleTextFuncs;
		m_listCustoms.AddTail(ts);
		ts->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)ts, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"ansi_text")==0)
	{
		SingleTextFuncs* ts = new SingleTextFuncs;
		m_listCustoms.AddTail(ts);
		ts->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)ts, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"page_title")==0)
	{
		SingleTextFuncs* ts = new SingleTextFuncs;
		m_listCustoms.AddTail(ts);
		ts->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)ts, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"name_text")==0)
	{
		SingleTextFuncs* ts = new SingleTextFuncs;
		m_listCustoms.AddTail(ts);
		ts->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)ts, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"force_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\势力配置表",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"level2_type")==0)
	{
		pNew_type = CreateIncontinuousEnumType(_level2_type, sizeof(_level2_type)/sizeof(_level2_type[0]));
		m_listTypes.AddTail(pNew_type);		
		m_pProperty->DynAddProperty(AVariant(var), name, pNew_type,NULL,nFlag);
	}
	else
	if(stricmp(type,"pet_evolved_config_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\进化宠技能配置表",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"task_list_config_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\任务列表配置表",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"time_length")==0)
	{
		SetTimeLengthFunc* fs = new SetTimeLengthFunc;
		m_listCustoms.AddTail(fs);
		fs->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"expire_time")==0)
	{
		SetExpireTimeFunc* fs = new SetExpireTimeFunc;
		m_listCustoms.AddTail(fs);
		fs->OnSetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)fs, NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"title_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\称号",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}
	else
	if(stricmp(type,"history_stage")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\历史阶段",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"chariot_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\战车配置表",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"poker_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\装备\\卡牌",
			".tmpl",
			var,
			enumBaseID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	if(stricmp(type,"token_shop_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\包裹商城",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else if(stricmp(type,"player_action_type")==0)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->Init(
			"BaseData\\Config\\动作属性表",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	{
		CString msg;
		msg.Format("CElementDataDoc::ExplainEx(),解释器不能解释的类型--%s",type);
		AfxMessageBox(msg);
		ASSERT(false);
	}
}

bool CElementDataDoc::EnumFiles(ASetTemplate<int>* pType,AVariant var)
{
	return  false;
	bool bFound = false;
	unsigned int id;
	CString path;
	abase::vector<AString> pathArray;
	g_ExtBaseIDMan.GeneratePathArray(pathArray);
	for( size_t i = 0; i < pathArray.size(); ++i)
	{
		path = pathArray[i];
		id = g_ExtBaseIDMan.GetIDByPath(AString(path));
		if(id==(int)var) bFound = true;
		int pos = path.ReverseFind('\\');
		CString ObjectName = path.Right(pos);
		pType->AddElement(AString(ObjectName),id);
	}
	
	if(!bFound && (int)var != 0) return false;
	else return true;
}

void CElementDataDoc::SetName()
{
	CString title = GetTitle();
	if(!bInited)
	{
		SetTitle(title + szID);
		bInited = true;
	}
}

void CElementDataDoc::OnFileSave() 
{
	// TODO: Add your command handler code here
	CString pathName = GetPathName();
	if(!FileIsReadOnly(AString(pathName)))
		CDocument::OnFileSave();
}

void CElementDataDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::OnCloseDocument();
}

void CElementDataDoc::Save()
{
	OnFileSave();	
}

void CElementDataDoc::DeleteContents() 
{
	CDocument::DeleteContents();
}

BOOL CElementDataDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(IsModifed())
	{
		SetModifiedFlag(true);
	}
	return CDocument::CanCloseFrame(pFrame);
}
void CElementDataDoc::Fresh()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CElementDataView* pView = (CElementDataView*)GetNextView(pos);
		pView->UpdateData();
	}  
}

void CElementDataDoc::SetTitle(LPCTSTR lpszTitle) 
{
	CDocument::SetTitle(lpszTitle);
}
