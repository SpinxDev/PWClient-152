// Filename	: DlgAutoHelp.cpp 
// Creator	: WYD


#include "DlgAutoHelp.h"

#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_Configs.h"
#include "EC_UIConfigs.h"
#include <A2DSprite.h>

#define new A_DEBUG_NEW

static const char* GirlImageNormal = "2013\\小美眨眼1.tga";
static const char* GirlImagePushed = "2013\\小美眨眼2.tga";

CImgAnimation::CImgAnimation(A3DDevice* device):m_pA3DDevice(device),m_tick(0)
{
	Reset();
	m_bRandPlay = true;
	m_bLoop = true;
	m_fPlayRate = 1.0f;
	m_pImg = NULL;
	m_bStart = false;
}
CImgAnimation::~CImgAnimation()
{
	ReleaseFrame();
}
void CImgAnimation::Start()
{
	Reset();
	m_bStart = true;
}

void CImgAnimation::Reset() 
{ 
	m_tick = 0; 
	m_curFrame = 0;
	m_RandTick=0;
	m_RandDelta=0; 
}

void CImgAnimation::ReleaseFrame()
{
	for (unsigned int i=0;i<m_frames.size();i++)
	{
		A3DRELEASE(m_frames[i].pSprite);
	}
	m_frames.clear();
}
void CImgAnimation::Tick(DWORD dt)
{
	int count = (int)m_frames.size();
 	if(!m_bStart ||count<1) return;
	
	m_RandTick++; // 间隔多少帧重播
	if(m_RandTick < m_RandDelta)				
		return;	
	
	m_tick++;	

	const keyframe& key = m_frames[m_curFrame];
	if (m_tick >= key.tick )
	{
		// play frame
		m_pImg->SetCover(key.pSprite,0);
		m_curFrame++;
	}
	if (m_curFrame>= count)
	{
		if(m_bLoop)
		{
			Reset();
			m_RandDelta = m_bRandPlay ? a_Random(10,512):120; // 生成重播的间隔	
			m_RandTick = 0;			
		}
		else
			Stop();
	}	
}
void CImgAnimation::AddFrame(const char* szPath, int tick)
{
	A2DSprite* sprite = new A2DSprite;
	if( !sprite ) 
	{
		AUI_ReportError(__LINE__, __FILE__);
		return;
	}
	
	bool bval = sprite->Init(m_pA3DDevice, szPath, 0);
	if( !bval ) 
	{
		delete sprite;
		AUI_ReportError(__LINE__, __FILE__);
		return;
	}
	sprite->SetLinearFilter(true);
	
	keyframe k;
	k.pSprite = sprite;
	k.tick = tick;
	m_frames.push_back(k);
}


void CImgAnimation::SetPlayRate(float r)
{
	m_fPlayRate = r;
	for (unsigned int i=0;i<m_frames.size();i++)
	{
		m_frames[i].tick = int(m_frames[i].tick/r);
	}
}

//////////////////////////////////////////////////////////////////////////
AUI_BEGIN_EVENT_MAP(CDlgAutoHelp, CDlgBase)
AUI_ON_EVENT(NULL,	WM_MOUSEMOVE,	OnEvent_mousemove)
AUI_ON_EVENT("*",	WM_MOUSEMOVE,	OnEvent_mousemove)
AUI_ON_EVENT("Img_Pop", WM_LBUTTONUP, OnActiveAutoHelp)
AUI_END_EVENT_MAP()

CDlgAutoHelp::CDlgAutoHelp()
{
	m_openTime = 0;
	m_closeTime = 0;
	m_pTxt_Msg = NULL;
	m_pImg_Talk = NULL;
	m_bAutoHelp = false;
	m_autoHelpCloseTime = 0;
	m_iMsgTime = 0;

	m_pImg_Girl = NULL;
	m_bAutoDisappear = true;
//	m_pAnim = NULL;
}

CDlgAutoHelp::~CDlgAutoHelp()
{
//	if (m_pAnim)
//	{
//		delete m_pAnim;
//	}
}
void CDlgAutoHelp::HideCloseButton()
{
	GetDlgItem("Btn_Close")->Show(false);
}
void CDlgAutoHelp::OnActiveAutoHelp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
//	if (!IsForbidAutoHelp())
	{
		SetAutoHelpState(true);
	}
}
bool CDlgAutoHelp::OnInitDialog()
{
	DDX_Control("Img_Pop",m_pImg_Girl);
	DDX_Control("Lbl_Tips",m_pTxt_Msg);
	DDX_Control("Edi_Bg", m_pImg_Talk);

	
/*
		m_pAnim = new CImgAnimation(g_pGame->GetA3DDevice());
		if (m_pAnim)
		{
			m_pAnim->AddFrame("2013\\微笑歌姬\\小美眨眼2.tga",0);
			m_pAnim->AddFrame("2013\\微笑歌姬\\小美眨眼1.tga",20);
			m_pAnim->SetTargetImgControl(m_pImg_Girl);
	
			m_pAnim->Start();
			m_pAnim->SetLoopFlag(true);
			m_pAnim->SetRandPlay(true);
		}*/
	

	return CDlgBase::OnInitDialog();
}
void CDlgAutoHelp::SetMsg(const ACString& str)
{
	m_iMsgTime = 0;
	m_pTxt_Msg->Show(true);
	m_pTxt_Msg->SetAlpha(255);
	m_pImg_Talk->Show(true);
	m_pImg_Talk->SetAlpha(255);
	m_pTxt_Msg->SetText(str);
	
//	SIZE sDlg = GetSize();
	/*
	SIZE sQQ = GetDlgItem("Img_Pop")->GetSize();
	SIZE sText = m_pTxt_Msg->GetSize();	
	POINT p = GetDlgItem("Img_Pop")->GetPos(true);
	POINT msgPos = m_pTxt_Msg->GetPos(true);

	m_pTxt_Msg->SetText(str);
	int hei = m_pTxt_Msg->GetRequiredHeight();
	hei += 3;
	m_pTxt_Msg->SetSize(sText.cx,hei);	
	m_pTxt_Msg->SetPos(msgPos.x,p.y- hei);
	
	hei += sQQ.cy;
	*/
//	SetSize(sDlg.cx,hei);	
}

void CDlgAutoHelp::OnShowDialog()
{
	if (!CECUIConfig::Instance().GetGameUI().bEnableAutoWiki || IsForbidAutoHelp())
	{
		Show(false);
		return;
	}
	m_closeTime = 0;
	m_openTime = 0;
	SetAutoHelpState(false);
	GetDlgItem("Btn_Close")->Show(false);

	SetMsg(GetGameUIMan()->GetStringFromTable(10753));

//	if(m_pAnim)
//		m_pAnim->Start();
}
void CDlgAutoHelp::OnHideDialog()
{
	SetAutoHelpState(false);
	m_openTime = 0;
	m_bAutoDisappear = false;
}
void CDlgAutoHelp::OnTick()
{
	// 战场里不显示
	if (GetHostPlayer()->GetBattleInfo().nType != CECHostPlayer::BT_NONE || IsForbidAutoHelp())
	{
		Show(false);
		return;
	}

	const CECUIConfig::GameUI& gameUI = CECUIConfig::Instance().GetGameUI();

	int delta = g_pGame->GetRealTickTime();
	m_closeTime +=	delta;
	m_autoHelpCloseTime += delta;
	m_iMsgTime += delta;

	if(m_autoHelpCloseTime>gameUI.nExitAutoExtractWikiStateTime) // 退出自动抓取
	{		
		SetAutoHelpState(false);
	}

	if (m_bAutoDisappear && m_closeTime>= gameUI.nCloseWikiPopDlgTime) // 5 min
	{
		Show(false);
		return;
	}
	if (m_iMsgTime >= gameUI.nCloseWikiMsgInfoTime)
	{
		int alpha = int(m_pTxt_Msg->GetAlpha() * 0.9);
		m_pTxt_Msg->SetAlpha(alpha);
		m_pImg_Talk->SetAlpha(alpha);
		if (m_pTxt_Msg->GetAlpha()<20) {
			m_pTxt_Msg->Show(false);
			m_pImg_Talk->Show(false);
		}	
	}

	if (GetHostPlayer()->IsFighting()||GetHostPlayer()->IsDead())
		SetAutoHelpState(false);	

//	if (m_pAnim)
//	{
//		m_pAnim->Tick(delta);
//	}
}
void CDlgAutoHelp::IncOpenTime(int delta)
{ 
	if(IsForbidAutoHelp()) return;

	if (GetHostPlayer()->GetBattleInfo().nType != CECHostPlayer::BT_NONE)
		return ;

	m_openTime += delta;

	if (m_openTime>= CECUIConfig::Instance().GetGameUI().nOpenWikiPopDlgTime)
	{
		m_openTime = 0;
		Show(true);
	}
}
void CDlgAutoHelp::OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if(!GetDlgItem("Btn_Close")->IsShow())
		GetDlgItem("Btn_Close")->Show(true);
}
void CDlgAutoHelp::SetAutoHelpState(bool bAuto)
{ 
	if(bAuto != m_bAutoHelp)
	{
		SetMsg(GetGameUIMan()->GetStringFromTable(bAuto ? 10751:10752));
		m_bAutoHelp = bAuto;
		m_autoHelpCloseTime=0;	
	}	
	if(bAuto) {
		m_closeTime = 0;
		m_bAutoDisappear = false;
		ScopedDialogSpriteModify _dummy(this);
		m_pImg_Girl->SetProperty("Image File", (PAUIOBJECT_SETPROPERTY)GirlImagePushed);
	} else {
		ScopedDialogSpriteModify _dummy(this);
		m_pImg_Girl->SetProperty("Image File", (PAUIOBJECT_SETPROPERTY)GirlImageNormal);
	}
}
bool CDlgAutoHelp::IsAutoHelp()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	return pGameUI->m_pDlgAutoHelp->m_bAutoHelp;
}
void CDlgAutoHelp::SetForbidAutoHelpFlag(bool bForbid)
{
	EC_GAME_SETTING setting = GetGame()->GetConfigs()->GetGameSettings();
	setting.bDisableAutoWikiHelp = bForbid;
	GetGame()->GetConfigs()->SetGameSettings(setting);
}
bool CDlgAutoHelp::IsForbidAutoHelp()
{
	return GetGame()->GetConfigs()->GetGameSettings().bDisableAutoWikiHelp;
}