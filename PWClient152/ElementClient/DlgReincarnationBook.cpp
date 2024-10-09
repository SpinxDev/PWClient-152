// Filename	: DlgReincarnationBook.cpp
// Creator	: Han Guanghui
// Date		: 2013/08/06

#include "DlgReincarnationBook.h"
#include "DlgReincarnationRewrite.h"
#include "AUIImagePicture.h"
#include "A2DSprite.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_UIHelper.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgReincarnationBook, CDlgBase)
AUI_ON_COMMAND("Btn_Wake",		OnCommandConfirm)
AUI_ON_COMMAND("Btn_Rewrite*",	OnCommandRewrite)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgReincarnationBook::CDlgReincarnationBook()
{

}

CDlgReincarnationBook::~CDlgReincarnationBook()
{
}

bool CDlgReincarnationBook::OnInitDialog()
{
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Wake"));
	if (pImg && pImg->GetImage())
		pImg->GetImage()->SetLinearFilter(true);
	return true;
}

void CDlgReincarnationBook::OnCommandConfirm(const char* szCommand)
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost) return;
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	GetGameSession()->c2s_CmdActivateReincarnationTome(1 - tome.tome_active);
}
void CDlgReincarnationBook::OnCommandRewrite(const char* szCommand)
{
	if (GetDlgItem(szCommand) == NULL) return;
	int rewrite_index;
	sscanf(szCommand, "Btn_Rewrite%d", &rewrite_index);
	rewrite_index--;
	CDlgReincarnationRewrite* pDlg = GetGameUIMan()->m_pDlgReincarnationRewrite;
	pDlg->SetData(rewrite_index);
	pDlg->Show(true);
	pDlg->SetPosEx(GetPos().x, GetPos().y);
	Show(false);
}
void CDlgReincarnationBook::OnShowDialog()
{
	CDlgReincarnationRewrite* pDlg = GetGameUIMan()->m_pDlgReincarnationRewrite;
	if (pDlg->IsShow) {
		pDlg->Show(false);
	}
	GetGameSession()->c2s_CmdGetReincarnationTome();
	Update();
}
void CDlgReincarnationBook::OnCommandCancel(const char *szCommand)
{
	Show(false);
}
void CDlgReincarnationBook::Update()
{
	PAUIOBJECT pObj;
	ACString strText;
	AString strTemp;
	tm birth_time;
	long ltime;
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost) return;
	bool bReach105EveryTime(true);
	pObj = GetDlgItem("Txt_Reincarnation");
	if (pObj) {
		ltime = pHost->GetRoleCreateTime();
		birth_time = *localtime(&ltime);
		strText.Format(GetStringFromTable(10811), birth_time.tm_year + 1900, 
			birth_time.tm_mon + 1, birth_time.tm_mday, pHost->GetName());
		pObj->SetText(strText);
	}
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	for (unsigned int i = 0; i < tome.reincarnations.size(); ++i) {
		strTemp.Format("Txt_Reincar%d", i + 1);
		pObj = GetDlgItem(strTemp);
		if(pObj) {
			ltime = tome.reincarnations[i].timestamp;
			birth_time = *localtime(&ltime);
			strText.Format(GetStringFromTable(10812), birth_time.tm_year + 1900, 
				birth_time.tm_mon + 1, birth_time.tm_mday, 
				pHost->GetName(), tome.reincarnations[i].level, i + 1);
			pObj->SetText(strText);
			pObj->Show(true);
		}
		strTemp.Format("Btn_Rewrite%d", i + 1);
		pObj = GetDlgItem(strTemp);
		if (pObj) {
			pObj->Show(true);
			pObj->Enable(tome.reincarnations[i].level < 105);
		}
		if (tome.reincarnations[i].level < 105) bReach105EveryTime = false;
	}
	while (true){
		strTemp.Format("Txt_Reincar%d", i + 1);
		pObj = GetDlgItem(strTemp);
		if (pObj) pObj->Show(false);
		else break;
		strTemp.Format("Btn_Rewrite%d", i + 1);
		pObj = GetDlgItem(strTemp);
		if (pObj) pObj->Show(false);
		else break;
		++i;
	}
	
	pObj = GetDlgItem("Img_Wake");
	UpdateWakeImg(pObj);

	pObj = GetDlgItem("Btn_Wake");
	UpdateSleepWakeBtn(pObj);
	
	pObj = GetDlgItem("Txt_HighLevel");
	UpdateHistoryMaxLv(pObj);

	pObj = GetDlgItem("Lbl_BookExp");
	UpdateLblBookExp(pObj);

	pObj = GetDlgItem("Txt_BookExp");
	UpdateTxtBookExp(pObj);
	
}

void CDlgReincarnationBook::UpdateSleepWakeBtn(AUIObject* pObj) {
	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	bool bReach105EveryTime = true;
	for (unsigned int i = 0; i < tome.reincarnations.size(); ++i) {
		if (tome.reincarnations[i].level < 105) {
			bReach105EveryTime = false;
			break;
		}
	}
	if (pObj) {
		ACString strReason;
		ACString strText;
		pObj->SetText(CECUIHelper::GetGameUIMan()->GetStringFromTable(10814 + tome.tome_active));
		
		bool bEnable = false;
		int iLevel = pHost->GetBasicProps().iLevel;
		
		if (iLevel<100)
			strReason = CECUIHelper::GetGameUIMan()->GetStringFromTable(10816);
		else {
			bEnable = true;
			if (iLevel>=105)
				strReason = CECUIHelper::GetGameUIMan()->GetStringFromTable(11166);
		}	
		
		if (tome.reincarnations.size() == 0) {
			if (!strReason.IsEmpty()) strReason += _AL("\r");
			strReason += CECUIHelper::GetGameUIMan()->GetStringFromTable(10799);
			bEnable = false;
		}
		if (pHost->GetReincarnationCount() >= MAX_REINCARNATION && bReach105EveryTime) {
			bEnable = false;
			if (!strReason.IsEmpty()) strReason += _AL("\r");
			strReason += CECUIHelper::GetGameUIMan()->GetStringFromTable(10817);
		}
		if (tome.tome_exp >= CECHostPlayer::REINCARNATION_TOME::max_exp) {
			bEnable = false;
			if (!strReason.IsEmpty()) strReason += _AL("\r");
			strReason += CECUIHelper::GetGameUIMan()->GetStringFromTable(10818);
		}
		
		if (tome.tome_active) // µ±Ç°ÇåÐÑ
		{
			if (bEnable)
				strText += CECUIHelper::GetGameUIMan()->GetStringFromTable(11167);
			else
				strText += strReason;
		}
		else
		{
			if (bEnable)
				strText += CECUIHelper::GetGameUIMan()->GetStringFromTable(11165);
			else
			{
				strText = CECUIHelper::GetGameUIMan()->GetStringFromTable(10788);
				strText += _AL("\r");
				strText += strReason;
			}
		}		
		
		pObj->Enable(bEnable);
		pObj->SetHint(strText);
	}
}

void CDlgReincarnationBook::UpdateHistoryMaxLv(AUIObject* pObj) {
	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	if (pObj) {
		ACString strText;
		strText.Format(_AL("%d"), pHost->GetMaxLevelSofar());
		pObj->SetText(strText);
	}
}

void CDlgReincarnationBook::UpdateTxtBookExp(AUIObject* pObj) {
	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	if (pObj) {
		ACString strText;
		strText.Format(_AL("%d"), tome.tome_exp);
		pObj->SetText(strText);
		strText.Format(CECUIHelper::GetGameUIMan()->GetStringFromTable(11163), tome.max_exp);
		pObj->SetHint(strText);
	}
}

void CDlgReincarnationBook::UpdateLblBookExp(AUIObject* pObj) {
	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	if (pObj)
	{
		ACString strText;
		strText.Format(CECUIHelper::GetGameUIMan()->GetStringFromTable(11163), tome.max_exp);
		pObj->SetHint(strText);
	}
}

void CDlgReincarnationBook::UpdateWakeImg(AUIObject* pObj) {
	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if (pImg) {
		pImg->FixFrame(tome.tome_active);
	}
}