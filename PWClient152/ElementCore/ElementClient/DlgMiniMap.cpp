// Filename	: DlgMiniMap.cpp
// Creator	: Tom Zhou
// Date		: October 15, 2005

#include "A3DFuncs.h"
#include "A2DSprite.h"
#include "A3DStream.h"
#include "A3DCamera.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DFlatCollector.h"
#include "A3DGdi.h"
#include "A3DGFXEx.h"
#include "CSplit.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUICheckBox.h"
#include "AUIImagePicture.h"
#include "DlgMiniMap.h"
#include "DlgSetting.h"
#include "DlgInputName.h"
#include "DlgWorldMap.h"
#include "DlgSystem.h"
#include "DlgActivity.h"
#include "DlgWiki.h"
#include "DlgTouchShop.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EL_Precinct.h"
#include "EC_NPCServer.h"
#include "EC_ManNPC.h"
#include "EC_Friend.h"
#include "EC_ElsePlayer.h"
#include "EC_ManPlayer.h"
#include "EC_Team.h"
#include "EC_Instance.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_Configs.h"
#include "EC_Utility.h"
#include "EC_SunMoon.h"
#include "EC_Faction.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "EC_CrossServer.h"
#include "globaldataman.h"
#include "EL_Precinct.h"
#include "DlgMailToFriends.h"
#include "EC_DomainCountry.h"
#include "EC_DomainGuild.h"
#include "DlgCountryMap.h"
#include "DlgCountryWarMap.h"
#include "DlgGuildMap.h"
#include "DlgGuildMapStatus.h"
#include "DlgAutoLock.h"
#include "TaskTemplMan.h"
#include "EC_UIHelper.h"
#include "EC_IntelligentRoute.h"
#include "DlgTitleChallenge.h"
#include "EC_FactionPVP.h"
#include "EC_TimeSafeChecker.h"
#include "EC_RandomMapPreProcessor.h"

#define new A_DEBUG_NEW
#define SQUARE(x)	((x) * (x))
#define Distance(p1, p2)	sqrt(SQUARE(p1.x - p2.x) + SQUARE(p1.y - p2.y) + SQUARE(p1.z - p2.z))

AUI_BEGIN_COMMAND_MAP(CDlgMiniMap, CDlgBase)

AUI_ON_COMMAND("treasurebag",	OnCommand_treasurebag)
AUI_ON_COMMAND("closeall",		OnCommand_closeall)
AUI_ON_COMMAND("openmarkwin",	OnCommand_openmarkwin)
AUI_ON_COMMAND("bigmap",		OnCommand_bigmap)
AUI_ON_COMMAND("roomout",		OnCommand_roomout)
AUI_ON_COMMAND("roomin",		OnCommand_roomin)
AUI_ON_COMMAND("mode",			OnCommand_mode)
AUI_ON_COMMAND("makemark",		OnCommand_makemark)
AUI_ON_COMMAND("listregion",	OnCommand_listregion)
AUI_ON_COMMAND("listchoose",	OnCommand_listchoose)
AUI_ON_COMMAND("edit",			OnCommand_edit)
AUI_ON_COMMAND("radar",			OnCommand_radar)
AUI_ON_COMMAND("arrow",			OnCommand_arrow)
AUI_ON_COMMAND("delete",		OnCommand_delete)
AUI_ON_COMMAND("deleteall",		OnCommand_deleteall)
AUI_ON_COMMAND("wiki",			OnCommand_wiki)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("radio",			OnCommand_Radio)
AUI_ON_COMMAND("btn_mailtofriends", OnCommand_MailToFriends)
AUI_ON_COMMAND("btn_touch",		OnCommand_TouchShop)
AUI_ON_COMMAND("Btn_OpenEvent", OnCommand_TitleChallenge)
AUI_ON_COMMAND("Btn_Calendar",	OnCommand_Activity)
AUI_ON_COMMAND("Chk_MapMonster",OnCommand_mapmonster)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMiniMap, CDlgBase)

AUI_ON_EVENT("Hide_2",		WM_MOUSEMOVE,		OnEventMouseMove_Hide_2)
AUI_ON_EVENT("Hide_2",		WM_LBUTTONUP,		OnEventLButtonUp_Hide_2)

AUI_END_EVENT_MAP()

int CDlgMiniMap::m_nMode;
float CDlgMiniMap::m_fZoom;
bool CDlgMiniMap::m_bShowMark;
bool CDlgMiniMap::m_bShowTargetArrow;
A3DTexture * CDlgMiniMap::m_pA3DTexMask = NULL;
A3DTexture * CDlgMiniMap::m_pA3DRadarBack = NULL;
abase::vector<CDlgMiniMap::MARK> CDlgMiniMap::m_vecMark;
abase::vector<CDlgMiniMap::MARK> CDlgMiniMap::m_vecNPCMark;
CDlgMiniMap::MAPTLVERTEX CDlgMiniMap::m_vertexMiniMap[MINIMAP_MAX][4];
A3DStream * CDlgMiniMap::m_pMiniMapStream;
abase::hash_map<AString, A3DTexture *> CDlgMiniMap::m_TexMap;
A2DSprite * CDlgMiniMap::m_pA2DSpriteHostDir = NULL;
A2DSprite * CDlgMiniMap::m_pA2DSpriteCompass = NULL;
A2DSprite * CDlgMiniMap::m_pA2DMapMark = NULL;
A2DSprite * CDlgMiniMap::m_pA2DRadar_NPC = NULL;
A2DSprite * CDlgMiniMap::m_pA2DRadar_TaskNPC = NULL;
A2DSprite * CDlgMiniMap::m_pA2DRadar_FTaskNPC = NULL;
A2DSprite * CDlgMiniMap::m_pA2DSpritePet = NULL;
A2DSprite * CDlgMiniMap::m_pA2DSpriteAutoMove = NULL;
int CDlgMiniMap::m_nHostDirFrames = 10;
abase::vector<CDlgMiniMap::MARK> CDlgMiniMap::m_vecTeamMate;
A3DVECTOR3 CDlgMiniMap::m_vecNowTarget;

static const char* TREASURE_DETECT_GFX[ATaskTempl::DISTANCE_NUM] = {
	"无特效gfx",
	"程序联入\\藏宝图特效1.gfx",
	"程序联入\\藏宝图特效2.gfx",
	"程序联入\\藏宝图特效3.gfx",
	"程序联入\\藏宝图特效4.gfx",
};

CDlgMiniMap::CDlgMiniMap()
{
	int i, j;

	m_fZoom = 1.0f;
	m_nMode = MODE_B;
	m_bShowMark = true;
	m_bShowTargetArrow = true;
	m_vecNowTarget = A3DVECTOR3(0.0f);
	m_bActivityClicked = false;
	m_iUpdateProfitTime = 0;
	m_iUpdateTitleTime = 0;
	m_TreasureUpdateCounter.SetPeriod(1000);
	m_factionPVPMapUpdateCounter.SetPeriod(30*1000);

	for( i = 0; i < MINIMAP_MAX; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			m_vertexMiniMap[i][j].z = 0.0f;
			m_vertexMiniMap[i][j].rhw = 1.0f;
			m_vertexMiniMap[i][j].diffuse = 0xFFFFFFFF;
			m_vertexMiniMap[i][j].specular = 0xFF000000;
		}
	}

	m_vecMark.clear();
	m_vecNPCMark.clear();
	m_TexMap.clear();
	m_vecTeamMate.clear();

	CECPlayerLevelRankRealmChangeCheck::Instance().RegisterObserver(this);
}

CDlgMiniMap::~CDlgMiniMap()
{
	if( m_pMiniMapStream )
	{
		m_pMiniMapStream->Release();
		delete m_pMiniMapStream;
		m_pMiniMapStream = NULL;
	}

	m_vecMark.clear();
	m_vecNPCMark.clear();
	m_vecTeamMate.clear();

	abase::hash_map<AString, A3DTexture *>::iterator it;
	for( it = m_TexMap.begin(); it != m_TexMap.end(); ++it )
	{
		if(it->second == m_pA3DRadarBack) continue;
		A3DRELEASE(it->second);
	}
	m_TexMap.clear();

	A3DRELEASE(m_pA2DSpriteHostDir);
	A3DRELEASE(m_pA2DSpriteCompass);
	A3DRELEASE(m_pA2DMapMark);
	A3DRELEASE(m_pA3DTexMask);
	A3DRELEASE(m_pA3DRadarBack);
	A3DRELEASE(m_pA2DRadar_NPC);
	A3DRELEASE(m_pA2DRadar_TaskNPC);
	A3DRELEASE(m_pA2DRadar_FTaskNPC);
	A3DRELEASE(m_pA2DSpritePet);
	A3DRELEASE(m_pA2DSpriteAutoMove);

	CECPlayerLevelRankRealmChangeCheck::Instance().UnregisterObserver(this);
}

void CDlgMiniMap::OnTick()
{
	if( stricmp(GetName(), "Win_Map") == 0 )
	{
		DWORD dwTick = GetTickCount();
		if( m_pA2DSpriteHostDir )
			m_pA2DSpriteHostDir->SetCurrentItem( dwTick / 200 % m_nHostDirFrames );
		if( !m_pChk_Mail->IsChecked() )
			if( (int)m_pChk_Mail->GetData() == -1 ||
				m_pChk_Mail->GetData() != 0 && 
				GetGame()->GetServerGMTTime() - (int)m_pChk_Mail->GetData() > 0 )
			{
				m_pChk_Mail->SetData(0);
				GetGameSession()->mail_CheckNew();
			}

		// 更新玩家的收益时间信息
		PAUIIMAGEPICTURE pFatigue;
		GetGameUIMan()->GetMouseOn(NULL, (PAUIOBJECT*)(&pFatigue));
		if( pFatigue && !stricmp(pFatigue->GetName(),"Img_Fatigue") )
		{
			if(CECTimeSafeChecker::ElapsedTime(dwTick, m_iUpdateProfitTime) > 16000 )
			{
				GetGameSession()->c2s_SendCmdQueryProfitTime();
				m_iUpdateProfitTime = dwTick;
			}
		}

		CECHostPlayer::BATTLEINFO binfo = GetHostPlayer()->GetBattleInfo();
		PAUILABEL pLabel;
		if( binfo.idBattle )
		{
			pLabel = (PAUILABEL)GetDlgItem("Txt_RestTime");
			ACString strText;
			int nRestTime = binfo.iEndTime - GetGame()->GetServerGMTTime();
			if( nRestTime < 0 )
				nRestTime = 0;
			strText.Format(GetStringFromTable(798),
				nRestTime / 3600, (nRestTime / 60) % 60, nRestTime % 60);
			if( pLabel )
				pLabel->SetText(strText);
			if( GetWorld()->GetInstanceID() % 2 == 1 && 
				binfo.iMaxScore_I - binfo.iScore_I > 0 &&
				binfo.IsGuildWar())
			{
				pLabel = (PAUILABEL)GetDlgItem("Txt_RestMonster");
				strText.Format(GetStringFromTable(799), binfo.iMaxScore_I - binfo.iScore_I);
				if( pLabel )
					pLabel->SetText(strText);
			}
			else
			{
				pLabel = (PAUILABEL)GetDlgItem("Txt_RestMonster");
				if( pLabel )
					pLabel->SetText(_AL(""));
			}
			CECHostPlayer *pHost = GetHostPlayer();
			if( pHost->GetBattleTankNum() > 0 )
			{
				pLabel = (PAUILABEL)GetDlgItem("Txt_TankDistance");
				if( pLabel )
					pLabel->SetText(GetStringFromTable(800));
			}
			else
			{
				pLabel = (PAUILABEL)GetDlgItem("Txt_TankDistance");
				if( pLabel )
					pLabel->SetText(_AL(""));
			}
		}
		else
		{
			pLabel = (PAUILABEL)GetDlgItem("Txt_RestTime");
			if( pLabel )
			{
				pLabel->SetText(_AL(""));
				
				//	显示基地倒计时信息
				if (GetHostPlayer()->IsInFortress())
				{
					const CECHostPlayer::FACTION_FORTRESS_ENTER *pEnterInfo = GetHostPlayer()->GetFactionFortressEnter();
					if (pEnterInfo->end_time)
					{
						//	不是战斗时间
						ACString strText;
						int nRestTime = pEnterInfo->end_time - GetGame()->GetServerGMTTime();
						if( nRestTime < 0 )
							nRestTime = 0;
						strText.Format(GetStringFromTable(798),	nRestTime/3600, (nRestTime/60)%60, nRestTime%60);

						pLabel->SetText(strText);
					}
				}
			}
			pLabel = (PAUILABEL)GetDlgItem("Txt_RestMonster");
			if( pLabel )
				pLabel->SetText(_AL(""));
			pLabel = (PAUILABEL)GetDlgItem("Txt_TankDistance");
			if( pLabel )
				pLabel->SetText(_AL(""));
		}

		m_pCheck_Mark->Check(GetGameUIMan()->m_pDlgMiniMapMark->IsShow());

		PAUIOBJECT pObj = GetDlgItem("btn_mailtofriends");
		if (pObj && pObj->IsShow())
		{
			if (GetGameUIMan()->m_pDlgMailToFriends->NeedAttrackAttention())
			{	
				if (!pObj->GetFlash())
					pObj->SetFlash(true);
			}
			else
			{
				if (pObj->GetFlash())
					pObj->SetFlash(false);
			}

		}
		
		const EC_VIDEO_SETTING &vs = GetGame()->GetConfigs()->GetVideoSettings();
		m_pChk_MapMonster->Check(vs.bMapMonster);

		UpdateMarks(GetDlgItem("Hide_2"));
		
		PAUIOBJECT pTitleObject = GetDlgItem("Txt_Tips");
		if (pTitleObject && pTitleObject->IsShow()) {
			if (CECTimeSafeChecker::ElapsedTime(dwTick, m_iUpdateTitleTime) > 3000) pTitleObject->Show(false);
		}
		UpdateTreasureMapHintGfx();

		// open event 按钮改为：有事件时显示，没有事件的时候不显示
		PAUIOBJECT pOpenEvent = GetDlgItem("Btn_OpenEvent");
		if (pOpenEvent && CECUIConfig::Instance().GetGameUI().bEnableTitle)
		{
			bool bShowWorldEventBnt = GetGameUIMan()->m_pDlgTitleChallenge && GetGameUIMan()->m_pDlgTitleChallenge->GetChallengeCount();
			if(bShowWorldEventBnt != pOpenEvent->IsShow())
				pOpenEvent->Show(bShowWorldEventBnt);
		}

		//	帮派PVP地图定期更新
		if (m_factionPVPMapUpdateCounter.IncCounter(GetGame()->GetRealTickTime())){
			m_factionPVPMapUpdateCounter.Reset();
			CECFactionPVPModel::Instance().GetFactionPVPMap();
		}

		// 切换底图和模式按钮
		if (m_nMode == MODE_A) {
			m_pChk_MapMode->Check(true);
			m_pImg_CoverCircle->Show(true);
			m_pImg_CoverSquare->Show(false);
		} else {
			m_pChk_MapMode->Check(false);
			m_pImg_CoverCircle->Show(false);
			m_pImg_CoverSquare->Show(true);
		}
	}
}

void CDlgMiniMap::UpdateTreasureMapHintGfx()
{
	if (m_TreasureUpdateCounter.IncCounter(GetGame()->GetRealTickTime())) {
		m_TreasureUpdateCounter.Reset();
		ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
		ATaskTempl::TREASURE_DISTANCE_LEVEL level = 
			pMan->CheckTreasureDistance(reinterpret_cast<TaskInterface*>(GetHostPlayer()->GetTaskInterface()));
		if (level >= 0 && level < ATaskTempl::DISTANCE_NUM) {
			PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_TreasureHint"));
			if (pImg) {
				A3DGFXEx* pGfx = pImg->GetGfx();
				if (pGfx == NULL || strstr(pGfx->GetPath(), TREASURE_DETECT_GFX[level]) == NULL) {
					if (level == ATaskTempl::DISTANCE_FAR_FAR_AWAY){
						pImg->Show(false);
						pImg->SetGfx("");
					}else {
						pImg->Show(true);
						pImg->SetGfx(TREASURE_DETECT_GFX[level]);
						GetGameUIMan()->AddChatMessage(GetStringFromTable(11029 + level), GP_CHAT_MISC);
					}
				}
			}
		}
	}
}
bool CDlgMiniMap::OnInitDialog()
{
	bool bval;

	m_pImg_SystemTime = (PAUIIMAGEPICTURE)GetDlgItem("Img_SystemTime");
	if( stricmp(GetName(), "Win_Map") == 0 )
	{
		m_pCheck_Mark = (PAUICHECKBOX)GetDlgItem("Check_Mark");
		m_pChk_Mail = (PAUICHECKBOX)GetDlgItem("Chk_Mail");
		m_pChk_Mail->SetAcceptMouseMessage(false);
		m_pChk_Mail->Check(false);
		m_pChk_Mail->SetHint(_AL(""));
		m_pChk_Mail->SetData(-1);
		if( globaldata_getgshopitems()->size() == 0 )
			GetDlgItem("Btn_TreasureBag")->Show(false);
		m_pChk_MapMonster = (PAUICHECKBOX)GetDlgItem("Chk_MapMonster");
		m_pChk_MapMode = (PAUICHECKBOX)GetDlgItem("Chk_Mode");
		m_pImg_CoverSquare = (PAUIIMAGEPICTURE)GetDlgItem("cover");
		m_pImg_CoverCircle = (PAUIIMAGEPICTURE)GetDlgItem("cover1");
		DisableForCrossServer("Btn_TreasureBag");
	}

	// Mini map.
	if( !m_pA2DSpriteHostDir )
	{
		m_pA2DSpriteHostDir = new A2DSprite;
		if( !m_pA2DSpriteHostDir ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DSpriteHostDir->Init(m_pA3DDevice, "InGame\\HostDir.tga", 0);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		m_nHostDirFrames = 10;
		int nWidth = m_pA2DSpriteHostDir->GetWidth();
		int nHeight = m_pA2DSpriteHostDir->GetHeight() / m_nHostDirFrames;

		A3DRECT a_rc[AUIIMAGEPICTURE_MAXFRAMES];
		for( int i = 0; i < m_nHostDirFrames; i++ )
			a_rc[i].SetRect(0, i * nHeight, nWidth, (i + 1) * nHeight);

		bval = m_pA2DSpriteHostDir->ResetItems(m_nHostDirFrames, a_rc);
		if (!bval) return AUI_ReportError(__LINE__, __FILE__);

		m_pA2DSpriteHostDir->SetLocalCenterPos(
			m_pA2DSpriteHostDir->GetWidth() / 2,
			m_pA2DSpriteHostDir->GetHeight() / ( 2 * m_nHostDirFrames) );
		m_nHostDirFrames --;
		m_pA2DSpriteHostDir->SetLinearFilter(true);
	}

	// Mini map dir.
	if( !m_pA2DSpriteCompass )
	{
		m_pA2DSpriteCompass = new A2DSprite;
		if( !m_pA2DSpriteCompass ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DSpriteCompass->Init(m_pA3DDevice, "InGame\\MiniMapDir.tga", 0);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		m_pA2DSpriteCompass->SetLocalCenterPos(
			m_pA2DSpriteCompass->GetWidth() / 2,
			m_pA2DSpriteCompass->GetHeight() / 2);
		m_pA2DSpriteCompass->SetLinearFilter(true);
	}

	// Mini map mask.
	if( !m_pA3DTexMask )
	{
		m_pA3DTexMask = new A3DTexture;
		AString strFile = "Surfaces\\InGame\\MiniMapMask.tga";
		bval = m_pA3DTexMask->LoadFromFile(m_pA3DDevice, strFile, strFile);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}

	if( !m_pA3DRadarBack )
	{
		m_pA3DRadarBack = new A3DTexture;
		AString strFile = "Surfaces\\InGame\\MiniMapBack.tga";
		bval = m_pA3DRadarBack->LoadFromFile(m_pA3DDevice, strFile, strFile);

		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}

	m_TexMap.clear();

	// Map marks.
	if( !m_pA2DMapMark )
	{
		m_pA2DMapMark = new A2DSprite;
		if( !m_pA2DMapMark ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DMapMark->Init(m_pA3DDevice, "InGame\\MapMark.tga", AUI_COLORKEY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		int i, j, W = 8, H = 8;
		int nNumX = m_pA2DMapMark->GetWidth() / W;
		int nNumY = m_pA2DMapMark->GetHeight() / H;
		A3DRECT *rc = (A3DRECT*)a_malloctemp(sizeof(A3DRECT) * (nNumX * nNumY));

		for( i = 0; i < nNumY; i++ )
		{
			for( j = 0; j < nNumX; j++ )
				rc[i * nNumX + j].SetRect(j * W, i * H, j * W + W, i * H + H);
		}
		m_pA2DMapMark->ResetItems(nNumX * nNumY, rc);
		m_pA2DMapMark->SetLocalCenterPos(W / 2, H / 2);
		a_freetemp(rc);
	}

	if( !m_pA2DRadar_NPC )
	{
		m_pA2DRadar_NPC = new A2DSprite;
		if( !m_pA2DRadar_NPC ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DRadar_NPC->Init(m_pA3DDevice, "InGame\\Radar_NPC.tga", AUI_COLORKEY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		m_pA2DRadar_NPC->SetLocalCenterPos(
			m_pA2DRadar_NPC->GetWidth() / 2,
			m_pA2DRadar_NPC->GetHeight() / 2);
	}

	if( !m_pA2DRadar_TaskNPC )
	{
		m_pA2DRadar_TaskNPC = new A2DSprite;
		if( !m_pA2DRadar_TaskNPC ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DRadar_TaskNPC->Init(m_pA3DDevice, "InGame\\Radar_TaskNPC.tga", AUI_COLORKEY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		m_pA2DRadar_TaskNPC->SetLocalCenterPos(
			m_pA2DRadar_TaskNPC->GetWidth() / 2,
			m_pA2DRadar_TaskNPC->GetHeight() / 2);
	}

	if( !m_pA2DRadar_FTaskNPC )
	{
		m_pA2DRadar_FTaskNPC = new A2DSprite;
		if( !m_pA2DRadar_FTaskNPC ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DRadar_FTaskNPC->Init(m_pA3DDevice, "InGame\\Radar_FTaskNPC.tga", AUI_COLORKEY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		m_pA2DRadar_FTaskNPC->SetLocalCenterPos(
			m_pA2DRadar_FTaskNPC->GetWidth() / 2,
			m_pA2DRadar_FTaskNPC->GetHeight() / 2);
	}

	// now create a dynamic stream for map render
	if( !m_pMiniMapStream )
	{
		m_pMiniMapStream = new A3DStream();
		if( !m_pMiniMapStream->Init(m_pA3DDevice, sizeof(MAPTLVERTEX),
			FVF_MAPTLVERTEX, 6, 0, A3DSTRM_REFERENCEPTR, 0) )
		{
			return AUI_ReportError(__LINE__, __FILE__);
		}
	}

	// pet
	if( !m_pA2DSpritePet )
	{
		m_pA2DSpritePet = new A2DSprite;
		if( !m_pA2DSpritePet ) return AUI_ReportError(__LINE__, __FILE__);

		bval = m_pA2DSpritePet->Init(m_pA3DDevice, "InGame\\Pet.tga", 0);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		m_pA2DSpritePet->SetLocalCenterPos(
			m_pA2DSpritePet->GetWidth() / 2, m_pA2DSpritePet->GetHeight() / 2);
	}

	if( !m_pA2DSpriteAutoMove )
	{
		m_pA2DSpriteAutoMove = new A2DSprite;
		if( !m_pA2DSpriteAutoMove ) return AUI_ReportError(__LINE__, __FILE__);
		
		bval = m_pA2DSpriteAutoMove->Init(m_pA3DDevice, "InGame\\AutoMove.tga", 0);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		m_pA2DSpriteAutoMove->SetLocalCenterPos(
			m_pA2DSpriteAutoMove->GetWidth() / 2,
			m_pA2DSpriteAutoMove->GetHeight() / 2);
		m_pA2DSpriteAutoMove->SetLinearFilter(true);
	}

	// touch
	if (!CECUIConfig::Instance().GetGameUI().bEnableTouch){
		PAUIOBJECT pObj = GetDlgItem("btn_touch");
		if (pObj) pObj->Show(false);
	}
	PAUIOBJECT pTitleObject = GetDlgItem("Txt_Tips");
	if (pTitleObject)pTitleObject->Show(false);
	pTitleObject = GetDlgItem("Btn_OpenEvent");
	if (pTitleObject) pTitleObject->Show(CECUIConfig::Instance().GetGameUI().bEnableTitle);
	pTitleObject = GetDlgItem("Btn_Calendar");
	if (pTitleObject) pTitleObject->SetFlash(true);

	ShowNewImg(false);

	return true;
}

void CDlgMiniMap::OnModelChange(const CECPlayerLevelRankRealmChangeCheck *p, const CECObservableChange *q) {
	if (AString(GetName()) == "Win_Map") {
		const CECPlayerLevelRankRealmChange* change = dynamic_cast<const CECPlayerLevelRankRealmChange*>(q);
		if (change) {
			if (CECAutoTeamConfig::Instance().UpdateNewActivity()) {
				// 显示新活动提示
				ShowNewImg(true);
			}
		}
	}
}

void CDlgMiniMap::OnCommand_treasurebag(const char * szCommand)
{
	GetGameUIMan()->m_pDlgSystem->OnCommandShop("");
}

void CDlgMiniMap::OnCommand_closeall(const char * szCommand)
{
	bool bCheck = ((PAUICHECKBOX)GetDlgItem("Chk_CloseMap"))->IsChecked();
	
	if(GetGameUIMan()->m_pDlgMiniMapMark->IsShow())
		GetGameUIMan()->m_pDlgMiniMapMark->Show(false);

	CDlgMiniMap* pDlg = dynamic_cast<CDlgMiniMap*>(GetGameUIMan()->GetDialog("Win_Map"));
	if (pDlg)
	{
		pDlg->m_pCheck_Mark->Check(false);
		PAUICHECKBOX pCheck = (PAUICHECKBOX)pDlg->GetDlgItem("Chk_CloseInfo");
		if(pCheck) pCheck->Check(false);

		pDlg->Show(!bCheck);
		pDlg->UpdateMarks(pDlg->GetDlgItem("Hide_2"));
	}
}

void CDlgMiniMap::OnCommand_openmarkwin(const char * szCommand)
{
	PAUIDIALOG pShow = GetGameUIMan()->m_pDlgMiniMapMark;
	
	// old : pShow->SetPos(ptPos.x, ptPos.y + 46);
	pShow->Show(!pShow->IsShow());

	ACHAR szText[40];
	A3DVECTOR3 vecPosHost = GetHostPlayer()->GetPos();

	a_sprintf(szText, _AL("%d %d"),
		int(vecPosHost.x) / 10 + 400,
		int(vecPosHost.z) / 10 + 550);
	pShow->GetDlgItem("Txt_Mark")->SetText(szText);
}

void CDlgMiniMap::OnCommand_bigmap(const char * szCommand)
{
	GetGameUIMan()->GetMapDlgsMgr()->SwitchWorldMapShow();
}

void CDlgMiniMap::OnCommand_roomout(const char * szCommand)
{
	if( m_fZoom > 0.5f )
		m_fZoom /= 2.0f;
}

void CDlgMiniMap::OnCommand_roomin(const char * szCommand)
{
	if( m_fZoom < 2.0f )
		m_fZoom *= 2.0f;
}

void CDlgMiniMap::OnCommand_mode(const char * szCommand)
{
	if( m_nMode == MODE_A )
		m_nMode = MODE_B;
	else
		m_nMode = MODE_A;
	SetItemPos();
}

void CDlgMiniMap::SetItemPos() {
	if (MODE_B == m_nMode) {
		GetDlgItem("Chk_MapMonster")->SetDefaultPos(308, 29);
		GetDlgItem("Btn_+")->SetDefaultPos(308, 52);
		GetDlgItem("Btn_-")->SetDefaultPos(308, 75);
		GetDlgItem("Chk_Mode")->SetDefaultPos(308, 98);
		GetDlgItem("Btn_BigMap")->SetDefaultPos(308, 121);
		GetDlgItem("Check_Mark")->SetDefaultPos(308, 144);
		GetDlgItem("Btn_TreasureBag")->SetDefaultPos(350, 165);
		GetDlgItem("Btn_Calendar")->SetDefaultPos(384, 164);
		GetDlgItem("Img_New")->SetDefaultPos(395, 164);
		GetDlgItem("Chk_Mail")->SetDefaultPos(403, 164);
		GetDlgItem("btn_mailtofriends")->SetDefaultPos(427, 164);
	} else {
		GetDlgItem("Chk_MapMonster")->SetDefaultPos(339, 28);
		GetDlgItem("Btn_+")->SetDefaultPos(324, 47);
		GetDlgItem("Btn_-")->SetDefaultPos(316, 70);
		GetDlgItem("Chk_Mode")->SetDefaultPos(316, 94);
		GetDlgItem("Btn_BigMap")->SetDefaultPos(325, 116);
		GetDlgItem("Check_Mark")->SetDefaultPos(339, 135);
		GetDlgItem("Btn_TreasureBag")->SetDefaultPos(357, 149);
		GetDlgItem("Btn_Calendar")->SetDefaultPos(387, 155);
		GetDlgItem("Img_New")->SetDefaultPos(400, 155);
		GetDlgItem("Chk_Mail")->SetDefaultPos(409, 152);
		GetDlgItem("btn_mailtofriends")->SetDefaultPos(431, 141);
	}
}

void CDlgMiniMap::OnCommand_makemark(const char * szCommand)
{
	int nInstance = GetWorld()->GetInstanceID();
	if(!GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstance))
		return;

	if( m_vecMark.size() >= CECGAMEUIMAN_MAX_MARKS )
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(507), GP_CHAT_MISC);
		return;
	}

	ACString strText = GetDlgItem("Txt_Mark")->GetText();
	if( strText.GetLength() <= 0 ) return;

	int i;
	CSplit s(strText);
	CSPLIT_STRING_VECTOR vec;

	for( i = 266; i <= 269; i++ )
	{
		vec = s.Split(GetStringFromTable(i));
		if( vec.size() >= 2 ) break;
	}

	if( vec.size() < 2 )
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(237), GP_CHAT_MISC);
		return;
	}

	GetGameUIMan()->m_vecTargetPos.x = (float)(a_atoi(vec[0]) - 400) * 10.0f;
	GetGameUIMan()->m_vecTargetPos.y = 0.0f;
	GetGameUIMan()->m_vecTargetPos.z = (float)(a_atoi(vec[1]) - 550) * 10.0f;

	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_MAKE_MARK);
}

void CDlgMiniMap::OnCommand_listregion(const char * szCommand)
{
	if( m_vecMark.size() >= CECGAMEUIMAN_MAX_MARKS )
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(507), GP_CHAT_MISC);
		return;
	}

	int i;
	PAUILISTBOX pListRegion = (PAUILISTBOX)GetDlgItem("List_Region");
	PAUILISTBOX pListChoose = (PAUILISTBOX)GetDlgItem("List_Choose");

	MARK Mark;
	CELPrecinct::MARK *pMark = GetGameUIMan()->m_pCurPrecinct->GetMark(pListRegion->GetCurSel());

	Mark.nNPC = pListRegion->GetCurSel();
	Mark.strName = pMark->strName;
	Mark.vecPos = pMark->vPos;
	Mark.mapID = pListRegion->GetItemData(pListRegion->GetCurSel(), 0, "MapID");

	ACString strText = GetMarkDesc(Mark, GetWorld()->GetInstanceID());
	
	for( i = 0; i < (int)m_vecMark.size(); i++ )
	{
		if( m_vecMark[i].nNPC >= 0 &&
			m_vecMark[i].strName == pListRegion->GetText(pListRegion->GetCurSel()) &&
			m_vecMark[i].mapID == Mark.mapID )
		{
			m_vecMark.erase(m_vecMark.begin() + i);
			m_vecMark.insert(m_vecMark.begin() + i, Mark);
			pListChoose->SetItemHint(i, strText);
			return;
		}
	}

	m_vecMark.push_back(Mark);
	pListChoose->AddString(pMark->strName);
	pListChoose->SetItemHint(pListChoose->GetCount() - 1, strText);
}

void CDlgMiniMap::OnCommand_listchoose(const char *szCommand)
{
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");
	int nCurSel = pList->GetCurSel();
	if( nCurSel < 0 || nCurSel >= pList->GetCount() ) return;

	const MARK &mark = m_vecMark[nCurSel];
	if(mark.mapID == GetWorld()->GetInstanceID())
	{
		CECUIHelper::AutoMoveStartComplex(mark.vecPos);
	}
}

void CDlgMiniMap::OnCommand_edit(const char * szCommand)
{
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");
	int nCurSel = pList->GetCurSel();
	if( nCurSel < 0 || nCurSel >= pList->GetCount() ) return;
	
	if( m_vecMark[nCurSel].nNPC >= 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(511),
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		ACString strName = pList->GetText(nCurSel);
		GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_RENAME_MARK, strName);
	}
}

void CDlgMiniMap::OnCommand_radar(const char * szCommand)
{
	m_bShowMark = !m_bShowMark;
}

void CDlgMiniMap::OnCommand_arrow(const char * szCommand)
{
	m_bShowTargetArrow = !m_bShowTargetArrow;
}

void CDlgMiniMap::OnCommand_delete(const char * szCommand)
{
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");

	if( pList->GetCurSel() >= 0 && pList->GetCurSel() < pList->GetCount() )
	{
		GetGameUIMan()->MessageBox("Game_Mark_Del", GetStringFromTable(509),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgMiniMap::OnCommand_deleteall(const char * szCommand)
{
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");

	if( pList->GetCount() > 0 )
	{
		GetGameUIMan()->MessageBox("Game_Mark_DelAll", GetStringFromTable(510),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgMiniMap::OnCommand_wiki(const char *szCommand)
{
	CDlgWiki* pDlg = GetGameUIMan()->m_pDlgWiki;
	pDlg->Show(!pDlg->IsShow());
	m_bActivityClicked = true;
}

void CDlgMiniMap::OnCommand_CANCEL(const char * szCommand)
{
	if (m_szName != "Win_Mark")
	{
		m_pAUIManager->RespawnMessage();
		return;
	}
	Show(false);
}

bool CDlgMiniMap::Render(void)
{
	if( m_szName == "Win_Map" )
		UpdateMiniMap();

	return CDlgBase::Render();
}

bool CDlgMiniMap::UpdateMiniMap()
{
	PAUIIMAGEPICTURE pObjMiniMap = (PAUIIMAGEPICTURE)GetDlgItem("Hide_2");
	pObjMiniMap->SetRenderCallback(NULL, (DWORD)this);

	if( GetGameUIMan()->GetMapDlgsMgr()->IsWorldMapShown() )
	{
		return true;
	}

	// 随机地图锁定缩放
	if(GetWorld()->IsRandomMap())
	{
		m_fZoom = 0.5f;
		if(GetDlgItem("Btn_-")->IsEnabled())
			GetDlgItem("Btn_-")->Enable(false);
		if(GetDlgItem("Btn_+")->IsEnabled())
			GetDlgItem("Btn_+")->Enable(false);
	}
	else
	{
		if(!GetDlgItem("Btn_-")->IsEnabled())
			GetDlgItem("Btn_-")->Enable(true);
		if(!GetDlgItem("Btn_+")->IsEnabled())
			GetDlgItem("Btn_+")->Enable(true);
	}

	// Set render callback.
	pObjMiniMap->SetRenderCallback(MiniMapRenderCallback, (DWORD)this);

	if( !pObjMiniMap->IsShow() ) return true;	// Mini map is minimized.

	ACString strText;

	A3DVECTOR3 vecPosHost = GetHostPlayer()->GetPos();
	strText.Format(GetStringFromTable(211), int(vecPosHost.x) / 10 + 400,
		int(vecPosHost.z) / 10 + 550, int(vecPosHost.y) / 10);
	GetDlgItem("Txt_xyz")->SetText(strText);

	float fRatio = 512.0f / 1024.0f / m_fZoom;
	float fNormal = fRatio;
	if( m_fZoom < 1.0f )
		fNormal = fRatio * m_fZoom;
	vecPosHost.x = float(floor(vecPosHost.x * fNormal) / fNormal);
	vecPosHost.z = float(floor(vecPosHost.z * fNormal) / fNormal);

	PAUILABEL pZone = (PAUILABEL)GetDlgItem("Txt_Address");
	CELPrecinct *pPrecinct = GetWorld()->GetCurPrecinct();
	const CECHostPlayer::BATTLEINFO &bInfo = GetHostPlayer()->GetBattleInfo();
	int idBattle = bInfo.idBattle;

	if(GetWorld()->IsRandomMap())
	{
		ACString str = _AL("");
		ACString col = _AL("^FFFFFF");
		
		CECRandomMapProcess* pRandomMap = GetGameRun()->GetRandomMapProc();
		ACString disName;
		if (pRandomMap && pRandomMap->GetRegionName(GetHostPlayer()->GetPos(),disName))		
			str = col + disName;	
		else
		{
			int idInst = GetWorld()->GetInstanceID();
			CECInstance * pInstance = GetGameRun()->GetInstance(idInst);
			str = col + pInstance->GetName();
		}
		
		pZone->SetText(str);
	}
	else if( idBattle )
	{
		pZone->SetText(_AL(""));
		if(bInfo.IsChariotWar())
		{
			int idInst = GetWorld()->GetInstanceID();
			CECInstance * pInstance = GetGameRun()->GetInstance(idInst);
			if (pInstance)
			{
				pZone->SetText(pInstance->GetName());
			}
		}
		else if (bInfo.IsGuildWar())
		{
			const DOMAIN_INFO *pInfo = CECDomainGuildInfo::Instance().Find(idBattle);
			if (pInfo)
			{
				ACString szText;
				szText.Format(GetStringFromTable(756), pInfo->name);
				pZone->SetText(szText);
			}
		}
		else
		{
			const DOMAIN2_INFO *pInfo = CECDomainCountryInfo::GetInstance()->Find(idBattle);
			if (pInfo)
			{
				ACString szText;
				szText.Format(GetStringFromTable(756), pInfo->name);
				pZone->SetText(szText);
			}
		}
	}
	else
	{
		ACString str = _AL("");
		ACString strHint = _AL("");
		if (pPrecinct)
		{
			ACString col = pPrecinct->IsPKProtect() ? _AL("^00FFFC") : _AL("^FFFFFF");
			str = col + pPrecinct->GetName();
			strHint = GetStringFromTable(pPrecinct->IsPKProtect() ? 9640:9639);
		}
		pZone->SetText(str);
		pZone->SetHint(strHint);
	}

	A3DRECT rcMiniMap = pObjMiniMap->GetRect();
	//ASSERT(rcMiniMap.Width() == rcMiniMap.Height());
	int W = rcMiniMap.Width();
	int H = rcMiniMap.Height();
	A3DPOINT2 C = rcMiniMap.CenterPoint();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	int idInstance = GetWorld()->GetInstanceID();
	CECInstance *pInstance = GetGameRun()->GetInstance(idInstance);
	int nRow = pInstance->GetRowNum();
	int nCol = pInstance->GetColNum();
	AString strSubPath = pInstance->GetPath();

	A3DPOINT2 ptHost, idxHost, ptThisCenter, idxCorner[MINIMAP_MAX];
	ARectI rcHost, rcThis, rcCell[MINIMAP_MAX], rcInter[MINIMAP_MAX];

	static int nSide = 256;
	static float fCell = 1024.0f;
	static float fSide = float(nSide);
	float fOffsetX = nCol * fCell / 2.0f;
	float fOffsetY = nRow * fCell / 2.0f;

	ptHost.x = int((vecPosHost.x + fOffsetX) * 512.f / 1024.0f);
	ptHost.y = int((fOffsetY - vecPosHost.z) * 512.f / 1024.0f);
	idxHost.x = ptHost.x / nSide;
	idxHost.y = ptHost.y / nSide;
	
	rcThis.left = idxHost.x * nSide;
	rcThis.top = idxHost.y * nSide;
	rcThis.right = rcThis.left + nSide;
	rcThis.bottom = rcThis.top + nSide;

	ptThisCenter = rcThis.CenterPoint();
	if( ptHost.y <= ptThisCenter.y )
	{
		if( ptHost.x <= ptThisCenter.x )	// Upper left.
		{
			idxCorner[MINIMAP_UL].x = idxHost.x - 1;
			idxCorner[MINIMAP_UL].y = idxHost.y - 1;
			idxCorner[MINIMAP_UR].x = idxHost.x;
			idxCorner[MINIMAP_UR].y = idxHost.y - 1;
			idxCorner[MINIMAP_LL].x = idxHost.x - 1;
			idxCorner[MINIMAP_LL].y = idxHost.y;
			idxCorner[MINIMAP_LR].x = idxHost.x;
			idxCorner[MINIMAP_LR].y = idxHost.y;
		}
		else								// Upper right.
		{
			idxCorner[MINIMAP_UL].x = idxHost.x;
			idxCorner[MINIMAP_UL].y = idxHost.y - 1;
			idxCorner[MINIMAP_UR].x = idxHost.x + 1;
			idxCorner[MINIMAP_UR].y = idxHost.y - 1;
			idxCorner[MINIMAP_LL].x = idxHost.x;
			idxCorner[MINIMAP_LL].y = idxHost.y;
			idxCorner[MINIMAP_LR].x = idxHost.x + 1;
			idxCorner[MINIMAP_LR].y = idxHost.y;
		}
	}
	else
	{
		if( ptHost.x <= ptThisCenter.x )	// Lower left.
		{
			idxCorner[MINIMAP_UL].x = idxHost.x - 1;
			idxCorner[MINIMAP_UL].y = idxHost.y;
			idxCorner[MINIMAP_UR].x = idxHost.x;
			idxCorner[MINIMAP_UR].y = idxHost.y;
			idxCorner[MINIMAP_LL].x = idxHost.x - 1;
			idxCorner[MINIMAP_LL].y = idxHost.y + 1;
			idxCorner[MINIMAP_LR].x = idxHost.x;
			idxCorner[MINIMAP_LR].y = idxHost.y + 1;
		}
		else								// Lower right.
		{
			idxCorner[MINIMAP_UL].x = idxHost.x;
			idxCorner[MINIMAP_UL].y = idxHost.y;
			idxCorner[MINIMAP_UR].x = idxHost.x + 1;
			idxCorner[MINIMAP_UR].y = idxHost.y;
			idxCorner[MINIMAP_LL].x = idxHost.x;
			idxCorner[MINIMAP_LL].y = idxHost.y + 1;
			idxCorner[MINIMAP_LR].x = idxHost.x + 1;
			idxCorner[MINIMAP_LR].y = idxHost.y + 1;
		}
	}

	rcHost.left = ptHost.x - int(W * m_fZoom) / 2;
	rcHost.top = ptHost.y - int(H * m_fZoom) / 2;
	rcHost.right = ptHost.x + int(W * m_fZoom) / 2;
	rcHost.bottom = ptHost.y + int(H * m_fZoom) / 2;

	int i, j;
	bool bval;
	char szIndex[20];
	AString strIndex;
	bool bLoadNewMap = false;
	for( i = MINIMAP_UL; i < MINIMAP_MAX; i++ )
	{
		rcCell[i].left = idxCorner[i].x * nSide;
		rcCell[i].top = idxCorner[i].y * nSide;
		rcCell[i].right = rcCell[i].left + nSide;
		rcCell[i].bottom = rcCell[i].top + nSide;

		rcInter[i] = rcCell[i] & rcHost;
		if( rcInter[i].IsEmpty() )
			rcInter[i].Offset(rcCell[i].left, rcCell[i].top);

		sprintf(szIndex, "%02d%02d", idxCorner[i].y, idxCorner[i].x);
		strIndex = szIndex;
		if( !m_TexMap[strIndex] )
		{
			A3DTexture *pA3DTex = m_pA3DRadarBack;

			if( idxCorner[i].x >= 0 && idxCorner[i].x < nCol * 2 &&
				idxCorner[i].y >= 0 && idxCorner[i].y < nRow * 2 )
			{
				pA3DTex = new A3DTexture;
				AString strFile;

				if(GetWorld()->IsRandomMap())
				{
					strFile = "temp\\Surfaces\\MiniMaps\\" + GetGameRun()->GetRandomMapProc()->GetMapName() + "\\" + strIndex + ".dds";
					pA3DTex->SetNoDownSample(true);
					unsigned char* pData = NULL;
					int iLen = 0;
					if(GetGameRun()->GetRandomMapProc()->EncodeMap(strFile,pData,iLen))
					{
						bool bval = pA3DTex->LoadFromMemory(m_pA3DDevice,pData,iLen,256,256,A3DFMT_UNKNOWN);
						ASSERT(bval);
						delete [] pData;
					}
				}
				else 
				{
					strFile = "Surfaces\\MiniMaps\\" + strSubPath + "\\" + strIndex + ".dds";

					pA3DTex->SetNoDownSample(true);
					pA3DTex->LoadFromFile(m_pA3DDevice, strFile, strFile);
				}
			}
			
			m_TexMap[strIndex] = pA3DTex;
			bLoadNewMap = true;
		}
	}

	if( bLoadNewMap )
	{
		bool bNoUse;
		abase::hash_map<AString, A3DTexture *>::iterator it;
		for( it = m_TexMap.begin(); it != m_TexMap.end(); )
		{
			if(it->second == m_pA3DRadarBack)
			{
				++it;
				continue;
			}

			bNoUse = true;
			for( i = MINIMAP_UL; i < MINIMAP_MAX; i++ )
			{
				sprintf(szIndex, "%02d%02d", idxCorner[i].y, idxCorner[i].x);
				strIndex = szIndex;
				if( 0 == strcmp(strIndex, it->first) )
				{
					bNoUse = false;
					break;
				}
			}
			if( bNoUse )
			{
				A3DRELEASE(it->second);
				it = m_TexMap.erase(it);
			}
			else
				++it;
		}
	}

	m_vertexMiniMap[0][0].x = -W / 2.0f;
	m_vertexMiniMap[0][0].y = -H / 2.0f;
	m_vertexMiniMap[0][1].x = -W / 2.0f + rcInter[MINIMAP_UL].Width() / m_fZoom;
	m_vertexMiniMap[0][1].y = -H / 2.0f;
	m_vertexMiniMap[0][2].x = -W / 2.0f;
	m_vertexMiniMap[0][2].y = -H / 2.0f + rcInter[MINIMAP_UL].Height() / m_fZoom;
	m_vertexMiniMap[0][3].x = -W / 2.0f + rcInter[MINIMAP_UL].Width() / m_fZoom;
	m_vertexMiniMap[0][3].y = -H / 2.0f + rcInter[MINIMAP_UL].Height() / m_fZoom;

	m_vertexMiniMap[1][0].x = +W / 2.0f - rcInter[MINIMAP_UR].Width() / m_fZoom;
	m_vertexMiniMap[1][0].y = -H / 2.0f;
	m_vertexMiniMap[1][1].x = +W / 2.0f;
	m_vertexMiniMap[1][1].y = -H / 2.0f;
	m_vertexMiniMap[1][2].x = +W / 2.0f - rcInter[MINIMAP_UR].Width() / m_fZoom;
	m_vertexMiniMap[1][2].y = -H / 2.0f + rcInter[MINIMAP_UR].Height() / m_fZoom;
	m_vertexMiniMap[1][3].x = +W / 2.0f;
	m_vertexMiniMap[1][3].y = -H / 2.0f + rcInter[MINIMAP_UR].Height() / m_fZoom;

	m_vertexMiniMap[2][0].x = -W / 2.0f;
	m_vertexMiniMap[2][0].y = +H / 2.0f - rcInter[MINIMAP_LL].Height() / m_fZoom;
	m_vertexMiniMap[2][1].x = -W / 2.0f + rcInter[MINIMAP_LL].Width() / m_fZoom;
	m_vertexMiniMap[2][1].y = +H / 2.0f - rcInter[MINIMAP_LL].Height() / m_fZoom;
	m_vertexMiniMap[2][2].x = -W / 2.0f;
	m_vertexMiniMap[2][2].y = +H / 2.0f;
	m_vertexMiniMap[2][3].x = -W / 2.0f + rcInter[MINIMAP_LL].Width() / m_fZoom;
	m_vertexMiniMap[2][3].y = +H / 2.0f;

	m_vertexMiniMap[3][0].x = +W / 2.0f - rcInter[MINIMAP_LR].Width() / m_fZoom;
	m_vertexMiniMap[3][0].y = +H / 2.0f - rcInter[MINIMAP_LR].Height() / m_fZoom;
	m_vertexMiniMap[3][1].x = +W / 2.0f;
	m_vertexMiniMap[3][1].y = +H / 2.0f - rcInter[MINIMAP_LR].Height() / m_fZoom;
	m_vertexMiniMap[3][2].x = +W / 2.0f - rcInter[MINIMAP_LR].Width() / m_fZoom;
	m_vertexMiniMap[3][2].y = +H / 2.0f;
	m_vertexMiniMap[3][3].x = +W / 2.0f;
	m_vertexMiniMap[3][3].y = +H / 2.0f;

	A3DMATRIX4 m;
	A3DCamera *pA3DCamera = g_pGame->GetViewport()->GetA3DCamera();
	if( MODE_A == m_nMode )
	{
		A3DVECTOR3 vecDir = pA3DCamera->GetDirH();
		m = TransformMatrix(g_vAxisZ,
			A3DVECTOR3(vecDir.x, vecDir.z, 0.0f),
			A3DVECTOR3(float(C.x), float(C.y), 0.0f));
	}

	if (MODE_A == m_nMode) {
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetDeviceVertexShader(FVF_MAPTLVERTEX);
		m_pA3DDevice->ClearPixelShader();
		m_pA3DDevice->ClearTexture(0);
		m_pA3DTexMask->Appear(1);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_SELECTARG2);
		m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG1);
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	}

	A3DTexture *pA3DTex;
	static WORD a_wIndex[6] = { 0, 1, 2, 2, 1, 3 };
	A3DVECTOR3 vecPos = A3DVECTOR3(0.0f, 0.0f, 0.0f);
	for( i = 0; i < MINIMAP_MAX; i++ )
	{
		sprintf(szIndex, "%02d%02d", idxCorner[i].y, idxCorner[i].x);
		strIndex = szIndex;
		pA3DTex = m_TexMap[strIndex];
		pA3DTex->Appear(0);

		m_vertexMiniMap[i][0].tu = float(rcInter[i].left - rcCell[i].left) / fSide;
		m_vertexMiniMap[i][0].tv = float(rcInter[i].top - rcCell[i].top) / fSide;
		m_vertexMiniMap[i][1].tu = float(rcInter[i].right - rcCell[i].left) / fSide;
		m_vertexMiniMap[i][1].tv = float(rcInter[i].top - rcCell[i].top) / fSide;
		m_vertexMiniMap[i][2].tu = float(rcInter[i].left - rcCell[i].left) / fSide;
		m_vertexMiniMap[i][2].tv = float(rcInter[i].bottom - rcCell[i].top) / fSide;
		m_vertexMiniMap[i][3].tu = float(rcInter[i].right - rcCell[i].left) / fSide;
		m_vertexMiniMap[i][3].tv = float(rcInter[i].bottom - rcCell[i].top) / fSide;

		for( j = 0; j < 4; j++ )
		{
			m_vertexMiniMap[i][j].tu2 = float(m_vertexMiniMap[i][j].x
				+ C.x - rcMiniMap.left) / float(W);
			m_vertexMiniMap[i][j].tv2 = float(m_vertexMiniMap[i][j].y
				+ C.y - rcMiniMap.top) / float(H);

			if( MODE_A == m_nMode )
			{
				vecPos.x = m_vertexMiniMap[i][j].x;
				vecPos.y = m_vertexMiniMap[i][j].y;
				vecPos = vecPos * m;
				m_vertexMiniMap[i][j].x = vecPos.x + p->X;
				m_vertexMiniMap[i][j].y = vecPos.y + p->Y;
			}
			else
			{
				m_vertexMiniMap[i][j].x += C.x + p->X;
				m_vertexMiniMap[i][j].y += C.y + p->Y;
			}
			m_vertexMiniMap[i][j].x -= 0.5f;
			m_vertexMiniMap[i][j].y -= 0.5f;
		}

		MAPTLVERTEX * pVerts;
		if( !m_pMiniMapStream->LockVertexBuffer(0, 0, (BYTE**) &pVerts, 0) )
			return false;

		pVerts[0] = m_vertexMiniMap[i][0];
		pVerts[1] = m_vertexMiniMap[i][1];
		pVerts[2] = m_vertexMiniMap[i][2];

		pVerts[3] = m_vertexMiniMap[i][2];
		pVerts[4] = m_vertexMiniMap[i][1];
		pVerts[5] = m_vertexMiniMap[i][3];

		m_pMiniMapStream->UnlockVertexBuffer();
		m_pMiniMapStream->Appear();

		bval = m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		pA3DTex->Disappear(0);
	}
	
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	
	m_pA3DTexMask->Disappear(1);
	m_pA3DDevice->SetLightingEnable(true);

	A3DVECTOR3 vecDir = GetHostPlayer()->GetDir();
	int nDegree = 90 - (int)RAD2DEG((float)atan2(vecDir.z, vecDir.x));

	if( MODE_A == m_nMode )
	{
		m_pA2DSpriteCompass->SetDegree(270 - (int)pA3DCamera->GetDeg());
		m_pA2DSpriteHostDir->SetDegree(270 - (int)pA3DCamera->GetDeg() + nDegree);
	}
	else
	{
		m_pA2DSpriteCompass->SetDegree(0);
		m_pA2DSpriteHostDir->SetDegree(nDegree);
	}

	bval = m_pA2DSpriteCompass->DrawToInternalBuffer((float)(p->X + C.x - 1), (float)(p->Y + C.y - 1));
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	int nTimeItem;
	float fDNFactor = GetWorld()->GetSunMoon()->GetDNFactor();
	float fDNFactorDest = GetWorld()->GetSunMoon()->GetDNFactorDest();
	int nTimeIndex = int(12.0f * GetWorld()->GetSunMoon()->GetTimeOfTheDay() + 0.5f) % 12;

	if( fDNFactor == 0.0f )
		nTimeItem = TIME_DAY;
	else if( fDNFactor == 1.0f )
		nTimeItem = TIME_NIGHT;
	else
	{
		if( fDNFactorDest == 1.0f )
			nTimeItem = TIME_DUSK;
		else
			nTimeItem = TIME_MORNING;
	}

	m_pImg_SystemTime->FixFrame(nTimeItem);
	strText.Format(GetStringFromTable(604), GetStringFromTable(1330 + nTimeIndex),
		int(GetWorld()->GetSunMoon()->GetTimeOfTheDay() * 24.0f));
	m_pImg_SystemTime->SetHint(strText);

	CECHostPlayer::BATTLEINFO binfo = GetHostPlayer()->GetBattleInfo();
	CECHostPlayer *pHost = GetHostPlayer();
	if( binfo.idBattle && pHost->GetBattleTankNum() > 0 )
	{
		PAUILABEL pLabel = (PAUILABEL)GetDlgItem("Txt_TankDistance");
		int j, k;
		for(j = 0; j < pHost->GetBattleTankNum(); j++)
		{
			A3DRECT rect;
			rect.left = pLabel->GetPos().x - j * 20 - 18;
			rect.top = pLabel->GetPos().y + 2;
			rect.right = rect.left + 16;
			rect.bottom = rect.top + 13;
			CECNPCMan *pNPCMan = GetWorld()->GetNPCMan();
			const APtrArray<CECNPC *> &NPCs = pNPCMan->GetNPCsInMiniMap();
			A3DCOLOR cl = A3DCOLORRGB(255, 0, 0);
			int MaxDistance = 40;
			for(k = 0; k < NPCs.GetSize(); k++ )
				if( NPCs[k]->GetNPCID() == pHost->GetBattleTank(j) )
				{
					double dis = Distance(NPCs[k]->GetServerPos(), pHost->GetPos());
					if( dis < 15 )
						cl = A3DCOLORRGB(0, 255, 0);
					else if( dis < 30 )
						cl = A3DCOLORRGB(255, 255, 0);
					else if( dis < 40 )
						cl = A3DCOLORRGB(255, 128, 0);
				}
			g_pA3DGDI->Draw2DRectangle(rect, cl);
		}
	}

	return true;
}

struct ConverToLocalPos
{
	A3DVECTOR3	vecPosHost;
	bool		bModeA;
	float		fRatio;
	A3DMATRIX4	mr;
	float		fViewRadius;
	ConverToLocalPos(const A3DVECTOR3 &vHost, bool bMode, float ratio, const A3DMATRIX4& matRotateView, float fRadius)
		: vecPosHost(vHost), bModeA(bMode), fRatio(ratio), mr(matRotateView), fViewRadius(fRadius)
	{
	}
	A3DVECTOR3 operator()(A3DVECTOR3 pos)const{
		pos.x -= vecPosHost.x;
		pos.z -= vecPosHost.z;
		if (bModeA){
			pos = pos * mr;
		}
		pos.x *= fRatio;
		pos.z *= fRatio;
		return pos;
	}
	bool CanShow(const A3DVECTOR3 &pos)const{
		return SQUARE(pos.x) + SQUARE(pos.z) < fViewRadius;
	}
};

void CDlgMiniMap::MiniMapRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3)
{
	CDlgMiniMap *pDlgMap = (CDlgMiniMap *)param1;
	PAUIOBJECT pObjMiniMap = pDlgMap->GetDlgItem("Hide_2");
	if( !pObjMiniMap->IsShow() ) return;	// Mini map is minimized.

	A3DRECT rcMiniMap = pObjMiniMap->GetRect();
	//ASSERT(rcMiniMap.Width() == rcMiniMap.Height());
	int W = rcMiniMap.Width();
	int H = rcMiniMap.Height();
	A3DPOINT2 C = rcMiniMap.CenterPoint();

	int i;
	A3DCOLOR color;
	A3DPOINT2 ptPos;
	A3DVECTOR3 vecPos;
	CECHostPlayer *pHost = pDlgMap->GetHostPlayer();
	A3DVECTOR3 vecPosHost = pHost->GetPos();
	int idTarget = pHost->GetSelectedTarget();
	bool bTarget = false;
	A3DPOINT2 ptTarget;
	float fRatio = 512.0f / 1024.0f / pDlgMap->m_fZoom;
	float fNormal = fRatio;
	if( m_fZoom < 1.0f )
		fNormal = fRatio * m_fZoom;
	vecPosHost.x = float(floor(vecPosHost.x * fNormal) / fNormal);
	vecPosHost.z = float(floor(vecPosHost.z * fNormal) / fNormal);
	float fRadius = (float)SQUARE(min(W, H) / 2 - 2);
	A3DCamera *pA3DCamera = g_pGame->GetViewport()->GetA3DCamera();
	A3DMATRIX4 mr = RotateY(DEG2RAD(-(int)pA3DCamera->GetDeg() - 90));
	A3DVIEWPORTPARAM *p = pDlgMap->m_pA3DEngine->GetActiveViewport()->GetParam();
	A3DFlatCollector *pFC = pDlgMap->m_pA3DEngine->GetA3DFlatCollector();

	MARK Mark;
	CECNPCServer *pServer;
	CECNPCMan *pNPCMan = pDlgMap->GetWorld()->GetNPCMan();
	const APtrArray<CECNPC *> &NPCs = pNPCMan->GetNPCsInMiniMap();
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();

	if (CECUIHelper::FactionPVPCanShowResourceInMap()){		
		ConverToLocalPos conv(vecPosHost, MODE_A == pDlgMap->m_nMode, fRatio, mr, fRadius);
		RenderFactionPVPIcon(&conv, p->X+C.x, p->Y+C.y);
	}

	if( pHost->IsInBattle() )
	{
		m_pA2DSpriteHostDir->SetCurrentItem(m_nHostDirFrames);
		m_pA2DSpriteHostDir->DrawToBack((float)(p->X + C.x - 1), (float)(p->Y + C.y - 1));
	}

	if( vs.bMapPlayer )
	{
		CECFriendMan::GROUP *pGroup;
		CECFriendMan::FRIEND *pFriend;
		CECTeam *pTeam = pHost->GetTeam();
		CECFriendMan *pMan = pHost->GetFriendMan();
		CECPlayerMan *pPlayerMan = pDlgMap->GetWorld()->GetPlayerMan();
		CECFactionMan* pFMan = g_pGame->GetFactionMan();
		const APtrArray<CECElsePlayer *> &Players = pPlayerMan->GetPlayersInMiniMap();

		for( i = 0; i < Players.GetSize(); i++ )
		{
			if( pTeam && pTeam->GetMemberByID(Players[i]->GetCharacterID()) )
				continue;

			vecPos = (Players[i]->GetPos() - vecPosHost);
			if( SQUARE(vecPos.x * fRatio) + SQUARE(vecPos.z * fRatio) >= fRadius )
				continue;

			if( MODE_A == pDlgMap->m_nMode )
				vecPos = vecPos * mr;
			ptPos.x = int(p->X + C.x + vecPos.x * fRatio);
			ptPos.y = int(p->Y + C.y - vecPos.z * fRatio);
			
			if( Players[i]->GetCharacterID() == idTarget )
			{
				bTarget = true;
				ptTarget = ptPos;
			}
			else
			{
				if( Players[i]->IsInBattle() )
				{
					if( Players[i]->GetBattleCamp() == GP_BATTLE_CAMP_INVADER )
						color = A3DCOLORRGB(255, 0, 0);
					else
 						color = A3DCOLORRGB(0, 0, 255);
				}
				else
				{
					if( pHost->IsFactionAllianceMember(Players[i]->GetFactionID()) )
						color = NAMECOL_ALLIANCE;
					else if( pFMan->IsFactionHostile(Players[i]->GetFactionID()) )
						color = NAMECOL_HOSTILE;
					else if( (pFriend = pMan->GetFriendByID(Players[i]->GetCharacterID())) )
					{
						pGroup = pMan->GetGroupByID(pFriend->iGroup);
						if( pGroup )
						{
							color = pGroup->color;
							pFC->AddRect_2D(ptPos.x - 1, ptPos.y - 1, ptPos.x + 2, ptPos.y + 2, color);
							continue;
						}
					}
					else
						color = A3DCOLORRGB(0, 255, 0);
				}

				if( !Players[i]->IsInBattle() ||
					Players[i]->GetBattleCamp() == pHost->GetBattleCamp() )
					pFC->AddRect_2D(ptPos.x - 1, ptPos.y - 1, ptPos.x + 1, ptPos.y + 1, color);
			}
		}
	}

	pFC->Flush();
	if( !pHost->IsInBattle() )
	{
		m_pA2DSpriteHostDir->SetCurrentItem(m_nHostDirFrames);
		m_pA2DSpriteHostDir->DrawToBack((float)(p->X + C.x - 1), (float)(p->Y + C.y - 1));
	}
	if (CECIntelligentRoute::Instance().IsMoveOn()){		
		int hostX = p->X + C.x;
		int hostY = p->Y + C.y;
		int nCount = CECIntelligentRoute::Instance().GetLeftNodeCount();
		ConverToLocalPos conv(vecPosHost, MODE_A == pDlgMap->m_nMode, fRatio, mr, fRadius);
		//	计算显示时移动步长，用于显示时处理的节点数，长路径时提高效率
		int nTestCount = a_Min(20, nCount);
		int i(0);
		A3DVECTOR3 lastPos(0.0f);
		double sumPixelLength(0.0);
		for (i = 0; i < nTestCount; ++ i)
		{
			A3DVECTOR3 pos = CECIntelligentRoute::Instance().GetLeftNodePosXZ(i);
			pos = conv(pos);
			if (i == 0){
				lastPos = pos;
				continue;
			}
			sumPixelLength += (pos-lastPos).MagnitudeH();
			lastPos = pos;
		}
		int nStep = (int)6/(sumPixelLength/(nTestCount-1));		//	6 个象素长 / 每步长平均象素 = 应走过步长
		nStep = a_Max(nStep, 1);
		//	按相同步长显示节点
		for (i = 0; i < nCount; i+= nStep)
		{
			A3DVECTOR3 pos = CECIntelligentRoute::Instance().GetLeftNodePosXZ(i);
			pos = conv(pos);
			if (MODE_A == m_nMode && SQUARE(pos.x) + SQUARE(pos.z) > fRadius){
				continue;
			}
			if (MODE_B == m_nMode && (SQUARE(pos.x) > fRadius || SQUARE(pos.z) > fRadius)) {
				continue;
			}
			pDlgMap->DrawPathNode(hostX + (int)pos.x, hostY - (int)pos.z);
		}
	}
	if (pHost->IsAutoMoving()){
		int hostX = p->X + C.x;
		int hostY = p->Y + C.y;
		if (CECIntelligentRoute::Instance().IsIdle()){
			CECGameUIMan* pMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			A3DPOINT2 TargetPos = pMan->AutoMoveTarget();
			vecPos.x = TargetPos.x - vecPosHost.x;
			vecPos.z = TargetPos.y - vecPosHost.z;
			if ( MODE_A == pDlgMap->m_nMode )
				vecPos = vecPos * mr;
			int targetX = hostX+(int)(vecPos.x*fRatio);
			int targetY = hostY-(int)(vecPos.z*fRatio);			
			pDlgMap->DrawPathFromCenter(hostX, hostY, targetX, targetY, 6, (float)sqrt(fRadius));
		}
	}

	pDlgMap->m_vecNPCMark.clear();
	for( i = 0; i < NPCs.GetSize(); i++ )
	{
		vecPos = (NPCs[i]->GetPos() - vecPosHost);
		if( NPCs[i]->GetMasterID() == pHost->GetCharacterID() )
		{
			if( SQUARE(vecPos.x * fRatio) + SQUARE(vecPos.z * fRatio) >= fRadius )
			{
				vecPos.Normalize();
				vecPos *= (float)sqrt(fRadius) / fRatio;
			}
			if( MODE_A == pDlgMap->m_nMode )
				vecPos = vecPos * mr;
			ptPos.Set(int(p->X + C.x + vecPos.x * fRatio),
				int(p->Y + C.y - vecPos.z * fRatio));
			m_pA2DSpritePet->DrawToInternalBuffer(ptPos.x, ptPos.y);
			continue;
		}

		if( (NPCs[i]->IsServerNPC() && !vs.bMapNPC) ||
			(!NPCs[i]->IsServerNPC() && !vs.bMapMonster) )
		{
			continue;
		}

		if( SQUARE(vecPos.x * fRatio) + SQUARE(vecPos.z * fRatio) < fRadius )
		{
			if( MODE_A == pDlgMap->m_nMode )
				vecPos = vecPos * mr;
			ptPos.Set(int(p->X + C.x + vecPos.x * fRatio),
				int(p->Y + C.y - vecPos.z * fRatio));

			if( NPCs[i]->GetNPCID() == idTarget )
			{
				bTarget = true;
				ptTarget = ptPos;
			}
			else
			{
				if( NPCs[i]->IsServerNPC() )
				{
					A2DSprite *pSprite;
					pServer = (CECNPCServer *)NPCs[i];
					int quest_icon = pServer->GetCurQuestIcon();
					if (quest_icon >= 0 && quest_icon != 3) {
						int remainder = quest_icon % 2;
						pSprite = (1 == remainder) ? pDlgMap->m_pA2DRadar_FTaskNPC : pDlgMap->m_pA2DRadar_TaskNPC;
					} else pSprite = pSprite = pDlgMap->m_pA2DRadar_NPC;
					pSprite->DrawToInternalBuffer(ptPos.x, ptPos.y);
				}
				else
				{
					color = A3DCOLORRGB(207, 207, 207);
					pFC->AddRect_2D(ptPos.x - 1, ptPos.y - 1,
						ptPos.x + 1, ptPos.y + 1, color);
				}
			}

			if( NPCs[i]->IsServerNPC() )
			{
				Mark.strName = NPCs[i]->GetName();
				Mark.vecPos.x = float(ptPos.x - p->X);
				Mark.vecPos.y = float(ptPos.y - p->Y);
				pDlgMap->m_vecNPCMark.push_back(Mark);
			}
		}
	}
	pDlgMap->m_vecTeamMate.clear();

	if( vs.bMapPlayer )
	{
		CECTeamMember *pMember;
		CECTeam *pTeam = pHost->GetTeam();
		CECFriendMan *pMan = pHost->GetFriendMan();
		for( i = 0; pTeam && i < pTeam->GetMemberNum(); i++ )
		{
			pMember = pTeam->GetMemberByIndex(i);
			if (!pMember->IsSameInstanceTeamMember())
				continue;

			vecPos = (pMember->GetPos() - vecPosHost);
			if( SQUARE(vecPos.x * fRatio) + SQUARE(vecPos.z * fRatio) >= fRadius )
			{
				vecPos.Normalize();
				vecPos *= (float)sqrt(fRadius) / fRatio;
			}

			if( MODE_A == pDlgMap->m_nMode )
				vecPos = vecPos * mr;
			ptPos.x = int(p->X + C.x + vecPos.x * fRatio);
			ptPos.y = int(p->Y + C.y - vecPos.z * fRatio);

			if( pMember->GetCharacterID() == pTeam->GetLeaderID() )
				m_pA2DMapMark->SetCurrentItem(MAPMARK_CAPTAIN);
			else
				m_pA2DMapMark->SetCurrentItem(MAPMARK_TEAMMATE);
			m_pA2DMapMark->DrawToInternalBuffer(ptPos.x, ptPos.y);

			Mark.strName = pMember->GetName();
			Mark.vecPos.x = float(ptPos.x - p->X);
			Mark.vecPos.y = float(ptPos.y - p->Y);
			pDlgMap->m_vecTeamMate.push_back(Mark);
		}
	}
	pFC->Flush();

	if( bTarget )
	{
		m_pA2DMapMark->SetCurrentItem(MAPMARK_TARGET);
		m_pA2DMapMark->DrawToInternalBuffer(ptTarget.x, ptTarget.y);
	}

}

void CDlgMiniMap::OnEventMouseMove_Hide_2(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int i;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	A3DRECT rcMiniMap = pObj->GetRect();
	A3DPOINT2 C = rcMiniMap.CenterPoint();
	float fRatio = 512.0f / 1024.0f / m_fZoom;
	A3DCamera *pA3DCamera = g_pGame->GetViewport()->GetA3DCamera();
	A3DMATRIX4 mr = InverseTM(RotateY(DEG2RAD(-(int)pA3DCamera->GetDeg() - 90)));
	A3DVECTOR3 vecHost = GetHostPlayer()->GetPos();

	if( SQUARE(x - C.x) + SQUARE(y - C.y) < SQUARE(rcMiniMap.Width() / 2) )
	{
		m_vecNowTarget.x = (x - C.x) / fRatio;
		m_vecNowTarget.z = (C.y - y) / fRatio;
		if( CDlgMiniMap::MODE_A == m_nMode )
			m_vecNowTarget = m_vecNowTarget * mr;
		m_vecNowTarget += vecHost;
	}

	pObj->SetHint(_AL(""));

	for( i = 0; i < (int)m_vecTeamMate.size(); i++ )
	{
		if( abs(x - (int)m_vecTeamMate[i].vecPos.x) <= 2 &&
			abs(y - (int)m_vecTeamMate[i].vecPos.y) <= 2 )
		{
			pObj->SetHint(m_vecTeamMate[i].strName);
			return;
		}
	}

	for( i = 0; i < (int)m_vecNPCMark.size(); i++ )
	{
		if( abs(x - (int)m_vecNPCMark[i].vecPos.x) <= 2 &&
			abs(y - (int)m_vecNPCMark[i].vecPos.y) <= 2 )
		{
			pObj->SetHint(m_vecNPCMark[i].strName);
			return;
		}
	}
}

void CDlgMiniMap::OnEventLButtonUp_Hide_2(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	A3DRECT rcMiniMap = pObj->GetRect();
	A3DPOINT2 C = rcMiniMap.CenterPoint();
	float fRatio = 512.0f / 1024.0f / m_fZoom;
		
	if( SQUARE(x - C.x) + SQUARE(y - C.y) < SQUARE(rcMiniMap.Width() / 2) || m_nMode ==  MODE_B)
	{
		A3DVECTOR3 vecTempTarget(0.0f);
		vecTempTarget.x = (x - C.x) / fRatio;
		vecTempTarget.z = (C.y - y) / fRatio;

		if( CDlgMiniMap::MODE_A == m_nMode )
		{
			// rotate the target because mini map is rotated
			A3DCamera *pA3DCamera = g_pGame->GetViewport()->GetA3DCamera();
			A3DMATRIX4 mr = InverseTM(RotateY(DEG2RAD(-(int)pA3DCamera->GetDeg() - 90)));
			vecTempTarget = vecTempTarget * mr;
		}

		A3DVECTOR3 vecHost = GetHostPlayer()->GetPos();
		vecTempTarget += vecHost;
		CECUIHelper::AutoMoveStartComplex(vecTempTarget);
	}
}

void CDlgMiniMap::ChangeWorldInstance(int idInstance)
{	
	if (GetGameUIMan()->m_pDlgGMapChallenge->IsShow())
		GetGameUIMan()->m_pDlgGMapChallenge->OnCommandCancel("");
	if (GetGameUIMan()->m_pDlgGuildMap->IsShow())
		GetGameUIMan()->m_pDlgGuildMap->OnCommandCancel("IDCANCEL");
	GetGameUIMan()->GetMapDlgsMgr()->ShowWorldMap(false);
	
	// refresh the player mark list
	PAUILISTBOX pList = (PAUILISTBOX)GetGameUIMan()->m_pDlgMiniMapMark->GetDlgItem("List_Choose");
	ASSERT(m_vecMark.size() == pList->GetCount());
	for(int i=0;i<pList->GetCount() && i < (int)m_vecMark.size();i++)
	{
		pList->SetItemTextColor(i, (m_vecMark[i].mapID != idInstance) ? 
			A3DCOLORRGB(128, 128, 128) : A3DCOLORRGB(255, 255, 255) );
		pList->SetItemHint(i, GetMarkDesc(m_vecMark[i], idInstance));
	}

	abase::hash_map<AString, A3DTexture *>::iterator it;
	for( it = m_TexMap.begin(); it != m_TexMap.end(); ++it )
	{
		if(it->second == m_pA3DRadarBack) continue;
		A3DRELEASE(it->second);
	}
	m_TexMap.clear();

	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_Closeall");
	bool bShow = !pCheck->IsChecked();
	if (bShow) CheckTouchButton();
}

void CDlgMiniMap::CheckTouchButton()
{
	PAUIOBJECT pObj = GetDlgItem("btn_touch");
	if (pObj){
		bool bShow(false);
		if (CECUIConfig::Instance().GetGameUI().bEnableTouch &&
			!CECCrossServer::Instance().IsOnSpecialServer()){
			int idInstance = GetWorld()->GetInstanceID();
			if (CECUIConfig::Instance().GetGameUI().GetCanShowTouchShop(idInstance)){
				bShow = true;
			}
		}
		pObj->Show(bShow);
		if (!bShow && GetGameUIMan()->m_pDlgTouchShop->IsShow()){
			GetGameUIMan()->m_pDlgTouchShop->Show(false);
		}
	}	
}

void CDlgMiniMap::SetNewMail(int nRemainTime)
{
	if( nRemainTime == 0 )
	{
		m_pChk_Mail->SetData(0);
		m_pChk_Mail->Check(true);
		m_pChk_Mail->SetHint(GetStringFromTable(723));
	}
	else if( nRemainTime > 0 )
	{
		m_pChk_Mail->SetData(nRemainTime + GetGame()->GetServerGMTTime() + 10);
		m_pChk_Mail->Check(false);
		m_pChk_Mail->SetHint(_AL(""));
	}
	else
	{
		m_pChk_Mail->SetData(0);
		m_pChk_Mail->Check(false);
		m_pChk_Mail->SetHint(_AL(""));
	}
}

void CDlgMiniMap::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	if (stricmp(GetName(), "Win_Map"))
	{
		float fScale = GetGameUIMan()->GetWindowScale();

		A2DSprite *pSprites[] = 
		{			
			m_pA2DSpriteHostDir,
				m_pA2DSpriteCompass,
				m_pA2DMapMark,
				m_pA2DRadar_NPC,
				m_pA2DRadar_TaskNPC,
				m_pA2DRadar_FTaskNPC,
				m_pA2DSpritePet,
				m_pA2DSpriteAutoMove,
		};

		int nSprite = sizeof(pSprites)/sizeof(pSprites[0]);
		for (int i = 0; i < nSprite; ++ i)
		{
			A2DSprite * p = pSprites[i];
			if (p)
			{
				p->SetScaleX(fScale);
				p->SetScaleY(fScale);
			}
		}
	}
	if (stricmp(GetName(), "Win_HideMap")==0)
	{
		SetPosEx(0,0,AUIDialog::alignMax,AUIDialog::alignMin,NULL);
	}
}

void CDlgMiniMap::OnCommand_Radio(const char *szCommand)
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_WebList");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}

void CDlgMiniMap::SetMarkName(int nIndex, const ACString& strName)
{
	m_vecMark[nIndex].strName = strName;
}

void CDlgMiniMap::SetMarkPos(int nIndex, const A3DVECTOR3& vec)
{
	if(nIndex < m_vecMark.size() && m_vecMark[nIndex].nNPC < 0)
	{
		m_vecMark[nIndex].vecPos = vec;
	}
}

void CDlgMiniMap::AddMark(const MARK& mark)
{
	m_vecMark.push_back(mark);
}

void CDlgMiniMap::ClearMark(int nIndex)
{
	if(nIndex >= 0)
	{
		m_vecMark.erase(m_vecMark.begin() + nIndex);
	}
	else
	{
		m_vecMark.clear();
	}
}

bool CDlgMiniMap::DrawHostMark(int nDegree, const A3DPOINT2& ptPos)
{
	m_pA2DSpriteHostDir->SetDegree(nDegree);
	return m_pA2DSpriteHostDir->DrawToBack((float)ptPos.x, (float)ptPos.y);
}

void CDlgMiniMap::DrawPath(int FromX, int FromY, int ToX, int ToY, int Step)
{
	double dist =  sqrt((double)((SQUARE(ToX - FromX) + SQUARE(ToY - FromY))));
	int num = int(dist/Step + 1);
	double DeltaX = (ToX - FromX)*Step/dist;
	double DeltaY = (ToY - FromY)*Step/dist;
	for (int i = 0; i < num; i++)
	{
		m_pA2DSpriteAutoMove->DrawToBack(int(ToX - i*DeltaX), int(ToY - i*DeltaY));
	}
}

void CDlgMiniMap::DrawPathFromCenter(int CenterX, int CenterY, int ToX, int ToY, int Step, float fMaxRadius)
{
	double dist =  sqrt((double)((SQUARE(ToX - CenterX) + SQUARE(ToY - CenterY))));
	int num = int(dist/Step + 1);
	double DeltaX = (ToX - CenterX)*Step/dist;
	double DeltaY = (ToY - CenterY)*Step/dist;
	for (int i = 0; i < num; i++)
	{
		if (MODE_A == m_nMode && i*Step >= fMaxRadius){
			break;
		} else if (MODE_B == m_nMode) {
			if (abs(i * DeltaX) >= fMaxRadius || abs(i * DeltaY) >= fMaxRadius) {
				break;
			}
		}
		m_pA2DSpriteAutoMove->DrawToBack(CenterX+int(i*DeltaX), CenterY+int(i*DeltaY));
	}
}

void CDlgMiniMap::DrawPathNode(int x, int y)
{
	m_pA2DSpriteAutoMove->DrawToBack(x, y);
}

void CDlgMiniMap::OnNameInput(int type, const ACString& strName)
{
	CDlgMiniMap* pMiniMap = GetGameUIMan()->m_pDlgMiniMap;
	CECMapDlgsMgr* pMgr = GetGameUIMan()->GetMapDlgsMgr();

	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");

	if( type == CDlgInputName::INPUTNAME_MAKE_MARK )
	{
		CDlgMiniMap::MARK Mark;

		pList->AddString(strName);
		Mark.nNPC = -1;
		Mark.strName = strName;
		Mark.vecPos = GetGameUIMan()->m_vecTargetPos;
		Mark.mapID = GetWorld()->GetInstanceID();

		pMiniMap->AddMark(Mark);

		ACString strText = GetMarkDesc(Mark, GetWorld()->GetInstanceID());
		pList->SetItemHint(pList->GetCount() - 1, strText);

		pMgr->RefreshWorldMapFlags(false);
		GetGameUIMan()->AddChatMessage(GetStringFromTable(238), GP_CHAT_MISC);
	}
	else if( type == CDlgInputName::INPUTNAME_RENAME_MARK )
	{
		pList->SetText(pList->GetCurSel(), strName);
		pMiniMap->SetMarkName(pList->GetCurSel(), strName);
	}
	else if( type == CDlgInputName::INPUTNAME_RENAME_WORLD_MARK )
	{
		PAUIIMAGEPICTURE pImage = pMgr->GetCurFlagImage();
		pImage->SetHint(strName);

		int curMark = (int)pImage->GetData();
		pList->SetText(curMark, strName);
		pMiniMap->SetMarkName(curMark, strName);
	}

	GetDlgItem("Txt_Mark")->SetText(_AL(""));
}

void CDlgMiniMap::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	CDlgMiniMap* pMiniMap = GetGameUIMan()->m_pDlgMiniMap;
	CECMapDlgsMgr* pMgr = GetGameUIMan()->GetMapDlgsMgr();

	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");

	if( 0 == stricmp(pDlg->GetName(), "Game_Mark_Del") && IDOK == iRetVal )
	{
		pMiniMap->ClearMark(pList->GetCurSel());
		pList->DeleteString(pList->GetCurSel());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_Mark_DelAll") && IDOK == iRetVal )
	{
		pList->ResetContent();
		pMiniMap->ClearMark();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_Mark_DelWorld") && IDOK == iRetVal )
	{
		int curMark = pMgr->GetCurMark();
		if( curMark >= 0 && 
			curMark < (int)pMiniMap->GetMarks().size() )
		{
			pMiniMap->ClearMark(curMark);
			pList->DeleteString(curMark);
			pMgr->RefreshWorldMapFlags(false);
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_Mark_DelAllWorld") && IDOK == iRetVal )
	{
		pMiniMap->ClearMark();
		pList->ResetContent();
		pMgr->RefreshWorldMapFlags(false);
	}
}

void CDlgMiniMap::LoadPlayerMarks(void* pData)
{
	CECGameUIMan::_USER_LAYOUT* pul = (CECGameUIMan::_USER_LAYOUT*)pData;
	CDlgMiniMap* pMiniMap = GetGameUIMan()->m_pDlgMiniMap;

	CDlgMiniMap::MARK Mark;
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Choose");

	int idInstance = GetWorld()->GetInstanceID();

	pList->ResetContent();
	for(int i = 0; i < CECGAMEUIMAN_MAX_MARKS; i++ )
	{
		if( a_strlen(pul->a_Mark[i].szName) == 0 ) break;

		Mark.nNPC = pul->a_Mark[i].nNPC;
		Mark.vecPos = pul->a_Mark[i].vecPos;
		Mark.strName = pul->a_Mark[i].szName;
		Mark.mapID = pul->a_MarkMapID[i];

		pMiniMap->AddMark(Mark);
		pList->AddString(Mark.strName);
		ACString strText = GetMarkDesc(Mark, idInstance);
		pList->SetItemHint(pList->GetCount() - 1, strText);
		pList->SetItemTextColor(pList->GetCount() - 1, (Mark.mapID != idInstance) ? 
								A3DCOLORRGB(128, 128, 128) : A3DCOLORRGB(255, 255, 255) );
	}
}

void CDlgMiniMap::LoadMapMarks(CELPrecinct* pPrecinct)
{
	CELPrecinct::MARK *pMark;
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List_Region");

	pList->ResetContent();
	if( pPrecinct )
	{
		for(int i = 0; i < pPrecinct->GetMarkNum(); i++ )
		{
			pMark = pPrecinct->GetMark(i);
			pList->AddString(pMark->strName);

			// show the desc
			CDlgMiniMap::MARK mark;
			mark.mapID = GetWorld()->GetInstanceID();
			mark.vecPos = pMark->vPos;
			pList->SetItemHint(i, GetMarkDesc(mark, mark.mapID));

			// store the map id in item data
			pList->SetItemData(i, mark.mapID , 0, "MapID");
		}
	}
}

ACString CDlgMiniMap::GetMarkDesc(const CDlgMiniMap::MARK& mark, int idInstance)
{
	bool isCurMap = (mark.mapID == idInstance);

	ACString strTxt;
	strTxt.Format(GetStringFromTable(isCurMap ? 9384:9385),
									int(mark.vecPos.x) / 10 + 400,
									int(mark.vecPos.z) / 10 + 550);
	return strTxt;
}

void CDlgMiniMap::UpdateMarks(PAUIOBJECT pObjMiniMap)
{
	// skip update when world map is shown
	if( GetGameUIMan()->GetMapDlgsMgr()->IsWorldMapShown() )
	{
		return;
	}

	// whether need to hide all marks
	if(!GetGameUIMan()->GetDialog("Win_Map")->IsShow())
	{
		int index = 1;
		while(true)
		{
			AString strName;
			strName.Format( "mMark%d", index++);
			PAUIDIALOG  pDlgMark = GetGameUIMan()->GetDialog(strName);
			if(!pDlgMark) break;
			pDlgMark->Show(false);
		}

		// all marks were hidden
		return;
	}

	int i;
	A3DVECTOR3 vecPosHost = GetHostPlayer()->GetPos();

	// get the canvas size
	A3DRECT rcMiniMap = pObjMiniMap->GetRect();
	int W = rcMiniMap.Width();
	int H = rcMiniMap.Height();
	A3DPOINT2 C = rcMiniMap.CenterPoint();

	// get the canvas scale
	float fRatio = 512.0f / 1024.0f / m_fZoom;
	float fRadius = (float)SQUARE(min(W, H) / 2 - 2);

	// get the rotation matrix
	A3DCamera *pA3DCamera = g_pGame->GetViewport()->GetA3DCamera();
	A3DMATRIX4 mr = RotateY(DEG2RAD(-(int)pA3DCamera->GetDeg() - 90));

	// show the player marks on mini map
	int nNumShow = 0;
	if (m_bShowMark){
		for( i = 0; i < m_vecMark.size(); i++ )
		{
			const MARK &mark = m_vecMark[i];
			if (mark.mapID != GetWorld()->GetInstanceID()){
				continue;
			}
			++ nNumShow;

			AString strName;
			strName.Format("mMark%d", nNumShow);
			PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(strName);
			if( !pDlg->IsShow() ) pDlg->Show(true);
			
			A3DVECTOR3 vecPos = (mark.vecPos - vecPosHost);
			vecPos.y = 0.0f;
			if( SQUARE(vecPos.x * fRatio) + SQUARE(vecPos.z * fRatio) > fRadius )
			{
				vecPos.Normalize();
				vecPos *= (float)sqrt(fRadius) / fRatio;
			}
			
			if( MODE_A == m_nMode ) vecPos = vecPos * mr;
			int x = C.x + int(vecPos.x * fRatio);
			int y = C.y - int(vecPos.z * fRatio);
			
			SIZE s = pDlg->GetSize();
			pDlg->SetPosEx(x - s.cx / 2, y - s.cy / 2);
			
			ACString strHint;
			if( mark.nNPC >= 0 )
				strHint = _AL("^FFFF00") + mark.strName;
			else
				strHint = mark.strName;
			
			ACString strText;
			strText.Format(_AL("\r%d, %d"),
				int(mark.vecPos.x) / 10 + 400,
				int(mark.vecPos.z) / 10 + 550);
			pDlg->GetDlgItem("mMark")->SetHint(strHint + strText);
			pDlg->SetCanMove(false);
		}
	}

	// hide unused flags
	for( i = nNumShow; i < CECGAMEUIMAN_MAX_MARKS; i++ )
	{
		AString strName;
		strName.Format( "mMark%d", i + 1);
		PAUIDIALOG  pDlg = GetGameUIMan()->GetDialog(strName);
		if (pDlg->IsShow()){
			pDlg->Show(false);
		}
	}

	// refresh the auto move flag
	PAUIDIALOG pDlgMark = GetGameUIMan()->GetDialog("mMark6");
	if (GetHostPlayer()->IsAutoMoving())
	{
		A3DPOINT2 TargetPos = GetGameUIMan()->AutoMoveTarget();
		A3DVECTOR3 vecTempTarget((float)TargetPos.x, 0, (float)TargetPos.y);
		A3DVECTOR3 vecPos = (vecTempTarget - vecPosHost);
		vecPos.y = 0.0f;
		
		if( MODE_A == m_nMode ) {
			if( SQUARE(vecPos.x * fRatio) + SQUARE(vecPos.z * fRatio) > fRadius )
			{
				vecPos.Normalize();
				vecPos *= (float)sqrt(fRadius) / fRatio;
			}
			vecPos = vecPos * mr;
		} else {
			if (SQUARE(vecPos.x * fRatio) > fRadius || SQUARE(vecPos.z * fRatio) > fRadius) {
				// 先放到圆上
				vecPos.Normalize();
				vecPos *= (float)sqrt(fRadius) / fRatio;
				// 再放到正方向形上
				float fScale = sqrt(fRadius) / fRatio / max(abs(vecPos.x), abs(vecPos.z));
				vecPos *= fScale;
			}
		}
		int x = C.x + int(vecPos.x * fRatio);
		int y = C.y - int(vecPos.z * fRatio);
		pDlgMark->SetPosEx(x, y);

		if(!pDlgMark->IsShow()) pDlgMark->Show(true);
	}
	else if (pDlgMark->IsShow())
	{
		pDlgMark->Show(false);
	}
}

void CDlgMiniMap::OnCommand_MailToFriends(const char * szCommand)
{
	GetGameUIMan()->m_pDlgMailToFriends->Show(true);
}
void CDlgMiniMap::OnCommand_TouchShop(const char * szCommand)
{
	AUIDialog* pTouch = GetGameUIMan()->m_pDlgTouchShop;
	pTouch->Show(!pTouch->IsShow());
}
void CDlgMiniMap::CheckMailToFriendsButton()
{
	PAUIOBJECT pObj = GetDlgItem("btn_mailtofriends");
	if (pObj){
		bool bShow(true);
		if (!CECUIConfig::Instance().GetGameUI().bMailToFriendsSwitch ||
			CECCrossServer::Instance().IsOnSpecialServer()){
			bShow = false;
		}
		pObj->Show(bShow);
	}
}

void CDlgMiniMap::OnCommand_TitleChallenge(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Event");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
	PAUIOBJECT pBtn = GetDlgItem(szCommand);
	if (pBtn) pBtn->SetFlash(false);
}

void CDlgMiniMap::OnAddTitleChallenge()
{
	PAUIOBJECT pBtn = GetDlgItem("Btn_OpenEvent");
	if (pBtn) pBtn->SetFlash(true);
	PAUIOBJECT pTitleObject = GetDlgItem("Txt_Tips");
	if (pTitleObject) {
		m_iUpdateTitleTime = GetTickCount();
		pTitleObject->Show(true);
	}
}

void CDlgMiniMap::OnCommand_Activity(const char * szCommand)
{
	ShowNewImg(false);
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Activity");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) pObj->SetFlash(false);
}

void CDlgMiniMap::OnCommand_mapmonster(const char * szCommand)
{
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	vs.bMapMonster = !vs.bMapMonster;
	GetGame()->GetConfigs()->SetVideoSettings(vs);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}

static const int GREEN_FRAME = 0;
static const int YELLOW_FRAME = 1;
static const int RED_FRAME = 2;
void CDlgMiniMap::UpdateProfitUI()
{
	ACString strHint;
	ACString strProfitTime;
	ACString strTimeColor;
	ACString strExplain;
	PAUIIMAGEPICTURE pFatigue = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Fatigue"));
	if (pFatigue == NULL) return;
	const CECHostPlayer::PROFIT_TIME_INFO* pProfitInfo = GetHostPlayer()->GetProfitTimeInfo();
	strProfitTime.Format(GetGameUIMan()->GetStringFromTable(9665),
		GetGameUIMan()->GetFormatTime(pProfitInfo->profit_time));
	
	bool bProfitNormal = pProfitInfo->profit_level != PROFIT_LEVEL_NONE;
	strTimeColor = pProfitInfo->profit_time ? _AL("^00ff00") : _AL("^ff0000");
	strExplain = bProfitNormal ? GetGameUIMan()->GetStringFromTable(9669) 
		: GetGameUIMan()->GetStringFromTable(9670);
	
	int iFrame = pProfitInfo->profit_map ? (bProfitNormal ? GREEN_FRAME : RED_FRAME) : YELLOW_FRAME; 
	
	if( CECHostPlayer::PROFIT_MAP_BATTLE == pProfitInfo->profit_map )
	{
		strExplain = bProfitNormal ? GetGameUIMan()->GetStringFromTable(9669) 
			: GetGameUIMan()->GetStringFromTable(9670);
		strHint = GetGameUIMan()->GetStringFromTable(9663) + strExplain + strTimeColor + strProfitTime;
	}
	else if ( CECHostPlayer::PROFIT_MAP_EXIST == pProfitInfo->profit_map )
	{
		strExplain = bProfitNormal ? GetGameUIMan()->GetStringFromTable(9675) 
			: GetGameUIMan()->GetStringFromTable(9670);
		strHint = GetGameUIMan()->GetStringFromTable(9674) + strExplain + strTimeColor + strProfitTime;
	}
	else
		strHint = GetGameUIMan()->GetStringFromTable(9662) + strTimeColor + strProfitTime;
	
	pFatigue->FixFrame(iFrame);
	pFatigue->SetFlash(!bProfitNormal);
	int len = ACString(strTimeColor + strProfitTime).GetLength();
	len *= pFatigue->GetFontHeight();
	pFatigue->SetHintMaxWidth(len);
	pFatigue->SetHint(strHint);
}

void CDlgMiniMap::RenderFactionPVPIcon(const void *posConverter, int hostX, int hostY)
{
	//	检查要使用的图片存在
	CECGameUIMan *pGameUIMan = CECUIHelper::GetGameUIMan();
	A2DSprite *pMineBaseSprite = pGameUIMan->m_pFactionPVPMineBaseSprite[CECGameUIMan::FACTION_PVP_MINI_ICON];
	A2DSprite *pMineSprite = pGameUIMan->m_pFactionPVPMineSprite[CECGameUIMan::FACTION_PVP_MINI_ICON];
	if (!pMineBaseSprite || !pMineSprite){
		return;
	}
	//	检查当前状态是否可以显示
	if (!CECUIHelper::FactionPVPCanShowResourceInMap()){
		return;
	}
	//	处理每个剩余资源的 domain
	const ConverToLocalPos &conv = *(const ConverToLocalPos *)posConverter;
	CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
	CECFactionPVPModel::DomainWithResourceIterator begin = factionPVPModel.BeginDomainWithResource();
	CECFactionPVPModel::DomainWithResourceIterator end = factionPVPModel.EndDomainWithResource();
	for (CECFactionPVPModel::DomainWithResourceIterator it = begin; it != end; ++ it){
		int domain = *it;
		const CECFactionPVPModel::ResourcePosition *pResourcePos = factionPVPModel.GetResourcePosition(domain);
		if (!pResourcePos){
			continue;
		}
		A3DVECTOR3 pos = conv(pResourcePos->minePos);
		if (conv.CanShow(pos)){
			pMineSprite->DrawToBack(hostX + (int)pos.x, hostY - (int)pos.z);
		}
		pos = conv(pResourcePos->mineBasePos);
		if (conv.CanShow(pos)){
			pMineBaseSprite->DrawToBack(hostX + (int)pos.x, hostY - (int)pos.z);
		}
	}
}

void CDlgMiniMap::ShowNewImg(bool bIsShow) {
	AUIObject* pObj = GetDlgItem("Img_New");
	if (pObj) {
		if (bIsShow) {
			ChangeFocus(pObj);
		}
		pObj->Show(bIsShow);
	}
}