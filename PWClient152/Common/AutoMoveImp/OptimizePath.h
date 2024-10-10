/********************************************************************
	created:	2007/03/12
	author:		kuiwu
	
	purpose:	A simple, efficient, and high quality path  optimizer
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _OPTIMIZE_PATH_H_
#define  _OPTIMIZE_PATH_H_


#include <vector.h>
#include <hashmap.h>
using namespace abase;
#include <A3DTypes.h>

//#include "MoveMap.h"
#include "PfConstant.h"

namespace AutoMove
{

class CLine
{
public:
	CLine(){};
	~CLine(){};
	void Init(APointF& from, int dirX, int dirY);
	void Init(APointF& from, APointF& dir);
	APointF  Next();
	int      GetCount() { return count;};
	APointF  GetFrom() {return from;};
	void  Reset() {count = 0;};
	APointF GetDir(){return dir;};
private:
	APointF  from;
	APointF  dir;  
	int      count;
	
};	

class CMoveMap;
class CBitImage;

class COptimizePath  
{
public:
	COptimizePath();
	void SetupOptimize(CMoveMap *pMoveMap, const vector<PathNode>& initpath, int catchCount = 10);
	void StepOptimize();
	bool NeedOptimize(int moveindex);
	vector<PathNode>* GetPath()
	{
		return &m_Path;
	}
	
	inline int GetFootprint(int x, int y)
	{
		LookUpMap::iterator it = m_LookUp.find(_GetLookUpKey(x, y));
		return it == m_LookUp.end() ? 0 : it->second;
	}
	inline int GetFootprint(const APointF& pt)
	{
		return GetFootprint((int)pt.x, (int)pt.y);
	}
	
	inline void SetFootprint(int x, int y, int val)
	{
		if (val == 0){
			m_LookUp.erase(_GetLookUpKey(x, y));
		}else{
			m_LookUp[_GetLookUpKey(x, y)] = val;
		}
	}
	
	inline void SetFootprint(const APointF& pt, int val)
	{
		SetFootprint((int)pt.x, (int)pt.y, val);
	}

	inline void SetFootprintRange(int fromIndex, int toIndex, int val)
	{
		for (int i = fromIndex; i <= toIndex; ++ i)
		{
			SetFootprint(m_Path[i].ptMap, val);
		}
	}

	inline	int        GetCatchCount() const
	{
		return m_CatchCount;
	}
	void Reset();
private:
	void _AddPathPortion(CLine& line, const int oldCount, const int newCount);

	void _LocalOptimize();
	bool _LineTo(CLine& line, APointF& to);
	void _CheckLayer();
	void _PathIntersect(const int to_index);

	CBitImage * _GetRMap(int layer);
	int _GetLookUpKey(int x, int y)
	{
		return (y*m_MapWidth + x);
	}
private:
	CMoveMap *				   m_pMoveMap;
	int                        m_MapWidth;
	int                        m_MapHeight;
	typedef abase::hash_map<int, short>	LookUpMap;
	LookUpMap					m_LookUp;   //used for accelerate optimizing
	abase::vector<PathNode>		m_Path;
	int                        m_CurIndex; // next node to optimize
	int                        m_CatchCount;    //the node count that movement catches optimization
	int                        m_CurLayer;

};


}

#endif