/********************************************************************
	created:	2008/05/23
	author:		kuiwu
	
	purpose:	layer move map
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef  _LAYER_MAP_H_
#define _LAYER_MAP_H_

#include "blockimage.h"

namespace AutoMove
{

class CCluGraph;
class CIslandList;
class CBitImage;
class CIslandList;

class CLayerMap
{
public:
	CLayerMap();
	~CLayerMap();
	
	bool LoadRMap(const char * szPath);
	bool LoadDHMap(const char * szPath);
	bool LoadClu(const char * szPath);
	bool LoadIsl(const char * szPath);
	
	void CreateClu(int iPfAlg, int cluSize, int entWid);
	void CreateIslandList();
	
	CBitImage * GetRMap() const
	{
		return m_pRMap;
	}
	CBlockImage<FIX16> * GetDHMap() const
	{
		return m_pDHMap;
	}
	CCluGraph * GetCluGraph() const
	{
		return m_pCluGraph;
	}
	CIslandList * GetIslandList() const
	{
		return m_pIslandList;
	}

	void Release();
	void ReleaseCluGraph();
private:

private:
	CBitImage			* m_pRMap;
	CBlockImage<FIX16>  * m_pDHMap;
	CCluGraph			* m_pCluGraph;	
	CIslandList         * m_pIslandList;
};

}

 
#endif