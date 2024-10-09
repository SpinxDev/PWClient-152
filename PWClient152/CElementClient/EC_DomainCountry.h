// Filename	: EC_DomainCountry.h
// Creator	: Han Guanghui
// Date		: 2012/7/19

#ifndef _EC_DOMAIN_COUNTRY_H_ 
#define _EC_DOMAIN_COUNTRY_H_

#include "EC_DomainBase.h"
#include <hashmap.h>


class CECDomainCountryInfo : public CECDomainInfo
{
public:
	CECDomainCountryInfo();
	virtual ~CECDomainCountryInfo();
	static	CECDomainCountryInfo* GetInstance(); 

	bool Initialize(int iDataType);
	int	GetDataType()const{ return m_iDataType; }

	virtual const DomainVerts& GetVerts(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainCountryInfo)[iIndex].verts;
	}
	virtual const DomainFaces& GetFaces(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainCountryInfo)[iIndex].faces;
	}
	virtual const A3DPOINT2& GetCenter(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainCountryInfo)[iIndex].center;
	}
	virtual const void*		GetInfo(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (void*)(&(*m_pDomainCountryInfo)[iIndex]);
	}
	virtual const void*		FindInfo(int iDomainID) const;
	virtual size_t GetSize() const
	{
		return m_DomainSize;
	}
	virtual int GetIndex(int iDomainID) const;

	//	本类增加成员
	const DOMAIN2_INFO* Find(int iDomainID)const;
	DOMAIN2_INFO* Find(int iDomainID);
	
	const DOMAIN2_INFO& Get(int iIndex)const{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainCountryInfo)[iIndex];
	}
	DOMAIN2_INFO& Get(int iIndex){		
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainCountryInfo)[iIndex];
	}

	void ClearState(){
		for (int i(0); i < m_DomainSize; ++ i)
			Get(i).ClearState();
	}

	bool Validate(int *pBreakIndex=NULL)const{
		bool valid = true;
		int i = 0;
		for (; (i < m_DomainSize) && valid; ++ i)
			valid = Get(i).Validate();
		if (!valid && pBreakIndex)
			*pBreakIndex = i;
		return valid;
	}
	
	enum FindRouteResult
	{
		FRR_INVALID,		//	无效的起点或/和终点，或其它查找状态错误
		FRR_INPLACE,		//	起点与终点相同
		FRR_LOCKED,			//	不可移动，当前领土是他国领土、或本国领土但即将或已经开战
		FRR_CAN_MOVETO,		//	是本国领土（未开战），可以移至
		FRR_CAN_ATTACK,		//	是他国领土（未开战），可以进攻
		FRR_CAN_ASSIST,		//	是本国战场，可以参战
		FRR_FULL,			//	是本国战场，人数已满，不可达
		FRR_COOLDOWN,		//	是他国领土，战后冷却中，不可达
		FRR_OTHERS_WAR,		//	是他国战场，不可达
		FRR_BLOCKED,		//	路径不通，不可达
	};
	struct RouteNode
	{
		int	idDomain;
		int	cost;
		RouteNode():idDomain(-1), cost(0){}
		RouteNode(int _id, int _cost):idDomain(_id), cost(_cost){}
	};
	struct Route 
	{
		typedef abase::vector<RouteNode> RouteNodes;
		RouteNodes	nodes;	//	经过领土（包含起点和终点）
		int			cost;	//	消耗时间
		
		void clear(){nodes.clear(); cost = 0;}
		void addNodeReverse(int iDomainID, int cost){ nodes.insert(nodes.begin(), RouteNode(iDomainID, cost));}
		size_t size()const{ return nodes.size(); }
		bool empty()const{ return nodes.empty(); }
		void setCost(int seconds){ cost = seconds; }
		int  getSrc()const{ return nodes.empty() ? -1 : nodes.front().idDomain; }
		int  getDst()const{ return nodes.empty() ? -1 : nodes.back().idDomain; }
		int  getNext()const{ return size() < 2 ? -1 : nodes[1].idDomain; }
		void swap(Route &rhs)
		{
			abase::swap(cost, rhs.cost);
			nodes.swap(rhs.nodes);
		}
		int	find(int iDomainID)const
		{
			int index(-1);
			for (size_t i(0); i < size(); ++ i)
			{
				const RouteNode &node = nodes[i];
				if (node.idDomain == iDomainID)
				{
					index = (int)i;
					break;
				}
			}
			return index;
		}
		void removeHead(size_t count)
		{
			if (count <= 0 || size() < count)
				return;
			while (count --)
			{
				nodes.erase(nodes.begin());
				if (!nodes.empty())
				{
					RouteNode &head = nodes.front();
					cost -= head.cost;
					head.cost = 0;
				}
			}
		}
		void removeTail(size_t count)
		{
			if (count <= 0 || size() < count)
				return;
			int p = count;
			while (p --)
			{
				RouteNode &tail = nodes.back();
				cost -= tail.cost;
			}
			nodes.erase(nodes.begin()+(size()-count), nodes.end());
		}
	};
	FindRouteResult FindRoute(int srcDomainID, int dstDomainID, Route &route)const;
	
protected:
	int							 m_iDataType;
	abase::vector<DOMAIN2_INFO>* m_pDomainCountryInfo;
	int							 m_DomainSize;

	struct Domain2MapInfo
	{
		DOMAIN2_INFO* pInfo;
		int			  Index;
	};
	typedef abase::hash_map<int, Domain2MapInfo>	DOMAIN2MAP;
	DOMAIN2MAP					 m_mDomain2;
};

class CSequenceImageAnimator
{
public:
	CSequenceImageAnimator();
	virtual ~CSequenceImageAnimator(){};
	
	void SetImage(A2DSprite* pSprite, DWORD dwTickCount, unsigned long ulFrames);
	void LoopImageAnimation();
protected:
	A2DSprite*			m_pA2DSprite;
	DWORD				m_dwTickStart;
	DWORD				m_dwTickCount;
	unsigned long		m_ulCurrFrame;
	unsigned long		m_ulFrames;
};

class CECDomainCountry
{
public:
	CECDomainCountry();
	virtual ~CECDomainCountry();

	void SetImage(PAUIIMAGEPICTURE pImgDomain);
	const CECDomain& GetDomain() const 
	{
		return m_Domain;
	}

	struct RenderParam
	{
		int iHoverIndex;
		int iCurrentDomain;
		int iNextDomain;
		DWORD StartTime;
		DWORD EndTime;
		DWORD CurrenTime;
		const CECDomainCountryInfo::Route& route;
		RenderParam(int hover, int current,	int next,
			DWORD starttime, DWORD endtime, DWORD currenttime,
			const CECDomainCountryInfo::Route& r):
		iHoverIndex(hover), iCurrentDomain(current), iNextDomain(next),
		StartTime(starttime), EndTime(endtime), CurrenTime(currenttime),
		route(r)
		{
		}
	};
	void Render(const RenderParam& param);
	void RenderRoute(const CECDomainCountryInfo::Route& route);
	void RenderMove(int iCurrentIndex, int iNextIndex, DWORD StartTime, DWORD EndTime, DWORD CurrentTime);
	void ResizeImages();
	static void Load2DSprite(A2DSprite*& p2DSprite, const char* strPath);
	static void SetImageRects(A2DSprite* p2DSprite, int nNumX, int nNumY);

	// 鼠标是否悬浮在区域状态的图标上
	bool IsHoverDomainStateImage(const A3DPOINT2& pt);

protected:

	CECDomain				m_Domain;
	A2DSprite*				m_pA2DCurrentPosition;
	A2DSprite*				m_pA2DCurrentPositionMirror;
	A2DSprite*				m_pBattleImg;
	A2DSprite*				m_pSpriteCapital[DOMAIN2_INFO::COUNTRY_COUNT];
	A2DSprite*				m_pA2DCurrentDomain;
	A2DSprite*				m_pDomainStateImg[DOMAIN2_INFO::DOMAIN_STATE_NUM];
	CSequenceImageAnimator	m_AnimPosition;
	CSequenceImageAnimator	m_AnimPositionMirror;
	CSequenceImageAnimator	m_AnimDomain;
};




#endif //_EC_DOMAIN_COUNTRY_H_
