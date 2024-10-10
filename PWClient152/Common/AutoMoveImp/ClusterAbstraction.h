/********************************************************************
	created:	2006/11/15
	author:		kuiwu
	
	purpose:	cluster abstraction of the original map
	credit:     Adi Botea(his paper&discussion)
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#ifndef  _CLUSTER_ABSTRACTION_H_
#define _CLUSTER_ABSTRACTION_H_



#include <vector.h>
using namespace abase;
#include <A3DTypes.h>

#include "CluCommon.h"


//#define  CLU_LOCAL_DEBUG 


class AFile;


namespace AutoMove
{


class CGGraph;
class CBitImage;
class CCluGraph;



class Entrance 
{
public:
  typedef enum{HORIZONTAL,VERTICAL} Orientation;
  Entrance(int z, int x, int cl1Id, int cl2Id,
					  int row, int col, int length,
					 Orientation orientation)
    :m_MapZ(z), m_MapX(x),
     m_Cluster1Id(cl1Id), m_Cluster2Id(cl2Id),
     m_Row(row), m_Col(col), m_Length(length),
     m_Orientation(orientation) {}

  int GetOrientation() {return m_Orientation;}
  int GetRow() {return m_Row;}
  int GetCol() {return m_Col;}
  void SetCluster1Id(int id) {m_Cluster1Id=id;}
  void SetCluster2Id(int id) {m_Cluster2Id=id;}  
  int GetMapZ() { return m_MapZ; }
  int GetMapX() { return m_MapX; }
  int GetCluster1Id() { return m_Cluster1Id; }
  int GetCluster2Id() { return m_Cluster2Id; } 
  

private:
  int m_MapZ; //z-coordinate of entrance in bottom / left cluster
  int m_MapX; //x-coordinate of entrance in bottom / left cluster
  int m_Cluster1Id; //bottom / left cluster's id
  int m_Cluster2Id; //top / right cluster's id
  int m_Row; // abstract row of the leftmost adjacent cluster
  int m_Col; // abstract col of the bottommost adjacent cluster
  int m_Length; // length of the entrance
  Orientation m_Orientation;

};



class CClusterAbstraction  
{
public:
	CClusterAbstraction(int iPfAlg);
	virtual ~CClusterAbstraction();
	void Init(CBitImage * pRMap);
	void Create(int clustersize,  int entrancewidth);
	void Release();

	//void RemoveSG(CGNode* start, CGNode* goal);
	CGGraph * GetAbsGraph()
	{
		return m_pGraph;
	}

	Cluster * GetClusterFromCoord(int x, int z);
	
	void Export(CCluGraph * pCluGraph);
private:
	void _CreateClustersAndEntrances();
	void _AddCluster(Cluster& c);
	void _CreateHorizEntrances(int start, int end, int latitude, int row, int col);
	void _CreateVertEntrances(int start, int end, int longitude, int row, int col);
	void _AddEntrance(Entrance& e);
    void _LinkEntrancesAndClusters();
	int  _GetClusterId(int row, int col) const;
	void _CreateAbstractGraph();
	void _AddAbsNodes(CGGraph * g);
	Cluster* _GetCluster(int id);
	void  _ComputeClusterPaths(CGGraph * g);
private:
	int	   m_iPfAlg;
	int    m_iClusterSize;
	int    m_iEntranceWidth;

	int    m_nRows;  //in cluster
	int    m_nCols;
	vector<Cluster>   m_Clusters;
	vector<Entrance>  m_Entrances;
	CGGraph         * m_pGraph;
	
	CBitImage         * m_pRMap;
};


}

#endif