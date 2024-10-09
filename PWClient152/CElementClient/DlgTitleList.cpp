// Filename	: DlgTitleList.cpp
// Creator	: Han Guanghui
// Date		: 2013/5/16

#include "DlgTitleList.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Utility.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "AUICheckBox.h"
#include "AUIDef.h"
#include "DlgTask.h"
#include "DlgItemDesc.h"

AUI_BEGIN_COMMAND_MAP(CDlgTitleList, CDlgBase)

AUI_ON_COMMAND("Btn_Search",	OnCommandSearch)
AUI_ON_COMMAND("Btn_up",		OnCommandPagePrevious)
AUI_ON_COMMAND("Btn_down",		OnCommandPageNext)
AUI_ON_COMMAND("Chk_Open*",		OnCommandCheckTitle)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Event",		OnCommand_ComplexTitle)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTitleList, CDlgBase)
AUI_ON_EVENT("*", WM_MOUSEWHEEL, OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgTitleList::CDlgTitleList():
m_iTitleNum(0)
{
	memset(m_pLbl_Prop,0,sizeof(m_pLbl_Prop));
}

CDlgTitleList::~CDlgTitleList()
{

}

bool CDlgTitleList::OnInitDialog()
{
	char szTemp[50];
	int i(0);
	m_iTitleNum = 0;
	while (true) {
		sprintf(szTemp, "Lbl_TitleName%d", i + 1);
		if (GetDlgItem(szTemp)) m_iTitleNum++;
		else break;
		++i;
	}

	DDX_Control("Label_PhyAttack",m_pLbl_Prop[0]);
	DDX_Control("Label_MgiAttack",m_pLbl_Prop[1]);
	DDX_Control("Label_PhyDefence",m_pLbl_Prop[2]);
	DDX_Control("Label_MgiDefence",m_pLbl_Prop[3]);
	DDX_Control("Label_Definition",m_pLbl_Prop[4]);
	DDX_Control("Label_Evade",m_pLbl_Prop[5]);	

	return true;
}

void CDlgTitleList::OnShowDialog()
{
	Update();
}

void CDlgTitleList::OnCommandSearch(const char* szCommand)
{
	PAUIOBJECT pSearchText = GetDlgItem("Txt_Search");
	if (pSearchText == NULL) return;
	ACString strSearch = pSearchText->GetText();
	const CECHostPlayer::TITLE_CONTAINER& titles = GetHostPlayer()->GetTitles();
	int sum_title = titles.size();
	int i = sum_title - 1; 
	for (; i > -1; --i)
	{
		const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(titles[i].id);
		if (!pConfig) continue;
		ACString strName(pConfig->name);
		if (strName.Find(strSearch) != -1) m_iFirstIndex = sum_title - i - 1;
	}
	Update();
}

void CDlgTitleList::OnCommandPagePrevious(const char* szCommand)
{
	if (m_iFirstIndex > 0) {
		m_iFirstIndex--;
		Update();
	}
}

void CDlgTitleList::OnCommandPageNext(const char* szCommand)
{
	PAUIOBJECT pDown = GetDlgItem("Btn_down");
	if (pDown && pDown->IsShow()) {
		m_iFirstIndex++;
		Update();
	}
}
void CDlgTitleList::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 ) OnCommandPagePrevious(NULL);
	else OnCommandPageNext(NULL);
}
void CDlgTitleList::OnCommandCheckTitle(const char* szCommand)
{
	PAUICHECKBOX pCheck = dynamic_cast<PAUICHECKBOX>(GetDlgItem(szCommand));
	if (pCheck == NULL) return;
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost == NULL) return;
	unsigned short title_id = (unsigned short)pCheck->GetData();
	if (pHost->GetCurrentTitle() == title_id) {
		title_id = 0;
	}
	GetGameSession()->c2s_CmdChangeTitle(title_id);
}
#define SET_TEXT(text, newline) \
{\
	if (newline) m_strDesc += _AL("\r");\
	m_strDesc += strColor;\
	m_strDesc += text;\
}
#define SET_PROP(prop, index) \
{\
	if (prop) {\
		m_strDesc += _AL("\r");\
		strNum.Format(GetStringFromTable(10611 + index), prop);\
		m_strDesc += strNum;\
	}\
}

void CDlgTitleList::GetTitleDesc(unsigned short title_id)
{
	m_strDesc.Empty();
	const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(title_id);
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	if (pConfig) {
		ACString strColor;
		// ³ÆºÅÃû
		strColor = pConfig->color ? A3DCOLOR_TO_STRING(pConfig->color) 
			: pDescTab->GetWideString(ITEMDESC_COL_WHITE);
		SET_TEXT(pConfig->name, false)
			// ³ÆºÅÃèÊö
			strColor = pDescTab->GetWideString(ITEMDESC_COL_WHITE);
		SET_TEXT(pConfig->desc, true)
		// »ñÈ¡Ìõ¼þÃèÊö
		strColor = pDescTab->GetWideString(ITEMDESC_COL_YELLOW);
		SET_TEXT(pConfig->condition, true)
		// ×Ó³ÆºÅ
		DATA_TYPE DataType = DT_INVALID;	
		const void* pDataPtr = g_pGame->GetElementDataMan()->get_data_ptr(pConfig->id, ID_SPACE_CONFIG, DataType);
		if (DataType == DT_COMPLEX_TITLE_CONFIG) {
			COMPLEX_TITLE_CONFIG* pComplex = (COMPLEX_TITLE_CONFIG*)pConfig;
			for (int i = 0; i < 9; ++i)
			{
				int id_sub = pComplex->sub_titles[i];
				if (id_sub == 0) break;
				const TITLE_CONFIG* pSub = CECPlayer::GetTitleConfig(id_sub);
				if (pSub) {
					strColor = pSub->color ? A3DCOLOR_TO_STRING(pSub->color) 
						: pDescTab->GetWideString(ITEMDESC_COL_WHITE);
					SET_TEXT(pSub->name, true)
				}
			}
		}
		ACString strNum;
		m_strDesc += pDescTab->GetWideString(ITEMDESC_COL_DARKGOLD);
		SET_PROP(pConfig->phy_damage, 0);
		SET_PROP(pConfig->magic_damage, 1);
		SET_PROP(pConfig->phy_defence, 2);
		SET_PROP(pConfig->magic_defence, 3);
		SET_PROP(pConfig->armor, 4);
		SET_PROP(pConfig->attack, 5);
	}
}
void CDlgTitleList::OnCommandCANCEL(const char * szCommand)
{
	CDlgItemDesc* pDlg = dynamic_cast<CDlgItemDesc*>(GetGameUIMan()->GetDialog("Win_ItemDesc"));
	CDlgBase::OnCommand_CANCEL(szCommand);
}
void CDlgTitleList::Update()
{
	CECHostPlayer::TITLE_CONTAINER titles_reverse;
	const CECHostPlayer::TITLE_CONTAINER& titles = GetHostPlayer()->GetTitles();
	int sum_title = titles.size();
	int i = sum_title - 1; 
	titles_reverse.reserve(i + 1);
	for (; i > -1; --i)
		titles_reverse.push_back(titles[i]);
	if (m_iFirstIndex < 0 || m_iFirstIndex > sum_title - 1)
		m_iFirstIndex = 0;
	char szTemp[50];
	PAUIOBJECT pName(NULL), pUp(NULL), pDown(NULL);
	PAUICHECKBOX pCheck(NULL);
	ACString strTemp,strText;	

	for (i = 0; i < m_iTitleNum; ++i)
	{
		sprintf(szTemp, "Lbl_TitleName%d", i + 1);
		pName = GetDlgItem(szTemp);
		sprintf(szTemp, "Chk_Open%d", i + 1);
		pCheck = (PAUICHECKBOX)GetDlgItem(szTemp);
		int index = m_iFirstIndex + i;
		if (index < sum_title) {
			const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(titles_reverse[index].id);
			if (pConfig) {
				pName->Show(true);
				pCheck->Show(true);
				pName->SetText(pConfig->name);
				pName->SetData(pConfig->id);
				pName->SetColor(pConfig->color ? pConfig->color : A3DCOLORRGB(255, 255, 255));
				GetTitleDesc(pConfig->id);
				int expire_time = titles_reverse[index].expire_time;
				if (expire_time){
					tm t = g_pGame->GetServerLocalTime(expire_time);
					strTemp.Format(m_pAUIManager->GetStringFromTable(8010),
						t.tm_year+1900,
						t.tm_mon+1,
						t.tm_mday,
						t.tm_hour,
						t.tm_min );
					strText.Format(m_pAUIManager->GetStringFromTable(10605), strTemp);
					m_strDesc += _AL("\r");
					m_strDesc += strText;
				}
				pName->SetHint(m_strDesc);
				pCheck->Check(titles_reverse[index].id == GetHostPlayer()->GetCurrentTitle());
				pCheck->SetData(titles_reverse[index].id);
			} else {
				pName->Show(false);
				pCheck->Show(false);
			}
		} else {
			pName->Show(false);
			pCheck->Show(false);
		}
	}
	pUp = GetDlgItem("Btn_up");
	if (pUp) pUp->Show(m_iFirstIndex != 0);
	pDown = GetDlgItem("Btn_down");
	if (pDown) pDown->Show(m_iFirstIndex + i < sum_title);

	memset(m_totalPropValue,0,sizeof(m_totalPropValue));
	for (i=0;i<(int)titles_reverse.size();i++)
	{
		const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(titles_reverse[i].id);
		m_totalPropValue[0] += pConfig->phy_damage;
		m_totalPropValue[1] += pConfig->magic_damage;
		m_totalPropValue[2] += pConfig->phy_defence;
		m_totalPropValue[3] += pConfig->magic_defence;
		m_totalPropValue[4] += pConfig->attack;
		m_totalPropValue[5] += pConfig->armor;
	}
	for (i=0;i<PROP_NUM;i++)
	{
		ACString str;
		str.Format(GetGameUIMan()->GetStringFromTable(11180+i),m_totalPropValue[i]);
		m_pLbl_Prop[i]->SetText(str);
	}
}
void CDlgTitleList::QueryTitleList()
{
	g_pGame->GetGameSession()->
		c2s_CmdQueryTitle(g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID());
}
void CDlgTitleList::OnCommand_ComplexTitle(const char * szCommand)
{
	PAUIDIALOG pComplexTitle = m_pAUIManager->GetDialog("Win_ComplexTitle");
	if (pComplexTitle) pComplexTitle->Show(!pComplexTitle->IsShow());
}
