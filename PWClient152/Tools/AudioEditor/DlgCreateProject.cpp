// DlgCreateProject.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgCreateProject.h"
#include "ProjectList.h"
#include "Global.h"
#include "Engine.h"
#include "MainFrm.h"

// CDlgCreateProject 对话框

IMPLEMENT_DYNAMIC(CDlgCreateProject, CBCGPDialog)

CDlgCreateProject::CDlgCreateProject(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgCreateProject::IDD, pParent)
	, m_strProjectName(_T(""))
{

}

CDlgCreateProject::~CDlgCreateProject()
{
}

void CDlgCreateProject::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROJ_NAME, m_strProjectName);
}

BOOL CDlgCreateProject::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BEGIN_MESSAGE_MAP(CDlgCreateProject, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgCreateProject::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgCreateProject 消息处理程序

void CDlgCreateProject::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_strProjectName.GetLength() == 0)
	{
		MessageBox("工程名不可为空", "警告", MB_ICONWARNING);
		return;
	}

	if(!ProjectList::GetInstance().LoadXML(g_Configs.szProjectListFile))
		return;

	if(ProjectList::GetInstance().IsExist(m_strProjectName))
	{
		MessageBox("与现有工程重名", "警告", MB_ICONWARNING);
		return;
	}
	if(!Engine::GetInstance().CreateProject(m_strProjectName))
		return;
	if(!ProjectList::GetInstance().Add(m_strProjectName))
		return;
	if(!ProjectList::GetInstance().SaveXML(g_Configs.szProjectListFile))
		return;

	if(!Engine::GetInstance().LoadProject(m_strProjectName))
		return;
	Project* pProject = Engine::GetInstance().GetCurProject();
	GF_GetMainFrame()->GetBankBar()->UpdateTree();
	GF_GetMainFrame()->GetSoundDefBar()->UpdateTree();
	GF_GetMainFrame()->GetEventBar()->UpdateTree();
	CBCGPDialog::OnOK();
}