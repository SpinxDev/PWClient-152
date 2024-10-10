#include "DlgFortressBuildSub.h"
#include "DlgFortressBuildSubList.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_HostPlayer.h"
#include "EC_UIConfigs.h"
#include "EC_TimeSafeChecker.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "AUIStillImageButton.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressBuildSub, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Build",	OnCommand_Build)
AUI_ON_COMMAND("Btn_+",		OnCommand_SpeedUp)
AUI_ON_COMMAND("Btn_-",		OnCommand_SlowDown)
AUI_ON_COMMAND("Combo_Candidates", OnCommand_ComboCandidates)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressBuildSub, CDlgBase)
AUI_ON_EVENT("Btn_+",	WM_LBUTTONDOWN,	OnEventLButtonDown_Add)
AUI_ON_EVENT("Btn_-",	WM_LBUTTONDOWN,	OnEventLButtonDown_Sub)
AUI_ON_EVENT("*",		WM_LBUTTONDOWN,	OnEventLButtonDown_)
AUI_END_EVENT_MAP()

CDlgFortressBuildSub::CDlgFortressBuildSub()
{
	m_pEdit_Highlight = NULL;
	m_pCombo_Candidates = NULL;
	m_pLab_Name	= NULL;
	m_pImg_Build = NULL;
	::ZeroMemory(m_pEdit, sizeof(m_pEdit));
	m_pPro_Time = NULL;
	m_pLab_Time = NULL;
	m_pLab_Speed = NULL;
	m_pBtn_Build = NULL;
	m_pBtn_Add	= NULL;
	m_pBtn_Sub = NULL;
	m_pLab_Gold = NULL;

	m_idBuilding = 0;
	m_nFinishTime = 0;
	m_state = BS_INVALID;
	
	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = 0;
	m_dwLastTime = 0;

	m_bSelected = false;
}

bool CDlgFortressBuildSub::OnInitDialog()
{
	DDX_Control("Edit_Highlight", m_pEdit_Highlight);
	DDX_Control("Combo_Candidates", m_pCombo_Candidates);
	DDX_Control("Lab_Name", m_pLab_Name);
	DDX_Control("Img_Build", m_pImg_Build);

	AString strName;
	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		strName.Format("Edit_%02d", i+1);
		DDX_Control(strName, m_pEdit[i]);
	}

	DDX_Control("Pro_Time", m_pPro_Time);
	DDX_Control("Lab_Time", m_pLab_Time);
	DDX_Control("Lab_Speed", m_pLab_Speed);
	DDX_Control("Btn_Build", m_pBtn_Build);
	DDX_Control("Btn_+", m_pBtn_Add);
	DDX_Control("Btn_-", m_pBtn_Sub);
	DDX_Control("Lab_Gold", m_pLab_Gold);

	ResetContent();

	return true;
}

void CDlgFortressBuildSub::OnShowDialog()
{
}

void CDlgFortressBuildSub::OnTick()
{
	switch(GetState())
	{
	case BS_IN_BUILD:
		{			
			//	更新建造进度显示
			UpdateBuildingProcess();
		}
		break;

	case BS_BEFORE_BUILD:
		{
			bool bAdd = m_pBtn_Add->GetState() != AUISTILLIMAGEBUTTON_STATE_NORMAL;
			bool bSub = m_pBtn_Sub->GetState() != AUISTILLIMAGEBUTTON_STATE_NORMAL;
			if (bAdd || bSub)
			{
				DWORD dwCurTime = GetTickCount();
				if (CECTimeSafeChecker::ElapsedTime(dwCurTime, m_dwLastTime) >= m_dwInterval)
				{
					//	实施自动加点
					int nAccelerate = GetAccelerate();
					nAccelerate += bAdd ? 1 : -1;
					SetAccelerate(nAccelerate);

					m_dwLastTime = dwCurTime;

					if (m_dwInterval == INTERVAL_WAIT_BEGIN)
					{
						//	首次加点时间间隔较长，以后较短
						m_dwInterval = INTERVAL_WAIT_NEXT;
					}
				}
			}

			UpdateColor();
		}
		break;
	}

	//	鼠标经过或离开时、更新对话框被选中时的高亮显示
	PAUIDIALOG pDlgMouseOn;
	PAUIOBJECT pObjMouseOn;
	m_pAUIManager->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);
	SetHighlight(pDlgMouseOn == this);	//	被选中时不受鼠标经过影响（内部已处理）
}

void CDlgFortressBuildSub::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgFortressBuildSub::ResetContent()
{
	//	清除当前所有设施相关内容
	//

	//	重置关于单个设施的界面
	ResetBuilding();

	//	重置关于多个设施的界面
	m_pCombo_Candidates->ResetContent();
	m_pCombo_Candidates->Enable(false);
}

void CDlgFortressBuildSub::SetCandidates(const CandidateList &candidates)
{
	//	此类型设施当前没有已建者，界面默认显示第一个可建，同时把其它设施设置为可选值
	//

	ResetContent();
	
	//	添加候选设施
	AppendCandidates(candidates);
	
	//	选中默认设施
	if (m_pCombo_Candidates->GetCount())
	{
		int nDefault = FindDefaultSel();
		m_pCombo_Candidates->SetCurSel(nDefault);
		OnCommand_ComboCandidates(NULL);
	}
	
	//	若还有其它候选设施，则显示选项供选择
	if (m_pCombo_Candidates->GetCount()>1)
		m_pCombo_Candidates->Enable(true);
}

void CDlgFortressBuildSub::SetBuildingAndCandidates(int id, int finish_time, const CandidateList &candidates)
{
	//	此类型设施有某个在建、或已建者，界面显示此在建\已建，在设施已建成时把其它设施设置为可选值
	//

	ResetContent();

	//	添加候选设施
	AppendCandidates(candidates);

	//	添加已建\在建设施
	AppendCandidate(id, finish_time);

	//	选中默认设施
	if (m_pCombo_Candidates->GetCount())
	{
		int nDefault = FindDefaultSel();
		m_pCombo_Candidates->SetCurSel(nDefault);
		OnCommand_ComboCandidates(NULL);
	}
	
	//	若还有其它候选设施，则显示选项供选择
	if (m_pCombo_Candidates->GetCount()>1)
		m_pCombo_Candidates->Enable(true);
}

void CDlgFortressBuildSub::AppendCandidates(const CandidateList &candidates)
{
	for (CandidateList::const_iterator it = candidates.begin(); it != candidates.end(); ++ it)
		AppendCandidate(*it, -1);
}

void CDlgFortressBuildSub::AppendCandidate(int idBuilding, int nFinishTime)
{
	//	向当前候选列表中增加一项（已建\在建设施作为特殊情况也包含在内）
	//

	ACString strName;

	CECHostPlayer *pHost = GetHostPlayer();
	const FACTION_BUILDING_ESSENCE *pEssence = pHost->GetBuildingEssence(idBuilding);
	if (pEssence)
	{
		strName = pEssence->name;
		if (strName.IsEmpty())
			strName.Format(_AL("(Empty Name:%d)"), idBuilding);
	}
	else
	{
		strName.Format(_AL("(Error item:%d)"), idBuilding);
	}

	m_pCombo_Candidates->AddString(strName);
	m_pCombo_Candidates->SetItemData(m_pCombo_Candidates->GetCount()-1, idBuilding);
	m_pCombo_Candidates->SetItemDataPtr(m_pCombo_Candidates->GetCount()-1, (void *)nFinishTime);
}

void CDlgFortressBuildSub::ResetBuilding(int id /* = 0 */, int finish_time /* = -1 */)
{
	//	以指定设施更新界面内容，也可用来重置关于单个设施的界面
	//

	//	清除旧设施的相关信息
	m_pEdit_Highlight->Show(false);
	m_pLab_Name->SetText(_AL(""));
	m_pImg_Build->SetCover(NULL, -1);

	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		m_pEdit[i]->SetText(_AL(""));

	m_pLab_Time->SetText(_AL(""));
	m_pPro_Time->SetProgress(0);
	m_pLab_Speed->SetText(_AL("1.0"));
	m_pLab_Speed->SetData(0);

	m_pBtn_Build->SetText(GetStringFromTable(9270));
	m_pBtn_Build->Enable(false);
	m_pBtn_Add->Enable(false);
	m_pBtn_Sub->Enable(false);

	m_pLab_Gold->SetText(_AL(""));

	m_idBuilding = 0;
	m_nFinishTime = 0;
	m_state	= BS_INVALID;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = 0;
	m_dwLastTime = 0;
	
	//	设置新设施
	
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(id);
	if (!pInfo || !pEssence)
	{
		UpdateColor();
		return;
	}

	//	设置名称
	m_pLab_Name->SetText(pEssence->name);

	//	设置图标
	AString strFile;	
	af_GetFileTitle(pEssence->file_icon, strFile);
	strFile.MakeLower();
	m_pImg_Build->SetCover(
		GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_idBuilding = id;
	m_nFinishTime = finish_time;

	if (id > 0)
	{
		if (finish_time >= 0)
		{
			//	已建或在建

			m_pBtn_Build->SetText(GetStringFromTable(9271));

			int serverTime = GetGame()->GetServerGMTTime();
			if (serverTime < finish_time)
			{
				//	还在建造中
				m_state = BS_IN_BUILD;
			}
			else
			{
				//	已建成
				m_state = BS_AFTER_BUILD;
				if (CECUIConfig::Instance().GetGameUI().bEnableFortressBuildDestroy)
					m_pBtn_Build->Enable(true);
			}
		}
		else
		{
			//	尚未建造
			m_state = BS_BEFORE_BUILD;
			m_pBtn_Build->Enable(true);
			m_pBtn_Add->Enable(true);
			m_pBtn_Sub->Enable(true);
			SetAccelerate(0);
		}

		//	更新建造进度显示
		UpdateBuildingProcess();
	}

	//  设置消耗金钱
	if (GetState() != BS_AFTER_BUILD)
		m_pLab_Gold->SetText(GetGameUIMan()->GetFormatNumber(pEssence->money));

	UpdateColor();
}

void CDlgFortressBuildSub::OnCommand_Build(const char *szCommand)
{
	if (GetState() == BS_BEFORE_BUILD)
	{
		const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(m_idBuilding);
		const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
		if (!pEssence || !pInfo)
			return;

		if (pEssence->money > GetHostPlayer()->GetMoneyAmount())
		{
			//	金钱不足
			GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9122));
			return;
		}
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		{
			int nMaterialNeed = a_atoi(m_pEdit[i]->GetText());
			if (nMaterialNeed > pInfo->material[i])
			{
				//	材料不足
				GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9123));
				return;
			}
		}

		//	显示确认建造信息，以免误点导致长时间建造
		ACString strLeftTime = m_pLab_Time->GetText();
		ACString strMsg;
		strMsg.Format(GetStringFromTable(9126), strLeftTime, GetMaterialToReturn());
		PAUIDIALOG pMsgBox(NULL);
		GetGameUIMan()->MessageBox("Fortress_Build", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(m_idBuilding);
		pMsgBox->SetDataPtr((void *)GetAccelerate());
		return;
	}

	if (GetState() == BS_AFTER_BUILD && CECUIConfig::Instance().GetGameUI().bEnableFortressBuildDestroy)
	{
		//	回收
		ACString strMsg;
		strMsg.Format(GetStringFromTable(9127), GetMaterialToReturn());
		
		PAUIDIALOG pMsgBox(NULL);
		GetGameUIMan()->MessageBox("Fortress_Destroy", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(m_idBuilding);
	}
}

void CDlgFortressBuildSub::OnCommand_SpeedUp(const char *szCommand)
{
	if (GetState() != BS_BEFORE_BUILD)
		return;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;

	int nAccelerate = GetAccelerate();
	SetAccelerate(nAccelerate+1);
}

void CDlgFortressBuildSub::OnCommand_SlowDown(const char *szCommand)
{
	if (GetState() != BS_BEFORE_BUILD)
		return;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;

	int nAccelerate = GetAccelerate();
	SetAccelerate(nAccelerate-1);
}

void CDlgFortressBuildSub::OnCommand_ComboCandidates(const char *szCommand)
{
	int nSel = m_pCombo_Candidates->GetCurSel();
	if (nSel >= 0 && nSel < m_pCombo_Candidates->GetCount())
	{
		int idBuilding = (int)m_pCombo_Candidates->GetItemData(nSel);
		int nFinishTime = (int)m_pCombo_Candidates->GetItemDataPtr(nSel);
		ResetBuilding(idBuilding, nFinishTime);
	}
}

void CDlgFortressBuildSub::OnEventLButtonDown_Add(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	//	所有事件处理程序都需要处理此消息，以正确处理高亮
	OnEventLButtonDown_(wParam, lParam, pObj);

	if (GetState() != BS_BEFORE_BUILD)
		return;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
}

void CDlgFortressBuildSub::OnEventLButtonDown_Sub(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	//	所有事件处理程序都需要处理此消息，以正确处理高亮
	OnEventLButtonDown_(wParam, lParam, pObj);

	if (GetState() != BS_BEFORE_BUILD)
		return;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
}

void CDlgFortressBuildSub::OnEventLButtonDown_(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	//	设置当前对话框为选中状态（高亮显示）
	//
	CDlgFortressBuildSubList *pDlg = (CDlgFortressBuildSubList *)GetGameUIMan()->GetDialog("Win_FortressBuildSubList");
	if (pDlg)
		pDlg->SelectBuilding(this);
}

int CDlgFortressBuildSub::GetAccelerate()
{
	return m_pLab_Speed->GetData();
}

bool CDlgFortressBuildSub::SetAccelerate(int nNewAccel)
{
	//	设置建造速度
	//	参数 nNewAccel : 0 原始速度, 1 加快10%，以此类推
	bool bOK(false);
	
	if (GetState() != BS_BEFORE_BUILD)
		return false;
	
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(m_idBuilding);
	while (true)
	{
		if (!pInfo)
		{
			//	无法获取材料信息
			break;
		}

		if (!pEssence)
		{
			//	无法获取设施模板及加速信息
			break;
		}

		if (pEssence->base_time <= 0)
		{
			//	不需要加速
			break;
		}

		if (pEssence->delta_time <= 0)
		{
			//	加速时间设置有问题，不会有加速效果，出于保护玩家财产目的，禁止修改
			break;
		}

		//	根据加速时间计算最大建造加速
		int nMaxAccel1 = pEssence->base_time/pEssence->delta_time;
		if (pEssence->base_time % pEssence->delta_time)
			nMaxAccel1 ++;

		//	根据消耗材料计算最大建造加速
		int nMaxAccel2 = nMaxAccel1;
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		{
			if (pEssence->material[i] > 0)
			{
				//	计算一成的材料数
				int tenPercentMaterial = pEssence->material[i]/10;
				if (pEssence->material[i]%10)
					tenPercentMaterial ++;
				int temp = (pInfo->material[i] - pEssence->material[i]) / tenPercentMaterial;
				a_ClampFloor(temp, 0);		//	有可能身上的材料数少于需要材料数，这时候显示为1.0，但实际上材料数是不足的（会以红色显示并禁止建造）
				if (temp < nMaxAccel2)
				{
					//	最少材料比限制最大加速值
					nMaxAccel2 = temp;
				}
			}
		}
		
		//	直接限制加速度
		int nMaxAccel3 = 20;

		//	限制加速度
		int nMaxAccel = a_Min(nMaxAccel1, nMaxAccel2);
		nMaxAccel = a_Min(nMaxAccel, nMaxAccel3);
		a_Clamp(nNewAccel, 0, nMaxAccel);
		m_pLab_Speed->SetData(nNewAccel);
		bOK = true;
		break;
	}
	
	if (pEssence && pInfo)
	{
		//	显示材料消耗
		int nAccelerate = GetAccelerate();
		
		ACString strText;
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		{
			int nMaterialSpeedup = pEssence->material[i] * nAccelerate;
			nMaterialSpeedup = (nMaterialSpeedup/10) + ((nMaterialSpeedup % 10) ? 1 : 0);
			int nMaterial = pEssence->material[i] + nMaterialSpeedup;
			strText.Format(_AL("%d"), nMaterial);
			m_pEdit[i]->SetText(strText);
		}
		
		//	显示加速倍数
		float fAccelerate = 1.0f + nAccelerate*0.1f;
		strText.Format(_AL("%2.1f"), fAccelerate);
		m_pLab_Speed->SetText(strText);

		//	显示加速后建造时间及进度
		int timeLeft = pEssence->base_time;
		if (pEssence->base_time > 0 && pEssence->delta_time > 0)
		{
			timeLeft -= pEssence->delta_time * nAccelerate;
			a_ClampFloor(timeLeft, 0);
		}
		ShowTimeLeft(timeLeft);
		ShowProgress(timeLeft, pEssence->base_time);
	}

	return bOK;
}

void CDlgFortressBuildSub::ShowTimeLeft(int timeLeft)
{
	ACString strText = GetGameUIMan()->GetFormatTime(timeLeft);
	m_pLab_Time->SetText(strText);
}

void CDlgFortressBuildSub::ShowProgress(int timeLeft, int timeAll)
{
	int nProgress = 0;
	if (timeAll <= 0)
	{
		//	建造总时间为0，即立刻完成
		nProgress = 100;
	}
	else
	{
		int timePassed = timeAll-timeLeft;
		nProgress = (int)(timePassed*100/(float)timeAll);
	}
	a_Clamp(nProgress, 0, 100);
	m_pPro_Time->SetProgress(nProgress);
}

void CDlgFortressBuildSub::UpdateBuildingProcess()
{
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(m_idBuilding);
	if (!pEssence || m_state == BS_INVALID)
	{
		m_pPro_Time->SetProgress(0);
		m_pLab_Time->SetText(_AL(""));
		return;
	}

	switch(m_state)
	{
	case BS_BEFORE_BUILD:
		//	建造前的进度显示由 SetAccelerate 处理
		break;

	case BS_AFTER_BUILD:
		//	建造后的进度显示很直接
		m_pPro_Time->SetProgress(100);
		m_pLab_Time->SetText(_AL(""));	//	剩余时间在建造后（服务器决定）不显示，以区分本地判断完成与服务器完成
		break;

	case BS_IN_BUILD:
		//	建造中进度显示
		{
			int serverTime = GetGame()->GetServerGMTTime();
			if (serverTime >= m_nFinishTime)
			{
				//	已完成，按建成后显示
				m_pPro_Time->SetProgress(100);
				ShowTimeLeft(0);
			}
			else
			{
				int timeLeft = m_nFinishTime-serverTime;
				ShowTimeLeft(timeLeft);
				ShowProgress(timeLeft, pEssence->base_time);
			}
		}
		break;
	}
}

bool CDlgFortressBuildSub::GetSelect()
{
	return m_bSelected;
}

void CDlgFortressBuildSub::SetSelect(bool bSelect)
{
	if (m_bSelected != bSelect)
	{
		m_bSelected = bSelect;

		//	选中时自然高亮显示，不选中时鼠标在别处，自然也不高亮显示
		SetHighlight(bSelect);
	}
}

void CDlgFortressBuildSub::SetHighlight(bool bHighlight)
{
	if (m_pEdit_Highlight->IsShow() != bHighlight)
	{
		if (!bHighlight && GetSelect())
		{
			//	被选中时应始终高亮显示
			return;
		}
		m_pEdit_Highlight->Show(bHighlight);
	}
}

bool CDlgFortressBuildSub::GetHighlight()
{
	return m_pEdit_Highlight->IsShow();
}

void CDlgFortressBuildSub::UpdateColor()
{
	//	为当前可建造设施更新材料、金钱的颜色，以正确反应条件的满足情况
	//	
	A3DCOLOR COLOR_WHITE = A3DCOLORRGB(255, 255, 255);
	A3DCOLOR COLOR_RED = A3DCOLORRGB(255, 0, 0);

	//	检查是否为空条件
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(m_idBuilding);
	if (!pInfo || !pEssence || GetState() != BS_BEFORE_BUILD)
	{
		m_pLab_Gold->SetColor(COLOR_WHITE);
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
			m_pEdit[i]->SetColor(COLOR_WHITE);
		return;
	}


	//	钱数颜色
	CECHostPlayer *pHost = GetHostPlayer();
	m_pLab_Gold->SetColor(pHost->GetMoneyAmount() >= pEssence->money ? COLOR_WHITE : COLOR_RED);

	//	材料颜色
	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		int nMaterialNeed = a_atoi(m_pEdit[i]->GetText());
		m_pEdit[i]->SetColor(pInfo->material[i] >= nMaterialNeed ? COLOR_WHITE : COLOR_RED);
	}
}

ACString CDlgFortressBuildSub::GetMaterialToReturn()
{
	//	查询已建成设施并显示其回收时返回材料数
	ACString str;
	
	int idBuilding(0);
	
	int serverTime = GetGame()->GetServerGMTTime();
	for (int i = 0; i < m_pCombo_Candidates->GetCount(); ++ i)
	{
		int nFinishTime = (int)m_pCombo_Candidates->GetItemDataPtr(i);
		if (nFinishTime >= 0 && serverTime >= nFinishTime)		//	nFinishTime 等于 0 时，表示已建造完成
		{
			idBuilding = (int)m_pCombo_Candidates->GetItemData(i);
			break;
		}
	}

	if (idBuilding > 0)
	{
		const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(idBuilding);
		if (pEssence)
		{
			ACString strText, strTemp;
			for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
			{
				int nMaterialReturn = pEssence->material[i]/2;
				if (nMaterialReturn > 0)
				{
					strTemp.Format(_AL("%s(^FF0000%d^FFFFFF)"), GetStringFromTable(9130+i), nMaterialReturn);
					if (!strText.IsEmpty())
						strText += _AL(" ");
					strText += strTemp;
				}
			}
			if (!strText.IsEmpty())
				str.Format(GetStringFromTable(9129), strText);
		}
	}

	return str;
}

int CDlgFortressBuildSub::FindDefaultSel()
{
	//	查找当前设施中默认者
	//	规则：若有已建或在建设施，则选为默认；否则，选为可建最低等级
	//	
	int nDefault = -1;
	
	int nFinishTime(0), idBuilding(0);
	const FACTION_BUILDING_ESSENCE *pEssence = NULL;
	int nMinLevel(INT_MAX);
	for (int i = 0; i < m_pCombo_Candidates->GetCount(); ++ i)
	{
		nFinishTime = (int)m_pCombo_Candidates->GetItemDataPtr(i);
		if (nFinishTime >= 0)
		{
			//	找到已建或在建者
			nDefault = i;
			break;
		}
		idBuilding = (int)m_pCombo_Candidates->GetItemData(i);
		pEssence = GetHostPlayer()->GetBuildingEssence(idBuilding);
		if (pEssence && pEssence->level<nMinLevel)
		{
			nMinLevel = pEssence->level;
			nDefault = i;
		}
	}

	return nDefault;
}