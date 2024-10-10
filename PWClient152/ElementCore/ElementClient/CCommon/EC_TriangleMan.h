/********************************************************************
  created:	   8/11/2005  
  filename:	   EC_TriangleMan.h
  author:      Wangkuiwu  
  description: Triangle manager, mainly used by shadow generator.
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma  once

#include <vector.h>
#include "a3dgeometry.h"
#include "hashtab.h"
using namespace abase;

//#define TMAN_VERBOSE_STAT


class CTriangleGrid;
class CTriangleCell;
struct TManTriangle;
class CTManTrianglePool;


//@note : NOT define an abstract interface for triangle provider.
//		  Use CELBrushBuilding directly.
//		  By Kuiwu[8/11/2005]
class CELBrushBuilding;
//build flag
enum
{
	TMAN_BUILD_DEFAULT = 0,
	TMAN_BUILD_FORCE = 1
};

//check  flag
enum
{
	TMAN_CHECK_DEFAULT = 0,				//fast, but not accurate
	TMAN_CHECK_CULLBACK = 1,			//cull back face
	TMAN_CHECK_STRICT   =  (1<<1),		//more accurate but a little slow
	TMAN_CHECK_OUTOFRANGE = (1<<2),		//check with out-of-range triangle, OBSOLETE!
	TMAN_CHECK_FRUSTUM = (1<<3),		// view frustum culling
  	TMAN_CHECK_AUTOBUILD = (1<<4),      //auto build when necessary    
	TMAN_CHECK_EXACT = (1<<5)			//exact check but expensive
};

typedef hashtab<CTriangleCell* , DWORD, abase::_hash_function>  TManCellTable;

struct  TMan_Overlap_Info
{
	//input
	A3DVECTOR3   vStart;			// as an aabb's center
	A3DVECTOR3   vExtent;     
	A3DVECTOR3   vDelta;
	A3DCameraBase    *pCamera;			//optional, for frustum culling
	DWORD        dwFlag;			//check flag, OBSOLETE!

	//output
	vector<A3DVECTOR3 >  pVert;   
	vector<WORD>         pIndex;  
	TMan_Overlap_Info()
	{
		pCamera = NULL;
		dwFlag = TMAN_CHECK_DEFAULT;
		vDelta.Clear();
		pIndex.clear();
	}

};

#ifdef TMAN_VERBOSE_STAT
class CTManStat
{
public:
	DWORD dwBuildTime;
	DWORD dwTestTime;
	int   nProvider;  // all provider
	int   nUnorgPrv;
	int   nTriangle;  // org triangle 
	int   nTestTriangle;
	int   nOutOfRangeTriangle;
	int   nCellSize;
	int   nWidth, nHeight;
	int   nAddTriangle;  //the number of triangles that added during one build process.
	//CTriangleCell  * pCells;
	TManCellTable * pCellTbl;
	A3DOBB  obb;
	CTManStat()
	{
		dwBuildTime = dwTestTime = 0;
		nUnorgPrv = nProvider = nTriangle = nTestTriangle = nOutOfRangeTriangle = 0;
		nCellSize = nWidth = nHeight =  nAddTriangle = 0;
		obb.Clear();
	}
protected:
private:
};
#endif


//@note : Maybe it is better to use a template class to 
//		  implement the grid method(brush grid, triangle grid, etc). 
//		  By Kuiwu[8/11/2005]
class CTriangleGrid
{
public:
	CTriangleGrid(int iCellSize = 10, int iW =11, int iH = 11);
	~CTriangleGrid();
	void AddProvider(CELBrushBuilding * pProvider);
	bool RemoveProvider(CELBrushBuilding * pProvider);
	void Build(const A3DVECTOR3& vCenter, DWORD dwFlag = TMAN_BUILD_DEFAULT);
	void BuildGradually(const A3DVECTOR3& vCenter, DWORD dwFlag = TMAN_BUILD_DEFAULT);
	
	void TestOverlap(TMan_Overlap_Info& tInfo);
	/*
	 * discard the flag.
	 * @desc :
	 * @param :     
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [18/11/2005]
	 * @ref:
	 */
	void TestOverlapNF(TMan_Overlap_Info& tInfo);

#ifdef TMAN_VERBOSE_STAT
	CTManStat * GetStat()
	{
		return &m_Stat;
	}
	
#endif	
protected:
private:
	int   m_iCellSize;
	int   m_iW, m_iH;   //in cell
	short m_iCenterX, m_iCenterZ;
	
	int   m_nCellCount;  //max cell count
	//CTriangleCell * m_pTriangleCell;  //as a triangle cell buffer

	//@note : use hash table to accelerate searching cell buffer. By Kuiwu[8/11/2005]
	TManCellTable	m_CellTbl;



	float   m_fOffset;   //push out dist
	
	vector<CELBrushBuilding *> m_UnOrganizedProvider;  //to be built.
	//vector<TManTriangle > m_OutOfRangeTriangle; 

	CTManTrianglePool   * m_pTrianglePool;

	DWORD                m_dwTestStamp;

	bool _UpdateCenter(const A3DVECTOR3& vCenter, DWORD dwFlag);
	inline void _GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z);
	inline CTriangleCell * _FindCell(short x, short z);
	inline	bool   _AddTriangle(TManTriangle * pTri, int index);

	bool   _TestTriangle(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri);
	inline bool _TestTriangleNF(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri, const A3DMATRIX4& w2lMat, const A3DMATRIX4& l2wMat);
	void   _CompactCellTbl();
	void   _ReleaseCellTbl();
	void   _BuildOBB(const TMan_Overlap_Info& tInfo, A3DOBB& obb);

	int    _AddProvider(CELBrushBuilding * pProvider);
	
#ifdef TMAN_VERBOSE_STAT
	CTManStat  m_Stat;
#endif
	
};

class CECTriangleMan  
{
public:
	CECTriangleMan();
	virtual ~CECTriangleMan();
	void AddProvider(CELBrushBuilding * pProvider);
	bool RemoveProvider(CELBrushBuilding * pProvider);
	void Build(const A3DVECTOR3& vCenter, DWORD dwFlag  = TMAN_BUILD_DEFAULT);
	void TestOverlap(TMan_Overlap_Info& tInfo);
#ifdef TMAN_VERBOSE_STAT
	CTManStat * GetStat()
	{
		return m_pGrid->GetStat();
	}
#endif
private:
	CTriangleGrid * m_pGrid;

};


struct TManTriangle
{
	CELBrushBuilding * pProvider;
	//A3DVECTOR3  * pVert[3];
	A3DVECTOR3     vVert[3];
	
	//A3DVECTOR3    vCenter;
	A3DVECTOR3    vNormal;  //face normal
	DWORD         dwCheckStamp;
	//@note : not need a DWORD, but for alignment. By Kuiwu[18/11/2005]
	DWORD          dwRef;     // ref count,  by triangle cell
	
	TManTriangle()
	:pProvider(NULL), dwCheckStamp(0), dwRef(0)
	{
	}
	
};


class CTriangleCell
{
public:
	A3DAABB  m_AABB;
	//vector<TManTriangle *>   m_Triangles;
	vector<int>   m_Triangles;
	CTriangleCell()
	{
		m_AABB.Clear();
		m_Triangles.clear();
	}
protected:
private:
};


class CTManTrianglePool
{
public:
	CTManTrianglePool(int nCount = 40000, int nGrow = 40000);
	~CTManTrianglePool();
	inline int CreateTriangle();
	TManTriangle * GetTriangle(int index)
	{
		return &m_pBuf[index];
	}
protected:
private:
	TManTriangle * m_pBuf;
	int            m_nMaxCount;

	int            m_nLast;
	int            m_nGrow;

	inline void _Grow(int  newcount);
};