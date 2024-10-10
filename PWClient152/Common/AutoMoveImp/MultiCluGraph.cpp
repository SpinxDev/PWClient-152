/********************************************************************
	created:	2008/05/28
	author:		kuiwu
	
	purpose:	multi clu graph
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/
#include <A3DMacros.h>
#include <assert.h>
#include <AFileImage.h>


#include "MultiCluGraph.h"
#include "Graph.h"

namespace AutoMove
{

//////////////////////////////////////////////////////////////////////////
//define&const
#define	   MLU_FILE_MAGIC      (DWORD)(('m'<<24)| ('l'<<16)|('u'<<8)|('f'))
#define    MLU_FILE_VERSION     2


//////////////////////////////////////////////////////////////////////////
//class CMultiCluGraph
CMultiCluGraph::CMultiCluGraph()
:m_pGraph(NULL)
{

}

CMultiCluGraph::~CMultiCluGraph()
{

}

void CMultiCluGraph::Release()
{
	A3DRELEASE(m_pGraph);
	m_aaClusters.swap(vector<vector<Cluster> >());
}

void CMultiCluGraph::SetCluParam(int cluSize, int rowCount, int colCount)
{
	m_iCluSize = cluSize;
	m_iRowCount = rowCount;
	m_iColCount = colCount;
}

int CMultiCluGraph::_GetCluId(const A3DPOINT2& ptMap) const
{
	int row = ptMap.y / m_iCluSize;
	int col = ptMap.x / m_iCluSize;

	if (row >= 0 && row <m_iRowCount && col >= 0 && col < m_iColCount)
	{
		return row * m_iColCount + col;
	}
	return -1;
}

Cluster * CMultiCluGraph::GetCluster(int iLayer, const A3DPOINT2& ptMap)
{
	int cluId = _GetCluId(ptMap);
	if (cluId < 0)
	{
		return NULL;
	}

	return	&m_aaClusters[iLayer][cluId];
}

void CMultiCluGraph::SetGraph(const CGGraph * pGraph)
{
	m_pGraph = pGraph->Clone();
}

void CMultiCluGraph::SetClusters(const vector<vector<Cluster> >& aaClusters)
{
	m_aaClusters = aaClusters;
}

bool CMultiCluGraph::Save(const char * szPath)
{
	if (!m_pGraph || m_aaClusters.empty())
	{
		return false;
	}
	AFile   fileimg;
	if (!fileimg.Open(szPath, AFILE_BINARY|AFILE_CREATENEW))
	{
		return false;
	}
	DWORD  writelen;

	DWORD  flag = MLU_FILE_MAGIC;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);
	flag = MLU_FILE_VERSION;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);

	fileimg.Write(&m_iCluSize, sizeof(int), &writelen);
	fileimg.Write(&m_iRowCount, sizeof(int), &writelen);
	fileimg.Write(&m_iColCount, sizeof(int), &writelen);
	//cluster
	unsigned int count1, count2;
	count1 = m_aaClusters.size();
	fileimg.Write(&count1, sizeof(unsigned int), &writelen);
	unsigned int i, j;
	for (i = 0; i < count1; ++i)
	{
		count2 = m_aaClusters[i].size();
		fileimg.Write(&count2, sizeof(unsigned int), &writelen);
		for (j = 0; j < count2; ++j)
		{
			Cluster& c = m_aaClusters[i][j];
			c.Save(fileimg);
		}
	}

	//graph
	SaveCluGraph(m_pGraph, fileimg);

	return true;
}

bool CMultiCluGraph::Load(const char * szPath)
{
	Release();
	
	AFileImage   fileimg;
	if (!fileimg.Open(szPath, AFILE_BINARY|AFILE_OPENEXIST))
	{
		return false;
	}
	unsigned long readlen;
	DWORD flag;
	fileimg.Read(&flag, sizeof(DWORD), &readlen);
	if (flag != MLU_FILE_MAGIC)
	{
		return false;
	}
	fileimg.Read(&flag, sizeof(DWORD), &readlen);  //version
	if (flag != MLU_FILE_VERSION)
	{
		return false;
	}
	
	fileimg.Read(&m_iCluSize, sizeof(int), &readlen);
	fileimg.Read(&m_iRowCount, sizeof(int), &readlen);
	fileimg.Read(&m_iColCount, sizeof(int), &readlen);


	//cluster
	unsigned int count1, count2;
	fileimg.Read(&count1, sizeof(unsigned int), &readlen);
	m_aaClusters.reserve(count1);
	unsigned int i, j;
	for (i = 0; i < count1; ++i)
	{
		m_aaClusters.push_back(vector<Cluster>());
		fileimg.Read(&count2, sizeof(unsigned int), &readlen);
		m_aaClusters[i].reserve(count2);
		for (j = 0; j < count2; ++j)
		{
			Cluster c;
			c.Load(fileimg);
			m_aaClusters[i].push_back(c);
		}
	}

	//graph
	m_pGraph = new CGGraph;
	if (!LoadCluGraph(m_pGraph, fileimg))
	{
		A3DRELEASE(m_pGraph);
		return false;
	}
	return true;
}

}
