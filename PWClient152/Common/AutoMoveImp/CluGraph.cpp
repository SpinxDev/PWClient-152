/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	clu graph
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/
#include <A3DMacros.h>
#include <AFile.h>
#include <AFileImage.h>
#include "CluGraph.h"
#include "Graph.h"
#include "PfConstant.h"

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//define
#define	   CLU_FILE_MAGIC      (DWORD)(('c'<<24)| ('l'<<16)|('u'<<8)|('f'))
#define    CLU_FILE_VERSION     3

//////////////////////////////////////////////////////////////////////////
//local func




//////////////////////////////////////////////////////////////////////////
//class CCluGraph
CCluGraph::CCluGraph()
:m_pGraph(NULL)
{

}

CCluGraph::~CCluGraph()
{

}

void CCluGraph::Release()
{
	m_aCluster.clear();
	A3DRELEASE(m_pGraph);
}

void CCluGraph::SetGraph(const CGGraph * pGraph)
{
	m_pGraph = pGraph->Clone();
}

bool CCluGraph::Load(const char * szPath)
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
	if (flag != CLU_FILE_MAGIC)
	{
		return false;
	}
	fileimg.Read(&flag, sizeof(DWORD), &readlen);  //version
	if (flag != CLU_FILE_VERSION)
	{
		return false;
	}
	
	fileimg.Read(&m_iCluSize, sizeof(int), &readlen);
	fileimg.Read(&m_iRowCount, sizeof(int), &readlen);
	fileimg.Read(&m_iColCount, sizeof(int), &readlen);
	//cluster
	unsigned int count;
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	m_aCluster.reserve(count);
	unsigned int i;
	for (i = 0; i < count; i++)
	{
		Cluster c;
		m_aCluster.push_back(c);
		m_aCluster.back().Load(fileimg);
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

void CCluGraph::Save(const char * szPath)
{
	if (!m_pGraph || m_aCluster.empty())
	{
		return;
	}
	AFile   fileimg;
	if (!fileimg.Open(szPath, AFILE_BINARY|AFILE_CREATENEW))
	{
		return;
	}
	unsigned long  writelen;
	
	DWORD  flag = CLU_FILE_MAGIC;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);
	flag = CLU_FILE_VERSION;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);

	fileimg.Write(&m_iCluSize, sizeof(int), &writelen);
	fileimg.Write(&m_iRowCount, sizeof(int), &writelen);
	fileimg.Write(&m_iColCount, sizeof(int), &writelen);
	//cluster
	unsigned int count;
	count = m_aCluster.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	unsigned int i;
	for (i = 0; i < count; i++)
	{
		Cluster& c = m_aCluster[i];
		c.Save(fileimg);
	}

	//graph
	SaveCluGraph(m_pGraph, fileimg);

}

}