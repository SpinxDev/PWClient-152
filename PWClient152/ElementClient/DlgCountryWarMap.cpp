// Filename	: DlgCountryWarMap.cpp
// Creator	: Han Guanghui
// Date		: 2012/7/31

#include "DlgCountryWarMap.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_CountryConfig.h"
#include "DlgWorldMap.h"
#include "DlgCountryMap.h"
#include "EC_UIHelper.h"
#include "EC_UIConfigs.h"
#include "A3DGFXExMan.h"

#include "A2DSprite.h"
#include "A2DSpriteItem.h"

AUI_BEGIN_COMMAND_MAP(CDlgCountryWarMap, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_ON_COMMAND("Btn_Refresh",		OnCommandRefresh)
AUI_ON_COMMAND("Btn_CountryMap",	OnCommandOpenCountryMap)
AUI_ON_COMMAND("Btn_Leave",			OnCommandLeave)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCountryWarMap, CDlgBase)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONUP,	OnEventLButtonUp_CountryMap)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONUP,	OnEventRButtonUp_CountryMap)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONDBLCLK,	OnEventLButtonDBClick)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONDOWN,	OnEventRButtonDown)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONDBLCLK,	OnEventRButtonDBClick)
AUI_ON_EVENT("CountryMap",	WM_MBUTTONDOWN,	OnEventMButtonDown)
AUI_ON_EVENT("CountryMap",	WM_MBUTTONUP,	OnEventMButtonUp)
AUI_ON_EVENT("CountryMap",	WM_MBUTTONDBLCLK,	OnEventMButtonDBClick)
AUI_END_EVENT_MAP()

static const char*			FLAG_PATH = "number\\旗帜.tga";
static const char*			FLAG_DEST_PATH = "Number\\战场抢旗按钮.tga";

static const char*			SKULL_PATH[2] = {"图标\\kulou_offense.dds", "图标\\kulou_defence.dds"};

static const unsigned long	FLAG_DEST_ANIMATION_FRAMES = 2;
static const DWORD			FLAG_DEST_ANIMATION_COUNT = 100;

// convert team member pos to screen pixel
struct PosConvertorCountryWar : public CECMapDlgsMgr::PosConvertor
{
	PosConvertorCountryWar(int nInstanceID)
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		PAUIDIALOG pDlg = pGameUI->GetDialog("Win_CountryWarMap");
		
		// get the background center
		A3DRECT rcItem = pDlg->GetDlgItem("CountryMap")->GetRect();
		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		absCenter = rcItem.CenterPoint() + A3DPOINT2(p->X, p->Y);
		
		this->nInstanceID = nInstanceID;
	}
	
	virtual A3DPOINT2 operator()(const A3DVECTOR3& v) const
	{		
		CECMapDlgsMgr* pMgr = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetMapDlgsMgr();
		A3DPOINT2 ret = absCenter + pMgr->GetPixelsByPos(v.x, v.z, nInstanceID);
		
		return ret;
	}
	
	virtual A3DVECTOR3 operator()(const A3DPOINT2& ret) const
	{		
		CECMapDlgsMgr* pMgr = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetMapDlgsMgr();
		APointF	v = pMgr->GetPosByPixels(ret.x - absCenter.x, ret.y - absCenter.y, nInstanceID);
		
		return A3DVECTOR3(v.x, 0.f, v.y);
	}
	
private:
	int nInstanceID;
	A3DPOINT2 absCenter;
};

CDlgCountryWarMap::CDlgCountryWarMap():
m_pImgWarMap(NULL),
m_pTxtWarTime(NULL),
m_pTxtKillCount(NULL),
m_pTxtDeathCount(NULL),
m_pTxtCountryKill(NULL),
m_pTxtCountryDeath(NULL),
m_pTxtComradesCount(NULL),
m_pTxtEnemysCount(NULL),
m_pTxtCombatTime(NULL),
m_pImgFlagDest(NULL),
m_pImgFlag(NULL),
m_pCheckRank(NULL),
m_pCheckDeath(NULL),
m_iUpdateLiveShowTime(0)
{
	memset(m_LiveShowDeathImages, 0, sizeof(m_LiveShowDeathImages));
}

CDlgCountryWarMap::~CDlgCountryWarMap()
{
	A3DRELEASE(m_pImgFlag);
	A3DRELEASE(m_pImgFlagDest);
}

void CDlgCountryWarMap::OnCommandCancel(const char * szCommand)
{
	Show(false);
	int nItem;
	A2DSprite *pA2DSprite;
	m_pImgWarMap->GetCover(&pA2DSprite, &nItem);
	A3DRELEASE(pA2DSprite);
	m_pImgWarMap->ClearCover();
}

void CDlgCountryWarMap::OnCommandRefresh(const char * szCommand)
{
	UpdateBattleInfo();
}

void CDlgCountryWarMap::OnCommandOpenCountryMap(const char * szCommand)
{
	OnCommandCancel("IDCANCEL");
	CECUIHelper::ShowCountryMap();
}

void CDlgCountryWarMap::OnCommandLeave(const char * szCommand)
{
	if (!GetHostPlayer()->CanMove())
	{
		//	死亡、摆摊等禁用，采用可移动逻辑处理
		return;
	}
	
	//	离开确定
	ACString strMsg = GetStringFromTable(9915);
	GetGameUIMan()->MessageBox("Country_LeaveWar2", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgCountryWarMap::OnTick()
{
	int current_time = g_pGame->GetServerAbsTime();
	if ((m_pCheckRank->IsChecked() || m_pCheckDeath->IsChecked()) &&
		current_time - m_iUpdateLiveShowTime >= CECUIConfig::Instance().GetGameUI().nCountryWarLiveShowUpdateInterval) {
		m_iUpdateLiveShowTime = current_time;
		if (!GetHostPlayer()->GetCoolTime(GP_CT_COUNTRYBATTLE_LIVESHOW)){
			GetGameSession()->c2s_CmdCountryBattleLiveShow();
		}
	}
	TickRank();
}

bool CDlgCountryWarMap::OnInitDialog()
{
	DDX_Control("CountryMap", m_pImgWarMap);
	DDX_Control("Lbl_Time", m_pTxtCombatTime);
	DDX_Control("Lbl_WarTime", m_pTxtWarTime);
	DDX_Control("Lbl_PKill", m_pTxtKillCount);
	DDX_Control("Lbl_PDie", m_pTxtDeathCount);
	DDX_Control("Lbl_Kill", m_pTxtCountryKill);
	DDX_Control("Lbl_Die", m_pTxtCountryDeath);
	DDX_Control("Lbl_People1", m_pTxtComradesCount);
	DDX_Control("Lbl_People2", m_pTxtEnemysCount);
	DDX_Control("Chk_HighPoint", m_pCheckRank);
	DDX_Control("Chk_Death", m_pCheckDeath);
	m_pCheckRank->Check(true);
	m_pCheckDeath->Check(true);
	CECDomainCountry::Load2DSprite(m_pImgFlag, FLAG_PATH);
	CECDomainCountry::Load2DSprite(m_pImgFlagDest, FLAG_DEST_PATH);
	if (m_pImgFlagDest)
	{
		CECDomainCountry::SetImageRects(m_pImgFlagDest, 1, FLAG_DEST_ANIMATION_FRAMES);
		m_pImgFlagDest->SetLocalCenterPos(0, 0);
	}
	m_AnimFlagDest.SetImage(m_pImgFlagDest, FLAG_DEST_ANIMATION_COUNT, FLAG_DEST_ANIMATION_FRAMES);
	return true;
}

void CDlgCountryWarMap::OnShowDialog()
{
	UpdateBattleInfo();
	UpdateLiveShowImg();
	if(!GetGameUIMan()->GetMapDlgsMgr()->LoadMapTexture(m_pA3DDevice, m_pImgWarMap, CECMapDlgsMgr::MAP_TYPE_NORMAL))
	{
		ASSERT(false);
		return;
	}
}

void CDlgCountryWarMap::OnHideDialog()
{	
	PAUIDIALOG pDlgLeaveConfirm = GetGameUIMan()->GetDialog("Country_LeaveWar2");
	if (pDlgLeaveConfirm)
		pDlgLeaveConfirm->OnCommand("IDCANCEL");
	for (int i = 0; i < 2; ++i) {
		ReleaseImages(m_LiveShowRankImages[i]);
		A3DRELEASE(m_LiveShowDeathImages[i]);
	}
}

bool CDlgCountryWarMap::Render()
{
	bool bval = CDlgBase::Render();
	int nInstanceID = GetWorld()->GetInstanceID();
	PosConvertorCountryWar conv(nInstanceID);
	RenderDeath();
	GetGameUIMan()->GetMapDlgsMgr()->DrawTeamMark(conv);
	GetGameUIMan()->GetMapDlgsMgr()->DrawHostMark(conv);
	RenderRank();
	RenderFlag();
	RenderFlagDestination();
	RenderStrongHoldState();
	return bval;
}

bool CDlgCountryWarMap::RenderFlag()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost->IsInCountryWar()) return false;

	if (pHost->HasCountryWarFlagCarrier())
	{
		PosConvertorCountryWar conv(GetWorld()->GetInstanceID());
		A3DVECTOR3 flagPos = pHost->GetCountryWarFlagCarrierPos();
		A3DPOINT2 pt = conv(flagPos);
		if (m_pImgFlag)
			m_pImgFlag->DrawToBack(float(pt.x), float(pt.y));
	}

	return true;
}

bool CDlgCountryWarMap::RenderFlagDestination()
{
	CECHostPlayer* pHost = GetHostPlayer();
	const CECHostPlayer::BATTLEINFO &bInfo = pHost->GetBattleInfo();
	if (pHost->IsInBattle() && pHost->GetBattleInfo().IsFlagCarrier())
	{
		bool bAttack = (pHost->GetBattleCamp() == GP_BATTLE_CAMP_INVADER);
		const float *flagGoal = CECCountryConfig::Instance().GetFlagGoal(bAttack);
		A3DVECTOR3 vecFlagGoal(flagGoal[0], flagGoal[1], flagGoal[2]);
		PosConvertorCountryWar conv(GetWorld()->GetInstanceID());
		A3DPOINT2 pt = conv(vecFlagGoal);
		m_AnimFlagDest.LoopImageAnimation();
		if (m_pImgFlagDest)
		{
			A3DRECT rcSize = m_pImgFlagDest->GetItem(0)->GetRect();
			m_pImgFlagDest->DrawToBack(float(pt.x) - rcSize.Width() / 2, float(pt.y) - rcSize.Height() / 2);
		}
	}
	return true;
}

void CDlgCountryWarMap::RenderStrongHoldState()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost->IsInCountryWar()) return;
	if (!GetWorld()->IsCountryWarStrongHoldMap()) return;
	
	const CECHostPlayer::BATTLEINFO &binfo = GetHostPlayer()->GetBattleInfo();
	if (binfo.iStrongHoldCount <= 0) return;
	
	PosConvertorCountryWar conv(GetWorld()->GetInstanceID());
	for (int i(0); i < binfo.iStrongHoldCount; ++ i)
	{
		const float *p = CECCountryConfig::Instance().GetStrongHoldPos(i);
		if (!p)	continue;
		A3DVECTOR3 pos(p[0], p[1], p[2]);
		A2DSprite *pSprite = CECCountryConfig::Instance().GetStrongHoldStateIcon(binfo.iStrongHoldState[i]);
		if (!pSprite) continue;
		A3DPOINT2 pt = conv(pos);
		pSprite->DrawToBack(float(pt.x), float(pt.y));
	}
}

void CDlgCountryWarMap::UpdateBattleInfo()
{
	GetGameSession()->c2s_CmdSendCountryGetPersonalScore();
	if (GetWorld()->IsCountryWarStrongHoldMap())
		GetGameSession()->c2s_CmdCountryGetStrongHoldState();
	GetGameUIMan()->m_pDlgCountryMap->GetMap();
	GetDlgItem("Btn_Refresh")->Enable(false);
}

void CDlgCountryWarMap::OnGetPersonalScore()
{
	GetDlgItem("Btn_Refresh")->Enable(true);
	if(GetHostPlayer()->IsInCountryWar())
	{
		const CECHostPlayer::BATTLEINFO &binfo = GetHostPlayer()->GetBattleInfo();
		
		ACString strTemp;
		strTemp.Format(GetStringFromTable(9916), GetGameUIMan()->GetFormatTimeSimple(binfo.iCombatTime));
		m_pTxtCombatTime->SetText(strTemp);
		strTemp.Format(GetStringFromTable(9917), GetGameUIMan()->GetFormatTimeSimple(binfo.iAttendTime));
		m_pTxtWarTime->SetText(strTemp);

		strTemp.Format(GetStringFromTable(9909), binfo.iKillCount);
		m_pTxtKillCount->SetText(strTemp);
		strTemp.Format(GetStringFromTable(9910), binfo.iDeathCount);
		m_pTxtDeathCount->SetText(strTemp);

		strTemp.Format(GetStringFromTable(9911), binfo.iCountryKillCount);
		m_pTxtCountryKill->SetText(strTemp);
		strTemp.Format(GetStringFromTable(9912), binfo.iCountryDeathCount);
		m_pTxtCountryDeath->SetText(strTemp);
	}
}

void CDlgCountryWarMap::OnEventLButtonUp_CountryMap(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	A3DPOINT2 pt(GET_X_LPARAM(lParam) - p->X, GET_Y_LPARAM(lParam) - p->Y);
	
	if (wParam & MK_CONTROL)
	{
		OnCommandCancel("IDCANCEL");

		int nInstanceID = GetWorld()->GetInstanceID();
		if (nInstanceID != 146)
		{
			PosConvertorCountryWar conv(nInstanceID);
			A3DVECTOR3 v = conv(pt);
			GetGameSession()->c2s_CmdGoto(v.x, 1.0f, v.z);
		}
		return;
	}
}

void CDlgCountryWarMap::OnEventRButtonUp_CountryMap(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommandCancel("IDCANCEL");
}

void CDlgCountryWarMap::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);
}
void CDlgCountryWarMap::OnEventLButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);
}
void CDlgCountryWarMap::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);	
}
void CDlgCountryWarMap::OnEventRButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);
}
void CDlgCountryWarMap::OnEventMButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);
}
void CDlgCountryWarMap::OnEventMButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);
}
void CDlgCountryWarMap::OnEventMButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(m_pTxtCountryKill);
}

void CDlgCountryWarMap::OnGetMapInfo()
{
	if(GetHostPlayer()->IsInCountryWar())
	{
		int iDomainloc = GetGameUIMan()->m_pDlgCountryMap->GetDomainLoc();
		const DOMAIN2_INFO* info = CECDomainCountryInfo::GetInstance()->Find(iDomainloc);
		int idCountry = GetHostPlayer()->GetCountry();
		if (info && CECCountryConfig::Instance().ValidateID(idCountry)
			&& CECCountryConfig::Instance().ValidateID(info->owner)
			&& CECCountryConfig::Instance().ValidateID(info->challenger))
		{
			ACString strTemp;
			bool isInvader = idCountry == info->challenger;
			int iDefenses = info->player_count[info->owner - 1];
			int iInvaders = info->player_count[info->challenger - 1];
			int iComrades = isInvader ? iInvaders : iDefenses;
			int iEnemys = isInvader ? iDefenses : iInvaders;
			strTemp.Format(GetStringFromTable(9931), iComrades);
			m_pTxtComradesCount->SetText(strTemp);
			strTemp.Format(GetStringFromTable(9932), iEnemys);
			m_pTxtEnemysCount->SetText(strTemp);
		}
	}
}

void CDlgCountryWarMap::RenderRank()
{
	if (!m_pCheckRank->IsChecked()) return;
	CECHostPlayer* pHost = GetHostPlayer();
	CECHostPlayer::BATTLEINFO& battle_info = pHost->GetBattleInfo();
	CECHostPlayer::BATTLEINFO::ScoreRankContainer* ranks[2] = { &battle_info.OffenseRanks, &battle_info.DefenceRanks };
	PosConvertorCountryWar conv(GetWorld()->GetInstanceID());
	A3DGFXExMan* pGfxMan = g_pGame->GetA3DGFXExMan();
	for (int i = 0; i < 2; ++i) {
		size_t sum = ranks[i]->size();
		for (size_t j = 0; j < sum; ++j) {
			if (j < m_LiveShowRankImages[i].size())	{
				A3DPOINT2 pt = conv((*ranks[i])[j].pos);
				if (m_LiveShowRankImages[i][j])
				{
					pGfxMan->RegisterGfx(m_LiveShowRankImages[i][j]);
					pGfxMan->Render2DGfx(false);
				}
			}
		}
	}
}

void CDlgCountryWarMap::RenderDeath()
{
	if (!m_pCheckDeath->IsChecked()) return;
	CECHostPlayer* pHost = GetHostPlayer();
	CECHostPlayer::BATTLEINFO& battle_info = pHost->GetBattleInfo();
	CECHostPlayer::BATTLEINFO::DeathContainer* deaths[2] = { &battle_info.OffenseDeaths, &battle_info.DefenceDeaths };
	PosConvertorCountryWar conv(GetWorld()->GetInstanceID());
	int current_time = g_pGame->GetServerAbsTime();
	for (unsigned i = 0; i < 2; ++i) {
		size_t sum = deaths[i]->size();
		for (size_t j = 0; j < sum; ++j) {
			const S2C::cmd_countrybattle_live_show_result::death_entry& entry = (*deaths[i])[j];
			A3DPOINT2 pt = conv(entry.pos);
			if (m_LiveShowDeathImages[i]) {
				m_LiveShowDeathImages[i]->SetColor(
					A3DCOLORRGBA(255, 255, 255, GetSkullAlpha(entry.timestamp, current_time)));
				m_LiveShowDeathImages[i]->DrawToBack(float(pt.x), float(pt.y));
			}
		}
	}
}

void CDlgCountryWarMap::ReleaseImages(ImageContainer& container)
{
	ImageContainer::iterator iter = container.begin();
	ImageContainer::iterator iterEnd = container.end();
	while(iter != iterEnd) {
		AfxGetGFXExMan()->CacheReleasedGfx(*iter);
		++iter;
	}
	container.clear();
}

void CDlgCountryWarMap::UpdateLiveShowImg()
{
	CECHostPlayer* pHost = GetHostPlayer();
	CECHostPlayer::BATTLEINFO& battle_info = pHost->GetBattleInfo();
	CECHostPlayer::BATTLEINFO::ScoreRankContainer* ranks[2] = { &battle_info.OffenseRanks, &battle_info.DefenceRanks };
	for (int i = 0; i < 2; ++i) {
		size_t sum_new = ranks[i]->size();
		for (size_t j = 0; j < sum_new; ++j) {
			if (j >= m_LiveShowRankImages[i].size()) {
				AString strImg = GetRankImage(i == 0, j + 1);
				if (!strImg.IsEmpty()) {
					A3DGFXEx* pGfx = g_pGame->GetA3DGFXExMan()->LoadGfx(m_pA3DDevice, strImg);
					m_LiveShowRankImages[i].push_back(pGfx);
					if (pGfx) pGfx->Start();
				}
			}
		}
	}
	CECHostPlayer::BATTLEINFO::DeathContainer* deaths[2] = { &battle_info.OffenseDeaths, &battle_info.DefenceDeaths };
	for (i = 0; i < 2; ++i) {
		if (m_LiveShowDeathImages[i] == NULL) {
			A2DSprite* pImg = NULL;
			CECDomainCountry::Load2DSprite(pImg, SKULL_PATH[i]);
			m_LiveShowDeathImages[i] = pImg;
		}
	}
}

AString CDlgCountryWarMap::GetRankImage(bool offense, int rank)
{
	AString ret;
	if (rank >= 1 && rank < 6)
		ret.Format("界面\\%s%d.gfx", offense ? "offense" : "defence", rank);
	return ret;
}

int CDlgCountryWarMap::GetSkullAlpha(int time_stamp, int current_time)
{
	int ret(0);
	int time_passed = current_time - time_stamp;
	if (time_passed >=0 && time_passed < 25)
		ret = 255 - 255 * time_passed / 25;
	return ret;
}

void CDlgCountryWarMap::TickRank()
{
	CECHostPlayer* pHost = GetHostPlayer();
	CECHostPlayer::BATTLEINFO& battle_info = pHost->GetBattleInfo();
	CECHostPlayer::BATTLEINFO::ScoreRankContainer* ranks[2] = { &battle_info.OffenseRanks, &battle_info.DefenceRanks };
	PosConvertorCountryWar conv(GetWorld()->GetInstanceID());
	for (int i = 0; i < 2; ++i) {
		size_t sum = ranks[i]->size();
		for (size_t j = 0; j < sum; ++j) {
			if (j < m_LiveShowRankImages[i].size())	{
				A3DPOINT2 pt = conv((*ranks[i])[j].pos);
				if (m_LiveShowRankImages[i][j])
				{
					m_LiveShowRankImages[i][j]->SetParentTM(a3d_Translate(pt.x, pt.y, 0));
					m_LiveShowRankImages[i][j]->TickAnimation(g_pGame->GetTickTime());
				}
			}
		}
	}
}

void CDlgCountryWarMap::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	for (int i = 0; i < 2; ++i) {
		size_t sum = m_LiveShowRankImages[i].size();
		for (size_t j = 0; j < sum; ++j) {
			if (m_LiveShowRankImages[i][j])
				m_LiveShowRankImages[i][j]->SetScale(GetAUIManager()->GetWindowScale() * GetScale());
		}
	}	
}