/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	clu path finding algorithm
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/


#include "PfClu.h"
#include "MoveMap.h"
#include "MultiCluGraph.h"
#include "bitimage.h"
#include "LayerMap.h"
#include "Graph.h"
#include "GAStar.h"

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//local funcs
vector<A3DPOINT2> * l_FindPath(vector<vector<A3DPOINT2> >& paths, const A3DPOINT2& from, const A3DPOINT2& to, bool& bReverse)
{
	vector<vector<A3DPOINT2> >::iterator it;
	for (it = paths.begin(); it != paths.end(); ++it)
	{
		if (it->front() == from && it->back() == to)
		{
			bReverse = false;
			return  it;
		}

		if (it->front() == to && it->back() == from)
		{
			bReverse = true;
			return it;
		}
	}

	return NULL;
}

inline void l_AddPathNode(const A3DPOINT2& pt, int layer, vector<PathNode>& path)
{
	PathNode pathNode;
	pathNode.SetI(pt, layer);
	path.push_back(pathNode);
}

void l_CatPath(vector<PathNode>& path1, const vector<A3DPOINT2>& path2,  int layer,  bool bReverse)
{
	if (path2.empty())
	{
		return;
	}

	int i;
	if (bReverse)
	{
		if ((!path1.empty()) && (path1.back().GetPtI() == path2.back()))
		{
			path1.pop_back();
		}
		for (i = (int)path2.size()-1; i >= 0; i--)
		{
			l_AddPathNode(path2[i], layer, path1);
		}
	}
	else
	{
		if ((!path1.empty()) && (path1.back().GetPtI() == path2.front()))
		{
			path1.pop_back();
		}
		for (i = 0; i < (int)path2.size(); i++)
		{
			l_AddPathNode(path2[i], layer, path1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//class CPfClu
CPfClu::CPfClu()
: m_pMoveMap(NULL)
, m_pGraph(NULL)
, m_iPfAlg(PF_ALGORITHM_CLU_MA)
, m_iLayerStart(-1)
, m_pNodeStart(NULL)
, m_bDelStart(false)
, m_iLayerGoal(-1)
, m_pNodeGoal(NULL)
, m_bDelGoal(false)
, m_State(PF_STATE_UNKNOWN)
{
	m_ptStart.x = m_ptStart.y = 0;
	m_ptGoal.x = m_ptGoal.y = 0;
}

CPfClu::~CPfClu()
{
	Reset();
}

void CPfClu::SetupSearch(CMoveMap *pMoveMap, int iPfAlg, int iLayerStart, const A3DPOINT2& ptStart, int iLayerGoal, const A3DPOINT2& ptGoal)
{
	m_iLayerStart = iLayerStart;
	m_ptStart = ptStart;
	m_pNodeStart = NULL;
	m_bDelStart = false;
	
	m_iLayerGoal = iLayerGoal;
	m_ptGoal = ptGoal;
	m_pNodeGoal = NULL;
	m_bDelGoal = false;
	
	m_pMoveMap = pMoveMap;
	m_pGraph = pMoveMap->GetMultiCluGraph();
	m_iPfAlg = iPfAlg;
	
	Cluster * pStartClu = m_pGraph->GetCluster(m_iLayerStart, m_ptStart);
	Cluster * pGoalClu  = m_pGraph->GetCluster(m_iLayerGoal, m_ptGoal);
	assert(pStartClu && pGoalClu);
	
	// Bug investigation for ShengDouShi Project, linzhehui  2011/10/27
	// repeated path will be added in same cluster
	// who disable the following code and for what reason?
	// 明白了，为了避免CLUSETER内部无法搜索到路线，因此所有数据都不先进行CLUSTER内部的直接搜索
	// 

	if (pStartClu == pGoalClu)
	{
		//同一CLUSTER 先内部搜索		
		CBitImage * pRMap = m_pMoveMap->GetLayer(m_iLayerStart)->GetRMap();
		CluExpandLimit  cluExpandLimit;
		cluExpandLimit.xMin = pStartClu->GetXMin();
		cluExpandLimit.zMin = pStartClu->GetZMin();
		cluExpandLimit.xExt = pStartClu->GetWidth();
		cluExpandLimit.zExt = pStartClu->GetHeight();
		
		float cost;
		CMAStar  mastar;
		cost = mastar.Search(m_iPfAlg, pRMap, m_ptStart, m_ptGoal, m_LocalCluPath, &cluExpandLimit);
		if(cost >= 0)
		{
			m_State = PF_STATE_FOUND;
			return;
		}
	}

	_InsertSG(pStartClu, pGoalClu);
	m_State = PF_STATE_SEARCHING;
}

CGNode * CPfClu::_InsertSG(Cluster * pClu, A3DPOINT2& pt, int iLayer, bool& bInsert, vector<vector<A3DPOINT2> >& cachePath)
{
	CGGraph * pGraph = m_pGraph->GetGraph();
	//check already exist
	int id = pClu->NodeExists(pt.x, pt.y, pGraph);
	if (id >=0)
	{
		bInsert = false;
		return m_pGraph->GetGraph()->GetNode(id);
	}

	CGNode * pNewNode = new CGNode;
	id =   pGraph->AddNode(pNewNode);
	pNewNode->SetLabelL(CGNode::kLayer, iLayer);
	pNewNode->SetMapCoord(pt);
	CBitImage * pRMap = m_pMoveMap->GetLayer(iLayer)->GetRMap();
	CluExpandLimit  cluExpandLimit;
	cluExpandLimit.xMin = pClu->GetXMin();
	cluExpandLimit.zMin = pClu->GetZMin();
	cluExpandLimit.xExt = pClu->GetWidth();
	cluExpandLimit.zExt = pClu->GetHeight();

	int i;
	for(i = 0; i < pClu->GetNodeCount(); ++i)
	{
		CGNode * tmpNode = pGraph->GetNode(pClu->GetNodeId(i));
		vector<A3DPOINT2> tmpPath;
		float cost;

		if (m_iPfAlg == PF_ALGORITHM_CLU_G)
		{
			CGAStar gastar;
			cost = gastar.Search(pRMap, pt, tmpNode->GetMapCoord(), tmpPath, &cluExpandLimit);	
		}
		else
		{
			CMAStar  mastar;
			cost = mastar.Search(m_iPfAlg, pRMap, pt, tmpNode->GetMapCoord(), tmpPath, &cluExpandLimit);
		}
		
		if (cost >= 0.0f)
		{
			cachePath.push_back(tmpPath);
			CGEdge * edge = new CGEdge(id, tmpNode->GetNum(), cost);
			pGraph->AddEdge(edge);
		}
	}
	
	bInsert = true;
	return pNewNode;
}
	


void CPfClu::_InsertSG(Cluster * pStartClu, Cluster* pGoalClu)
{
	m_pNodeStart = _InsertSG(pStartClu, m_ptStart,m_iLayerStart, m_bDelStart, m_StartPath);
	m_pNodeGoal  = _InsertSG(pGoalClu, m_ptGoal, m_iLayerGoal, m_bDelGoal, m_GoalPath);

}


bool CPfClu::Search(vector<PathNode>& path, int nMaxExpand)
{
	path.clear();
	if((m_State==PF_STATE_FOUND)&&(m_iLayerStart==m_iLayerGoal))
	{
		for(int i=0;i<m_LocalCluPath.size();i++)
		{
			PathNode node;
			node.layer = m_iLayerStart;
			node.ptMap.x  = (float)m_LocalCluPath[i].x;
			node.ptMap.y  = (float)m_LocalCluPath[i].y;
			path.push_back(node);
		}
		return true;
	}
	CGAStar astar;
	if (nMaxExpand > 0){
		astar.SetMaxExpand(nMaxExpand);
	}
	vector<CGNode*> gnPath;
	float cost = astar.Search(m_pGraph->GetGraph(), m_pNodeStart, m_pNodeGoal, gnPath);
	m_State = astar.GetStat();

	if (m_State != PF_STATE_FOUND)
	{
		_CleanupSearch();
		return false;
	}

	_RetrieveFullPath(gnPath, path);
	_CleanupSearch();
	return true;
}


void CPfClu::_RetrieveFullPath(const vector<CGNode*>& gPath, vector<PathNode>& path)
{

	CGNode * pCurNode = gPath[0];
	A3DPOINT2 ptCur, ptPrv;
	bool bReverse;
	ptCur = pCurNode->GetMapCoord();
	ptPrv = ptCur;
	int i = 1;
	int curLayer, prvLayer;
	if (m_bDelStart)
	{
		pCurNode = gPath[i];
		++i;
		ptCur = pCurNode->GetMapCoord();
		curLayer = pCurNode->GetLabelL(CGNode::kLayer);
		vector<A3DPOINT2> * tmpPath = l_FindPath(m_StartPath, ptPrv, ptCur, bReverse);
		assert(tmpPath);
		l_CatPath(path, *tmpPath, curLayer, bReverse);
		ptPrv = ptCur;
		prvLayer = curLayer;
	}
	
	while (i < (int)gPath.size()-1)
	{
		pCurNode = gPath[i];
		++i;
		ptCur = pCurNode->GetMapCoord();
		curLayer = pCurNode->GetLabelL(CGNode::kLayer);
		
		if (curLayer != prvLayer)
		{//layer change
			//l_AddPathNode(ptCur, curLayer, path);
			ptPrv = ptCur;
			prvLayer = curLayer;
			continue;
		}
		if (abs(ptCur.x - ptPrv.x) <= 1 && abs(ptCur.y - ptPrv.y) <=1)
		{//inter-edge
			l_AddPathNode(ptCur, curLayer, path);
			ptPrv = ptCur;
			prvLayer = curLayer;
			continue;
		}
		//intra-edge
		Cluster * pClu =  m_pGraph->GetCluster(curLayer, ptCur);
		assert(pClu);
		vector<A3DPOINT2> * tmpPath = l_FindPath( *(pClu->GetIntraPaths()), ptPrv, ptCur, bReverse);
		l_CatPath(path, *tmpPath, curLayer, bReverse);
		ptPrv = ptCur;
		prvLayer = curLayer;
	}

	if (i == (int)gPath.size())
	{
		return;
	}

	pCurNode = gPath[i];
	++i;
	ptCur = pCurNode->GetMapCoord();
	curLayer = pCurNode->GetLabelL(CGNode::kLayer);

	if (m_bDelGoal)
	{
		vector<A3DPOINT2> * tmpPath = l_FindPath(m_GoalPath, ptPrv, ptCur, bReverse);
		l_CatPath(path, *tmpPath, curLayer, bReverse);	
	}
	else if (curLayer != prvLayer)
	{
		//l_AddPathNode(ptCur, curLayer, path);
	}
	else if (abs(ptCur.x - ptPrv.x) <= 1 && abs(ptCur.y - ptPrv.y) <=1)
	{//inter-edge
		l_AddPathNode(ptCur, curLayer, path);
	}
	else
	{//intra-edge
		Cluster * pClu =  m_pGraph->GetCluster(curLayer, ptCur);
		assert(pClu);
		vector<A3DPOINT2> * tmpPath = l_FindPath( *(pClu->GetIntraPaths()), ptPrv, ptCur, bReverse);
		assert(tmpPath);
		l_CatPath(path, *tmpPath, curLayer,  bReverse);
	}
	
}


void CPfClu::_CleanupSearch()
{
	//	清除搜索中用到的临时数据，但保留搜索结果等状态
	_RemoveSG();
	
	m_StartPath.clear();
	m_GoalPath.clear();
	m_LocalCluPath.clear();
}

void CPfClu::Reset()
{
	//	清除所有状态
	_CleanupSearch();
	
	m_pMoveMap = NULL;
	m_pGraph = NULL;
	m_iPfAlg = PF_ALGORITHM_CLU_MA;
	
	m_iLayerStart = -1;
	m_pNodeStart = NULL;
	m_ptStart.x = m_ptStart.y = 0;

	m_iLayerGoal = -1;
	m_pNodeGoal = NULL;
	m_ptGoal.x = m_ptGoal.y = 0;

	m_State = PF_STATE_UNKNOWN;
}

void CPfClu::_RemoveSG()
{
	if (m_bDelStart)
	{
		assert(m_pNodeStart);
		m_StartPath.clear();
		int oldid;
		m_pGraph->GetGraph()->RemoveNode(m_pNodeStart, oldid);
		m_pNodeStart = NULL;
		m_bDelStart = false;
	}


	if (m_bDelGoal)
	{
		assert(m_pNodeGoal);
		m_GoalPath.clear();
		int oldid;
		m_pGraph->GetGraph()->RemoveNode(m_pNodeGoal, oldid);
		m_pNodeGoal = NULL;
		m_bDelGoal = false;
	}

}

}//end of namesapce