/********************************************************************
	created:	2008/05/23
	author:		kuiwu
	
	purpose:	layer move map
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#include <A3DMacros.h>

#include "LayerMap.h"
#include "ClusterAbstraction.h"
#include "bitimage.h"
#include "CluGraph.h"
#include "Island.h"

namespace AutoMove
{

//////////////////////////////////////////////////////////////////////////
//local funcs



//////////////////////////////////////////////////////////////////////////
//class CLayerMap
CLayerMap::CLayerMap()
:m_pRMap(NULL), m_pDHMap(NULL),
 m_pCluGraph(NULL), m_pIslandList(NULL)
{

}

CLayerMap::~CLayerMap()
{
	
}

void CLayerMap::Release()
{
	ReleaseCluGraph();
	A3DRELEASE(m_pDHMap);
	A3DRELEASE(m_pRMap);
	A3DRELEASE(m_pIslandList);
}

void CLayerMap::ReleaseCluGraph()
{
	A3DRELEASE(m_pCluGraph);
}

bool CLayerMap::LoadRMap(const char * szPath)
{
	Release();

	m_pRMap = new CBitImage;
	if (!m_pRMap->Load(szPath))
	{
		A3DRELEASE(m_pRMap);
		return false;
	}
		
	return true;
}

bool CLayerMap::LoadDHMap(const char * szPath)
{
	A3DRELEASE(m_pDHMap);
	m_pDHMap = new CBlockImage<FIX16>;
	if (!m_pDHMap->Load(szPath))
	{
		A3DRELEASE(m_pDHMap);
		return false;
	}

	return true;
}

bool CLayerMap::LoadClu(const char * szPath)
{
	assert(m_pRMap);
	A3DRELEASE(m_pCluGraph);
	m_pCluGraph = new CCluGraph;
	if (!m_pCluGraph->Load(szPath))
	{
		A3DRELEASE(m_pCluGraph);
		return false;
	}
	return true;
}

bool CLayerMap::LoadIsl(const char * szPath)
{
	assert(m_pRMap);
	A3DRELEASE(m_pIslandList);
	m_pIslandList = new CIslandList(m_pRMap);
	if (!m_pIslandList->Load(szPath))
	{
		A3DRELEASE(m_pIslandList);
		return false;
	}

	return true;
}

void CLayerMap::CreateClu(int iPfAlg, int cluSize, int entWid)
{
	A3DRELEASE(m_pCluGraph);
	CClusterAbstraction clu_abs(iPfAlg);
	clu_abs.Init(m_pRMap);
	clu_abs.Create(cluSize, entWid);
	
	m_pCluGraph = new CCluGraph;
	clu_abs.Export(m_pCluGraph);
}

void CLayerMap::CreateIslandList()
{
	assert(m_pRMap);
	A3DRELEASE(m_pIslandList);
	m_pIslandList = new CIslandList(m_pRMap);
}

}