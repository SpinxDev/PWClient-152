/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	create multi clu graph
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef  _CLU_GRAPH_CONNECTOR_H_
#define _CLU_GRAPH_CONNECTOR_H_

#include "CluCommon.h"

namespace AutoMove
{
class CLayerMap;
class CGGraph;
class CJointList;
class CCluGraph;
struct Joint;
class CBitImage;
class CMultiCluGraph;

class CCluGraphConnector
{
public:
	CCluGraphConnector(int iPfAlg);
	~CCluGraphConnector();
	bool Connect(vector<CLayerMap*>& aLayers, CJointList * pJointList);
	void Release();
	
	Cluster * GetCluster(int iLayer, const A3DPOINT2& ptMap);
	void Export(CMultiCluGraph * pMultiCluGraph);
private:
	void _SetCluParam(CCluGraph * pCluGraph);
	void _AddLayer(CCluGraph * pCluGraph);
	int  _GetCluId(const A3DPOINT2& ptMap) const;
	void _AddJoint(const Joint * pJoint, vector<CLayerMap*>& aLayers);
	void _AddNode2Clu(int id, Cluster * pClu, CBitImage * pRMap);
private:
	int	  m_iPfAlg;
	int	  m_iCluSize;
	int   m_iRowCount;  //row count in single layer 
	int   m_iColCount;
	vector<vector<Cluster> >   m_aaClusters;
	CGGraph					*  m_pGraph;
};


}

#endif