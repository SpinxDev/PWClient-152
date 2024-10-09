// TreasureItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "TreasureItemDlg.h"
#include "PropertyList.h"
#include "AObject.h"
#include "TemplIDSelDlg.h"
#include "TimeSetDlg.h"
#include "Treasure.h"

#define new A_DEBUG_NEW




BOOL CALLBACK TIME_SET_FUNCS::OnActivate(void)
{
	CTimeSetDlg dlg;
	dlg.m_Time = CTime((time_t)m_var);
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.m_Time.GetTime();
	if ((unsigned int)m_var == 0) m_strText = "（无效）";
	else 
	{
		CTime tTime = ((unsigned int)m_var);
		m_strText.Format("%d-%d-%d %d:%d:%d",tTime.GetYear(),tTime.GetMonth(),tTime.GetDay(),tTime.GetHour(),tTime.GetMinute(),tTime.GetSecond());
	}
	return TRUE;
}

LPCTSTR CALLBACK TIME_SET_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK TIME_SET_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK TIME_SET_FUNCS::OnSetValue(const AVariant& var)
{
	m_var = var;
	unsigned long uTime = unsigned long(m_var);

	AString str;
	if (uTime != 0)
	{
		CTime tTime = (uTime);
		m_strText.Format("%d-%d-%d %d:%d:%d",tTime.GetYear(),tTime.GetMonth(),tTime.GetDay(),tTime.GetHour(),tTime.GetMinute(),tTime.GetSecond());
	}
	else
		m_strText = "（无效）";
}
/////////////////////////////////////////////////////////////////////////////
// CTreasureItemDlg dialog


CTreasureItemDlg::CTreasureItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreasureItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreasureItemDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Init_Treasure_item(&m_Item);
	m_pListIDSel = 0;
	m_bReadOnly = false;
	sort_type.AddElement("类型0", 0);
	sort_type.AddElement("类型1", 1);
	sort_type.AddElement("类型2", 2);
	sort_type.AddElement("类型3", 3);
	sort_type.AddElement("类型4", 4);
	sort_type.AddElement("类型5", 5);
	sort_type.AddElement("类型6", 6);
	sort_type.AddElement("类型7", 7);
}


void CTreasureItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreasureItemDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreasureItemDlg, CDialog)
	//{{AFX_MSG_MAP(CTreasureItemDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreasureItemDlg message handlers

BOOL CTreasureItemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pList = new CPropertyList();
	m_pProperty = new ADynPropertyObject;
	m_pTimeSet1 = new TIME_SET_FUNCS;
	m_pTimeSet2 = new TIME_SET_FUNCS;
	m_pTimeSet3 = new TIME_SET_FUNCS;
	m_pTimeSet4 = new TIME_SET_FUNCS;

	
	CWnd *pWnd = GetDlgItem(IDC_LIST_TREASURE);
	CRect rc,rc1;
	pWnd->GetClientRect(&rc);

	int width = rc.Width();
	int length = rc.Height();

	pWnd->GetWindowRect(&rc);
	GetWindowRect(&rc1);
	rc.left = rc.left - rc1.left;
	rc.top = rc.top - rc1.top - 20;
	rc.right = rc.left + width;
	rc.bottom = rc.top + length;

	BOOL bResult = m_pList->Create("宝物", WS_CHILD | WS_VISIBLE, rc, this, 121);
	
	BuildProperty();
	m_pList->AttachDataObject(m_pProperty);
	UpdateProperty(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTreasureItemDlg::BuildProperty()
{
	m_pListIDSel = new CTemplIDSel;
	m_pListIDSel->Init(
			"BaseData",
			".tmpl",
			AVariant(0),
			enumBaseID);
	DWORD flag = 0;
	if(m_bReadOnly) flag = WAY_READONLY;
	m_pProperty->DynAddProperty(AVariant(AString(m_Item.szName)), "宝物名(32字符)",NULL,NULL,flag);
	
	m_pProperty->DynAddProperty(AVariant(m_Item.tid), "宝物ID",NULL,NULL,WAY_READONLY);
	m_pProperty->DynAddProperty(AVariant(AString(m_Item.file_icon)), "图标路径",NULL,NULL,WAY_FILENAME | flag);

	m_pProperty->DynAddProperty(AVariant(m_Item.id), "物品",(ASet*)m_pListIDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.count), "物品数量",NULL,NULL,flag);
	
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[0].price), "购买方式1(价格)",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[0].until_time), "到期时间点1([秒])",(ASet*)m_pTimeSet1,NULL,WAY_CUSTOM | flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[0].time), "购买方式1(租借时间[秒])",NULL,NULL,flag);
	
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[1].price), "购买方式2(价格)",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[1].until_time), "到期时间点2([秒])",(ASet*)m_pTimeSet2,NULL,WAY_CUSTOM | flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[1].time), "购买方式2(租借时间[秒])",NULL,NULL,flag);

	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[2].price), "购买方式3(价格)",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[2].until_time), "到期时间点3([秒])",(ASet*)m_pTimeSet3,NULL,WAY_CUSTOM | flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[2].time), "购买方式3(租借时间[秒])",NULL,NULL,flag);

	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[3].price), "购买方式4(价格)",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[3].until_time), "到期时间点4([秒])",(ASet*)m_pTimeSet4,NULL,WAY_CUSTOM | flag);
	m_pProperty->DynAddProperty(AVariant(m_Item.buy_fashion[3].time), "购买方式4(租借时间[秒])",NULL,NULL,flag);

	bool temp = false;
	m_pProperty->DynAddProperty(AVariant(temp), "是否为新品",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(temp), "是否为推荐品",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(temp), "是否为促销品",NULL,NULL,flag);
	m_pProperty->DynAddProperty(AVariant(AString(WC2AC(m_Item.desc))), "物品描述(512字符)",NULL,NULL,flag);
}

void CTreasureItemDlg::UpdateProperty(bool bGet)
{
	m_pList->UpdateData(true);
	if(bGet)
	{
		
		AString name = AString(m_pProperty->GetPropVal(0));
		strcpy(m_Item.szName, name);
		//m_Item.sid = m_pProperty->GetPropVal(2);	
		name = AString(m_pProperty->GetPropVal(2));
		if(!name.IsEmpty()) strcpy(m_Item.file_icon,name);
		
		m_Item.id = m_pProperty->GetPropVal(3);	
		m_Item.count = m_pProperty->GetPropVal(4);	
		m_Item.buy_fashion[0].price = m_pProperty->GetPropVal(5);
		m_Item.buy_fashion[0].until_time = m_pProperty->GetPropVal(6);
		m_Item.buy_fashion[0].time = m_pProperty->GetPropVal(7);

		m_Item.buy_fashion[1].price = m_pProperty->GetPropVal(8);
		m_Item.buy_fashion[1].until_time = m_pProperty->GetPropVal(9);
		m_Item.buy_fashion[1].time = m_pProperty->GetPropVal(10);

		m_Item.buy_fashion[2].price = m_pProperty->GetPropVal(11);
		m_Item.buy_fashion[2].until_time = m_pProperty->GetPropVal(12);
		m_Item.buy_fashion[2].time = m_pProperty->GetPropVal(13);

		m_Item.buy_fashion[3].price = m_pProperty->GetPropVal(14);
		m_Item.buy_fashion[3].until_time = m_pProperty->GetPropVal(15);
		m_Item.buy_fashion[3].time = m_pProperty->GetPropVal(16);
		
		DWORD flag = BIT_PRESENT;
		bool temp = (bool) m_pProperty->GetPropVal(17);
		if(temp) m_Item.others = m_Item.others | flag;
		else m_Item.others &= ~flag;

		flag = BIT_RECOMMEND;
		temp = (bool) m_pProperty->GetPropVal(18);
		if(temp) m_Item.others = m_Item.others | flag;
		else m_Item.others &= ~flag;

		flag = BIT_SALES_PROMOTION;
		temp = (bool) m_pProperty->GetPropVal(19);
		if(temp) m_Item.others = m_Item.others | flag;
		else m_Item.others &= ~flag;
		wcscpy(m_Item.desc,AC2WC(AString(m_pProperty->GetPropVal(20))));

	}else
	{
		m_pProperty->SetPropVal(0,AString(m_Item.szName));
		m_pProperty->SetPropVal(1,m_Item.tid);
		//m_pProperty->SetPropVal(2,m_Item.sid);
		m_pProperty->SetPropVal(2,AString(m_Item.file_icon));
		
		m_pProperty->SetPropVal(3,m_Item.id);
		m_pListIDSel->SetValue(AVariant(m_Item.id));
		m_pProperty->SetPropVal(4,m_Item.count);
		
		m_pProperty->SetPropVal(5,m_Item.buy_fashion[0].price);
		m_pTimeSet1->OnSetValue(m_Item.buy_fashion[0].until_time);
		m_pProperty->SetPropVal(7,m_Item.buy_fashion[0].time);

		m_pProperty->SetPropVal(8,m_Item.buy_fashion[1].price);
		m_pTimeSet2->OnSetValue(m_Item.buy_fashion[1].until_time);
		m_pProperty->SetPropVal(10,m_Item.buy_fashion[1].time);

		m_pProperty->SetPropVal(11,m_Item.buy_fashion[2].price);
		m_pTimeSet3->OnSetValue(m_Item.buy_fashion[2].until_time);
		m_pProperty->SetPropVal(13,m_Item.buy_fashion[2].time);

		m_pProperty->SetPropVal(14,m_Item.buy_fashion[3].price);
		m_pTimeSet4->OnSetValue(m_Item.buy_fashion[3].until_time);
		m_pProperty->SetPropVal(16,m_Item.buy_fashion[3].time);

		if(m_Item.others & BIT_PRESENT) 
			m_pProperty->SetPropVal(17,true);
		else m_pProperty->SetPropVal(17,false);

		if(m_Item.others & BIT_RECOMMEND) 
			m_pProperty->SetPropVal(18,true);
		else m_pProperty->SetPropVal(18,false);

		if(m_Item.others & BIT_SALES_PROMOTION) 
			m_pProperty->SetPropVal(19,true);
		else m_pProperty->SetPropVal(19,false);

		m_pProperty->SetPropVal(20,WC2AC(m_Item.desc));
	}
	m_pList->AttachDataObject(m_pProperty);
};

BOOL CTreasureItemDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pList)
	{
		m_pList->CleanItems();
		m_pList->DestroyWindow();
		delete m_pList;
	}
	if(m_pProperty) delete m_pProperty;

	if(m_pListIDSel) delete m_pListIDSel; 
	if(m_pTimeSet1) delete m_pTimeSet1;
	if(m_pTimeSet2) delete m_pTimeSet2;
	if(m_pTimeSet3) delete m_pTimeSet3;
	if(m_pTimeSet4) delete m_pTimeSet4;

	return CDialog::DestroyWindow();
}

void CTreasureItemDlg::OnOK()
{
	UpdateData(true);
	UpdateProperty(true);
	CDialog::OnOK();
}
