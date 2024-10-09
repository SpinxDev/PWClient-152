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
			//	���½��������ʾ
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
					//	ʵʩ�Զ��ӵ�
					int nAccelerate = GetAccelerate();
					nAccelerate += bAdd ? 1 : -1;
					SetAccelerate(nAccelerate);

					m_dwLastTime = dwCurTime;

					if (m_dwInterval == INTERVAL_WAIT_BEGIN)
					{
						//	�״μӵ�ʱ�����ϳ����Ժ�϶�
						m_dwInterval = INTERVAL_WAIT_NEXT;
					}
				}
			}

			UpdateColor();
		}
		break;
	}

	//	��꾭�����뿪ʱ�����¶Ի���ѡ��ʱ�ĸ�����ʾ
	PAUIDIALOG pDlgMouseOn;
	PAUIOBJECT pObjMouseOn;
	m_pAUIManager->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);
	SetHighlight(pDlgMouseOn == this);	//	��ѡ��ʱ������꾭��Ӱ�죨�ڲ��Ѵ���
}

void CDlgFortressBuildSub::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgFortressBuildSub::ResetContent()
{
	//	�����ǰ������ʩ�������
	//

	//	���ù��ڵ�����ʩ�Ľ���
	ResetBuilding();

	//	���ù��ڶ����ʩ�Ľ���
	m_pCombo_Candidates->ResetContent();
	m_pCombo_Candidates->Enable(false);
}

void CDlgFortressBuildSub::SetCandidates(const CandidateList &candidates)
{
	//	��������ʩ��ǰû���ѽ��ߣ�����Ĭ����ʾ��һ���ɽ���ͬʱ��������ʩ����Ϊ��ѡֵ
	//

	ResetContent();
	
	//	��Ӻ�ѡ��ʩ
	AppendCandidates(candidates);
	
	//	ѡ��Ĭ����ʩ
	if (m_pCombo_Candidates->GetCount())
	{
		int nDefault = FindDefaultSel();
		m_pCombo_Candidates->SetCurSel(nDefault);
		OnCommand_ComboCandidates(NULL);
	}
	
	//	������������ѡ��ʩ������ʾѡ�ѡ��
	if (m_pCombo_Candidates->GetCount()>1)
		m_pCombo_Candidates->Enable(true);
}

void CDlgFortressBuildSub::SetBuildingAndCandidates(int id, int finish_time, const CandidateList &candidates)
{
	//	��������ʩ��ĳ���ڽ������ѽ��ߣ�������ʾ���ڽ�\�ѽ�������ʩ�ѽ���ʱ��������ʩ����Ϊ��ѡֵ
	//

	ResetContent();

	//	��Ӻ�ѡ��ʩ
	AppendCandidates(candidates);

	//	����ѽ�\�ڽ���ʩ
	AppendCandidate(id, finish_time);

	//	ѡ��Ĭ����ʩ
	if (m_pCombo_Candidates->GetCount())
	{
		int nDefault = FindDefaultSel();
		m_pCombo_Candidates->SetCurSel(nDefault);
		OnCommand_ComboCandidates(NULL);
	}
	
	//	������������ѡ��ʩ������ʾѡ�ѡ��
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
	//	��ǰ��ѡ�б�������һ��ѽ�\�ڽ���ʩ��Ϊ�������Ҳ�������ڣ�
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
	//	��ָ����ʩ���½������ݣ�Ҳ���������ù��ڵ�����ʩ�Ľ���
	//

	//	�������ʩ�������Ϣ
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
	
	//	��������ʩ
	
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(id);
	if (!pInfo || !pEssence)
	{
		UpdateColor();
		return;
	}

	//	��������
	m_pLab_Name->SetText(pEssence->name);

	//	����ͼ��
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
			//	�ѽ����ڽ�

			m_pBtn_Build->SetText(GetStringFromTable(9271));

			int serverTime = GetGame()->GetServerGMTTime();
			if (serverTime < finish_time)
			{
				//	���ڽ�����
				m_state = BS_IN_BUILD;
			}
			else
			{
				//	�ѽ���
				m_state = BS_AFTER_BUILD;
				if (CECUIConfig::Instance().GetGameUI().bEnableFortressBuildDestroy)
					m_pBtn_Build->Enable(true);
			}
		}
		else
		{
			//	��δ����
			m_state = BS_BEFORE_BUILD;
			m_pBtn_Build->Enable(true);
			m_pBtn_Add->Enable(true);
			m_pBtn_Sub->Enable(true);
			SetAccelerate(0);
		}

		//	���½��������ʾ
		UpdateBuildingProcess();
	}

	//  �������Ľ�Ǯ
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
			//	��Ǯ����
			GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9122));
			return;
		}
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		{
			int nMaterialNeed = a_atoi(m_pEdit[i]->GetText());
			if (nMaterialNeed > pInfo->material[i])
			{
				//	���ϲ���
				GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9123));
				return;
			}
		}

		//	��ʾȷ�Ͻ�����Ϣ��������㵼�³�ʱ�佨��
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
		//	����
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
	//	�����¼����������Ҫ�������Ϣ������ȷ�������
	OnEventLButtonDown_(wParam, lParam, pObj);

	if (GetState() != BS_BEFORE_BUILD)
		return;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
}

void CDlgFortressBuildSub::OnEventLButtonDown_Sub(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	//	�����¼����������Ҫ�������Ϣ������ȷ�������
	OnEventLButtonDown_(wParam, lParam, pObj);

	if (GetState() != BS_BEFORE_BUILD)
		return;

	m_dwInterval = INTERVAL_WAIT_BEGIN;
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
}

void CDlgFortressBuildSub::OnEventLButtonDown_(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	//	���õ�ǰ�Ի���Ϊѡ��״̬��������ʾ��
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
	//	���ý����ٶ�
	//	���� nNewAccel : 0 ԭʼ�ٶ�, 1 �ӿ�10%���Դ�����
	bool bOK(false);
	
	if (GetState() != BS_BEFORE_BUILD)
		return false;
	
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(m_idBuilding);
	while (true)
	{
		if (!pInfo)
		{
			//	�޷���ȡ������Ϣ
			break;
		}

		if (!pEssence)
		{
			//	�޷���ȡ��ʩģ�弰������Ϣ
			break;
		}

		if (pEssence->base_time <= 0)
		{
			//	����Ҫ����
			break;
		}

		if (pEssence->delta_time <= 0)
		{
			//	����ʱ�����������⣬�����м���Ч�������ڱ�����ҲƲ�Ŀ�ģ���ֹ�޸�
			break;
		}

		//	���ݼ���ʱ�������������
		int nMaxAccel1 = pEssence->base_time/pEssence->delta_time;
		if (pEssence->base_time % pEssence->delta_time)
			nMaxAccel1 ++;

		//	�������Ĳ��ϼ�����������
		int nMaxAccel2 = nMaxAccel1;
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		{
			if (pEssence->material[i] > 0)
			{
				//	����һ�ɵĲ�����
				int tenPercentMaterial = pEssence->material[i]/10;
				if (pEssence->material[i]%10)
					tenPercentMaterial ++;
				int temp = (pInfo->material[i] - pEssence->material[i]) / tenPercentMaterial;
				a_ClampFloor(temp, 0);		//	�п������ϵĲ�����������Ҫ����������ʱ����ʾΪ1.0����ʵ���ϲ������ǲ���ģ����Ժ�ɫ��ʾ����ֹ���죩
				if (temp < nMaxAccel2)
				{
					//	���ٲ��ϱ�����������ֵ
					nMaxAccel2 = temp;
				}
			}
		}
		
		//	ֱ�����Ƽ��ٶ�
		int nMaxAccel3 = 20;

		//	���Ƽ��ٶ�
		int nMaxAccel = a_Min(nMaxAccel1, nMaxAccel2);
		nMaxAccel = a_Min(nMaxAccel, nMaxAccel3);
		a_Clamp(nNewAccel, 0, nMaxAccel);
		m_pLab_Speed->SetData(nNewAccel);
		bOK = true;
		break;
	}
	
	if (pEssence && pInfo)
	{
		//	��ʾ��������
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
		
		//	��ʾ���ٱ���
		float fAccelerate = 1.0f + nAccelerate*0.1f;
		strText.Format(_AL("%2.1f"), fAccelerate);
		m_pLab_Speed->SetText(strText);

		//	��ʾ���ٺ���ʱ�估����
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
		//	������ʱ��Ϊ0�����������
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
		//	����ǰ�Ľ�����ʾ�� SetAccelerate ����
		break;

	case BS_AFTER_BUILD:
		//	�����Ľ�����ʾ��ֱ��
		m_pPro_Time->SetProgress(100);
		m_pLab_Time->SetText(_AL(""));	//	ʣ��ʱ���ڽ���󣨷���������������ʾ�������ֱ����ж��������������
		break;

	case BS_IN_BUILD:
		//	�����н�����ʾ
		{
			int serverTime = GetGame()->GetServerGMTTime();
			if (serverTime >= m_nFinishTime)
			{
				//	����ɣ������ɺ���ʾ
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

		//	ѡ��ʱ��Ȼ������ʾ����ѡ��ʱ����ڱ𴦣���ȻҲ��������ʾ
		SetHighlight(bSelect);
	}
}

void CDlgFortressBuildSub::SetHighlight(bool bHighlight)
{
	if (m_pEdit_Highlight->IsShow() != bHighlight)
	{
		if (!bHighlight && GetSelect())
		{
			//	��ѡ��ʱӦʼ�ո�����ʾ
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
	//	Ϊ��ǰ�ɽ�����ʩ���²��ϡ���Ǯ����ɫ������ȷ��Ӧ�������������
	//	
	A3DCOLOR COLOR_WHITE = A3DCOLORRGB(255, 255, 255);
	A3DCOLOR COLOR_RED = A3DCOLORRGB(255, 0, 0);

	//	����Ƿ�Ϊ������
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(m_idBuilding);
	if (!pInfo || !pEssence || GetState() != BS_BEFORE_BUILD)
	{
		m_pLab_Gold->SetColor(COLOR_WHITE);
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
			m_pEdit[i]->SetColor(COLOR_WHITE);
		return;
	}


	//	Ǯ����ɫ
	CECHostPlayer *pHost = GetHostPlayer();
	m_pLab_Gold->SetColor(pHost->GetMoneyAmount() >= pEssence->money ? COLOR_WHITE : COLOR_RED);

	//	������ɫ
	for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		int nMaterialNeed = a_atoi(m_pEdit[i]->GetText());
		m_pEdit[i]->SetColor(pInfo->material[i] >= nMaterialNeed ? COLOR_WHITE : COLOR_RED);
	}
}

ACString CDlgFortressBuildSub::GetMaterialToReturn()
{
	//	��ѯ�ѽ�����ʩ����ʾ�����ʱ���ز�����
	ACString str;
	
	int idBuilding(0);
	
	int serverTime = GetGame()->GetServerGMTTime();
	for (int i = 0; i < m_pCombo_Candidates->GetCount(); ++ i)
	{
		int nFinishTime = (int)m_pCombo_Candidates->GetItemDataPtr(i);
		if (nFinishTime >= 0 && serverTime >= nFinishTime)		//	nFinishTime ���� 0 ʱ����ʾ�ѽ������
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
	//	���ҵ�ǰ��ʩ��Ĭ����
	//	���������ѽ����ڽ���ʩ����ѡΪĬ�ϣ�����ѡΪ�ɽ���͵ȼ�
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
			//	�ҵ��ѽ����ڽ���
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