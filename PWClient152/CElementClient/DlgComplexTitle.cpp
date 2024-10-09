// Filename	: DlgComplexTitle.cpp
// Creator	: Han Guanghui
// Date		: 2013/5/16

#include "DlgComplexTitle.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "elementdataman.h"


AUI_BEGIN_COMMAND_MAP(CDlgComplexTitle, CDlgBase)
AUI_ON_COMMAND("Btn_TitleList",		OnCommandTitleList)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgComplexTitle, CDlgBase)
AUI_ON_EVENT("Lst_NameList",	WM_LBUTTONUP,	OnEventSelectList)
AUI_ON_EVENT("Lst_NameList",	WM_KEYUP,		OnEventSelectList)
AUI_ON_EVENT("Lst_NameList",	WM_KEYDOWN,		OnEventSelectList)
AUI_END_EVENT_MAP()

static const ACHAR GRAY_COLOR[] = _AL("^6E503A");

CDlgComplexTitle::CDlgComplexTitle():
m_pListTitles(NULL),
m_pListComplexTitles(NULL),
m_iCurrentSelect(0)
{

}

CDlgComplexTitle::~CDlgComplexTitle()
{

}

bool CDlgComplexTitle::OnInitDialog()
{
	DDX_Control("Lst_NameList", m_pListComplexTitles);
	DDX_Control("Lst_TitleName", m_pListTitles);

	return true;
}

void CDlgComplexTitle::OnShowDialog()
{
	if (!m_ComplexTitles.empty()) return;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
	while(tid) {
		if(DataType == DT_COMPLEX_TITLE_CONFIG) {
			COMPLEX_TITLE_CONFIG* pConfig = (COMPLEX_TITLE_CONFIG* )pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
			if (pConfig && pConfig->name[0] != 0) {
				m_ComplexTitles.push_back(pConfig);
				m_pListComplexTitles->AddString(pConfig->name);
			}
		}
		tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
	m_pListComplexTitles->SetSel(m_iCurrentSelect, true);
	Update();
}

void CDlgComplexTitle::OnCommandTitleList(const char* szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_TitleList");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}

void CDlgComplexTitle::SetPropertyText(int prop)
{
	char szTemp[50];
	sprintf(szTemp, "Lbl_Reward%d", m_iLabIndex);
	PAUIOBJECT pObj = GetDlgItem(szTemp);
	if (pObj == NULL) return;
	ACString strText;
	if (prop) {
		strText.Format(GetStringFromTable(10611 + m_iPropIndex), prop);
		pObj->SetText(strText);
		pObj->Show(true);
		m_iLabIndex++;
	}
	m_iPropIndex++;
}
void CDlgComplexTitle::Update()
{
	int title_sum = m_ComplexTitles.size();
	const CECHostPlayer::TITLE_CONTAINER& titles = GetHostPlayer()->GetTitles();
	CECHostPlayer::TITLE_CONTAINER::const_iterator iter;
	ACString strText;
	ACString strGetTitle;
	if (m_iCurrentSelect >= 0 && m_iCurrentSelect < title_sum)
	{
		m_pListTitles->ResetContent();
		COMPLEX_TITLE_CONFIG* pConfig = m_ComplexTitles[m_iCurrentSelect];
		PAUIOBJECT pObj = GetDlgItem("Lbl_EventName");
		if (pObj) pObj->SetText(pConfig->name);
		pObj = GetDlgItem("Lbl_EventDes");
		ACString strText;
		if (pObj) {
			strText += pConfig->desc;
			strText += _AL("\r");
			strText += pConfig->condition;
			pObj->SetText(strText);
		}
		char szTemp[50];
		int i(1);
		while (true) {
			sprintf(szTemp, "Lbl_Reward%d", i++);
			pObj = GetDlgItem(szTemp);
			if (pObj) pObj->Show(false);
			else break;
		}
		m_iPropIndex = 0;
		m_iLabIndex = 1;
		SetPropertyText(pConfig->phy_damage);
		SetPropertyText(pConfig->magic_damage);
		SetPropertyText(pConfig->phy_defence);
		SetPropertyText(pConfig->magic_defence);
		SetPropertyText(pConfig->armor);
		SetPropertyText(pConfig->attack);

		for (i = 0; i < 9; ++i)
		{
			int id_sub = pConfig->sub_titles[i];
			if (id_sub == 0) break;
			iter = std::find(titles.begin(), titles.end(), CECHostPlayer::TITLE(id_sub, 0));
			bool bHasTitle = iter != titles.end();
			const TITLE_CONFIG* pSub = CECPlayer::GetTitleConfig(id_sub);
			if (pSub) {
				if (bHasTitle) m_pListTitles->AddString(pSub->name);
				else {
					strText.Format(_AL("%s%s"), GRAY_COLOR, pSub->name);
					m_pListTitles->AddString(strText);
				}
			}
		}
	}
}
void CDlgComplexTitle::OnEventSelectList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	m_iCurrentSelect = m_pListComplexTitles->GetCurSel();
	Update();
}
