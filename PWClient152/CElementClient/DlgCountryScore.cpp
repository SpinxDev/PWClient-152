#include "DlgCountryScore.h"
#include "EC_HostPlayer.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_DomainCountry.h"
#include "EC_CountryConfig.h"
#include "DlgTask.h"

static const char* COUNTRY_IMG_PATH[DOMAIN2_INFO::COUNTRY_COUNT] = {"number\\青帝.tga", "number\\赤帝.tga", "number\\白帝.tga", "number\\黑帝.tga"};

AUI_BEGIN_COMMAND_MAP(CDlgCountryScore, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgCountryScore::CDlgCountryScore()
: m_pImgCountryDefense(NULL)
, m_pImgCountryInvader(NULL)
{
	::ZeroMemory(m_pDigit, sizeof(m_pDigit));
	::ZeroMemory(m_pPeopleCount, sizeof(m_pPeopleCount));
	::ZeroMemory(m_pSpriteCountry,sizeof(m_pSpriteCountry));
}

CDlgCountryScore::~CDlgCountryScore()
{
	int i;
	for (i = 0; i < DOMAIN2_INFO::COUNTRY_COUNT; ++i)
		A3DRELEASE(m_pSpriteCountry[i]);
}

void CDlgCountryScore::OnCommandCancel(const char * szCommand)
{
	Show(false);
}

bool CDlgCountryScore::OnInitDialog()
{
	DDX_Control("Img_Point1", m_pDigit[0][0]);
	DDX_Control("Img_Point2", m_pDigit[0][1]);
	DDX_Control("Img_Point3", m_pDigit[1][0]);
	DDX_Control("Img_Point4", m_pDigit[1][1]);
	DDX_Control("Img_People1", m_pPeopleCount[0][0]);
	DDX_Control("Img_People2", m_pPeopleCount[0][1]);
	DDX_Control("Img_People3", m_pPeopleCount[1][0]);
	DDX_Control("Img_People4", m_pPeopleCount[1][1]);
	DDX_Control("Img_Icon1", m_pImgCountryDefense);
	DDX_Control("Img_Icon2", m_pImgCountryInvader);
	int i, j;
	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			m_pDigit[i][j]->GetImage()->SetLinearFilter(true);
			m_pPeopleCount[i][j]->GetImage()->SetLinearFilter(true);
		}
	}

	A2DSprite* p2DSprite;
	for (i = 0; i < DOMAIN2_INFO::COUNTRY_COUNT; ++i)
	{
		p2DSprite = new A2DSprite;
		if (p2DSprite->Init(g_pGame->GetA3DEngine()->GetA3DDevice(), COUNTRY_IMG_PATH[i], 0))
		{
			p2DSprite->SetLinearFilter(true);		
		}
		else
		{
			delete p2DSprite;
			p2DSprite = NULL;
		}
		m_pSpriteCountry[i] = p2DSprite;
	}
	SetCanEscClose(false);
	return true;
}

void CDlgCountryScore::UpdateScore()
{
	if (!IsShow())
	{
		AlignTo(GetGameUIMan()->GetDialog("Win_Map"), 
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT, 
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM,
			-10, 0);
		
		Show(true);
		UpdateCountryImg();
	}

	if(GetHostPlayer()->IsInCountryWar())
	{
		const CECHostPlayer::BATTLEINFO &binfo = GetHostPlayer()->GetBattleInfo();
		int iDefenceScore = min(99, binfo.iScore_D);
		int iInvaderScore = min(99, binfo.iScore_I);
		m_pDigit[0][0]->FixFrame(iDefenceScore / 10);
		m_pDigit[0][1]->FixFrame(iDefenceScore % 10);
		m_pDigit[1][0]->FixFrame(iInvaderScore / 10);
		m_pDigit[1][1]->FixFrame(iInvaderScore % 10);
		m_pPeopleCount[0][0]->FixFrame(binfo.iDefenderCount / 10);
		m_pPeopleCount[0][1]->FixFrame(binfo.iDefenderCount % 10);
		m_pPeopleCount[1][0]->FixFrame(binfo.iAttackerCount / 10);
		m_pPeopleCount[1][1]->FixFrame(binfo.iAttackerCount % 10);
	}
}
void CDlgCountryScore::UpdateCountryImg()
{
	const CECHostPlayer::BATTLEINFO &binfo = GetHostPlayer()->GetBattleInfo();

	int iDefence = binfo.iDefenceCountry;
	if (!CECCountryConfig::ValidateID(iDefence))
	{
		ASSERT(false);
		m_pImgCountryDefense->ClearCover();
	}
	else
	{
		m_pImgCountryDefense->SetCover(m_pSpriteCountry[iDefence - 1], 0);
		m_pImgCountryDefense->SetHint(*(CECCountryConfig::Instance().GetName(iDefence)));
	}

	int iInvader = binfo.iOffenseCountry;
	if (!CECCountryConfig::ValidateID(iInvader))
	{
		ASSERT(false);
		m_pImgCountryInvader->ClearCover();
	}
	else
	{
		m_pImgCountryInvader->SetCover(m_pSpriteCountry[iInvader - 1], 0);
		m_pImgCountryInvader->SetHint(*(CECCountryConfig::Instance().GetName(iInvader)));
	}
}

void CDlgCountryScore::OnShowDialog()
{
	CDlgTask* pTask = GetGameUIMan()->m_pDlgTask;
	if (pTask->IsShowTrace())
	{
		pTask->OnCommand_showtrace(NULL);
	}
}

void CDlgCountryScore::OnCountryEnterWar()
{
	if (!GetHostPlayer()->IsInCountryWar())
	{
		//	已离开战场，隐藏比分界面
		OnCommandCancel("IDCANCEL");
		return;
	}
}