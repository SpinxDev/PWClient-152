// Filename	: DlgWedding.h
// Creator	: Xu Wenbin
// Date		: 2010/07/06

#include "AFI.h"
#include "DlgWedding.h"
#include "DlgActivity.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Resource.h"
#include "EC_IvtrConsume.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_Team.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_Time.h"
#include "elementdataman.h"
#include "AUIRadioButton.h"
#include "CSplit.h"
#include <A2DSprite.h>
#include <time.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWedding, CDlgBase)
AUI_ON_COMMAND("prevmonth",	OnCommand_PrevMonth)
AUI_ON_COMMAND("nextmonth",	OnCommand_NextMonth)
AUI_ON_COMMAND("Btn_Book",	OnCommand_Book)
AUI_ON_COMMAND("Btn_CancelBook",	OnCommand_CancelBook)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_Cancel)
AUI_ON_COMMAND("Rdo_*",		OnCommand_Rdo)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWedding, CDlgBase)
AUI_ON_EVENT("Img_*", WM_LBUTTONDOWN, OnEventLButtonDown_Img)
AUI_ON_EVENT("Img_Special", WM_RBUTTONUP, OnEventRButtonUP_Img)
AUI_ON_EVENT("Txt_Sun*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_Mon*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_Tue*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_Wed*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_Thu*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_Fri*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_Sat*", WM_LBUTTONDOWN, OnEventLButtonDown_Txt)
AUI_ON_EVENT("Txt_List1", WM_LBUTTONDOWN, OnEventLButtonDown_List1)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgWedding
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgWedding> WeddingClickShortcut;
//------------------------------------------------------------------------

CDlgWedding::CDlgWedding()
{
	::ZeroMemory(m_pImg_Week, sizeof(m_pImg_Week));
	::ZeroMemory(m_pLab_Week, sizeof(m_pLab_Week));
	m_pTxt_List1 = NULL;
	m_pTxt_Title = NULL;
	m_pImg_Special = NULL;
	m_pBtn_PrevMonth = NULL;
	m_pBtn_NextMonth = NULL;
	m_pBtn_Book = NULL;
	m_pBtn_CancelBook = NULL;
	m_curYear = m_curMonth = m_curDay = 0;
	m_selYear = m_selMonth = m_selDay = 0;
	m_selScene = -1;
	m_iSlot = -1;
	m_pSprite_Icon1 = NULL;
	m_pSprite_Icon2 = NULL;
	m_bSpriteInited = false;
}

bool CDlgWedding::Release()
{
	A3DRELEASE(m_pSprite_Icon1);
	A3DRELEASE(m_pSprite_Icon2);
	m_bSpriteInited = false;
	return CDlgBase::Release();
}

bool CDlgWedding::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new WeddingClickShortcut(this));

	DDX_Control("Txt_title", m_pTxt_Title);
	DDX_Control("Txt_List1", m_pTxt_List1);
	DDX_Control("Img_Special", m_pImg_Special);
	DDX_Control("Btn_PrevMonth", m_pBtn_PrevMonth);
	DDX_Control("Btn_NextMonth", m_pBtn_NextMonth);
	DDX_Control("Btn_Book", m_pBtn_Book);
	DDX_Control("Btn_CancelBook", m_pBtn_CancelBook);

	m_pTxt_Title->SetText(_AL(""));

	AString imgFormat[7] = {"Img_Sunday%d",
		"Img_Monday%d",
		"Img_Tuesday%d",
		"Img_Wednesday%d",
		"Img_Thursday%d",
		"Img_Friday%d",
		"Img_Saturday%d"};

	AString labFormat[7] = {"Txt_Sunday%d",
		"Txt_Monday%d",
		"Txt_Tuesday%d",
		"Txt_Wednesday%d",
		"Txt_Thursday%d",
		"Txt_Friday%d",
		"Txt_Saturday%d"};

	AString strName;
	PAUIIMAGEPICTURE pImage(NULL);
	PAUILABEL pLabel(NULL);
	for (int i = 0; i < 6; ++ i)
	{
		for (int j = 0; j < 7; ++ j)
		{
			strName.Format(imgFormat[j], i);
			pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
			m_pImg_Week[i][j] = pImage;
			
			strName.Format(labFormat[j], i);
			pLabel = static_cast<PAUILABEL>(GetDlgItem(strName));
			pLabel->SetFontAndColor(_AL("方正细黑一简体"), 12, 0xFFFFFFFF, 0, 1, true);
			m_pLab_Week[i][j] = pLabel;
		}
	}
	
	elementdataman* pDB = GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	WEDDING_CONFIG *pWeddingConfig = (WEDDING_CONFIG*)pDB->get_data_ptr(CECIvtrWeddingInviteCard::WEDDING_CONFIG_ID, ID_SPACE_CONFIG, DataType);
	if (DataType!=DT_WEDDING_CONFIG)
		pWeddingConfig = NULL;
	int nMaxScene = 0;
	if (pWeddingConfig)
	{
		nMaxScene = sizeof(pWeddingConfig->wedding_scene)/sizeof(pWeddingConfig->wedding_scene[0]);
	}

	// 处理结婚场景控件
	PAUIOBJECT pRdo(NULL), pTxt(NULL);
	int k(0);
	while (true)
	{
		strName.Format("Rdo_%02d", ++k);
		pRdo = GetDlgItem(strName);
		if (!pRdo)
		{
			// 没有更多的控件了
			break;
		}

		strName.Format("Txt_%02d", k);
		pTxt = GetDlgItem(strName);
		if (pWeddingConfig && k<=nMaxScene &&
			pWeddingConfig->wedding_scene[k-1].name[0] &&
			pTxt != NULL)
		{
			// 当文本有效时，显示内容
			pTxt->SetText(pWeddingConfig->wedding_scene[k-1].name);
		}
		else
		{
			// 当文本无效时，隐藏内容
			pRdo->Show(false);
			if (pTxt)
				pTxt->Show(false);
		}
	}

	return true;
}

void CDlgWedding::OnShowDialog()
{
	//	重置，以便重新开始
	Reset();

	//	从服务器查询预订列表
	GetWeddingBookListFromServer();
}

void CDlgWedding::OnTick()
{
	// 获取系统当前时间并显示
	char hour, minute, second;
	GetCurServerTime(m_curYear, m_curMonth, m_curDay, hour, minute, second);

	ACString strText;
	strText.Format(GetStringFromTable(8010), m_curYear, m_curMonth, m_curDay, hour, minute);
	m_pTxt_Title->SetText(strText);

	//	更新列表中的玩家名称显示
	UpdatePlayerNames();
}

void CDlgWedding::OnMonthSelectChange()
{
	// 更新当前月份的显示
	PAUIOBJECT pTxt_Month = GetDlgItem("Txt_Month");
	if (m_selMonth > 0)
	{
		ACString strText;
		strText.Format(_AL("%d"), m_selMonth);
		pTxt_Month->SetText(strText);
	}
	else
	{
		pTxt_Month->SetText(_AL(""));
	}
	m_pBtn_PrevMonth->Enable(CanGoPrevMonth());
	m_pBtn_NextMonth->Enable(CanGoNextMonth());

	// 更新日历显示
	UpdateCalendar();

	// 计算选中所有月的第一个有效日期（可能没有有效日期）
	m_selDay = GetFirstSelectDay();
	OnDaySelectChange();
}

void CDlgWedding::UpdateCalendar()
{
	// 更新当前选中月份的日历显示

	// 计算当前月份的所有日子在图片阵列的起止范围（范围可法初始化）
	int start(-1), end(-1);
	if (m_selMonth != 0)
	{
		// 当前选中月份已经初始化

		// start 0 对应 Sunday
		start = CECTime::GetWeek(m_selYear, m_selMonth, 1);
		start = (start + 1)%7;

		int count = CECTime::GetMonthDayCount(m_selYear, m_selMonth);
		end = start+count;
	}

	// 更新日历显示（范围未初始化时，显示空）
	ACString strDay;
	int p(0);
	for (int row = 0; row < 6; ++ row)
	{
		for (int col = 0; col < 7; ++ col, ++ p)
		{
			PAUIIMAGEPICTURE pImage = m_pImg_Week[row][col];
			PAUILABEL pLabel = m_pLab_Week[row][col];

			if (p<start || p>=end)
			{
				pImage->SetCover(NULL, -1);
				pImage->SetData(0);
				pLabel->SetText(_AL(""));
			}
			else
			{
				int day = p-start+1;
				strDay.Format(_AL("%d"), day);
				pLabel->SetText(strDay);
				pLabel->SetData(day);

				pImage->SetData(day);
				SetIcon(pImage, HaveSessionDay(m_selYear, m_selMonth, day));
			}
		}
	}
}

void CDlgWedding::SetIcon(PAUIIMAGEPICTURE pImage, bool bWithSessionList)
{
	if (!m_bSpriteInited)
	{
		m_pSprite_Icon1 = LoadSprite("window\\婚礼排期a.dds");
		m_pSprite_Icon2 = LoadSprite("window\\婚礼排期b.dds");
		m_bSpriteInited = true;
	}
	if (pImage)
	{
		A2DSprite * &pSprite = bWithSessionList ? m_pSprite_Icon1 : m_pSprite_Icon2;
		if (pSprite)
			pImage->SetCover(pSprite, 0);
		else
			pImage->SetCover(NULL, -1);
	}
}

A2DSprite * CDlgWedding::LoadSprite(AString strFile)
{
	A2DSprite *pSprite = NULL;
	if (!strFile.IsEmpty())
	{
		pSprite = new A2DSprite;
		if (!pSprite->Init(GetGame()->GetA3DDevice(), strFile, 0))
			A3DRELEASE(pSprite);
	}
	return pSprite;
}

void CDlgWedding::UpdateCalendarColor()
{
	//	高亮显示被选中日子、能预订的日子
	//
	for (int row = 0; row < 6; ++ row)
	{
		for (int col = 0; col < 7; ++ col)
		{
			PAUIIMAGEPICTURE p = m_pImg_Week[row][col];
			int nDay = p->GetData();
			if(nDay == m_selDay)
			{
				//	被选中日期（红色显示）
				p->SetColor(A3DCOLORRGB(255, 0, 0));
			}
			else
			{
				//	其它日期
				p->SetColor(A3DCOLORRGB(255, 255, 255));
			}
		}
	}
}

void CDlgWedding::OnDaySelectChange()
{
	//	更新被选中时图标状态
	UpdateCalendarColor();

	//	选取当前第一个场景
	m_selScene = GetFirstScene();
	OnWeddingSceneSelectChange();
}

void CDlgWedding::OnWeddingSceneSelectChange()
{
	//	更新场景选择界面显示
	CheckRadioButton(0, m_selScene>=0 ? m_selScene : -1);

	//	清空场次列表
	m_pTxt_List1->ResetContent();

	//	查询列表
	RebuildSessionList();

	// 选取第一项为默认项
	if (m_pTxt_List1->GetCount() > 0)
		m_pTxt_List1->SetCurSel(0);
	OnWeddingSessionSelectChange();
}

void CDlgWedding::OnWeddingSessionSelectChange()
{
	//	更新按钮控制
	m_pBtn_Book->Enable(CanBook());
	m_pBtn_CancelBook->Enable(CanCancelBook());
}

void CDlgWedding::OnEventLButtonDown_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIIMAGEPICTURE pImage = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if (pImage)
	{
		if (stricmp(pImage->GetName(), "Img_Special"))
		{
			//	特殊物品的处理
			return;
		}

		//	日历的处理
		int nDay = pImage->GetData();
		if (nDay > 0 && m_selDay != nDay && CanShowDay(m_selYear, m_selMonth, nDay))
		{
			m_selDay = nDay;
			OnDaySelectChange();
		}
	}	
}

void CDlgWedding::OnEventRButtonUP_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//	清除特殊物品
	SetItem(-1);
}

void CDlgWedding::OnEventLButtonDown_Txt(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUILABEL pLabel = static_cast<PAUILABEL>(pObj);
	if (pLabel)
	{
		int nDay = pLabel->GetData();
		if (nDay > 0 && m_selDay != nDay && CanShowDay(m_selYear, m_selMonth, nDay))
		{
			m_selDay = nDay;
			OnDaySelectChange();
		}
	}
}

void CDlgWedding::OnEventLButtonDown_List1(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnWeddingSessionSelectChange();
}

void CDlgWedding::OnCommand_PrevMonth(const char *szCommand)
{
	if (!CanGoPrevMonth())
		return;

	int year(m_selYear), month(m_selMonth);
	GetPrevMonth(year, month, m_selYear, m_selMonth);

	if (year == m_selYear && month == m_selMonth)
		return;

	OnMonthSelectChange();
}

void CDlgWedding::OnCommand_NextMonth(const char *szCommand)
{
	if (!CanGoNextMonth())
		return;

	int year(m_selYear), month(m_selMonth);
	GetNextMonth(year, month, m_selYear, m_selMonth);

	if (year == m_selYear && month == m_selMonth)
		return;

	OnMonthSelectChange();
}

void CDlgWedding::OnCommand_Rdo(const char *szCommand)
{
	//	切换场景
	if (szCommand && szCommand[0] && strstr(szCommand, "Rdo_"))
	{
		int nSel = atoi(szCommand + strlen("Rdo_"));
		if (nSel >= 1)
		{
			m_selScene = nSel-1;
			OnWeddingSceneSelectChange();
		}
	}
}

void CDlgWedding::OnCommand_Book(const char *szCommand)
{
	if (!CanBook())
		return;

	//	发送预订协议
	int nSession = GetSelectedSession();
	const wedding_book &temp = m_bookList[nSession];
	GetGameSession()->c2s_CmdNPCSevWeddingBook(1, temp.start_time, temp.end_time, temp.scene, m_iSlot);

	//	清除预约物品
	SetItem(-1);
}

void CDlgWedding::OnCommand_CancelBook(const char *szCommand)
{
	if (!CanCancelBook())
		return;

	//	弹出确认对话框	
	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_CancelBook", GetStringFromTable(8775),
		MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
}

void CDlgWedding::CancelBook(bool bConfirm)
{
	if (bConfirm)
	{
		//	确认取消
		
		if (!CanCancelBook())
			return;
		
		//	发送取消预订协议
		int nSession = GetSelectedSession();
		const wedding_book &temp = m_bookList[nSession];
		GetGameSession()->c2s_CmdNPCSevWeddingBook(2, temp.start_time, temp.end_time, temp.scene, -1);
	}
}

void CDlgWedding::OnCommand_Cancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	SetItem(-1);
	Show(false);
}

int CDlgWedding::GetFirstSelectDay()
{
	//	参照当前时间，计算当前选中月份里第一个可选中的日子
	//
	int nFirst(-1);

	if (m_curMonth > 0 && m_selMonth > 0)
	{
		int count = CECTime::GetMonthDayCount(m_selYear, m_selMonth);

		if (m_selYear == m_curYear && m_selMonth == m_curMonth)
		{
			//	当前月份，从当前日开始查询
			for (int nDay = m_curDay; nDay <= count; ++ nDay)
			{
				if (CanShowDay(m_selYear, m_selMonth, nDay))
				{
					nFirst = nDay;
					break;
				}
			}
			if (nFirst <= 0)
			{
				//	没找到，从月初开始找
				for (int nDay = 1; nDay<m_curDay; ++ nDay)
				{
					if (CanShowDay(m_selYear, m_selMonth, nDay))
					{
						nFirst = nDay;
						break;
					}
				}
			}
		}
		else
		{
			//	其它月份（应该是后继月份），从月初开始查询
			for (int nDay = 1; nDay <= count; ++ nDay)
			{
				if (CanShowDay(m_selYear, m_selMonth, nDay))
				{
					nFirst = nDay;
					break;
				}
			}
		}
	}

	return nFirst;
}

int CDlgWedding::GetFirstScene()
{
	//	根据配置表里的顺序及控件情况，查找第一个场景

	int nFirst(-1);
	
	AString strName;
	PAUIOBJECT pRdo(NULL);
	int k(0);
	while (true)
	{
		strName.Format("Rdo_%02d", ++k);
		pRdo = GetDlgItem(strName);
		if (!pRdo)
		{
			// 没有更多的控件了
			break;
		}
		PAUIRADIOBUTTON pRadio = dynamic_cast<PAUIRADIOBUTTON>(pRdo);
		if (pRadio && pRadio->IsShow())
		{
			nFirst = k-1;
			break;
		}
	}

	return nFirst;
}

bool CDlgWedding::CanShowMonth(int year, int month)
{
	//	参照当前日期，判断能否显示此月日历
	//	只要当前月有某天可以显示，即需要显示此月日历
	//
	bool bShow(false);

	if (year > 0 && month > 0)
	{
		int count = CECTime::GetMonthDayCount(year, month);
		for (int nDay = 1; nDay <= count; ++ nDay)
		{
			if (CanShowDay(year, month, nDay))
			{
				bShow = true;
				break;
			}
		}
	}

	return bShow;
}

bool CDlgWedding::CanShowDay(int year, int month, int day)
{
	//	参考当前日期，是否可显示此日

	bool bShow(false);

	while (true)
	{
		//	检查传入日期是否正确
		if (year <= 0 || month <= 0 || month >= 13 || day <= 0)
			break;		
		int count = CECTime::GetMonthDayCount(year, month);
		if (day > count)
			break;

		//	检查当前预订列表
		//
		int temp_year(0), temp_month(0), temp_day(0);
		for (size_t i(0); i < m_bookList.size(); ++ i)
		{
			const wedding_book & cur = m_bookList[i];
			GetServerLocalTime(cur.start_time, temp_year, temp_month, temp_day);
			if (temp_year == year && temp_month == month)
			{
				//	传入日期所在月份有某天含预订列表，则此日可显示
				bShow = true;
				break;
			}
		}
		break;
	}

	return bShow;
}

bool CDlgWedding::HaveSessionDay(int year, int month, int day)
{
	//	判断当前日期是否有可显示的预订列表
	//
	
	bool bHave(false);

	while (true)
	{		
		//	检查传入日期是否正确
		if (year <= 0 || month <= 0 || month >= 13 || day <= 0)
			break;		
		int count = CECTime::GetMonthDayCount(year, month);
		if (day > count)
			break;

		//	查询预订列表
		int temp_year(0), temp_month(0), temp_day(0);
		for (size_t i(0); i < m_bookList.size(); ++ i)
		{
			const wedding_book & cur = m_bookList[i];
			GetServerLocalTime(cur.start_time, temp_year, temp_month, temp_day);
			if (temp_year == year && temp_month == month && temp_day == day)
			{
				//	传入参数给定天有预订列表
				bHave = true;
				break;
			}
		}
		
		break;
	}

	return bHave;
}

bool CDlgWedding::IsSpecialDay(int year, int month, int day)
{
	//	判断给定日期是否是特殊日子
	bool bSpecial(false);

	while (true)
	{
		//	获取结婚配置表，内有特殊日期
		//
		elementdataman* pDB = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		WEDDING_CONFIG *pWeddingConfig = (WEDDING_CONFIG*)pDB->get_data_ptr(CECIvtrWeddingInviteCard::WEDDING_CONFIG_ID, ID_SPACE_CONFIG, DataType);
		if (!pWeddingConfig || DataType!=DT_WEDDING_CONFIG)
			break;

		int nMaxReserved = sizeof(pWeddingConfig->reserved_day)/sizeof(pWeddingConfig->reserved_day[0]);
		for (int i = 0; i < nMaxReserved; ++ i)
		{
			if (pWeddingConfig->reserved_day[i].year <= 0 ||
				pWeddingConfig->reserved_day[i].month <= 0 ||
				pWeddingConfig->reserved_day[i].day <= 0 )
			{
				//	无效项
				continue;
			}
			if (year != pWeddingConfig->reserved_day[i].year ||
				month != pWeddingConfig->reserved_day[i].month ||
				day != pWeddingConfig->reserved_day[i].day)
			{
				//	非此特殊日期
				continue;
			}

			//	找到一对应的特殊日期
			bSpecial = true;
			break;
		}

		break;
	}

	return bSpecial;
}

bool CDlgWedding::HaveSpecialDayItem(int year, int month, int day)
{
	bool bHave(false);

	while (true)
	{
		if (!m_pImg_Special)
			break;

		if (!IsImageItemValid(m_iSlot))
			break;

		CECHostPlayer *pHost = GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		CECIvtrItem *pItem = pPack->GetItem(m_iSlot);
		CECIvtrWeddingBookCard *pBookCard = dynamic_cast<CECIvtrWeddingBookCard*>(pItem);
		const IVTR_ESSENCE_WEDDING_BOOKCARD &essence = pBookCard->GetEssence();
		if (year != essence.year ||
			month != essence.month ||
			day != essence.day)
		{
			break;
		}

		bHave = true;
		break;
	}

	return bHave;
}

bool CDlgWedding::HaveInviteCard(const wedding_book &session)
{
	//	查找包裹中是否有相应请柬（专为新人所有）
	//
	bool bHave(false);	
	
	CECHostPlayer *pHost = GetHostPlayer();
	int id = pHost->GetCharacterID();
	CECInventory *pPack = pHost->GetPack();
	for (int i(0); i<pPack->GetSize(); ++ i)
	{
		CECIvtrItem *pItem = pPack->GetItem(i);
		if (pItem && pItem->GetClassID() == CECIvtrItem::ICID_WEDDINGINVITECARD)
		{
			CECIvtrWeddingInviteCard *pInviteCard = dynamic_cast<CECIvtrWeddingInviteCard*>(pItem);
			if (pInviteCard)
			{
				const IVTR_ESSENCE_WEDDING_INVITECARD &essence = pInviteCard->GetEssence();
				if (essence.start_time == session.start_time &&
					essence.end_time == session.end_time &&
					essence.scene == session.scene &&
					essence.groom == session.groom &&
					essence.bride == session.bride &&
					essence.invitee == id)
				{
					bHave = true;
					break;
				}
			}
		}
	}

	return bHave;
}

bool CDlgWedding::CanGoPrevMonth()
{
	bool bCan(false);

	if (m_selMonth > 0)
	{
		int year(0), month(0);
		GetPrevMonth(m_selYear, m_selMonth, year, month);
		if (CanShowMonth(year, month))
			bCan = true;
	}

	return bCan;
}

bool CDlgWedding::CanGoNextMonth()
{
	bool bCan(false);

	if (m_selMonth > 0)
	{
		int year(0), month(0);
		GetNextMonth(m_selYear, m_selMonth, year, month);
		if (CanShowMonth(year, month))
			bCan = true;
	}

	return bCan;
}

CECPlayer * CDlgWedding::GetTeamOtherPlayer()
{
	//	查找符合要求的二人组队中的另一个
	//

	CECPlayer *pPlayerRet = NULL;

	while (true)
	{
		CECHostPlayer *pHost = GetHostPlayer();

		CECTeam * pTeam = pHost->GetTeam();
		if (!pTeam ||
			pTeam->GetMemberNum() != 2 ||
			pTeam->GetLeaderID() != pHost->GetCharacterID())
		{
			//	不是2人组队，或者不是队长
			break;
		}

		int iHostTeamIndex(0);
		CECTeamMember *pOtherMember = NULL;
		CECPlayer *pOtherPlayer = NULL;
		if (!pTeam->GetMemberByID(pHost->GetCharacterID(), &iHostTeamIndex) ||
			!(pOtherMember = pTeam->GetMemberByIndex(1 - iHostTeamIndex)) ||
			!(pOtherPlayer = GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(pOtherMember->GetCharacterID())) ||
			pOtherPlayer->GetGender() == pHost->GetGender())
		{
			//	组队的二人性别不符
			break;
		}

		pPlayerRet = pOtherPlayer;
		break;
	}

	return pPlayerRet;
}

bool CDlgWedding::CheckTeamCondition(bool bBook)
{
	//	检查 预订/取消预订 的组队条件
	//
	return GetTeamOtherPlayer() != NULL;
}

bool CDlgWedding::CanBook()
{
	//	能否预订当前选中的日期、场景、场次
	//
	bool bCan(false);

	while (true)
	{
		if (!CheckTeamCondition(true))
		{
			//	预订组队条件不符合
			break;
		}

		if (m_selMonth <= 0)
		{
			//	尚未初始化
			break;
		}

		if (m_selScene < 0)
		{
			//	场景未选择
			break;
		}

		int nSession = GetSelectedSession();
		if (nSession < 0)
		{
			//	场次未选择
			break;
		}

		if (!CanBookSession(nSession))
		{
			//	不允许预订
			break;
		}

		bCan = true;
		break;
	}
	return bCan;
}

bool CDlgWedding::CanCancelBook()
{
	//	能否预订当前选中的日期、场景、场次
	//
	bool bCan(false);

	while (true)
	{
		if (!CheckTeamCondition(false))
		{
			//	取消预订组队条件不符合
			break;
		}

		if (m_selMonth <= 0)
		{
			//	尚未初始化
			break;
		}

		if (m_selScene < 0)
		{
			//	场景未选择
			break;
		}

		int nSession = GetSelectedSession();
		if (nSession < 0)
		{
			//	场次未选择
			break;
		}

		if (!CanCancelBookSession(nSession))
		{
			//	不允许取消预订
			break;
		}

		bCan = true;
		break;
	}
	return bCan;
}

void CDlgWedding::GetPrevMonth(int year, int month, int &prevMonthYear, int &prevMonth)
{
	if (month == 1)
	{
		prevMonthYear = year-1;
		prevMonth = 12;
	}
	else
	{
		prevMonthYear = year;
		prevMonth = month-1;
	}
}

void CDlgWedding::GetNextMonth(int year, int month, int &nextMonthYear, int &nextMonth)
{
	if (month == 12)
	{
		nextMonthYear = year+1;
		nextMonth = 1;
	}
	else
	{
		nextMonthYear = year;
		nextMonth = month+1;
	}
}

bool CDlgWedding::IsContinuousMonth(int year1, int month1, int year2, int month2)
{
	int tempYear, tempMonth;
	GetNextMonth(year1, month1, tempYear, tempMonth);
	return (tempYear == year2) && (tempMonth == month2);
}

void CDlgWedding::SetItem(int iSlot /* = -1 */)
{
	bool bItemChanged(false);

	while (true)
	{
		CECIvtrItem *pItem = NULL;
		CECHostPlayer *pHost = GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		if (IsImageItemValid(iSlot))
			pItem = pPack->GetItem(iSlot);
		
		//	参数合法
		CECIvtrItem *pItemOld = NULL;
		if (IsImageItemValid(m_iSlot))
			pItemOld = pPack->GetItem(m_iSlot);

		//	判断重复设置
		if (pItemOld != NULL &&
			pItem == pItemOld)
		{
			//	仅在非空时执行
			break;
		}

		bItemChanged = true;
		
		//	清除图标
		m_pImg_Special->SetCover(NULL, -1);
		
		if (pItemOld)
			pItemOld->Freeze(false);
		
		//	显示新结果
		m_iSlot = iSlot;
		
		if (!pItem)
			break;
		
		pItem->Freeze(true);
		
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Special->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

		break;
	}

	if (bItemChanged)
	{
		//	链接物品发生改变，影响预约和取消预约条件
		OnWeddingSessionSelectChange();
	}
}

bool CDlgWedding::IsImageItemValid(int iSlot)
{
	bool bValid(false);
	while (true)
	{
		CECHostPlayer *pHost = GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		if (iSlot < 0 && iSlot >= pPack->GetSize())
			break;
		
		CECIvtrItem *pItem = pPack->GetItem(iSlot);
		if (!pItem)
			break;
				
		if (pItem->GetClassID() != CECIvtrItem::ICID_WEDDINGBOOKCARD)
			break;
		
		CECIvtrWeddingBookCard *pBookCard = dynamic_cast<CECIvtrWeddingBookCard*>(pItem);
		if (!pBookCard)
			break;
		
		const IVTR_ESSENCE_WEDDING_BOOKCARD &essence = pBookCard->GetEssence();
		if (essence.year <= 0 ||
			essence.month <= 0 ||
			essence.day <= 0)
		{
			break;
		}
		
		bValid = true;
		break;
	}
	return bValid;
}

void CDlgWedding::OnWeddingBookList(void *pData)
{
	//	清除旧有列表
	m_bookList.clear();

	//	生成新列表
	if (pData != NULL)
	{
		wedding_book temp;
		S2C::cmd_wedding_book_list *pWeddingBookList = (S2C::cmd_wedding_book_list *)pData;
		for (int i = 0; i < pWeddingBookList->count; ++ i)
		{
			temp.start_time = pWeddingBookList->list[i].start_time;
			temp.end_time = pWeddingBookList->list[i].end_time;
			temp.groom = pWeddingBookList->list[i].groom;
			temp.bride = pWeddingBookList->list[i].bride;
			temp.scene = pWeddingBookList->list[i].scene;
			temp.status = pWeddingBookList->list[i].status;
			temp.special = pWeddingBookList->list[i].special;
			
			m_bookList.push_back(temp);
		}
	}

	//	通知界面
	OnBookListChange();
}

void CDlgWedding::OnBookListChange()
{
	//	预约列表已经发生变化
	//

	//	搜索可以显示的月份

	if (m_selMonth > 0 && CanShowMonth(m_selYear, m_selMonth))
	{
		//	上次列表对应的月份还能够显示，则不改变月份

		//	更新月份翻页（因为列表变化）		
		m_pBtn_PrevMonth->Enable(CanGoPrevMonth());
		m_pBtn_NextMonth->Enable(CanGoNextMonth());

		//	更新日历（日历中的图标可能发生变化）
		UpdateCalendar();

		if (CanShowDay(m_selYear, m_selMonth, m_selDay))
		{
			//	上次显示的日子还能够显示，则不改变日子

			//	更新日历选中显示（因为日历更新）
			UpdateCalendarColor();

			//	更新界面列表（因为列表变化）
			RebuildSessionList();
			
			// 选取第一项为默认项
			if (m_pTxt_List1->GetCount() > 0)
				m_pTxt_List1->SetCurSel(0);
			OnWeddingSessionSelectChange();

			return;
		}

		//	搜索第一个可显示的天
		m_selDay = GetFirstSelectDay();
		OnDaySelectChange();
		return;
	}

	//	从当前年月出发，查找第一个可以显示的月份
	m_selYear = m_selMonth = m_selDay = 0;
	if (CanShowMonth(m_curYear, m_curMonth))
	{
		//	当前月份能显示
		m_selYear = m_curYear;
		m_selMonth = m_curMonth;
	}
	else
	{
		//	从列表中查找第一个年月
		if (!m_bookList.empty())
		{
			const wedding_book & cur = m_bookList[0];
			GetServerLocalTime(cur.start_time, m_selYear, m_selMonth, m_selDay);
		}
	}
	
	OnMonthSelectChange();
}

int CDlgWedding::GetSelectedSession()
{
	//	查找当前选中项，当可以预订时返回
	int nSession(-1);

	while (true)
	{
		int nSel = m_pTxt_List1->GetCurSel();
		int nCount = m_pTxt_List1->GetCount();
		if (nSel < 0 || nSel >= nCount)
		{
			//	没有选中场次
			break;
		}

		int candidate = m_pTxt_List1->GetItemData(nSel);
		if (candidate < 0 || candidate >= (int)m_bookList.size())
		{
			//	选中项的内容不合法
			break;
		}

		nSession = candidate;
		break;
	}

	return nSession;
}

bool CDlgWedding::CanBookSession(int nSession)
{
	//	判断指定场次是否可预定
	//
	bool bCan(false);

	while (true)
	{		
		if (nSession < 0 || nSession >= (int)m_bookList.size())
		{
			//	选中项的内容不合法
			break;
		}
		
		const wedding_book & session = m_bookList[nSession];
		
		if (session.status != S2C::WBS_UNBOOKED)
		{
			//	已经预订
			break;
		}
		
		int curServerTime = GetGame()->GetServerGMTTime();
		if (curServerTime >= session.start_time)
		{
			//	已经过了预约时间
			break;
		}

		if (GetHostPlayer()->GetPack()->GetItemTotalNum(GENERAL_BOOKCARD_ID) <= 0)
		{
			//	没有通用排期凭证
			break;
		}
		
		if (session.special)
		{
			//	是特殊日期
			int year(0), month(0), day(0);
			GetServerLocalTime(session.start_time, year, month, day);
			if (!HaveSpecialDayItem(year, month, day))
			{
				//	没有特殊物品
				break;
			}
		}
		bCan = true;
		break;
	}

	return bCan;
}

bool CDlgWedding::CanCancelBookSession(int nSession)
{
	//	判断指定场次是否可预定
	//
	bool bCan(false);

	while (true)
	{		
		if (nSession < 0 || nSession >= (int)m_bookList.size())
		{
			//	选中项的内容不合法
			break;
		}
		
		const wedding_book & session = m_bookList[nSession];
		
		if (session.status != S2C::WBS_BOOKED)
		{
			//	不是预订状态
			break;
		}

		int id = GetHostPlayer()->GetCharacterID();
		if (id != session.groom && id != session.bride)
		{
			//	不是新人之一
			break;
		}

		if (session.special)
		{
			if (!HaveInviteCard(session))
			{
				//	没有场次预约凭证
				break;
			}
		}
		bCan = true;
		break;
	}

	return bCan;
}

void CDlgWedding::GetCurServerTime(int &year, int &month, int &day, char &hour, char &minute, char &second)
{
	//	获取服务器当前时间（客户端估计得出）
	tm t1 = GetGame()->GetServerLocalTime();
	GetYearMonthDay(t1, year, month, day);
	hour = t1.tm_hour;
	minute = t1.tm_min;
	second = t1.tm_sec;
}

tm   CDlgWedding::GetServerLocalTime(int t)
{
	//	将服务器传来的秒数，转化为服务器本地的时间（用于显示或判断）
	//
	int timeBias = GetGame()->GetTimeZoneBias() * 60;
	int serverLocalTime = t - timeBias;
	return *gmtime((time_t*)&serverLocalTime);
}

void CDlgWedding::GetServerLocalTime(int t, int &year, int &month, int &day)
{
	tm t1 = GetServerLocalTime(t);
	GetYearMonthDay(t1, year, month, day);
}

void CDlgWedding::GetYearMonthDay(const tm &t1, int &year, int &month, int &day)
{
	year = 1900 + t1.tm_year;
	month = 1 + t1.tm_mon;
	day = t1.tm_mday;
}

void CDlgWedding::RebuildSessionList()
{
	//	根据当前设置的日期、场景，重新 build 场次列表
	m_pTxt_List1->ResetContent();

	if (m_selMonth > 0 && m_selScene >= 0)
	{
		//	当前选中的时间、场景均已初始化

		int idHost = GetHostPlayer()->GetCharacterID();
		ACString strQuery = GetStringFromTable(8754);	//	玩家名称查询中
		ACString strNone = GetStringFromTable(8756);	//	未预订时无玩家名
		ACString strStatus[5] = {						//	预订状态
			GetStringFromTable(8757),
			GetStringFromTable(8758),
			GetStringFromTable(8759),
			GetStringFromTable(8760),
			GetStringFromTable(8761)};
		ACString strLineFormat = _AL("%s-%s\t%s\t%s");	//	预订列表的格式：时间 预订状态 预订人
		ACString strLineText, strPlayerName;
		ACString strStartTime, strEndTime;
		abase::vector<int> UnknownIDs;
		for (size_t i = 0; i < m_bookList.size(); ++ i)
		{
			const wedding_book &cur = m_bookList[i];

			//	判断场景
			if (cur.scene != m_selScene)
			{
				continue;
			}

			//	判断日期
			int year(0), month(0), day(0);
			GetServerLocalTime(cur.start_time, year, month, day);
			if (year != m_selYear ||
				month != m_selMonth ||
				day != m_selDay)
			{
				continue;
			}

			//	增加一行
			strStartTime = GetSessionTimeString(cur.start_time);
			strEndTime = GetSessionTimeString(cur.end_time);
			strLineText.Format(strLineFormat, strStartTime, strEndTime,
				(cur.status>=0&&cur.status<sizeof(strStatus)/sizeof(strStatus[0]))?strStatus[cur.status]:strNone,
				(cur.groom>0 && cur.bride>0)?strQuery:strNone);
			m_pTxt_List1->AddString(strLineText);
			UpdateSessionListHint(m_pTxt_List1->GetCount()-1);

			//	高亮显示自己预订的场次
			if (cur.groom == idHost || cur.bride == idHost)
				m_pTxt_List1->SetItemTextColor(m_pTxt_List1->GetCount()-1, A3DCOLORRGB(255, 0, 0), 2);

			//	存储对应列表的索引以备后续查询
			m_pTxt_List1->SetItemData(m_pTxt_List1->GetCount()-1, i, 0);

			//	通过 ID 查询预订人名称
			if (cur.groom > 0)
			{
				strPlayerName = GetGameRun()->GetPlayerName(cur.groom, false);
				if (strPlayerName.IsEmpty())
					UnknownIDs.push_back(cur.groom);
			}
			if (cur.bride > 0)
			{
				strPlayerName = GetGameRun()->GetPlayerName(cur.bride, false);
				if (strPlayerName.IsEmpty())
					UnknownIDs.push_back(cur.bride);
			}

			//	预订人名称的更新，留个记号在 Tick 里检查
			if (cur.groom > 0 && cur.bride > 0)
				m_pTxt_List1->SetItemDataPtr(m_pTxt_List1->GetCount()-1, (void *)1, 0);

			//	存储数据在第2列用于排序
			m_pTxt_List1->SetItemData(m_pTxt_List1->GetCount()-1, cur.start_time, 1);
		}

		//	查询玩家名称
		if (!UnknownIDs.empty())
			GetGameSession()->CacheGetPlayerBriefInfo(UnknownIDs.size(), &UnknownIDs[0], 2);

		//	按时间排序（根据第2列存的数据）
		if (m_pTxt_List1->GetCount() > 0)
			m_pTxt_List1->SortItemsByData(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 1);
	}
}

void CDlgWedding::UpdateSessionListHint(int nItem)
{
	//	根据列表内容，自动生成场次列表的悬浮提示
	//
	int nCount = m_pTxt_List1->GetCount();
	if (nItem >= 0 && nItem < nCount)
	{
		ACString strLineText = m_pTxt_List1->GetText(nItem);
		CSplit s(strLineText);
		CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
		ACString strHint;
		for (size_t i = 0; i < vec.size(); ++ i)
		{
			if (i>0)
				strHint += _AL("\r");
			strHint += vec[i];
		}
		m_pTxt_List1->SetItemHint(nItem, strHint);
	}
}

void CDlgWedding::UpdatePlayerNames()
{
	//	根据玩家的名称，更新预订列表
	//
	int nSessionSize = (int)m_bookList.size();
	ACString strLineText, strName1, strName2;
	ACString strFormat = GetStringFromTable(8752);
	CSPLIT_STRING_VECTOR vec;
	for (int i(0); i < m_pTxt_List1->GetCount(); ++ i)
	{
		if (m_pTxt_List1->GetItemDataPtr(i, 0) == NULL)
			continue;

		//	指针未置0，表明预订人名称尚且未更新

		int nSession = m_pTxt_List1->GetItemData(i, 0);
		if (nSession < 0 || nSession >= nSessionSize)
		{
			//	出现一致性错误，不再查询
			m_pTxt_List1->SetItemDataPtr(i, NULL, 0);
			continue;
		}

		const wedding_book &cur = m_bookList[nSession];

		//	查询新郎名称
		strName1 = GetGameRun()->GetPlayerName(cur.groom, false);
		if (strName1.IsEmpty())
			continue;

		//	查询新娘名称
		strName2 = GetGameRun()->GetPlayerName(cur.bride, false);
		if (strName2.IsEmpty())
			continue;

		//	名称都查到，更新显示
		strLineText = m_pTxt_List1->GetText(i);
		CSplit s(strLineText);
		vec = s.Split(_AL("\t"));
		strLineText.Format(strFormat,
			vec[0],		//	起止时间
			vec[1],		//	预订状态
			strName1, strName2);	//	预订人（新查询到的）

		m_pTxt_List1->SetText(i, strLineText);
		UpdateSessionListHint(i);

		//	更新标识，不需要再查了
		m_pTxt_List1->SetItemDataPtr(i, NULL, 0);
	}
}

ACString CDlgWedding::GetSessionTimeString(int t)
{
	//	获取场次时间显示格式（开始时间、结束时间）
	//	参数为服务器传来的时间
	//

	tm t1 = GetServerLocalTime(t);
	
	ACString strText;
	strText.Format(GetStringFromTable(8753),
		t1.tm_hour,
		t1.tm_min);

	return strText;
}

void CDlgWedding::OnWeddingBookSuccess(void *pData)
{
	if (pData)
	{
		S2C::cmd_wedding_book_success *pWeddingBookSuccess = (S2C::cmd_wedding_book_success *)pData;
		
		//	弹出提示框
		int idMsg = pWeddingBookSuccess->type == 1 ? 8755 : 8770;
		GetGameUIMan()->MessageBox("", GetStringFromTable(idMsg), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		
		//	更新列表
		GetWeddingBookListFromServer();
	}
}

void CDlgWedding::GetWeddingBookListFromServer()
{
	//	从服务器获取当前的预订列表
	//	需在 NPC 服务开启时进行
	//
	GetGameSession()->c2s_CmdNPCSevGetContent(GP_NPCSEV_WEDDING_BOOK);
}

void CDlgWedding::Reset()
{
	//	重置所有内容，以便一切重头再来
	//

	//	当前时间在 OnTick 中更新
	m_curYear = m_curMonth = m_curDay = 0;
	m_pTxt_Title->SetText(_AL(""));

	//	更新列表为空列表
	OnWeddingBookList(NULL);
}

void CDlgWedding::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Special", only one available drag-drop target
	this->SetItem(iSrc);
}