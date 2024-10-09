// File		: DlgCreate.cpp
// Creator	: Feng Ning
// Date		: 2010/10/20

#include "DlgCreate.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "EC_ProfConfigs.h"
#include "EC_Configs.h"

#include <AUIStillImageButton.h>
#include <AUIImagePicture.h>

#define new A_DEBUG_NEW

//	CDlgCreateProfession
AUI_BEGIN_COMMAND_MAP(CDlgCreateProfession, CDlgBase)
AUI_ON_COMMAND("Btn_Prof*", OnCommand_Prof)
AUI_ON_COMMAND("Btn_Race*", OnCommand_Race)
AUI_ON_COMMAND("Btn_Confirm", OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Cancel)
AUI_ON_COMMAND("Btn_PrevRace", OnCommand_PrevRace)
AUI_ON_COMMAND("Btn_NextRace", OnCommand_NextRace)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCreateProfession, CDlgBase)
AUI_ON_EVENT("Btn_Prof*", WM_LBUTTONDBLCLK, OnEventLButtonDblClick_Prof)
AUI_END_EVENT_MAP()

CDlgCreateProfession::CDlgCreateProfession()
{
}

void CDlgCreateProfession::OnShowDialog()
{
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	pUIMan->ChangeScene(CECLoginUIMan::LOGIN_SCENE_CREATE_CHOOSE_PROFESSION);

	PAUIDIALOG pDlgCreateRace = pUIMan->GetDialog("Win_CreateRace");
	if (!pDlgCreateRace->IsShow()){
		pDlgCreateRace->Show(true);
	}
	
	PAUIDIALOG pDlgCreateButton1 = pUIMan->GetDialog("Win_CreateButton1");
	if (!pDlgCreateButton1->IsShow()){
		pDlgCreateButton1->Show(true);
	}
	this->AlignTo(pDlgCreateButton1, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_CENTER, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);

	//	选中上次选中种族或默认种族
	int lastProf = pUIMan->GetCurProfession();
	int race(-1), prof(lastProf);
	if (lastProf < 0){
		//	找出优先显示的种族
		int order(INT_MAX);
		for (int r(0); r < NUM_RACE; ++ r)
		{
			int o = CECProfConfig::Instance().GetRaceShowOrder(r);
			if (o < order){
				order = o;
				race = r;
			}
		}
	}else{
		race = CECProfConfig::Instance().GetRaceByProfession(lastProf);
	}
	AString strCmd;
	strCmd.Format("Btn_Race%d", race);
	pDlgCreateRace->OnCommand(strCmd);

	CDlgBase::OnShowDialog();
}

void CDlgCreateProfession::SendCmdChooseProfession(int prof)
{
	if (!CECProfConfig::Instance().IsProfession(prof)){
		assert(false);
		return;
	}
	AString strName;
	for (int i(0); i < 2; ++ i)
	{
		strName.Format("Btn_Prof%d", i);
		PAUIOBJECT pBtn_Prof = GetDlgItem(strName);
		if (!pBtn_Prof){
			break;
		}
		if ((int)pBtn_Prof->GetData() == prof){
			this->OnCommand(strName);
			break;
		}
	}
}

void CDlgCreateProfession::OnHideDialog()
{
	PAUIMANAGER pUIMan = GetAUIManager();
	PAUIDIALOG pDlgCreateRace = pUIMan->GetDialog("Win_CreateRace");
	if (pDlgCreateRace->IsShow()){
		pDlgCreateRace->Show(false);
	}
	
	PAUIDIALOG pDlgCreateButton1 = pUIMan->GetDialog("Win_CreateButton1");
	if (pDlgCreateButton1->IsShow()){
		pDlgCreateButton1->Show(false);
	}
	
	AString strName;
	for (int r(0); r < NUM_RACE; ++ r)
	{
		strName.Format("Win_RaceInfo%d", r);
		PAUIDIALOG pDlgRaceInfo = pUIMan->GetDialog(strName);
		if (pDlgRaceInfo->IsShow()){
			pDlgRaceInfo->Show(false);
		}
	}
	for (int p(0); p < NUM_PROFESSION; ++ p)
	{
		PAUIDIALOG pDlgProfRadar = pUIMan->GetDialog(strName.Format("Win_ProfRadar%d", p));
		if (pDlgProfRadar->IsShow()){
			pDlgProfRadar->Show(false);
		}
	}
	CDlgBase::OnHideDialog();
}

void CDlgCreateProfession::OnTick()
{
	PAUIMANAGER pUIMan = GetAUIManager();
	PAUIDIALOG pDlgMouseOn(NULL);
	PAUIOBJECT pObjMouseOn(NULL);
	pUIMan->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);
	int profHover = -1;
	bool atLeft(false);
	AString strName;
	if (pDlgMouseOn == this){
		for (int i(0); i < 2; ++ i)
		{
			PAUIOBJECT pObj = GetDlgItem(strName.Format("Btn_Prof%d", i));
			if (pObj != pObjMouseOn){
				continue;
			}
			profHover = (int)pObj->GetData();
			atLeft = (i == 0);
			break;
		}
	}
	for (int p(0); p < NUM_PROFESSION; ++ p)
	{
		PAUIDIALOG pDlgProfRadar = pUIMan->GetDialog(strName.Format("Win_ProfRadar%d", p));
		if (p != profHover){
			if (pDlgProfRadar->IsShow()){
				pDlgProfRadar->Show(false);
			}
			continue;
		}
		if (!pDlgProfRadar->IsShow()){
			PAUIDIALOG pDlgRaceInfo = pUIMan->GetDialog("Win_RaceInfo0");
			int x = pDlgRaceInfo->GetPos().x + pDlgRaceInfo->GetSize().cx;
			if (!atLeft){
				x = pUIMan->GetRect().right - x - pDlgProfRadar->GetSize().cx;
			}
			int y = this->GetPos().y - pDlgProfRadar->GetSize().cy - 10;
			pDlgProfRadar->SetPosEx(x, y);
			pDlgProfRadar->Show(true);
			PAUIOBJECT pImg = pDlgProfRadar->GetDlgItem("Img_Prof");
			pImg->SetAlpha(192);
			pUIMan->BringWindowToTop(this);
		}
		pDlgProfRadar->SetWholeAlpha(this->GetWholeAlpha());	//	跟当前对话框 alpha 一致，以实现 alpha 动画
	}
}

void CDlgCreateProfession::OnCommand_Race(const char* szCommand)
{
	int race = atoi(szCommand + strlen("Btn_Race"));
	if (race < 0 || race >= NUM_RACE){
		assert(false);
		return;
	}
	CECLoginUIMan * pUIMan = GetLoginUIMan();
	AString strName;

	PAUIDIALOG pDlgCreateRace = pUIMan->GetDialog("Win_CreateRace");
	for (int i = 0; i < NUM_RACE; ++ i)
	{
		if (PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(pDlgCreateRace->GetDlgItem(strName.Format("Btn_Race%d", i)))){
			pBtn->SetPushed(i == race);
		}
	}

	//	显示种族介绍
	for (int r(0); r < NUM_RACE; ++ r)
	{
		PAUIDIALOG pDlgRaceInfo = pUIMan->GetDialog(strName.Format("Win_RaceInfo%d", r));
		bool bShouldShow = (r == race);
		if (pDlgRaceInfo->IsShow() != bShouldShow){
			pDlgRaceInfo->Show(bShouldShow);
		}
	}

	int lastProf = pUIMan->GetCurProfession();
	if (CECProfConfig::Instance().IsProfession(lastProf) &&
		CECProfConfig::Instance().GetRaceByProfession(lastProf) == race){
		return;
	}

	//	显示此种族可选择的职业
	int prof(-1), order(INT_MAX);
	for (int p(0); p < NUM_PROFESSION; ++ p)
	{
		if (CECProfConfig::Instance().GetRaceByProfession(p) != race){
			continue;
		}
		const ACHAR *szProfName = GetGameRun()->GetProfName(p);
		int btnIndex = CECProfConfig::Instance().GetProfessionShowOrderInRace(p);
		for (int g(0); g < NUM_GENDER; ++ g)
		{
			if (CECProfConfig::Instance().CanShowOnCreate(p, g)){
				PAUIOBJECT pBtnProf = GetDlgItem(strName.Format("Btn_Prof%d", btnIndex));
				if (!pBtnProf){
					break;
				}
				pBtnProf->SetData(p);
				pBtnProf->SetHint(szProfName);
				
				PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)GetDlgItem(strName.Format("Img_Prof%d", btnIndex));
				pImg->FixFrame(p);
				pImg->SetHint(szProfName);
				PAUIIMAGEPICTURE pImgDisable = (PAUIIMAGEPICTURE)GetDlgItem(strName.Format("Img_Prof_Disable%d", btnIndex));
				pImgDisable->FixFrame(p);
				pImgDisable->SetHint(szProfName);
				
				PAUIOBJECT pTxtProf = GetDlgItem(strName.Format("Txt_Prof%d", btnIndex));
				pTxtProf->SetText(szProfName);
				
				PAUIOBJECT pTxtMsg = GetDlgItem(strName.Format("Txt_Msg%d", btnIndex));
				pTxtMsg->SetText(GetStringFromTable(2100+p));
				
				int o = CECProfConfig::Instance().GetProfessionShowOrderInRace(p);
				if (o < order){
					order = o;
					prof = p;
				}
			}
		}
	}
	
	//	默认选中某职业
	SendCmdChooseProfession(prof);
	
	//	再次播放渐变显示动画
	StartAnimation("show");
}

void CDlgCreateProfession::OnCommand_Prof(const char* szCommand)
{
	//	设置职业和性别
	int btnIndex = atoi(szCommand + strlen("Btn_Prof"));
	PAUIOBJECT pBtn_Prof = GetDlgItem(szCommand);
	int prof = (int)pBtn_Prof->GetData();
	if (!CECProfConfig::Instance().IsProfession(prof)){
		assert(false);
		return;
	}
	int gender = GENDER_MALE;
	for (int g(0); g < NUM_GENDER; ++ g)
	{
		if (CECProfConfig::Instance().CanShowOnCreate(prof, g)){
			gender = g;
			break;
		}
	}
	GetLoginUIMan()->ChangeShowModel(prof, gender);

	//	更新界面	
	AString strName;
	for (int i(0); i < 2; ++ i)
	{
		PAUISTILLIMAGEBUTTON pBtn_Prof = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(strName.Format("Btn_Prof%d", i)));
		if (!pBtn_Prof){
			break;
		}
		bool bSelected = (int)pBtn_Prof->GetData() == prof;
		pBtn_Prof->SetPushed(bSelected);		
		PAUIOBJECT pImg = GetDlgItem(strName.Format("Img_Prof%d", i));
		pImg->Show(bSelected);
		PAUIOBJECT pImgDisable = GetDlgItem(strName.Format("Img_Prof_Disable%d", i));
		pImgDisable->Show(!bSelected);		
		PAUIOBJECT pTxtMsg = GetDlgItem(strName.Format("Txt_Msg%d", i));
		pTxtMsg->SetAlpha(bSelected ? 255 : 64);
	}
}

void CDlgCreateProfession::OnCommand_Confirm(const char* szCommand)
{
	Show(false);
	GetAUIManager()->GetDialog("Win_CreateGenderName")->Show(true);
}

void CDlgCreateProfession::OnCommand_Cancel(const char* szCommand)
{
	Show(false);
	GetLoginUIMan()->SwitchToSelectChar();
}

void CDlgCreateProfession::OnCommand_PrevRace(const char* szCommand)
{
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	int nCurProfession = pUIMan->GetCurProfession();
	if (CECProfConfig::Instance().IsProfession(nCurProfession)){
		int race = CECProfConfig::Instance().GetRaceByProfession(nCurProfession);
		int raceShowOrder = CECProfConfig::Instance().GetRaceShowOrder(race);
		int nextShowOrder = (raceShowOrder + NUM_RACE-1)%NUM_RACE;
		for (int i(0); i < NUM_RACE; ++ i)
		{
			if (i != race){
				int order = CECProfConfig::Instance().GetRaceShowOrder(i);
				if (order == nextShowOrder){
					AString strCmd;
					strCmd.Format("Btn_Race%d", i);
					pUIMan->GetDialog("Win_CreateRace")->OnCommand(strCmd);
					break;
				}
			}
		}
	}
}

void CDlgCreateProfession::OnCommand_NextRace(const char* szCommand)
{
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	int nCurProfession = pUIMan->GetCurProfession();
	if (CECProfConfig::Instance().IsProfession(nCurProfession)){
		int race = CECProfConfig::Instance().GetRaceByProfession(nCurProfession);
		int raceShowOrder = CECProfConfig::Instance().GetRaceShowOrder(race);
		int nextShowOrder = (raceShowOrder + 1)%NUM_RACE;
		for (int i(0); i < NUM_RACE; ++ i)
		{
			if (i != race){
				int order = CECProfConfig::Instance().GetRaceShowOrder(i);
				if (order == nextShowOrder){
					AString strCmd;
					strCmd.Format("Btn_Race%d", i);
					pUIMan->GetDialog("Win_CreateRace")->OnCommand(strCmd);
					break;
				}
			}
		}
	}
}

void CDlgCreateProfession::OnEventLButtonDblClick_Prof(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommand_Confirm(NULL);
}

//	CDlgCreateGenderName
AUI_BEGIN_COMMAND_MAP(CDlgCreateGenderName, CDlgBase)
AUI_ON_COMMAND("Btn_Gender*", OnCommand_Gender)
AUI_ON_COMMAND("Btn_Back", OnCommand_Back)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Back)
AUI_ON_COMMAND("Btn_Confirm", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_CancelCreate", OnCommand_Cancel)
AUI_ON_COMMAND("Chk_SwitchEquip", OnCommand_SwitchEquip)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCreateGenderName, CDlgBase)
AUI_ON_EVENT("DEFAULT_CharName", WM_KEYDOWN, OnEventKeyDown)
AUI_END_EVENT_MAP()

bool CDlgCreateGenderName::OnInitDialog()
{
	PAUIEDITBOX pEdt_CharName = dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_CharName"));
	pEdt_CharName->SetMaxLength(GetGame()->GetConfigs()->GetMaxNameLen());
	return CDlgBase::OnInitDialog();
}

void CDlgCreateGenderName::OnShowDialog()
{
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	int prof = pUIMan->GetCurProfession();
	int gender = pUIMan->GetCurGender();
	pUIMan->ChangeScene((CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_CREATE_BEGIN + prof));

	AString strName;
	PAUIDIALOG pDlgCreateInfo = pUIMan->GetDialog(strName.Format("Win_CreateInfo%d", prof));
	if (!pDlgCreateInfo->IsShow()){
		pDlgCreateInfo->Show(true);
	}
	
	PAUIDIALOG pDlgCreateButton2 = pUIMan->GetDialog("Win_CreateButton2");
	if (!pDlgCreateButton2->IsShow()){
		pDlgCreateButton2->Show(true);
	}
	this->AlignTo(pDlgCreateButton2, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_CENTER, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);

	PAUIDIALOG pDlgSwitchNewCharEquip = pUIMan->GetDialog("Win_SwitchNewCharEquip");
	if (!pDlgSwitchNewCharEquip->IsShow()){
		pDlgSwitchNewCharEquip->Show(true);
	}
	PAUICHECKBOX pChk_SwitchEquip = dynamic_cast<PAUICHECKBOX>(pDlgSwitchNewCharEquip->GetDlgItem("Chk_SwitchEquip"));
	pChk_SwitchEquip->Check(!pUIMan->IsShownNewModelEquip());

	AString strCmd;
	OnCommand_Gender(strCmd.Format("Btn_Gender%d", gender));
	SetCharName(_AL(""));
	
	GetAUIManager()->BringWindowToTop(this);
	FocusOnInput();
	CDlgBase::OnShowDialog();
}

void CDlgCreateGenderName::OnHideDialog()
{
	CECLoginUIMan *pUIMan = GetLoginUIMan();

	AString strName;
	for (int r(0); r < NUM_PROFESSION; ++ r)
	{
		PAUIDIALOG pDlgRaceInfo = pUIMan->GetDialog(strName.Format("Win_CreateInfo%d", r));
		if (pDlgRaceInfo->IsShow()){
			pDlgRaceInfo->Show(false);
		}
	}

	PAUIDIALOG pDlgCreateButton2 = pUIMan->GetDialog("Win_CreateButton2");
	if (pDlgCreateButton2->IsShow()){
		pDlgCreateButton2->Show(false);
	}
	PAUIDIALOG pDlgSwitchNewCharEquip = pUIMan->GetDialog("Win_SwitchNewCharEquip");
	if (pDlgSwitchNewCharEquip->IsShow()){
		pDlgSwitchNewCharEquip->Show(false);
	}

	CDlgBase::OnHideDialog();
}

void CDlgCreateGenderName::OnTick()
{
	PAUIDIALOG pDlgCreateButton2 = GetAUIManager()->GetDialog("Win_CreateButton2");
	PAUIOBJECT pBtn_Confirm = pDlgCreateButton2->GetDlgItem("Btn_Confirm");
	pBtn_Confirm->Enable(CanConfirm());
	CDlgBase::OnTick();
}

void CDlgCreateGenderName::OnCommand_Gender(const char* szCommand)
{
	int gender = atoi(szCommand + strlen("Btn_Gender"));
	if (!CECProfConfig::Instance().IsGender(gender)){
		assert(false);
		return;
	}
	
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	int prof = pUIMan->GetCurProfession();
	pUIMan->ChangeShowModel(prof, gender);

	pUIMan->SwitchToCustomize(true);

	AString strName;
	for (int g(0); g < NUM_GENDER; ++ g)
	{
		PAUISTILLIMAGEBUTTON pBtn_Gender = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(strName.Format("Btn_Gender%d", g)));
		pBtn_Gender->SetPushed(g == gender);
		pBtn_Gender->Enable(CECProfConfig::Instance().IsExist(prof, g));
	}
	FocusOnInput();
}

void CDlgCreateGenderName::OnCommand_Back(const char* szCommand)
{
	//	取消个性化设置
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	pUIMan->CancelCustomize();

	SetCharName(_AL(""));
	Show(false);
	PAUIDIALOG pDlgCreateProfession = GetAUIManager()->GetDialog("Win_CreateProfession");
	pDlgCreateProfession->Show(true);
}

void CDlgCreateGenderName::OnCommand_Confirm(const char* szCommand)
{
	if (!CanConfirm()){
		return;
	}
	//	接受当前个性化
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	pUIMan->ConfirmCustomize();

	ACString strCharName = GetCharName();
	SetCharName(_AL(""));
	pUIMan->NewCharacter(strCharName);	
}

void CDlgCreateGenderName::OnCommand_Cancel(const char* szCommand)
{
	//	取消个性化设置
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	pUIMan->CancelCustomize();

	SetCharName(_AL(""));
	Show(false);
	GetLoginUIMan()->SwitchToSelectChar();
}

void CDlgCreateGenderName::OnCommand_SwitchEquip(const char* szCommand)
{
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	PAUIDIALOG pDlgSwitchNewCharEquip = pUIMan->GetDialog("Win_SwitchNewCharEquip");
	PAUICHECKBOX pChk_SwitchEquip = dynamic_cast<PAUICHECKBOX>(pDlgSwitchNewCharEquip->GetDlgItem("Chk_SwitchEquip"));
	pUIMan->SwitchShowModelEquip(!pChk_SwitchEquip->IsChecked());
}

void CDlgCreateGenderName::OnCreateSucccess()
{
	//	角色创建成功、回退到选人界面
	CECLoginUIMan *pUIMan = GetLoginUIMan();
	pUIMan->GetCustomizeMgr()->HideRelatedDialogs();
	
	SetCharName(_AL(""));
	Show(false);
	GetLoginUIMan()->SwitchToSelectChar();
}

bool CDlgCreateGenderName::CanConfirm()
{
	return !GetCharName().IsEmpty();
}

ACString CDlgCreateGenderName::GetCharName()
{
	PAUIOBJECT pObj_CharName = GetDlgItem("DEFAULT_CharName");
	return pObj_CharName->GetText();
}

void CDlgCreateGenderName::SetCharName(const ACHAR *szName)
{
	GetDlgItem("DEFAULT_CharName")->SetText(szName);
}

void CDlgCreateGenderName::FocusOnInput()
{
	ChangeFocus(GetDlgItem("DEFAULT_CharName"));
}

void CDlgCreateGenderName::OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_ESCAPE){
		ChangeFocus(NULL);
	}else if (wParam == VK_RETURN){
		OnCommand_Confirm(NULL);
	}
}
