#include "DlgFortressBuild.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "DlgFortressBuildSubList.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressBuild, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressBuild, CDlgBase)
AUI_END_EVENT_MAP()

CDlgFortressBuild::CDlgFortressBuild()
{
	m_pDlgBuildList = NULL;
}

bool CDlgFortressBuild::OnInitDialog()
{
	return true;
}

void CDlgFortressBuild::OnShowDialog()
{
	UpdateInfo();
}

void CDlgFortressBuild::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	
	//	�ر���ضԻ���
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");

	//	�ر� NPC ����
	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
		GetGameUIMan()->EndNPCService();
}

void CDlgFortressBuild::UpdateInfo()
{
	if (!m_pDlgBuildList)
		m_pDlgBuildList = (CDlgFortressBuildSubList *)GetGameUIMan()->GetDialog("Win_FortressBuildSubList");
	if (m_pDlgBuildList && m_pDlgBuildList->IsShow())
		m_pDlgBuildList->UpdateInfo();
}

bool CDlgFortressBuild::OnChangeLayoutBegin()
{
	OnCommand_CANCEL(NULL);

	if (m_pDlgBuildList)
	{
		//	���������ʩ����ؿؼ��Ի���
		//	�Լ���Ϸ�����������
		m_pDlgBuildList->ClearBuilding();
		m_pDlgBuildList = NULL;
		m_pAUIManager->Tick(0);
	}
	
	return true;
}
