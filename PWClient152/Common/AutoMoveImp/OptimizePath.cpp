/********************************************************************
	created:	2007/03/12
	author:		kuiwu
	
	purpose:	A simple, efficient, and high quality path  optimizer
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/


#include "OptimizePath.h"
#include "MoveMap.h"
#include "bitimage.h"
#include "LayerMap.h"

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//local var


const int   LOOK_AHEAD = 60;
const int   LOOK_STEP  = 3;

//////////////////////////////////////////////////////////////////////////
//class CLine
void CLine::Init(APointF& from, int dirX, int dirY)
{
	Init(from, APointF((float)dirX, (float)dirY));
}

void CLine::Init(APointF& from, APointF& dir)
{
	this->from = from;
	this->dir = dir;
	
	//normalize
	float len = sqrtf(this->dir.x * this->dir.x + this->dir.y * this->dir.y);
	assert(len != 0.0f);
	this->dir.x /= len;
	this->dir.y /= len;
	count = 0;

}


APointF CLine::Next()
{
	count++;
	//set sample dist = 1.0
	return APointF(from.x + dir.x*count, from.y + dir.y * count);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptimizePath::COptimizePath()
: m_pMoveMap(NULL)
, m_MapWidth(0)
, m_MapHeight(0)
, m_CurIndex(-1)
, m_CatchCount(10)
, m_CurLayer(-1)
{
}

void COptimizePath::Reset()
{
	m_pMoveMap = NULL;
	m_MapWidth = 0;
	m_MapHeight = 0;
	m_LookUp.clear();
	m_Path.clear();
	m_CurIndex = -1;
	m_CurLayer = -1;
}

bool COptimizePath::NeedOptimize(int moveindex)
{
	if (m_CurIndex < (int)m_Path.size() 
		&& moveindex < (int)m_Path.size()
		&&  m_CurIndex-moveindex< m_CatchCount)
	{
		if (moveindex > m_CurIndex)
		{
			SetFootprintRange(m_CurIndex, moveindex-1, 0);
			m_CurIndex = moveindex;
		}
		return true;
	}
	return false;
}

void COptimizePath::_CheckLayer()
{
	if (m_Path[m_CurIndex].layer ==  m_CurLayer )
	{
		return;
	}

	m_CurLayer = m_Path[m_CurIndex].layer;
	m_LookUp.clear();

	int i = m_CurIndex;
	while ((i < (int)m_Path.size()) && (m_Path[i].layer == m_CurLayer))
	{
		SetFootprint(m_Path[i].ptMap, i+1);
		++i;
	}

}

void COptimizePath::SetupOptimize(CMoveMap *pMoveMap, const vector<PathNode>& initpath, int catchCount)
{
	assert(!initpath.empty());
	m_Path = initpath;
	
	m_pMoveMap = pMoveMap;
	m_MapWidth = pMoveMap->GetMapWidth();
	m_MapHeight = pMoveMap->GetMapLength();

	m_CurIndex = 0;
	m_CatchCount = catchCount;
	m_CurLayer = -1;

	if (m_Path.empty())
	{
		return;
	}
	
	_CheckLayer();
}


void COptimizePath::StepOptimize()
{

	_CheckLayer();

	int step = m_CatchCount*2;
    assert(step > 0);

	int i = 0;
	while (i < step && m_CurIndex < (int)m_Path.size())
	{
		_LocalOptimize();
		i++;
		m_CurIndex++;
	}
}

void COptimizePath::_LocalOptimize()
{
	int to_index = a_Min(m_CurIndex + LOOK_AHEAD, (int)m_Path.size()-1);  //see some nodes further
	
	CLine line;
	APointF dir;
	int new_count = -1;
	while (to_index - m_CurIndex > LOOK_STEP)
	{
		if (GetFootprint(m_Path[to_index].ptMap) == 0)
		{//not in the original path
			to_index -= LOOK_STEP;
			continue;
		}
		if (m_Path[to_index].layer != m_CurLayer)
		{
			to_index -= LOOK_STEP;
			continue;
		}
		dir = m_Path[to_index].ptMap - m_Path[m_CurIndex].ptMap;
		if (((int)dir.x) == 0 && ((int)dir.y) == 0)
		{
			_PathIntersect(to_index);
			return;
		}
		line.Init(m_Path[m_CurIndex].ptMap, dir);
		if (_LineTo(line, m_Path[to_index].ptMap))
		{
			new_count = line.GetCount();
			break;
		}
		to_index -= LOOK_STEP;
	}

	if (new_count > 0)
	{
		line.Reset();
		//_AddPathPortion(line, A3DPOINT2((int)m_Path[to_index].ptMap.x, (int)m_Path[to_index].ptMap.y), new_count);
		_AddPathPortion(line, to_index - m_CurIndex, new_count);
	}

}

CBitImage * COptimizePath::_GetRMap(int layer)
{
	if (m_pMoveMap){
		CLayerMap *pLayerMap = m_pMoveMap->GetLayer(layer);
		if (pLayerMap){
			return pLayerMap->GetRMap();
		}
	}
	return NULL;
}

bool COptimizePath::_LineTo(CLine& line, APointF& to)
{

#define  LOCAL_STRICT_LINE
	A3DPOINT2 to_pt((int)to.x, (int)to.y);
	A3DPOINT2 cur_pt((int)line.GetFrom().x, (int)line.GetFrom().y);

	CBitImage * pRMap = _GetRMap(m_CurLayer);
	assert(pRMap);

#ifdef LOCAL_STRICT_LINE
	A3DPOINT2  last_pt(cur_pt);

#endif
	while (cur_pt != to_pt )
	{
		
		APointF cur(line.Next());
		cur_pt.x = (int)cur.x;
		cur_pt.y = (int)cur.y;

		if (!pRMap->GetPixel(cur_pt.x, cur_pt.y))
		{
			return false;
		}

		bool bNeighbor1 = pRMap->GetPixel(last_pt.x, cur_pt.y);
		bool bNeighbor2 = pRMap->GetPixel(cur_pt.x, last_pt.y);

		if (to_pt.x == last_pt.x && to_pt.y == cur_pt.y)
		{
			break;
		}

		if (to_pt.x == cur_pt.x && to_pt.y == last_pt.y)
		{
			break;
		}

#ifdef LOCAL_STRICT_LINE
		if ((cur_pt.x != last_pt.x && cur_pt.y != last_pt.y) 
			&&(!bNeighbor1 || !bNeighbor2) )
		{
			return false;
		}
		last_pt = cur_pt;
#endif


	}
	
#undef  LOCAL_STRICT_LINE
	return true;
}



//m_CurIndex and to_index in the same node
void COptimizePath::_PathIntersect(const int to_index)
{
	//clear  the old footprint
	SetFootprintRange(m_CurIndex+1, to_index, 0);
	//erase 
	vector<PathNode>::iterator it1, it2;
	it1 = m_Path.begin() + m_CurIndex + 1;
	it2 = m_Path.begin() + to_index +1;
	
	m_Path.erase(it1, it2);

}

//exclude the oldCount + m_CurIndex
void COptimizePath::_AddPathPortion(CLine& line, const int oldCount, const int newCount)
{
	//clear  the old footprint
	SetFootprintRange(m_CurIndex+1, m_CurIndex+oldCount, 0);

	//adjust the path space between current to dest,  have oldCount, need newCount 
	if (oldCount > newCount)
	{
		//erase 
		vector<PathNode>::iterator it1, it2;
		it1 = &m_Path[m_CurIndex+1];
		it2 = it1 + oldCount - newCount;
		m_Path.erase(it1, it2);
	}
	else if (oldCount < newCount)
	{
		//insert
		vector<PathNode>::iterator it1;
		it1 = &m_Path[m_CurIndex+1];
		m_Path.insert(it1, newCount - oldCount, PathNode());
	}
	
	//replace the old path portion
	int index = m_CurIndex+1;
	while (line.GetCount() < newCount)
	{
		m_Path[index].ptMap = line.Next();
		m_Path[index].layer = m_CurLayer;
		index++;
	}

	// refresh new footprint
	SetFootprintRange(m_CurIndex+1, m_CurIndex+newCount, 1);

}

}