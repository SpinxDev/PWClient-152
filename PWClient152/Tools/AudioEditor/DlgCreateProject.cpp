// DlgCreateProject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgCreateProject.h"
#include "ProjectList.h"
#include "Global.h"
#include "Engine.h"
#include "MainFrm.h"

// CDlgCreateProject �Ի���

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BEGIN_MESSAGE_MAP(CDlgCreateProject, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgCreateProject::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgCreateProject ��Ϣ�������

void CDlgCreateProject::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_strProjectName.GetLength() == 0)
	{
		MessageBox("����������Ϊ��", "����", MB_ICONWARNING);
		return;
	}

	if(!ProjectList::GetInstance().LoadXML(g_Configs.szProjectListFile))
		return;

	if(ProjectList::GetInstance().IsExist(m_strProjectName))
	{
		MessageBox("�����й�������", "����", MB_ICONWARNING);
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