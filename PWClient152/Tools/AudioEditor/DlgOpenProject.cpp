// DlgOpenProject.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgOpenProject.h"
#include "ProjectList.h"
#include "Global.h"
#include "Project.h"
#include "MainFrm.h"
#include "Engine.h"


// CDlgOpenProject 对话框

IMPLEMENT_DYNAMIC(CDlgOpenProject, CBCGPDialog)

CDlgOpenProject::CDlgOpenProject(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgOpenProject::IDD, pParent)
{

}

CDlgOpenProject::~CDlgOpenProject()
{
}

void CDlgOpenProject::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROJ_NAMES, m_wndListbox);
}


BEGIN_MESSAGE_MAP(CDlgOpenProject, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgOpenProject::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DEL_PROJECT, &CDlgOpenProject::OnBnClickedDelProject)
	ON_LBN_DBLCLK(IDC_LIST_PROJ_NAMES, &CDlgOpenProject::OnLbnDblclkListProjNames)
END_MESSAGE_MAP()


// CDlgOpenProject 消息处理程序

void CDlgOpenProject::OnBnClickedOk()
{
	int i = m_wndListbox.GetCurSel();
	if(i == -1)
		return;
	CString strName;
	m_wndListbox.GetText(i, strName);	
	if(!Engine::GetInstance().LoadProject(strName))
	{
		GF_Log(LOG_ERROR, "打开工程“%s”失败", strName);
		return;
	}
	Project* pProject = Engine::GetInstance().GetCurProject();
	GF_GetMainFrame()->GetBankBar()->UpdateTree();
	GF_GetMainFrame()->GetSoundDefBar()->UpdateTree();
	GF_GetMainFrame()->GetEventBar()->UpdateTree();
	
	CBCGPDialog::OnOK();
}

BOOL CDlgOpenProject::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();

	if(!ProjectList::GetInstance().LoadXML(g_Configs.szProjectListFile))
		return FALSE;
	int iNum = ProjectList::GetInstance().GetNum();
	for (int i=0; i<iNum; ++i)
	{
		m_wndListbox.InsertString(i, ProjectList::GetInstance().GetName(i));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgOpenProject::OnBnClickedDelProject()
{
	int i = m_wndListbox.GetCurSel();
	if(i == -1)
		return;
	CString strName;
	m_wndListbox.GetText(i, strName);
	CString csTips;
	csTips.Format("确定删除工程“%s”吗？", strName);
	if(IDYES != MessageBox(csTips, "警告", MB_ICONWARNING | MB_YESNO))
		return;

	if(!Engine::GetInstance().DeleteProject(strName))
	{
		GF_Log(LOG_ERROR, "删除工程“%s”失败", strName);
		return;
	}

	while(m_wndListbox.GetCount())
		m_wndListbox.DeleteString(0);

	int iNum = ProjectList::GetInstance().GetNum();
	for (int i=0; i<iNum; ++i)
	{
		m_wndListbox.InsertString(i, ProjectList::GetInstance().GetName(i));
	}
	GF_Log(LOG_NORMAL, "成功删除工程“%s”", strName);
}

void CDlgOpenProject::OnLbnDblclkListProjNames()
{
	OnBnClickedOk();
}
