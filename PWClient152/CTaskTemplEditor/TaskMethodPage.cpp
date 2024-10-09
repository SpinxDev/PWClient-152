// TaskMethodPage.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskMethodPage.h"
#include "TemplIDSelDlg.h"
#include "ItemsWantedDlg.h"
#include "VectorDlg.h"
#include "MonWantedDlg.h"
#include "AMemory.h"
#include "DisplayGlobalValue.h"
#include "TaskRegionDlg.h"
#include "PlayerWanted.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
static const unsigned char TRANFORMED_MASK_MAP_4_METHOD[] = 
{
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
int find_transform_index_4_method(unsigned char ucTransform)
{
	for (int i = 0; i < sizeof(TRANFORMED_MASK_MAP_4_METHOD) / sizeof(unsigned char); i++)
		if (TRANFORMED_MASK_MAP_4_METHOD[i] == ucTransform)
			return i;
		
		return 0;
}
// 顺序请参考 客户端EC_RoleTypes.h 对应的表情动作序列
static const char* pEmotion[] = {
	"无",
	"招手",
	"点头",
	"摇头",
	"耸肩膀",
	"大笑",
	"生气",
	"晕倒",
	"沮丧",
	"飞吻",
	"害羞",		
	"抱拳",
	"坐下",
	"冲锋",
	"思考",
	"挑衅",
	"胜利",
	"伸懒腰",
	"战斗",
	"攻击1",
	"攻击2",
	"攻击3",
	"攻击4",
	"防御",
	"摔倒",
	"倒地",
	"张望",
	"舞蹈",
	"时装武器",
	"无",
	"亲吻",
	"亲亲密密",
	"放烟花"
};

static const char* pAction[] = 
{
	"相依相偎",
	"打坐",
	"跳跃翻滚",
	"跑动翻滚",
};

/////////////////////////////////////////////////////////////////////////////
// CTaskMethodPage property page

IMPLEMENT_DYNCREATE(CTaskMethodPage, CPropertyPage)

CTaskMethodPage::CTaskMethodPage() : CPropertyPage(CTaskMethodPage::IDD), m_pTask(0)
{
	//{{AFX_DATA_INIT(CTaskMethodPage)
	m_nFinishType = -1;
	m_nMethod = -1;
	m_dwTimeLen = 0;
	m_strNPCMoving = _T("");
	m_strNPCToProtect = _T("");
	m_dwWaitTime = 0;
	m_dwGoldWanted = 0;
	m_dwFactionContribWanted = 0;
	m_dwFactionExpContribWanted = 0;
	m_dwSiteId = 0;
	m_bNeedComp = FALSE;
	m_nExp1AndOrExp2 = 0;
	m_nLeftType1 = 0;
	m_lLeftNum1  = 0;
	m_nCompOper1 = 0;
	m_nRightType1= 0;
	m_lRightNum1 = 0;
	m_nLeftType2 = 0;
	m_lLeftNum2  = 0;
	m_nCompOper2= 0;
	m_nRightType2= 0;
	m_lRightNum2 = 0;
	m_dwLeaveSiteId = 0;
	m_iZoneStartX = 0;
	m_iZoneStartZ = 0;
	m_iZonesNumX = 1;
	m_iZonesNumZ = 1;
	m_iZoneSide = 10;
	m_iTransformed = 0;
	m_iReachReincarnation = 0;
	m_iReachRealmLevel = 0;
	m_iReachLevel = 0;
	m_iEmotion = 0;
	//}}AFX_DATA_INIT
}

CTaskMethodPage::~CTaskMethodPage()
{
}

void CTaskMethodPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskMethodPage)
	DDX_Control(pDX, IDC_CMB_EMOTION, m_cmbEmotinAction);
	DDX_Control(pDX, IDC_ITEMS_WANTED, m_ItemsBox);
	DDX_Radio(pDX, IDC_TFT_DIRECT, m_nFinishType);
	DDX_Radio(pDX, IDC_TMK_NONE, m_nMethod);
	DDX_Text(pDX, IDC_PROTECT_TIME, m_dwTimeLen);
	DDX_Text(pDX, IDC_NPC_MOVING, m_strNPCMoving);
	DDX_Text(pDX, IDC_NPC_TO_PROTECT, m_strNPCToProtect);
	DDX_Text(pDX, IDC_WAITTIME_LEN, m_dwWaitTime);
	DDX_Text(pDX, IDC_GOLD_WANTED, m_dwGoldWanted);
	DDX_Text(pDX, IDC_FACTION_CONTRIB_WANTED, m_dwFactionContribWanted);
	DDX_Text(pDX, IDC_FACTION_EXP_CONTRIB_WANTED, m_dwFactionExpContribWanted);
	DDX_Text(pDX, IDC_SITE_ID, m_dwSiteId);
	DDX_Check(pDX, IDC_FIN_NEED_COMP, m_bNeedComp);
	DDX_CBIndex(pDX, IDC_FIN_AND_OR, m_nExp1AndOrExp2);
	DDX_CBIndex(pDX, IDC_FIN1_COMPARE1, m_nLeftType1);
	DDX_Text(pDX, IDC_FIN1_KV1, m_lLeftNum1);
	DDX_CBIndex(pDX, IDC_FIN1_COMPARE, m_nCompOper1);
	DDX_CBIndex(pDX, IDC_FIN1_COMPARE2, m_nRightType1);
	DDX_Text(pDX, IDC_FIN1_KV2, m_lRightNum1);	
	DDX_CBIndex(pDX, IDC_FIN2_COMPARE1, m_nLeftType2);
	DDX_Text(pDX, IDC_FIN2_KV1, m_lLeftNum2);
	DDX_CBIndex(pDX, IDC_FIN2_COMPARE, m_nCompOper2);
	DDX_CBIndex(pDX, IDC_FIN2_COMPARE2, m_nRightType2);
	DDX_Text(pDX, IDC_FIN2_KV2, m_lRightNum2);
	DDX_Text(pDX, IDC_SITE_ID2, m_dwLeaveSiteId);
	DDX_Text(pDX, IDC_ZONE_START_X, m_iZoneStartX);
	DDX_Text(pDX, IDC_ZONE_START_Z, m_iZoneStartZ);
	DDX_Text(pDX, IDC_ZONES_NUM_X, m_iZonesNumX);
	DDV_MinMaxInt(pDX,m_iZonesNumX,1,128);
	DDX_Text(pDX, IDC_ZONES_NUM_Z, m_iZonesNumZ);
	DDV_MinMaxInt(pDX,m_iZonesNumZ,1,128);
	DDX_Text(pDX, IDC_ZONE_SIDE, m_iZoneSide);
	DDV_MinMaxInt(pDX,m_iZoneSide,1,50);
	DDX_CBIndex(pDX, IDC_TRANSFORMED, m_iTransformed);
	DDX_Text(pDX, IDC_REACH_LEVEL, m_iReachLevel);
	DDV_MinMaxInt(pDX, m_iReachLevel, 0, 105);
	DDX_Text(pDX, IDC_REACH_REINCARNATION, m_iReachReincarnation);
	DDV_MinMaxInt(pDX, m_iReachReincarnation, 0, 10);
	DDX_CBIndex(pDX, IDC_CMB_EMOTION, m_iEmotion);
	DDX_Text(pDX, IDC_REACH_REINCARNATIONLEVEL, m_iReachRealmLevel);
	DDV_MinMaxInt(pDX, m_iReachRealmLevel, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskMethodPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskMethodPage)
	ON_BN_CLICKED(IDC_SEL_MONSTER, OnSelMonster)
	ON_BN_CLICKED(IDC_SEL_NPC_MOVING, OnSelNpcMoving)
	ON_BN_CLICKED(IDC_SEL_NPC_TO_PROTECT, OnSelNpcToProtect)
	ON_BN_CLICKED(IDC_EDIT_ITEMS_WANTED, OnEditItemsWanted)
	ON_BN_CLICKED(IDC_SEL_MAX_POINT, OnSelMaxPoint)
	ON_BN_CLICKED(IDC_SHOW_TASKCHAR, OnShowTaskchar)
	ON_BN_CLICKED(IDC_SEL_MAX_POINT2, OnSelMaxPoint2)
	ON_BN_CLICKED(IDC_SEL_PLAYER, OnSelPlayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskMethodPage message handlers

BOOL CTaskMethodPage::UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly)
{
	int emotion_cnt = sizeof(pEmotion)/sizeof(pEmotion[0]);

	if (bUpdateData)
	{
		UpdateData();

		if (m_iZonesNumZ * m_iZonesNumX > 65535)
		{
			AfxMessageBox("两个方向上的划分数的乘积不能大于65535！");
			return FALSE;
		}

		if (m_iZoneSide * TASK_TREASURE_MAP_SIDE_MULTIPLE > 1024)
		{
			AfxMessageBox("小区域边长过长！");
			return FALSE;
		}

		pTask->m_enumMethod	= (DWORD)m_nMethod;
		pTask->m_enumFinishType	= (DWORD)m_nFinishType;

		pTask->m_ulNPCToProtect = m_ulNPCToProtect;
		pTask->m_ulProtectTimeLen = m_dwTimeLen;
		pTask->m_ulNPCMoving = m_ulNPCMoving;
		pTask->m_ulWaitTime = m_dwWaitTime;
		pTask->m_ulReachSiteId = m_dwSiteId;
		pTask->m_ulLeaveSiteId = m_dwLeaveSiteId;
		pTask->m_ulGoldWanted = m_dwGoldWanted;
		pTask->m_iFactionContribWanted = m_dwFactionContribWanted;
		pTask->m_iFactionExpContribWanted = m_dwFactionExpContribWanted;

		pTask->m_bFinNeedComp			= TRUE;
		pTask->m_nFinExp1AndOrExp2		= m_nExp1AndOrExp2;

		pTask->m_Fin1KeyValue.nLeftType = m_nLeftType1;
		pTask->m_Fin1KeyValue.lLeftNum  = m_lLeftNum1;
		pTask->m_Fin1KeyValue.nCompOper = m_nCompOper1;
		pTask->m_Fin1KeyValue.nRightType= m_nRightType1;
		pTask->m_Fin1KeyValue.lRightNum = m_lRightNum1;

		pTask->m_Fin2KeyValue.nLeftType = m_nLeftType2;
		pTask->m_Fin2KeyValue.lLeftNum  = m_lLeftNum2;
		pTask->m_Fin2KeyValue.nCompOper = m_nCompOper2;
		pTask->m_Fin2KeyValue.nRightType= m_nRightType2;
		pTask->m_Fin2KeyValue.lRightNum = m_lRightNum2;
		
		pTask->m_TreasureStartZone.x = m_iZoneStartX;
		pTask->m_TreasureStartZone.z = m_iZoneStartZ;
		pTask->m_ucZoneSide = m_iZoneSide;
		pTask->m_ucZonesNumX = m_iZonesNumX;
		pTask->m_ucZonesNumZ = m_iZonesNumZ;
		pTask->m_ucTransformedForm = TRANFORMED_MASK_MAP_4_METHOD[m_iTransformed];
		pTask->m_ulReachLevel = m_iReachLevel;
		pTask->m_ulReachReincarnationCount = m_iReachReincarnation;
		pTask->m_ulReachRealmLevel = m_iReachRealmLevel;

		
		if (m_iEmotion>=emotion_cnt)
		{
			m_iEmotion -= emotion_cnt;
			m_iEmotion += TaskInterface::CMD_EMOTION_BINDBUDDY;
		}
		pTask->m_uiEmotion = m_iEmotion;
	}
	else
	{
		CWnd* pFirst = GetWindow(GW_CHILD);
		while (pFirst)
		{
			if (pFirst->GetDlgCtrlID() != 0xFFFF)
				pFirst->EnableWindow(!bReadOnly);
			pFirst = pFirst->GetNextWindow();
		}

		if (pTask->m_pFirstChild)
		{
			pTask->m_enumMethod = enumTMNone;
			if (!bReadOnly)
			{
				CWnd* pFirst = GetWindow(GW_CHILD);
				while (pFirst)
				{
					if (pFirst->GetDlgCtrlID() != 0xFFFF)
						pFirst->EnableWindow(FALSE);
					pFirst = pFirst->GetNextWindow();
				}

				GetDlgItem(IDC_TFT_DIRECT)->EnableWindow();
				GetDlgItem(IDC_TFT_NPC)->EnableWindow();
			}
		}
		// Begin: update for PQ [Yongdong,2010-1-21]
		if (pTask->m_pParent && pTask->m_pParent->m_bPQTask && pTask->m_bPQSubTask)
		{
			CWnd* pFirst = GetWindow(GW_CHILD);
			while (pFirst)
			{
				if (pFirst->GetDlgCtrlID() != 0xFFFF)
					pFirst->EnableWindow(FALSE);
				pFirst = pFirst->GetNextWindow();
			}
			GetDlgItem(IDC_TM_GLOBAL_VAL_OK)->EnableWindow();
			GetDlgItem(IDC_SHOW_TASKCHAR)->EnableWindow();
			GetDlgItem(IDC_FIN_AND_OR)->EnableWindow();

			GetDlgItem(IDC_FIN1_COMPARE1)->EnableWindow();
			GetDlgItem(IDC_FIN1_KV1)->EnableWindow();
			GetDlgItem(IDC_FIN1_COMPARE)->EnableWindow();
			GetDlgItem(IDC_FIN1_COMPARE2)->EnableWindow();
			GetDlgItem(IDC_FIN1_KV2)->EnableWindow();

			GetDlgItem(IDC_FIN2_COMPARE1)->EnableWindow();
			GetDlgItem(IDC_FIN2_KV1)->EnableWindow();
			GetDlgItem(IDC_FIN2_COMPARE)->EnableWindow();
			GetDlgItem(IDC_FIN2_COMPARE2)->EnableWindow();
			GetDlgItem(IDC_FIN2_KV2)->EnableWindow();


			GetDlgItem(IDC_TFT_DIRECT)->EnableWindow();
			GetDlgItem(IDC_TFT_NPC)->EnableWindow();
		}
		// End.
		
		m_pTask			= pTask;
		m_nMethod		= (int)pTask->m_enumMethod;
		m_nFinishType	= (int)pTask->m_enumFinishType;

		m_ulNPCToProtect = 0;
		m_strNPCToProtect.Empty();
		m_dwTimeLen = 0;
		m_ulNPCMoving = 0;
		m_strNPCMoving.Empty();
		m_dwWaitTime = 0;
		m_ItemsBox.ResetContent();

		ShowItemsWanted();

		m_ulNPCToProtect = pTask->m_ulNPCToProtect;
		m_strNPCToProtect = GetBaseDataNameByID(m_ulNPCToProtect);
		m_dwTimeLen = pTask->m_ulProtectTimeLen;
		m_ulNPCMoving = pTask->m_ulNPCMoving;
		m_strNPCMoving = GetBaseDataNameByID(m_ulNPCMoving);
		m_dwWaitTime = pTask->m_ulWaitTime;
		m_dwSiteId = pTask->m_ulReachSiteId;
		m_dwLeaveSiteId = pTask->m_ulLeaveSiteId;
		m_dwGoldWanted = pTask->m_ulGoldWanted;
		m_dwFactionContribWanted = pTask->m_iFactionContribWanted;
		m_dwFactionExpContribWanted = pTask->m_iFactionExpContribWanted;

		m_bNeedComp		 = TRUE;
		m_nExp1AndOrExp2 = pTask->m_nFinExp1AndOrExp2;

		m_nLeftType1     = pTask->m_Fin1KeyValue.nLeftType; 
		m_lLeftNum1      = pTask->m_Fin1KeyValue.lLeftNum;
		m_nCompOper1     = pTask->m_Fin1KeyValue.nCompOper;
		m_nRightType1    = pTask->m_Fin1KeyValue.nRightType;
		m_lRightNum1     = pTask->m_Fin1KeyValue.lRightNum ;

		m_nLeftType2     = pTask->m_Fin2KeyValue.nLeftType; 
		m_lLeftNum2      = pTask->m_Fin2KeyValue.lLeftNum;
		m_nCompOper2     = pTask->m_Fin2KeyValue.nCompOper;
		m_nRightType2    = pTask->m_Fin2KeyValue.nRightType;
		m_lRightNum2     = pTask->m_Fin2KeyValue.lRightNum ;

		m_iZoneStartX = pTask->m_TreasureStartZone.x;
		m_iZoneStartZ = pTask->m_TreasureStartZone.z;
		m_iZoneSide = pTask->m_ucZoneSide;
		m_iZonesNumX = pTask->m_ucZonesNumX;
		m_iZonesNumZ = pTask->m_ucZonesNumZ;
		m_iTransformed = find_transform_index_4_method(pTask->m_ucTransformedForm);
		m_iReachLevel = pTask->m_ulReachLevel;
		m_iReachReincarnation = pTask->m_ulReachReincarnationCount;
		m_iReachRealmLevel = pTask->m_ulReachRealmLevel;

		if (pTask->m_uiEmotion >= TaskInterface::CMD_EMOTION_BINDBUDDY)
		{
			m_iEmotion = pTask->m_uiEmotion - TaskInterface::CMD_EMOTION_BINDBUDDY + emotion_cnt;
		}
		else
			m_iEmotion = pTask->m_uiEmotion;
		

		UpdateData(FALSE);
	}

	return TRUE;
}

void CTaskMethodPage::OnSelMonster()
{
	if (!m_pTask) return;

	CMonWantedDlg dlg(m_pTask->m_MonsterWanted, &m_pTask->m_ulMonsterWanted);
	dlg.DoModal();
}

void CTaskMethodPage::OnSelNpcMoving() 
{
	if (!m_pTask) return;
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\NPC\\功能NPC";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = m_ulNPCMoving;

	if (dlg.DoModal() == IDOK)
	{
		m_ulNPCMoving = dlg.m_ulID;
		GetDlgItem(IDC_NPC_MOVING)->SetWindowText(dlg.m_strName);
	}
}

void CTaskMethodPage::OnSelNpcToProtect()
{
	if (!m_pTask) return;
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\怪物";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = m_ulNPCToProtect;

	if (dlg.DoModal() == IDOK)
	{
		m_ulNPCToProtect = dlg.m_ulID;
		GetDlgItem(IDC_NPC_TO_PROTECT)->SetWindowText(dlg.m_strName);
	}
}

void CTaskMethodPage::OnEditItemsWanted() 
{
	if (!m_pTask) return;

	CItemsWantedDlg dlg(
		MAX_ITEM_WANTED,
		m_pTask->m_ItemsWanted,
		&m_pTask->m_ulItemsWanted);

	dlg.DoModal();
	ShowItemsWanted();
}

void CTaskMethodPage::ShowItemsWanted()
{
	m_ItemsBox.ResetContent();

	for (unsigned long i = 0; i < m_pTask->m_ulItemsWanted; i++)
	{
		ITEM_WANTED& wi = m_pTask->m_ItemsWanted[i];

		CString str;
		str.Format(
			"%s(%d个)",
			GetBaseDataNameByID(wi.m_ulItemTemplId),
			wi.m_ulItemNum);

		m_ItemsBox.AddString(str);
	}

	if (m_ItemsBox.GetCount()) m_ItemsBox.SetCurSel(0);
}

void CTaskMethodPage::OnSelMaxPoint() 
{
	if (!m_pTask) return;

	CTaskRegionDlg dlg(m_pTask->m_pReachSite,m_pTask->m_ulReachSiteCnt);
	dlg.DoModal();
}

void CTaskMethodPage::OnShowTaskchar() 
{
	// TODO: Add your control notification handler code here
	CDisplayGlobalValue dlg(m_pTask);
	dlg.DoModal();
}

void CTaskMethodPage::OnSelMaxPoint2() 
{
	if (!m_pTask) return;

	CTaskRegionDlg dlg(m_pTask->m_pLeaveSite,m_pTask->m_ulLeaveSiteCnt);
	dlg.DoModal();
}

void CTaskMethodPage::OnSelPlayer() 
{
	// TODO: Add your control notification handler code here
	if (!m_pTask)
	{
		return;
	}
	PlayerWanted dlg(m_pTask->m_PlayerWanted,&m_pTask->m_ulPlayerWanted);
	dlg.DoModal();
}

BOOL CTaskMethodPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_cmbEmotinAction.ResetContent();
	int i=0;
	for (i=0;i<sizeof(pEmotion)/sizeof(pEmotion[0]);i++)
	{
		m_cmbEmotinAction.AddString(pEmotion[i]);
	}
	for (i=0;i<sizeof(pAction)/sizeof(pAction[0]);i++)
	{
		m_cmbEmotinAction.AddString(pAction[i]);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
