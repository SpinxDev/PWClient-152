/********************************************************************
	created:	2008/05/29
	author:		kuiwu
	
	purpose:	common files for cluster
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _CLU_COMMON_H_
#define _CLU_COMMON_H_

#include <A3DTypes.h>
#include <vector.h>
using namespace abase;




#include "GAStar.h"
#include "Graph.h"
#include "MAStar.h"



class AFile;

namespace AutoMove
{

class CGGraph;
class CBitImage;

//class expand limit

class CluExpandLimit:public IGExpandLimit, public IMExpandLimit

{
public:

	bool Test(CGNode * n)
	{
	  A3DPOINT2 pos = n->GetMapCoord();
	  if (pos.x < xMin || pos.x>= xMin+ xExt || pos.y <zMin || pos.y >= zMin+ zExt)
	  {
		  return false;
	  }
	  return true;	
	};

	bool Test(MapNode * n)
	{
		A3DPOINT2 pos(n->x, n->z);
	    if (pos.x < xMin || pos.x>= xMin+ xExt || pos.y <zMin || pos.y >= zMin+ zExt)
		{
		   return false;
		}
		 return true;	
	}

public:
	int xMin, zMin, xExt, zExt;
};

class Cluster 
{
public:
  Cluster(int horizOrigin, int vertOrigin,int width, int height)
    :m_XMin(horizOrigin),m_ZMin(vertOrigin),
     m_Width(width),m_Height(height)
  {
	  
  }

  Cluster(const Cluster& rhs);
  Cluster() {};

  Cluster& operator=(const Cluster& rhs);
  ~Cluster();
  int GetNodeId(int i) const { return m_Nodes[i];}
  int GetNodeCount() const { return m_Nodes.size(); }

  void SetNodeId(int id, int index);
  
  // Add an abstract node corresponding to an entrance to the cluster
  void AddNode(int n)
  {
	m_Nodes.push_back(n);
  }	



  /**
   * \brief try to connect the abstract node in the local cluster graph
   * \param[in]  pAbsGraph: the abstract graph
   * \param[in]  i: the ith abstract node in the cluster
   * \param[in]  j: the jth abstract node in the cluster	
   * \param[in]  pRMap: the rmap
   * \return     the cost if connected,  otherwise -1.0f
   * \note
   * \warning
   * \todo   
   * \author kuiwu 
   * \date 17/11/2006
   * \see 
   */
  float Connect(int iPfAlg, CGGraph * pAbsGraph, int i, int j, CBitImage * pRMap);
  int   GetXMin() const
  {
	  return m_XMin;
  }
  int GetZMin() const
  {
	  return m_ZMin;
  }
  int GetWidth()const
  {
	  return m_Width;
  }
  int GetHeight() const
  {
	  return m_Height;
  }

  vector<vector<A3DPOINT2> > * GetIntraPaths()
  {
	  return &m_IntraPahs;
  }
 /**
  * \brief check if a node already exists in a certain cluster with a particular set of coordinates.
  * \param[in]  x, z: map coordinates
  * \param[out] index: the node index in the cluster if neccessary
  * \return  the node id in the graph, -1 if the node not exist
  * \note
  * \warning
  * \todo   
  * \author kuiwu 
  * \date 20/11/2006
  * \see 
  */
  int NodeExists(long x, long z, CGGraph* g, int * index = NULL) const;

  void Save(AFile& fileimg);
  void Load(AFile& fileimg);
  
  
private:
  int m_XMin; // horizontal/column origin
  int m_ZMin; //vertical / row origin
  int m_Width; // width of this cluster
  int m_Height; // high of this cluster
 
  vector<int> m_Nodes; // node numbers in abstract graph of the entrances
  //CGGraph * m_pGraph;  // the cluster graph

  vector<vector<A3DPOINT2> > m_IntraPahs;
};

bool  LoadCluGraph(CGGraph * g, AFile& fileimg);
void SaveCluGraph(CGGraph * g, AFile& fileimg);

}
#endif