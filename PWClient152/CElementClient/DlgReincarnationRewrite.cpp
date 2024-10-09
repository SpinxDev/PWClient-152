// Filename	: DlgReincarnationRewrite.cpp
// Creator	: Han Guanghui
// Date		: 2013/08/06

#include "DlgReincarnationRewrite.h"
#include "DlgReincarnation.h"
#include "DlgReincarnationBook.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "AUIProgress.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgReincarnationRewrite, CDlgBase)
AUI_ON_COMMAND("Btn_Start",	OnCommandRewrite)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCancel)
AUI_ON_COMMAND("Btn_Wake",		OnCommandSleepWake)
AUI_END_COMMAND_MAP()

CDlgReincarnationRewrite::CDlgReincarnationRewrite()
{
}

CDlgReincarnationRewrite::~CDlgReincarnationRewrite()
{
}

bool CDlgReincarnationRewrite::OnInitDialog()
{
	return true;
}

void CDlgReincarnationRewrite::OnCommandRewrite(const char* szCommand)
{
	unsigned int index = GetData();
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost) return;
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	if (index < tome.reincarnations.size()) 
		GetGameSession()->c2s_CmdRewriteReincarnationTome(index, tome.reincarnations[index].level);
	
}

void CDlgReincarnationRewrite::OnShowDialog()
{
	Update();
}

void CDlgReincarnationRewrite::OnHideDialog() {
	CDlgReincarnationBook* pDlg = GetGameUIMan()->m_pDlgReincarnationBook;
	pDlg->SetPosEx(GetPos().x, GetPos().y);
}

void CDlgReincarnationRewrite::OnCommandCancel(const char *szCommand)
{
	Show(false);
	CDlgReincarnationBook* pDlg = GetGameUIMan()->m_pDlgReincarnationBook;
	pDlg->SetPosEx(GetPos().x, GetPos().y);
	pDlg->Show(true);
}

void CDlgReincarnationRewrite::OnCommandSleepWake(const char* szCommand) {
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost) return;
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	GetGameSession()->c2s_CmdActivateReincarnationTome(1 - tome.tome_active);
}

#define SET_TEXT(val) \
{\
	if (pObj) {\
		strText.Format(_AL("%d"), val);\
		pObj->SetText(strText);\
	}\
}
void CDlgReincarnationRewrite::Update()
{
	unsigned int index = GetData();
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost) return;
	const CECHostPlayer::REINCARNATION_TOME& tome = pHost->GetReincarnationTome();
	if (index < tome.reincarnations.size()) {
		ACString strText;
		int iLevel = tome.reincarnations[index].level;
		if (iLevel > 104 || iLevel < 100) {
			OnCommandCancel(NULL);
			return;
		}
		PAUIOBJECT pObj = GetDlgItem("Txt_RewriteInfo");
		if(pObj) {
			long ltime = tome.reincarnations[index].timestamp;
			tm birth_time = *localtime(&ltime);
			strText.Format(GetStringFromTable(10812), birth_time.tm_year + 1900, 
				birth_time.tm_mon + 1, birth_time.tm_mday, 
				pHost->GetName(), tome.reincarnations[index].level, index + 1);
			pObj->SetText(strText);
		}
		pObj = GetDlgItem("Txt_Level");
		if (pObj) {
			strText.Format(GetStringFromTable(10813), iLevel + 1);
			pObj->SetText(strText);
		}
		pObj = GetDlgItem("Txt_FullExp");
		SET_TEXT(pHost->GetLevelUpExp(iLevel));
		pObj = GetDlgItem("Txt_LastEXP");
		SET_TEXT(tome.reincarnations[index].exp);
		pObj = GetDlgItem("Txt_BookLevel");
		SET_TEXT(pHost->GetLevelUpExp(iLevel) - tome.reincarnations[index].exp);
		pObj = GetDlgItem("Txt_Point");
		SET_TEXT(ATTRIBUTE_POINT[iLevel + 1 - 100]);

		bool bEnable = tome.tome_exp - pHost->GetLevelUpExp(iLevel) + tome.reincarnations[index].exp >= 0;

		pObj = GetDlgItem("Txt_BookExp");
		SET_TEXT(tome.tome_exp);
		if(pObj) pObj->SetColor(bEnable ? A3DCOLORRGB(57,37,0):A3DCOLORRGB(255,0,0));
		
		pObj = GetDlgItem("Btn_Start");		
		if (pObj) pObj->Enable(bEnable);

		pObj = GetDlgItem("Gfx_Btn");
		if (pObj) pObj->Show(bEnable); 
		
		AUIProgress* pPrgExp = dynamic_cast<AUIProgress*>(GetDlgItem("Pgs_Inject"));
		if (pPrgExp) {
			int iProgress = int((double)(tome.tome_exp + tome.reincarnations[index].exp) / pHost->GetLevelUpExp(iLevel) * pPrgExp->GetRangeMax());
			pPrgExp->SetProgress(min(iProgress, pPrgExp->GetRangeMax()));
		}

		pObj = GetDlgItem("Txt_ExpPercent");
		if (pObj) {
			pObj->SetText(ACString().Format(_AL("%d%%"), pPrgExp->GetProgress() * 100 / pPrgExp->GetRangeMax()));
		}
	}
	UpdateBookUI();
}

void CDlgReincarnationRewrite::UpdateBookUI() {
	AUIObject* pObj;

	pObj = GetDlgItem("Img_Wake");
	CDlgReincarnationBook::UpdateWakeImg(pObj);
	
	pObj = GetDlgItem("Btn_Wake");
	CDlgReincarnationBook::UpdateSleepWakeBtn(pObj);
	
	pObj = GetDlgItem("Txt_HighLevel");
	CDlgReincarnationBook::UpdateHistoryMaxLv(pObj);
	
	pObj = GetDlgItem("Lbl_BookExp");
	CDlgReincarnationBook::UpdateLblBookExp(pObj);
	
	pObj = GetDlgItem("Txt_BookExp1");
	CDlgReincarnationBook::UpdateTxtBookExp(pObj);
}