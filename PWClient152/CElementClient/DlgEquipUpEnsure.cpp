

#include "AFI.h"
#include "DlgEquipUpEnsure.h"
#include "DlgProduce.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_UIHelper.h"
#include "EC_FixedMsg.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipUpEnsure, CDlgBase)
AUI_ON_COMMAND("confirm",  OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

CDlgEquipUpEnsure::CDlgEquipUpEnsure()
{
	m_pBtn_Confirm = NULL;
	m_pTxt_Num = NULL;
	m_pTxt_Inherit = NULL;
	m_pTxt_NoInherit = NULL;

	m_inheriStatus = 0;
	m_iTotalFee = 0;
}

bool CDlgEquipUpEnsure::OnInitDialog()
{
	CDlgBase::OnInitDialog();
	
	DDX_Control("Txt_Succeed", m_pTxt_Inherit);
	DDX_Control("Txt_NotSucceed", m_pTxt_NoInherit);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Lbl_StoneCost", m_pTxt_Num);

	return true;
}

void CDlgEquipUpEnsure::OnShowDialog()
{
	m_pTxt_Num->SetText(_AL(""));
	m_pTxt_Inherit->SetText(_AL(""));
	m_pTxt_NoInherit->SetText(_AL(""));

	int i(0);
	ACString strTex;
	char inheritFlag = INHERIT_REFINE;
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	for (i=0;i<INHERIT_PROPERTYNUM;i++){
		ACString inheritName;
		strTex.Format(GetStringFromTable(11330+i));
		if (!(m_activeStatus & inheritFlag)){
			inheritFlag = inheritFlag << 1;		//若没有激活则不显示，检测下一位属性是否继承
			continue;
		}
		if (m_inheriStatus & inheritFlag){
			inheritName += pDescTab->GetWideString(ITEMDESC_COL_GREEN) + strTex + _AL('\r');
			m_pTxt_Inherit->AppendText(inheritName);
		}else{
			inheritName += pDescTab->GetWideString(ITEMDESC_COL_RED) + strTex + _AL('\r');
			m_pTxt_NoInherit->AppendText(inheritName);
		}
		inheritFlag = inheritFlag << 1;
	}
	if (m_iTotalFee>0)
		strTex.Format(GetStringFromTable(8063), m_iTotalFee);
	else
		strTex = GetStringFromTable(8064);
	m_pTxt_Num->SetText(strTex);
	
}

void CDlgEquipUpEnsure::OnCommand_Confirm( const char *szCommand )
{
	GetGameUIMan()->m_pDlgProduce->UpgradeEquipment(m_inheriStatus);
	GetGameUIMan()->m_pDlgEquipUp->Show(false);
	OnCommand_CANCEL("");

}
void CDlgEquipUpEnsure::OnCommand_CANCEL( const char *szCommand )
{
	m_inheriStatus = 0;
	m_iTotalFee = 0;
	Show(false);
}

void CDlgEquipUpEnsure::SetInheritStatus( char status, char activestatus, int Inheritfee )
{
	m_inheriStatus = status;
	m_iTotalFee = Inheritfee;
	m_activeStatus = activestatus;
}