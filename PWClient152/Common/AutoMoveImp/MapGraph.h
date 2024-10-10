/********************************************************************
	created:	2008/05/27
	author:		kuiwu
	
	purpose:	graph for map to accelerate searching
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _MAP_GRAPH_H_
#define _MAP_GRAPH_H_

#include <A3DTypes.h>

namespace AutoMove
{

class CBitImage;
class CGGraph;
class CGNode;
class CGEdge;

class CMapGraph
{
public:
	CMapGraph();
	~CMapGraph();
	void Release();
	void SetRMap(CBitImage * pRMap);
	void Reset();
	CGNode * GetNode(const A3DPOINT2& ptMap);
	CGEdge * GetEdge(const A3DPOINT2& ptMap1, const A3DPOINT2& ptMap2, float cost);
	CBitImage * GetRMap() const
	{
		return m_pRMap;
	}
	CGGraph * GetGraph() const
	{
		return m_pGraph;
	}
private:
	CGGraph * m_pGraph;
	int   * m_pCoord2Node;
	CBitImage * m_pRMap;
	int         m_iWidth;
	int         m_iLength;
};


}

#endif