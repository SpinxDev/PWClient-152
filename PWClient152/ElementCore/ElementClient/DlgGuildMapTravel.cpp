// Filename	: DlgGuildMapTravel.cpp
// Creator	: Xiao Zhou
// Date		: 2005/2/7

#include "DlgGuildMapTravel.h"
#include "DlgGuildMap.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "globaldataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildMapTravel, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)

AUI_END_COMMAND_MAP()

CDlgGuildMapTravel::CDlgGuildMapTravel()
{
}

CDlgGuildMapTravel::~CDlgGuildMapTravel()
{
}

void CDlgGuildMapTravel::OnCommandCancel(const char * szCommand)
{
	Show(false);
}

void CDlgGuildMapTravel::OnCommandConfirm(const char * szCommand)
{
	int nSel = m_pLst_Target->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Target->GetCount() && 
		m_pLst_Target->GetItemData(nSel) != 0 )
	{
		Show(false);
		if( GetHostPlayer()->ReturnToTargetTown(m_pLst_Target->GetItemData(nSel)) )
			GetGameUIMan()->m_pDlgGuildMap->OnCommandCancel("");
		else
			GetGameUIMan()->MessageBox("ChallengeErr", GetStringFromTable(760),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

	}
}

bool CDlgGuildMapTravel::OnInitDialog()
{
	m_pLst_Target = (PAUILISTBOX)GetDlgItem("Lst_Target");
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Confirm");

	return true;
}

void CDlgGuildMapTravel::OnTick()
{
	int nSel = m_pLst_Target->GetCurSel();
	m_pBtn_Confirm->Enable(nSel >= 0 && nSel < m_pLst_Target->GetCount() &&
		m_pLst_Target->GetItemData(nSel) != 0 );
}

void CDlgGuildMapTravel::SetDomainId(int n)
{
	m_pLst_Target->ResetContent();
	abase::vector<TRANS_TARGET> * transtarget = globaldata_gettranstargets();
	DWORD i;
	for(i = 0; i < transtarget->size(); i++ )
		if( (*transtarget)[i].domain_id == n )
		{
			m_pLst_Target->AddString((*transtarget)[i].name);
			m_pLst_Target->SetItemTextColor(m_pLst_Target->GetCount() - 1, A3DCOLORRGB(128, 128, 128));
			int j;
			for( j = 0; j < GetHostPlayer()->GetWayPointNum(); j++ )
				if( GetHostPlayer()->GetWayPoint(j) == (*transtarget)[i].id )
				{
					m_pLst_Target->SetItemTextColor(m_pLst_Target->GetCount() - 1, A3DCOLORRGB(255, 255, 255));
					m_pLst_Target->SetItemData(m_pLst_Target->GetCount() - 1, (*transtarget)[i].id);
					break;
				}
		}
}
