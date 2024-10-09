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
	_T("����"),
	_T("ͷ����ͷ��"),
	_T("����"),
	_T("���硢Ʈ��"),
	_T("����"),
	_T("����"),
	_T("����"),
	_T("ѥ��"),
	_T("����"),
	_T("��ָ1"),
	_T("��ָ2"),
	_T("����"),
	_T("�ɽ�"),
	_T("ʱװ����"),
	_T("ʱװ����"),
	_T("ʱװЬ��"),
	_T("ʱװ����"),
	_T("���"),
	_T("����"),
	_T("����"),
	_T("�Զ���Ѫ"),
	_T("�Զ���ħ"),
	_T("��ƽ���"),
};

static LPCTSTR _mask_equip_name2[6] = {
	_T("����1"),
	_T("����2"),
	_T("����3"),
	_T("����4"),
	_T("����5"),
	_T("����6"),
};

static LPCTSTR _mask_visualize_id[11] =  {
	_T("�Ը�"),
	_T("�Ǹ�"),
	_T("�ɰ�"),
	_T("����"),
	_T("����"),
	_T("����"),
	_T("����"),
	_T("Ұ��"),
	_T("Ӳ��"),
	_T("����"),
	_T("���")
};

static LPCTSTR _mask_patroll_mode[1] = {
	_T("����߶�")
};

static LPCTSTR _mask_character_combo_id[ELEMENTDATA_NUM_PROFESSION] =
{
	_T("����"),
	_T("��ʦ"),
	_T("��ʦ"),
	_T("����"),
	_T("����"),
	_T("�̿�"),
	_T("��â"),
	_T("����"),
	_T("����"),
	_T("����"),
	_T("ҹӰ"),
	_T("����"),
};

static LPCTSTR _mask_pet_food[] =
{
	_T("����"),
	_T("����"),
	_T("Ұ��"),
	_T("ˮ��"),
	_T("��ˮ"),
};

static LPCTSTR _equip_fashion_mask[] =
{
	_T("����"),
	_T("����"),
	_T("ѥ��"),
	_T("����"),
	_T("ͷ��"),
	_T("����"),
};

static LPCTSTR _weapon_action_mask[NUM_WEAPON_TYPE] = 
{
	_T("���ֶ�"),
	_T("˫�ֶ�"),
	_T("˫�ֳ�"),
	_T("˫�ֶ���"),
	_T("˫�ֳ���"),
	_T("���"),
	_T("��"),
	_T("��"),
	_T("ȭ��"),
	_T("ǹ"),
	_T("����"),
	_T("ذ��"),
	_T("����"),
	_T("�ʵ�"),
	_T("����"),
};

static LPCTSTR _combined_services[] =
{
	_T("������������"),		//	0
	_T("��׷���"),
	_T("��"),
	_T("��վ���ַ���"),
	_T("���ɷ���"),
	_T("���ݷ���"),			//	5
	_T("�ʼķ���"),
	_T("��������"),
	_T("˫������򿨷���"),
	_T("�������ﵰ����"),
	_T("��ԭ���ﵰ����"),	//	10
	_T("��ս�������"),
	_T("�뿪ս������"),
	_T("�㿨���۷���"),
	_T("װ����������"),
	_T("Ⱦɫ����"),			//	15
	_T("ŤתǬ������"),
	_T("С����ϴ���Ե�"),
	_T("С����ϴ���ܵ�"),
	_T("С������������"),
	_T("С���龫������"),	//	20
	_T("С�����ַ���"),
	_T("С����װ����ж"),
	_T("�޸����װ��"),
	_T("����ƽ̨����"),
	_T("��ħת������"),		//	25
	_T("�����ط���"),
	_T("���ؿ�������"),
	_T("�����������Ͻ�"),
	_T("���ز��϶һ�"),
	_T("������鿴�ֿ�"),	//	30
	_T("�鿴ɳ�����а�"),
};

static LPCTSTR _combined_services2[] =
{
	_T("��ս����"),			//	0
	_T("�뿪��սս��"),
	_T("װ��ǩ��"),
	_T("������"),
	_T("����ԭ��"),
	_T("��������"),	
	_T("��ʯת�����滻"),
	_T("����ѡ��"),
	_T("��������"),
	_T("ʱװ���"),
	_T("ת������"),
	_T("��Ʒ��"),	
	_T("ս��ս������"),
	_T("����ת��"),
	_T("�ɽ�ǿ��"),
	_T("���������Ӷ�"),
	_T("����ר��Ǭ�����̳�"),
	_T("����ר�ú����̳�"),
	_T("���Է���"),
};

static LPCTSTR _immune_type[] =
{
	_T("����������"),
	_T("���߽�ϵ"),
	_T("����ľϵ"),
	_T("����ˮϵ"),
	_T("���߻�ϵ"),
	_T("������ϵ"),
	_T("���߻���"),
	_T("����˯��"),
	_T("���߼���"),
	_T("���߶���"),
	_T("���߷�ӡ"),
	_T("�����˺��ӱ�"),
	_T("���߻���"),
	_T("����HP�����½�����������HP"),
	_T("���ߴ�ϼ���"),
	_T("�������ӷ����˺�"),
	_T("�������Filter(��Ҫ��)"),
	_T("������Ѫ"),
	_T("�������и���״̬"),	
	_T("���ߵ�����Ʒ����(��Ҫ��)"),
};

static LPCTSTR _combined_switch[] = 
{
	_T("ֻ�����ٻ��߹���"),
	_T("��Ҳ��ɽ���ѡ��"),
	_T("ͷ������ʾ����"),
	_T("������¼DPS���а�"),
};

static LPCTSTR _mine_combined_switch[] = 
{
	_T("�������ĳ���"),
	_T("�ɹ��ɼ���㲥"),
};

static LPCTSTR _npc_combined_switch[] = 
{
	_T("�����޾�������"),
};

static LPCTSTR _pet_combined_switch[] = 
{
	_T("ʹ���ٻ���ģ��"),
	_T("��Ҳ��ɽ���ѡ��"),
	_T("ͷ������ʾ����"),
	_T("ͷ������ʾ����"),
};

static LPCTSTR _autoteam_combined_switch[] = 
{
	_T("����"),
	_T("�Ƽ�"),
};

static LPCTSTR _targetitem_combined_switch[] = 
{
	_T("����Ļ����ʾʹ��"),
	_T("ֻ�Ա���Ŀ��ʹ��"),
};

static LPCTSTR _universal_token_combined_switch[] = 
{
	_T("����token��������ַ"),
};

static LPCTSTR _fashion_combined_switch[] = 
{
	_T("ָ����Χ�������ɫ"),
};

static LPCTSTR _week_combo[] = 
{
	_T("��һ"),
	_T("�ܶ�"),
	_T("����"),
	_T("����"),
	_T("����"),
	_T("����"),
	_T("����"),
};

struct  IncontinuousEnumType
{
	const char *	name;		//	ö������
	int					value;		//	��Ӧֵ
};

static IncontinuousEnumType _level2_type[] = 
{
	{"����", 0},
	{"����", 1},
	{"�ͺ�", 2},
	{"ԪӤ", 3},
	{"��ڤ", 4},
	{"��˪", 5},
	{"�ɽ�", 6},
	{"����", 7},
	{"���", 8},
	{"����", 20},
	{"����", 21},
	{"����", 22},
	{"��ħ", 30},
	{"ħʥ", 31},
	{"ħ��", 32},
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
		{//��һ��ģ��
			m_bExt = false;
			int id = m_Tmpl.GetID();
			AString tname = m_Tmpl.GetName();
			m_pProperty->DynAddProperty(AVariant(id), "ģ��ID",NULL,NULL,WAY_READONLY);
			m_pProperty->DynAddProperty(AVariant(tname), "ģ������",NULL,NULL,nFlag);
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
		{//��һ��ģ��
			m_bExt = true;
			int id = m_ExtTmpl.GetID();
			AString tname = m_ExtTmpl.GetName();
			m_pProperty->DynAddProperty(AVariant(id), "ģ��ID",NULL,NULL,WAY_READONLY);
			m_pProperty->DynAddProperty(AVariant(tname), "ģ������",NULL,NULL,nFlag);
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
		{//���Ƿ���ö����������
			ASetTemplate<int> *pNew_type = new ASetTemplate<int>;
			m_listTypes.AddTail(pNew_type);
			for(DWORD i=0; i<pEType->dwItemNum; i++)
			{
				pNew_type->AddElement(pEType->listItemName[i],i);
			}
			m_pProperty->DynAddProperty(AVariant(var), name, pNew_type,NULL,nFlag);
			return;
		}else
		{//�����ɵ�ģ������û��
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
	if(stricmp(type,"ext_armor_type")==0)//BaseData\\װ��\\����\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_ornament_type")==0)//BaseData\\װ��\\��Ʒ\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_suite_type")==0)//BaseData\\װ��\\��װ\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_projectile_type")==0)//BaseData\\װ��\\��ҩ\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_npc_type")==0)//"BaseData\\����\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_stone_type")==0)//BaseData\\��Ʒ\\��ʯ\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else		
	if(stricmp(type,"ext_bible_type")==0)//BaseData\\��Ʒ\\����\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_sharpener_type")==0)//BaseData\\��Ʒ\\ĥ��ʯ\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else
	if(stricmp(type,"ext_engrave_type")==0)//BaseData\\�Կ�\\��������
	{
		CExtTemplIDSel* _IDSel = new CExtTemplIDSel;
		m_listCustoms.AddTail(_IDSel);
		_IDSel->SetValue(var);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM |nFlag);
	}else		
	if(stricmp(type,"ext_poker_type")==0)//BaseData\\װ��\\����\\��������
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
			"BaseData\\������ʩ",
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
			"BaseData\\װ��",
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
			"BaseData\\װ��\\����",
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
			"BaseData\\װ��\\ʱװ",
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
			"BaseData\\��Ʒ",
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
			"BaseData\\NPC\\NPC����",
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
			"BaseData\\NPC\\��",
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
			"BaseData\\����",
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
			"BaseData\\����",
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
			"BaseData\\Config\\�����輼��������ñ�",
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
			"BaseData\\��Ʒ\\���ﵰ",
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
			"BaseData\\��Ʒ\\��ʯ",
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
			"BaseData\\����",
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
			"BaseData\\װ��\\��ҩ����",
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
			"BaseData\\װ��\\��ҩ",
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
			"BaseData\\Face\\������������",
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
			"BaseData\\NPC\\����\\��̸",
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
			"BaseData\\NPC\\����\\������Ʒ",
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
			"BaseData\\NPC\\����\\�չ���Ʒ",
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
			"BaseData\\NPC\\����\\������Ʒ",
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
			"BaseData\\NPC\\����\\��Ƕ",
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
			"BaseData\\NPC\\����\\���",
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
			"BaseData\\NPC\\����\\��������",
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
			"BaseData\\NPC\\����\\��֤�������",
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
			"BaseData\\NPC\\����\\����������Ʒ",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\�洢",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\�ֽ�",
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
			"BaseData\\NPC\\����\\����",
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
			"BaseData\\NPC\\����\\��ս��������",
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
			"BaseData\\NPC\\����\\ϴ�����",
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
			"BaseData\\NPC\\����\\�����������",
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
			"BaseData\\NPC\\����\\����ѧϰ���ܷ���",
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
			"BaseData\\NPC\\����\\�����������ܷ���",
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
			"BaseData\\NPC\\����\\װ���󶨷���",
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
			"BaseData\\NPC\\����\\װ�����ٷ���",
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
			"BaseData\\NPC\\����\\װ��������ٷ���",
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
			"BaseData\\NPC\\����\\�Կ�",
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
			"BaseData\\NPC\\����\\����ֵ���",
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
			"BaseData\\NPC\\����\\��������",
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
			"BaseData\\�䷽",
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
			"BaseData\\�Կ�",
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
			"BaseData\\����ֵ���",
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
			"BaseData\\Config\\�������ñ�",
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
			"BaseData\\Config\\�����輼�����ñ�",
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
			"BaseData\\Config\\�����б����ñ�",
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
			"BaseData\\Config\\�ƺ�",
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
			"BaseData\\Config\\��ʷ�׶�",
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
			"BaseData\\Config\\ս�����ñ�",
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
			"BaseData\\װ��\\����",
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
			"BaseData\\Config\\�����̳�",
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
			"BaseData\\Config\\�������Ա�",
			".tmpl",
			var,
			enumConfigID);
		m_pProperty->DynAddProperty(var, name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | nFlag);
	}
	else
	{
		CString msg;
		msg.Format("CElementDataDoc::ExplainEx(),���������ܽ��͵�����--%s",type);
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
