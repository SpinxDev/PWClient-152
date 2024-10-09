#include "DlgGuildDiplomacyMan.h"
#include "DlgGuildMan.h"
#include "DlgGuildDiplomacyApply.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "Network/factionlistrelation_re.hpp"
#include "Network/rpcdata/gfactionalliance"
#include "Network/rpcdata/gfactionhostile"
#include "Network/rpcdata/gfactionrelationapply"
#include "Network\\ids.hxx"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildDiplomacyMan, CDlgBase)
AUI_ON_COMMAND("Btn_Alliance01", OnCommand_KillAlliance)
AUI_ON_COMMAND("Btn_Alliance02", OnCommand_Force_KillAlliance)
AUI_ON_COMMAND("Btn_Hostility01", OnCommand_KillHostility)
AUI_ON_COMMAND("Btn_Hostility02", OnCommand_Force_KillHostility)
AUI_ON_COMMAND("Btn_Agree", OnCommand_Reply_Agree)
AUI_ON_COMMAND("Btn_Refuse", OnCommand_Reply_Refuse)
AUI_ON_COMMAND("Btn_ApplyFaction", OnCommand_ApplyFaction)
AUI_ON_COMMAND("Btn_ApplyType",		OnCommand_ApplyType)
AUI_ON_COMMAND("Btn_ApplyEndTime", OnCommand_ApplyEndTime)
AUI_ON_COMMAND("Btn_DiplomacyApply", OnCommand_DiplomacyApply)
AUI_ON_COMMAND("Btn_Refresh", OnCommand_Refresh)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGuildDiplomacyMan, CDlgBase)
AUI_ON_EVENT("List_Apply", WM_LBUTTONDOWN, OnEventLButtonDown_List_Apply)
AUI_END_EVENT_MAP()

CDlgGuildDiplomacyMan::CDlgGuildDiplomacyMan()
{
	m_pList_Alliance = NULL;
	m_pList_Hostility = NULL;
	m_pList_Apply = NULL;
	m_pBtn_Agree = NULL;
	m_pBtn_Refuse = NULL;	
	m_pBtn_Alliance01 = NULL;
	m_pBtn_Alliance02 = NULL;
	m_pBtn_Hostility01 = NULL;
	m_pBtn_Hostility02 = NULL;
	m_pBtn_DiplomacyApply = NULL;
	m_iLastSendTime = 0;
}

bool CDlgGuildDiplomacyMan::OnInitDialog()
{
	DDX_Control("List_Alliance", m_pList_Alliance);
	DDX_Control("List_Hostility", m_pList_Hostility);
	DDX_Control("List_Apply", m_pList_Apply);
	DDX_Control("Btn_Agree", m_pBtn_Agree);
	DDX_Control("Btn_Refuse", m_pBtn_Refuse);
	DDX_Control("Btn_Alliance01", m_pBtn_Alliance01);
	DDX_Control("Btn_Alliance02", m_pBtn_Alliance02);
	DDX_Control("Btn_Hostility01", m_pBtn_Hostility01);
	DDX_Control("Btn_Hostility02", m_pBtn_Hostility02);
	DDX_Control("Btn_DiplomacyApply", m_pBtn_DiplomacyApply);
	return true;
}

void CDlgGuildDiplomacyMan::OnShowDialog()
{
	Refresh();
}

void CDlgGuildDiplomacyMan::OnTick()
{
	ACString strText;
	int i(0);
	int fid(0), leftTime(0), type(0);
	PAUILISTBOX pListBox = NULL;
	int serverTime = GetGame()->GetServerGMTTime();

	//	同盟帮派列表
	pListBox = m_pList_Alliance;
	for (i = 0; i < pListBox->GetCount(); ++ i)
	{
		fid = pListBox->GetItemData(i, ALLIANCE_FACTION_INDEX);
		leftTime = pListBox->GetItemData(i, ALLIANCE_TIME_INDEX) - serverTime;
		a_ClampFloor(leftTime, 0);
		if (GetAllianceLineString(fid, leftTime, strText))
		{
			pListBox->SetText(i, strText);
			if (pListBox->GetItemData(i, ALLIANCE_OK_INDEX))
				pListBox->SetItemData(i, 0, ALLIANCE_OK_INDEX);
		}
	}

	//	敌对帮派列表
	pListBox = m_pList_Hostility;
	for (i = 0; i < pListBox->GetCount(); ++ i)
	{
		fid = pListBox->GetItemData(i, HOSTILITY_FACTION_INDEX);
		leftTime = pListBox->GetItemData(i, HOSTILITY_TIME_INDEX) - serverTime;
		a_ClampFloor(leftTime, 0);
		if (GetHostilityLineString(fid, leftTime, strText))
		{
			pListBox->SetText(i, strText);
			if (pListBox->GetItemData(i, HOSTILITY_OK_INDEX))
				pListBox->SetItemData(i, 0, HOSTILITY_OK_INDEX);
		}
	}

	//	申请列表
	pListBox = m_pList_Apply;
	for (i = 0; i < pListBox->GetCount(); ++ i)
	{
		fid = pListBox->GetItemData(i, APPLY_FACTION_INDEX);
		type = pListBox->GetItemData(i, APPLY_TYPE_INDEX);
		leftTime = pListBox->GetItemData(i, APPLY_TIME_INDEX) - serverTime;
		a_ClampFloor(leftTime, 0);
		if (GetApplyLineString(fid, type, leftTime, strText))
		{
			pListBox->SetText(i, strText);
			if (pListBox->GetItemData(i, APPLY_OK_INDEX))
				pListBox->SetItemData(i, 0, APPLY_OK_INDEX);
		}
	}
}

void CDlgGuildDiplomacyMan::Refresh()
{
	//	非帮主或副帮主时屏蔽操作功能
	bool bCan = CanDoAction();
	m_pBtn_Alliance01->Enable(bCan);
	m_pBtn_Alliance02->Enable(bCan);
	m_pBtn_Hostility01->Enable(bCan);
	m_pBtn_Hostility02->Enable(bCan);
	m_pBtn_DiplomacyApply->Enable(bCan);

	m_pList_Alliance->ResetContent();
	m_pList_Hostility->ResetContent();
	m_pList_Apply->ResetContent();
	OnEventLButtonDown_List_Apply(0, 0, m_pList_Apply);
	
	GetGameSession()->faction_get_relationlist();
}

void CDlgGuildDiplomacyMan::OnPrtcFactionListRelation_Re(GNET::FactionListRelation_Re *pProtocol)
{
	if (!pProtocol)
		return;

	ACString strText;
	size_t i(0);
	PAUILISTBOX pListBox = NULL;
	int serverTime = GetGame()->GetServerGMTTime();
	int leftTime = 0;

	//	同盟帮派列表
	pListBox = m_pList_Alliance;
	pListBox->ResetContent();
	for (i = 0; i < pProtocol->alliance.size(); ++ i)
	{
		const GNET::GFactionAlliance &a = pProtocol->alliance[i];
		leftTime = a.end_time - serverTime;
		if (GetAllianceLineString(a.fid, leftTime, strText, true))
		{
			pListBox->AddString(strText);
		}
		else
		{
			pListBox->AddString(strText);
			pListBox->SetItemData(pListBox->GetCount()-1, 1, ALLIANCE_OK_INDEX);
		}
		pListBox->SetItemData(pListBox->GetCount()-1, a.fid, ALLIANCE_FACTION_INDEX);
		pListBox->SetItemData(pListBox->GetCount()-1, a.end_time, ALLIANCE_TIME_INDEX);
	}

	//	敌对帮派列表
	pListBox = m_pList_Hostility;
	pListBox->ResetContent();
	for (i = 0; i < pProtocol->hostile.size(); ++ i)
	{
		const GNET::GFactionHostile &h = pProtocol->hostile[i];
		leftTime = h.end_time - serverTime;
		if (GetHostilityLineString(h.fid, leftTime, strText, true))
		{
			pListBox->AddString(strText);
		}
		else
		{
			pListBox->AddString(strText);
			pListBox->SetItemData(pListBox->GetCount()-1, 1, HOSTILITY_OK_INDEX);
		}
		pListBox->SetItemData(pListBox->GetCount()-1, h.fid, HOSTILITY_FACTION_INDEX);
		pListBox->SetItemData(pListBox->GetCount()-1, h.end_time, HOSTILITY_TIME_INDEX);
	}

	//	申请列表
	pListBox = m_pList_Apply;
	pListBox->ResetContent();
	for (i = 0; i < pProtocol->apply.size(); ++ i)
	{
		const GNET::GFactionRelationApply &r = pProtocol->apply[i];
		leftTime = r.end_time - serverTime;
		if (GetApplyLineString(r.fid, r.type, leftTime, strText, true))
		{
			pListBox->AddString(strText);
		}
		else
		{
			pListBox->AddString(strText);
			pListBox->SetItemData(pListBox->GetCount()-1, 1, APPLY_OK_INDEX);
		}
		pListBox->SetItemData(pListBox->GetCount()-1, r.fid, APPLY_FACTION_INDEX);
		pListBox->SetItemData(pListBox->GetCount()-1, r.type, APPLY_TYPE_INDEX);
		pListBox->SetItemData(pListBox->GetCount()-1, r.end_time, APPLY_TIME_INDEX);
	}

	//	更新界面按钮状态
	OnEventLButtonDown_List_Apply(0, 0, m_pList_Apply);
}

bool CDlgGuildDiplomacyMan::GetAllianceLineString(int fid, int leftTime, ACString &strText, bool bRequestFromServer)
{
	//	根据帮派 ID 获取同盟帮派列表每行显示的内容
	//	返回 true 表示成功获取帮派信息
	//

	bool bRet(false);

	CECFactionMan *pFMan = GetGame()->GetFactionMan();	
	const Faction_Info *pFInfo = pFMan->GetFaction(fid, bRequestFromServer);
	if (pFInfo)
	{
		ACString strName = pFInfo->GetName();
		if (strName.IsEmpty())
			strName = _AL("-");

		a_ClampFloor(leftTime, 0);
		ACString strTime = GetGameUIMan()->GetFormatTime(leftTime);
		if (strTime.IsEmpty())
			strTime = _AL("-");

		strText.Format(_AL("%s\t%s"), strName, strTime);
		bRet = true;
	}
	else
	{
		strText = _AL("-\t-");
	}
	return bRet;
}

bool CDlgGuildDiplomacyMan::GetHostilityLineString(int fid, int leftTime, ACString &strText, bool bRequestFromServer)
{
	//	根据帮派 ID 获取敌对帮派列表每行显示的内容
	//	返回 true 表示成功获取帮派信息
	//

	//	跟同盟帮派显示一致
	return GetAllianceLineString(fid, leftTime, strText, bRequestFromServer);
}

bool CDlgGuildDiplomacyMan::GetApplyLineString(int fid, int type, int leftTime, ACString &strText, bool bRequestFromServer)
{
	//	根据帮派 ID 获取帮派关系申请列表每行显示的内容
	//	返回 true 表示成功获取帮派信息
	//

	bool bRet(false);

	CECFactionMan *pFMan = GetGame()->GetFactionMan();	
	const Faction_Info *pFInfo = pFMan->GetFaction(fid, bRequestFromServer);
	if (pFInfo)
	{
		ACString strName = pFInfo->GetName();
		if (strName.IsEmpty())
			strName = _AL("-");

		ACString strType = GetStringFromTable(9190 + type);

		a_ClampFloor(leftTime, 0);
		ACString strTime = GetGameUIMan()->GetFormatTime(leftTime);
		if (strTime.IsEmpty())
			strTime = _AL("-");

		strText.Format(_AL("%s\t%s\t%s"), strName, strType, strTime);
		bRet = true;
	}
	else
	{
		strText = _AL("-\t-\t-");
	}
	return bRet;
}


void CDlgGuildDiplomacyMan::OnCommand_KillAlliance(const char *szCommand)
{
	int idFaction = GetFactionAllianceSel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		if (CanSend())
		{
			ACString strMsg = GetStringFromTable(9173);
			PAUIDIALOG pMsgBox = NULL;
			GetGameUIMan()->MessageBox("GDiplomacy_Relation_Remove_Apply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(idFaction);
		}
	}
}

void CDlgGuildDiplomacyMan::OnCommand_Force_KillAlliance(const char *szCommand)
{
	int idFaction = GetFactionAllianceSel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		int nMoneyNeeded = 6000000;
		ACString strMsg;
		if (GetHostPlayer()->GetMoneyAmount() < nMoneyNeeded)
		{
			//	条件不满足
			strMsg.Format(GetStringFromTable(9175), nMoneyNeeded);
			GetGameUIMan()->ShowErrorMsg(strMsg);
		}
		else
		{
			if (CanSend())
			{
				strMsg.Format(GetStringFromTable(9176), nMoneyNeeded);
				PAUIDIALOG pMsgBox = NULL;
				GetGameUIMan()->MessageBox("GDiplomacy_Relation_Remove_Apply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
				pMsgBox->SetData(idFaction);
				pMsgBox->SetDataPtr((void *)1);
			}
		}
	}
}

void CDlgGuildDiplomacyMan::OnCommand_KillHostility(const char *szCommand)
{
	int idFaction = GetFactionHostilitySel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		if (CanSend())
		{
			ACString strMsg = GetStringFromTable(9174);
			PAUIDIALOG pMsgBox = NULL;
			GetGameUIMan()->MessageBox("GDiplomacy_Relation_Remove_Apply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(idFaction);
		}
	}
}

void CDlgGuildDiplomacyMan::OnCommand_Force_KillHostility(const char *szCommand)
{
	int idFaction = GetFactionHostilitySel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		int nMoneyNeeded = 6000000;
		ACString strMsg;
		if (GetHostPlayer()->GetMoneyAmount() < nMoneyNeeded)
		{
			//	条件不满足
			strMsg.Format(GetStringFromTable(9177), nMoneyNeeded);
			GetGameUIMan()->ShowErrorMsg(strMsg);
		}
		else
		{
			if (CanSend())
			{
				strMsg.Format(GetStringFromTable(9178), nMoneyNeeded);
				PAUIDIALOG pMsgBox = NULL;
				GetGameUIMan()->MessageBox("GDiplomacy_Relation_Remove_Apply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
				pMsgBox->SetData(idFaction);
				pMsgBox->SetDataPtr((void *)1);
			}
		}
	}
}

void CDlgGuildDiplomacyMan::OnCommand_Reply_Agree(const char *szCommand)
{
	int idFaction(0);
	int applyType(0);
	if (GetReplyFactionAndType(idFaction, applyType))
	{
		switch(applyType)
		{
		case ALLIANCE_FROM_OTHER:
			{
				//	接受同盟
				int nMoneyNeeded = 3000000;
				ACString strMsg;
				if (GetHostPlayer()->GetMoneyAmount() < nMoneyNeeded)
				{
					//	条件不满足
					strMsg.Format(GetStringFromTable(9179), nMoneyNeeded);
					GetGameUIMan()->ShowErrorMsg(strMsg);
				}
				else
				{
					if (CanSend())
					{
						strMsg.Format(GetStringFromTable(9180), nMoneyNeeded);
						PAUIDIALOG pMsgBox = NULL;
						GetGameUIMan()->MessageBox("GDiplomacy_Reply_Agree", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
						pMsgBox->SetData(idFaction);
						pMsgBox->SetDataPtr((void *)1);
					}
				}
			}
			break;

		case HOSTILE_FROM_OTHER:
			{
				//	接受敌对
					if (CanSend())
					{
						ACString strMsg;
						strMsg.Format(GetStringFromTable(9181));
						PAUIDIALOG pMsgBox = NULL;
						GetGameUIMan()->MessageBox("GDiplomacy_Reply_Agree", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
						pMsgBox->SetData(idFaction);
						pMsgBox->SetDataPtr((void *)0);
					}
			}
			break;

		case REMOVE_RELATION_FROM_OTHER:
			{				
				if (CanSend())
				{
					//	同意解除当前关系
					ACString strMsg = GetStringFromTable(9182);
					PAUIDIALOG pMsgBox = NULL;
					GetGameUIMan()->MessageBox("GDiplomacy_Relation_Remove_Reply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
					pMsgBox->SetData(idFaction);
					pMsgBox->SetDataPtr((void *)1);
				}
			}
			break;
		}
	}
}

void CDlgGuildDiplomacyMan::OnCommand_Reply_Refuse(const char *szCommand)
{
	int idFaction(0);
	int applyType(0);
	if (GetReplyFactionAndType(idFaction, applyType))
	{
		switch(applyType)
		{
		case ALLIANCE_FROM_OTHER:
			{
				if (CanSend())
				{
					ACString strMsg = GetStringFromTable( 9183);
					PAUIDIALOG pMsgBox = NULL;
					GetGameUIMan()->MessageBox("GDiplomacy_Reply_Refuse", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
					pMsgBox->SetData(idFaction);
					pMsgBox->SetDataPtr((void *)1);
				}
			}
			break;

		case HOSTILE_FROM_OTHER:
			{
				//	拒绝敌对
				int nMoneyNeeded = 3000000;
				ACString strMsg;
				if (GetHostPlayer()->GetMoneyAmount() < nMoneyNeeded)
				{
					//	条件不满足
					strMsg.Format(GetStringFromTable(9184), nMoneyNeeded);
					GetGameUIMan()->ShowErrorMsg(strMsg);
				}
				else
				{
					if (CanSend())
					{
						strMsg.Format(GetStringFromTable(9185), nMoneyNeeded);
						PAUIDIALOG pMsgBox = NULL;
						GetGameUIMan()->MessageBox("GDiplomacy_Reply_Refuse", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
						pMsgBox->SetData(idFaction);
						pMsgBox->SetDataPtr((void *)0);
					}
				}
			}
			break;

		case REMOVE_RELATION_FROM_OTHER:
			{
				if (CanSend())
				{
					//	拒绝解除当前关系
					ACString strMsg = GetStringFromTable(9186);
					PAUIDIALOG pMsgBox = NULL;
					GetGameUIMan()->MessageBox("GDiplomacy_Relation_Remove_Reply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
					pMsgBox->SetData(idFaction);
				}
			}
			break;
		}
	}
}

void CDlgGuildDiplomacyMan::OnEventLButtonDown_List_Apply(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//	申请内容变化时，按钮状态有变
	int idFaction(0);
	int applyType(0);
	if (GetReplyFactionAndType(idFaction, applyType))
	{
		m_pBtn_Agree->Enable(true);
		m_pBtn_Refuse->Enable(true);
	}
	else
	{
		m_pBtn_Agree->Enable(false);
		m_pBtn_Refuse->Enable(false);
	}
}

bool CDlgGuildDiplomacyMan::GetReplyFactionAndType(int &applyFaction, int &applyType)
{
	//	获取当前选中的申请帮派及申请类型
	//	当是其他人发给我的申请时，返回true
	bool bRet(false);
	
	if (CanDoAction())
	{
		int nSel = m_pList_Apply->GetCurSel();
		if (nSel >= 0 && nSel < m_pList_Apply->GetCount())
		{
			applyFaction = m_pList_Apply->GetItemData(nSel, APPLY_FACTION_INDEX);
			applyType = m_pList_Apply->GetItemData(nSel, APPLY_TYPE_INDEX);
			switch(applyType)
			{
			case ALLIANCE_FROM_OTHER:
			case HOSTILE_FROM_OTHER:
			case REMOVE_RELATION_FROM_OTHER:
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

int CDlgGuildDiplomacyMan::GetFactionAllianceSel()
{
	//	获取当前选中的帮派 id
	int idFaction(0);
	
	if (CanDoAction())
	{
		int nSel = m_pList_Alliance->GetCurSel();
		if (nSel >= 0 && nSel < m_pList_Alliance->GetCount())
		{
			idFaction = m_pList_Alliance->GetItemData(nSel, ALLIANCE_FACTION_INDEX);
		}
	}

	return idFaction;
}

int CDlgGuildDiplomacyMan::GetFactionHostilitySel()
{
	//	获取当前选中的帮派 id
	int idFaction(0);
	
	if (CanDoAction())
	{
		int nSel = m_pList_Hostility->GetCurSel();
		if (nSel >= 0 && nSel < m_pList_Hostility->GetCount())
		{
			idFaction = m_pList_Hostility->GetItemData(nSel, HOSTILITY_FACTION_INDEX);
		}
	}

	return idFaction;
}

bool CDlgGuildDiplomacyMan::CanDoAction()
{
	//	只有帮主或副帮主可进行实际操作
	int idFRole = GetHostPlayer()->GetFRoleID();
	return (idFRole == GNET::_R_MASTER || idFRole == GNET::_R_VICEMASTER);
}

void CDlgGuildDiplomacyMan::OnCommand_ApplyFaction(const char *szCommand)
{
	//	按帮派名称排序
	if (m_pList_Apply->GetCount() > 0)
	{
		static bool s_bAscent = true;
		m_pList_Apply->SortItems(s_bAscent ? AUILISTBOX_SORT_ASCENT : AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_STRING, 0);
		s_bAscent = !s_bAscent;
	}
}

void CDlgGuildDiplomacyMan::OnCommand_ApplyType(const char *szCommand)
{
	//	按申请类型排序
	if (m_pList_Apply->GetCount() > 0)
	{
		static bool s_bAscent = true;
		m_pList_Apply->SortItemsByData(s_bAscent ? AUILISTBOX_SORT_ASCENT : AUILISTBOX_SORT_DESCENT, AUILISTBOX_DATASORTTYPE_INT, APPLY_TYPE_INDEX);
		s_bAscent = !s_bAscent;
	}
}

void CDlgGuildDiplomacyMan::OnCommand_ApplyEndTime(const char *szCommand)
{
	//	按剩余时间排序
	if (m_pList_Apply->GetCount() > 0)
	{
		static bool s_bAscent = true;
		m_pList_Apply->SortItemsByData(s_bAscent ? AUILISTBOX_SORT_ASCENT : AUILISTBOX_SORT_DESCENT, AUILISTBOX_DATASORTTYPE_INT, APPLY_TIME_INDEX);
		s_bAscent = !s_bAscent;
	}
}

void CDlgGuildDiplomacyMan::OnCommand_DiplomacyApply(const char *szCommand)
{
	CDlgGuildDiplomacyApply *pDlg = GetGameUIMan()->m_pDlgGuildDiplomacyApply;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgGuildDiplomacyMan::OnCommand_Refresh(const char *szCommand)
{
	Refresh();
}

void CDlgGuildDiplomacyMan::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	GetGameUIMan()->m_pDlgGuildDiplomacyApply->Show(false);
}

int CDlgGuildDiplomacyMan::GetLastSendTime()
{
	return m_iLastSendTime;
}

void CDlgGuildDiplomacyMan::SetLastSendTime(int t)
{
	m_iLastSendTime = t;
}

bool CDlgGuildDiplomacyMan::CanSend()
{
	bool bRet(false);
	if (time(NULL) < m_iLastSendTime+5)
	{
		//	检查失败时，顺便显示消息框
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9203));
	}
	else bRet = true;
	return bRet;
}
