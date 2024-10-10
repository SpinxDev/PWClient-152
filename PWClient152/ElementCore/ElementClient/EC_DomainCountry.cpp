// Filename	: EC_DomainCountry.cpp
// Creator	: Han Guanghui
// Date		: 2012/7/19

#include "EC_DomainCountry.h"
#include "A2DSprite.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "elementdataman.h"
#include "EC_CountryConfig.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "DlgMiniMap.h"
#include "EC_TimeSafeChecker.h"

#include "A3DGDI.h"
#include "A3DTexture.h"

static const A3DCOLOR HOVER_COLOR = A3DCOLORRGBA(0, 229, 255, 128);
static const A3DCOLOR ROUTE_COLOR = A3DCOLORRGBA(255, 255, 255, 200);
static const A3DCOLOR POSITION_COLOR = A3DCOLORRGBA(250, 219, 6, 128);
static const A3DCOLOR NEAR_WAR_COLOR = A3DCOLORRGBA(255, 255, 0, 128);
static const A3DCOLOR AFTER_WAR_COLOR = A3DCOLORRGBA(0, 0, 255, 128);

static const char* PATH_CURPOS_IMG = "图标\\跑.tga";
static const char* PATH_CURPOS_MIRROR_IMG = "图标\\跑2.tga";
static const char* PATH_BATTLE_IMG = "Number\\战斗中.tga";
static const char* PATH_CURR_DOMAIN_IMG = "Number\\玩家所在区域.tga";
static const char* COUNTRY_IMG_PATH[DOMAIN2_INFO::COUNTRY_COUNT] = 
	{"number\\青帝.tga", "number\\赤帝.tga", "number\\白帝.tga", "number\\黑帝.tga"};
static const char* DOMAIN_STATE_IMG_PATH[DOMAIN2_INFO::DOMAIN_STATE_NUM] =
	{"king\\疾援令小.tga", "king\\疾援令大.tga", "king\\坚壁清野.tga", "king\\坚壁清野22.tga"};

// 战斗图标宽度
static const int LEN_BATTLE = 20;

static const DWORD			 MOVE_ANIMATION_COUNT = 70;
static const unsigned long   MOVE_ANIMATION_FRAMES = 5;
static const DWORD			 DOMAIN_ANIMATION_COUNT = 100;
static const unsigned long	 DOMAIN_ANIMATION_FRAMES = 2;


//	class CECDomainCountryInfo
CECDomainCountryInfo::CECDomainCountryInfo()
: m_iDataType(-1)
, m_pDomainCountryInfo(NULL)
, m_DomainSize(0)
{
}

CECDomainCountryInfo::~CECDomainCountryInfo()
{

}

CECDomainCountryInfo* CECDomainCountryInfo::GetInstance()
{
	static CECDomainCountryInfo instance;
	return &instance;
}

bool CECDomainCountryInfo::Initialize(int iDataType)
{
	if (iDataType != m_iDataType){
		ClearState();
		m_pDomainCountryInfo = NULL;
		m_DomainSize = 0;
		m_mDomain2.clear();
		m_iDataType = -1;

		m_pDomainCountryInfo = globaldata_getdomain2infos(iDataType);
		if (m_pDomainCountryInfo){
			m_iDataType = iDataType;
			m_DomainSize = m_pDomainCountryInfo->size();
			for (int i(0); i < m_DomainSize; ++ i)
			{
				Domain2MapInfo info;
				info.pInfo = &(*m_pDomainCountryInfo)[i];
				info.Index = i;
				m_mDomain2[(info.pInfo)->id] = info;
			}
		}
	}
	return m_pDomainCountryInfo != NULL;
}

const void* CECDomainCountryInfo::FindInfo(int iDomainID) const
{
	return Find(iDomainID);
}

const DOMAIN2_INFO* CECDomainCountryInfo::Find(int iDomainID)const
{
	const DOMAIN2_INFO *ret = NULL;
	DOMAIN2MAP::const_iterator cit = m_mDomain2.find(iDomainID);
	if (cit != m_mDomain2.end())
		ret = (cit->second).pInfo;
	return ret;
}

DOMAIN2_INFO* CECDomainCountryInfo::Find(int iDomainID)
{
	DOMAIN2_INFO *ret = NULL;
	DOMAIN2MAP::iterator it = m_mDomain2.find(iDomainID);
	if (it != m_mDomain2.end())
		ret = (it->second).pInfo;
	return ret;
}

int CECDomainCountryInfo::GetIndex(int iDomainID) const
{
	int ret = -1;
	DOMAIN2MAP::const_iterator cit = m_mDomain2.find(iDomainID);
	if (cit != m_mDomain2.end())
		ret = (cit->second).Index;
	return ret;
}

namespace
{
	struct DomainNode
	{
		const DOMAIN2_INFO* p;
		int		elapsed;		//	起始节点中心移到此节点前一节点中心用时（用于冷却）
		int		timecost;		//	起始节点中心移到此节点中心用时
		int		parent;			//	前一节点ID
		
		DomainNode()
			: p(0), timecost(0), elapsed(0), parent(-1){}
		
		DomainNode(const DOMAIN2_INFO *_p, int _elapsed, int _timecost, int _parent)
			: p(_p), elapsed(_elapsed), timecost(_timecost), parent(_parent){}

		bool operator == (int iDomainID)const{
			return iDomainID == p->id;
		}
		
		struct Less
		{
			bool operator()(const DomainNode& lhs, const DomainNode& rhs) const
			{
				return lhs.timecost < rhs.timecost;
			}
		};
	};
}

CECDomainCountryInfo::FindRouteResult  CECDomainCountryInfo::FindRoute(int srcDomainID, int dstDomainID, Route &route)const
{
	//	从玩家当前领土 srcDomainID 查找到达目标领土 dstDomainID 的路径

	FindRouteResult ret = FRR_INVALID;
	route.clear();

	while (true)
	{
		if (!g_pGame ||
			!g_pGame->GetGameRun() ||
			!g_pGame->GetGameRun()->GetHostPlayer())
			break;

		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (!pHost->HostIsReady() ||
			!pHost->GetCountry())
			break;

		const DOMAIN2_INFO *pSrc = Find(srcDomainID);
		const DOMAIN2_INFO *pDst = Find(dstDomainID);
		if (!pSrc || !pDst)
			break;

		if (srcDomainID == dstDomainID)
		{
			ret = FRR_INPLACE;
			break;
		}
		
		int idCountry = pHost->GetCountry();

		//	检查起点
		if (!pSrc->IsInvolved(idCountry) ||		//	不是本国相关领土
			pSrc->IsNearWar() ||				//	是本国相关领土，但即将或已经开战（此时可通过其它协议离开）
			pSrc->IsAtWar())
		{
			ret = FRR_LOCKED;
			break;
		}

		//	检查终点（避免不必要搜索）
		if (pDst->IsOthersCapital(idCountry))
		{
			ret = FRR_BLOCKED;
			break;
		}
		if (pDst->IsChallenged())
		{
			if (!pDst->IsInvolved(idCountry))
			{
				ret = FRR_OTHERS_WAR;
				break;
			}
			int nPlayerLimit = CECUIConfig::Instance().GetGameUI().GetCountryWarPlayerLimit(pDst->wartype);
			if (pDst->IsFullFor(idCountry, nPlayerLimit))
			{
				ret = FRR_FULL;
				break;
			}
		}		
		
		// 使用 A* 搜索路径
		ret = FRR_BLOCKED;
		
		typedef std::vector<DomainNode> OpenList;
		OpenList openlist;
		openlist.push_back(DomainNode(pSrc, 0, 0, -1));

		typedef abase::hash_map<int, DomainNode> CloseMap;
		CloseMap closed;
		
		DomainNode::Less comp;
		while (!openlist.empty())
		{
			DomainNode current = *openlist.begin();
			openlist.erase(openlist.begin());			
			if (current.p->id == dstDomainID)
			{
				if (pDst->IsCoolDownFor(timeGetTime(), (DWORD)(1000*current.elapsed), idCountry))
				{
					ret = FRR_COOLDOWN;
					break;
				}

				route.setCost(current.timecost);

				const DomainNode *to = &current;
				const DomainNode *from = to;
				CloseMap::iterator it = closed.find(current.parent);
				while (it != closed.end())
				{
					from = &it->second;
					route.addNodeReverse(to->p->id, to->timecost - from->timecost);
					to = from;
					it = closed.find(it->second.parent);
				}
				route.addNodeReverse(from->p->id, 0);

				if (pDst->IsChallenged())
				{
					ret = FRR_CAN_ASSIST;
				}
				else if (pDst->IsOwner(idCountry))
				{
					ret = FRR_CAN_MOVETO;
				}
				else ret = FRR_CAN_ATTACK;

				break;
			}
			else
			{
				int idCurrent = current.p->id;
				closed[idCurrent] = current;
				const DOMAIN2_INFO *pCurrent = current.p;
				for (size_t i(0); i < pCurrent->neighbours.size(); ++ i)
				{
					int idCandidate = pCurrent->neighbours[i];
					const DOMAIN2_INFO *pCandidate = Find(idCandidate);
					if (idCandidate != dstDomainID &&
						(pCandidate->IsChallenged() || !pCandidate->IsOwner(idCountry)))
						continue;

					CloseMap::iterator isClosed = closed.find(idCandidate);
					if (isClosed != closed.end())
						continue;

					DomainNode candidate(pCandidate, current.timecost, current.timecost + pCurrent->time_neighbours[i], idCurrent);
					OpenList::iterator it = std::find(openlist.begin(), openlist.end(), idCandidate);
					if (it == openlist.end())
					{
						openlist.push_back(candidate);
					}
					else if (it->timecost > candidate.timecost)
					{
						*it = candidate;
					}
				}
			}
			
			std::sort(openlist.begin(), openlist.end(), comp);
		}

		break;
	}

	return ret;
}

//	class CECDomainCountry
CECDomainCountry::CECDomainCountry():
m_pA2DCurrentPosition(NULL),
m_pBattleImg(NULL),
m_pA2DCurrentPositionMirror(NULL),
m_pA2DCurrentDomain(NULL)
{
	::ZeroMemory(m_pSpriteCapital, sizeof(m_pSpriteCapital));
	::ZeroMemory(m_pDomainStateImg, sizeof(m_pDomainStateImg));
}

CECDomainCountry::~CECDomainCountry()
{
	A3DRELEASE(m_pA2DCurrentPosition);
	A3DRELEASE(m_pBattleImg);
	A3DRELEASE(m_pA2DCurrentPositionMirror);
	A3DRELEASE(m_pA2DCurrentDomain);
	int i;
	for (i = 0; i < DOMAIN2_INFO::COUNTRY_COUNT; ++i)
	{
		A3DRELEASE(m_pSpriteCapital[i]);
	}
	for (i = 0; i < DOMAIN2_INFO::DOMAIN_STATE_NUM; ++i)
	{
		A3DRELEASE(m_pDomainStateImg[i]);
	}
}

void CECDomainCountry::Load2DSprite(A2DSprite*& p2DSprite, const char* strPath)
{
	if (p2DSprite)
	{
		return;
	}
	p2DSprite = new A2DSprite;
	if (p2DSprite)
	{
		if (p2DSprite->Init(g_pGame->GetA3DEngine()->GetA3DDevice(), strPath, 0))
		{
			p2DSprite->SetLocalCenterPos(
				p2DSprite->GetWidth() / 2,
				p2DSprite->GetHeight() / 2 );
				p2DSprite->SetLinearFilter(true);		
		}
		else
		{
			ASSERT(false);
			delete p2DSprite;
			p2DSprite = NULL;
		}
	}
}

void CECDomainCountry::SetImageRects(A2DSprite* p2DSprite, int nNumX, int nNumY)
{
	if (NULL == p2DSprite) return;
	int iWidth = p2DSprite->GetWidth();
	int iHeight = p2DSprite->GetHeight();
	int W = iWidth / nNumX;
	int H = iHeight / nNumY;
	A3DRECT* rc = (A3DRECT*)a_malloctemp(sizeof(A3DRECT) * (nNumX * nNumY));
	int i, j, nIndex;
	for( i = 0; i < nNumY; i++ )
	{
		for( j = 0; j < nNumX; j++ )
		{
			nIndex = i * nNumX + j;
			rc[nIndex].SetRect(j * W, i * H, j * W + W, i * H + H);
		}
	}
	p2DSprite->ResetItems(nNumX * nNumY, rc);
	a_freetemp(rc);
}
void CECDomainCountry::SetImage(PAUIIMAGEPICTURE pImgDomain)
{
	m_Domain.SetDomainInfo(pImgDomain, CECDomainCountryInfo::GetInstance());
	Load2DSprite(m_pA2DCurrentDomain, PATH_CURR_DOMAIN_IMG);
	SetImageRects(m_pA2DCurrentDomain, 1, DOMAIN_ANIMATION_FRAMES);
	Load2DSprite(m_pA2DCurrentPosition, PATH_CURPOS_IMG);
	if (m_pA2DCurrentPosition)
	{
		SetImageRects(m_pA2DCurrentPosition, 1, MOVE_ANIMATION_FRAMES);
		m_pA2DCurrentPosition->SetLocalCenterPos(0, 0);
	}
	Load2DSprite(m_pA2DCurrentPositionMirror, PATH_CURPOS_MIRROR_IMG);
	if (m_pA2DCurrentPositionMirror)
	{
		SetImageRects(m_pA2DCurrentPositionMirror, 1, MOVE_ANIMATION_FRAMES);
		m_pA2DCurrentPositionMirror->SetLocalCenterPos(0, 0);
	}
	Load2DSprite(m_pBattleImg, PATH_BATTLE_IMG);
	int i;
	for (i = 0; i < DOMAIN2_INFO::COUNTRY_COUNT; ++i)
	{
		Load2DSprite(m_pSpriteCapital[i], COUNTRY_IMG_PATH[i]);
	}
	for (i = 0; i < DOMAIN2_INFO::DOMAIN_STATE_NUM; ++i)
	{
		Load2DSprite(m_pDomainStateImg[i], DOMAIN_STATE_IMG_PATH[i]);
	}
	m_AnimPosition.SetImage(m_pA2DCurrentPosition, MOVE_ANIMATION_COUNT, MOVE_ANIMATION_FRAMES);
	m_AnimPositionMirror.SetImage(m_pA2DCurrentPositionMirror, MOVE_ANIMATION_COUNT, MOVE_ANIMATION_FRAMES);
	m_AnimDomain.SetImage(m_pA2DCurrentDomain, DOMAIN_ANIMATION_COUNT, DOMAIN_ANIMATION_FRAMES);
}

void CECDomainCountry::Render(const RenderParam& param)
{
	CECDomainCountryInfo* countryInfo = CECDomainCountryInfo::GetInstance();
	int iCurrentIndex = countryInfo->GetIndex(param.iCurrentDomain);
	size_t size_domain = countryInfo->GetSize();
	size_t i;
	for (i = 0; i < size_domain; ++i)
	{
		DOMAIN2_INFO* pInfo = (DOMAIN2_INFO*)countryInfo->GetInfo(i);
		DWORD countrycolor = CECCountryConfig::Instance().GetColor(pInfo->owner);
		A3DCOLOR color;
		if (i == (size_t)param.iHoverIndex)	color = HOVER_COLOR;
		else if (i == (size_t)iCurrentIndex) color = POSITION_COLOR;
		else color = countrycolor;
		m_Domain.RenderArea(i, color);
	}
	for (i = 0; i < size_domain; ++i)
	{
		DOMAIN2_INFO* pInfo = (DOMAIN2_INFO*)countryInfo->GetInfo(i);
		switch (pInfo->status)
		{
		case D2S_NORMAL:
			break;
		case D2S_NEARWAR:
			m_Domain.RenderArea(i, NEAR_WAR_COLOR);
			break;
		case D2S_ATWAR:
			m_Domain.RenderImage(i, m_pBattleImg);
			break;
		case D2S_AFTERWAR:
			m_Domain.RenderArea(i, AFTER_WAR_COLOR);
			break;
		}

		if( (pInfo->byStateMask & DOMAIN2_INFO::STATE_SMALLTOKEN) ||
			(pInfo->byStateMask & DOMAIN2_INFO::STATE_BIGTOKEN) )
		{
			if( pInfo->byStateMask & DOMAIN2_INFO::STATE_RESTRICT )
				m_Domain.RenderImage(i, m_pDomainStateImg[3]);
			else
			{
				if( pInfo->byStateMask & DOMAIN2_INFO::STATE_SMALLTOKEN )
					m_Domain.RenderImage(i, m_pDomainStateImg[0]);
				else
					m_Domain.RenderImage(i, m_pDomainStateImg[1]);
			}
		}
		else if( pInfo->byStateMask & DOMAIN2_INFO::STATE_RESTRICT )
			m_Domain.RenderImage(i, m_pDomainStateImg[2]);
	}
	for (i = 0; i < size_domain; ++i)
	{
		DOMAIN2_INFO* pInfo = (DOMAIN2_INFO*)CECDomainCountryInfo::GetInstance()->GetInfo(i);
		if (pInfo->iscapital && CECCountryConfig::ValidateID(pInfo->owner))
		{
			m_Domain.RenderImage(i, m_pSpriteCapital[pInfo->owner - 1]);
		}
		if (i == (size_t)iCurrentIndex)
		{
			m_AnimDomain.LoopImageAnimation();
			m_Domain.RenderImage(i, m_pA2DCurrentDomain);
		}
	}
	RenderRoute(param.route);
	int iNextIndex = CECDomainCountryInfo::GetInstance()->GetIndex(param.iNextDomain);
	if (-1 != iCurrentIndex && -1 != iNextIndex)
	{
		RenderMove(iCurrentIndex, iNextIndex, param.StartTime, param.EndTime, param.CurrenTime);
	}
}

void CECDomainCountry::ResizeImages()
{
	PAUIIMAGEPICTURE pDomain = m_Domain.GetDomainImg();
	float RadioW = (float)pDomain->GetRect().Width() / pDomain->GetImage()->GetWidth();
	float RadioH = (float)pDomain->GetRect().Height() / pDomain->GetImage()->GetHeight();
	if (m_pBattleImg)
	{
		m_pBattleImg->SetScaleX(RadioW);
		m_pBattleImg->SetScaleY(RadioH);
	}
	int i;
	for (i = 0; i < DOMAIN2_INFO::COUNTRY_COUNT; ++i)
	{
		if (m_pSpriteCapital[i])
		{
			m_pSpriteCapital[i]->SetScaleX(RadioW);
			m_pSpriteCapital[i]->SetScaleY(RadioH);
		}
	}
	for (i = 0; i < DOMAIN2_INFO::DOMAIN_STATE_NUM; ++i)
	{
		if (m_pDomainStateImg[i])
		{
			m_pDomainStateImg[i]->SetScaleX(RadioW);
			m_pDomainStateImg[i]->SetScaleY(RadioH);
		}
	}
}

void CECDomainCountry::RenderRoute(const CECDomainCountryInfo::Route& route)
{
	int len = route.size();
	if (len < 2) return;

	const CECDomainCountryInfo *pCountryInfo = CECDomainCountryInfo::GetInstance();

	const DOMAIN2_INFO *pStart = pCountryInfo->Find(route.nodes[0].idDomain);
	if (!pStart) return;
	A3DPOINT2 start = m_Domain.PixelToScreen(pStart->center);

	const DOMAIN2_INFO *pEnd = NULL;
	A3DPOINT2 end;

	CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgMiniMap* pDlgMini = dynamic_cast<CDlgMiniMap*>(pGameUIMan->GetDialog("Win_Map"));

	for (int i = 1; i < len; ++i)
	{
		pEnd = pCountryInfo->Find(route.nodes[i].idDomain);
		if (!pEnd) return;
		end = m_Domain.PixelToScreen(pEnd->center);
		pDlgMini->DrawPath(start.x, start.y, end.x, end.y, 6);
		pStart = pEnd;
		start = end;
	}
}

void CECDomainCountry::RenderMove(int iCurrentIndex, int iNextIndex, DWORD StartTime, DWORD EndTime, DWORD CurrentTime)
{
	if(-1 == iNextIndex || CurrentTime > EndTime) return;
	A3DPOINT2 ptStart = m_Domain.GetDomainInfo()->GetCenter(iCurrentIndex);
	A3DPOINT2 ptEnd = m_Domain.GetDomainInfo()->GetCenter(iNextIndex);
	A3DPOINT2 ptDirection = ptEnd - ptStart;
	A3DVECTOR3 vec3D(ptDirection.x, ptDirection.y, 0);
	float len = vec3D.Magnitude();
	vec3D.Normalize();
	A2DSprite* pSprite = NULL;
	if (vec3D.x > 0)
	{
		pSprite = m_pA2DCurrentPosition;
		m_AnimPosition.LoopImageAnimation();
	}
	else
	{
		pSprite = m_pA2DCurrentPositionMirror;
		m_AnimPositionMirror.LoopImageAnimation();
	}
	
	float movePercent = (EndTime > StartTime) ? (float)(CurrentTime - StartTime) / (EndTime - StartTime) : 0.0f;
	a_Clamp(movePercent, 0.0f, 1.0f);
	float movedlen = movePercent * len;
	A3DPOINT2 ptOffset(movedlen * vec3D.x, movedlen * vec3D.y);
	m_Domain.RenderImageOffsetCenter(iCurrentIndex, ptOffset, pSprite, true);
}

bool CECDomainCountry::IsHoverDomainStateImage(const A3DPOINT2& pt)
{
	A3DRECT rcImage;
	int iHoverIdx = m_Domain.GetDomainIndexByCoordinate(pt);
	if( iHoverIdx >= 0 )
	{
		DOMAIN2_INFO* pDomain = (DOMAIN2_INFO*)CECDomainCountryInfo::GetInstance()->GetInfo(iHoverIdx);
		if( pDomain->byStateMask == 0 ) return false;

		A3DPOINT2 ptCenter = CECDomainCountryInfo::GetInstance()->GetCenter(iHoverIdx);
		int w = m_pDomainStateImg[0]->GetWidth() >> 1;
		int h = m_pDomainStateImg[0]->GetHeight() >> 1;
		rcImage.SetRect(ptCenter.x - w, ptCenter.y - h, ptCenter.x + w, ptCenter.y + h);
		A3DPOINT2 ptReal = m_Domain.ScreenToPixel(pt);
		return rcImage.PtInRect(ptReal);
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////

CSequenceImageAnimator::CSequenceImageAnimator():
m_pA2DSprite(NULL)
,m_dwTickCount(0)
,m_dwTickStart(0)
,m_ulFrames(0)
,m_ulCurrFrame(0)
{
}

void CSequenceImageAnimator::SetImage(A2DSprite* pSprite, DWORD dwTickCount, unsigned long ulFrames)
{
	m_pA2DSprite = pSprite;
	m_dwTickCount = dwTickCount;
	m_dwTickStart = 0;
	m_ulFrames = ulFrames;
	m_ulCurrFrame = ulFrames - 1;
}

void CSequenceImageAnimator::LoopImageAnimation()
{
	DWORD dwTickCount = GetTickCount();
	if (m_pA2DSprite && (CECTimeSafeChecker::ElapsedTime(dwTickCount, m_dwTickStart) > m_dwTickCount))
	{
		m_dwTickStart = dwTickCount;
		++m_ulCurrFrame;
		if (m_ulCurrFrame > m_ulFrames)	m_ulCurrFrame = 0;
		m_pA2DSprite->SetCurrentItem(m_ulCurrFrame);
	}
}


