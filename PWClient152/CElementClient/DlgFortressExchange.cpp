#include "DlgFortressExchange.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressExchange, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Exchange", OnCommand_Exchange)
AUI_ON_COMMAND("Combo_M01", OnCommand_Combo_M01)
AUI_ON_COMMAND("Combo_M02", OnCommand_Combo_M02)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressExchange, CDlgBase)
AUI_ON_EVENT("Edit_01",	WM_KEYDOWN,	OnEventKeyDown_Edit_01)
AUI_END_EVENT_MAP()

CDlgFortressExchange::CDlgFortressExchange()
{
}

bool CDlgFortressExchange::OnInitDialog()
{
	m_pCombo_M01 = (PAUICOMBOBOX)GetDlgItem("Combo_M01");
	m_pEdit_01 = GetDlgItem("Edit_01");
	m_pCombo_M02 = (PAUICOMBOBOX)GetDlgItem("Combo_M02");
	m_pEdit_02 = GetDlgItem("Edit_02");
	m_pBtn_Exchange = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Exchange");
	return true;
}

void CDlgFortressExchange::OnShowDialog()
{
	UpdateInfo();
	
	if (m_pCombo_M01->GetCount() <= 0)
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9162));
}

void CDlgFortressExchange::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	
	//	关闭相关对话框
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");
	
	//	关闭 NPC 服务
	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
		GetGameUIMan()->EndNPCService();
}

void CDlgFortressExchange::OnCommand_Exchange(const char *szCommand)
{
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	if (!pInfo)
	{
		//	尚无基地信息，不知道材料信息
		return;
	}

	int iMaterial1 = GetSrcMaterial();
	if (iMaterial1 < 0)
	{
		//	源材料信息有问题
		return;
	}
	
	int iMaterial2 = GetDstMaterial();
	if (iMaterial2 < 0)
	{
		//	目标材料信息有问题
		return;
	}

	int nConvert = GetMaterialCntToConvert();
	if (nConvert <= 0)
	{
		//	输入材料个数不正确
		ChangeFocus(m_pEdit_01);
		return;
	}

	//	执行兑换
	GetGameSession()->c2s_CmdNPCSevFactionFortressConvertMaterial(iMaterial1, iMaterial2, nConvert);
}

void CDlgFortressExchange::UpdateInfo()
{
	m_pEdit_01->SetText(_AL(""));
	m_pEdit_01->Enable(false);
	m_pCombo_M01->ResetContent();
	m_pCombo_M01->Enable(false);

	m_pEdit_02->SetText(_AL(""));
	m_pCombo_M02->ResetContent();
	m_pCombo_M02->Enable(false);

	m_pBtn_Exchange->Enable(false);
	
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	if (!pInfo)
	{
		//	尚无基地信息，不知道材料信息
		return;
	}

	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		if (pInfo->material[i] > 0)
		{
			int nIndex = m_pCombo_M01->GetCount();
			m_pCombo_M01->InsertString(nIndex, GetStringFromTable(9130 + i));
			m_pCombo_M01->SetItemData(nIndex, i);
		}
	}
	if (m_pCombo_M01->GetCount() <= 0)
	{
		//	无任何材料剩余
		return;
	}
	m_pCombo_M01->Enable(true);
	m_pEdit_01->Enable(true);

	m_pCombo_M01->SetCurSel(0);
	OnCommand_Combo_M01(NULL);
	ChangeFocus(m_pEdit_01);
}

void CDlgFortressExchange::OnCommand_Combo_M01(const char *szCommand)
{
	//	源材料选取变化，更新目标材料内容等
	//
	m_pEdit_02->SetText(_AL(""));
	m_pCombo_M02->ResetContent();
	m_pCombo_M02->Enable(false);

	m_pBtn_Exchange->Enable(false);

	int iMaterial1 = GetSrcMaterial();
	if (iMaterial1 < 0)
	{
		//	源材料信息有问题
		return;
	}

	//	更新目标列表
	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		if (i != iMaterial1)
		{
			int nIndex = m_pCombo_M02->GetCount();
			m_pCombo_M02->InsertString(nIndex, GetStringFromTable(9130 + i));
			m_pCombo_M02->SetItemData(nIndex, i);
		}
	}
	m_pCombo_M02->Enable(true);
	m_pCombo_M02->SetCurSel(0);
	OnCommand_Combo_M02(NULL);
	ChangeFocus(m_pEdit_01);
}

void CDlgFortressExchange::OnCommand_Combo_M02(const char *szCommand)
{
	//	目标材料内容变化，更新可兑换数目等
	//
	m_pEdit_02->SetText(_AL(""));
	m_pBtn_Exchange->Enable(false);

	int iMaterial2 = GetDstMaterial();
	if (iMaterial2 < 0)
	{
		//	目标材料信息有问题
		return;
	}

	int nConvert = GetMaterialCntToConvert();
	if (nConvert < 0)
	{
		//	源材料数目输入有问题
		return;
	}

	//	显示转换后数值
	int nConverted = GetMaterialCntConverted();
	ShowConverted(nConverted);

	//	开启转换按钮
	m_pBtn_Exchange->Enable(IsCanConvert());
	ChangeFocus(m_pEdit_01);
}

void CDlgFortressExchange::OnEventKeyDown_Edit_01(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_ESCAPE)
	{
		ChangeFocus(NULL);
		return;
	}
}

int CDlgFortressExchange::GetMaterialCntConverted()
{
	//	材料转换计算
	int nConvert = GetMaterialCntToConvert();
	int nConverted = nConvert/2;
	return nConverted;
}

void CDlgFortressExchange::OnTick()
{
	if (IsCanConvert())
	{
		m_pBtn_Exchange->Enable(true);
		int nConverted = GetMaterialCntConverted();
		ShowConverted(nConverted);
	}
	else
	{
		m_pBtn_Exchange->Enable(false);
		m_pEdit_02->SetText(_AL(""));
	}
}

void CDlgFortressExchange::ShowConverted(int nConverted)
{
	ACString strText;
	strText.Format(_AL("%d"), nConverted);
	m_pEdit_02->SetText(strText);
}

bool CDlgFortressExchange::IsCanConvert()
{
	int nConverted = GetMaterialCntConverted();
	return nConverted > 0;
}

int CDlgFortressExchange::GetSrcMaterial()
{
	int iMaterial1 = -1;
	int iSel1 = m_pCombo_M01->GetCurSel();
	if (iSel1 >= 0 || iSel1 < m_pCombo_M01->GetCount())
	{
		iMaterial1 = m_pCombo_M01->GetItemData(iSel1);
	}
	return iMaterial1;
}

int CDlgFortressExchange::GetDstMaterial()
{
	int iMaterial2 = -1;
	int iSel2 = m_pCombo_M02->GetCurSel();
	if (iSel2 >= 0 || iSel2 < m_pCombo_M02->GetCount())
	{
		iMaterial2 = m_pCombo_M02->GetItemData(iSel2);
	}
	return iMaterial2;
}

int CDlgFortressExchange::GetMaterialCntToConvert()
{
	int nConvert(-1);
	
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	int iMaterial1 = GetSrcMaterial();
	if (iMaterial1 >= 0 && pInfo != NULL)
	{
		ACString strNumber = m_pEdit_01->GetText();
		int count = strNumber.ToInt();
		if (count > 0 && count <= pInfo->material[iMaterial1])
		{
			nConvert = count;
		}
	}

	return nConvert;
}