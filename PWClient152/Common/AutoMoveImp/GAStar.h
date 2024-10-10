/********************************************************************
	created:	2006/11/16
	author:		kuiwu
	
	purpose:	little graph a*  (typically search space < 300*300)
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#pragma once

#include <A3DTypes.h>
#include <vector.h>
using namespace abase;

namespace AutoMove
{

class CGGraph;
class CGNode;
class CGEdge;
class CGHeap;
class CBitImage;

class IGExpandLimit
{
public:
	virtual	bool Test(CGNode * n) = 0;
};


class CGAStar  
{
public:

	CGAStar();
	virtual ~CGAStar();

	/**
	 * \brief 
	 * \param[in]
	 * \param[out]
	 * \return  the path cost, -1.0 means no path
	 * \note
	 * \warning
	 * \todo   
	 * \author kuiwu 
	 * \date 16/11/2006
	 * \see 
	 */
	float Search(CGGraph * g, CGNode * from, CGNode * to, vector<CGNode*>& path);
	float Search(CBitImage * rmap, A3DPOINT2& from, A3DPOINT2& to, vector<A3DPOINT2>& path, IGExpandLimit * expandLimit = NULL);

	static float GetHeuristic(CGNode * start, CGNode * target);
	int GetStat()
	{
		return m_State;
	}
	void SetMaxExpand(int maxExpand)
	{
		m_MaxExpand = maxExpand;
	}
private:

	void _RelaxEdge(CGHeap *nodeHeap, CGEdge *e, CGNode * source, CGNode * through, CGNode *dest);
	double  _GeneratePath(CGGraph * g, unsigned int dest, vector<CGNode*>& path);

private:
	int   m_NodesExpanded;
	int   m_NodesTouched;
	int   m_State;
	int   m_MaxExpand;

};

}
