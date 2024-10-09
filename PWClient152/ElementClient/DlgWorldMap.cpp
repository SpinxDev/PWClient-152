// Filename	: DlgWorldMap.cpp
// Creator	: Tom Zhou
// Date		: October 16, 2005

#include "A3DGDI.h"
#include "A3DDevice.h"
#include "A3DCamera.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DTexture.h"
#include <AWIniFile.h>
#include "DlgWorldMap.h"
#include "DlgInputName.h"
#include "DlgTeamMain.h"
#include "DlgGuildMap.h"
#include "DlgGuildMapStatus.h"
#include "DlgFactionPVPStatus.h"
#include "DlgCountryMap.h"
#include "EC_UIHelper.h"
#include "DlgCountryMapSub.h"
#include "DlgCountryWarMap.h"
#include "DlgTask.h"
#include "DlgMiniMap.h"
#include "DlgHistoryStage.h"
#include "DlgRandomMap.h"
#include "EC_ElsePlayer.h"
#include "EC_ManPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Resource.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_Team.h"
#include "EC_Utility.h"
#include "EC_Configs.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_Instance.h"
#include "EC_Inventory.h"
#include "EC_IntelligentRoute.h"
#include "globaldataman.h"
#include "elementdataman.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AStringWithWildcard.h"
#include "AUIDef.h"
#include "A3DFlatCollector.h"
#include "AFI.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"
#include "EC_FactionPVP.h"
#include "EC_DomainGuild.h"
#include "EC_UIConfigs.h"

#include "WikiSearchCommand.h"
#include "WikiSearchNPC.h"

#define SQUARE(x)	((x) * (x))

#define MAP_DEFAULT_WIDTH 1024
#define MAP_DEFAULT_HEIGHT 768
#define new A_DEBUG_NEW

static int GetTransmitTargetsNum(const NPC_TRANSMIT_SERVICE *pService)
{
	int num_targets = 0;
	if (pService)
	{
		num_targets = max(0, pService->num_targets);
		num_targets = min(num_targets, sizeof(pService->targets)/sizeof(pService->targets[0]));
	}
	return num_targets;
}

static int GetTransIndex(const NPC_ESSENCE *pNPC, int idTarget)
{
	int nIndex(-1);
	while (true)
	{
		if (!idTarget)
			break;

		if (!pNPC || pNPC->id_transmit_service <= 0)
			break;

		DATA_TYPE dataType = DT_INVALID;
		const NPC_TRANSMIT_SERVICE *pTransServ = (const NPC_TRANSMIT_SERVICE *)g_pGame->GetElementDataMan()->get_data_ptr(pNPC->id_transmit_service, ID_SPACE_ESSENCE, dataType);
		if (!pTransServ || dataType != DT_NPC_TRANSMIT_SERVICE)
			break;

		for (int i(0); i < GetTransmitTargetsNum(pTransServ); ++ i)
		{
			int id = pTransServ->targets[i].idTarget;
			if (id != 0 && id == idTarget)
			{
				nIndex = i;
				break;
			}
		}
		break;
	}
	return nIndex;
}

//------------------------------------------------------------------------
// cache for valid NPC who has a valid transmit service
//------------------------------------------------------------------------
class CECNPCTransmitCache
{
public:
	struct Info
	{
		const NPC_ESSENCE* pNPC;
		const NPC_TRANSMIT_SERVICE* pSrv;
		int		fee;
		int		required_level;
	};
	
	typedef std::pair<int, Info> CachePair;
	typedef std::multimap<int, Info> CacheType;
	typedef std::pair<CacheType::const_iterator, CacheType::const_iterator> CacheRange;

public:
	CECNPCTransmitCache();
	
	CacheRange GetValidNPC(int target_id) const { return m_NPCCache.equal_range(target_id); }

	bool IsInvalidNPC(const NPC_ESSENCE *pNPC)const;
	
private:
	CacheType m_NPCCache;
};

//------------------------------------------------------------------------
AUI_BEGIN_COMMAND_MAP(CDlgWorldMap, CDlgBase)

AUI_ON_COMMAND("makemark",		OnCommand_makemark)
AUI_ON_COMMAND("edit",			OnCommand_edit)
AUI_ON_COMMAND("delete",		OnCommand_delete)
AUI_ON_COMMAND("deleteall",		OnCommand_deleteall)
AUI_ON_COMMAND("radar",			OnCommand_radar)
AUI_ON_COMMAND("arrow",			OnCommand_arrow)
AUI_ON_COMMAND("inc",			OnCommand_inc)
AUI_ON_COMMAND("dec",			OnCommand_dec)
AUI_ON_COMMAND("guildmap",		OnCommand_guildmap)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_SwitchTaskShow", OnCommand_SwitchTaskShow)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWorldMap, CDlgBase)

AUI_ON_EVENT("Flag*",			WM_LBUTTONUP,	OnEventLButtonUp_Flag)
AUI_ON_EVENT("Flag*",			WM_LBUTTONDBLCLK, OnEventLButtonDblClk_Flag)
AUI_ON_EVENT("WorldMap",		WM_MOUSEMOVE,	OnEventMouseMove_World)
AUI_ON_EVENT("Control_*",		WM_MOUSEMOVE,	OnEventMouseMove_Control)
AUI_ON_EVENT("WorldMap",		WM_LBUTTONUP,	OnEventLButtonUp_World)
AUI_ON_EVENT("WorldMap",		WM_RBUTTONUP,	OnEventRButtonUp_World)
AUI_ON_EVENT("Part*",			WM_MOUSEMOVE,	OnEventMouseMove_Detail)
AUI_ON_EVENT("Part*",			WM_LBUTTONDOWN,	OnEventLButtonDown_Detail)
AUI_ON_EVENT("Part*",			WM_LBUTTONUP,	OnEventLButtonUp_Detail)
AUI_ON_EVENT("Part*",			WM_RBUTTONUP,	OnEventRButtonUp_Detail)
//AUI_ON_EVENT("Part*",			WM_LBUTTONDBLCLK, OnEventLButtonDblClk_Detail)
AUI_ON_EVENT("WorldMapTravel",	WM_MOUSEMOVE,	OnEventMouseMove_Travel)
AUI_ON_EVENT("WorldMapTravel",	WM_LBUTTONUP,	OnEventLButtonUp_Travel)
AUI_ON_EVENT("WorldMapTravel",	WM_RBUTTONUP,	OnEventRButtonUp_Travel)
AUI_ON_EVENT("MapBG",			WM_RBUTTONUP,	OnEventRButtonUp_Travel)
AUI_ON_EVENT("*",	WM_LBUTTONUP,		OnEventLButtonUp_Sign)

AUI_END_EVENT_MAP()

CDlgWorldMap::CDlgWorldMap()
{
	m_pCurNPCEssence = NULL;
	m_iTaskShowType = TASK_TYPE_NONE;
}

CDlgWorldMap::~CDlgWorldMap()
{
}

bool CDlgWorldMap::OnInitDialog()
{
	if( stricmp(m_szName, "Win_BigMapMark") == 0 )
	{
		GetDlgItem("Btn_GuildMap")->Enable(false);
	}

	m_pImgHint = (PAUIIMAGEPICTURE)GetDlgItem("Hint");

	return true;
}

void CDlgWorldMap::OnShowDialog()
{
	if (m_szName != "Win_WorldMapTravel")
	{
		GetGameUIMan()->BringWindowToTop(GetGameUIMan()->GetDialog("Win_AutoMove"));
		GetGameUIMan()->BringWindowToTop(GetGameUIMan()->GetDialog("Win_HideMove"));
		GetGameUIMan()->BringWindowToTop(GetGameUIMan()->GetDialog("mMark6"));
		GetGameUIMan()->BringWindowToTop(GetGameUIMan()->GetDialog("Win_BigMapMark"));
	}
	else
	{
		m_pCurNPCEssence = GetGameUIMan()->GetCurNPCEssence();
	}

	if (m_szName == "Win_WorldMap")
	{
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("WorldMap");
		if (GetGameUIMan()->GetMapDlgsMgr()->LoadMapTexture(m_pA3DDevice, pImage, CECMapDlgsMgr::MAP_TYPE_NORMAL))
		{
			GetGameUIMan()->GetMapDlgsMgr()->RefreshWorldMapFlags(false);
			bool bShow = GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(GetWorld()->GetInstanceID());		
			GetGameUIMan()->GetDialog("Win_Bigmap")->Show(bShow);
			GetGameUIMan()->GetDialog("Win_BigMapMark")->Show(bShow);
		}		
		CECFactionPVPModel::Instance().GetFactionPVPMap();
	}
	else if (m_szName == "Win_WorldMapDetail")
	{
		GetGameUIMan()->GetMapDlgsMgr()->RefreshDetailMap();
		GetGameUIMan()->GetMapDlgsMgr()->RefreshWorldMapFlags(false);
		UpdateDetailMapControlPos();
	}
	UpdateTaskControl();
}

void CDlgWorldMap::OnHideDialog()
{
	if (m_szName == "Win_WorldMapTravel")
	{
		m_pCurNPCEssence = NULL;
	}
	else if (m_szName == "Win_WorldMap")
	{
		if (GetGameUIMan()->GetMapDlgsMgr()->IsMarking())
		{
			GetGameUIMan()->GetMapDlgsMgr()->SetMarking(false);
			GetGameUIMan()->m_pDlgInputName->OnCommand_CANCEL("");
		}

		int nItem;
		A2DSprite *pA2DSprite;
		
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("WorldMap");
		pImage->GetCover(&pA2DSprite, &nItem);
		A3DRELEASE(pA2DSprite);
		pImage->ClearCover();
		
		GetGameUIMan()->GetMapDlgsMgr()->RefreshWorldMapFlags(true);
	}
	else if (m_szName == "Win_WorldMapDetail")
	{
		GetGameUIMan()->GetMapDlgsMgr()->ClearDetailMap();
		GetGameUIMan()->GetMapDlgsMgr()->SetDraging(false);
		GetGameUIMan()->GetMapDlgsMgr()->RefreshWorldMapFlags(true);
		
		if (GetGameUIMan()->GetMapDlgsMgr()->IsMarking())
		{
			GetGameUIMan()->GetMapDlgsMgr()->SetMarking(false);
			GetGameUIMan()->m_pDlgInputName->OnCommand_CANCEL("");
		}
	}
}

void CDlgWorldMap::OnCommand_makemark(const char * szCommand)
{
	int nInstanceID = GetWorld()->GetInstanceID();
	if(!GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstanceID))
		return;

	if( GetGameUIMan()->m_pDlgMiniMap->GetMarks().size() >= CECGAMEUIMAN_MAX_MARKS )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(507),
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	GetGameUIMan()->GetMapDlgsMgr()->SetMarking(true);
	GetGame()->ChangeCursor(RES_CUR_FLAG);
}

void CDlgWorldMap::OnCommand_edit(const char * szCommand)
{
	int curMark = GetGameUIMan()->GetMapDlgsMgr()->GetCurMark();
	if( curMark < 0 ) return;

	if( GetGameUIMan()->m_pDlgMiniMap->GetMarks()[curMark].nNPC >= 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(511), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		ACString strName = GetGameUIMan()->m_pDlgMiniMap->GetMarks()[curMark].strName;
		GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_RENAME_WORLD_MARK, strName);
	}
}

void CDlgWorldMap::OnCommand_delete(const char * szCommand)
{
	if( GetGameUIMan()->GetMapDlgsMgr()->GetCurMark() >= 0 )
	{
		GetGameUIMan()->MessageBox("Game_Mark_DelWorld", GetStringFromTable(509),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgWorldMap::OnCommand_deleteall(const char * szCommand)
{
	if( GetGameUIMan()->m_pDlgMiniMap->GetMarks().size() > 0 )
	{
		GetGameUIMan()->MessageBox("Game_Mark_DelAllWorld", GetStringFromTable(510),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgWorldMap::OnCommand_radar(const char * szCommand)
{
	GetGameUIMan()->m_pDlgMiniMap->OnCommand_radar(szCommand);
}

void CDlgWorldMap::OnCommand_arrow(const char * szCommand)
{
	GetGameUIMan()->m_pDlgMiniMap->OnCommand_arrow(szCommand);
}

void CDlgWorldMap::OnCommand_inc(const char * szCommand)
{
	if( GetWorld()->GetInstanceID() != MAJOR_MAP )
		return;

	if( GetGameUIMan()->m_pDlgWorldMap->IsShow() )
	{
		PAUIOBJECT pObj = GetGameUIMan()->m_pDlgWorldMap->GetDlgItem("WorldMap");
		A3DRECT rcItem = pObj->GetRect();
		A3DVECTOR3 vecPos = GetHostPlayer()->GetPos();

		A3DPOINT2 mapPos = rcItem.CenterPoint() + 
			GetGameUIMan()->GetMapDlgsMgr()->GetPixelsByPos(vecPos.x, vecPos.z, MAJOR_MAP);

		LPARAM lParam = MAKELPARAM(mapPos.x, mapPos.y);

		GetGameUIMan()->m_pDlgWorldMap->OnEventLButtonUp_World(0, lParam, pObj);
	}
}

void CDlgWorldMap::OnCommand_dec(const char * szCommand)
{
	if( GetWorld()->GetInstanceID() != MAJOR_MAP )
		return;

	if( GetGameUIMan()->m_pDlgWorldMap->IsShow() )
	{
		GetGameUIMan()->GetMapDlgsMgr()->ShowWorldMap(false);
	}
	else if( GetGameUIMan()->m_pDlgWorldMapDetail->IsShow() )
	{
		GetGameUIMan()->m_pDlgWorldMapDetail->OnEventRButtonUp_Detail(
			0, 0, GetGameUIMan()->m_pDlgWorldMapDetail->GetDlgItem("Part1"));
	}
}

void CDlgWorldMap::OnCommand_guildmap(const char * szCommand)
{
	GetGameUIMan()->m_pDlgWorldMap->OnCommand_CANCEL("IDCANCEL");

	if( !GetGameUIMan()->m_pDlgGuildMap->IsShow() )
	{
		GetGameUIMan()->m_pDlgGuildMap->SetType(CDlgGuildMap::FACTIONMAP_TYPE_NORMAL);
		GetGameUIMan()->m_pDlgGuildMap->Show(true);
		GetGameUIMan()->m_pDlgGMapStatus1->Show(true);
		GetGameUIMan()->m_pDlgGMapStatus1->SetCanMove(false);
	}
	else if( GetGameUIMan()->m_pDlgGuildMap->GetType() == CDlgGuildMap::FACTIONMAP_TYPE_NORMAL )
	{
		GetGameUIMan()->m_pDlgGuildMap->Show(false);
		GetGameUIMan()->m_pDlgGMapStatus1->Show(false);
	}
}

void CDlgWorldMap::OnCommand_CANCEL(const char * szCommand)
{
	GetGameUIMan()->GetMapDlgsMgr()->ShowWorldMap(false);
}

void CDlgWorldMap::OnTick()
{
	CDlgBase::OnTick();
	
	if( m_szName == "Win_WorldMap" ||
		m_szName == "Win_WorldMapDetail" )
	{
		BringUpTools();
	}
}

bool CDlgWorldMap::Render(void)
{
	// skip rendering the flags when not markable
	int nInstanceID = GetWorld()->GetInstanceID();
	if( !GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstanceID) && (IsMatch(m_szName, "Flag*") || IsMatch(m_szName, "Sign*")))
		return true;

	bool bval = CDlgBase::Render();

	if( m_szName == "Win_WorldMap" )
		UpdateWorldMap();
	else if( m_szName == "Win_WorldMapDetail" )
		UpdateWorldMapDetail();
	else if( m_szName == "Win_WorldMapTravel" )
		UpdateWorldMapTravel();

	return bval;
}
// convert team member pos to screen pixel
struct PosConvertorWorld : public CECMapDlgsMgr::PosConvertor
{
	PosConvertorWorld(int nInstanceID)
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		PAUIDIALOG pDlg = pGameUI->GetDialog("Win_WorldMap");

		// get the background center
		A3DRECT rcItem = pDlg->GetDlgItem("WorldMap")->GetRect();
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

bool CDlgWorldMap::UpdateWorldMap()
{
	if( !IsShow() )
		return true;

	int nInstanceID = GetWorld()->GetInstanceID();

	// check whether the mark could be drawn	
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if(!pGameUIMan->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstanceID))
	{
		return true;
	}

	// draw the team member mark
	PosConvertorWorld conv(nInstanceID);
	
	RenderFactionPVPIcon(&conv
		, pGameUIMan->m_pFactionPVPMineBaseSprite[CECGameUIMan::FACTION_PVP_SMALL_ICON]
		, pGameUIMan->m_pFactionPVPMineBaseSprite[CECGameUIMan::FACTION_PVP_SMALL_ICON_FOR_MY_FACTION]
		, pGameUIMan->m_pFactionPVPMineSprite[CECGameUIMan::FACTION_PVP_SMALL_ICON]
		, pGameUIMan->m_pFactionPVPMineSprite[CECGameUIMan::FACTION_PVP_SMALL_ICON_FOR_MY_FACTION]);

	pGameUIMan->GetMapDlgsMgr()->DrawTeamMark(conv);

	// draw the host player mark
	pGameUIMan->GetMapDlgsMgr()->DrawHostMark(conv);
	
	// draw automove path
	pGameUIMan->GetMapDlgsMgr()->DrawAutoMoveFlag(conv);

	// draw mark on guild war map
	if( GUILD_WAR_MAP(nInstanceID) && 
		GetGame()->GetConfigs()->GetVideoSettings().bMapPlayer )
	{
		A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
		CECTeam *pTeam = GetHostPlayer()->GetTeam();

		int i;
		CECPlayerMan *pPlayerMan = GetWorld()->GetPlayerMan();
		const APtrArray<CECElsePlayer *> &Players = pPlayerMan->GetPlayersInMiniMap();
		A3DCOLOR color;
		for( i = 0; i < Players.GetSize(); i++ )
		{
			if( pTeam && pTeam->GetMemberByID(Players[i]->GetCharacterID()) )
				continue;
			if( Players[i]->GetBattleCamp() != GetHostPlayer()->GetBattleCamp() )
				continue;

			if( Players[i]->IsInBattle() )
				if( Players[i]->GetBattleCamp() == GP_BATTLE_CAMP_INVADER )
					color = A3DCOLORRGB(255, 0, 0);
				else
					color = A3DCOLORRGB(0, 0, 255);

			A3DVECTOR3 vecPos = Players[i]->GetPos();
			A3DPOINT2 absPos = conv(vecPos);
			
			pFC->AddRect_2D(absPos.x - 1, absPos.y - 1, absPos.x + 2, absPos.y + 2, color);
		}

		pFC->Flush();
	}
	
	return true;
}

// convert team member pos to screen pixel
struct PosConvertorDetail : public CECMapDlgsMgr::PosConvertor
{
	PosConvertorDetail()
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		vecULCorner.x = pGameUI->GetMapDlgsMgr()->GetCorner(CECMapDlgsMgr::MINIMAP_UL).x * 1024.0f - 4096.0f;
		vecULCorner.y = 0.0f;
		vecULCorner.z = 5632.0f - pGameUI->GetMapDlgsMgr()->GetCorner(CECMapDlgsMgr::MINIMAP_UL).y * 1024.0f;

		PAUIDIALOG pDlg = pGameUI->GetDialog("Win_WorldMapDetail");
		s = pDlg->GetSize();

		POINT ptPosDetail = pDlg->GetPos();
		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		ptPos.x = ptPosDetail.x + p->X;
		ptPos.y = ptPosDetail.y + p->Y;
	}
	
	// from pos to pixel
	virtual A3DPOINT2 operator()(const A3DVECTOR3& vecPos) const
	{
		A3DPOINT2 ret;
		
		ret.x = ptPos.x + int(s.cx * (vecPos.x - vecULCorner.x) / 2048.0f);
		ret.y = ptPos.y + int(s.cy * (vecULCorner.z - vecPos.z) / 2048.0f);
		return ret;
	}

	// from pixel to pos
	virtual A3DVECTOR3 operator()(const A3DPOINT2& ret) const
	{
		A3DVECTOR3 vecPos;
		vecPos.x = (ret.x - ptPos.x) * 2048.0f / s.cx + vecULCorner.x;
		vecPos.y = 0;
		vecPos.z = vecULCorner.z - (ret.y - ptPos.y) * 2048.0f / s.cy;
		return vecPos;
	}

	bool IsInside(const A3DVECTOR3& vecPos)
	{
		return ( vecPos.x > vecULCorner.x && vecPos.x < vecULCorner.x + 2048.0f &&
				 vecPos.z < vecULCorner.z && vecPos.z > vecULCorner.z - 2048.0f );
	}

private:
	SIZE s;
	POINT ptPos;
	A3DVECTOR3 vecULCorner;
};

bool CDlgWorldMap::UpdateWorldMapDetail()
{
	if( !IsShow() ) return true;
	
	PosConvertorDetail conv;
	
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	
	RenderFactionPVPIcon(&conv
		, pGameUIMan->m_pFactionPVPMineBaseSprite[CECGameUIMan::FACTION_PVP_NORMAL_ICON]
		, pGameUIMan->m_pFactionPVPMineBaseSprite[CECGameUIMan::FACTION_PVP_NORMAL_ICON_FOR_MY_FACTION]
		, pGameUIMan->m_pFactionPVPMineSprite[CECGameUIMan::FACTION_PVP_NORMAL_ICON]
		, pGameUIMan->m_pFactionPVPMineSprite[CECGameUIMan::FACTION_PVP_NORMAL_ICON_FOR_MY_FACTION]);

	// draw the host player mark
	if( conv.IsInside(GetHostPlayer()->GetPos()) )
	{
		pGameUIMan->GetMapDlgsMgr()->DrawHostMark(conv);
	}

	// draw automove path
	pGameUIMan->GetMapDlgsMgr()->DrawAutoMoveFlag(conv);

	// draw the team mark
	pGameUIMan->GetMapDlgsMgr()->DrawTeamMark(conv);

	return true;
}

int CDlgWorldMap::GetTravelWorldID()
{
	ASSERT(0 == stricmp(GetName(), "Win_WorldMapTravel") );

	int worldid = MAJOR_MAP;

	DWORD dwType = GetData();
	if(dwType == DT_NPC_TRANSMIT_SERVICE)
	{
		worldid = (int)GetDataPtr("WorldID");
	}

	return worldid;
}

bool CDlgWorldMap::UpdateWorldMapTravel()
{
	if( !IsShow() ) return true;

	A3DRECT rcItem = GetDlgItem("WorldMapTravel")->GetRect();
	A3DPOINT2 ptCenter = rcItem.CenterPoint();

	CECMapDlgsMgr* pMgr = GetGameUIMan()->GetMapDlgsMgr();

	// draw the valid way-points
	pMgr->DrawWayPoints(GetCurNPCEssence(), ptCenter, GetTravelWorldID(), GetData() == DT_NPC_TRANSMIT_SERVICE);

	// draw the transmit way-points
	pMgr->DrawTransWay(ptCenter);

	return true;
}

const abase::vector<CECMapDlgsMgr::DefaultData>& CECMapDlgsMgr::GetDlgDefaultData(PAUIDIALOG pDlg)
{
	DlgDefault::iterator iter = m_DefaultPosAndSize.find(pDlg);
	abase::vector<DefaultData> vecDefault;
	// 保存default pos 和default size以供缩放使用
	if (iter == m_DefaultPosAndSize.end())
	{
		PAUIOBJECTLISTELEMENT pElement = pDlg->GetFirstControl();
		POINT p;
		SIZE s;
		int fontsize;
		while( pElement )
		{
			p = pElement->pThis->GetDefaultPos();
			s = pElement->pThis->GetDefaultSize();
			fontsize = pElement->pThis->GetFontHeight();
			vecDefault.push_back(DefaultData(p, s, fontsize));
			pElement = pDlg->GetNextControl(pElement);
		}
		p = pDlg->GetPos();
		s = pDlg->GetDefaultSize();
		AUIOBJECT_SETPROPERTY prop;
		fontsize = pDlg->GetProperty("Font", &prop);
		vecDefault.push_back(DefaultData(p, s, prop.font.nHeight));
		m_DefaultPosAndSize[pDlg] = vecDefault;
	}

	return m_DefaultPosAndSize[pDlg];
}
void CECMapDlgsMgr::ResizeFullScreenDialog(PAUIDIALOG pDlg, int iWidth, int iHeight)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if (NULL == pDlg || 0 == iWidth || 0 == iHeight)
	{
		ASSERT(0);
		return;
	}
	pDlg->SetData(1,"DoSomethingWhenResized");
	const abase::vector<DefaultData>& vecDefault = GetDlgDefaultData(pDlg);

	size_t len = vecDefault.size();
	int iOldWidth = vecDefault[len - 1].s.cx;
	int iOldHeight = vecDefault[len - 1].s.cy;
	if (0 == iOldHeight || 0 == iOldWidth)
	{
		ASSERT(0);
		return;
	}
	float fWidthRadio = (float)iWidth / iOldWidth;
	float fHeightRadio = (float)iHeight / iOldHeight;
	pDlg->SetSize(iWidth, iHeight);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	PAUIOBJECTLISTELEMENT pElement = pDlg->GetFirstControl();
	int i = 0;
	while( pElement )
	{
		PAUIOBJECT pObj = pElement->pThis;
		int x = vecDefault[i].p.x;
		int y = vecDefault[i].p.y;
		int sx = vecDefault[i].s.cx;
		int sy = vecDefault[i].s.cy;
		int newx = (int)(x * fWidthRadio);
		int newy = (int)(y * fHeightRadio);
		int neww = int((x + sx) * fWidthRadio) - newx;
		int newh = int((y + sy) * fHeightRadio) - newy;
		pObj->SetPos(newx, newy);
		pObj->SetSize(neww, newh);

		pElement = pDlg->GetNextControl(pElement);
		i++;
	}
}

void CECMapDlgsMgr::ResizeDlgKeepingMapRadio(PAUIDIALOG pDlg, PAUIOBJECT pMap, int iDlgWid, int iDlgHt, int iTargetW, int iTargetH, int iOffsetX, int iOffsetY)
{
	const abase::vector<DefaultData>& vecDefault = GetDlgDefaultData(pDlg);
	pDlg->SetSize(iDlgWid, iDlgHt);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);
	pDlg->SetData(1,"DoSomethingWhenResized");
	
	pMap->SetSize( iTargetW, iTargetH );
	pMap->SetPos( iOffsetX, iOffsetY ); 
	pMap->SetAcceptMouseMessage(true);
	
	float fWidthRadio, fHeightRadio;
	PAUIOBJECTLISTELEMENT pElement = pDlg->GetFirstControl();
	PAUIOBJECT pObj;
	int i = 0;
	while( pElement )
	{
		pObj = pElement->pThis;
		if (pObj == pMap) 
		{
			if (0 == vecDefault[i].s.cx || 0 == vecDefault[i].s.cy)
			{
				ASSERT(false);
				return;
			}			
			fWidthRadio = (float)iTargetW / vecDefault[i].s.cx;
			fHeightRadio = (float)iTargetH / vecDefault[i].s.cy;
			pElement = pDlg->GetNextControl(pElement);
			++i;
			continue;
		}
		int newx = int((vecDefault[i].p.x) * fWidthRadio + iOffsetX);
		int newy = int((vecDefault[i].p.y) * fHeightRadio + iOffsetY);
		int neww = int(vecDefault[i].s.cx * fWidthRadio);
		int newh = int(vecDefault[i].s.cy * fHeightRadio);
		pObj->SetPos(newx, newy);
		pObj->SetSize(neww, newh);
		// 缩放字体
		AUIOBJECT_SETPROPERTY prop;
		pObj->GetProperty("Font", &prop);
		prop.font.nHeight = vecDefault[i].FontSize * fWidthRadio;
		pObj->SetProperty("Font", &prop);
		pElement = pDlg->GetNextControl(pElement);
		++i;
	}
}

void CECMapDlgsMgr::ResizeWorldMap(A3DRECT *prc)
{
	RECT rc;
	if( prc )
	{
		rc.left = prc->left;
		rc.top = prc->top;
		rc.right = prc->right;
		rc.bottom = prc->bottom;
	}
	else
	{
		A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
		rc.left = param.X;
		rc.top = param.Y;
		rc.right = param.X + param.Width;
		rc.bottom = param.Y + param.Height;
	}

	// offset (x,y) and target (w, h)
	int ox, oy, tw, th;
	
	const int dw = MAP_DEFAULT_WIDTH; // default width
	const int dh = MAP_DEFAULT_HEIGHT; // default height
	const float dr = (float)dw / dh; // default ratio
	
	const int w = rc.right - rc.left;
	const int h = rc.bottom - rc.top;
	const float r = (float)w / h; // current ratio

	// whether a wide-screen or not
	if(r > dr)
	{
		// width is larger
		tw = h * dw / dh; // h * dr;
		ox = (w - tw) / 2;
		oy = 0;
		th = h;
	}
	else
	{
		// height is larger
		th = w * dh / dw; // w / dr;
		ox = 0;
		oy = ( h - th ) / 2;
		tw = w;
	}
	
	// NOTICE: here assume the texture is a square shape
	oy -= (tw - th) / 2; // adjust offset
	th = tw; // adjust size

	PAUIDIALOG pDlg = NULL;
	PAUIOBJECT pObj = NULL;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	
	m_nMinSize = (m_WideMode ? w:tw) * 3 / 4;

	//resize the random map
	pDlg = pGameUI->GetDialog("Win_RandomMap");
	pObj = pDlg->GetDlgItem("Edi_BG");
	ResizeDlgKeepingMapRadio(pDlg, pObj, w, h, tw, th, ox, oy);
	CDlgRandomMap* pRandomMapDlg = dynamic_cast<CDlgRandomMap*>(pDlg);
	if(pRandomMapDlg) pRandomMapDlg->OnResize();

	//resize the country map
	pDlg = pGameUI->GetDialog("Win_CountryMap");
	pObj = pDlg->GetDlgItem("CountryMap");
	ResizeDlgKeepingMapRadio(pDlg, pObj, w, h, tw, th, ox, oy);
	CDlgCountryMapSub *pDlgCountryMapSub = dynamic_cast<CDlgCountryMapSub *>(pGameUI->GetDialog("Win_CountryMapSub"));
	if (pDlgCountryMapSub) pDlgCountryMapSub->OnResize();

	// resize the country war map
	pDlg = pGameUI->GetDialog("Win_CountryWarMap");
	pObj = pDlg->GetDlgItem("CountryMap");
	ResizeDlgKeepingMapRadio(pDlg, pObj, w, h, tw, th, ox, oy);
	// resize the guide map
	pDlg = pGameUI->GetDialog("Win_GuildMap");
	pDlg->SetSize(w, h);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	pObj = pDlg->GetDlgItem("GuildMap");
	pObj->SetSize( tw, th );
	pObj->SetPos( ox, oy ); 
	pObj->SetAcceptMouseMessage(true);

	// resize the worldmap size
	pDlg = pGameUI->GetDialog("Win_WorldMap");
	pDlg->SetSize(w, h);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	PAUIOBJECT pBG = pDlg->GetDlgItem("MapBG");
	pBG->SetSize( w, th );
	pBG->SetPos( 0, oy ); 
	pBG->Show(m_WideMode); // hide in normal mode

	pObj = pDlg->GetDlgItem("WorldMap");
	pObj->SetSize( tw, th );
	pObj->SetPos( ox, oy ); 
	pObj->SetAcceptMouseMessage(true);

	if(m_WideMode)
	{
		PAUIIMAGEPICTURE pImage = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
		AdjustMapTexture(pImage, dw, pBG->GetSize(), pBG->GetPos());
	}

	// resize the travel map
	pDlg = pGameUI->GetDialog("Win_WorldMapTravel");
	pDlg->SetSize(w, h);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	pBG = pDlg->GetDlgItem("MapBG");
	pBG->SetSize( w, th );
	pBG->SetPos( 0, oy ); 
	pBG->Show(m_WideMode); // hide in normal mode

	pObj = pDlg->GetDlgItem("WorldMapTravel");
	pObj->SetSize( tw, th ); 
	pObj->SetPos( ox, oy ); 
	pObj->SetAcceptMouseMessage(true);

	if(m_WideMode)
	{
		PAUIIMAGEPICTURE pImage = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
		AdjustMapTexture(pImage, dw, pBG->GetSize(), pBG->GetPos());
	}

	// resize the detail map
	pDlg = pGameUI->GetDialog("Win_WorldMapDetail");
	pDlg->SetSize(w * 2, w * 2);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	pObj = pDlg->GetDlgItem("Part1");
	pObj->SetSize(w, w);
	pObj->SetPos(0, 0);

	pObj = pDlg->GetDlgItem("Part2");
	pObj->SetSize(w, w);
	pObj->SetPos(w, 0);

	pObj = pDlg->GetDlgItem("Part3");
	pObj->SetSize(w, w);
	pObj->SetPos(0, w);

	pObj = pDlg->GetDlgItem("Part4");
	pObj->SetSize(w, w);
	pObj->SetPos(w, w);

	CDlgHistoryStage* pHistoryDlg = dynamic_cast<CDlgHistoryStage*>(pGameUI->GetDialog("Win_HistoryPro"));
	if (pHistoryDlg && pHistoryDlg->IsShow())
		pHistoryDlg->OnRefreshPos();
}

void CECMapDlgsMgr::RefreshWorldMapFlags(bool bCloseAll)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_WorldMapDetail");
	PAUIDIALOG pDlgWorldMap = pGameUI->GetDialog("Win_WorldMap");
	bool bMapShow = pDlgWorldMap->IsShow() || pDlgDetail->IsShow();

	int idInstance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	
	// select proper converter
	PosConvertor& conv = *GetCurConvertor();

	// for player flag
	int markIndex = 0, flagIndex = 1;
	CDlgMiniMap* pDlgMini = (CDlgMiniMap*)pGameUI->GetDialog("Win_Map");
	while(true)
	{
		AString strName;
		strName.Format("Flag%d", flagIndex);
		PAUIDIALOG pShow = pGameUI->GetDialog(strName);
		if(!pShow) break;
		
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pShow->GetDlgItem("Flag");
		pImage->SetHint(_AL(""));
		pImage->FixFrame(0);

		bool bShow = false;
		if( bCloseAll || markIndex >= (int)pDlgMini->GetMarks().size())
		{
			// skip to next flag
			flagIndex++;
		}
		else
		{
			const CDlgMiniMap::MARK& mark = pDlgMini->GetMarks()[markIndex];
			markIndex++;

			// only show the flag on current map
			if(mark.mapID != idInstance)
				continue;

			// should show the flag
			bShow = true;

			// bind and jump to next flag
			pImage->SetData(markIndex-1, "PlayerMarkIndex");
			flagIndex++;

			A3DPOINT2 absPos = conv(mark.vecPos);
			A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
			pShow->SetPosEx(absPos.x - p->X, absPos.y - pShow->GetSize().cy - p->Y);

			if( mark.nNPC >= 0 )
			{
				pImage->SetHint(_AL("^FFFF00") + mark.strName);
				pShow->SetCanMove(false);
			}
			else
			{
				pImage->SetHint(mark.strName);
				pShow->SetCanMove(true);
			}
		}

		bShow &= bMapShow;
		if(pShow->IsShow() != bShow) pShow->Show(bShow);
	}

	// for task flag
	markIndex = 0; flagIndex = 1;
	CDlgTask* pDlgTask = (CDlgTask*)pGameUI->GetDialog("Win_Quest");
	while(true)
	{
		AString strName;
		strName.Format("Sign%d", flagIndex);
		PAUIDIALOG pShow = pGameUI->GetDialog(strName);
		if(!pShow) break;
		
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pShow->GetDlgItem("Sign");
		pImage->SetHint(_AL(""));
		bool bShow = false;
		
		if( bCloseAll || markIndex >= (int)pDlgTask->GetObjectCoords().size() )
		{
			// skip to next flag
			flagIndex++;
		}
		else
		{
			const CECGame::OBJECT_COORD& coord = pDlgTask->GetObjectCoords()[markIndex];
			markIndex++;

			// only show the flag on current map
			CECInstance* pTargetInst = g_pGame->GetGameRun()->GetInstance(coord.strMap);
			ASSERT(pTargetInst);
			int targetInstID = !pTargetInst ? MAJOR_MAP : pTargetInst->GetID();
			if(targetInstID != idInstance)
				continue;

			// should show the flag
			bShow = true;

			// bind and jump to next flag
			pImage->SetData(markIndex-1, "PlayerMarkIndex");
			flagIndex++;

			SIZE sz = pShow->GetSize();
			A3DPOINT2 absPos = conv(coord.vPos);
			A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
			pShow->SetPosEx(absPos.x - p->X - sz.cx/2, absPos.y - p->Y - sz.cy/2);

			pImage->SetHint(pDlgTask->GetTraceName());
			pShow->SetCanMove(false);
		}

		bShow &= bMapShow;
		if(pShow->IsShow() != bShow) pShow->Show(bShow);
	}

	m_nCurFlagIndex = -1;
	delete &conv;
}

void CDlgWorldMap::OnEventLButtonUp_Flag(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnFlagSelected();

	CDlgMiniMap* pDlgMini = GetGameUIMan()->m_pDlgMiniMap;
	CECMapDlgsMgr* pMgr = GetGameUIMan()->GetMapDlgsMgr();
	int markIndex = pMgr->GetCurMark();
	if(markIndex >= 0 && markIndex < (int)pDlgMini->GetMarks().size())
	{
		// only update non-NPC flag
		const CDlgMiniMap::MARK& mark = pDlgMini->GetMarks()[markIndex];
		if(mark.nNPC < 0)
		{
			// get current flag location
			POINT ptPos = GetPos();

			// select proper converter
			CECMapDlgsMgr::PosConvertor& conv = *pMgr->GetCurConvertor();

			// reverse convert the point to mark location
			A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
			A3DPOINT2 absPos(ptPos.x + p->X, ptPos.y + p->Y + GetSize().cy);
			A3DVECTOR3 markPos = conv(absPos);

			// update the mark location
			pDlgMini->SetMarkPos(markIndex, markPos);

			delete &conv;
		}
	}
}

void CDlgWorldMap::OnEventLButtonDblClk_Flag(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnFlagSelected();

	// move to this place
	int curMark = GetGameUIMan()->GetMapDlgsMgr()->GetCurMark();
	if(curMark >= 0 && curMark < (int)GetGameUIMan()->m_pDlgMiniMap->GetMarks().size())
	{
		const CDlgMiniMap::MARK& mark = GetGameUIMan()->m_pDlgMiniMap->GetMarks()[curMark];
		CECUIHelper::AutoMoveStartComplex(mark.vecPos);
	}
}

void CDlgWorldMap::OnFlagSelected()
{
	// cancel selection on last flag
	PAUIIMAGEPICTURE pImage = GetGameUIMan()->GetMapDlgsMgr()->GetCurFlagImage();
	if( pImage ) pImage->FixFrame(0);
	
	// get flag index from dialog name
	int flagIndex = atoi(GetName() + strlen("Flag")) - 1;
	GetGameUIMan()->GetMapDlgsMgr()->SetCurFlagIndex(flagIndex);

	// mark selection on current flag
	pImage = GetGameUIMan()->GetMapDlgsMgr()->GetCurFlagImage();
	if( pImage ) pImage->FixFrame(1);
}

void CDlgWorldMap::OnEventMouseMove_World(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int i, x2, y2;
	POINT ptPos = GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	SIZE s = m_pImgHint->GetSize();

	const CECMapDlgsMgr::Points& vecTeamMate = GetGameUIMan()->GetMapDlgsMgr()->GetTeamMate();
	m_pImgHint->SetHint(_AL(""));
	for( i = 0; i < (int)vecTeamMate.size(); i++ )
	{
		x2 = (int)vecTeamMate[i].vecPos.x;
		y2 = (int)vecTeamMate[i].vecPos.y;
		if( abs(x - x2) <= 7 && abs(y - y2) <= 7 )
		{
			m_pImgHint->SetHint(vecTeamMate[i].strName);
			m_pImgHint->SetPos(x - ptPos.x - s.cx / 2, y - ptPos.y - s.cy / 2);
			ChangeFocus(m_pImgHint);
			break;
		}
	}
}

void CDlgWorldMap::OnEventMouseMove_Control(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!GetGameUIMan()->GetMapDlgsMgr()->IsMarking()) {
		size_t count = m_Controls.size();
		bool bMouseOnAutoMoveButton(false);
		for (size_t i = 0; i < count; ++i) {
			const DynamicControl& entry = m_Controls[i];
			if (entry.control == pObj) {
				bMouseOnAutoMoveButton = true;
				break;
			}
		}
		GetGame()->ChangeCursor(bMouseOnAutoMoveButton ? RES_CUR_HAND : RES_CUR_NORMAL);
	}
}

void CDlgWorldMap::OnEventLButtonUp_World(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nInstanceID = GetWorld()->GetInstanceID();
	if(!GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstanceID))
		return;
	
	A3DRECT rcItem = pObj->GetRect();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - rcItem.CenterPoint().x - p->X;
	int y = GET_Y_LPARAM(lParam) - rcItem.CenterPoint().y - p->Y;

	APointF ptClick = GetGameUIMan()->GetMapDlgsMgr()->GetPosByPixels(x, y, nInstanceID);
	float fX = ptClick.x;
	float fZ = ptClick.y;

	if( GetGameUIMan()->GetMapDlgsMgr()->IsMarking() )
	{
		GetGameUIMan()->GetMapDlgsMgr()->SetMarking(false);

		GetGameUIMan()->m_vecTargetPos.x = fX;
		GetGameUIMan()->m_vecTargetPos.y = 0.0f;
		GetGameUIMan()->m_vecTargetPos.z = fZ;

		GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_MAKE_MARK);
	}
	else if( wParam & MK_CONTROL )
	{
		OnCommand_CANCEL("IDCANCEL");
		GetGameSession()->c2s_CmdGoto(fX, 1.0f, fZ);
	}
	else if (AUI_PRESS(VK_LMENU) || AUI_PRESS(VK_RMENU) || AUI_PRESS(VK_MENU))
	{
		CECUIHelper::AutoMoveStartComplex(A3DVECTOR3(fX, 0.0f, fZ));
	}
	else if(nInstanceID == MAJOR_MAP)
	{
		// try to move the detail map to correct position
		if(GetGameUIMan()->GetMapDlgsMgr()->InitDetailMap(fX, fZ))
		{
			Show(false);
			GetGameUIMan()->m_pDlgWorldMapDetail->Show(true);
		}
	}
	if (!m_Controls.empty())
		ChangeFocus(m_Controls[0].control);
}

void CDlgWorldMap::OnEventRButtonUp_World(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( GetGameUIMan()->GetMapDlgsMgr()->IsMarking() )
		GetGameUIMan()->GetMapDlgsMgr()->SetMarking(false);
	else
		GetGameUIMan()->GetMapDlgsMgr()->ShowWorldMap(false);
}

void CDlgWorldMap::OnEventMouseMove_Travel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ACString strHint;
	POINT ptPos = GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	SIZE s = m_pImgHint->GetSize();
	A3DRECT rcItem = GetDlgItem("WorldMapTravel")->GetRect();
	A3DPOINT2 ptCenter = rcItem.CenterPoint();

	m_pImgHint->SetData(-1);
	m_pImgHint->SetHint(_AL(""));

	CECMapDlgsMgr* pMgr = GetGameUIMan()->GetMapDlgsMgr();

	pMgr->ResetTransWay();

	int worldid = GetTravelWorldID();

	const CECMapDlgsMgr::PointMap& transPoints = pMgr->GetTransPoint();
	CECMapDlgsMgr::PointMap::const_iterator itr;
	for( itr = transPoints.begin(); itr != transPoints.end(); ++itr)
	{
		const CECMapDlgsMgr::TRANS_POINT& tp = itr->second;
		if (tp.worldid != worldid)	continue;

		A3DPOINT2 tpPos = ptCenter + pMgr->GetPixelsByPos(tp.vecPos.x, tp.vecPos.z, tp.worldid);
		if( abs(x - tpPos.x) > 5 || abs(y - tpPos.y) > 5 ) continue;

		m_pImgHint->SetPos(x - ptPos.x - s.cx / 2, y - ptPos.y - s.cy / 2);
		m_pImgHint->SetData(tp.id);

		if (pMgr->UpdateTransWay(GetCurNPCEssence(), tp.id))
		{
			const CECMapDlgsMgr::TransWay &transWay = pMgr->GetTransWay();
			if (transWay.mode == CECMapDlgsMgr::TransWay::TM_NPC_TRANSMIT_NORMAL ||
				transWay.mode == CECMapDlgsMgr::TransWay::TM_NPC_TRANSMIT_DIRECT)
			{
				strHint.Format(GetStringFromTable(546), tp.strName, transWay.nCost, transWay.nLevel);
			}
			else strHint = tp.strName;
		}
		else strHint = tp.strName;
		if (	g_pGame->GetConfigs()->GetShowIDFlag())
		{
			ACString strID;
			strID.Format(_AL("\r^ff0000id = %d"), tp.id);
			strHint += strID;
		}
		m_pImgHint->SetHint(strHint);

		ChangeFocus(m_pImgHint);
		break;
	}
}

void CDlgWorldMap::OnEventLButtonUp_Travel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int id = (int)m_pImgHint->GetData();
	if( id >= 0 )
	{
		int worldid = GetGameUIMan()->GetMapDlgsMgr()->FindTransmitWorldID(id);
		CECInstance* pInst = GetGameRun()->GetInstance(worldid);
		if (!pInst)
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(10701), MB_OK, A3DCOLORRGB(255, 255, 255));
			OnCommand_CANCEL("IDCANCEL");
			return;
		}

		if( GetData() == DT_NPC_TRANSMIT_SERVICE )
		{
			const NPC_ESSENCE *pNPC = GetCurNPCEssence();
			ASSERT(pNPC != NULL);
			CECMapDlgsMgr *pMgr = GetGameUIMan()->GetMapDlgsMgr();
			if (pMgr->UpdateTransWay(pNPC, id))
			{
				const CECMapDlgsMgr::TransWay &transWay = pMgr->GetTransWay();
				if (transWay.mode == CECMapDlgsMgr::TransWay::TM_NPC_TRANSMIT_NORMAL)
				{
					int nIndex = GetTransIndex(pNPC, id);
					if (nIndex >= 0)
					{
						GetGameSession()->c2s_CmdNPCSevTransmit(nIndex);
						OnCommand_CANCEL("IDCANCEL");
						return;
					}
				}
				else if (transWay.mode == CECMapDlgsMgr::TransWay::TM_NPC_TRANSMIT_DIRECT)
				{
					const CECMapDlgsMgr::TransWayPoints &transWayPoints = transWay.wayPoints;
					if (pNPC && pNPC->id_to_discover != 0 && transWayPoints.GetCount()>0)
					{
						abase::vector<int> aWayPoints;

						aWayPoints.push_back(pNPC->id_to_discover);	//	压入起点
						ALISTPOSITION pos = transWayPoints.GetHeadPosition();
						while(pos)
						{ 
							CECMapDlgsMgr::TRANS_POINT* tp = transWayPoints.GetNext(pos);
							aWayPoints.push_back(tp->id);
						}

						GetGameSession()->c2s_CmdNPCSevTransmitDirect((int)aWayPoints.size(), &aWayPoints.front());
						OnCommand_CANCEL("IDCANCEL");
						return;
					}
				}
			}
		}
		else if(GetData() == DT_TRANSMITSCROLL_ESSENCE)
		{
			// item transmit
			unsigned short slot = (unsigned short)GetDataPtr("ItemSlot");
			CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(slot);
			GetGameSession()->c2s_CmdUseItemWithArg(IVTRTYPE_PACK, 1, slot, pItem->GetTemplateID(), &id, sizeof(id));
			OnCommand_CANCEL("IDCANCEL");
		}
		else
		{
			// invalid transmit type
			ASSERT(false);
		}
	}
}

void CDlgWorldMap::OnEventRButtonUp_Travel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommand_CANCEL("IDCANCEL");
}

void CDlgWorldMap::OnEventMouseMove_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !(wParam & MK_LBUTTON) )
	{
		int i, x2, y2;
		A3DVECTOR3 vecPos;
		POINT ptPos = GetPos();
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		int x = GET_X_LPARAM(lParam) - p->X;
		int y = GET_Y_LPARAM(lParam) - p->Y;
		SIZE s = m_pImgHint->GetSize();

		const CECMapDlgsMgr::Points& vecTeamMate = GetGameUIMan()->GetMapDlgsMgr()->GetTeamMate();

		m_pImgHint->SetHint(_AL(""));
		for( i = 0; i < (int)vecTeamMate.size(); i++ )
		{
			x2 = (int)vecTeamMate[i].vecPos.x;
			y2 = (int)vecTeamMate[i].vecPos.y;
			if( abs(x - x2) <= 7 && abs(y - y2) <= 7 )
			{
				m_pImgHint->SetHint(vecTeamMate[i].strName);
				m_pImgHint->SetPos(x - ptPos.x - s.cx / 2, y - ptPos.y - s.cy / 2);
				ChangeFocus(m_pImgHint);
				break;
			}
		}
	}
	else
	{
		GetGameUIMan()->GetMapDlgsMgr()->RefreshDetailMap();
		GetGameUIMan()->GetMapDlgsMgr()->RefreshWorldMapFlags(false);
		UpdateDetailMapControlPos();
	}
}

void CDlgWorldMap::OnEventLButtonDown_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGame()->ChangeCursor(RES_CUR_HAND);
	GetGameUIMan()->GetMapDlgsMgr()->SetDraging(true);
}

void CDlgWorldMap::OnEventLButtonUp_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SIZE s = pObj->GetSize();
	POINT ptPos = pObj->GetPos();
	int idObj = atoi(pObj->GetName() + strlen("Part")) - 1;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int xDelta = (GET_X_LPARAM(lParam) - ptPos.x - p->X) * 1024 / s.cx;
	int yDelta = (GET_Y_LPARAM(lParam) - ptPos.y - p->Y) * 1024 / s.cy;
	float fX = GetGameUIMan()->GetMapDlgsMgr()->GetCorner(idObj).x * 1024.0f - 4096.0f + float(xDelta);
	float fZ = 5632.0f - GetGameUIMan()->GetMapDlgsMgr()->GetCorner(idObj).y * 1024.0f - float(yDelta);
	if (AUI_PRESS(VK_LMENU) || AUI_PRESS(VK_RMENU) || AUI_PRESS(VK_MENU))
	{
		CECUIHelper::AutoMoveStartComplex(A3DVECTOR3(fX, 0.0f, fZ));
	}
	if( GetGameUIMan()->GetMapDlgsMgr()->IsMarking() )
	{
		GetGameUIMan()->GetMapDlgsMgr()->SetMarking(false);

		GetGameUIMan()->m_vecTargetPos.x = fX;
		GetGameUIMan()->m_vecTargetPos.y = 0.0f;
		GetGameUIMan()->m_vecTargetPos.z = fZ;

		GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_MAKE_MARK);
	}
	else if( wParam & MK_CONTROL )
	{
		OnCommand_CANCEL("IDCANCEL");
		GetGameSession()->c2s_CmdGoto(fX, 1.0f, fZ);
	}

	GetGameUIMan()->GetMapDlgsMgr()->SetDraging(false);
	ChangeFocus(m_pImgHint);
}

void CDlgWorldMap::OnEventRButtonUp_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( GetGameUIMan()->GetMapDlgsMgr()->IsMarking() )
	{
		GetGameUIMan()->GetMapDlgsMgr()->SetMarking(false);
		GetGameUIMan()->GetMapDlgsMgr()->SetDraging(false);
	}
	else
	{
		Show(false);
		GetGameUIMan()->m_pDlgWorldMap->Show(true);
	}
}

void CDlgWorldMap::OnEventLButtonDblClk_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SIZE s = pObj->GetSize();
	POINT ptPos = pObj->GetPos();
	int idObj = atoi(pObj->GetName() + strlen("Part")) - 1;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int xDelta = (GET_X_LPARAM(lParam) - ptPos.x - p->X) * 1024 / s.cx;
	int yDelta = (GET_Y_LPARAM(lParam) - ptPos.y - p->Y) * 1024 / s.cy;
	float fX = GetGameUIMan()->GetMapDlgsMgr()->GetCorner(idObj).x * 1024.0f - 4096.0f + float(xDelta);
	float fZ = 5632.0f - GetGameUIMan()->GetMapDlgsMgr()->GetCorner(idObj).y * 1024.0f - float(yDelta);	
	CECUIHelper::AutoMoveStartComplex(A3DVECTOR3(fX, 0.0f, fZ));
}

void CDlgWorldMap::OnEventLButtonUp_Sign(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (strstr(m_szName, "Sign"))
	{
		int index = m_szName.Right(1).ToInt() - 1;
		if (index >=0 && index < (int)GetGameUIMan()->m_pDlgTask->GetObjectCoords().size())
		{
			A3DVECTOR3 vPos = GetGameUIMan()->m_pDlgTask->GetObjectCoords()[index].vPos;
			CECUIHelper::AutoMoveStartComplex(vPos);
		}
	}
}

void CDlgWorldMap::BringUpTools()
{
	int nInstanceID = GetWorld()->GetInstanceID();
	if(!GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstanceID))
		return;

	PAUIDIALOG pDlg = NULL;
	
	pDlg = GetGameUIMan()->m_pDlgInputName;
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);
	
	pDlg = GetGameUIMan()->GetDialog("Win_AutoMove");
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);

	pDlg = GetGameUIMan()->GetDialog("Win_HideMove");
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);

	pDlg = GetGameUIMan()->GetDialog("mMark6");
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);
	
	pDlg = GetGameUIMan()->GetDialog("Win_BigMapMark");
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);
	
	pDlg = GetGameUIMan()->GetDialog("Win_Bigmap");
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);
	
	char szName[40];
	int i(0);
	for(i = 0; i < CECGAMEUIMAN_MAX_MARKS; i++ )
	{
		sprintf(szName, "Flag%d", i + 1);
		pDlg = GetGameUIMan()->GetDialog(szName);
		if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);
	}

	for( i = 0; i < CECGAMEUIMAN_MAX_MARKS; i++ )
	{
		sprintf(szName, "Sign%d", i + 1);
		pDlg = GetGameUIMan()->GetDialog(szName);
		if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg, this);
	}

	pDlg = GetGameUIMan()->GetDialog("Win_HistoryPro");
	if(pDlg && pDlg->IsShow()) GetGameUIMan()->InsertDialogBefore(pDlg,this);
}

int CECMapDlgsMgr::FindTransmitWorldID(NPC_TRANSMIT_SERVICE* pData)
{
	int worldid = 0;
	ASSERT(0 < MAJOR_MAP);

	const abase::vector<TRANS_TARGET>& vecTarget = *globaldata_gettranstargets();
	for(int i = 0; pData && i < GetTransmitTargetsNum(pData); i++ )
	{
		int idThis = pData->targets[i].idTarget;
		if(idThis > 0)
		{
			// select the first valid id as current world id
			for(int j = 0; worldid == 0 && j < (int)vecTarget.size(); j++ )
			{
				// find the correct target (may not active)
				if( idThis == vecTarget[j].id )
				{
					worldid = vecTarget[j].world_id;
					break;
				}
			}
		}
	}

	return worldid != 0 ? worldid : MAJOR_MAP;
}

int CECMapDlgsMgr::FindTransmitWorldID(int idTarget)
{
	int worldid = 0;
	if (idTarget > 0){
		const abase::vector<TRANS_TARGET>& vecTarget = *globaldata_gettranstargets();
		for(int j = 0; j < (int)vecTarget.size(); j++ )
		{
			// find the correct target (may not active)
			if( idTarget == vecTarget[j].id )
			{
				worldid = vecTarget[j].world_id;
				break;
			}
		}
	}
	return worldid != 0 ? worldid : MAJOR_MAP;
}

void CDlgWorldMap::BuildTravelMap(DWORD dwType, void* pData)
{
	// default world id
	int worldid = MAJOR_MAP;

	// select the correct map background
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("WorldMapTravel");

	// prepare the necessary information
	SetData(dwType);

	CECMapDlgsMgr *pMgr = GetGameUIMan()->GetMapDlgsMgr();

	//	重新开始计算传送线路
	pMgr->ResetTransWay();

	if(dwType == DT_NPC_TRANSMIT_SERVICE)
	{
		worldid = pMgr->FindTransmitWorldID((NPC_TRANSMIT_SERVICE*)pData);

		// save the worldid
		SetDataPtr((void*)worldid, "WorldID");

		pMgr->LoadMapTexture(m_pA3DDevice, pImage, CECMapDlgsMgr::MAP_TYPE_TRANSMIT);
	}
	else if(dwType == DT_TRANSMITSCROLL_ESSENCE)
	{
		// save the slot info
		SetDataPtr(pData, "ItemSlot");

		pMgr->LoadMapTexture(m_pA3DDevice, pImage, CECMapDlgsMgr::MAP_TYPE_SCROLL);
	}
	else
	{
		// invalid transmit type
		ASSERT(false);
	}
}

CECMapDlgsMgr::CECMapDlgsMgr()
:m_nCurFlagIndex(-1)
,m_bMarking(false)
,m_bMapDragging(false)
,m_pA2DSpriteWaypoint(NULL)
,m_pA2DSpriteProfession(NULL)
,m_nMinSize(0)
,m_WideMode(false)
,m_pTransCache(0)
{
	m_MapMap.clear();
	m_vecTeamMate.clear();
	m_transPoints.clear();
}

CECMapDlgsMgr::~CECMapDlgsMgr()
{
	abase::hash_map<AString, A3DTexture *>::iterator it;
	
	if( m_MapMap.size() > 0 )
	{
		for( it = m_MapMap.begin(); it != m_MapMap.end(); ++it )
			A3DRELEASE(it->second);
		m_MapMap.clear();
	}
	MAP_PARAM::iterator iterParam;
	for (iterParam = m_MapParam.begin(); iterParam != m_MapParam.end(); ++iterParam)
		delete iterParam->second;
	m_MapParam.clear();
	A3DRELEASE(m_pA2DSpriteWaypoint);
	A3DRELEASE(m_pA2DSpriteProfession);

	delete m_pTransCache;
	m_pTransCache = NULL;
}

bool CECMapDlgsMgr::Init()
{
	bool bval;
	A3DRECT *rc;
	int i, j, W, H, nNumX, nNumY, nIndex;

	if( !m_pA2DSpriteWaypoint )
	{
		// Waypoint icons.
		m_pA2DSpriteWaypoint = new A2DSprite;
		if( !m_pA2DSpriteWaypoint ) return AUI_ReportError(__LINE__, __FILE__);
		
		bval = m_pA2DSpriteWaypoint->Init(g_pGame->GetA3DDevice(),
			"InGame\\WayPoint.dds", AUI_COLORKEY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		
		W = H = 16;
		nNumX = m_pA2DSpriteWaypoint->GetWidth() / W;
		nNumY = m_pA2DSpriteWaypoint->GetHeight() / H;
		m_pA2DSpriteWaypoint->SetLocalCenterPos(W / 2, H / 2);
		
		rc = (A3DRECT*)a_malloctemp(sizeof(A3DRECT) * (nNumX * nNumY));
		for( i = 0; i < nNumY; i++ )
		{
			for( j = 0; j < nNumX; j++ )
			{
				nIndex = i * nNumX + j;
				rc[nIndex].SetRect(j * W, i * H, j * W + W, i * H + H);
			}
		}
		m_pA2DSpriteWaypoint->ResetItems(nNumX * nNumY, rc);
		m_pA2DSpriteWaypoint->SetLinearFilter(true);
		a_freetemp(rc);
	}
	
	if( !m_pA2DSpriteProfession )
	{
		// Profession icons.
		m_pA2DSpriteProfession = new A2DSprite;
		if( !m_pA2DSpriteProfession ) return AUI_ReportError(__LINE__, __FILE__);
		
		bval = m_pA2DSpriteProfession->Init(g_pGame->GetA3DDevice(),
			"InGame\\Profession.tga", AUI_COLORKEY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		
		// make sure the image layout is 1 x (NUM_PROFESSION * NUM_GENDER)
		// and sign will auto fit the icon size.
		nNumX = 1;
		nNumY = NUM_PROFESSION * NUM_GENDER;
		W = m_pA2DSpriteProfession->GetWidth() / nNumX;
		H = m_pA2DSpriteProfession->GetHeight() / nNumY;
		
		rc = (A3DRECT*)a_malloctemp(sizeof(A3DRECT) * (nNumX * nNumY));
		for( i = 0; i < nNumY; i++ )
		{
			for( j = 0; j < nNumX; j++ )
			{
				nIndex = i * nNumX + j;
				rc[nIndex].SetRect(j * W, i * H, j * W + W, i * H + H);
			}
		}
		m_pA2DSpriteProfession->ResetItems(nNumX * nNumY, rc);
		a_freetemp(rc);
	}

	if(!m_pTransCache)
	{
		m_pTransCache = new CECNPCTransmitCache();
	}
	MAP_PARAM::iterator iterParam;
	for (iterParam = m_MapParam.begin(); iterParam != m_MapParam.end(); ++iterParam)
		delete iterParam->second;
	m_MapParam.clear();
	AWIniFile file;
	if (file.Open("configs\\map_param.ini"))
	{
		PosConvertParam pTemp;
		int world_id;
		ACString strSect;
		ACString strTemp;
		int i(0);
		while (true)
		{
			strSect.Format(_AL("MapConvertParam%d"), i++);		
			
			if ((world_id = file.GetValueAsInt(strSect, _AL("id"), -1)) == -1) break;
			if ((pTemp.a = file.GetValueAsInt(strSect, _AL("a"), -1)) == -1) break;
			if ((pTemp.b = file.GetValueAsInt(strSect, _AL("b"), -1)) == -1) break;
			if ((pTemp.ox = file.GetValueAsFloat(strSect, _AL("ox"), -1.f)) == -1.f) break;
			if ((pTemp.oy = file.GetValueAsFloat(strSect, _AL("oy"), -1.f)) == -1.f) break;
			if ((pTemp.w = file.GetValueAsInt(strSect, _AL("w"), -1)) == -1) break;
			if ((pTemp.h = file.GetValueAsInt(strSect, _AL("h"), -1)) == -1) break;
			PosConvertParam* pParam = new PosConvertParam;
			if (pParam == NULL) break;
			memcpy(pParam, &pTemp, sizeof(PosConvertParam));
			if (m_MapParam.find(world_id) == m_MapParam.end())
				m_MapParam[world_id] = pParam;
			else {
				ASSERT(!"world id overlap in map_param.ini");
				delete pParam;
				break;
			}
		}
		file.Close();
	}
	return true;
}
bool CECMapDlgsMgr::IsMapCanShowHostPos(int worldid)
{
	return m_MapParam.find(worldid) != m_MapParam.end();
}
void CECMapDlgsMgr::GetPosConvertParam(int worldid, PosConvertParam& param)
{
	if (m_MapParam.find(worldid) == m_MapParam.end()){
		CECInstance *pInstance =g_pGame->GetGameRun()->GetInstance(worldid);
		param.a = pInstance->GetColNum();
		param.b = pInstance->GetRowNum();
		param.ox = 0.f;
		param.oy = 0.f;
		param.w = 768;
		param.h = 768;
	} else {
		param.a = m_MapParam[worldid]->a;
		param.b = m_MapParam[worldid]->b;
		param.ox = m_MapParam[worldid]->ox;
		param.oy = m_MapParam[worldid]->oy;
		param.w = m_MapParam[worldid]->w;
		param.h = m_MapParam[worldid]->h;
	}
}
// GetPixelsByPos：把世界坐标转换到屏幕坐标
// 已知：	副本由1024x1024的方块组成，屏幕上的地图横向能容下a个方块，纵向b个(如：世界地图为11x11)
//			地图中心点的屏幕坐标为(cx,cy)
//			地图中心点对应的世界坐标为(ox,oy)
//          地图图片上用于显示地图的分辨率大小为w*h
//			玩家世界水平面坐标(x,y) (这里用y代表z)
//          能用于显示地图的最大高度min(m_nMinSize)
//          正方形副本的地图为分辨率1024*1024的正方形图片；为适应窗口1024*768的分辨率，中间768长度的部分为地图内容，上下为黑边；
//			美术还要在地图左右两边加上装饰，实际地图的边长最大为768
//
// 求解:	玩家位置对应的屏幕坐标(sx,sy)
//
// 地图在屏幕中实际的大小为 W*H
//   W     min*4/3     H     min
//  --- = ---------， --- = -----
//   w      1024       h     768
//
// sx  :      cx  -->  ox
//		   cx+W/2 --> (a%2)*512+(a/2)*1024+ox
// 故有:			
//			(sx-cx)     	  (x-ox)
//          ------- = ----------------------
//            W/2      (a%2)*512+(a/2)*1024
//
// sy :       cy -->   oy
//		  cy+H/2 --> -((b%2)*512+(b/2)*1024+oy)
//
//			(sy-cy)     	 -(y-oy)
//          ------- = ----------------------
//            H/2      (b%2)*512+(b/2)*1024  
//
A3DPOINT2 CECMapDlgsMgr::GetPixelsByPos(float x, float y, int worldid)
{
	PosConvertParam param;
	memset(&param, 0, sizeof(param));
	GetPosConvertParam(worldid, param);

	int W,H;

	W = m_nMinSize * 4 / 3 * param.w / MAP_DEFAULT_WIDTH;
	H = m_nMinSize * param.h / MAP_DEFAULT_HEIGHT;
	
	int sx = int((x - param.ox) * W / (((param.a & 1) ? 1024 : 0) + (param.a >> 1) * 2048));
	int sy = int((param.oy - y) * H / (((param.b & 1) ? 1024 : 0) + (param.b >> 1) * 2048));
	return A3DPOINT2(sx, sy);
}

APointF CECMapDlgsMgr::GetPosByPixels(int x, int y, int worldid)
{
	PosConvertParam param;
	memset(&param, 0, sizeof(param));
	GetPosConvertParam(worldid, param);

	int W,H;

	W = m_nMinSize * 4 / 3 * param.w / MAP_DEFAULT_WIDTH;
	H = m_nMinSize * param.h / MAP_DEFAULT_HEIGHT;
	
	float wx = param.ox + (((param.a & 1) ? 1024 : 0) + (param.a >> 1) * 2048) * x / W;
	float wy = param.oy - (((param.b & 1) ? 1024 : 0) + (param.b >> 1) * 2048) * y / H;
	return APointF(wx, wy);
}

void CECMapDlgsMgr::RefreshDetailMapTexture()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_WorldMapDetail");

	int i;
	bool bLoadNewMap = false;
	for( i = MINIMAP_UL; i < MINIMAP_MAX; i++ )
	{
		AString strIndex;
		strIndex.Format("Surfaces\\Maps\\%02d%d.dds", GetCorner(i).y, GetCorner(i).x);
	
		A3DTexture *pMap = NULL;
		if( !m_MapMap[strIndex] )
		{
			pMap = new A3DTexture;
			pMap->SetNoDownSample(true);
			pMap->LoadFromFile(g_pGame->GetA3DDevice(), strIndex, strIndex);
			
			m_MapMap[strIndex] = pMap;
			bLoadNewMap = true;
		}
		else
			pMap = m_MapMap[strIndex];

		strIndex.Format("Part%d", i + 1);
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem(strIndex);
		pImage->SetTexture(pMap);
	}
	
	if( bLoadNewMap )
	{
		bool bNoUse;
		AString strIndex;
		abase::hash_map<AString, A3DTexture *>::iterator it;
		for( it = m_MapMap.begin(); it != m_MapMap.end(); )
		{
			bNoUse = true;
			for( i = MINIMAP_UL; i < MINIMAP_MAX; i++ )
			{
				strIndex.Format("Surfaces\\Maps\\%02d%d.dds", GetCorner(i).y, GetCorner(i).x);
				if( 0 == strcmp(strIndex, it->first) )
				{
					bNoUse = false;
					break;
				}
			}
			if( bNoUse )
			{
				A3DRELEASE(it->second);
				it = m_MapMap.erase(it);
			}
			else
				++it;
		}
	}
}

void CECMapDlgsMgr::ClearDetailMap()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_WorldMapDetail");

	char szName[MAX_PATH];
	for(int i = MINIMAP_UL; i < MINIMAP_MAX; i++ )
	{
		sprintf(szName, "Part%d", i + 1);
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem(szName);
		pImage->SetTexture(NULL);
	}
	
	abase::hash_map<AString, A3DTexture *>::iterator it;
	for( it = m_MapMap.begin(); it != m_MapMap.end(); ++it )
		A3DRELEASE(it->second);
	m_MapMap.clear();
}

void CECMapDlgsMgr::DrawTeamMark(const CECMapDlgsMgr::PosConvertor& conv)
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	CECHostPlayer* pHostPlayer = g_pGame->GetGameRun()->GetHostPlayer();

	int i;
	TRANS_POINT tp;
	CECTeamMember *pMember;
	CECTeam *pTeam = pHostPlayer->GetTeam();
	A3DRECT rcSize = m_pA2DSpriteProfession->GetItem(0)->GetRect();
	
	m_vecTeamMate.clear();
	for( i = 0; pTeam && i < pTeam->GetMemberNum(); i++ )
	{
		pMember = pTeam->GetMemberByIndex(i);
		if (!pMember->IsSameInstanceTeamMember())
			continue;
		
		A3DVECTOR3 vecPos = pMember->GetPos();
		A3DPOINT2 pos = conv(vecPos);
		
		m_pA2DSpriteProfession->SetCurrentItem(pMember->GetProfession());
		m_pA2DSpriteProfession->DrawToBack(pos.x - rcSize.Width() / 2, pos.y - rcSize.Height() / 2);
		
		tp.strName = pMember->GetName();
		tp.vecPos.x = float(pos.x);
		tp.vecPos.y = float(pos.y);
		m_vecTeamMate.push_back(tp);
	}
}

void CECMapDlgsMgr::UpdateWayPoints(WORD* pData, int iSize, bool bClear)
{
	const abase::vector<TRANS_TARGET>& vecTarget = *globaldata_gettranstargets();

	if(bClear)
	{
		m_transPoints.clear();
	}

	// got valid points.
	for(int i = 0; i < iSize; i++ )
	{
		int idThis = *(pData+i);
		for(int j = 0; j < (int)vecTarget.size(); j++ )
		{
			if( idThis != vecTarget[j].id ) continue;

			CECMapDlgsMgr::TRANS_POINT tp;

			tp.id = idThis;
			tp.strName = vecTarget[j].name;
			tp.vecPos = vecTarget[j].vecPos;
			tp.worldid = vecTarget[j].world_id;

			m_transPoints[idThis] = tp;
			break;
		}
	}
}

void CECMapDlgsMgr::DrawWayPoints(const NPC_ESSENCE *pNPC, const A3DPOINT2& ptCenter, int iWorldID, bool bDrawLines)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	
	if(!pNPC && bDrawLines)
	{
		// travel line should only be drawn when NPC service opened
		return;
	}

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	A3DPOINT2 absCenter= ptCenter + A3DPOINT2(p->X, p->Y);
	
	int idCur = pNPC ? pNPC->id_to_discover : -1;
	
	// draw valid lines
	if( bDrawLines )
	{
		DATA_TYPE DataType;		
		NPC_TRANSMIT_SERVICE *pData = (NPC_TRANSMIT_SERVICE *)g_pGame->GetElementDataMan()->
			get_data_ptr(pNPC->id_transmit_service, ID_SPACE_ESSENCE, DataType);

		// ignore the invalid service data
		if( DataType != DT_NPC_TRANSMIT_SERVICE )
		{
			ASSERT(false);
			return;
		}
		
		// start point is current location of the player or NPC
		A3DVECTOR3 vecPos = pHost->GetPos();
		PointMap::const_iterator itr = GetTransPoint().find(idCur);
		if(itr != GetTransPoint().end())
		{
			vecPos = itr->second.vecPos;
		}

		A3DPOINT2 startPos = absCenter + GetPixelsByPos(vecPos.x, vecPos.z, iWorldID);

		for(int j = 0; j < GetTransmitTargetsNum(pData); j++ )
		{
			PointMap::const_iterator itr = GetTransPoint().find(pData->targets[j].idTarget);

			if(itr != GetTransPoint().end())
			{
				const TRANS_POINT& tp = itr->second;

				// ignore the invalid point
				if(tp.worldid != iWorldID) continue;
				
				A3DPOINT2 endPos = absCenter + GetPixelsByPos(tp.vecPos.x, tp.vecPos.z, tp.worldid);
				bool bval = g_pA3DGDI->Draw2DLine(startPos, endPos, A3DCOLORRGB(255, 255, 255));
				if(!bval) AUI_ReportError(__LINE__, 1, "CECMapDlgsMgr::DrawWayPoints(), failed in line drawing");
			}
		}
	}

	// draw way points
	for(PointMap::const_iterator itr = GetTransPoint().begin(); itr != GetTransPoint().end(); ++itr)
	{
		const TRANS_POINT& tp = itr->second;

		// ignore the invalid point
		if(tp.worldid != iWorldID) continue;

		A3DPOINT2 markPos = absCenter + GetPixelsByPos(tp.vecPos.x, tp.vecPos.z, tp.worldid);
		if( idCur == tp.id )
			m_pA2DSpriteWaypoint->SetCurrentItem(CECMapDlgsMgr::WAYPOINT_ME);
		else
			m_pA2DSpriteWaypoint->SetCurrentItem(CECMapDlgsMgr::WAYPOINT_TARGET);
		bool bval = m_pA2DSpriteWaypoint->DrawToBack(markPos.x, markPos.y);
		if( !bval ) AUI_ReportError(__LINE__, 1, "CECMapDlgsMgr::DrawWayPoints(), failed in point drawing");
	}
}

bool CECMapDlgsMgr::InitDetailMap(float fX, float fZ)
{
	POINT ptTarget;
	ptTarget.x = int((fX + 4096.0f) / 1024.0f);
	if( ptTarget.x < 0 || ptTarget.x > 7 ) return false;

	ptTarget.y = int((5632.0f - fZ) / 1024.0f);
	if( ptTarget.y < 0 || ptTarget.y > 10 ) return false;
	
	int nDeltaX = int(fX + 4096.0f - ptTarget.x * 1024.0f);
	int nDeltaY = int(5632.0f - fZ - ptTarget.y * 1024.0f);

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_WorldMapDetail");
	SIZE s = pDlgDetail->GetSize();

	POINT ptPos;
	if( ptTarget.x > 0 )
	{
		m_idxCorner[MINIMAP_UL].x = ptTarget.x - 1;
		m_idxCorner[MINIMAP_UR].x = ptTarget.x;
		m_idxCorner[MINIMAP_LL].x = ptTarget.x - 1;
		m_idxCorner[MINIMAP_LR].x = ptTarget.x;
		
		ptPos.x = p->Width / 2 - s.cx / 2 - nDeltaX;
	}
	else
	{
		m_idxCorner[MINIMAP_UL].x = ptTarget.x;
		m_idxCorner[MINIMAP_UR].x = ptTarget.x + 1;
		m_idxCorner[MINIMAP_LL].x = ptTarget.x;
		m_idxCorner[MINIMAP_LR].x = ptTarget.x + 1;
		
		ptPos.x = p->Width / 2 - nDeltaX;
	}
	if( ptTarget.y > 0 )
	{
		m_idxCorner[MINIMAP_UL].y = ptTarget.y - 1;
		m_idxCorner[MINIMAP_UR].y = ptTarget.y - 1;
		m_idxCorner[MINIMAP_LL].y = ptTarget.y;
		m_idxCorner[MINIMAP_LR].y = ptTarget.y;
		
		ptPos.y = p->Height / 2 - s.cy / 2 - nDeltaY;
	}
	else
	{
		m_idxCorner[MINIMAP_UL].y = ptTarget.y;
		m_idxCorner[MINIMAP_UR].y = ptTarget.y;
		m_idxCorner[MINIMAP_LL].y = ptTarget.y + 1;
		m_idxCorner[MINIMAP_LR].y = ptTarget.y + 1;
		
		ptPos.y = p->Height / 2 - nDeltaY;
	}

	pDlgDetail->SetPosEx(ptPos.x, ptPos.y);
	return true;
}

void CECMapDlgsMgr::RefreshDetailMap()
{
	A3DTexture *pTex;
	PAUIIMAGEPICTURE pImage;

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	POINT ptCenterWin = { p->Width / 2, p->Height / 2 };

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_WorldMapDetail");

	SIZE s = pDlgDetail->GetSize();
	POINT ptPos = pDlgDetail->GetPos();
	A3DRECT rcDlg = pDlgDetail->GetRect();
	A3DPOINT2 ptCenterDlg = rcDlg.CenterPoint();

	if( ptCenterWin.y <= ptCenterDlg.y - s.cy / 4 )
	{
		if( m_idxCorner[MINIMAP_UL].y > 0 )
		{
			ptPos.y -= s.cy / 2;

			m_idxCorner[MINIMAP_LL] = m_idxCorner[MINIMAP_UL];
			m_idxCorner[MINIMAP_UL].y--;
			m_idxCorner[MINIMAP_LR] = m_idxCorner[MINIMAP_UR];
			m_idxCorner[MINIMAP_UR].y--;

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part1");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part3");
			pImage->SetTexture(pTex);

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part2");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part4");
			pImage->SetTexture(pTex);
		}
		else if( rcDlg.top > 0 )
		{
			ptPos.y = 0;
		}
	}
	else if( ptCenterWin.y >= ptCenterDlg.y + s.cy / 4 )
	{
		if( m_idxCorner[MINIMAP_LL].y < 10 )
		{
			ptPos.y += s.cy / 2;

			m_idxCorner[MINIMAP_UL] = m_idxCorner[MINIMAP_LL];
			m_idxCorner[MINIMAP_LL].y++;
			m_idxCorner[MINIMAP_UR] = m_idxCorner[MINIMAP_LR];
			m_idxCorner[MINIMAP_LR].y++;

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part3");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part1");
			pImage->SetTexture(pTex);

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part4");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part2");
			pImage->SetTexture(pTex);
		}
		else if( rcDlg.bottom < (int)p->Height )
		{
			ptPos.y = p->Height - s.cy;
		}
	}
	if( ptCenterWin.x <= ptCenterDlg.x - s.cx / 4 )
	{
		if( m_idxCorner[MINIMAP_UL].x > 0 )
		{
			ptPos.x -= s.cx / 2;

			m_idxCorner[MINIMAP_UR] = m_idxCorner[MINIMAP_UL];
			m_idxCorner[MINIMAP_UL].x--;
			m_idxCorner[MINIMAP_LR] = m_idxCorner[MINIMAP_LL];
			m_idxCorner[MINIMAP_LL].x--;

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part1");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part2");
			pImage->SetTexture(pTex);

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part3");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part4");
			pImage->SetTexture(pTex);
		}
		else if( rcDlg.left > 0 )
		{
			ptPos.x = 0;
		}
	}
	else if( ptCenterWin.x >= ptCenterDlg.x + s.cx / 4 )
	{
		if( m_idxCorner[MINIMAP_UR].x < 7 )
		{
			ptPos.x += s.cx / 2;

			m_idxCorner[MINIMAP_UL] = m_idxCorner[MINIMAP_UR];
			m_idxCorner[MINIMAP_UR].x++;
			m_idxCorner[MINIMAP_LL] = m_idxCorner[MINIMAP_LR];
			m_idxCorner[MINIMAP_LR].x++;

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part2");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part1");
			pImage->SetTexture(pTex);

			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part4");
			pImage->GetTexture(&pTex);
			pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Part3");
			pImage->SetTexture(pTex);
		}
		else if( rcDlg.right < (int)p->Width )
		{
			ptPos.x = p->Width - s.cx;
		}
	}

	pDlgDetail->SetPosEx(ptPos.x, ptPos.y);

	RefreshDetailMapTexture();
}

struct TransPointCost
{
	typedef CECMapDlgsMgr::TRANS_POINT TP;
	
	const TP* p;
	unsigned int sum;
	int	required_level;
	int parent;

	TransPointCost()
		: p(0), sum(0), required_level(0), parent(-1){};

	TransPointCost(const TP& _tp, unsigned int _sum, int _required_level, int _parent)
		: p(&_tp), sum(_sum), required_level(_required_level), parent(_parent){};

	struct Less
	{
		bool operator()(const TransPointCost& lhs, const TransPointCost& rhs) const
		{
			return lhs.sum < rhs.sum;
		}
	};
};

bool CECMapDlgsMgr::UpdateTransWayWithinNPC(const NPC_ESSENCE *pNPC, int idTarget)
{
	//	只在 NPC 对应的传送服务里查找传送路径，不做图搜索
	while (true)
	{
		PointMap::const_iterator it = GetTransPoint().find(idTarget);
		if (it == GetTransPoint().end())
			break;

		if (pNPC->id_transmit_service <= 0)
			break;

		DATA_TYPE dataType = DT_INVALID;
		const NPC_TRANSMIT_SERVICE *pTransServ = (const NPC_TRANSMIT_SERVICE *)g_pGame->GetElementDataMan()->get_data_ptr(pNPC->id_transmit_service, ID_SPACE_ESSENCE, dataType);
		if (!pTransServ || dataType != DT_NPC_TRANSMIT_SERVICE)
			break;

		for (int i(0); i < GetTransmitTargetsNum(pTransServ); ++ i)
		{
			int id = pTransServ->targets[i].idTarget;
			if (id != 0 && id == idTarget)
			{
				m_transWay.mode = TransWay::TM_NPC_TRANSMIT_NORMAL;
				m_transWay.nCost = pTransServ->targets[i].fee;
				m_transWay.nLevel = pTransServ->targets[i].required_level;
				m_transWay.wayPoints.AddTail(const_cast<TRANS_POINT*>(&it->second));
				break;
			}
		}
		break;
	}
	return m_transWay.IsValid();
}

bool CECMapDlgsMgr::UpdateTransWay(const NPC_ESSENCE* pNPC, int idTarget)
{
	ResetTransWay();

	if (!idTarget)	return false;

	CECGameRun *pGameRun  = g_pGame->GetGameRun();

	if (!pNPC)
	{
		//	使用传送卷轴时，直接传送到目标点
		PointMap::const_iterator it = GetTransPoint().find(idTarget);
		if (it != GetTransPoint().end())
		{
			m_transWay.mode = TransWay::TM_TRANSMITSCROLL;
			m_transWay.wayPoints.AddTail(const_cast<TRANS_POINT*>(&it->second));
		}
		return m_transWay.IsValid();
	}

	if (m_pTransCache->IsInvalidNPC(pNPC))
	{
		//	不可搜索传送路径的 NPC 节点，统一使用普通方式检查并传送
		return UpdateTransWayWithinNPC(pNPC, idTarget);
	}

	// self transmit id
	const int idSelf = pNPC->id_to_discover;
	if (idSelf == idTarget)
	{
		return false;
	}

	// check invalid self position
	PointMap::const_iterator point = GetTransPoint().find(idSelf);
	if(point == GetTransPoint().end())
	{
		return false;
	}

	// check whether target found by player
	point = GetTransPoint().find(idTarget);
	if(point == GetTransPoint().end())
	{
		return false;
	}

	//	获取玩家的等级用于查找条件检查
	int iLevel = g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel;

	// open queue
	std::vector<TransPointCost> working;
	working.push_back(TransPointCost(point->second, 0, 0, -1)); // start point

	// check whether way point is duplicated
	abase::hash_map<int, bool> duplicated;

	// close queue, id-cost
	typedef abase::hash_map<int, TransPointCost> CloseMap;
	CloseMap closed;

	// start search by A*
	TransPointCost::Less comp; // compare operation
	while(!working.empty())
	{
		// pop open
		TransPointCost cost = *working.begin();
		duplicated.erase(cost.p->id);
		working.erase(working.begin());

		int idCur = cost.p->id;

		// found target
		if(idCur == idSelf)
		{
			CECHostPlayer *pHost = pGameRun->GetHostPlayer();
			m_transWay.mode = TransWay::HasDirectTransmitItem() ? TransWay::TM_NPC_TRANSMIT_DIRECT : TransWay::TM_NPC_TRANSMIT_NORMAL; //	若没有指定物品，则路径只用于显示，最终使用普通方式传送
			m_transWay.nCost = cost.sum;
			m_transWay.nLevel = cost.required_level;

			// back trace the way points
			CloseMap::iterator itr = closed.find(cost.parent);
			while(itr != closed.end())
			{
				m_transWay.wayPoints.AddTail(const_cast<CECMapDlgsMgr::TRANS_POINT*>(itr->second.p));
				itr = closed.find(itr->second.parent);
			}
			break;
		}
		else
		{
			closed[idCur] = cost;
			
			// find the way point that related to current one 
			typedef CECNPCTransmitCache::CacheRange Results;
			Results results = m_pTransCache->GetValidNPC(idCur);
			typedef CECNPCTransmitCache::CacheType::const_iterator Iterator;
			for(Iterator itr = results.first; itr != results.second; ++itr)
			{
				// from valid npc with transmit service
				typedef CECNPCTransmitCache::Info Info;
				const Info& info = itr->second;
				int idCandidate = info.pNPC->id_to_discover;

				// skip the duplicated points
				if(duplicated.find(idCandidate) != duplicated.end())
				{
					continue;
				}

				//	跳过等级条件不满足的节点
				if (info.required_level > iLevel)
				{
					continue;
				}

				// whether discovered already
				PointMap::iterator point = m_transPoints.find(idCandidate);
				if(point != m_transPoints.end())
				{
					CloseMap::iterator isClosed = closed.find(idCandidate);
					TransPointCost newCost(point->second, cost.sum + info.fee, max(cost.required_level, info.required_level), cost.p->id);

					if(isClosed == closed.end())
					{
						// normal open
						duplicated[idCandidate] = true;
						working.push_back(newCost);
					}
					else if(newCost.sum < isClosed->second.sum)
					{
						// reopen
						closed.erase(isClosed);
						duplicated[idCandidate] = true;
						working.push_back(newCost);
					}
				}
			}
		}

		std::sort(working.begin(), working.end(), comp);
	}

	return m_transWay.IsValid();
}

void CECMapDlgsMgr::DrawTransWay(const A3DPOINT2& ptCenter)
{
	const TransWayPoints &transWayPoints = GetTransWay().wayPoints;
	if(transWayPoints.GetCount())
	{
		CDlgMiniMap* pMini = (CDlgMiniMap*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_Map");

		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		ALISTPOSITION pos = transWayPoints.GetHeadPosition();
		int x = 0, y = 0;

		// change the style
		m_pA2DSpriteWaypoint->SetScaleX(1.25f);
		m_pA2DSpriteWaypoint->SetScaleY(1.25f);
		m_pA2DSpriteWaypoint->SetAlpha(0x80);
		m_pA2DSpriteWaypoint->SetCurrentItem(CECMapDlgsMgr::WAYPOINT_TARGET);

		A3DPOINT2 absCenter = ptCenter + A3DPOINT2(p->X, p->Y);
		while(pos)
		{ 
			TRANS_POINT& tp = *transWayPoints.GetNext(pos);

			A3DPOINT2 nextPos = absCenter + GetPixelsByPos(tp.vecPos.x, tp.vecPos.z, tp.worldid);

			if(x != 0 && y != 0)
			{
				pMini->DrawPath(x, y, nextPos.x, nextPos.y, 4);
			}
			x = nextPos.x;
			y = nextPos.y;

			// redraw the point
			A3DPOINT2 markPos = absCenter + GetPixelsByPos(tp.vecPos.x, tp.vecPos.z, tp.worldid);
			m_pA2DSpriteWaypoint->DrawToBack(markPos.x, markPos.y);
		}
		// restore property
		m_pA2DSpriteWaypoint->SetAlpha(0xff);
		m_pA2DSpriteWaypoint->SetScaleX(1.f);
		m_pA2DSpriteWaypoint->SetScaleY(1.f);
	}
}

void CECMapDlgsMgr::SetWorldMapMode(bool isWide)
{
	m_WideMode = isWide;
	ResizeWorldMap(NULL);
}

AString CECMapDlgsMgr::GetMapTexture(MAP_TYPE type){	
	AString strFile;
	CECGameRun *pGameRun = g_pGame->GetGameRun();
	int idInstance = pGameRun->GetWorld()->GetInstanceID();
	if( (type == MAP_TYPE_TRANSMIT && idInstance == MAJOR_MAP) ||
		type == MAP_TYPE_SCROLL ){
		strFile = "InGame\\JourneyMap.dds";
	}else{
		CECInstance *pInstance = pGameRun->GetInstance(idInstance);
		AString strSubPath = pInstance->GetPath();

		//	优先使用分历史阶段规则
		int curStageIndex = pGameRun->GetCurStageIndex()+1;
		if (curStageIndex>=1){
			AString historymap;
			bool bMapExist = false;
			do{
				historymap.Format("-%d.dds",curStageIndex);
				strFile = "InGame\\" + strSubPath + historymap;
				bMapExist = af_IsFileExist("surfaces\\"+strFile);
			} while (!bMapExist && --curStageIndex);
			if (!bMapExist){
				strFile.Empty();
			}
		}

		//	再检查是否按位置显示不同M图
		if (strFile.IsEmpty()){
			CECHostPlayer *pHost = pGameRun->GetHostPlayer();
			while (pHost->HostIsReady()){
				A3DVECTOR3 pos = pHost->GetPos();
				if (!pInstance->GetPositionRelatedTexture(pos.x, pos.z, strFile)){
					break;
				}
				strFile = AString("InGame\\") + strFile;
				break;
			}
		}

		//	其它情况
		if (strFile.IsEmpty()){
			strFile = "InGame\\" + strSubPath + ".dds";
		}
	}
	return strFile;
}

bool CECMapDlgsMgr::LoadMapTexture(A3DDevice* pDevice, PAUIIMAGEPICTURE pImage, MAP_TYPE type)
{
	int idInstance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	AString strFile = GetMapTexture(type);
	A2DSprite *pA2DSprite = new A2DSprite;

	if (strFile.IsEmpty() || !pA2DSprite->Init(pDevice, strFile, 0)){
		if (!strFile.IsEmpty()){
			AUI_ReportError(__LINE__, 1, "Failed to load map texture(%s) for idInstance=%d", strFile, idInstance);
		}
		// use empty map to try again
		if (!pA2DSprite->Init(pDevice, "InGame\\map_bg.dds", 0)){
			A3DRELEASE(pA2DSprite);
			AUI_ReportError(__LINE__, __FILE__);
			return false;
		}
	}
	pA2DSprite->SetLinearFilter(true);
	
	int nItem;
	A2DSprite *pOldA2DSprite = NULL;
	pImage->GetCover(&pOldA2DSprite, &nItem);
	A3DRELEASE(pOldA2DSprite);
	pImage->ClearCover();

	pImage->SetCover(pA2DSprite, 0);

	// store the original size in data
	DWORD pData = (((DWORD)pA2DSprite->GetWidth()) << 16) | (DWORD)pA2DSprite->GetHeight();
	pImage->SetData(pData, "TEXTURE_SIZE");

	// set to wide mode when the ratio is larger than 4:3
	float ratio = (float)pA2DSprite->GetWidth() / pA2DSprite->GetHeight();
	SetWorldMapMode(ratio > (4.f / 3.f) );

	return true;
}

void CECMapDlgsMgr::AdjustMapTexture(PAUIIMAGEPICTURE pImage, int defaultWidth, const SIZE& bgSize, const POINT& offset)
{
	DWORD data = pImage ? pImage->GetData("TEXTURE_SIZE") : 0;
	if(data)
	{
		int imgWidth = (data >> 16);
		int imgHeight = (data & 0xFFFF);

		// current scaler factor for the background
		// only consider the horizon scale in wide mode
		float scale = (float)bgSize.cx / defaultWidth;
		imgWidth = int(imgWidth * scale);
		imgHeight = int(imgHeight * scale);

		// the offset if we put the wide texture on 
		// the center of the map background.
		int offsetX = (bgSize.cx - imgWidth) / 2;
		int offsetY = (bgSize.cy - imgHeight) / 2;

		// adjust the final pos and size
		pImage->SetSize( imgWidth, imgHeight );
		pImage->SetPos( offsetX + offset.x, offsetY + offset.y); 
	}
}

int CECMapDlgsMgr::GetCurMark()
{
	PAUIIMAGEPICTURE pImage = GetCurFlagImage();
	return pImage ? pImage->GetData("PlayerMarkIndex") : -1;
}

void CECMapDlgsMgr::SetCurFlagIndex(int val)
{
	m_nCurFlagIndex = val;
}

PAUIIMAGEPICTURE CECMapDlgsMgr::GetCurFlagImage()
{
	PAUIIMAGEPICTURE pImage = NULL;

	if(m_nCurFlagIndex >= 0)
	{
		AString strName;
		strName.Format("Flag%d", m_nCurFlagIndex + 1);
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		PAUIDIALOG pShow = pGameUI->GetDialog(strName);
		pImage = pShow ? (PAUIIMAGEPICTURE)pShow->GetDlgItem("Flag") : NULL;
	}

	return pImage;
}

void CECMapDlgsMgr::DrawAutoMoveFlag(const CECMapDlgsMgr::PosConvertor& conv)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	PAUIDIALOG pDlgMark = pGameUI->GetDialog("mMark6");
	CDlgMiniMap* pDlgMini = (CDlgMiniMap*)pGameUI->GetDialog("Win_Map");

	// draw automove path
	if (CECIntelligentRoute::Instance().IsMoveOn()){		
		int nCount = CECIntelligentRoute::Instance().GetLeftNodeCount();
		//	计算显示时移动步长，用于显示时处理的节点数，长路径时提高效率
		int nTestCount = a_Min(20, nCount);
		int i(0);
		A3DPOINT2 lastPos(0, 0);
		double sumPixelLength(0.0);
		for (i = 0; i < nTestCount; ++ i)
		{
			A3DVECTOR3 pos = CECIntelligentRoute::Instance().GetLeftNodePosXZ(i);
			A3DPOINT2  absPos = conv(pos);
			if (i == 0){
				lastPos = absPos;
				continue;;
			}
			sumPixelLength += sqrt(pow(absPos.x-lastPos.x, 2)+pow(absPos.y-lastPos.y,2));
			lastPos = absPos;
		}
		int nStep = (int)6/(sumPixelLength/(nTestCount-1));		//	6 个象素长 / 每步长平均象素 = 应走过步长
		nStep = a_Max(nStep, 1);
		//	按相同步长显示节点
		for (i = 0; i < nCount; i+= nStep)
		{
			A3DVECTOR3 pos = CECIntelligentRoute::Instance().GetLeftNodePosXZ(i);
			A3DPOINT2  absPos = conv(pos);
			pDlgMini->DrawPathNode(absPos.x, absPos.y);
		}
	}
	if (pHost->IsAutoMoving()){
		// convert target position to screen pos
		A3DPOINT2 TargetPos2D = pGameUI->AutoMoveTarget();
		A3DPOINT2 absPos = conv(A3DVECTOR3((float)TargetPos2D.x, 0.0f, (float)TargetPos2D.y));
		if (CECIntelligentRoute::Instance().IsIdle()){
			A3DPOINT2 absHostPos = conv(pHost->GetPos());
			
			// draw the path line
			pDlgMini->DrawPath(absHostPos.x, absHostPos.y, absPos.x, absPos.y, 6);
		}

		// adjust the mark pos
		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		pDlgMark->SetPosEx(absPos.x - p->X, absPos.y - p->Y);
		if (!pDlgMark->IsShow()) pDlgMark->Show(true);
	}
	else
	{
		pDlgMark->Show(false);
	}
}

void CECMapDlgsMgr::DrawHostMark(const PosConvertor& conv)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	A3DVECTOR3 hostDir = pHost->GetDir();
	int nDegree = 90 - (int)RAD2DEG((float)atan2(hostDir.z, hostDir.x));

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgMiniMap* pDlgMini = (CDlgMiniMap*)pGameUI->GetDialog("Win_Map");
	bool bval = pDlgMini->DrawHostMark(nDegree, conv(pHost->GetPos()));
	if( !bval )
	{
		AUI_ReportError(__LINE__, __FILE__);
	}
}

bool CECMapDlgsMgr::IsWorldMapShown()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if( pGameUI->GetDialog("Win_WorldMap")->IsShow() ||
		pGameUI->GetDialog("Win_WorldMapDetail")->IsShow() ||
		pGameUI->GetDialog("Win_WorldMapTravel")->IsShow() ||
		pGameUI->GetDialog("Win_CountryMap")->IsShow() ||
		pGameUI->GetDialog("Win_CountryWarMap")->IsShow() ||
		pGameUI->GetDialog("Win_RandomMap")->IsShow())
	{
		return true;
	}

	return false;
}

void CECMapDlgsMgr::ShowWorldMap(bool bShow)
{
	bool bWorldMapShown = IsWorldMapShown();
	if (bShow == bWorldMapShown) return;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgWorldMap = pGameUI->GetDialog("Win_WorldMap");
	PAUIDIALOG pDlgWorldMapDetail = pGameUI->GetDialog("Win_WorldMapDetail");
	PAUIDIALOG pDlgWorldMapTravel = pGameUI->GetDialog("Win_WorldMapTravel");
	PAUIDIALOG pDlgCountryMap = pGameUI->GetDialog("Win_CountryMap");
	PAUIDIALOG pDlgCountryWarMap = pGameUI->GetDialog("Win_CountryWarMap");
	PAUIDIALOG pDlgHistoryPro = pGameUI->GetDialog("Win_HistoryPro");
	PAUIDIALOG pDlgRandomMap = pGameUI->GetDialog("Win_RandomMap");

	if (!bShow)
	{		
		if (pDlgWorldMapTravel->IsShow())
		{
			PAUIDIALOG pDlg = pDlgWorldMapTravel;
			int nItem;
			A2DSprite *pA2DSprite;
			PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pDlg->GetDlgItem("WorldMapTravel");
			
			pImage->GetCover(&pA2DSprite, &nItem);
			A3DRELEASE(pA2DSprite);
			pImage->ClearCover();
			
			pGameUI->EndNPCService();
			pDlg->Show(false);
		}

		if (pDlgWorldMapDetail->IsShow()) pDlgWorldMapDetail->Show(false);
		if (pDlgWorldMap->IsShow())	pDlgWorldMap->Show(false);

		pGameUI->GetDialog("Win_Bigmap")->Show(false);
		pGameUI->GetDialog("Win_BigMapMark")->Show(false);

		if (pDlgCountryMap->IsShow())
			pDlgCountryMap->OnCommand("IDCANCEL");
		if (pDlgCountryWarMap->IsShow())
			pDlgCountryWarMap->OnCommand("IDCANCEL");
		
		if(pDlgHistoryPro->IsShow())
			pDlgHistoryPro->Show(false);

		if(pDlgRandomMap->IsShow())
			pDlgRandomMap->Show(false);
	}
	else
	{
		CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
		if (pWorld->IsRandomMap())
			pDlgRandomMap->Show(true);
		else if (pWorld->IsCountryWarMap())	pDlgCountryWarMap->Show(true);
		else if (pWorld->IsCountryMap()) CECUIHelper::ShowCountryMap();
		else pDlgWorldMap->Show(true);

		if(g_pGame->GetGameRun()->IsInCurStage(pWorld->GetInstanceID()))
			pDlgHistoryPro->Show(true);
	}
}

void CECMapDlgsMgr::SwitchWorldMapShow()
{
	ShowWorldMap(!IsWorldMapShown());
}

CECMapDlgsMgr::PosConvertor* CECMapDlgsMgr::GetCurConvertor()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_WorldMapDetail");

	// select proper converter
	int idInstance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	return pDlgDetail->IsShow() ? (PosConvertor*)new PosConvertorDetail() : (PosConvertor*)new PosConvertorWorld(idInstance);
}

bool CECMapDlgsMgr::TransWay::HasDirectTransmitItem()
{
	bool bRet(false);

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECInventory *pPack = pHost->GetPack();

	int		specialItems[] = {35206, 35207, 35208, 35209};
	for (int i(0); i < sizeof(specialItems)/sizeof(specialItems[0]); ++ i)
	{
		if (pPack->FindItem(specialItems[i]) >= 0)
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}
// =======================================================================
// CECNPCTransmitCache
// =======================================================================
bool CECNPCTransmitCache::IsInvalidNPC(const NPC_ESSENCE *pNPC)const
{
	//	判断此 NPC 是否是可用于传送图搜索的合法 NPC
	return pNPC == NULL
		|| pNPC->id_to_discover == 0		//	传送图节点通过 id_to_discover 标识
		|| pNPC->id == 2218					//	以下 NPC 与其它 NPC 重复标识某个传送点，属冗余非法数据，将引起搜索混乱，在搜索时过滤
		|| pNPC->id == 25843
		|| pNPC->id == 25844;
}

CECNPCTransmitCache::CECNPCTransmitCache()
{
	// search all valid NPC transmit service
	DATA_TYPE dataType = DT_INVALID;
	elementdataman *pElementDataMan = g_pGame->GetElementDataMan();	
	unsigned int id = pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	do
	{
		if(dataType == DT_NPC_ESSENCE)
		{
			// get data from entity first
			const NPC_ESSENCE* pEssence = 
				(const NPC_ESSENCE *)pElementDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dataType);
			ASSERT(dataType == DT_NPC_ESSENCE);

			if (IsInvalidNPC(pEssence))
			{
				continue;
			}

			unsigned int idTrans = pEssence->id_transmit_service;
			if(!idTrans) continue;

			const NPC_TRANSMIT_SERVICE *pServ = 
				(const NPC_TRANSMIT_SERVICE *)pElementDataMan->get_data_ptr(idTrans, ID_SPACE_ESSENCE, dataType);
			if(dataType != DT_NPC_TRANSMIT_SERVICE) continue;
			
			size_t maxTarget = GetTransmitTargetsNum(pServ);
			CECNPCTransmitCache::Info npcInfo = {pEssence, pServ, 0, 0};
			
			for(size_t i=0;i<maxTarget;i++)
			{
				int targetID = pServ->targets[i].idTarget;
				if(targetID == 0) continue;
				npcInfo.fee = pServ->targets[i].fee;
				npcInfo.required_level = pServ->targets[i].required_level;
				m_NPCCache.insert(CachePair(targetID, npcInfo));
			}
		}
	}while(id = pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType));
}

class AutoMoveCommand : public CDlgDynamicControl::ControlCommand
{
public:
	AutoMoveCommand():m_Pos(0.f, 0.f, 0.f){}
	virtual ~AutoMoveCommand(){};

	AutoMoveCommand(const A3DVECTOR3& p):m_Pos(p){}
	virtual bool operator ()(){
		CECUIHelper::AutoMoveStartComplex(m_Pos);
		return true;
	}
	const A3DVECTOR3& GetPos(){ return m_Pos; }

protected:
	A3DVECTOR3 m_Pos;
};
void CDlgWorldMap::AddAutoMoveButton(const ACString& hint, const char* image, const A3DVECTOR3& pos, DWORD data)
{
	if (!image || strlen(image) == 0) return;
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>
		(CreateControlWithCommand(hint, AUIOBJECT_TYPE_STILLIMAGEBUTTON, new AutoMoveCommand(pos)));
	if (pBtn) {
		pBtn->SetData(data);
		pBtn->SetHint(hint);
		AUIOBJECT_SETPROPERTY poperty;
		strcpy(poperty.fn, image);
		{			
			ScopedAUIControlSpriteModify _dummy(pBtn);
			pBtn->SetProperty("Up Frame File", &poperty);
			pBtn->SetProperty("Down Frame File", &poperty);
		}
		PAUIFRAME pFrame = pBtn->GetFrame(AUISTILLIMAGEBUTTON_FRAME_UP);
		if (pFrame) {
			A2DSprite* pSprite = pFrame->GetA2DSprite();
			if (pSprite)
				pBtn->SetSize(pSprite->GetWidth(), pSprite->GetHeight());
		}
		CECMapDlgsMgr* pMgr = GetGameUIMan()->GetMapDlgsMgr();
		int idInstance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
		CECMapDlgsMgr::PosConvertor* conv = (m_szName == "Win_WorldMapDetail") ? (CECMapDlgsMgr::PosConvertor*)new PosConvertorDetail()
			: (CECMapDlgsMgr::PosConvertor*)new PosConvertorWorld(idInstance);
		A3DPOINT2 absPos = (*conv)(pos);
		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		SIZE sz = pBtn->GetSize();
		pBtn->SetPos(absPos.x - p->X - sz.cx/2, absPos.y - p->Y - sz.cy/2);
		delete conv;
	}
}

void CDlgWorldMap::OnCommand_SwitchTaskShow(const char * szCommand)
{
	CECGameUIMan* pGameUI = GetGameUIMan();
	CDlgTask* pDlg = dynamic_cast<CDlgTask*>(pGameUI->GetDialog("Win_Quest"));
	if (pDlg) {
		m_iTaskShowType = (m_iTaskShowType + 1) % TASK_TYPE_NUM;

		CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
		ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
		
		pGameUI->m_pDlgWorldMap->ClearControls();
		pGameUI->m_pDlgWorldMapDetail->ClearControls();
		abase::vector<int> tasks;
		if (m_iTaskShowType != TASK_TYPE_NONE) {
			ActiveTaskEntry* aEntries = pLst->m_TaskEntries;
			if (m_iTaskShowType == TASK_TYPE_TODO) {
				ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
				TaskTemplLst ttl;
				pMan->GetAvailableTasks(pTask, ttl);
				size_t count = ttl.size();
				for(size_t i = 0; i < count; i++ ) {
					ATaskTempl* pTemp = ttl[i];
					if (pTemp) {
						if (pTemp->m_ulType == enumTTDaily ||
							pTemp->m_ulType == enumTTLevel2 ||
							pTemp->m_ulType == enumTTMajor ||
							pTemp->m_ulType == enumTTEvent ||
							(pTemp->m_ulType == enumTTBranch && 
							GetHostPlayer()->GetBasicProps().iLevel - pTemp->m_ulPremise_Lev_Min <= 5))
							tasks.push_back(pTemp->m_ID);
					}
				}
				
			} else {
				for (unsigned char i = 0; i < pLst->m_uTaskCount; i++) {
					ActiveTaskEntry& CurEntry = aEntries[i];
					if (!CurEntry.m_ulTemplAddr) continue;
					
					bool bCanfinish = pTask->CanFinishTask(CurEntry.m_ID);
					if (m_iTaskShowType == TASK_TYPE_ALL || 
						(m_iTaskShowType == TASK_TYPE_FINISHED && bCanfinish) ||
						(m_iTaskShowType == TASK_TYPE_UNFINISHED && !bCanfinish))
						tasks.push_back(CurEntry.m_ID);
				}
			}
			CECUIHelper::ShowTasksInMap(tasks);
			pGameUI->m_pDlgWorldMapDetail->UpdateDetailMapControlPos();
		}
	}
}

void CDlgWorldMap::UpdateDetailMapControlPos()
{
	if (m_szName != "Win_WorldMapDetail") return;
	PosConvertorDetail conv;
	size_t count = m_Controls.size();
	POINT pos_dlg = GetPos();
	for (size_t i = 0; i < count; ++i) {
		AutoMoveCommand* pCommand = dynamic_cast<AutoMoveCommand*>(m_Controls[i].command);
		PAUIOBJECT pObj = m_Controls[i].control;
		if (pCommand && pObj) {
			A3DPOINT2 pos_screen = conv(pCommand->GetPos());
			SIZE s = pObj->GetSize();
			pObj->SetPos(pos_screen.x - pos_dlg.x - s.cx / 2, pos_screen.y - pos_dlg.y - s.cy / 2);
		}
	}
}

void CDlgWorldMap::UpdateTaskControl()
{
	size_t count = m_Controls.size();
	std::vector<DynamicControl> tmp;
	CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
	for (size_t i = 0; i < count; ++i) {
		PAUIOBJECT pObj = m_Controls[i].control;
		if (pObj) {
			int data = pObj->GetData();
			if (data != 0 && !pTask->HasTask(data)) {
				DeleteControl(pObj);
				delete m_Controls[i].command;
			} else tmp.push_back(m_Controls[i]);
		}
	}
	if (count != tmp.size()) {
		m_Controls.swap(tmp);
		if (m_Controls.empty())
			SetTaskShowType(TASK_TYPE_NONE);
	}
}

void CDlgWorldMap::RenderFactionPVPIcon(const void *posConverter
										, A2DSprite *pA2DSpriteMineBase, A2DSprite *pA2DSpriteMineBaseForMyFaction
										, A2DSprite *pA2DSpriteMine, A2DSprite *pA2DSpriteMineForMyFaction)
{
	//	检查要使用的图片存在
	if (!pA2DSpriteMineBase || !pA2DSpriteMineBaseForMyFaction ||
		!pA2DSpriteMine || !pA2DSpriteMineForMyFaction){
		return;
	}
	//	检查当前状态是否可以显示
	if (!CECUIHelper::FactionPVPCanShowResourceInMap()){
		return;
	}
	//	处理每个有剩余资源的 domain
	const CECMapDlgsMgr::PosConvertor &conv = *(const CECMapDlgsMgr::PosConvertor *)posConverter;
	CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
	int	myFactionID = GetHostPlayer()->GetFactionID();
	CECFactionPVPModel::DomainWithResourceIterator begin = factionPVPModel.BeginDomainWithResource();
	CECFactionPVPModel::DomainWithResourceIterator end = factionPVPModel.EndDomainWithResource();
	for (CECFactionPVPModel::DomainWithResourceIterator it = begin; it != end; ++ it){
		int domain = *it;
		const CECFactionPVPModel::ResourcePosition *pResourcePos = factionPVPModel.GetResourcePosition(domain);
		if (!pResourcePos){
			continue;
		}
		bool belongToMyFaction = CECDomainGuildInfo::Instance().IsDomainOwner(domain, myFactionID);	//	domain 是否属于本帮
		A3DPOINT2 pt = conv(pResourcePos->minePos);
		if (belongToMyFaction){
			pA2DSpriteMineForMyFaction->DrawToBack(pt.x, pt.y);
		}else{
			pA2DSpriteMine->DrawToBack(pt.x, pt.y);
		}
		pt = conv(pResourcePos->mineBasePos);
		if (belongToMyFaction){
			pA2DSpriteMineBaseForMyFaction->DrawToBack(pt.x, pt.y);
		}else{
			pA2DSpriteMineBase->DrawToBack(pt.x, pt.y);
		}
	}
}

