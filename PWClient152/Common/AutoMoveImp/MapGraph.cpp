/********************************************************************
	created:	2008/05/27
	author:		kuiwu
	
	purpose:	graph for map to accelerate searching
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#include <A3DMacros.h>

#include "MapGraph.h"
#include "Graph.h"
#include "bitimage.h"

namespace AutoMove
{
CMapGraph::CMapGraph()
:m_pGraph(NULL), m_pCoord2Node(NULL),
m_pRMap(NULL), m_iWidth(0), m_iLength(0)
{

}

CMapGraph::~CMapGraph()
{
	Release();
}

void CMapGraph::SetRMap(CBitImage * pRMap)
{
	if (m_pRMap == pRMap)
	{
		return;
	}

	m_pRMap = pRMap;
	int width, length;
	pRMap->GetImageSize(width, length);
	if (width != m_iWidth || length != m_iLength)
	{
		m_iWidth = width;
		m_iLength = length;
		if (m_pCoord2Node)
		{
			delete[] m_pCoord2Node;
		}
		m_pCoord2Node = new int[m_iWidth *m_iLength];
	}
	Reset();
}

void CMapGraph::Release()
{
	if (m_pCoord2Node)
	{
		delete[] m_pCoord2Node;
		m_pCoord2Node = NULL;
	}
	A3DRELEASE(m_pGraph);
}

void CMapGraph::Reset()
{
	memset(m_pCoord2Node, -1, sizeof(int)*m_iWidth*m_iLength);
	A3DRELEASE(m_pGraph);
}

CGNode * CMapGraph::GetNode(const A3DPOINT2& ptMap)
{
	int node_id = m_pCoord2Node[ptMap.y * m_iWidth + ptMap.x];
	if (node_id >= 0)
	{
		return m_pGraph->GetNode(node_id);
	}

	if (!m_pGraph)
	{
		m_pGraph = new CGGraph;
	}

	CGNode * node = new CGNode;
	node->SetMapCoord(ptMap);
	node_id = m_pGraph->AddNode(node);
	m_pCoord2Node[ptMap.y * m_iWidth + ptMap.x] = node_id;
	
	return node;
	
}

CGEdge * CMapGraph::GetEdge(const A3DPOINT2& ptMap1, const A3DPOINT2& ptMap2, float cost)
{
	int width, length;
	m_pRMap->GetImageSize(width, length);
	int node_id1, node_id2;
	CGEdge *edge;
	CGNode *node1, *node2;

	node_id1 = m_pCoord2Node[ptMap1.y * width + ptMap1.x];
	assert(node_id1 >= 0);
	node1 = m_pGraph->GetNode(node_id1);
	vector<CGEdge *> * all_edges = node1->GetAllEdges();
	int i;
	for (i = 0; i < (int)all_edges->size(); ++i)
	{
		edge = all_edges->at(i);
		node_id2 = (edge->GetTo() == node_id1)?(edge->GetFrom()):(edge->GetTo());
		node2  = m_pGraph->GetNode(node_id2);
		assert(node2);
		if (node2->GetMapCoord() == ptMap2)
		{
			return edge;
		}
	}


	node_id2 = m_pCoord2Node[ptMap2.y * width + ptMap2.x];
	assert(node_id2 >= 0);
	
	edge = new CGEdge(node_id1, node_id2, cost);
	m_pGraph->AddEdge(edge);
	return edge;
}

}