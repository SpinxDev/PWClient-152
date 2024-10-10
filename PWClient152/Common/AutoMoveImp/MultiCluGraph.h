/********************************************************************
	created:	2008/05/28
	author:		kuiwu
	
	purpose:	multi clu graph
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _MULTI_CLU_GRAPH_H_
#define _MULTI_CLU_GRAPH_H_

#include "CluCommon.h"

namespace AutoMove
{

class CGGraph;

class CMultiCluGraph
{
public:
	CMultiCluGraph();
	~CMultiCluGraph();
	void Release();
	Cluster * GetCluster(int iLayer, const A3DPOINT2& ptMap);
	void SetCluParam(int cluSize, int rowCount, int colCount);
	void SetGraph(const CGGraph * pGraph);
	void SetClusters(const vector<vector<Cluster> >& aaClusters);
	CGGraph * GetGraph()
	{
		return m_pGraph;
	}
	bool Load(const char * szPath);
	bool Save(const char * szPath);
private:
	int  _GetCluId(const A3DPOINT2& ptMap) const;
private:
	int	  m_iCluSize;
	int   m_iRowCount;  //row count in single layer 
	int   m_iColCount;
	vector<vector<Cluster> >   m_aaClusters;
	CGGraph					*  m_pGraph;
};

}

#endif