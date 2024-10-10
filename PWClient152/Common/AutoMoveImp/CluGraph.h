/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	clu graph
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _CLU_GRAPH_H_
#define  _CLU_GRAPH_H_

#include "CluCommon.h"

namespace AutoMove
{
class CGGraph;

class CCluGraph
{
public:
	CCluGraph();
	~CCluGraph();
	void Release();
	vector<Cluster>* GetAllClusters() 
	{
		return &m_aCluster;
	}
	CGGraph * GetGraph()
	{
		return m_pGraph;
	}
	void SetGraph(const CGGraph * pGraph);
	int  GetCluSize() const
	{
		return m_iCluSize;
	}
	void SetCluSize(int iCluSize)
	{
		m_iCluSize = iCluSize;
	}
	int  GetRowCount() const
	{
		return m_iRowCount;
	}
	void SetRowCount(int nRows) 
	{
		m_iRowCount = nRows;
	}
	int GetColCount() const
	{
		return m_iColCount;
	}
	void SetColCount(int nCols)
	{
		m_iColCount = nCols;
	}
	bool Load(const char * szPath);
	void Save(const char * szPath);
	
private:
	vector<Cluster>  m_aCluster;
	CGGraph *		 m_pGraph;
	int				 m_iCluSize;
	int              m_iRowCount;
	int              m_iColCount;
};




}
#endif