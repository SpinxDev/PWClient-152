// Filename	: DlgExp.cpp
// Creator	: Xu Wenbin
// Date		: 2009/12/23

#include "DlgExp.h"
#include "AUIRADIOBUTTON.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "DlgOnlineAward.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgExp, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("confirm", OnCommand_Confirm)
AUI_ON_COMMAND("begin", OnCommand_Begin)
AUI_ON_COMMAND("stop", OnCommand_Stop)
AUI_END_COMMAND_MAP()

bool CDlgExp::OnInitDialog()
{
	m_pTxt_Time = static_cast<PAUILABEL>(GetDlgItem("Txt_Time"));
	m_pTxt_Wait = static_cast<PAUILABEL>(GetDlgItem("Txt_Wait"));
	m_pTxt_Over = static_cast<PAUILABEL>(GetDlgItem("Txt_Over"));
	
	m_pBtn_Begin = static_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Begin"));
	m_pBtn_Stop = static_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Stop"));

	return true;
}

void CDlgExp::OnShowDialog()
{
	int nHour(0), nMinute(0);

	ACString strTextDefault;
	strTextDefault.Format(GetStringFromTable(8500), nHour, nMinute);

	m_pTxt_Time->SetText(strTextDefault);
	m_pTxt_Wait->SetText(strTextDefault);
	m_pTxt_Over->SetText(strTextDefault);

	if (IsUISetExp())
	{
		m_pBtn_Begin->Enable(false);
		m_pBtn_Stop->Enable(false);
	}
}

bool CDlgExp::Tick()
{
	ACString strColor1 = GetStringFromTable(8504); // ��ɫ��ʾ��ǰ״̬
	ACString strColor2 = GetStringFromTable(8500); // ��ɫ��ʾ����״̬

	CECHostPlayer::MultiExpState state = GetHostPlayer()->MultiExp_GetState();

	ACString strTime;
	// ���飨�౶���飩ʱ��
	strTime = state==CECHostPlayer::MES_ENHANCE?strColor1:strColor2;
	strTime += GetGameUIMan()->GetFormatTime(GetHostPlayer()->MultiExp_GetEnhanceTime());
	m_pTxt_Time->SetText(strTime);
	
	// ���飨�౶���飩����ʱ��
	strTime = state==CECHostPlayer::MES_BUFFER?strColor1:strColor2;
	strTime += GetGameUIMan()->GetFormatTime(GetHostPlayer()->MultiExp_GetBufferTime());
	m_pTxt_Wait->SetText(strTime);
	
	// ɢ�飨�౶���飩ʱ��
	strTime = state==CECHostPlayer::MES_IMPAIR?strColor1:strColor2;
	strTime += GetGameUIMan()->GetFormatTime(GetHostPlayer()->MultiExp_GetImpairTime());
	m_pTxt_Over->SetText(strTime);

	if (IsUISetExp())
	{
		// ���鿪������ͣ����
		m_pBtn_Begin->Enable(CanBegin());
		m_pBtn_Stop->Enable(CanStop());
	}

	return CDlgBase::Tick();
}

void CDlgExp::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
}

void CDlgExp::OnCommand_Confirm(const char *szCommand)
{
	int iSel(0);
	AString strName;
	PAUIRADIOBUTTON pRdo_Exp(NULL);
	while (true)
	{
		strName.Format("Rdo_Exp%d", iSel+1);
		pRdo_Exp = static_cast<PAUIRADIOBUTTON>(GetDlgItem(strName));
		if (!pRdo_Exp)
		{
			break;
		}
		if (!pRdo_Exp->IsChecked())
		{
			++ iSel;
			continue;
		}
		const int c_nMaxMultiExpChoice = 20;
		if (iSel >= c_nMaxMultiExpChoice)
			break;

		if (!GetHostPlayer()->MultiExp_CanChooseNow())
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(8505), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			break;
		}

		// ���������Ʒ��Ŀ
		elementdataman* pDB = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		const MULTI_EXP_CONFIG* pConfig = static_cast<const MULTI_EXP_CONFIG*>(pDB->get_data_ptr(701, ID_SPACE_CONFIG, DataType));
		if (!pConfig || DataType != DT_MULTI_EXP_CONFIG)
			break;
		int nNeeded = pConfig->choice[iSel].item_count;

		// ����������Ʒ�Ƿ��㹻
		const int c_idCostItem1 = 27424;
		const int c_idCostItem2 = 27425;
		int nHave1 = GetHostPlayer()->GetPack()->GetItemTotalNum(c_idCostItem1);
		int nHave2 = GetHostPlayer()->GetPack()->GetItemTotalNum(c_idCostItem2);
		int nHave = nHave1+nHave2;
		if (nNeeded>nHave)
		{
			// ��Ʒ��������ʾ��ʾ��
			GetGameUIMan()->MessageBox("", GetStringFromTable(8501), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
		{
			// �����Ի���ȷ��
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_Exp", GetStringFromTable(8502), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(1);
			pMsgBox->SetDataPtr((void *)iSel);
			Show(false);
		}

		break;
	}
}

void CDlgExp::OnCommand_Begin(const char *szCommand)
{
	// ��������
	if (CanBegin())
	{
		if (GetHostPlayer()->MultiExp_GetBeginTimesLeft()<=0)
		{
			// �����޷��ٿ���
			GetGameUIMan()->MessageBox("", GetStringFromTable(8507), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		PAUIDIALOG pMsgBox;
		ACString strMsg;
		strMsg.Format(GetStringFromTable(8506), GetHostPlayer()->MultiExp_GetBeginTimesLeft());
		strMsg += GetStringFromTable(8503);
		GetGameUIMan()->MessageBox("Game_Exp", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(2);
	}
}

void CDlgExp::OnCommand_Stop(const char *szCommand)
{
	// ��ͣ����
	if (CanStop())
	{
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_Exp", GetStringFromTable(8508), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(3);
	}
}

void CDlgExp::OnStateChange(bool firstTime)
{
	if (IsUISetExp())
	{
		if (GetHostPlayer()->MultiExp_StateEmpty())
		{
			// ���ָ���ͨ״̬ʱ�Զ�����
			if (IsShow())
				Show(false);
		}
		else if (!firstTime || !(GetHostPlayer()->MultiExp_EnhanceStateOnly() && GetHostPlayer()->MultiExp_GetState() == CECHostPlayer::MES_NORMAL))
		{
			// �״�����ʱ�����ֻʣ�¾���ʱ����״̬δ����������ʾ��ԭ��Ϊ������˵���鿪��ʱ����١����Ա������Ҹ��ţ�
			// �������Ϊ�л�״̬���£���Ҫ��ʾ
			// �������״̬����ʾ������
			if (!IsShow())
				Show(true);
		}
	}
}

bool CDlgExp::CanBegin()
{
	// ���¿�������ʱ��
	CECHostPlayer::MultiExpState state = GetHostPlayer()->MultiExp_GetState();
	return state!=CECHostPlayer::MES_ENHANCE
		&& GetHostPlayer()->MultiExp_GetEnhanceTime()>0;
}

bool CDlgExp::CanStop()
{
	CECHostPlayer::MultiExpState state = GetHostPlayer()->MultiExp_GetState();
	return state == CECHostPlayer::MES_ENHANCE;
}

bool CDlgExp::IsUISetExp()
{
	return m_szName == "Win_SetExp";
}
