// TaskPremisePage.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskPremisePage.h"
#include "TaskTemplEditorView.h"
#include "TemplIDSelDlg.h"
#include "TaskIDSelDlg.h"
#include "ItemsWantedDlg.h"
#include "TeamInfoDlg.h"
#include "Occupation.h"
#include "PreTaskDlg.h"
#include "ProficiencyDlg.h"
#include "TaskTitle.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCSTR g_strTaskIdSvrPath;
extern CString GetBaseDataNameByID(unsigned long ulID);

//帮派职位  见ids.hxx
static const int FACTION_ROLES_MEMBER = 6;

unsigned long period_map[15] =
{
	0,			// 筑基
	1,			// 灵犀
	2,			// 和合
	3,			// 元婴
	4,			// 空冥
	5,			// 履霜
	6,			// 渡劫
	7,			// 寂灭
	8,			// 大乘
	20,			// 上仙
	30,			// 狂魔
	21,			// 真仙
	31,			// 魔圣
	22,			// 天仙
	32			// 魔尊
};

int find_period_index(unsigned long ulPeriod)
{
	for (int i = 0; i < sizeof(period_map) / sizeof(unsigned long); i++)
		if (period_map[i] == ulPeriod)
			return i;

	return 0;
}

static const int FORCE_ID[] = {
	0,
	1004,
	1005,
	1006,
	-1,
};
enum
{
	RES_MOD_DEPRESSEC_AGED_PERSION = 10,
	RES_MOD_RED_BEAR,
	RES_MOD_PONYCAR,
	RES_MOD_SNOWMAN,
	RES_MOD_MONEYFROG = 26,
	RES_MOD_WEDDING_KID_M,
	RES_MOD_WEDDING_KID_F,
	RES_MOD_SOLDIER,
	RES_MOD_TREEBIRD,
	RES_MOD_EVIL_TIGER,
	RES_MOD_RED_SQUIRREL,
	RES_MOD_LAND_TURTLE,
	RES_MOD_BRIDE_COUSIN,
	RES_MOD_SENTRY_PUPPET,
	RES_MOD_PENGUIN_KING,
	RES_MOD_PENGUIN_QUEEN,
	RES_MOD_SNOW_RABBIT,
	RES_MOD_TEDDY_BEAR,
	RES_MOD_RED_SQUIRREL_BIG,
	RES_MOD_TREEBIRD_BIG,
	RES_MOD_SENTRY_ANGRY,
	RES_MOD_SENTRY_DEPRESSED,
	RES_MOD_SENTRY_LOST,
	RES_MOD_SNOW_RABBIT_BLAZE,
	RES_MOD_DRAGON,
	RES_MOD_47,
	RES_MOD_48,
	RES_MOD_49,
	RES_MOD_50,
	RES_MOD_51,
	RES_MOD_52,
};
unsigned char TRANFORMED_MASK_MAP[] = 
{
	0xFF,
	0x80,
	0x80 | RES_MOD_DEPRESSEC_AGED_PERSION,
	0x80 | RES_MOD_RED_BEAR,
	0x80 | RES_MOD_PONYCAR,
	0x80 | RES_MOD_SNOWMAN,
	0x80 | RES_MOD_MONEYFROG,
	0x80 | RES_MOD_WEDDING_KID_M,
	0x80 | RES_MOD_WEDDING_KID_F,
	0x80 | RES_MOD_SOLDIER,
	0x80 | RES_MOD_TREEBIRD,
	0x80 | RES_MOD_EVIL_TIGER,
	0x80 | RES_MOD_RED_SQUIRREL,
	0x80 | RES_MOD_LAND_TURTLE,
	0x80 | RES_MOD_BRIDE_COUSIN,
	0x80 | RES_MOD_SENTRY_PUPPET,
	0x80 | RES_MOD_PENGUIN_KING,
	0x80 | RES_MOD_PENGUIN_QUEEN,
	0x80 | RES_MOD_SNOW_RABBIT,
	0x80 | RES_MOD_TEDDY_BEAR,
	0x80 | RES_MOD_RED_SQUIRREL_BIG,
	0x80 | RES_MOD_TREEBIRD_BIG,
	0x80 | RES_MOD_SENTRY_ANGRY,
	0x80 | RES_MOD_SENTRY_DEPRESSED,
	0x80 | RES_MOD_SENTRY_LOST,
	0x80 | RES_MOD_SNOW_RABBIT_BLAZE,
	0x80 | RES_MOD_DRAGON,
	0x80 | RES_MOD_47,
	0x80 | RES_MOD_48,
	0x80 | RES_MOD_49,
	0x80 | RES_MOD_50,
	0x80 | RES_MOD_51,
	0x80 | RES_MOD_52,
};

int find_transform_index(unsigned char ucTransform)
{
	for (int i = 0; i < sizeof(TRANFORMED_MASK_MAP) / sizeof(unsigned char); i++)
		if (TRANFORMED_MASK_MAP[i] == ucTransform)
			return i;
		
		return 0;
}
int find_force_index2(int iForce)
{
	for (int i = 0; i < sizeof(FORCE_ID) / sizeof(unsigned int); i++)
		if (FORCE_ID[i] == iForce)
			return i;
		
		return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CTaskPremisePage property page

IMPLEMENT_DYNCREATE(CTaskPremisePage, CPropertyPage)

CTaskPremisePage::CTaskPremisePage() : CPropertyPage(CTaskPremisePage::IDD), m_pTask(0)
{
	//{{AFX_DATA_INIT(CTaskPremisePage)
	m_bIsClan = FALSE;
	m_nFactionRole = 0;
	m_bIsSpouse = FALSE;
	m_bIsTeam = FALSE;
	m_strPremCotask = _T("");
	m_dwDeposit = 0;
	m_lReputation = 0;
	m_iFactionContrib = 0;
	m_dwPremLevMin = 0;
	m_dwPremLevMax = 0;
	m_nGender = 0;
	m_bShowByDeposit = FALSE;
	m_bShowByGender = FALSE;
	m_bShowByItems = FALSE;
	m_bShowByLev = FALSE;
	m_bShowByOccup = FALSE;
	m_bShowByPreTask = FALSE;
	m_bShowByRepu = FALSE;
	m_bShowByFactionContrib = FALSE;
	m_bShowByTeam = FALSE;
	m_bShowByPeriod = FALSE;
	m_bShowBySpouse = FALSE;
	m_bShowByFaction = FALSE;
	m_nCoTaskCond = 0;
	m_nPeriod = 0;
	m_bIsGM = FALSE;
	m_lRepuMax = 0;
	m_bNeedComp = FALSE;
	m_nExp1AndOrExp2 = 0;
	m_nLeftType1 = 0;
	m_lLeftNum1  = 0;
	m_nCompOper1 = 0;
	m_nRightType1= 0;
	m_lRightNum1 = 0;
	m_nLeftType2 = 0;
	m_lLeftNum2  = 0;
	m_nCompOper2 = 0;
	m_nRightType2= 0;
	m_lRightNum2 = 0;
	m_bIsShieldUser = FALSE;
	m_dwPremRMBMin = 0;
	m_dwPremRMBMax = 0;
	m_iCharStartTime = -1;
	m_bShowByCharTime = FALSE;
	m_bCharTime = FALSE;
	m_iCharTimeGreaterThan = 1;
	m_nCurrTimeCond = 0;
	m_tmCharEndTime = CTime::GetCurrentTime();
	m_bShowByRMB = FALSE;
	m_bIsLibraryTask = FALSE;
	m_fLibraryTasksProbability = 0.f;
	m_iWorldContribution = 0;
	m_iNeedRecordTasksNum = 0;
	m_bShowByRecordTasksNum = FALSE;
	m_iTransformed = 0;
	m_iForce = 0;
	m_bShowByForce = FALSE;
	m_bCheckForce = FALSE;
	m_iForceContrib = 0;
	m_bShowByForceContrib = FALSE;
	m_iForceRepu = 0;
	m_bShowByForceRepu = FALSE;
	m_iExp = 0;
	m_bShowByExp = FALSE;
	m_iSP = 0;
	m_bShowBySP = FALSE;
	m_bEnableForce = FALSE;
	m_bEnableForcePro = FALSE;
	m_iForceAL = 0;
	m_bShowByForceAL = TRUE;
	m_bWeddingOwner = FALSE;
	m_bShowByWeddingOwner = TRUE;
	m_bIsKing = FALSE;
	m_bShowByKing = TRUE;
	m_bNotInTeam = FALSE;
	m_bShowByNotInTeam = TRUE;
	m_iTitleRequired = 0;
	m_iHistoryIndexLow = 0;
	m_iHistoryIndexHigh = 0;
	m_bCheckHistoryMaxLevel = FALSE;
	m_bCheckReincarnation = FALSE;
	m_bCheckRealmLevel = FALSE;
	m_bShowByReincarnation = FALSE;
	m_bShowByRealmLevel = FALSE;
	m_iReincarnationMin = 0;
	m_iReincarnationMax = 0;
	m_iRealmLevelMin = 0;
	m_iRealmLevelMax = 0;
	m_bCheckRealmExpFull = FALSE;
	m_bShowByCardCount = FALSE;
	m_iGeneralCardCount = 0;
	m_iGeneralCardRank = -1;
	m_iGeneralCardRankCount = 0;
	m_bShowByCardRank = 0;
	m_bShowByHistoryStage = FALSE;
	//}}AFX_DATA_INIT
}

CTaskPremisePage::~CTaskPremisePage()
{
}

void CTaskPremisePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskPremisePage)
	DDX_Control(pDX, IDC_ITEMS_WANTED, m_ItemsBox);
	DDX_Check(pDX, IDC_IS_CLAN, m_bIsClan);
	DDX_CBIndex(pDX, IDC_FACTION_ROLE, m_nFactionRole);
	DDX_Check(pDX, IDC_IS_SPOUSE, m_bIsSpouse);
	DDX_Check(pDX, IDC_IS_TEAM, m_bIsTeam);
	DDX_Text(pDX, IDC_PREMISE_COTASK, m_strPremCotask);
	DDX_Text(pDX, IDC_PREMISE_DEPOSIT, m_dwDeposit);
	DDX_Text(pDX, IDC_PREMISE_REPUTATION, m_lReputation);
	DDX_Text(pDX, IDC_PREMISE_FACTION_CONTRIB, m_iFactionContrib);
	DDX_Text(pDX, IDC_PREMISE_LEVEL_MIN, m_dwPremLevMin);
	DDX_Text(pDX, IDC_PREMISE_LEVEL_MAX, m_dwPremLevMax);
	DDX_CBIndex(pDX, IDC_GENDER, m_nGender);
	DDX_Check(pDX, IDC_SHOW_BY_DEPOSIT, m_bShowByDeposit);
	DDX_Check(pDX, IDC_SHOW_BY_GENDER, m_bShowByGender);
	DDX_Check(pDX, IDC_SHOW_BY_ITEMS, m_bShowByItems);
	DDX_Check(pDX, IDC_SHOW_BY_LEVEL, m_bShowByLev);
	DDX_Check(pDX, IDC_SHOW_BY_OCCUP, m_bShowByOccup);
	DDX_Check(pDX, IDC_SHOW_BY_PRE_TASK, m_bShowByPreTask);
	DDX_Check(pDX, IDC_SHOW_BY_REPUTATION, m_bShowByRepu);
	DDX_Check(pDX, IDC_SHOW_BY_FACTION_CONTRIB, m_bShowByFactionContrib);
	DDX_Check(pDX, IDC_SHOW_BY_TEAM, m_bShowByTeam);
	DDX_Check(pDX, IDC_SHOW_BY_PERIOD, m_bShowByPeriod);
	DDX_Check(pDX, IDC_SHOW_BY_SPOUSE, m_bShowBySpouse);
	DDX_Check(pDX, IDC_SHOW_BY_FACTION, m_bShowByFaction);
	DDX_Radio(pDX, IDC_COTASK_COND1, m_nCoTaskCond);
	DDX_CBIndex(pDX, IDC_PERIOD, m_nPeriod);
	DDX_Check(pDX, IDC_IS_GM, m_bIsGM);
	DDX_Text(pDX, IDC_PREMISE_REPUMAX, m_lRepuMax);
	DDX_Check(pDX, IDC_PREM_NEED_COMP, m_bNeedComp);
	DDX_CBIndex(pDX, IDC_PREM_AND_OR, m_nExp1AndOrExp2);
	DDX_CBIndex(pDX, IDC_PREM1_COMPARE1, m_nLeftType1);
	DDX_Text(pDX, IDC_PREM1_KV1, m_lLeftNum1);
	DDX_CBIndex(pDX, IDC_PREM1_COMPARE, m_nCompOper1);
	DDX_CBIndex(pDX, IDC_PREM1_COMPARE2, m_nRightType1);
	DDX_Text(pDX, IDC_PREM1_KV2, m_lRightNum1);
	DDX_CBIndex(pDX, IDC_PREM2_COMPARE1, m_nLeftType2);
	DDX_Text(pDX, IDC_PREM2_KV1, m_lLeftNum2);
	DDX_CBIndex(pDX, IDC_PREM2_COMPARE, m_nCompOper2);
	DDX_CBIndex(pDX, IDC_PREM2_COMPARE2, m_nRightType2);
	DDX_Text(pDX, IDC_PREM2_KV2, m_lRightNum2);
	DDX_Check(pDX, IDC_IS_SHIELD_USER, m_bIsShieldUser);
	DDX_Text(pDX, IDC_PREMISE_RMB_MIN, m_dwPremRMBMin);
	DDX_Text(pDX, IDC_PREMISE_RMB_MAX, m_dwPremRMBMax);
	DDX_CBIndex(pDX, IDC_CHAR_START_TIME, m_iCharStartTime);
	DDX_Check(pDX, IDC_SHOW_BY_CHAR_TIME, m_bShowByCharTime);
	DDX_Check(pDX, IDC_CHAR_TIME, m_bCharTime);
	DDX_Text(pDX, IDC_CHAR_TIME_GREATER_THAN, m_iCharTimeGreaterThan);
	DDV_MinMaxUInt(pDX, m_iCharTimeGreaterThan, 0, 65536);
	DDX_Radio(pDX, IDC_CHAR_CURR_TIME, m_nCurrTimeCond);
	DDX_DateTimeCtrl(pDX, IDC_SPECIFIED_DATE, m_tmCharEndTime);
	DDX_Check(pDX, IDC_SHOW_BY_RMB, m_bShowByRMB);
	DDX_Check(pDX, IDC_IS_KU_TASK, m_bIsLibraryTask);
	DDX_Text(pDX, IDC_CHAR_PROBABILITY, m_fLibraryTasksProbability);
	DDX_Text(pDX, IDC_WORLD_CONTRIB_REQUIRE, m_iWorldContribution);
	DDX_Text(pDX, IDC_PREMISE_NEED_RECORD_TASKS_NUM,m_iNeedRecordTasksNum);
	DDV_MinMaxInt(pDX,m_iNeedRecordTasksNum,0,2040);
	DDX_Check(pDX, IDC_SHOW_BY_NEEDRECORD_TASKS_NUM,m_bShowByRecordTasksNum);
	DDX_Check(pDX, IDC_IS_UNIQUE_STORAGE_TASK,m_bIsUniqueStorageTask);
	DDX_CBIndex(pDX, IDC_TRANSFORMED, m_iTransformed);
	DDX_Check(pDX, IDC_SHOW_BY_TRANSFORMED,m_bShowByTransformed);
	DDX_Check(pDX,IDC_CHECK_FORCE,m_bCheckForce);
	DDX_CBIndex(pDX,IDC_PREM_FORCE,m_iForce);
	DDX_Check(pDX,IDC_SHOW_BY_FORCE,m_bShowByForce);
	DDX_Text(pDX,IDC_PREM_FORCE_CONTRIB,m_iForceContrib);
	DDX_Check(pDX,IDC_SHOW_BY_FORCE_CONTRIB,m_bShowByForceContrib);
	DDX_Text(pDX,IDC_PREM_FORCE_REPU,m_iForceRepu);
	DDX_Check(pDX,IDC_SHOW_BY_FORCE_REPU,m_bShowByForceRepu);
	DDX_Text(pDX,IDC_PREM_FORCE_EXP,m_iExp);
	DDX_Check(pDX,IDC_SHOW_BY_FORCE_EXP,m_bShowByExp);
	DDX_Text(pDX,IDC_PREM_FORCE_SP,m_iSP);
	DDX_Check(pDX,IDC_SHOW_BY_FORCE_SP,m_bShowBySP);
	DDX_CBIndex(pDX,IDC_PREM_FORCE_AL,m_iForceAL);
	DDX_Check(pDX,IDC_SHOW_BY_FORCE_AL,m_bShowByForceAL);
	DDX_Check(pDX,IDC_WEDDING_OWNER,m_bWeddingOwner);
	DDX_Check(pDX,IDC_SHOW_BY_WEDDING_OWNER,m_bShowByWeddingOwner);
	DDX_Check(pDX,IDC_IS_KING,m_bIsKing);
	DDX_Check(pDX,IDC_SHOW_BY_KING,m_bShowByKing);
	DDX_Check(pDX,IDC_NOT_IN_TEAM,m_bNotInTeam);
	DDX_Check(pDX,IDC_SHOW_BY_NOT_IN_TEAM,m_bShowByNotInTeam);
	DDX_Text(pDX, IDC_PREMISE_TITLE_REQUIRED, m_iTitleRequired);
	DDX_Text(pDX, IDC_EDIT_HISTORYLOW, m_iHistoryIndexLow);
	DDX_Text(pDX, IDC_EDIT_HISTORYHIGH, m_iHistoryIndexHigh);
	DDX_Check(pDX, IDC_CHECK_HISTORY_MAX_LEVEL, m_bCheckHistoryMaxLevel);
	DDX_Check(pDX, IDC_PREM_CHECK_REINCARNATION, m_bCheckReincarnation);
	DDX_Check(pDX, IDC_PREM_CHECK_REINCARNATIONLEVEL, m_bCheckRealmLevel);
	DDX_Check(pDX, IDC_SHOW_BY_REINCARNATION, m_bShowByReincarnation);
	DDX_Check(pDX, IDC_SHOW_BY_REINCARNATIONLEVEL, m_bShowByRealmLevel);
	DDX_Text(pDX, IDC_PREMISE_REINCARNATION_MIN, m_iReincarnationMin);
	DDV_MinMaxInt(pDX, m_iReincarnationMin, 0, 10);
	DDX_Text(pDX, IDC_PREMISE_REINCARNATION_MAX, m_iReincarnationMax);
	DDV_MinMaxInt(pDX, m_iReincarnationMax, 0, 10);
	DDX_Text(pDX, IDC_PREMISE_REINCARNATIONLEVEL_MIN, m_iRealmLevelMin);
	DDV_MinMaxInt(pDX, m_iRealmLevelMin, 0, 100);
	DDX_Text(pDX, IDC_PREMISE_REINCARNATIONLEVEL_MAX, m_iRealmLevelMax);
	DDV_MinMaxInt(pDX, m_iRealmLevelMax, 0, 100);
	DDX_Check(pDX,IDC_CHK_CARDCOUNT, m_bShowByCardCount);
	DDX_Text(pDX,IDC_CARDCOUNT,m_iGeneralCardCount);
	DDX_CBIndex(pDX, IDC_GENERALCARD_RANK, m_iGeneralCardRank);
	DDX_Check(pDX,IDC_CHK_CARDRANKCOUNT, m_bShowByCardRank);
	DDX_Text(pDX,IDC_CARDRANKCOUNT,m_iGeneralCardRankCount);
	DDX_Check(pDX,IDC_CHK_SHOWPREMHISTORY, m_bShowByHistoryStage);

	//}}AFX_DATA_MAP
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CTaskTemplEditorView *pView = (CTaskTemplEditorView *) pChild->GetActiveView();
	HTREEITEM hSel = pView->m_TaskTree.GetSelectedItem();
	if (hSel == 0) return;
	ATaskTempl* pTask = (ATaskTempl*)pView->m_TaskTree.GetItemData(hSel);
	GetDlgItem(IDC_PREM_CHECK_REALMEXP_FULL)->EnableWindow(!pTask->m_Award_S->m_bExpandRealmLevelMax);
	if (pTask->m_Award_S->m_bExpandRealmLevelMax) {
		pTask->m_bPremCheckRealmExpFull = true;
		m_bCheckRealmExpFull = TRUE;
	}
	DDX_Check(pDX, IDC_PREM_CHECK_REALMEXP_FULL, m_bCheckRealmExpFull);
}


BEGIN_MESSAGE_MAP(CTaskPremisePage, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskPremisePage)
	ON_BN_CLICKED(IDC_SEL_COTASK, OnSelCotask)
	ON_BN_CLICKED(IDC_EDIT_ITEMS_WANTED, OnEditItemsWanted)
	ON_BN_CLICKED(IDC_EDIT_TEAM, OnEditTeam)
	ON_BN_CLICKED(IDC_EDIT_OCCUP, OnEditOccup)
	ON_BN_CLICKED(IDC_DELIVER_PREM_ITEM, OnDeliverPremItem)
	ON_BN_CLICKED(IDC_SEL_PRETASK, OnSelPretask)
	ON_BN_CLICKED(IDC_SEL_MUTEX_TASK, OnSelMutexTask)
	ON_BN_CLICKED(IDC_CHANGE_PROFICIENCY, OnChangeProficiency)
	ON_BN_CLICKED(IDC_CHECK_FORCE, OnCheckForce)
	ON_CBN_SELCHANGE(IDC_PREM_FORCE,OnCBForceSelectChange)
	ON_BN_CLICKED(IDC_SEL_TITLE, OnSelTitle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskPremisePage message handlers

BOOL CTaskPremisePage::UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly)
{
	if (bUpdateData)
	{
		UpdateData();

		pTask->m_ulGender				= m_nGender;
		pTask->m_bPremise_Spouse		= (m_bIsSpouse != 0);
		pTask->m_bTeamwork				= (m_bIsTeam != 0);
		pTask->m_ulPremise_Lev_Min		= m_dwPremLevMin;
		pTask->m_ulPremise_Lev_Max		= m_dwPremLevMax;
		pTask->m_ulPremise_Deposit		= m_dwDeposit;
		pTask->m_lPremise_Reputation	= m_lReputation;
		pTask->m_iPremiseFactionContrib = m_iFactionContrib;
		pTask->m_lPremise_RepuMax		= m_lRepuMax;
		pTask->m_ulCoTaskCond			= m_nCoTaskCond;
		pTask->m_ulPremise_Period		= period_map[m_nPeriod];
		pTask->m_bGM					= (m_bIsGM != 0);
		pTask->m_bShieldUser			= (m_bIsShieldUser != 0);
		pTask->m_ulPremise_Faction		= (m_bIsClan != 0);
		pTask->m_iPremise_FactionRole	= FACTION_ROLES_MEMBER - m_nFactionRole;
		
		pTask->m_bPremNeedComp               = (m_bNeedComp != 0);
		pTask->m_nPremExp1AndOrExp2          = m_nExp1AndOrExp2;

		pTask->m_Prem1KeyValue.nLeftType = m_nLeftType1;
		pTask->m_Prem1KeyValue.lLeftNum  = m_lLeftNum1;
		pTask->m_Prem1KeyValue.nCompOper = m_nCompOper1;
		pTask->m_Prem1KeyValue.nRightType= m_nRightType1;
		pTask->m_Prem1KeyValue.lRightNum = m_lRightNum1;

		pTask->m_Prem2KeyValue.nLeftType = m_nLeftType2;
		pTask->m_Prem2KeyValue.lLeftNum  = m_lLeftNum2;
		pTask->m_Prem2KeyValue.nCompOper = m_nCompOper2;
		pTask->m_Prem2KeyValue.nRightType= m_nRightType2;
		pTask->m_Prem2KeyValue.lRightNum = m_lRightNum2;

		pTask->m_bShowByDeposit			= (m_bShowByDeposit != 0);
		pTask->m_bShowByGender			= (m_bShowByGender != 0);
		pTask->m_bShowByItems			= (m_bShowByItems != 0);
		pTask->m_bShowByLev				= (m_bShowByLev != 0);
		pTask->m_bShowByOccup			= (m_bShowByOccup != 0);
		pTask->m_bShowByPreTask			= (m_bShowByPreTask != 0);
		pTask->m_bShowByRepu			= (m_bShowByRepu != 0);
		pTask->m_bShowByFactionContrib  = (m_bShowByFactionContrib != 0);
		pTask->m_bShowByTeam			= (m_bShowByTeam != 0);
		pTask->m_bShowByPeriod			= (m_bShowByPeriod != 0);
		pTask->m_bShowBySpouse			= (m_bShowBySpouse != 0);
		pTask->m_bShowByFaction			= (m_bShowByFaction != 0);
		pTask->m_bShowByCharTime		= (m_bShowByCharTime != 0);
		pTask->m_bShowByRMB				= (m_bShowByRMB != 0);
		pTask->m_bShowByTransformed		= (m_bShowByTransformed != 0);

		pTask->m_ulPremRMBMin			= m_dwPremRMBMin;
		pTask->m_ulPremRMBMax			= m_dwPremRMBMax;
		pTask->m_iCharStartTime			= m_iCharStartTime;
		
		pTask->m_iCharEndTime			= m_nCurrTimeCond;
		pTask->m_tmCharEndTime.year		= m_tmCharEndTime.GetYear();
		pTask->m_tmCharEndTime.month	= m_tmCharEndTime.GetMonth();
		pTask->m_tmCharEndTime.day		= m_tmCharEndTime.GetDay();	
		pTask->m_bCharTime				= (m_bCharTime != 0);
		pTask->m_ulCharTimeGreaterThan	= m_iCharTimeGreaterThan;
		pTask->m_bIsLibraryTask			= (m_bIsLibraryTask != 0);
		pTask->m_fLibraryTasksProbability = m_fLibraryTasksProbability;
		pTask->m_iWorldContribution = m_iWorldContribution;
		pTask->m_iPremNeedRecordTasksNum = m_iNeedRecordTasksNum;
		pTask->m_bShowByNeedRecordTasksNum = (m_bShowByRecordTasksNum != 0);
		pTask->m_bIsUniqueStorageTask = (m_bIsUniqueStorageTask != 0);
		pTask->m_ucPremiseTransformedForm = TRANFORMED_MASK_MAP[m_iTransformed];

		pTask->m_bPremCheckForce = (m_bCheckForce != 0);
		pTask->m_iPremForce = FORCE_ID[m_iForce];
		pTask->m_bShowByForce = (m_bShowByForce != 0);
		pTask->m_iPremForceContribution = m_iForceContrib;
		pTask->m_bShowByForceContribution = (m_bShowByForceContrib != 0);
		pTask->m_iPremForceReputation = m_iForceRepu;
		pTask->m_bShowByForceReputation = (m_bShowByForceRepu != 0);
		pTask->m_iPremForceExp = m_iExp;
		pTask->m_bShowByForceExp = (m_bShowByExp != 0);
		pTask->m_iPremForceSP = m_iSP;
		pTask->m_bShowByForceSP = (m_bShowBySP != 0);
		pTask->m_iPremForceActivityLevel = m_iForceAL - 1;
		pTask->m_bShowByForceActivityLevel = (m_bShowByForceAL != 0);
		pTask->m_bPremiseWeddingOwner = (m_bWeddingOwner != 0);
		pTask->m_bShowByWeddingOwner = (m_bShowByWeddingOwner != 0);
		pTask->m_bPremIsKing = (m_bIsKing != 0);
		pTask->m_bShowByKing = (m_bShowByKing != 0);
		pTask->m_bPremNotInTeam = (m_bNotInTeam != 0);
		pTask->m_bShowByNotInTeam = (m_bShowByNotInTeam != 0);
		pTask->m_iPremTitleNumRequired = m_iTitleRequired;

		pTask->m_iPremHistoryStageIndex[0]	= m_iHistoryIndexLow;
		pTask->m_iPremHistoryStageIndex[1]	= m_iHistoryIndexHigh;

		pTask->m_bPremCheckMaxHistoryLevel = (m_bCheckHistoryMaxLevel != 0);
		pTask->m_bPremCheckReincarnation = (m_bCheckReincarnation != 0);
		pTask->m_bPremCheckRealmLevel = (m_bCheckRealmLevel != 0);
		pTask->m_bShowByReincarnation = (m_bShowByReincarnation != 0);
		pTask->m_bShowByRealmLevel = (m_bShowByRealmLevel != 0);
		pTask->m_ulPremReincarnationMin = m_iReincarnationMin;
		pTask->m_ulPremReincarnationMax = m_iReincarnationMax;
		pTask->m_ulPremRealmLevelMin = m_iRealmLevelMin;
		pTask->m_ulPremRealmLevelMax = m_iRealmLevelMax;
		pTask->m_bPremCheckRealmExpFull = (m_bCheckRealmExpFull != 0);

		pTask->m_ulPremGeneralCardCount = m_iGeneralCardCount;
		pTask->m_bShowByGeneralCard = (m_bShowByCardCount !=0);
		pTask->m_bShowByGeneralCardRank = (m_bShowByCardRank != 0);
		pTask->m_iPremGeneralCardRank = m_iGeneralCardRank - 1;
		pTask->m_ulPremGeneralCardRankCount = m_iGeneralCardRankCount;

		pTask->m_bShowByHistoryStage = (m_bShowByHistoryStage !=0);
	}
	else
	{
		m_nGender			= pTask->m_ulGender;
		m_bIsSpouse			= pTask->m_bPremise_Spouse;
		m_bIsTeam			= pTask->m_bTeamwork;
		m_dwPremLevMin		= pTask->m_ulPremise_Lev_Min;
		m_dwPremLevMax		= pTask->m_ulPremise_Lev_Max;
		m_dwDeposit			= pTask->m_ulPremise_Deposit;
		m_lReputation		= pTask->m_lPremise_Reputation;
		m_iFactionContrib   = pTask->m_iPremiseFactionContrib;
		m_lRepuMax			= pTask->m_lPremise_RepuMax;
		m_strPremCotask		= GetTaskNameByID(pTask->m_ulPremise_Cotask);
		m_nCoTaskCond		= pTask->m_ulCoTaskCond;
		m_nPeriod			= find_period_index(pTask->m_ulPremise_Period);
		m_bIsGM				= pTask->m_bGM;
		m_bIsShieldUser		= pTask->m_bShieldUser;
		m_bIsClan			= (pTask->m_ulPremise_Faction != 0);
		m_nFactionRole		= FACTION_ROLES_MEMBER - pTask->m_iPremise_FactionRole;

		m_bNeedComp         = pTask->m_bPremNeedComp;
		m_nExp1AndOrExp2    = pTask->m_nPremExp1AndOrExp2;

		m_nLeftType1        = pTask->m_Prem1KeyValue.nLeftType; 
		m_lLeftNum1         = pTask->m_Prem1KeyValue.lLeftNum;
		m_nCompOper1        = pTask->m_Prem1KeyValue.nCompOper;
		m_nRightType1       = pTask->m_Prem1KeyValue.nRightType;
		m_lRightNum1        = pTask->m_Prem1KeyValue.lRightNum;

		m_nLeftType2        = pTask->m_Prem2KeyValue.nLeftType; 
		m_lLeftNum2         = pTask->m_Prem2KeyValue.lLeftNum;
		m_nCompOper2        = pTask->m_Prem2KeyValue.nCompOper;
		m_nRightType2       = pTask->m_Prem2KeyValue.nRightType;
		m_lRightNum2        = pTask->m_Prem2KeyValue.lRightNum;

		m_bShowByDeposit	= pTask->m_bShowByDeposit;
		m_bShowByGender		= pTask->m_bShowByGender;
		m_bShowByItems		= pTask->m_bShowByItems;
		m_bShowByLev		= pTask->m_bShowByLev;
		m_bShowByOccup		= pTask->m_bShowByOccup;
		m_bShowByPreTask	= pTask->m_bShowByPreTask;
		m_bShowByRepu		= pTask->m_bShowByRepu;
		m_bShowByFactionContrib = pTask->m_bShowByFactionContrib;
		m_bShowByTeam		= pTask->m_bShowByTeam;
		m_bShowByPeriod		= pTask->m_bShowByPeriod;
		m_bShowBySpouse		= pTask->m_bShowBySpouse;
		m_bShowByFaction	= pTask->m_bShowByFaction;
		m_bShowByCharTime	= pTask->m_bShowByCharTime;
		m_bShowByRMB		= pTask->m_bShowByRMB;
		m_bShowByTransformed= pTask->m_bShowByTransformed;

		m_dwPremRMBMin		= pTask->m_ulPremRMBMin;
		m_dwPremRMBMax		= pTask->m_ulPremRMBMax;
		m_iCharStartTime	= pTask->m_iCharStartTime;
		
		m_nCurrTimeCond		= pTask->m_iCharEndTime;
		if(m_nCurrTimeCond)
			m_tmCharEndTime		= CTime(pTask->m_tmCharEndTime.year, pTask->m_tmCharEndTime.month, pTask->m_tmCharEndTime.day, 0, 0, 0);
		m_bCharTime			= pTask->m_bCharTime;
		m_iCharTimeGreaterThan = pTask->m_ulCharTimeGreaterThan;

		m_bIsLibraryTask = pTask->m_bIsLibraryTask;
		m_fLibraryTasksProbability = pTask->m_fLibraryTasksProbability;
		m_iWorldContribution = pTask->m_iWorldContribution;

		m_iNeedRecordTasksNum = pTask->m_iPremNeedRecordTasksNum;
		m_bShowByRecordTasksNum = pTask->m_bShowByNeedRecordTasksNum;
		m_bIsUniqueStorageTask = pTask->m_bIsUniqueStorageTask;

		m_iTransformed = find_transform_index(pTask->m_ucPremiseTransformedForm);

		m_bCheckForce = pTask->m_bPremCheckForce;
		m_iForce = find_force_index2(pTask->m_iPremForce);
		m_bShowByForce = pTask->m_bShowByForce;
		m_iForceContrib = pTask->m_iPremForceContribution;
		m_iForceRepu = pTask->m_iPremForceReputation;
		m_iExp = pTask->m_iPremForceExp;
		m_iSP = pTask->m_iPremForceSP;
		m_bShowByForceContrib = pTask->m_bShowByForceContribution;
		m_bShowByForceRepu = pTask->m_bShowByForceReputation;
		m_bShowByExp = pTask->m_bShowByForceExp;
		m_bShowBySP = pTask->m_bShowByForceSP;
		m_iForceAL = pTask->m_iPremForceActivityLevel + 1;
		m_bShowByForceAL = pTask->m_bShowByForceActivityLevel;

		m_pTask				= pTask;
		m_bEnableForce = pTask->m_bPremCheckForce;
		m_bEnableForcePro = pTask->m_iPremForce != 0;

		m_bWeddingOwner = pTask->m_bPremiseWeddingOwner;
		m_bShowByWeddingOwner = pTask->m_bShowByWeddingOwner;

		m_bIsKing = pTask->m_bPremIsKing;
		m_bShowByKing = pTask->m_bShowByKing;

		m_bNotInTeam = pTask->m_bPremNotInTeam;
		m_bShowByNotInTeam = pTask->m_bShowByNotInTeam;
		m_iTitleRequired = pTask->m_iPremTitleNumRequired;

		m_iHistoryIndexLow		= pTask->m_iPremHistoryStageIndex[0];
		m_iHistoryIndexHigh		= pTask->m_iPremHistoryStageIndex[1];

		m_bCheckHistoryMaxLevel = pTask->m_bPremCheckMaxHistoryLevel;
		m_bCheckReincarnation = pTask->m_bPremCheckReincarnation;
		m_bCheckRealmLevel = pTask->m_bPremCheckRealmLevel;
		m_bShowByReincarnation = pTask->m_bShowByReincarnation;
		m_bShowByRealmLevel = pTask->m_bShowByRealmLevel;
		m_iReincarnationMin = pTask->m_ulPremReincarnationMin;
		m_iReincarnationMax = pTask->m_ulPremReincarnationMax;
		m_iRealmLevelMin = pTask->m_ulPremRealmLevelMin;
		m_iRealmLevelMax = pTask->m_ulPremRealmLevelMax;
		m_bCheckRealmExpFull = pTask->m_bPremCheckRealmExpFull;

		m_bShowByCardCount = pTask->m_bShowByGeneralCard;
		m_iGeneralCardCount = pTask->m_ulPremGeneralCardCount;
		m_bShowByCardRank = pTask->m_bShowByGeneralCardRank;
		m_iGeneralCardRank = pTask->m_iPremGeneralCardRank + 1;
		m_iGeneralCardRankCount = pTask->m_ulPremGeneralCardRankCount;

		m_bShowByHistoryStage = pTask->m_bShowByHistoryStage;

		ShowItemsWanted();

		CWnd* pFirst = GetWindow(GW_CHILD);
		while (pFirst)
		{
			if (pFirst->GetDlgCtrlID() != 0xFFFF)
				pFirst->EnableWindow(!bReadOnly);
			pFirst = pFirst->GetNextWindow();
		}

		if (pTask->m_pParent)
			GetDlgItem(IDC_DELIVER_PREM_ITEM)->EnableWindow(FALSE);

		if (!m_bEnableForce)
		{
			GetDlgItem(IDC_PREM_FORCE)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_BY_FORCE)->EnableWindow(FALSE);
		}

		if (!m_bEnableForcePro)
		{
			GetDlgItem(IDC_PREM_FORCE_CONTRIB)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_BY_FORCE_CONTRIB)->EnableWindow(FALSE);
			GetDlgItem(IDC_PREM_FORCE_REPU)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_BY_FORCE_REPU)->EnableWindow(FALSE);
			GetDlgItem(IDC_PREM_FORCE_EXP)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_BY_FORCE_EXP)->EnableWindow(FALSE);
			GetDlgItem(IDC_PREM_FORCE_SP)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_BY_FORCE_SP)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_BY_FORCE_AL)->EnableWindow(FALSE);
			GetDlgItem(IDC_PREM_FORCE_AL)->EnableWindow(FALSE);
		}


		UpdateData(FALSE);
	}

	return TRUE;
}

void CTaskPremisePage::OnSelCotask()
{
	if (!m_pTask) return;

	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = m_pTask->m_ulPremise_Cotask;

	if (dlg.DoModal() == IDOK)
	{
		m_pTask->m_ulPremise_Cotask = dlg.m_ulID;
		GetDlgItem(IDC_PREMISE_COTASK)->SetWindowText(dlg.m_strName);
	}
}

void CTaskPremisePage::OnEditItemsWanted() 
{
	if (!m_pTask) return;

	CItemsWantedDlg dlg(
		MAX_ITEM_WANTED,
		m_pTask->m_PremItems,
		&m_pTask->m_ulPremItems,
		FALSE,
		TRUE,
		NULL,
		TRUE);

	dlg.m_bRandOne = m_pTask->m_bPremItemsAnyOne;
	dlg.DoModal();
	m_pTask->m_bPremItemsAnyOne = (dlg.m_bRandOne != 0);
	ShowItemsWanted();
}

void CTaskPremisePage::ShowItemsWanted()
{
	m_ItemsBox.ResetContent();

	for (unsigned char i = 0; i < m_pTask->m_ulPremItems; i++)
	{
		ITEM_WANTED& wi = m_pTask->m_PremItems[i];

		CString str;
		str.Format(
			"%s(%d个)",
			GetBaseDataNameByID(wi.m_ulItemTemplId),
			wi.m_ulItemNum);

		m_ItemsBox.AddString(str);
	}

	if (m_ItemsBox.GetCount()) m_ItemsBox.SetCurSel(0);
}

void CTaskPremisePage::OnEditTeam() 
{
	if (!m_pTask) return;

	CTeamInfoDlg dlg(m_pTask);
	dlg.DoModal();
}

void CTaskPremisePage::OnEditOccup() 
{
	if (!m_pTask) return;

	COccupation dlg(m_pTask);
	dlg.DoModal();
}

void CTaskPremisePage::OnDeliverPremItem() 
{
	if (!m_pTask) return;

	CItemsWantedDlg dlg(
		MAX_ITEM_WANTED,
		m_pTask->m_GivenItems,
		&m_pTask->m_ulGivenItems);

	dlg.DoModal();

}

void CTaskPremisePage::OnSelPretask() 
{
	if (!m_pTask) return;

	CPreTaskDlg dlg(m_pTask, true);
	dlg.DoModal();
}

void CTaskPremisePage::OnSelMutexTask() 
{
	if (!m_pTask) return;

	CPreTaskDlg dlg(m_pTask, false);
	dlg.DoModal();
}

void CTaskPremisePage::OnChangeProficiency()
{
	if (!m_pTask) return;

	CProficiencyDlg dlg(m_pTask->m_lSkillLev);
	dlg.DoModal();
}

void CTaskPremisePage::OnCheckForce()
{
	if (!m_pTask) return;

	m_bEnableForce = !m_bEnableForce;
	GetDlgItem(IDC_PREM_FORCE)->EnableWindow(m_bEnableForce);
	GetDlgItem(IDC_SHOW_BY_FORCE)->EnableWindow(m_bEnableForce);

	if (!m_bEnableForce)
	{
		m_iForce = 0;
		m_bShowByForce = FALSE;
		m_bCheckForce = FALSE;
	}
	
}

void CTaskPremisePage::OnCBForceSelectChange()
{
	if (!m_pTask) return;
	
	int curSel = ((CComboBox*)GetDlgItem(IDC_PREM_FORCE))->GetCurSel();
	if (curSel != 0)
	{
		GetDlgItem(IDC_PREM_FORCE_CONTRIB)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOW_BY_FORCE_CONTRIB)->EnableWindow(TRUE);
		GetDlgItem(IDC_PREM_FORCE_REPU)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOW_BY_FORCE_REPU)->EnableWindow(TRUE);
		GetDlgItem(IDC_PREM_FORCE_EXP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOW_BY_FORCE_EXP)->EnableWindow(TRUE);
		GetDlgItem(IDC_PREM_FORCE_SP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOW_BY_FORCE_SP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOW_BY_FORCE_AL)->EnableWindow(TRUE);
		GetDlgItem(IDC_PREM_FORCE_AL)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_PREM_FORCE_CONTRIB)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOW_BY_FORCE_CONTRIB)->EnableWindow(FALSE);
		GetDlgItem(IDC_PREM_FORCE_REPU)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOW_BY_FORCE_REPU)->EnableWindow(FALSE);
		GetDlgItem(IDC_PREM_FORCE_EXP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOW_BY_FORCE_EXP)->EnableWindow(FALSE);
		GetDlgItem(IDC_PREM_FORCE_SP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOW_BY_FORCE_SP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOW_BY_FORCE_AL)->EnableWindow(FALSE);
		GetDlgItem(IDC_PREM_FORCE_AL)->EnableWindow(FALSE);
		m_iForceContrib = 0;
		m_bShowByForceContrib = FALSE;
		m_iForceRepu = 0;
		m_bShowByForceRepu = FALSE;
		m_iExp = 0;
		m_bShowByExp = FALSE;
		m_iSP = 0;
		m_bShowBySP = FALSE;
		m_iForceAL = 0;
		m_bShowByForceAL = FALSE;
	}
}

void CTaskPremisePage::OnSelTitle()
{
	CTaskTitle dlg(m_pTask, CTaskTitle::TITLE_FOR_PREM);
	dlg.DoModal();
}
