/********************************************************************
	created:	2006/10/08
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#include <A3DMacros.h>


#include "MoveAgent.h"
#include "MoveMap.h"
#include "PfClu.h"
#include "OptimizePath.h"
#include "bitimage.h"
#include "LayerMap.h"
#include "Island.h"

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//global vars

//////////////////////////////////////////////////////////////////////////
//local vars

//////////////////////////////////////////////////////////////////////////
//local funcs

class BrushTestWrapper
{
	CMoveAgent::BrushTest * m_pBrushTest;
	AutoMove::CMoveMap * m_pMoveMap;
	int				m_iLayer;
	A3DVECTOR3		m_vFrom;
public:
	BrushTestWrapper(CMoveAgent::BrushTest *pBrushTest, AutoMove::CMoveMap* pMoveMap, int iLayer)
		: m_pBrushTest(pBrushTest)
		, m_pMoveMap(pMoveMap)
		, m_iLayer(iLayer)
		, m_vFrom(0.0f)
	{
	}
	A3DVECTOR3 ToWorld(const A3DPOINT2& p2)
	{
		APointF pf = m_pMoveMap->TransMap2Wld(p2);
		return A3DVECTOR3(pf.x, m_pMoveMap->GetDH(m_iLayer, p2), pf.y);
	}
	void SetFrom(const A3DPOINT2& from)
	{
		m_vFrom = ToWorld(from);
	}
	virtual bool CanMoveTo(const A3DPOINT2& to)
	{
		return !m_pBrushTest->Collide(m_vFrom, ToWorld(to));
	}
};

//	正方形二维数组标记
class SquareMark
{
	int	m_length;
	abase::vector<abase::vector<char> >	m_marked;
public:
	SquareMark(int len) : m_length(0){
		Init(len);
	}
	void Init(int length)
	{
		if (length <= 0){
			return;
		}
		m_length = length;
		m_marked.clear();
		abase::vector<char> row(m_length, 0);
		m_marked.insert(m_marked.end(), m_length, row);
	}
	bool IsValid(const A3DPOINT2 &it)const{
		return it.x >= 0 && it.x < m_length
			&& it.y >= 0 && it.y < m_length;
	}
	bool Marked(const A3DPOINT2 &it)const{
		return m_marked[it.x][it.y]!=0;
	}
	void Mark(const A3DPOINT2 &it){
		if (IsValid(it)){
			m_marked[it.x][it.y] = 1;
		}
	}
};

//	在起点、终点周围查找可用点的基类
class CheckNeighborBase
{
	A3DPOINT2	m_origin;
	int			m_length;

protected:
	A3DPOINT2 ToWorld(const A3DPOINT2 &it)const{
		return it + m_origin;
	}
	A3DPOINT2 ToLocal(const A3DPOINT2 &pt)const{
		return pt - m_origin;
	}
	int	 GetLength()const{
		return m_length;
	}
	bool IsValid(const A3DPOINT2 &it)const{
		return it.x >= 0 && it.x < m_length
			&& it.y >= 0 && it.y < m_length;
	}
	virtual bool DoCheckImpl(A3DPOINT2 &pt, CBitImage * pRMap, BrushTestWrapper * pBrushTest)=0;

public:
	CheckNeighborBase() : m_origin(0, 0), m_length(0) {}
	virtual ~CheckNeighborBase(){}
	bool DoCheck(A3DPOINT2 &pt, int radius, CBitImage * pRMap, BrushTestWrapper * pBrushTest)
	{
		if (radius <= 0){
			assert(false);
			return false;
		}
		m_origin = pt - A3DPOINT2(radius, radius);
		m_length = radius*2+1;
		return DoCheckImpl(pt, pRMap, pBrushTest);
	}
};

//	从起点可到点
class CheckNeighborReachable : public CheckNeighborBase
{
protected:
	template <typename T>
	void swap(T &lhs, T &rhs)const
	{
		T tmp = rhs;
		rhs = lhs;
		lhs = tmp;
	}
	unsigned int GreatestCommonDivisor(unsigned int a, unsigned int b)const
	{
		//	求最大公约数，循环中、大数以除以小数的余数代替
		if (a > 0 && b > 0){
			if (a < b){
				swap(a, b);
			}
			while (true)
			{
				a = a%b;
				if (a == 0) break;
				swap(a, b);
			}
			return b;
		}
		return 0;
	}
	virtual bool DoCheckImpl(A3DPOINT2 &pt, CBitImage * pRMap, BrushTestWrapper * pBrushTest)
	{
		bool bOK = false;
		
		A3DPOINT2 center = pt;

		abase::vector<A3DPOINT2>	todo;
		SquareMark			added(GetLength());			//	被标记点已添加到 todo
		SquareMark			collide(GetLength());		//	从起点到被标记点有碰撞

		A3DPOINT2 c = ToLocal(pt);
		todo.push_back(c);
		added.Mark(c);

		int current(0);
		while (current < (int)todo.size())
		{
			A3DPOINT2 it = todo[current++];	//	此处 it 不能使用引用，todo 可能改变
			if (!collide.Marked(it)){
				pt = ToWorld(it);
				if (pRMap->GetPixel(pt.x, pt.y)){
					if (pBrushTest->CanMoveTo(pt)){
						bOK = true;
						break;
					}else{
						collide.Mark(it);
						//	为减少碰撞检测消耗时间、做如下处理
						
						//	从 center（实际为 BrushTestWrapper::SetFrom 设置的值）-> pt 有碰撞
						//	则从 center->pt 方向的连线延伸都不需要再做处理
						
						//	计算单位移动距离
						int dx(pt.x - center.x), dy(pt.y - center.y);
						if (dx != 0 || dy != 0){
							if (dx == 0){
								dy = (dy > 0 ? 1 : -1);
							}else if (dy == 0){
								dx = (dx > 0 ? 1 : -1);
							}else{
								int divisor = (int)GreatestCommonDivisor(abs(dx), abs(dy));
								dx /= divisor;
								dy /= divisor;
							}
							
							//	从 center 按单位距离移动，范围内的点都设为已标记，后续不再处理
							A3DPOINT2 p = todo[0];
							while (true)
							{
								p.x += dx;
								p.y += dy;
								if (!IsValid(p)){
									break;
								}
								if (!collide.Marked(p)){
									collide.Mark(p);
								}
							}
						}
					}
				}
			}

			//	发现邻居，只要是存在点、未添加到 todo 中的，都添加
			A3DPOINT2 neighbor;
			for (int i = 0; i < PF_NEIGHBOR_COUNT; i++)
			{
				neighbor = it;
				neighbor += A3DPOINT2(PF2D_NeighborD[i*2], PF2D_NeighborD[i*2+1]);
				if (IsValid(neighbor) && !added.Marked(neighbor)){
					todo.push_back(neighbor);
					added.Mark(neighbor);
				}
			}
		}
		return bOK;
	}
};

//	可达图上查找周围任意可达点
class CheckNeighborAny : public CheckNeighborBase
{
protected:
	virtual bool DoCheckImpl(A3DPOINT2 &pt, CBitImage * pRMap, BrushTestWrapper * pBrushTest)
	{
		//	在周围可达图找到可达点即可
		bool bOK = false;
				
		abase::vector<A3DPOINT2>	todo;
		SquareMark			added(GetLength());
		
		A3DPOINT2 c = ToLocal(pt);
		todo.push_back(c);
		added.Mark(c);

		int current(0);
		while (current < (int)todo.size())
		{
			A3DPOINT2 it = todo[current++];	//	此处 it 不能使用引用，todo 可能改变
			pt = ToWorld(it);
			if (pRMap->GetPixel(pt.x, pt.y)){
				bOK = true;
				break;
			}
			A3DPOINT2 neighbor;
			for (int i = 0; i < PF_NEIGHBOR_COUNT; i++)
			{
				neighbor = it;
				neighbor += A3DPOINT2(PF2D_NeighborD[i*2], PF2D_NeighborD[i*2+1]);
				if (IsValid(neighbor) && !added.Marked(neighbor)){
					todo.push_back(neighbor);
					added.Mark(neighbor);
				}
			}
		}
		return bOK;
	}
};

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoveAgent::CMoveAgent()
: m_pPathOptimizer(NULL)
, m_iStat(PF_STATE_UNKNOWN)
, m_iLayerStart(-1)
, m_iLayerGoal(-1)
, m_pBrushTest(NULL)
{
	m_pMoveMap = new CMoveMap;
	m_pPfAlg = new CPfClu;
	SetPfAlg(PF_ALGORITHM_CLU_MA);
	m_ptStart.x = m_ptStart.y = 0;
	m_ptGoal.x = m_ptGoal.y = 0;
}

CMoveAgent::~CMoveAgent()
{
	Release();
}

bool CMoveAgent::Load(const char * szPath, const A3DVECTOR3 *pvOrigin)
{
	if (m_pPathOptimizer)
	{
		delete m_pPathOptimizer;
		m_pPathOptimizer = NULL;
	}
	if (!m_pMoveMap->Load(szPath))
	{
		m_pMoveMap->Release();
		return false;
	}
	m_pMoveMap->CalcOrigin();
	if (pvOrigin){
		m_pMoveMap->SetOrigin(*pvOrigin);
	}
	CreateOptimizer();
	return true;
}

void CMoveAgent::CreateOptimizer()
{
	if (m_pPathOptimizer)
	{
		delete m_pPathOptimizer;
	}
	m_pPathOptimizer= new COptimizePath;
}

A3DVECTOR3 CMoveAgent::Get2DPathNode(int index)const
{	
	abase::vector<PathNode> * path2d = Get2DPath();
	const PathNode& pathNode = path2d->at(index);
	APointF ptWld(m_pMoveMap->TransMap2Wld(pathNode.ptMap));
	return A3DVECTOR3(ptWld.x, 0.0f, ptWld.y);
}

abase::vector<PathNode> * CMoveAgent::Get2DPath() const
{
	if (!m_pPathOptimizer)
	{
		return NULL;
	}

	return m_pPathOptimizer->GetPath();
}


bool CMoveAgent::IsReady() const
{
	return (m_pPfAlg && m_pMoveMap && (m_pMoveMap->GetLayerCount()>0) && m_pMoveMap->GetMultiCluGraph());
}

void CMoveAgent::Release()
{
	if (m_pPfAlg)
	{
		delete m_pPfAlg;
		m_pPfAlg = NULL;
	}
	if(m_pPathOptimizer)
	{
		delete m_pPathOptimizer;
		m_pPathOptimizer = NULL;
	}
	A3DRELEASE(m_pMoveMap);
	m_pBrushTest = NULL;
}

int CMoveAgent::WhichLayer(const A3DPOINT2& ptMap, float dH, float *pLayerDist)
{
	return m_pMoveMap->WhichLayer(ptMap, dH, pLayerDist);
}

int CMoveAgent::WhichLayer(const A3DVECTOR3& vWld, float dH, float *pLayerDist)
{
	if (!IsContain(vWld)){
		return -1;
	}
	return WhichLayer(TransWld2Map(vWld), dH, pLayerDist);
}

int CMoveAgent::GetMinPassableLayer(const A3DPOINT2& ptMap) const
{
	return m_pMoveMap->GetMinPassableLayer(ptMap);
}

int  CMoveAgent::GetAllPassableLayers(const A3DPOINT2& ptMap,int* pLayers)const
{
	return m_pMoveMap->GetAllPassableLayers(ptMap,pLayers);
}
A3DPOINT2 CMoveAgent::TransWld2Map(const A3DVECTOR3& vWld)
{
	return m_pMoveMap->TransWld2Map(APointF(vWld.x, vWld.z));
}

bool CMoveAgent::IsContain(const A3DVECTOR3& vWld)
{
	A3DPOINT2 ptMap = TransWld2Map(vWld);
	if (ptMap.x < 0 || ptMap.x >= m_pMoveMap->GetMapWidth() ||
		ptMap.y < 0 || ptMap.y >= m_pMoveMap->GetMapLength()){
		return false;
	}
	return true;
}

void CMoveAgent::SetBrushTest(BrushTest *p)
{
	m_pBrushTest = p;
}

bool CMoveAgent::SetStartEnd(const A3DPOINT2& ptStart, int iLayerStart, const A3DPOINT2& ptGoal, int iLayerGoal)
{
	m_iStat = PF_STATE_UNKNOWN;

	CLayerMap *pLayer = m_pMoveMap->GetLayer(iLayerStart);
	if (!pLayer || !pLayer->GetRMap() || !pLayer->GetDHMap()){
		m_iStat = PF_STATE_INVALIDSTART;
		return false;
	}
	pLayer = m_pMoveMap->GetLayer(iLayerGoal);
	if (!pLayer || !pLayer->GetRMap() || !pLayer->GetDHMap()){
		m_iStat = PF_STATE_INVALIDEND;
		return false;
	}
	
	m_ptStart = ptStart;
	m_iLayerStart = iLayerStart;
	m_ptGoal  = ptGoal;
	m_iLayerGoal = iLayerGoal;

	if (!_CheckStart())
	{
		m_iStat = PF_STATE_INVALIDSTART;
		return false;
	}

	if (!_CheckEnd())
	{
		m_iStat = PF_STATE_INVALIDEND;
		return false;
	}
	

	m_pPfAlg->SetupSearch(GetMoveMap(), GetPfAlg(), m_iLayerStart, m_ptStart, m_iLayerGoal, m_ptGoal);
	m_iStat  = m_pPfAlg->GetState();
	
	return true;
}

bool CMoveAgent::SetStartEnd(const A3DVECTOR3& vStart, int iLayerStart, const A3DVECTOR3& vGoal, int iLayerGoal, BrushTest *pBrushTest/* =NULL */)
{
	if (!IsContain(vStart)){
		m_iStat = PF_STATE_INVALIDSTART;
		return false;
	}
	if (!IsContain(vGoal)){
		m_iStat = PF_STATE_INVALIDEND;
		return false;
	}
	class BrushTestSwitch
	{
		CMoveAgent*	m_pMoveAgent;
	public:
		BrushTestSwitch(CMoveAgent *pMoveAgent, BrushTest *pBrushTest) : m_pMoveAgent(pMoveAgent){
			m_pMoveAgent->SetBrushTest(pBrushTest);
		}
		~BrushTestSwitch(){
			m_pMoveAgent->SetBrushTest(NULL);
		}
	};
	BrushTestSwitch _dummy(this, pBrushTest);
	return SetStartEnd(TransWld2Map(vStart), iLayerStart, TransWld2Map(vGoal), iLayerGoal);
}


bool CMoveAgent::Search(abase::vector<PathNode>*  pPath, int nMaxExpand)
{
	if (m_iStat != PF_STATE_FOUND && m_iStat != PF_STATE_SEARCHING){
		//	状态错误，必定先成功调用 SetStartEnd
		assert(false);
		return false;
	}
	abase::vector<PathNode>  path;
	bool ret = m_pPfAlg->Search(path, nMaxExpand);
	m_iStat = m_pPfAlg->GetState();
	if (pPath)
	{
		*pPath = path;
	}

	if (ret && m_pPathOptimizer)
	{
		//m_pPathOptimizer->SetupOptimize(path,1);
		m_pPathOptimizer->SetupOptimize(GetMoveMap(), path);
	}

	return ret;
}



bool CMoveAgent::Optimize(const int moveIndex, abase::vector<A3DVECTOR3>* remain3DPath)
{
	assert(m_iStat == PF_STATE_FOUND);
	if (!m_pPathOptimizer->NeedOptimize(moveIndex))
	{
		return false;
	}

	m_pPathOptimizer->StepOptimize();

	//@todo only copy remaining path can save time???  [3/21/2007 kuiwu]
	abase::vector<PathNode> * path2d = m_pPathOptimizer->GetPath();
	
	if (remain3DPath)
	{
		remain3DPath->clear();
		unsigned int i;
		for (i = moveIndex+1; i < path2d->size(); i++)
		{
			// todo: refine 3d path  [6/2/2008 kuiwu]
			A3DVECTOR3 pos;
			APointF ptWld(m_pMoveMap->TransMap2Wld(path2d->at(i).ptMap));
			pos.x = ptWld.x;
			pos.y = 0.0f;
			pos.z = ptWld.y;
			remain3DPath->push_back(pos);
			//remain3DPath->push_back(GetMoveMap()->Trans2DTo3D(path2d->at(i)));
		}
	}

	return true;
		
}

void CMoveAgent::SetPfAlg(int alg)
{
	assert(alg > PF_ALGORITHM_NONE && alg < PF_ALGORITHM_COUNT);
	m_iPfAlg = alg;
}


bool CMoveAgent::_CheckStart()
{
	CLayerMap * pLayerMap = m_pMoveMap->GetLayer(m_iLayerStart);
	CBitImage * pRMap = pLayerMap->GetRMap();
	int radius = 10;
	if (m_pBrushTest){
		BrushTestWrapper brushTester(m_pBrushTest, GetMoveMap(), m_iLayerStart);
		brushTester.SetFrom(m_ptStart);
		CheckNeighborReachable c;
		return c.DoCheck(m_ptStart, radius, pRMap, &brushTester);
	}else{
		CheckNeighborAny c;
		return c.DoCheck(m_ptStart, radius, pRMap, NULL);
	}
}

bool CMoveAgent::_CheckEnd()
{
	
	if (_CheckThrough())
	{
		return true; 
	}

	CLayerMap * pLayerMap = m_pMoveMap->GetLayer(m_iLayerGoal);
	CBitImage * pRMap = pLayerMap->GetRMap();
	CheckNeighborAny c;
	if (c.DoCheck(m_ptGoal, 10, pRMap, NULL))
	{
		return true;
	}

	if (m_iLayerStart == m_iLayerGoal)
	{
		CLine   line;
		APointF dir(m_ptStart.x-m_ptGoal.x, m_ptStart.y-m_ptGoal.y);
		line.Init(APointF(m_ptGoal.x, m_ptGoal.y), dir);
		while (line.GetCount() < 10000)
		{
			APointF next = line.Next();
			if (pRMap->GetPixel((int)next.x, (int)next.y))
			{
				m_ptGoal.x = (int)next.x;
				m_ptGoal.y = (int)next.y;
				return true;
			}
		}
	}

	return false;	
}

//note: return true, if the ptEnd is modified
bool CMoveAgent::_CheckThrough()
{
	CLayerMap * pLayerGoal = m_pMoveMap->GetLayer(m_iLayerGoal);
	if (!pLayerGoal->GetIslandList())
	{
		return false;
	}
	CIsland * pStartIsl = NULL;
	if (m_pMoveMap->GetLayer(m_iLayerStart)->GetIslandList())
	{
		pStartIsl = m_pMoveMap->GetLayer(m_iLayerStart)->GetIslandList()->SelectIsland(m_ptStart);
	}
	CIsland * pGoalIsl = pLayerGoal->GetIslandList()->SelectIsland(m_ptGoal);		

	if (pStartIsl == pGoalIsl)
	{
		return false;
	}


	if (m_iLayerStart == m_iLayerGoal)
	{

		CIsland::Pass * pPass = pLayerGoal->GetIslandList()->FindPass(pStartIsl, pGoalIsl);
		if (!pPass)
		{
			assert(0);
			return false;
		}
		m_ptGoal = pPass->through;
		return  true;
	}
	return false;
}

void CMoveAgent::Retrieve3DPath(abase::vector<A3DVECTOR3>& path, int from /* = 0 */) const
{
	path.clear();
	abase::vector<PathNode> * path2d = Get2DPath();
	if (!path2d)
	{
		return;
	}

	int i;
	for (i = from; i < (int)path2d->size(); ++i)
	{
		const PathNode& pathNode = path2d->at(i);
		APointF ptWld(m_pMoveMap->TransMap2Wld(pathNode.ptMap));
		float   dh = m_pMoveMap->GetDH(pathNode.layer, A3DPOINT2((int)pathNode.ptMap.x, (int)pathNode.ptMap.y));
		path.push_back(A3DVECTOR3(ptWld.x, dh, ptWld.y));
	}
}

int CMoveAgent::GetOptimizeCatchCount() const
{
	if (m_pPathOptimizer)
	{
		return m_pPathOptimizer->GetCatchCount();
	}

	return 0;
}

int CMoveAgent::GetPathCount() const
{
	if (m_pPathOptimizer && m_pPathOptimizer->GetPath())
	{
		return m_pPathOptimizer->GetPath()->size();
	}

	return 0;
}
A3DVECTOR3 CMoveAgent::Get3DPathNode(const int index) const
{
	abase::vector<PathNode> * path2d = Get2DPath();
	const PathNode& pathNode = path2d->at(index);
	APointF ptWld(m_pMoveMap->TransMap2Wld(pathNode.ptMap));
	float   dh = m_pMoveMap->GetDH(pathNode.layer, A3DPOINT2((int)pathNode.ptMap.x, (int)pathNode.ptMap.y));
	
	return A3DVECTOR3(ptWld.x, dh, ptWld.y);

}

void CMoveAgent::ResetSearch()
{
	//	清除当前搜索状态
	if (m_iStat == PF_STATE_UNKNOWN){
		return;
	}
	if (m_pPfAlg){
		m_pPfAlg->Reset();
	}
	if (m_pPathOptimizer){
		m_pPathOptimizer->Reset();
	}
	m_ptStart.x = m_ptStart.y = 0;
	m_iLayerStart = -1;
	m_ptGoal.x = m_ptGoal.y = 0;
	m_iLayerGoal = -1;
	m_pBrushTest = NULL;

	m_iStat = PF_STATE_UNKNOWN;
}


}