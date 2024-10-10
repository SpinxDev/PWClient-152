/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	common files for cluster
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/
#include <AFile.h>
#include <assert.h>

#include "CluCommon.h"
#include "MapGraph.h"
#include "PfConstant.h"
#include "Graph.h"
#include "MoveAgent.h"

namespace AutoMove
{

//////////////////////////////////////////////////////////////////////////
//local var
CluExpandLimit   l_clu_limit;

//////////////////////////////////////////////////////////////////////////
//local&global func
bool LoadCluGraph(CGGraph * g, AFile& fileimg)
{
	assert(g);
	unsigned int count;
	DWORD readlen;
	unsigned int i;

	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	g->GetAllNodes()->reserve(count);
	for (i = 0; i < count; i++)
	{
		CGNode * n = new CGNode;
		g->AddNode(n);

		int layer;
		fileimg.Read(&layer, sizeof(int), &readlen);
		n->SetLabelL(CGNode::kLayer, layer);

		DWORD dw;
		fileimg.Read(&dw, sizeof(DWORD), &readlen);
		n->SetMapCoord(GET_LOWORD(dw), GET_HIWORD(dw));
	}

	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	g->GetAllEdges()->reserve(count);

	vector<CGEdge*> tempEdges;
	tempEdges.reserve(count);
	hash_map<unsigned int, unsigned int> nodeEdgeCount;
	for (i = 0; i < count; i++)
	{
		unsigned int from, to;
		float weight;
		fileimg.Read(&from, sizeof(unsigned int), &readlen);
		fileimg.Read(&to, sizeof(unsigned int), &readlen);
		fileimg.Read(&weight, sizeof(float), &readlen);

		CGEdge * e = new CGEdge(from, to, weight);
		tempEdges.push_back(e);
		++ nodeEdgeCount[from];
		++ nodeEdgeCount[to];
	}
	for (i = 0; i < g->GetAllNodes()->size(); ++ i)
	{
		CGNode *pNode = (*g->GetAllNodes())[i];
		pNode->GetAllEdges()->reserve(nodeEdgeCount[i]);
	}
	for (i = 0; i < count; ++i)
	{
		g->AddEdge(tempEdges[i]);
	}
	return true;
}

void SaveCluGraph(CGGraph * g, AFile& fileimg)
{
	unsigned int count;
	DWORD writelen;
	unsigned int i;

	vector<CGNode *> * nodes = g->GetAllNodes();

	count  = nodes->size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; i++)
	{
		CGNode * n = (*nodes)[i];
		
		int layer = (int)n->GetLabelL(CGNode::kLayer);
		fileimg.Write(&layer, sizeof(int), &writelen);
		A3DPOINT2 pt(n->GetMapCoord());
		DWORD    dw = MAKE_DWORD(pt.x, pt.y);
		fileimg.Write(&dw, sizeof(DWORD), &writelen);
		//skip others
	}


	//edges
	vector<CGEdge*> * edges = g->GetAllEdges();
	count = edges->size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; i++)
	{
		CGEdge *e = (*edges)[i];
		unsigned int which;
		which = e->GetFrom();
		fileimg.Write(&which, sizeof(unsigned int), &writelen);
		which = e->GetTo();
		fileimg.Write(&which, sizeof(unsigned int), &writelen);

		float weight;
		weight = e->GetWeight();
		fileimg.Write(&weight, sizeof(float), &writelen);
	}
}

//////////////////////////////////////////////////////////////////////////
//class Cluster
Cluster::Cluster(const Cluster& rhs)
{
	m_XMin = rhs.m_XMin;
	m_ZMin = rhs.m_ZMin;
	m_Width = rhs.m_Width;
	m_Height = rhs.m_Height;
	m_Nodes = rhs.m_Nodes;
	m_IntraPahs = rhs.m_IntraPahs;

}

Cluster::~Cluster()
{

}

Cluster& Cluster::operator=(const Cluster& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	m_XMin = rhs.m_XMin;
	m_ZMin = rhs.m_ZMin;
	m_Width = rhs.m_Width;
	m_Height = rhs.m_Height;
	m_Nodes = rhs.m_Nodes;
	m_IntraPahs = rhs.m_IntraPahs;

	return *this;
}

void Cluster::Save(AFile& fileimg)
{
	DWORD writelen;
//	fileimg.Write(&m_Row, sizeof(int), &writelen);
//	fileimg.Write(&m_Column, sizeof(int), &writelen);
	fileimg.Write(&m_XMin, sizeof(int), &writelen);
	fileimg.Write(&m_ZMin, sizeof(int), &writelen);
	fileimg.Write(&m_Width, sizeof(int), &writelen);
	fileimg.Write(&m_Height, sizeof(int), &writelen);
	
	unsigned int count;
	unsigned int i;

	count = m_Nodes.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i< count; i++)
	{
		int num = m_Nodes[i];
		fileimg.Write(&num, sizeof(int), &writelen);
	}
	
	count = m_IntraPahs.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i =0; i < count; i++)
	{
		unsigned int pcount = m_IntraPahs[i].size();
		fileimg.Write(&pcount, sizeof(unsigned int), &writelen);
		for (unsigned int j = 0; j < pcount; j++)
		{
			A3DPOINT2& pt = m_IntraPahs[i][j];
			//fileimg.Write(&pt, sizeof(A3DPOINT2), &writelen);
			DWORD dw = MAKE_DWORD(pt.x, pt.y);
			fileimg.Write(&dw, sizeof(DWORD), &writelen);
		}
	}

}

void Cluster::Load(AFile& fileimg)
{

	DWORD readlen;
//	fileimg.Read(&m_Row, sizeof(int), &readlen);
//	fileimg.Read(&m_Column, sizeof(int), &readlen);
	fileimg.Read(&m_XMin, sizeof(int), &readlen);
	fileimg.Read(&m_ZMin, sizeof(int), &readlen);
	fileimg.Read(&m_Width, sizeof(int), &readlen);
	fileimg.Read(&m_Height, sizeof(int), &readlen);

	unsigned int count;
	unsigned int i;
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	m_Nodes.clear();
	m_Nodes.reserve(count);
	for (i = 0; i < count; i++)
	{
		int num;
		fileimg.Read(&num, sizeof(int), &readlen);
		m_Nodes.push_back(num);
	}
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	m_IntraPahs.clear();
	m_IntraPahs.reserve(count);
	for (i = 0; i < count; i++)
	{
		vector<A3DPOINT2> tmp;
		m_IntraPahs.push_back(tmp);
		vector<A3DPOINT2>& path = m_IntraPahs[m_IntraPahs.size()-1];
		unsigned int pcount;
		fileimg.Read(&pcount, sizeof(unsigned int), &readlen);
		path.reserve(pcount);
		for (unsigned int j = 0; j < pcount; j++)
		{
			A3DPOINT2 pt;
			DWORD dw;
			fileimg.Read(&dw, sizeof(DWORD), &readlen);
			pt.x = GET_LOWORD(dw);
			pt.y = GET_HIWORD(dw);
			path.push_back(pt);
		}
	}


}

float  Cluster::Connect(int iPfAlg, CGGraph * pAbsGraph, int i, int j, CBitImage * pRMap)
{
	assert(pAbsGraph && i >=0 && i < m_Nodes.size() && j >=0 && j < m_Nodes.size());
	CGNode * ni_abs = pAbsGraph->GetNode(m_Nodes[i]);
	CGNode * nj_abs = pAbsGraph->GetNode(m_Nodes[j]);
	assert(ni_abs&& nj_abs);
	A3DPOINT2 ni_map = ni_abs->GetMapCoord();
	A3DPOINT2 nj_map = nj_abs->GetMapCoord();

	l_clu_limit.xMin = m_XMin;
	l_clu_limit.zMin = m_ZMin;
	l_clu_limit.xExt = m_Width;
	l_clu_limit.zExt = m_Height;

	vector<A3DPOINT2> path;
	float cost;
	if (iPfAlg == PF_ALGORITHM_CLU_G)
	{
		CGAStar  gastar;
		cost =	gastar.Search(pRMap, ni_map, nj_map, path, &l_clu_limit);
	}
	else 
	{
		CMAStar  mastar;
		cost =	mastar.Search(iPfAlg, pRMap, ni_map, nj_map, path, &l_clu_limit);
	}

	if (cost > 0.0f)
	{
		m_IntraPahs.push_back(path);
	}

	return cost;
}

int Cluster::NodeExists(long x, long z, CGGraph* g, int * index) const
{
	int id;
	int i;
	for (i=0; i< (int)m_Nodes.size(); i++)
	{
		id = m_Nodes[i];
		CGNode* node = g->GetNode(id);
		if ((node->GetLabelL(CGNode::kXCoordinate)==x) && (node->GetLabelL(CGNode::kZCoordinate)==z))
		{
			if (index)
			{
				*index = i;
			}

			return id;
		}
	}
	return -1;
}

void Cluster::SetNodeId(int id, int index)
{
	assert(index >= 0 && index < (int)m_Nodes.size());
	m_Nodes[index] = id;
}


}