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
			pLabel->SetFontAndColor(_AL("����ϸ��һ����"), 12, 0xFFFFFFFF, 0, 1, true);
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

	// �����鳡���ؼ�
	PAUIOBJECT pRdo(NULL), pTxt(NULL);
	int k(0);
	while (true)
	{
		strName.Format("Rdo_%02d", ++k);
		pRdo = GetDlgItem(strName);
		if (!pRdo)
		{
			// û�и���Ŀؼ���
			break;
		}

		strName.Format("Txt_%02d", k);
		pTxt = GetDlgItem(strName);
		if (pWeddingConfig && k<=nMaxScene &&
			pWeddingConfig->wedding_scene[k-1].name[0] &&
			pTxt != NULL)
		{
			// ���ı���Чʱ����ʾ����
			pTxt->SetText(pWeddingConfig->wedding_scene[k-1].name);
		}
		else
		{
			// ���ı���Чʱ����������
			pRdo->Show(false);
			if (pTxt)
				pTxt->Show(false);
		}
	}

	return true;
}

void CDlgWedding::OnShowDialog()
{
	//	���ã��Ա����¿�ʼ
	Reset();

	//	�ӷ�������ѯԤ���б�
	GetWeddingBookListFromServer();
}

void CDlgWedding::OnTick()
{
	// ��ȡϵͳ��ǰʱ�䲢��ʾ
	char hour, minute, second;
	GetCurServerTime(m_curYear, m_curMonth, m_curDay, hour, minute, second);

	ACString strText;
	strText.Format(GetStringFromTable(8010), m_curYear, m_curMonth, m_curDay, hour, minute);
	m_pTxt_Title->SetText(strText);

	//	�����б��е����������ʾ
	UpdatePlayerNames();
}

void CDlgWedding::OnMonthSelectChange()
{
	// ���µ�ǰ�·ݵ���ʾ
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

	// ����������ʾ
	UpdateCalendar();

	// ����ѡ�������µĵ�һ����Ч���ڣ�����û����Ч���ڣ�
	m_selDay = GetFirstSelectDay();
	OnDaySelectChange();
}

void CDlgWedding::UpdateCalendar()
{
	// ���µ�ǰѡ���·ݵ�������ʾ

	// ���㵱ǰ�·ݵ�����������ͼƬ���е���ֹ��Χ����Χ�ɷ���ʼ����
	int start(-1), end(-1);
	if (m_selMonth != 0)
	{
		// ��ǰѡ���·��Ѿ���ʼ��

		// start 0 ��Ӧ Sunday
		start = CECTime::GetWeek(m_selYear, m_selMonth, 1);
		start = (start + 1)%7;

		int count = CECTime::GetMonthDayCount(m_selYear, m_selMonth);
		end = start+count;
	}

	// ����������ʾ����Χδ��ʼ��ʱ����ʾ�գ�
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
		m_pSprite_Icon1 = LoadSprite("window\\��������a.dds");
		m_pSprite_Icon2 = LoadSprite("window\\��������b.dds");
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
	//	������ʾ��ѡ�����ӡ���Ԥ��������
	//
	for (int row = 0; row < 6; ++ row)
	{
		for (int col = 0; col < 7; ++ col)
		{
			PAUIIMAGEPICTURE p = m_pImg_Week[row][col];
			int nDay = p->GetData();
			if(nDay == m_selDay)
			{
				//	��ѡ�����ڣ���ɫ��ʾ��
				p->SetColor(A3DCOLORRGB(255, 0, 0));
			}
			else
			{
				//	��������
				p->SetColor(A3DCOLORRGB(255, 255, 255));
			}
		}
	}
}

void CDlgWedding::OnDaySelectChange()
{
	//	���±�ѡ��ʱͼ��״̬
	UpdateCalendarColor();

	//	ѡȡ��ǰ��һ������
	m_selScene = GetFirstScene();
	OnWeddingSceneSelectChange();
}

void CDlgWedding::OnWeddingSceneSelectChange()
{
	//	���³���ѡ�������ʾ
	CheckRadioButton(0, m_selScene>=0 ? m_selScene : -1);

	//	��ճ����б�
	m_pTxt_List1->ResetContent();

	//	��ѯ�б�
	RebuildSessionList();

	// ѡȡ��һ��ΪĬ����
	if (m_pTxt_List1->GetCount() > 0)
		m_pTxt_List1->SetCurSel(0);
	OnWeddingSessionSelectChange();
}

void CDlgWedding::OnWeddingSessionSelectChange()
{
	//	���°�ť����
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
			//	������Ʒ�Ĵ���
			return;
		}

		//	�����Ĵ���
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
	//	���������Ʒ
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
	//	�л�����
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

	//	����Ԥ��Э��
	int nSession = GetSelectedSession();
	const wedding_book &temp = m_bookList[nSession];
	GetGameSession()->c2s_CmdNPCSevWeddingBook(1, temp.start_time, temp.end_time, temp.scene, m_iSlot);

	//	���ԤԼ��Ʒ
	SetItem(-1);
}

void CDlgWedding::OnCommand_CancelBook(const char *szCommand)
{
	if (!CanCancelBook())
		return;

	//	����ȷ�϶Ի���	
	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_CancelBook", GetStringFromTable(8775),
		MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
}

void CDlgWedding::CancelBook(bool bConfirm)
{
	if (bConfirm)
	{
		//	ȷ��ȡ��
		
		if (!CanCancelBook())
			return;
		
		//	����ȡ��Ԥ��Э��
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
	//	���յ�ǰʱ�䣬���㵱ǰѡ���·����һ����ѡ�е�����
	//
	int nFirst(-1);

	if (m_curMonth > 0 && m_selMonth > 0)
	{
		int count = CECTime::GetMonthDayCount(m_selYear, m_selMonth);

		if (m_selYear == m_curYear && m_selMonth == m_curMonth)
		{
			//	��ǰ�·ݣ��ӵ�ǰ�տ�ʼ��ѯ
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
				//	û�ҵ������³���ʼ��
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
			//	�����·ݣ�Ӧ���Ǻ���·ݣ������³���ʼ��ѯ
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
	//	�������ñ����˳�򼰿ؼ���������ҵ�һ������

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
			// û�и���Ŀؼ���
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
	//	���յ�ǰ���ڣ��ж��ܷ���ʾ��������
	//	ֻҪ��ǰ����ĳ�������ʾ������Ҫ��ʾ��������
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
	//	�ο���ǰ���ڣ��Ƿ����ʾ����

	bool bShow(false);

	while (true)
	{
		//	��鴫�������Ƿ���ȷ
		if (year <= 0 || month <= 0 || month >= 13 || day <= 0)
			break;		
		int count = CECTime::GetMonthDayCount(year, month);
		if (day > count)
			break;

		//	��鵱ǰԤ���б�
		//
		int temp_year(0), temp_month(0), temp_day(0);
		for (size_t i(0); i < m_bookList.size(); ++ i)
		{
			const wedding_book & cur = m_bookList[i];
			GetServerLocalTime(cur.start_time, temp_year, temp_month, temp_day);
			if (temp_year == year && temp_month == month)
			{
				//	�������������·���ĳ�캬Ԥ���б�����տ���ʾ
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
	//	�жϵ�ǰ�����Ƿ��п���ʾ��Ԥ���б�
	//
	
	bool bHave(false);

	while (true)
	{		
		//	��鴫�������Ƿ���ȷ
		if (year <= 0 || month <= 0 || month >= 13 || day <= 0)
			break;		
		int count = CECTime::GetMonthDayCount(year, month);
		if (day > count)
			break;

		//	��ѯԤ���б�
		int temp_year(0), temp_month(0), temp_day(0);
		for (size_t i(0); i < m_bookList.size(); ++ i)
		{
			const wedding_book & cur = m_bookList[i];
			GetServerLocalTime(cur.start_time, temp_year, temp_month, temp_day);
			if (temp_year == year && temp_month == month && temp_day == day)
			{
				//	���������������Ԥ���б�
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
	//	�жϸ��������Ƿ�����������
	bool bSpecial(false);

	while (true)
	{
		//	��ȡ������ñ�������������
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
				//	��Ч��
				continue;
			}
			if (year != pWeddingConfig->reserved_day[i].year ||
				month != pWeddingConfig->reserved_day[i].month ||
				day != pWeddingConfig->reserved_day[i].day)
			{
				//	�Ǵ���������
				continue;
			}

			//	�ҵ�һ��Ӧ����������
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
	//	���Ұ������Ƿ�����Ӧ���רΪ�������У�
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
	//	���ҷ���Ҫ��Ķ�������е���һ��
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
			//	����2����ӣ����߲��Ƕӳ�
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
			//	��ӵĶ����Ա𲻷�
			break;
		}

		pPlayerRet = pOtherPlayer;
		break;
	}

	return pPlayerRet;
}

bool CDlgWedding::CheckTeamCondition(bool bBook)
{
	//	��� Ԥ��/ȡ��Ԥ�� ���������
	//
	return GetTeamOtherPlayer() != NULL;
}

bool CDlgWedding::CanBook()
{
	//	�ܷ�Ԥ����ǰѡ�е����ڡ�����������
	//
	bool bCan(false);

	while (true)
	{
		if (!CheckTeamCondition(true))
		{
			//	Ԥ���������������
			break;
		}

		if (m_selMonth <= 0)
		{
			//	��δ��ʼ��
			break;
		}

		if (m_selScene < 0)
		{
			//	����δѡ��
			break;
		}

		int nSession = GetSelectedSession();
		if (nSession < 0)
		{
			//	����δѡ��
			break;
		}

		if (!CanBookSession(nSession))
		{
			//	������Ԥ��
			break;
		}

		bCan = true;
		break;
	}
	return bCan;
}

bool CDlgWedding::CanCancelBook()
{
	//	�ܷ�Ԥ����ǰѡ�е����ڡ�����������
	//
	bool bCan(false);

	while (true)
	{
		if (!CheckTeamCondition(false))
		{
			//	ȡ��Ԥ���������������
			break;
		}

		if (m_selMonth <= 0)
		{
			//	��δ��ʼ��
			break;
		}

		if (m_selScene < 0)
		{
			//	����δѡ��
			break;
		}

		int nSession = GetSelectedSession();
		if (nSession < 0)
		{
			//	����δѡ��
			break;
		}

		if (!CanCancelBookSession(nSession))
		{
			//	������ȡ��Ԥ��
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
		
		//	�����Ϸ�
		CECIvtrItem *pItemOld = NULL;
		if (IsImageItemValid(m_iSlot))
			pItemOld = pPack->GetItem(m_iSlot);

		//	�ж��ظ�����
		if (pItemOld != NULL &&
			pItem == pItemOld)
		{
			//	���ڷǿ�ʱִ��
			break;
		}

		bItemChanged = true;
		
		//	���ͼ��
		m_pImg_Special->SetCover(NULL, -1);
		
		if (pItemOld)
			pItemOld->Freeze(false);
		
		//	��ʾ�½��
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
		//	������Ʒ�����ı䣬Ӱ��ԤԼ��ȡ��ԤԼ����
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
	//	��������б�
	m_bookList.clear();

	//	�������б�
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

	//	֪ͨ����
	OnBookListChange();
}

void CDlgWedding::OnBookListChange()
{
	//	ԤԼ�б��Ѿ������仯
	//

	//	����������ʾ���·�

	if (m_selMonth > 0 && CanShowMonth(m_selYear, m_selMonth))
	{
		//	�ϴ��б��Ӧ���·ݻ��ܹ���ʾ���򲻸ı��·�

		//	�����·ݷ�ҳ����Ϊ�б�仯��		
		m_pBtn_PrevMonth->Enable(CanGoPrevMonth());
		m_pBtn_NextMonth->Enable(CanGoNextMonth());

		//	���������������е�ͼ����ܷ����仯��
		UpdateCalendar();

		if (CanShowDay(m_selYear, m_selMonth, m_selDay))
		{
			//	�ϴ���ʾ�����ӻ��ܹ���ʾ���򲻸ı�����

			//	��������ѡ����ʾ����Ϊ�������£�
			UpdateCalendarColor();

			//	���½����б���Ϊ�б�仯��
			RebuildSessionList();
			
			// ѡȡ��һ��ΪĬ����
			if (m_pTxt_List1->GetCount() > 0)
				m_pTxt_List1->SetCurSel(0);
			OnWeddingSessionSelectChange();

			return;
		}

		//	������һ������ʾ����
		m_selDay = GetFirstSelectDay();
		OnDaySelectChange();
		return;
	}

	//	�ӵ�ǰ���³��������ҵ�һ��������ʾ���·�
	m_selYear = m_selMonth = m_selDay = 0;
	if (CanShowMonth(m_curYear, m_curMonth))
	{
		//	��ǰ�·�����ʾ
		m_selYear = m_curYear;
		m_selMonth = m_curMonth;
	}
	else
	{
		//	���б��в��ҵ�һ������
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
	//	���ҵ�ǰѡ���������Ԥ��ʱ����
	int nSession(-1);

	while (true)
	{
		int nSel = m_pTxt_List1->GetCurSel();
		int nCount = m_pTxt_List1->GetCount();
		if (nSel < 0 || nSel >= nCount)
		{
			//	û��ѡ�г���
			break;
		}

		int candidate = m_pTxt_List1->GetItemData(nSel);
		if (candidate < 0 || candidate >= (int)m_bookList.size())
		{
			//	ѡ��������ݲ��Ϸ�
			break;
		}

		nSession = candidate;
		break;
	}

	return nSession;
}

bool CDlgWedding::CanBookSession(int nSession)
{
	//	�ж�ָ�������Ƿ��Ԥ��
	//
	bool bCan(false);

	while (true)
	{		
		if (nSession < 0 || nSession >= (int)m_bookList.size())
		{
			//	ѡ��������ݲ��Ϸ�
			break;
		}
		
		const wedding_book & session = m_bookList[nSession];
		
		if (session.status != S2C::WBS_UNBOOKED)
		{
			//	�Ѿ�Ԥ��
			break;
		}
		
		int curServerTime = GetGame()->GetServerGMTTime();
		if (curServerTime >= session.start_time)
		{
			//	�Ѿ�����ԤԼʱ��
			break;
		}

		if (GetHostPlayer()->GetPack()->GetItemTotalNum(GENERAL_BOOKCARD_ID) <= 0)
		{
			//	û��ͨ������ƾ֤
			break;
		}
		
		if (session.special)
		{
			//	����������
			int year(0), month(0), day(0);
			GetServerLocalTime(session.start_time, year, month, day);
			if (!HaveSpecialDayItem(year, month, day))
			{
				//	û��������Ʒ
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
	//	�ж�ָ�������Ƿ��Ԥ��
	//
	bool bCan(false);

	while (true)
	{		
		if (nSession < 0 || nSession >= (int)m_bookList.size())
		{
			//	ѡ��������ݲ��Ϸ�
			break;
		}
		
		const wedding_book & session = m_bookList[nSession];
		
		if (session.status != S2C::WBS_BOOKED)
		{
			//	����Ԥ��״̬
			break;
		}

		int id = GetHostPlayer()->GetCharacterID();
		if (id != session.groom && id != session.bride)
		{
			//	��������֮һ
			break;
		}

		if (session.special)
		{
			if (!HaveInviteCard(session))
			{
				//	û�г���ԤԼƾ֤
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
	//	��ȡ��������ǰʱ�䣨�ͻ��˹��Ƶó���
	tm t1 = GetGame()->GetServerLocalTime();
	GetYearMonthDay(t1, year, month, day);
	hour = t1.tm_hour;
	minute = t1.tm_min;
	second = t1.tm_sec;
}

tm   CDlgWedding::GetServerLocalTime(int t)
{
	//	��������������������ת��Ϊ���������ص�ʱ�䣨������ʾ���жϣ�
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
	//	���ݵ�ǰ���õ����ڡ����������� build �����б�
	m_pTxt_List1->ResetContent();

	if (m_selMonth > 0 && m_selScene >= 0)
	{
		//	��ǰѡ�е�ʱ�䡢�������ѳ�ʼ��

		int idHost = GetHostPlayer()->GetCharacterID();
		ACString strQuery = GetStringFromTable(8754);	//	������Ʋ�ѯ��
		ACString strNone = GetStringFromTable(8756);	//	δԤ��ʱ�������
		ACString strStatus[5] = {						//	Ԥ��״̬
			GetStringFromTable(8757),
			GetStringFromTable(8758),
			GetStringFromTable(8759),
			GetStringFromTable(8760),
			GetStringFromTable(8761)};
		ACString strLineFormat = _AL("%s-%s\t%s\t%s");	//	Ԥ���б�ĸ�ʽ��ʱ�� Ԥ��״̬ Ԥ����
		ACString strLineText, strPlayerName;
		ACString strStartTime, strEndTime;
		abase::vector<int> UnknownIDs;
		for (size_t i = 0; i < m_bookList.size(); ++ i)
		{
			const wedding_book &cur = m_bookList[i];

			//	�жϳ���
			if (cur.scene != m_selScene)
			{
				continue;
			}

			//	�ж�����
			int year(0), month(0), day(0);
			GetServerLocalTime(cur.start_time, year, month, day);
			if (year != m_selYear ||
				month != m_selMonth ||
				day != m_selDay)
			{
				continue;
			}

			//	����һ��
			strStartTime = GetSessionTimeString(cur.start_time);
			strEndTime = GetSessionTimeString(cur.end_time);
			strLineText.Format(strLineFormat, strStartTime, strEndTime,
				(cur.status>=0&&cur.status<sizeof(strStatus)/sizeof(strStatus[0]))?strStatus[cur.status]:strNone,
				(cur.groom>0 && cur.bride>0)?strQuery:strNone);
			m_pTxt_List1->AddString(strLineText);
			UpdateSessionListHint(m_pTxt_List1->GetCount()-1);

			//	������ʾ�Լ�Ԥ���ĳ���
			if (cur.groom == idHost || cur.bride == idHost)
				m_pTxt_List1->SetItemTextColor(m_pTxt_List1->GetCount()-1, A3DCOLORRGB(255, 0, 0), 2);

			//	�洢��Ӧ�б�������Ա�������ѯ
			m_pTxt_List1->SetItemData(m_pTxt_List1->GetCount()-1, i, 0);

			//	ͨ�� ID ��ѯԤ��������
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

			//	Ԥ�������Ƶĸ��£������Ǻ��� Tick ����
			if (cur.groom > 0 && cur.bride > 0)
				m_pTxt_List1->SetItemDataPtr(m_pTxt_List1->GetCount()-1, (void *)1, 0);

			//	�洢�����ڵ�2����������
			m_pTxt_List1->SetItemData(m_pTxt_List1->GetCount()-1, cur.start_time, 1);
		}

		//	��ѯ�������
		if (!UnknownIDs.empty())
			GetGameSession()->CacheGetPlayerBriefInfo(UnknownIDs.size(), &UnknownIDs[0], 2);

		//	��ʱ�����򣨸��ݵ�2�д�����ݣ�
		if (m_pTxt_List1->GetCount() > 0)
			m_pTxt_List1->SortItemsByData(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 1);
	}
}

void CDlgWedding::UpdateSessionListHint(int nItem)
{
	//	�����б����ݣ��Զ����ɳ����б��������ʾ
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
	//	������ҵ����ƣ�����Ԥ���б�
	//
	int nSessionSize = (int)m_bookList.size();
	ACString strLineText, strName1, strName2;
	ACString strFormat = GetStringFromTable(8752);
	CSPLIT_STRING_VECTOR vec;
	for (int i(0); i < m_pTxt_List1->GetCount(); ++ i)
	{
		if (m_pTxt_List1->GetItemDataPtr(i, 0) == NULL)
			continue;

		//	ָ��δ��0������Ԥ������������δ����

		int nSession = m_pTxt_List1->GetItemData(i, 0);
		if (nSession < 0 || nSession >= nSessionSize)
		{
			//	����һ���Դ��󣬲��ٲ�ѯ
			m_pTxt_List1->SetItemDataPtr(i, NULL, 0);
			continue;
		}

		const wedding_book &cur = m_bookList[nSession];

		//	��ѯ��������
		strName1 = GetGameRun()->GetPlayerName(cur.groom, false);
		if (strName1.IsEmpty())
			continue;

		//	��ѯ��������
		strName2 = GetGameRun()->GetPlayerName(cur.bride, false);
		if (strName2.IsEmpty())
			continue;

		//	���ƶ��鵽��������ʾ
		strLineText = m_pTxt_List1->GetText(i);
		CSplit s(strLineText);
		vec = s.Split(_AL("\t"));
		strLineText.Format(strFormat,
			vec[0],		//	��ֹʱ��
			vec[1],		//	Ԥ��״̬
			strName1, strName2);	//	Ԥ���ˣ��²�ѯ���ģ�

		m_pTxt_List1->SetText(i, strLineText);
		UpdateSessionListHint(i);

		//	���±�ʶ������Ҫ�ٲ���
		m_pTxt_List1->SetItemDataPtr(i, NULL, 0);
	}
}

ACString CDlgWedding::GetSessionTimeString(int t)
{
	//	��ȡ����ʱ����ʾ��ʽ����ʼʱ�䡢����ʱ�䣩
	//	����Ϊ������������ʱ��
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
		
		//	������ʾ��
		int idMsg = pWeddingBookSuccess->type == 1 ? 8755 : 8770;
		GetGameUIMan()->MessageBox("", GetStringFromTable(idMsg), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		
		//	�����б�
		GetWeddingBookListFromServer();
	}
}

void CDlgWedding::GetWeddingBookListFromServer()
{
	//	�ӷ�������ȡ��ǰ��Ԥ���б�
	//	���� NPC ������ʱ����
	//
	GetGameSession()->c2s_CmdNPCSevGetContent(GP_NPCSEV_WEDDING_BOOK);
}

void CDlgWedding::Reset()
{
	//	�����������ݣ��Ա�һ����ͷ����
	//

	//	��ǰʱ���� OnTick �и���
	m_curYear = m_curMonth = m_curDay = 0;
	m_pTxt_Title->SetText(_AL(""));

	//	�����б�Ϊ���б�
	OnWeddingBookList(NULL);
}

void CDlgWedding::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Special", only one available drag-drop target
	this->SetItem(iSrc);
}