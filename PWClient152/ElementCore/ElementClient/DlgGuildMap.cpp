// Filename	: DlgGuildMap.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/31

#include "DlgGuildMap.h"
#include "DlgGuildMapStatus.h"
#include "DlgGuildMapTravel.h"
#include "DlgFactionPVPStatus.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Faction.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EL_Precinct.h"
#include "EC_DomainGuild.h"

#include "A2DSprite.h"
#include "A3DDevice.h"
#include "A3DGdi.h"
#include "globaldataman.h"
#include "Home\\ClosedArea.h"
#include "A3DFlatCollector.h"
#include "EC_GameSession.h"
#include "battlegetmap_re.hpp"
#include "battlechallenge_re.hpp"
#include "battlechallengemap_re.hpp"
#include "battlestatus_re.hpp"
#include "battleenter_re.hpp"
#include "Network\\ids.hxx"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildMap, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGuildMap, CDlgBase)

AUI_ON_EVENT("GuildMap",	WM_MOUSEMOVE,	OnEventMouseMove)
AUI_ON_EVENT("GuildMap",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("GuildMap",	WM_RBUTTONDOWN,	OnEventRButtonDown)

AUI_END_EVENT_MAP()

CDlgGuildMap::CDlgGuildMap()
{
	m_pImg_GuildMap = NULL;
	m_nHoverAreaIndex = -1;
	m_nType = FACTIONMAP_TYPE_NORMAL;
	m_bGetMap = false;
	m_bGetChallengeMap = false;
	m_pA2DSpriteArrow1 = NULL;
	m_pA2DSpriteArrow2 = NULL;
	m_pA2DSpriteArrow3 = NULL;
	m_nMyFactionColor = 0;
	m_nMaxBid = 0;
	m_nBattleChallengeRandNum = 0;

	memset(&m_Info, 0, sizeof(m_Info));

	ClearMessage();
}

CDlgGuildMap::~CDlgGuildMap()
{
	A3DRELEASE(m_pA2DSpriteArrow1);
	A3DRELEASE(m_pA2DSpriteArrow2);
	A3DRELEASE(m_pA2DSpriteArrow3);
}

A3DPOINT2 CDlgGuildMap::ScreenToPixel(A3DPOINT2 pt)
{
	A3DRECT objRect = m_pImg_GuildMap->GetRect();
	A3DPOINT2 ptCenter = objRect.CenterPoint();
	SIZE size;
	size.cx = m_pImg_GuildMap->GetImage()->GetWidth();
	size.cy = m_pImg_GuildMap->GetImage()->GetHeight();
	pt.x = size.cx / 2 + (pt.x - ptCenter.x) * size.cx / objRect.Width();
	pt.y = size.cy / 2 + (pt.y - ptCenter.y) * size.cy / objRect.Height();
	return pt;
}

A3DPOINT2 CDlgGuildMap::PixelToScreen(A3DPOINT2 pt)
{
	A3DRECT objRect = m_pImg_GuildMap->GetRect();
	A3DPOINT2 ptCenter = objRect.CenterPoint();
	SIZE size;
	size.cx = m_pImg_GuildMap->GetImage()->GetWidth();
	size.cy = m_pImg_GuildMap->GetImage()->GetHeight();
	pt.x = (pt.x - size.cx / 2) * objRect.Width() / size.cx + ptCenter.x;
	pt.y = (pt.y - size.cy / 2) * objRect.Height() / size.cy + ptCenter.y;
	return pt;
}

void CDlgGuildMap::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	A3DPOINT2 pt;
	pt.x = GET_X_LPARAM(lParam) - p->X;
	pt.y = GET_Y_LPARAM(lParam) - p->Y;
	pt = ScreenToPixel(pt);
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	DWORD i, j;
	POINT_FLOAT pf;
	pf.x = (float)pt.x;
	pf.y = (float)pt.y;
	m_nHoverAreaIndex = -1;
	CClosedArea cArea;
	for(i = 0; i < m_vecAreaRect.size(); i++)
		if( m_vecAreaRect[i].PtInRect(pt.x, pt.y) )
		{
			cArea.Clear();
			for(j = 0; j < (*info)[i].verts.size(); j++)
			{
				POINT_FLOAT temp;
				temp.x = (float)(*info)[i].verts[j].x;
				temp.y = (float)(*info)[i].verts[j].y;
				cArea.AddPoint(temp);
			}
			cArea.AreaFinish();
			if( cArea.IsInArea(pf) )
			{
				m_nHoverAreaIndex = i;
				break;
			}
		}

	if( m_nType == FACTIONMAP_TYPE_NORMAL || 
		m_nType == FACTIONMAP_TYPE_ENTER )
		GetGameUIMan()->m_pDlgGMapStatus1->SetIndex(m_nHoverAreaIndex);
	else if( m_nType == FACTIONMAP_TYPE_CHALLENGE )
		GetGameUIMan()->m_pDlgGMapStatus2->SetIndex(m_nHoverAreaIndex);
}

void CDlgGuildMap::OnEventMButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
}

void CDlgGuildMap::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnCommandCancel("");
}

void CDlgGuildMap::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	int idFaction = GetHostPlayer()->GetFactionID();
	if( m_nType == FACTIONMAP_TYPE_ENTER )
	{
		if( m_nHoverAreaIndex != -1 && 
			(m_vecAreaType[m_nHoverAreaIndex] & AREA_TYPE_CANENTER) &&
			((int)(*info)[m_nHoverAreaIndex].id_owner == idFaction || 
			(int)(*info)[m_nHoverAreaIndex].id_challenger == idFaction) )
		{
			abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
			GetGameSession()->battle_Enter((*info)[m_nHoverAreaIndex].id);
			OnCommandCancel("");
		}
	}
	else if( m_nType == FACTIONMAP_TYPE_CHALLENGE )
	{
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		if( GetHostPlayer()->GetFRoleID() == GNET::_R_MASTER &&
			!pGameUIMan->GetDialog("ChallengeErr") )
			if( m_nHoverAreaIndex != -1 )
				if(	m_vecAreaType[m_nHoverAreaIndex] & AREA_TYPE_CANCHALLENGE )
				{
					pGameUIMan->m_pDlgGMapChallenge->SetIndex(m_nHoverAreaIndex);
					pGameUIMan->m_pDlgGMapChallenge->Show(true, true);
					pGameUIMan->m_pDlgGMapChallenge->SetPosEx(0, 0, alignCenter, alignCenter);
					pGameUIMan->m_pDlgGMapChallenge->SetCanMove(false);
				}
				else if( m_vecAreaType[m_nHoverAreaIndex] & AREA_TYPE_TEMPCHALLENGED )
						pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(819),
							MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				else
				{
					int j;
					DWORD nDomain = 0;
					for(j = 0; j < (int)info->size(); j++ )
						if( (*info)[j].id_owner == idFaction )
							nDomain++;
					int nChallenged = 0;
					for(j = 0; j < (int)info->size(); j++ )
						if( (*info)[j].id_tempchallenger == idFaction )
							nChallenged++;
					if( nChallenged >= m_nMaxBid )
						pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(761),
							MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					else if( nDomain == 0 )
						pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(762),
							MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					else
					{
						ACString strText, strLv = _AL("");
						int k;
						if( nDomain > 3 )
							k = 3;
						else
							k = nDomain;
						for(j = k - 1; j >= 0; j--)
						{
							strLv += ACHAR((3 - j) + 48);
							if(j != 0)
								strLv += _AL(",");
						}
						strText.Format(GetStringFromTable(763), nDomain, strLv);
						pGameUIMan->MessageBox("ChallengeErr", strText,
							MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					}
				}
	}
	else if( m_nType == FACTIONMAP_TYPE_NORMAL )
	{
		if( idFaction != 0 && m_nHoverAreaIndex != -1 )
		{
			bool bCanTravel = false;
			DWORD i;
			for(i = 0; i < info->size(); i++ )
				if( (*info)[i].id_owner == idFaction )
				{
					if( (int)i == m_nHoverAreaIndex )
					{
						bCanTravel = true;
						break;
					}
				}

			if( bCanTravel )
			{
				CECGameUIMan *pGameUIMan = GetGameUIMan();
				pGameUIMan->m_pDlgGMapTravel->SetDomainId((*info)[m_nHoverAreaIndex].id);
				pGameUIMan->m_pDlgGMapTravel->Show(true, true);
				pGameUIMan->m_pDlgGMapTravel->SetPosEx(0, 0, alignCenter, alignCenter);
				pGameUIMan->m_pDlgGMapTravel->SetCanMove(false);
			}
		}
	}
}

bool CDlgGuildMap::OnInitDialog()
{
	m_bBattleOpen = false;
	m_bBattleChallengeOpen = false;
	m_bBattleInFight = false;
	DDX_Control("GuildMap", m_pImg_GuildMap);
	m_vecAreaRect.clear();
	m_vecAreaType.clear();
	m_vecFactionColor.clear();

	A2DSprite *pA2DSprite = m_pImg_GuildMap->GetImage();
	pA2DSprite->SetLinearFilter(true);
	
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	DWORD i, j;
	for(i = 0; i < info->size(); i++ )
	{
		A3DRECT rect;
		rect.SetRect(9999, 9999, 0, 0);
		(*info)[i].id_owner = 0;
		(*info)[i].id_challenger = 0;
		(*info)[i].battle_time = 0;
		(*info)[i].id_tempchallenger = 0;
		(*info)[i].deposit = 0;
		(*info)[i].cutoff_time = 0;
		(*info)[i].color = 0;
		for(j = 0; j < (*info)[i].verts.size(); j++)
		{
			if( (*info)[i].verts[j].x < rect.left )
				rect.left = (*info)[i].verts[j].x;
			if( (*info)[i].verts[j].x > rect.right )
				rect.right = (*info)[i].verts[j].x;
			if( (*info)[i].verts[j].y < rect.top )
				rect.top = (*info)[i].verts[j].y;
			if( (*info)[i].verts[j].y > rect.bottom )
				rect.bottom = (*info)[i].verts[j].y;
		}
		m_vecAreaRect.push_back(rect);
		m_vecAreaType.push_back(AREA_TYPE_NORMAL);
	}
	CECFactionPVPModel::Instance().OnGuildBattleDomainOwnerChange();

	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if (!pGameUIMan->LoadSprite("InGame\\城战箭头1.tga", m_pA2DSpriteArrow1)){
		return AUI_ReportError(__LINE__, __FILE__);
	}
	if (!pGameUIMan->LoadSprite("InGame\\城战箭头2.tga", m_pA2DSpriteArrow2)){
		return AUI_ReportError(__LINE__, __FILE__);
	}
	if (!pGameUIMan->LoadSprite("InGame\\城战箭头3.tga", m_pA2DSpriteArrow3)){
		return AUI_ReportError(__LINE__, __FILE__);
	}

	AFileImage aFile;
	if( !aFile.Open("Surfaces\\InGame\\FactionColor.txt", AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
		return AUI_ReportError(__LINE__, __FILE__);
	char szText[100];
	DWORD len;
	int r, g, b;
	do 
	{
		if( !aFile.ReadLine(szText, 100, &len) || len == 0 )
			break;
		sscanf(szText, "%d%d%d", &r, &g, &b);
		m_vecFactionColor.push_back(A3DCOLORRGBA(r, g, b, 64));
	} while(true);
	aFile.Close();
	return true;
}

void CDlgGuildMap::OnCommandCancel(const char * szCommand)
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if( pGameUIMan->m_pDlgGMapChallenge->IsShow() )
		return;

	m_nHoverAreaIndex = -1;
	Show(false);
	if( m_nType == FACTIONMAP_TYPE_CHALLENGE )
	{
		pGameUIMan->EndNPCService();
		pGameUIMan->m_pDlgGMapStatus2->Show(false);
	}
	else
	{
		if( m_nType == FACTIONMAP_TYPE_ENTER )
			pGameUIMan->EndNPCService();
		pGameUIMan->m_pDlgGMapTravel->Show(false);
		pGameUIMan->GetDialog("Win_GMapBattleTime")->Show(false);
		pGameUIMan->m_pDlgGMapStatus1->Show(false);
	}
	pGameUIMan->GetDialog("Win_MyFactionColor")->Show(false);
	if (pGameUIMan->m_pDlgFactionPVPStatus->IsShow()){
		pGameUIMan->m_pDlgFactionPVPStatus->Show(false);
	}
	if (PAUIDIALOG pDlgFactionPVPRank = pGameUIMan->GetDialog("Win_PvpReward")){
		if (pDlgFactionPVPRank->IsShow()){
			pDlgFactionPVPRank->Show(false);
		}
	}
}

const ACString&	CDlgGuildMap::AddMessage(const ACString& msg)
{
	if(m_MsgPos == -1)
	{
		// use this time stamp to show first message immediately
		m_LastUpdateTime = GetGame()->GetServerGMTTime() - 30;
		m_MsgPos = 0;
	}

	m_Msgs.push_back(msg);
	return msg;
}

void CDlgGuildMap::ClearMessage()
{
	m_Msgs.clear();
	m_MsgPos = -1;
	m_LastUpdateTime = -1;
}

void CDlgGuildMap::ShowCachedMessage()
{
	// show message every 30 seconds
	if(m_MsgPos >= 0 && m_MsgPos < (int)m_Msgs.size())
	{
		int curTime = GetGame()->GetServerGMTTime();
		if(curTime - m_LastUpdateTime >= 30)
		{
			m_LastUpdateTime = curTime;
			// show next message
			GetGame()->GetGameRun()->AddChatMessage(m_Msgs[m_MsgPos], GP_CHAT_BROADCAST, 0, NULL, 1, 0);
			++m_MsgPos;
		}
	}
}

void CDlgGuildMap::OnTick()
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if( pGameUIMan->GetDialog("Win_MyFactionColor")->IsShow() )
		pGameUIMan->BringWindowToTop(pGameUIMan->GetDialog("Win_MyFactionColor"));
	else if( m_nMyFactionColor != 0 )
	{
		pGameUIMan->GetDialog("Win_MyFactionColor")->Show(true);
		pGameUIMan->GetDialog("Win_MyFactionColor")->SetCanMove(false);
		AlignMyFactionColor();
	}
	if( m_nType == FACTIONMAP_TYPE_NORMAL ||
		m_nType == FACTIONMAP_TYPE_ENTER )
	{
		pGameUIMan->BringWindowToTop(pGameUIMan->m_pDlgGMapTravel);
		pGameUIMan->BringWindowToTop(pGameUIMan->GetDialog("Win_GMapBattleTime"));
		pGameUIMan->BringWindowToTop(pGameUIMan->m_pDlgGMapStatus1);
	}
	else if( m_nType == FACTIONMAP_TYPE_CHALLENGE )
	{
		pGameUIMan->BringWindowToTop(pGameUIMan->m_pDlgGMapStatus2);
		if( pGameUIMan->m_pDlgGMapChallenge->IsShow() )
			pGameUIMan->BringWindowToTop(pGameUIMan->m_pDlgGMapChallenge);
		PAUIDIALOG pDlg = pGameUIMan->GetDialog("ChallengeErr");
		if( pDlg )
			pGameUIMan->BringWindowToTop(pDlg);

		pDlg = pGameUIMan->GetDialog("Game_GuildChallenge");
		if( pDlg )
			pGameUIMan->BringWindowToTop(pDlg);

		pDlg = pGameUIMan->GetDialog("ChallengeConfirm"); 
		if(pDlg)
			pGameUIMan->BringWindowToTop(pDlg);

		pDlg = pGameUIMan->GetDialog("ChallengeMax"); 
		if(pDlg)
			pGameUIMan->BringWindowToTop(pDlg);
	}
	if (pGameUIMan->m_pDlgFactionPVPStatus->IsShow()){
		pGameUIMan->BringWindowToTop(pGameUIMan->m_pDlgFactionPVPStatus);
	}
	if (PAUIDIALOG pDlgFactionPVPRank = pGameUIMan->GetDialog("Win_PvpReward")){
		if (pDlgFactionPVPRank->IsShow()){
			pGameUIMan->BringWindowToTop(pDlgFactionPVPRank);
		}
	}
}

void CDlgGuildMap::RenderArrow(int nIndex1, int nIndex2, A2DSprite *pArrow)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	A3DPOINT2 pt1 = PixelToScreen((*info)[nIndex1].center);
	if( nIndex2 == -1 )
	{
		pArrow->DrawToBack(int(p->X + pt1.x), int(p->Y + pt1.y) );
		return;
	}

	A3DPOINT2 pt2 = PixelToScreen((*info)[nIndex2].center);
	int nDegree = 90 + (int)RAD2DEG(atan2((float)(pt2.y - pt1.y), (float)(pt2.x - pt1.x)));
	pArrow->SetDegree(nDegree);
	int dx = pt2.x - pt1.x;
	int dy = pt2.y - pt1.y;
	float fScale = (float)(sqrt((float)(dy * dy + dx * dx)) - 20) / pArrow->GetHeight();
	if( fScale < 0.6f )
		fScale = 0.6f;
	pArrow->SetScaleX(fScale);
	pArrow->SetScaleY(fScale);
	pArrow->DrawToBack((int)p->X + (pt2.x + pt1.x) / 2, (int)p->Y + (pt2.y + pt1.y) / 2);
}

void CDlgGuildMap::RenderArea(int nIndex, A3DCOLOR cl)
{
	A3DFlatCollector *pFC = GetGame()->GetA3DEngine()->GetA3DFlatCollector();
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	DWORD i;
	A3DVECTOR3 *verts = (A3DVECTOR3*)a_malloctemp(sizeof(A3DVECTOR3) * (*info)[nIndex].verts.size());
	WORD *indices = (WORD*)a_malloctemp(sizeof(WORD) * (*info)[nIndex].faces.size());
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	for(i = 0; i < (*info)[nIndex].verts.size(); i++ )
	{
		A3DPOINT2 pt = PixelToScreen((*info)[nIndex].verts[i]);
		verts[i].x = (float)pt.x + p->X;
		verts[i].y = (float)pt.y + p->Y;
		verts[i].z = 0.0f;
	}
	for(i = 0; i < (*info)[nIndex].faces.size(); i++ )
		indices[i] = (*info)[nIndex].faces[i];

	A3DCULLTYPE oldtype = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	pFC->AddRenderData_2D(verts, (*info)[nIndex].verts.size(), 
		indices, (*info)[nIndex].faces.size(), cl);
	pFC->Flush_2D();
	m_pA3DDevice->SetFaceCull(oldtype);
	a_freetemp(indices);
	a_freetemp(verts);
}

void CDlgGuildMap::GetMap()
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	DWORD i;
	for(i = 0; i < info->size(); i++ )
	{
		(*info)[i].id_owner = 0;
		(*info)[i].id_challenger = 0;
		(*info)[i].battle_time = 0;
		(*info)[i].color = 0;
	}
	CECFactionPVPModel::Instance().OnGuildBattleDomainOwnerChange();
	GetGameSession()->battle_GetMap();	
	m_bGetMap = true;
}

void CDlgGuildMap::SetBattleChallengeOpen(bool b)
{
	m_bBattleChallengeOpen = b;
	CECFactionPVPModel::Instance().OnGuildBattleEnter(!IsBattleChallengeOpen() && IsBattleInFight());
}

void CDlgGuildMap::OnShowDialog()
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	if( !m_bGetMap )
		GetMap();
	else if( m_nType == FACTIONMAP_TYPE_CHALLENGE )
	{
		m_bGetChallengeMap = false;
		GetGameSession()->battle_ChallengeMap();
	}
	else
		UpdateNormalMap();
	if( m_nType == FACTIONMAP_TYPE_ENTER )
	{
		DWORD j;
		for(j = 0; j < info->size(); j++ )
			m_vecAreaType[j] &= ~(AREA_TYPE_CANENTER);
		GetGameSession()->battle_GetStatus();
	}
	m_nMyFactionColor = 0;
	int idFaction = GetHostPlayer()->GetFactionID();
	if( idFaction )
	{
		DWORD j;
		for(j = 0; j < info->size(); j++ )
			if( (int)(*info)[j].id_owner == idFaction )
			{
				m_nMyFactionColor = (*info)[j].color;
//				PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pGameUIMan->
//					GetDialog("Win_MyFactionColor")->GetDlgItem("Img_MyFactionColor");
//				pImage->SetColor(m_vecFactionColor[m_nMyFactionColor - 1]);
				break;
			}
	}
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if (PAUIDIALOG pDlgMyFactionColor = pGameUIMan->GetDialog("Win_MyFactionColor")){
		pDlgMyFactionColor->SetCanMove(false);
	}
	AlignMyFactionColor();

	CDlgFactionPVPStatus *pDlgFactionPVPStatus = pGameUIMan->m_pDlgFactionPVPStatus;
	if (pDlgFactionPVPStatus->IsShow() != pDlgFactionPVPStatus->ShouldShow()){
		pDlgFactionPVPStatus->SwitchShow();
	}
	CECFactionPVPModel::Instance().GetFactionPVPMap();
}

void CDlgGuildMap::AlignMyFactionColor()
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if (PAUIDIALOG pDlgMyFactionColor = pGameUIMan->GetDialog("Win_MyFactionColor")){
		if (pDlgMyFactionColor->IsShow()){
			pDlgMyFactionColor->SetPosEx(
				pGameUIMan->m_pDlgGuildMap->GetSize().cx / 20, 
				pGameUIMan->m_pDlgGuildMap->GetSize().cy / 15, 
				AUIDialog::alignNone, AUIDialog::alignNone);
		}
	}
}

void CDlgGuildMap::AlignFactionPVPStatus()
{
	if (PAUIDIALOG pDlgFactionPVPStatus = GetGameUIMan()->GetDialog("Win_GMapStatus3")){
		if (pDlgFactionPVPStatus->IsShow()){
			POINT objPoint = m_pImg_GuildMap->GetPos();
			SIZE  dialogSize = GetSize();
			pDlgFactionPVPStatus->SetPosEx(objPoint.x + dialogSize.cx / 20, dialogSize.cy / 5);
		}
	}
}

bool CDlgGuildMap::Render()
{
	bool bFlash = ((GetTickCount() / 500) % 2 == 0);
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int idFaction = GetHostPlayer()->GetFactionID();
	bool bval = CDlgBase::Render();
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	DWORD i, j;
//	A3DCOLOR *cl = new A3DCOLOR[info->size()];
//	for(i = 0; i < info->size(); i++ )
//		cl[i] = 0;
//	DWORD nColor = 1;
//	for(i = 0; i < info->size(); i++ )
//		if( (*info)[i].id_owner == idFaction )
//			cl[i] = m_vecFactionColor[0];
//	for(i = 0; i < info->size(); i++ )
//		if( (*info)[i].id_owner && (*info)[i].id_owner != idFaction && cl[i] == 0 )
//		{
//			for(j = i; j < info->size(); j++ )
//				if((*info)[j].id_owner == (*info)[i].id_owner )
//					cl[j] = m_vecFactionColor[nColor];
//			nColor++;
//			if( nColor == m_vecFactionColor.size() )
//				nColor = 1;
//		}
//	for(i = 0; i < info->size(); i++ )
//		if( cl[i] )
//			RenderArea(i, cl[i]);
//	delete []cl;
	
	A3DFlatCollector *pFC = GetGame()->GetA3DEngine()->GetA3DFlatCollector();
	pFC->Flush_2D();
	GetGame()->GetA3DEngine()->GetA3DDevice()->SetSourceAlpha(A3DBLEND_DESTCOLOR);
	GetGame()->GetA3DEngine()->GetA3DDevice()->SetDestAlpha(A3DBLEND_ZERO);
	for(i = 0; i < info->size(); i++ )
		if( (*info)[i].color > 0 && (*info)[i].color <= (int)m_vecFactionColor.size() )
		{
			if( !(m_nType == FACTIONMAP_TYPE_CHALLENGE && 
				GetHostPlayer()->GetFRoleID() == GNET::_R_MASTER &&
				(!((m_vecAreaType[i] & AREA_TYPE_CANCHALLENGE) ||
				(m_vecAreaType[i] & AREA_TYPE_TEMPCHALLENGED) ||
				(m_vecAreaType[i] & AREA_TYPE_OWNER)) ) 
				||
				m_nType == FACTIONMAP_TYPE_ENTER && 
				(!(m_vecAreaType[i] & AREA_TYPE_CANENTER) ||
				(int)(*info)[i].id_owner != idFaction && 
				(int)(*info)[i].id_challenger != idFaction)))
				RenderArea(i, m_vecFactionColor[(*info)[i].color - 1]);
		}
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	PAUILABEL  pLabel = (PAUILABEL)pGameUIMan->
		GetDialog("Win_MyFactionColor")->GetDlgItem("Txt_MyFactionColor");

	if( m_nMyFactionColor != 0 )
	{
		A3DRECT rect;
		rect.left = pLabel->GetPos().x + pLabel->GetSize().cx + p->X;
		rect.top = pLabel->GetPos().y + 2 + p->Y;
		rect.right = rect.left + 20;
		rect.bottom = rect.top + 13;
		GetGame()->GetA3DEngine()->GetA3DDevice()->SetSourceAlpha(A3DBLEND_SRCALPHA);
		GetGame()->GetA3DEngine()->GetA3DDevice()->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		g_pA3DGDI->Draw2DRectangle(rect, A3DCOLORRGB(210, 190, 140));
		GetGame()->GetA3DEngine()->GetA3DDevice()->SetSourceAlpha(A3DBLEND_DESTCOLOR);
		GetGame()->GetA3DEngine()->GetA3DDevice()->SetDestAlpha(A3DBLEND_ZERO);
		g_pA3DGDI->Draw2DRectangle(rect, m_vecFactionColor[m_nMyFactionColor - 1]);
	}

	GetGame()->GetA3DEngine()->GetA3DDevice()->SetSourceAlpha(A3DBLEND_SRCALPHA);
	GetGame()->GetA3DEngine()->GetA3DDevice()->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	if( m_nType == FACTIONMAP_TYPE_CHALLENGE && 
		GetHostPlayer()->GetFRoleID() == GNET::_R_MASTER)
	{
		for(i = 0; i < info->size(); i++ )
			if( !((m_vecAreaType[i] & AREA_TYPE_CANCHALLENGE) ||
				(m_vecAreaType[i] & AREA_TYPE_TEMPCHALLENGED) ||
				(m_vecAreaType[i] & AREA_TYPE_OWNER)) )
				RenderArea(i, A3DCOLORRGBA(64, 64, 64, 196));
			else if( bFlash && m_vecAreaType[i] & AREA_TYPE_TEMPCHALLENGED )
				RenderArea(i, A3DCOLORRGBA(64, 64, 64, 196));
	}
	if( m_nType == FACTIONMAP_TYPE_ENTER )
	{
		for(i = 0; i < info->size(); i++ )
			if( !(m_vecAreaType[i] & AREA_TYPE_CANENTER) ||
				(int)(*info)[i].id_owner != idFaction && 
				(int)(*info)[i].id_challenger != idFaction )
				RenderArea(i, A3DCOLORRGBA(64, 64, 64, 196));
	}

	float fWindowScale = pGameUIMan->GetWindowScale();
	for(i = 0; i < info->size(); i++ )
	{
		if( (*info)[i].id_owner )
		{
			char szIcon[20];
			sprintf(szIcon, "%d_%d.dds", GetGame()->GetGameInit().iServerID, (*info)[i].id_owner);
			int nIndex = 0;
			A2DSprite *pGuildIconSprite = pGameUIMan->GetGuildIcon(szIcon, &nIndex);
			A3DPOINT2 ptCenter = PixelToScreen((*info)[i].center);
		
			pGuildIconSprite->SetCurrentItem(nIndex);
			pGuildIconSprite->DrawToBack((int)p->X + ptCenter.x - 8*fWindowScale, (int)p->Y + ptCenter.y - 8*fWindowScale);
		}
	}

	if( m_nType == FACTIONMAP_TYPE_NORMAL || m_nType == FACTIONMAP_TYPE_ENTER )
	{
		for(i = 0; i < info->size(); i++ )
			for(j = 0; j < (*info)[i].neighbours.size(); j++ )
			{
				DWORD neighbour = 0;
				DWORD k;
				for(k = 0; k < info->size(); k++ )
					if( (*info)[k].id == (*info)[i].neighbours[j])
					{
						neighbour = k;
						break;
					}
				if( neighbour < i )
					continue;
				bool bAttack = false;
				bool bDefence = false;
				if( (*info)[neighbour].id_owner && (*info)[i].id_challenger &&
					(*info)[neighbour].id_owner == (*info)[i].id_challenger )
					bDefence = true;
				if( (*info)[neighbour].id_challenger && (*info)[i].id_owner &&
					(*info)[neighbour].id_challenger == (*info)[i].id_owner )
					bAttack = true;
				if( bAttack && bDefence )
					RenderArrow(neighbour, i, m_pA2DSpriteArrow2);
				else if( bAttack )
					RenderArrow(i, neighbour, m_pA2DSpriteArrow1);
				else if( bDefence )
					RenderArrow(neighbour, i, m_pA2DSpriteArrow1);
			}
		for(i = 0; i < info->size(); i++ )
			if( (*info)[i].id_challenger != 0 )
			{
				bool b = false;
				for(j = 0; j < info->size(); j++ )
					if( (*info)[i].id_challenger == (*info)[j].id_owner )
					{
						b = true;
						break;
					}
				if( !b )
					RenderArrow(i, -1, m_pA2DSpriteArrow3);
			}
	}
	else if( m_nType == FACTIONMAP_TYPE_CHALLENGE )
	{
		for(i = 0; i < info->size(); i++ )
			for(j = 0; j < (*info)[i].neighbours.size(); j++ )
			{
				DWORD neighbour = 0;
				DWORD k;
				for(k = 0; k < info->size(); k++ )
					if( (*info)[k].id == (*info)[i].neighbours[j])
					{
						neighbour = k;
						break;
					}
				if( neighbour < i )
					continue;
				bool bAttack = false;
				bool bDefence = false;
				if( (*info)[neighbour].id_owner && (*info)[i].id_tempchallenger &&
					(*info)[neighbour].id_owner == (*info)[i].id_tempchallenger )
					bDefence = true;
				if( (*info)[neighbour].id_tempchallenger && (*info)[i].id_owner &&
					(*info)[neighbour].id_tempchallenger == (*info)[i].id_owner )
					bAttack = true;
				if( bAttack && bDefence )
					RenderArrow(neighbour, i, m_pA2DSpriteArrow2);
				else if( bAttack )
					RenderArrow(i, neighbour, m_pA2DSpriteArrow1);
				else if( bDefence )
					RenderArrow(neighbour, i, m_pA2DSpriteArrow1);
			}
		for(i = 0; i < info->size(); i++ )
			if( (*info)[i].id_tempchallenger != 0 )
			{
				bool b = false;
				for(j = 0; j < info->size(); j++ )
					if( (*info)[i].id_tempchallenger == (*info)[j].id_owner )
					{
						b = true;
						break;
					}
				if( !b )
					RenderArrow(i, -1, m_pA2DSpriteArrow3);
			}
	}

	if( m_nHoverAreaIndex != -1 )
		RenderArea(m_nHoverAreaIndex, A3DCOLORRGBA(255, 255, 255, 128));

	//	显示帮派 PVP 相关图标：还剩余哪些资源点可攻击掠夺
	A2DSprite *pA2DSpriteFactionPVPHasMine = pGameUIMan->m_pA2DSpriteFactionPVPHasMine;
	CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
	if (pA2DSpriteFactionPVPHasMine && factionPVPModel.CanShowResource()){
		CECFactionPVPModel::DomainWithResourceIterator begin = factionPVPModel.BeginDomainWithResource();
		CECFactionPVPModel::DomainWithResourceIterator end = factionPVPModel.EndDomainWithResource();
		float offsetX = 10*fWindowScale;
		float offsetY = 10*fWindowScale;
		for (CECFactionPVPModel::DomainWithResourceIterator it = begin; it != end; ++ it){
			int domain = *it;
			const DOMAIN_INFO *pDomain = CECDomainGuildInfo::Instance().Find(domain);
			if (!pDomain){
				ASSERT(false);
				continue;
			}
			A3DPOINT2 pt = PixelToScreen(pDomain->center);
			pA2DSpriteFactionPVPHasMine->DrawToBack(int(p->X + pt.x + offsetX), int(p->Y + pt.y + offsetY));
		}
	}

	return bval;
}

void CDlgGuildMap::BattleAction(int idAction, void *pData)
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	DWORD i, j;
	int idFaction = GetHostPlayer()->GetFactionID();
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if( idAction == BATTLE_ACTION_BATTLEGETMAP_RE )
	{
		BattleGetMap_Re *p = (BattleGetMap_Re *)pData;
		m_bBattleOpen = (p->status & 1) != 0;
		m_bBattleChallengeOpen = (p->status & 2) != 0;
		m_bBattleInFight = (p->status & 4) != 0;
		CECFactionPVPModel::Instance().OnGuildBattleEnter(!IsBattleChallengeOpen() && IsBattleInFight());
		if( m_bBattleOpen )
		{
			pGameUIMan->GetDialog("Win_BigMapMark")->GetDlgItem("Btn_GuildMap")->Enable(true);
		}
		if( p->retcode != ERR_SUCCESS )
		{
			pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(703),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			m_bGetMap = false;
		}
		else
		{
			m_Info.item_id = p->bonus_id;
			m_Info.count1 = p->bonus_count1;
			m_Info.count2 = p->bonus_count2;
			m_Info.count3 = p->bonus_count3;

			m_nMaxBid = p->maxbid;
			m_nMyFactionColor = 0;
			for(i = 0; i < p->cities.size(); i++)
				for(j = 0; j < info->size(); j++ )
					if( (*info)[j].id == p->cities[i].id )
					{
						(*info)[j].id_owner = p->cities[i].owner;
						switch(p->cities[i].level)
						{
						case 1:
							(*info)[j].type = DOMAIN_TYPE_3RD_CLASS;
							break;
						case 2:
							(*info)[j].type = DOMAIN_TYPE_2ND_CLASS;
							break;
						case 3:
							(*info)[j].type = DOMAIN_TYPE_1ST_CLASS;
							break;
						default:
							(*info)[j].type = DOMAIN_TYPE_NULL;
						}
						(*info)[j].id_challenger = p->cities[i].challenger;
						(*info)[j].battle_time = p->cities[i].battle_time;
						(*info)[j].color = p->cities[i].color;
						(*info)[j].deposit = p->cities[i].deposit;
						(*info)[j].maxbonus = p->cities[i].maxbonus;
						if( (*info)[j].id_owner == idFaction )
							m_nMyFactionColor = (*info)[j].color;
						break;
					}
			CECFactionPVPModel::Instance().OnGuildBattleDomainOwnerChange();
			abase::hash_map<int,bool> mapFactonID;
			mapFactonID.clear();
			abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
			CECFactionMan *pMan = GetGame()->GetFactionMan();
			for(j = 0; j < info->size(); j++)
			{
				if( (*info)[j].id_owner && mapFactonID.find((*info)[j].id_owner) == mapFactonID.end() )
				{
					pMan->GetFaction((*info)[j].id_owner, true);
					mapFactonID[(*info)[j].id_owner] = true;
				}
				if( (*info)[j].id_challenger && mapFactonID.find((*info)[j].id_challenger) == mapFactonID.end() )
				{
					pMan->GetFaction((*info)[j].id_challenger, true);
					mapFactonID[(*info)[j].id_challenger] = true;
				}
			}
//			CELPrecinct *pPrecinct = GetWorld()->GetCurPrecinct();
//			if( pPrecinct && pPrecinct->GetDomainID() )
//				pGameUIMan->ChangePrecinct(pPrecinct);

//			(*info)[0].id_owner = 194;
//			(*info)[1].id_owner = 56;
//			(*info)[0].id_challenger = 0;
//			(*info)[1].id_challenger = 194;
			UpdateNormalMap();
			if( IsShow() && m_nType == FACTIONMAP_TYPE_CHALLENGE )
				GetGameSession()->battle_ChallengeMap();
//			PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pGameUIMan->
//				GetDialog("Win_MyFactionColor")->GetDlgItem("Img_MyFactionColor");
//			if( m_nMyFactionColor != 0 )
//				pImage->SetColor(m_vecFactionColor[m_nMyFactionColor - 1]);
		}
	}
	else if( idAction == BATTLE_ACTION_BATTLECHALLENGEMAP_RE )
	{
		BattleChallengeMap_Re *p = (BattleChallengeMap_Re *)pData;
		if( p->retcode != ERR_SUCCESS )
		{
			pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(703),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
		{
			SetBattleChallengeRandNum(p->rand_num);

			if( p->maxbonus < BATTLEFUND_MAX )
				pGameUIMan->m_pDlgGMapChallenge->SetData(p->maxbonus);
			else
				pGameUIMan->m_pDlgGMapChallenge->SetData(BATTLEFUND_MAX);
			for(i = 0; i < p->cities.size(); i++)
				for(j = 0; j < info->size(); j++ )
					if( (*info)[j].id == p->cities[i].id )
					{
						(*info)[j].id_tempchallenger = p->cities[i].challenger;
						(*info)[j].deposit = p->cities[i].deposit;
						(*info)[j].maxbonus = p->cities[i].maxbonus;
						(*info)[j].cutoff_time = p->cities[i].cutoff_time;
						break;
					}
			abase::hash_map<int,bool> mapFactonID;
			mapFactonID.clear();
			abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
			CECFactionMan *pMan = GetGame()->GetFactionMan();
			for(j = 0; j < info->size(); j++)
				if( (*info)[j].id_tempchallenger && mapFactonID.find((*info)[j].id_tempchallenger) == mapFactonID.end() )
				{
					pMan->GetFaction((*info)[j].id_tempchallenger, true);
					mapFactonID[(*info)[j].id_tempchallenger] = true;
				}
			UpdateChallengeMap();			
			m_bGetChallengeMap = true;
		}
	}
	else if( idAction == BATTLE_ACTION_BATTLECHALLENGE_RE )
	{
		BattleChallenge_Re *p = (BattleChallenge_Re *)pData;
		int index = -1;
		int nIndexChallenged = -1;
		for(j = 0; j < info->size(); j++ )
			if( (*info)[j].id == p->id )
			{
				index = j;
				break;
			}
		ACString szText;
		if( index != -1 )
		{
			if( p->retcode == ERR_SUCCESS )
			{
				szText.Format(GetStringFromTable(700), (*info)[index].name);
				(*info)[index].id_tempchallenger = p->challenger;
				(*info)[index].maxbonus = p->maxbonus;
				(*info)[index].deposit = p->deposit;
				(*info)[index].cutoff_time = p->cutoff_time;
			}
			else if( p->retcode == ERR_AS_MAILBOXFULL )
				szText.Format(GetStringFromTable(766), (*info)[index].name);
			else if( p->retcode == ERR_BS_OUTOFSERVICE )
				szText.Format(GetStringFromTable(764), (*info)[index].name);
			else if( p->retcode == ERR_BS_FAILED )
			{
				szText.Format(GetStringFromTable(765), (*info)[index].name);
			}
			else
				szText.Format(GetStringFromTable(765), (*info)[index].name);

			pGameUIMan->MessageBox("ChallengeErr", szText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		UpdateChallengeMap();			
	}
	else if( idAction == BATTLE_ACTION_BATTLESTATUS_RE )
	{
		BattleStatus_Re *p = (BattleStatus_Re *)pData;
		for(j = 0; j < p->cities.size(); j++ )
			if( p->cities[j] )
				m_vecAreaType[j] |= AREA_TYPE_CANENTER;
	}
	else if( idAction == BATTLE_ACTION_BATTLEENTER_RE )
	{
		BattleEnter_Re *p = (BattleEnter_Re *)pData;
		if( p->retcode == ERR_BS_NEWBIE_BANNED )
		{
			pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(767), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else if( p->retcode != ERROR_SUCCESS )
		{
			pGameUIMan->MessageBox("ChallengeErr", GetStringFromTable(729), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
}

void CDlgGuildMap::UpdateNormalMap()
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	int idFaction = GetHostPlayer()->GetFactionID();
	DWORD j;
	for(j = 0; j < info->size(); j++ )
		m_vecAreaType[j] &= ~(AREA_TYPE_CHALLENGED | AREA_TYPE_BECHALLENGED | AREA_TYPE_OWNER);

	if( idFaction )
	{
		for(j = 0; j < info->size(); j++ )
		{
			if( (*info)[j].id_challenger == idFaction )
				m_vecAreaType[j] |= AREA_TYPE_CHALLENGED;
			if( (*info)[j].id_owner == idFaction )
			{
				m_vecAreaType[j] |= AREA_TYPE_OWNER;
				if( (*info)[j].id_challenger != 0 ) 
					m_vecAreaType[j] |= AREA_TYPE_BECHALLENGED;
			}
		}
	}
}

void CDlgGuildMap::UpdateChallengeMap()
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	int idFaction = GetHostPlayer()->GetFactionID();
	int nDomain = 0;
	DWORD j, k;
	for(j = 0; j < info->size(); j++ )
		m_vecAreaType[j] &= ~(AREA_TYPE_CANCHALLENGE | AREA_TYPE_TEMPCHALLENGED);
	if( idFaction )
	{
		for(j = 0; j < info->size(); j++ )
			if( (*info)[j].id_owner == idFaction )
				nDomain++;
		int nChallenged = 0;
		for(j = 0; j < info->size(); j++ )
			if( (*info)[j].id_tempchallenger == idFaction )
			{
				m_vecAreaType[j] |= AREA_TYPE_TEMPCHALLENGED;
				nChallenged++;
			}
		
		if( nChallenged < m_nMaxBid )
		{
			if( nDomain == 0 )
			{
				for(j = 0; j < info->size(); j++ )
					if( (*info)[j].id_owner != idFaction &&
						(*info)[j].type == DOMAIN_TYPE_3RD_CLASS &&
						!(m_vecAreaType[j] & AREA_TYPE_TEMPCHALLENGED) )
						m_vecAreaType[j] |= AREA_TYPE_CANCHALLENGE;
			}
			else
			{
				for(j = 0; j < info->size(); j++ )
					if( (*info)[j].id_owner == idFaction ) 
						for(k = 0; k < (*info)[j].neighbours.size(); k++)
						{
							DWORD neighbour = 0;
							DWORD l;
							for(l = 0; l < info->size(); l++ )
								if( (*info)[l].id == (*info)[j].neighbours[k])
								{
									neighbour = l;
									break;
								}
							if( (*info)[neighbour].id_owner != idFaction &&
								nDomain >= (*info)[neighbour].type )
								m_vecAreaType[neighbour] |= AREA_TYPE_CANCHALLENGE;
						}
			}
		}
	}
}

bool CDlgGuildMap::GetFactionColor(int colorIndex, A3DCOLOR & color)const{
	if (colorIndex < 1 || colorIndex > (int)m_vecFactionColor.size()){
		return false;
	}
	color = m_vecFactionColor[colorIndex-1];
	return true;
}