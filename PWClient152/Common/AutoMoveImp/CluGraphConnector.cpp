/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	create multi clu graph
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#include <A3DMacros.h>
#include <assert.h>

#include "CluGraphConnector.h"
#include "MultiCluGraph.h"
#include "Graph.h"
#include "CluGraph.h"
#include "JointList.h"
#include "LayerMap.h"
#include "MapGraph.h"
#include "MoveMap.h"

namespace AutoMove
{

CCluGraphConnector::CCluGraphConnector(int iPfAlg)
:m_pGraph(NULL)
,m_iPfAlg(iPfAlg)
{

}

CCluGraphConnector::~CCluGraphConnector()
{

}

void CCluGraphConnector::Release()
{
	A3DRELEASE(m_pGraph);
	m_aaClusters.clear();
}

void CCluGraphConnector::_SetCluParam(CCluGraph * pCluGraph)
{
	m_iCluSize = pCluGraph->GetCluSize();
	m_iRowCount = pCluGraph->GetRowCount();
	m_iColCount = pCluGraph->GetColCount();
}

int CCluGraphConnector::_GetCluId(const A3DPOINT2& ptMap) const
{
	int row = ptMap.y / m_iCluSize;
	int col = ptMap.x / m_iCluSize;

	if (row >= 0 && row <m_iRowCount && col >= 0 && col < m_iColCount)
	{
		return row * m_iColCount + col;
	}
	return -1;
}

void CCluGraphConnector::_AddLayer(CCluGraph * pCluGraph)
{
	int iLayer = m_aaClusters.size();
	m_aaClusters.push_back(vector<Cluster>());
	vector<Cluster> * pAOldClus = pCluGraph->GetAllClusters();
	vector<Cluster> * pANewClus = m_aaClusters.end()-1;
	*pANewClus = *pAOldClus;
	CGGraph * pOldGraph = pCluGraph->GetGraph();

	int i;
	A3DPOINT2 ptMap;
	vector<CGNode *> * pAOldNodes = pOldGraph->GetAllNodes();
	for (i = 0; i < (int)pAOldNodes->size(); ++i)
	{
		CGNode * pOldNode = pAOldNodes->at(i);
		ptMap = pOldNode->GetMapCoord();
		int cluId = _GetCluId(ptMap);
		assert(cluId >= 0);
		Cluster * pOldClu = pAOldClus->begin() + cluId;
		Cluster * pNewClu = pANewClus->begin() + cluId;
		int index;
		int iOldNodeId = pOldClu->NodeExists(ptMap.x, ptMap.y, pOldGraph, &index);
		assert(iOldNodeId >= 0);
		CGNode * pNewNode = pOldNode->ModerateClone();
		pNewNode->SetLabelL(CGNode::kLayer, iLayer);
		int iNewNodeId = m_pGraph->AddNode(pNewNode);
		pNewClu->SetNodeId(iNewNodeId, index);
	}
	vector<CGEdge*> * pAOldEdges = pOldGraph->GetAllEdges();
	for (i = 0; i < (int)pAOldEdges->size(); ++i)
	{
		CGEdge * pOldEdge = pAOldEdges->at(i);
		CGNode * pOldNode;
		int cluId;
		Cluster * pNewClu;

		pOldNode = pOldGraph->GetNode(pOldEdge->GetFrom());
		ptMap = pOldNode->GetMapCoord();
		cluId = _GetCluId(ptMap);
		assert(cluId >= 0);
		pNewClu = pANewClus->begin() + cluId;
		int iNewFromId = pNewClu->NodeExists(ptMap.x, ptMap.y, m_pGraph);
		assert(iNewFromId >= 0);

		pOldNode = pOldGraph->GetNode(pOldEdge->GetTo());
		ptMap = pOldNode->GetMapCoord();
		cluId = _GetCluId(ptMap);
		assert(cluId >= 0);
		pNewClu = pANewClus->begin() + cluId;
		int iNewToId = pNewClu->NodeExists(ptMap.x, ptMap.y, m_pGraph);
		assert(iNewToId >= 0);

		CGEdge * pNewEdge = new CGEdge(iNewFromId, iNewToId, pOldEdge->GetWeight());
		m_pGraph->AddEdge(pNewEdge);
	}

	
	


}

bool CCluGraphConnector::Connect(vector<CLayerMap*>& aLayers, CJointList * pJointList)
{
	Release();

	if (!pJointList || pJointList->GetCount() == 0)
	{//single
		if (aLayers.empty() || aLayers.size() > 1)
		{
			return false;
		}
		
		CLayerMap * pLayerMap = aLayers.back();
		CCluGraph * pCluGraph = pLayerMap->GetCluGraph();
		if (!pCluGraph)
		{
			assert(0);
			return false;
		}
		
		_SetCluParam(pCluGraph);
		
		m_pGraph = new CGGraph;
		_AddLayer(pCluGraph);
		return true;
	}
	
	//multi
	CLayerMap * pLayerMap = aLayers.back();
	CCluGraph * pCluGraph = pLayerMap->GetCluGraph();
	if (!pCluGraph)
	{
		assert(0);
		return false;
	}
	_SetCluParam(pCluGraph);
	m_pGraph = new CGGraph;

	int i;
	for (i = 0; i < (int)aLayers.size(); ++i)
	{
		_AddLayer(aLayers[i]->GetCluGraph());
	}

	//joint
	for (i = 0; i < pJointList->GetCount(); ++i)
	{
		_AddJoint(pJointList->GetJoint(i), aLayers);	
	}

	return true;



}

Cluster * CCluGraphConnector::GetCluster(int iLayer, const A3DPOINT2& ptMap)
{
	int cluId = _GetCluId(ptMap);
	if (cluId < 0)
	{
		return NULL;
	}

	return	&m_aaClusters[iLayer][cluId];
}


void CCluGraphConnector::_AddJoint(const Joint * pJoint, vector<CLayerMap*>& aLayers)
{
	CGNode * pNode1 = new CGNode;
	pNode1->SetLabelL(CGNode::kLayer, pJoint->layer1);
	pNode1->SetMapCoord(pJoint->ptMap);
	int iNodeId1 = m_pGraph->AddNode(pNode1);

	CGNode * pNode2 = new CGNode;
	pNode2->SetLabelL(CGNode::kLayer, pJoint->layer2);
	pNode2->SetMapCoord(pJoint->ptMap);
	int iNodeId2 = m_pGraph->AddNode(pNode2);

	CGEdge * pEdge = new CGEdge(iNodeId1, iNodeId2, 0.0);
	m_pGraph->AddEdge(pEdge);
	
	//connect
	Cluster * pClu;
	pClu  = GetCluster(pJoint->layer1, pJoint->ptMap);
	assert(pClu);
	_AddNode2Clu(iNodeId1, pClu, aLayers[pJoint->layer1]->GetRMap());

	pClu  = GetCluster(pJoint->layer2, pJoint->ptMap);
	assert(pClu);
	_AddNode2Clu(iNodeId2, pClu, aLayers[pJoint->layer2]->GetRMap());
}

void CCluGraphConnector::_AddNode2Clu(int id, Cluster * pClu, CBitImage * pRMap)
{
	int i;
	pClu->AddNode(id);
	int index = pClu->GetNodeCount()-1;
	for (i = 0; i < pClu->GetNodeCount() -1; ++i)
	{
		float cost = pClu->Connect(m_iPfAlg, m_pGraph, i, index, pRMap);
		if (cost >= 0.0f)
		{
			CGEdge * pEdge = new CGEdge(id, pClu->GetNodeId(i), cost);
			m_pGraph->AddEdge(pEdge);
		}
	}
}


void CCluGraphConnector::Export(CMultiCluGraph * pMultiCluGraph)
{
	pMultiCluGraph->SetCluParam(m_iCluSize, m_iRowCount, m_iColCount);
	pMultiCluGraph->SetClusters(m_aaClusters);
	pMultiCluGraph->SetGraph(m_pGraph);
}


}