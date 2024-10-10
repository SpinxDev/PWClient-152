/********************************************************************
	created:	2006/11/15
	author:		kuiwu
	
	purpose:	cluster abstraction of the original map
	credit:     Adi Botea(his paper&discussion)
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#include <AFile.h>
#include <A3DMacros.h>

#ifdef _AUTOMOVE_EDITOR
#include <A3DViewport.h>
#endif


#include "ClusterAbstraction.h"
#include "Graph.h"
#include "PfConstant.h"
#include "GAStar.h"
#include "bitimage.h"
#include "MapGraph.h"
#include "CluGraph.h"
#include "MoveMap.h"

namespace AutoMove
{





/************************************************************************/
/* cluster abstract                                                     */
/************************************************************************/

CClusterAbstraction::CClusterAbstraction(int iPfAlg)
:m_nCols(-1), m_nRows(-1),
m_pGraph(NULL), m_iClusterSize(-1), m_iEntranceWidth(-1),
m_pRMap(NULL),m_iPfAlg(iPfAlg)
{

}

CClusterAbstraction::~CClusterAbstraction()
{
	Release();
}

void CClusterAbstraction::Init(CBitImage * pRMap)
{
	m_pRMap = pRMap;
}

void CClusterAbstraction::Release()
{
	A3DRELEASE(m_pGraph);
	m_Clusters.clear();
	m_Entrances.clear();
}

void CClusterAbstraction::Create(int clustersize, int entrancewidth)
{
	m_iClusterSize = clustersize;
	m_iEntranceWidth = entrancewidth;
	_CreateClustersAndEntrances();
	_LinkEntrancesAndClusters();
	_CreateAbstractGraph();

}

/**
 * create clusters on the abstract level and create corresponding  entrances. 
 */ 
void CClusterAbstraction::_CreateClustersAndEntrances()
{
	int col=0, row=0;
	int horizSize,vertSize;
	
	int mapW, mapH;
	
	m_pRMap->GetImageSize(mapW, mapH);

	int i, j;
	
	for (j = 0; j < mapH; j+=m_iClusterSize)
	{
		col=0; 
		for (i = 0; i< mapW; i+=  m_iClusterSize)
		{
			horizSize = a_Min(m_iClusterSize, mapW-i);
			vertSize  = a_Min(m_iClusterSize, mapH-j);
			
			Cluster cluster(i,j,horizSize,vertSize);
			
			_AddCluster(cluster);

			if (j > 0 && j < mapH )
			{
				_CreateHorizEntrances(i,i+horizSize-1,j-1,row-1,col);
			}
			if (i > 0 && i < mapW)
			{
				
				_CreateVertEntrances(j,j+vertSize-1,i-1,row,col-1);
			}
			col++;
		}
		row++;
	}  
	
	m_nRows = row;
	m_nCols = col;
	
	
}


void CClusterAbstraction::_AddCluster(Cluster& c)
{
	m_Clusters.push_back(c);
}

void CClusterAbstraction::_AddEntrance(Entrance& e)
{
	m_Entrances.push_back(e);
}


/*
 * Create horizontal entrances. Between 2 obstacles, there is either one or two entrances,
 * depending on how many tiles there are between these obstacles. 
 */ 
void CClusterAbstraction::_CreateHorizEntrances(int start, int end, int latitude, int row, int col)
{

	for (int i=start; i<=end; i++)
	{
		while((i<=end) && (!m_pRMap->GetPixel(i,latitude) || !m_pRMap->GetPixel(i,latitude+1)))
		{
			i++;
		}
		if (i>end)
			return;
		
		int begin = i;
		i++;
		
		while((i<=end)&&(m_pRMap->GetPixel(i,latitude)) && (m_pRMap->GetPixel(i,latitude+1)))
		{
			i++;
		}
		
		//add entrance(s)
		if ((i - begin) >= m_iEntranceWidth)
		{
			// create two new entrances, one for each end
			Entrance entrance1(latitude, begin,  -1,-1, row, col, 1, Entrance::HORIZONTAL);
			_AddEntrance(entrance1);
			Entrance entrance2(latitude, (i - 1),-1,-1, row, col, 1, Entrance::HORIZONTAL);
			_AddEntrance(entrance2);
		}
		else 
		{
			// create one entrance in the middle 
			Entrance entrance(latitude, ((i - 1) + begin)/2,-1,-1,	row, col, (i - begin), Entrance::HORIZONTAL);
			_AddEntrance(entrance);
		}
		i--;
	}

}

void CClusterAbstraction::_CreateVertEntrances(int start, int end, int longitude, int row, int col)
{
	for (int i=start; i<=end; i++)
	{
		while((i<=end)&& (!m_pRMap->GetPixel(longitude,i) || !m_pRMap->GetPixel(longitude+1,i)))
		{
			i++;
		}
		if (i>end)
			return;
		
		int begin = i;
		i++;

		while((i<=end) && (m_pRMap->GetPixel(longitude, i)) && (m_pRMap->GetPixel(longitude+1, i)))
		{
			i++;
		}

		//add entrance(s)
		if ((i - begin) >= m_iEntranceWidth)
		{
			// create two entrances, one for each end
			Entrance entrance1(begin, longitude,-1,-1, row, col, 1, Entrance::VERTICAL);
			_AddEntrance(entrance1);
			Entrance entrance2((i - 1), longitude,-1,-1,row, col, 1, Entrance::VERTICAL);
			_AddEntrance(entrance2);
		}
		else
		{
			// create one entrance
			Entrance entrance(((i - 1) + begin)/2, longitude,-1,-1,row, col, (i - begin), Entrance::VERTICAL);
			_AddEntrance(entrance);
		}
		
		i--;
		
	}
}


int CClusterAbstraction::_GetClusterId(int row, int col)const
{
		return row * m_nCols + col;

}


Cluster * CClusterAbstraction::GetClusterFromCoord(int x, int z)
{
	int row = z / m_iClusterSize;
	int col = x / m_iClusterSize;

	if (row >= 0 && row <m_nRows && col >= 0 && col < m_nCols)
	{
		return &m_Clusters[row * m_nCols + col];
	}

	return NULL;
}


void CClusterAbstraction::_LinkEntrancesAndClusters()
{
	int cluster1Id;
	int cluster2Id;
	for (unsigned int i = 0; i < m_Entrances.size(); i++)
	{
		
		Entrance &entrance = m_Entrances[i];
		switch (entrance.GetOrientation())
		{
		case Entrance::HORIZONTAL:
			{
				cluster1Id = _GetClusterId(entrance.GetRow(), entrance.GetCol());  //bottom
				cluster2Id = _GetClusterId(entrance.GetRow() + 1, entrance.GetCol()); //top
				
				// update entrance
				entrance.SetCluster1Id(cluster1Id);
				entrance.SetCluster2Id(cluster2Id);
			}
				break;
			case Entrance::VERTICAL:
			{
				cluster1Id = _GetClusterId(entrance.GetRow(), entrance.GetCol());  //left
				cluster2Id = _GetClusterId(entrance.GetRow(), entrance.GetCol() + 1); //right
				entrance.SetCluster1Id(cluster1Id);
				entrance.SetCluster2Id(cluster2Id);
			}
				break;
			default:
				assert(false);
				break;
		}
	}
}

/*
 * create the abstract graph: create a node for each entrance and connect entrances with their 
 * "counterparts" in adjacent clusters, then create intra edges. 
 */
void CClusterAbstraction::_CreateAbstractGraph()
{
	m_pGraph = new CGGraph;
	_AddAbsNodes(m_pGraph);
	_ComputeClusterPaths(m_pGraph);
}


Cluster* CClusterAbstraction::_GetCluster(int id)
{
	assert (0 <= id && id < (int)m_Clusters.size());
	return &m_Clusters[id];
}


/**
 * Add a node for cluster for each entrance
 */
void CClusterAbstraction::_AddAbsNodes(CGGraph * g)
{
	int newnode1=-1;
	int newnode2=-1;
	int num=-1;
	for (unsigned int i=0; i<m_Entrances.size(); i++)
	{
		Entrance &entrance = m_Entrances[i];
		int cluster1Id = entrance.GetCluster1Id();
		int cluster2Id = entrance.GetCluster2Id();
		switch (entrance.GetOrientation())
		{
		case Entrance::HORIZONTAL:
			{
				// create node for 1st cluster
				Cluster* c1 = _GetCluster(cluster1Id);
				num = c1->NodeExists(entrance.GetMapX(), entrance.GetMapZ(),g);
				if (num==-1)
				{
					
					CGNode* n1 = new CGNode();
					newnode1 = g->AddNode(n1);
					n1->SetMapCoord(entrance.GetMapX(), entrance.GetMapZ());
					c1->AddNode(newnode1);
				}
				else
				{
					newnode1 = num;
				}
				
				
				Cluster* c2 = _GetCluster(cluster2Id);	  
				num = c2->NodeExists(entrance.GetMapX(),entrance.GetMapZ()+1,g);
				if (num==-1)
				{
					
					CGNode* n2= new CGNode();
					newnode2 = g->AddNode(n2);
					n2->SetMapCoord(entrance.GetMapX(), entrance.GetMapZ()+1);
					c2->AddNode(newnode2);
				}
				else
				{
					newnode2=num;
				}
				
				//add inter-edge
				g->AddEdge(new CGEdge(newnode1,newnode2,1));
				
			}
				break;
		case Entrance::VERTICAL:
			{
				Cluster* c1 = _GetCluster(cluster1Id);
				num = c1->NodeExists(entrance.GetMapX(),entrance.GetMapZ(),g);
				if (num==-1)
				{
					
					CGNode* n1 = new CGNode();
					newnode1 = g->AddNode(n1);
					n1->SetMapCoord(entrance.GetMapX(), entrance.GetMapZ());
					c1->AddNode(newnode1);
				}
				else
				{
					newnode1 = num;
				}
				
								
				Cluster* c2 = _GetCluster(cluster2Id);
				num = c2->NodeExists(entrance.GetMapX()+1,entrance.GetMapZ(),g);
				if (num==-1)
				{
					
					CGNode* n2 = new CGNode();
					
					newnode2 = g->AddNode(n2);
					n2->SetMapCoord(entrance.GetMapX()+1, entrance.GetMapZ());
					c2->AddNode(newnode2);
				}
				else
				{
					newnode2=num;
				}
				//add inter-edge
				g->AddEdge(new CGEdge(newnode1, newnode2, 1));
				
			}	     
				break;
			default:
				assert(false);
				break;
		}
	}
		
}


/*
 * Compute the paths inside each cluster. For each pair of entrances inside a cluster, find out if 
 * there is a path that only uses nodes inside the cluster. If there is, add an edge to the abstract
 * graph with the path distance as its weight and cache the path in the vector(hash map???).  
 * 
 */
void CClusterAbstraction::_ComputeClusterPaths(CGGraph * g)
{
	int i;
	for (i=0; i< (int)m_Clusters.size(); i++)
	{
		Cluster& c = m_Clusters[i];
		
		int j, k;
		for (j = 0;  j < c.GetNodeCount(); j++)
		{
			for (k = j+1; k < c.GetNodeCount(); k++)
			{
				float cost = c.Connect(m_iPfAlg, g, j, k, m_pRMap);
				if (cost >= 0.0f)
				{
					CGEdge * newedge = new CGEdge(c.GetNodeId(j), c.GetNodeId(k), cost);
					g->AddEdge(newedge);
				}
			}
		}

	}	
}

#ifdef _AUTOMOVE_EDITOR
/*
void CClusterAbstraction::Render(A3DViewport * pViewPort)
{
	
	A3DCOLOR   col = A3DCOLORRGB(255,255, 255);
	unsigned int i;
	A3DCameraBase * pCamera = pViewPort->GetCamera();


	//cluster
	for (i=0; i<m_Clusters.size(); i++)
	{
		Cluster& c = m_Clusters[i];
		A3DPOINT2 ptScr0, ptScr1, ptScr2, ptScr3;

		A3DVECTOR3 v0, v1, v2, v3;
		v0 = g_AutoMoveMap.Trans2DTo3D(A3DPOINT2(c.GetXMin(),        c.GetZMin()));
		v1 = g_AutoMoveMap.Trans2DTo3D(A3DPOINT2(c.GetXMin(),        c.GetZMin() + c.GetHeight() ));
		v2 = g_AutoMoveMap.Trans2DTo3D(A3DPOINT2(c.GetXMin() +c.GetWidth(), c.GetZMin()+c.GetHeight()));
		v3 = g_AutoMoveMap.Trans2DTo3D(A3DPOINT2(c.GetXMin() +c.GetWidth(),c.GetZMin()));

		

		if (pCamera->PointInViewFrustum(v0)
			||pCamera->PointInViewFrustum(v1)
			||pCamera->PointInViewFrustum(v2)
			||pCamera->PointInViewFrustum(v3))
		{
		
			TransMap2Scr(A3DPOINT2(c.GetXMin(),        c.GetZMin()), ptScr0);
			TransMap2Scr(A3DPOINT2(c.GetXMin(),        c.GetZMin() + c.GetHeight() ), ptScr1);
			TransMap2Scr(A3DPOINT2(c.GetXMin() +c.GetWidth(), c.GetZMin()+c.GetHeight()), ptScr2);
			TransMap2Scr(A3DPOINT2(c.GetXMin() +c.GetWidth(),c.GetZMin()), ptScr3);
			g_pA3DGDI->Draw2DLine(ptScr0, ptScr1, col);	
			g_pA3DGDI->Draw2DLine(ptScr1, ptScr2, col);
			g_pA3DGDI->Draw2DLine(ptScr2, ptScr3, col);
			g_pA3DGDI->Draw2DLine(ptScr3, ptScr0, col);
		}
		
	}
	//graph
	if (m_pGraph)
	{
		vector<CGNode *>* nodes = m_pGraph->GetAllNodes();
		A3DPOINT2 ptScr0, ptScr1;
		col = A3DCOLORRGB(0, 0, 0);
		for (i = 0; i < nodes->size(); i++)
		{
			A3DVECTOR3 v0;
			v0 = g_AutoMoveMap.Trans2DTo3D((*nodes)[i]->GetMapCoord());
			if (pCamera->PointInViewFrustum(v0))
			{
				TransMap2Scr((*nodes)[i]->GetMapCoord(), ptScr0);
				g_pA3DGDI->Draw2DPoint(ptScr0, col, 3.0f);
			}
		}

// 		col = A3DCOLORRGB(0, 0, 255);
// 		vector<CGEdge*>* edges = m_pGraph->GetAllEdges();
// 		for (i = 0;  i < edges->size(); i++)
// 		{
// 			int from, to;
// 			from = (*edges)[i]->GetFrom();
// 			to   = (*edges)[i]->GetTo();
// 			CGNode * n1 = m_pGraph->GetNode(from);
// 			CGNode * n2 = m_pGraph->GetNode(to);
// 			A3DVECTOR3 v0, v1;
// 			v0 = g_AutoMoveMap.Trans2DTo3D(n1->GetMapCoord());
// 			v1 = g_AutoMoveMap.Trans2DTo3D(n2->GetMapCoord());
// 			
// 			if (pCamera->PointInViewFrustum(v0)
// 			||pCamera->PointInViewFrustum(v1))
// 			{
// 				TransMap2Scr(n1->GetMapCoord(), ptScr0);
// 				TransMap2Scr(n2->GetMapCoord(), ptScr1);
// 				g_pA3DGDI->Draw2DLine(ptScr0, ptScr1, col);
// 			}
// 			
// 
// 		}

	}

}
*/

#endif

#if 0
/**
 * remove a start or goal node after the search has been completed. The node is
 * removed, as well as all the edges that were added to connect it to the cluster. 
 * the caller should assure the start&goal not in the original graph
 */ 
void CClusterAbstraction::RemoveSG(CGNode* start, CGNode* goal)
{
	 int oldid;
	 //inverse order of insert
 	 if (goal)
	 {
		 m_pGraph->RemoveNode(goal, oldid);
	 }

	 if (start)
	 {
		 m_pGraph->RemoveNode(start, oldid);
	 }

}

#endif


void CClusterAbstraction::Export(CCluGraph * pCluGraph)
{
	pCluGraph->SetCluSize(m_iClusterSize);
	pCluGraph->SetRowCount(m_nRows);
	pCluGraph->SetColCount(m_nCols);
	vector<Cluster> * pClusters = pCluGraph->GetAllClusters();
	*pClusters = m_Clusters;
	pCluGraph->SetGraph(m_pGraph);
}

}