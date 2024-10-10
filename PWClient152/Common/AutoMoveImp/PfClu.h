/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	clu path finding algorithm
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/
#ifndef  _PF_CLU_H_
#define  _PF_CLU_H_
#include <A3DTypes.h>

#include <vector.h>
using namespace abase;

//#include "MoveMap.h"
#include "PfConstant.h"

namespace AutoMove
{

class CMultiCluGraph;
class CGNode;
class Cluster;
class CMoveMap;

class CPfClu
{
public:
	CPfClu();
	virtual ~CPfClu();

	void Reset();
	void SetupSearch(CMoveMap *pMoveMap, int iPfAlg, int iLayerStart, const A3DPOINT2& ptStart, int iLayerGoal, const A3DPOINT2& ptGoal);
	bool Search(vector<PathNode>& path, int nMaxExpand=-1);
	int  GetState() const
	{
		return m_State;
	}
protected:
private:
   /**
	* insert a start and goal node in to the abstract graph (for searching purposes). 
	* The node is inserted in the same location as it is in on the map. Edges are
	* added between this node and all entrance nodes in its cluster when there is a path 
    * between the nodes. The weight of the edge is set to the distance of the map path.
    */
	void    _InsertSG(Cluster * pStartClu, Cluster* pGoalClu);
	CGNode * _InsertSG(Cluster * pClu, A3DPOINT2& pt, int iLayer, bool& bInsert, vector<vector<A3DPOINT2> >& cachePath);
	void     _RetrieveFullPath(const vector<CGNode*>& gPath, vector<PathNode>& path);
	void     _RemoveSG();
	void	_CleanupSearch();
private:
	CMoveMap *	m_pMoveMap;
	CMultiCluGraph * m_pGraph;
	int			m_iPfAlg;
	int         m_iLayerStart;
	A3DPOINT2   m_ptStart;
	CGNode	*   m_pNodeStart;
	bool		m_bDelStart;  // whether delete start node from abs graph after searching
	int         m_iLayerGoal;
	A3DPOINT2   m_ptGoal;
	CGNode  *   m_pNodeGoal;
	bool		m_bDelGoal;
	int         m_State;

	vector<vector<A3DPOINT2> > m_StartPath;  //cache the path for start node 
	vector<vector<A3DPOINT2> > m_GoalPath;
	vector<A3DPOINT2>          m_LocalCluPath; //cache the path for points which are in the same cluster.

};

}

#endif