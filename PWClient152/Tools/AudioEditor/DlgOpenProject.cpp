// DlgOpenProject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgOpenProject.h"
#include "ProjectList.h"
#include "Global.h"
#include "Project.h"
#include "MainFrm.h"
#include "Engine.h"


// CDlgOpenProject �Ի���

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


// CDlgOpenProject ��Ϣ�������

void CDlgOpenProject::OnBnClickedOk()
{
	int i = m_wndListbox.GetCurSel();
	if(i == -1)
		return;
	CString strName;
	m_wndListbox.GetText(i, strName);	
	if(!Engine::GetInstance().LoadProject(strName))
	{
		GF_Log(LOG_ERROR, "�򿪹��̡�%s��ʧ��", strName);
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgOpenProject::OnBnClickedDelProject()
{
	int i = m_wndListbox.GetCurSel();
	if(i == -1)
		return;
	CString strName;
	m_wndListbox.GetText(i, strName);
	CString csTips;
	csTips.Format("ȷ��ɾ�����̡�%s����", strName);
	if(IDYES != MessageBox(csTips, "����", MB_ICONWARNING | MB_YESNO))
		return;

	if(!Engine::GetInstance().DeleteProject(strName))
	{
		GF_Log(LOG_ERROR, "ɾ�����̡�%s��ʧ��", strName);
		return;
	}

	while(m_wndListbox.GetCount())
		m_wndListbox.DeleteString(0);

	int iNum = ProjectList::GetInstance().GetNum();
	for (int i=0; i<iNum; ++i)
	{
		m_wndListbox.InsertString(i, ProjectList::GetInstance().GetName(i));
	}
	GF_Log(LOG_NORMAL, "�ɹ�ɾ�����̡�%s��", strName);
}

void CDlgOpenProject::OnLbnDblclkListProjNames()
{
	OnBnClickedOk();
}
