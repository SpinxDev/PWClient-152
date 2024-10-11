/********************************************************************
	created:	2006/11/16
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#ifndef _PF_CONSTNANT_H_
#define _PF_CONSTNANT_H_

#include <math.h>
#include <A3DTypes.h>

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//enum
enum  
{
	PF_STATE_UNKNOWN,
	PF_STATE_SEARCHING, 
	PF_STATE_FOUND    , 
	PF_STATE_NOPATH,
	PF_STATE_EXCEED,
	PF_STATE_INVALIDSTART,
	PF_STATE_INVALIDEND,
};

//path finding algorithm
enum
{
	PF_ALGORITHM_NONE = -1,
	PF_ALGORITHM_ASTAR = 0,
	PF_ALGORITHM_BFS,
	//PF_ALGORITHM_CLU,
	PF_ALGORITHM_CLU_G,  //clu + gastar(in single cluster)
	PF_ALGORITHM_CLU_MA,  //clu + mastar+ openarray(in single cluster)
	PF_ALGORITHM_CLU_MH,  //clu + mastar+ openheap(in single cluster)
	PF_ALGORITHM_COUNT
};

//dir
enum
{
   PF_NEIGHBOR_LEFT = 0,
   PF_NEIGHBOR_RIGHT,
   PF_NEIGHBOR_TOP,
   PF_NEIGHBOR_BOTTOM,
   PF_NEIGHBOR_TOPRIGHT,
   PF_NEIGHBOR_BOTTOMRIGHT,
   PF_NEIGHBOR_TOPLEFT,
   PF_NEIGHBOR_BOTTOMLEFT,
   PF_NEIGHBOR_COUNT
};

//////////////////////////////////////////////////////////////////////////
//defines
#define	 MAKE_DWORD(l, h)    ((DWORD)(((WORD)((l) & 0xffff)) | ((DWORD)((WORD)((h) & 0xffff))) << 16))
#define  GET_LOWORD(dw)		((WORD)((DWORD)(dw) & 0xffff))	
#define	 GET_HIWORD(dw)		((WORD)((DWORD)(dw) >> 16))

#define PF_MAP_INVALID_NODE    (-30000)

//////////////////////////////////////////////////////////////////////////
//types
struct PathNode 
{
	APointF ptMap;
	int       layer;


	A3DPOINT2 GetPtI()
	{
		return A3DPOINT2((int)ptMap.x, (int)ptMap.y);
	}
	void SetI(const A3DPOINT2& pt, int iLayer)
	{
		ptMap.x = (float)pt.x;
		ptMap.y = (float)pt.y;
		layer = iLayer;
	}
};

//////////////////////////////////////////////////////////////////////////
//const var
const double   PF_ROOT_TWO  = 1.414;
// Floating point comparisons 
const double ZERO_TOLERANCE = 0.000001;    // floating point tolerance

	
//////////////////////////////////////////////////////////////////////////
//inline funcs
inline bool fless(double a, double b) { return (a < b - ZERO_TOLERANCE); }
inline bool fgreater(double a, double b) { return (a > b + ZERO_TOLERANCE); }
inline bool fequal(double a, double b)
{ 
	return (a > b - ZERO_TOLERANCE) && (a < b+ZERO_TOLERANCE); 
}

inline float GetManhDist(int x1, int z1, int x2, int z2)
{
	 return (float)(fabs((float)x1- x2) + fabs((float)z1 - z2));
}
inline float GetEuclDist(int x1, int z1, int x2, int z2)
{
	return (sqrtf((x1 - (float)x2)*(x1 -x2) + (z1 -z2)*(z1 -z2)));
}


//////////////////////////////////////////////////////////////////////////
//extern var
extern int  PF2D_NeighborD[];
extern float PF2D_NeighborCost[];

} //end of namespace

#ifdef _AUTOMOVE_EDITOR
extern void TransMap2Scr(const A3DPOINT2& ptMap, A3DPOINT2& ptScr);
#endif

#endif