// PolicyOperationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "PolicyOperationDlg.h"
#include "PropertyList.h"
#include "AObject.h"
#include "Policy.h"
#include "OperationParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString strOperationList[CTriggerData::o_num] = 
{ "发起攻击","使用技能","说话","重置仇恨列表","执行一个新触发器",
"停止一个触发器","激活一个触发器","创建一个定时器","停止一个定时器",
"逃跑","调整仇恨到第一","调整仇恨度到最低",
"仇恨度减少50%",
"跳过后面的操作条目",
"激活产生怪物控制器",
"设置全局变量",
"修正全局变量",
"召唤怪物",
"沿路巡径",
"播放动作",
"修正历史变量",
"设置历史变量",
"发放帮派掠夺战积分",
"计算变量",
"召唤怪物（新）",
"沿路巡径（新）",
"使用技能（新）",
"激活产生怪物控制器（新）",
"发放任务",
"召唤矿物",
"召唤NPC",
"区域内随机发放任务",
"仇恨列表内发放任务",
};
static CString strTargetList[CTriggerData::t_num] = { "仇恨排名第一位","仇恨排名第二位","仇恨排名第一位以后对象的随机一位","最多hp","最多mp","最少hp","职业的组合","自己"
, "怪物击杀归属玩家"
, "怪物出生地归属帮派"
, "仇恨列表随机一个目标"
, "仇恨列表中最近目标"
, "仇恨列表中最远目标"
, "仇恨排名第一位（宠物主人）"
};

CString GetTargetTypeName(int type)
{
	if (type >= 0 && type < CTriggerData::t_num){
		return strTargetList[type];
	}
	CString str;
	str.Format("Unknown TargetType(%d)", type);
	return str;
}
/////////////////////////////////////////////////////////////////////////////
// CPolicyOperationDlg dialog


CPolicyOperationDlg::CPolicyOperationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolicyOperationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolicyOperationDlg)
	m_bWuXia = FALSE;
	m_bYaoShou = FALSE;
	m_bYuLin = FALSE;
	m_bYuMang = FALSE;
	m_bFaShi = FALSE;
	m_bCiKe = FALSE;
	m_bWuShi = FALSE;
	m_bYaoJing = FALSE;
	m_bJianLing = FALSE;
	m_bMeiLing = FALSE;
	//}}AFX_DATA_INIT
	m_pList = NULL;
	m_pOperationModifier = NULL;
	m_pPolicyData = NULL;
	m_bModify = false;
}


void CPolicyOperationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolicyOperationDlg)
	DDX_Control(pDX, IDC_COMBO_TARGET_TYPE, m_cbTargetType);
	DDX_Control(pDX, IDC_COMBO_OPERATION_TYPE, m_cbOperationType);
	DDX_Check(pDX, IDC_CHECK_WUXIA, m_bWuXia);
	DDX_Check(pDX, IDC_CHECK_YAOSHOU, m_bYaoShou);
	DDX_Check(pDX, IDC_CHECK_YULIN, m_bYuLin);
	DDX_Check(pDX, IDC_CHECK_YUMANG, m_bYuMang);
	DDX_Check(pDX, IDC_CHECK_FASHI, m_bFaShi);
	DDX_Check(pDX, IDC_CHECK_CIKE, m_bCiKe);
	DDX_Check(pDX, IDC_CHECK_WUSHI, m_bWuShi);
	DDX_Check(pDX, IDC_CHECK_YAOJING, m_bYaoJing);
	DDX_Check(pDX, IDC_CHECK_JIANLING, m_bJianLing);
	DDX_Check(pDX, IDC_CHECK_MEILING, m_bMeiLing);
	DDX_Check(pDX, IDC_CHECK_YEYING, m_bYeYing);
	DDX_Check(pDX, IDC_CHECK_YUEXIAN, m_bYueXian);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolicyOperationDlg, CDialog)
	//{{AFX_MSG_MAP(CPolicyOperationDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATION_TYPE, OnSelchangeComboOperationType)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGET_TYPE, OnSelchangeComboTargetType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolicyOperationDlg message handlers

int CPolicyOperationDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}

BOOL CPolicyOperationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pList = new CPropertyList();
	m_pTemp = new ADynPropertyObject;
	CWnd *pWnd = GetDlgItem(IDC_LIST_OPERATION);
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

	BOOL bResult = m_pList->Create("操作表", WS_CHILD | WS_VISIBLE, rc, this, 111);
	
	//填充操作类型列表
	int i(0);
	for( i = 0; i < CTriggerData::o_num; i++)
		m_cbOperationType.InsertString(i,strOperationList[i]);
	if(m_cbOperationType.GetCount()>0) m_cbOperationType.SetCurSel(0);

	for( i = 0; i < CTriggerData::t_num; i++)
		m_cbTargetType.InsertString(i,GetTargetTypeName(i));
	if(m_cbTargetType.GetCount()>0) m_cbTargetType.SetCurSel(0);
	EnableCheckBox(false);
	if(m_bModify)
	{
		InitOperation();
	}
	else ChangeProperty(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPolicyOperationDlg::ReleaseList(){	
	if(m_pList){
		m_pList->CleanItems();
		m_pList->DestroyWindow();
		delete m_pList;
		if(m_pTemp) delete m_pTemp;
		m_pList = 0;
		m_pTemp = 0;
	}
}

void CPolicyOperationDlg::ReleaseOperation(){
	if (m_pOperationModifier){
		CTriggerData::FreeOperationTarget(m_pOperationModifier->mTarget);
		delete m_pOperationModifier;
		m_pOperationModifier = NULL;
	}
}

void CPolicyOperationDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	ReleaseList();
}

void CPolicyOperationDlg::UpdateProperty(ADynPropertyObject *pProObj)
{
	if(m_pList && pProObj) m_pList->AttachDataObject(pProObj);
}

void CPolicyOperationDlg::OnSelchangeComboOperationType() 
{
	// TODO: Add your control notification handler code here
	int sel = m_cbOperationType.GetCurSel();
	if(sel!=-1)
	{
		CString szOp;
		m_cbOperationType.GetLBText(sel,szOp);
		ChangeProperty(sel);
	}
}

void CPolicyOperationDlg::ChangeProperty(int iOpType)
{
	UpdateProperty(m_pTemp);
	ReleaseOperation();
	m_pOperationModifier = COperationParam::Create(iOpType);
	if (!m_pOperationModifier){
		MessageBox("不认识的操作类型!");
	}else{
		m_pOperationModifier->m_pPolicyData = m_pPolicyData;
		m_pOperationModifier->BuildProperty();
		UpdateProperty(m_pOperationModifier->m_pProperty);
	}
}

void CPolicyOperationDlg::InitOperation()
{
	if(m_pOperation==NULL) return; 
	ReleaseOperation();	
	m_cbOperationType.SetCurSel(m_pOperation->iType);
	m_cbTargetType.SetCurSel(m_pOperation->mTarget.iType);
	m_pOperationModifier = COperationParam::Create(m_pOperation->iType);
	if (!m_pOperationModifier){
		MessageBox("不认识的操作类型!");
	}else{
		m_pOperationModifier->CopyFrom(m_pOperation);
		m_pOperationModifier->mTarget.iType = m_pOperation->mTarget.iType;
		m_pOperationModifier->m_pPolicyData = m_pPolicyData;
		m_pOperationModifier->BuildProperty();
		UpdateProperty(m_pOperationModifier->m_pProperty);
	}

	if(m_pOperation->mTarget.iType == CTriggerData::t_occupation_list)
	{
		unsigned int uBit = ((T_OCCUPATION*)m_pOperation->mTarget.pParam)->uBit;
		
		if(uBit & CTriggerData::o_wuxia) m_bWuXia = TRUE;
		if(uBit & CTriggerData::o_fashi) m_bFaShi = TRUE;
		if(uBit & CTriggerData::o_wushi) m_bWuShi = TRUE;
		if(uBit & CTriggerData::o_yaojing) m_bYaoJing = TRUE;
		if(uBit & CTriggerData::o_yaoshou) m_bYaoShou = TRUE;
		if(uBit & CTriggerData::o_cike) m_bCiKe = TRUE;
		if(uBit & CTriggerData::o_yumang) m_bYuMang = TRUE;
		if(uBit & CTriggerData::o_yuling) m_bYuLin = TRUE;
		if(uBit & CTriggerData::o_jianling) m_bJianLing = TRUE;
		if(uBit & CTriggerData::o_meiling) m_bMeiLing = TRUE;
		if(uBit & CTriggerData::o_yeying) m_bYeYing = TRUE;
		if(uBit & CTriggerData::o_yuexian) m_bYueXian = TRUE;
		UpdateData(false);
	}

	OnSelchangeComboTargetType();
}

void CPolicyOperationDlg::OnOK()
{
	
	int sel = m_cbTargetType.GetCurSel();
	if(sel==-1) return;
	UpdateData(true);
	if(m_pOperationModifier==NULL) return;
	
	m_pOperationModifier->mTarget.iType = sel;
	CTriggerData::FreeOperationTarget(m_pOperationModifier->mTarget);
	
	if(sel==CTriggerData::t_occupation_list)
	{
		T_OCCUPATION *pNewData = (T_OCCUPATION*)malloc(sizeof(T_OCCUPATION));
		pNewData->uBit = 0;
		if(m_bWuXia) pNewData->uBit |=CTriggerData::o_wuxia;
		if(m_bFaShi) pNewData->uBit |=CTriggerData::o_fashi;
		if(m_bYaoShou) pNewData->uBit |=CTriggerData::o_yaoshou;
		if(m_bYuLin) pNewData->uBit |=CTriggerData::o_yuling;
		if(m_bYuMang) pNewData->uBit |=CTriggerData::o_yumang;
		if(m_bYaoJing) pNewData->uBit |=CTriggerData::o_yaojing;
		if(m_bCiKe) pNewData->uBit |=CTriggerData::o_cike;
		if(m_bWuShi) pNewData->uBit |=CTriggerData::o_wushi;
		if(m_bJianLing) pNewData->uBit |=CTriggerData::o_jianling;
		if(m_bMeiLing) pNewData->uBit |=CTriggerData::o_meiling;
		if(m_bYeYing) pNewData->uBit |=CTriggerData::o_yeying;
		if(m_bYueXian) pNewData->uBit |=CTriggerData::o_yuexian;
		m_pOperationModifier->mTarget.pParam = pNewData;
	}else m_pOperationModifier->mTarget.pParam = 0;
	
	if(m_pList) m_pList->UpdateData(true);
	if(m_pOperationModifier) m_pOperationModifier->UpdateProperty(true);
	CDialog::OnOK();
}

void CPolicyOperationDlg::OnCancel()
{
	ReleaseList();
	ReleaseOperation();
	CDialog::OnCancel();
}

void CPolicyOperationDlg::OnSelchangeComboTargetType() 
{
	// TODO: Add your control notification handler code here
	int sel = m_cbTargetType.GetCurSel();
	if(sel!=-1)
	{
		if(sel==CTriggerData::t_occupation_list)
		{
			EnableCheckBox(true);
			return;
		}
	}
	EnableCheckBox(false);
}

void CPolicyOperationDlg::OnButtonAddTarget() 
{
	// TODO: Add your control notification handler code here
}

void CPolicyOperationDlg::OnButtonDelTarget() 
{

}

void CPolicyOperationDlg::FreshTargetList()
{
}

void CPolicyOperationDlg::EnableCheckBox(bool bEnable)
{
	CWnd *pWnd = GetDlgItem(IDC_CHECK_WUXIA);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_FASHI);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_YAOSHOU);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_YULIN);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_YUMANG);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_WUSHI);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_YAOJING);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_CIKE);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_JIANLING);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_MEILING);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_YEYING);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHECK_YUEXIAN);
	pWnd->EnableWindow(bEnable);
}
