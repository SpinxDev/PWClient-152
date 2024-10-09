/********************************************************************
  created:	   27/10/2005  
  filename:	   EC_BrushManUtil.h
  author:      Wangkuiwu  
  description: utility of brush manager
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma  once 

#include <vector.h>
#include "aabbcd.h"
#include "a3dgeometry.h"
#include "hashtab.h"
using namespace abase;
using namespace CHBasedCD;

class CBrushProvider;
class CAABBTreeLeaf;



//@note : verbose statistics. By Kuiwu[26/10/2005]
#define BMAN_VERBOSE_STAT  0
#define  BMAN_USE_GRID
//#define  BMAN_AUTO_BUILD

//#define  BMAN_TRACE_OUTOFRANGE

#if BMAN_VERBOSE_STAT
class CBManStat
{
public:
	DWORD dwBuildTime;
	DWORD dwTraceTime;
	int   nProvider;
	int   nBrush;
	int   nTraceBrush;
	CCDBrush * pHitBrush;
#ifdef BMAN_USE_GRID
	int   nOutOfRangeBrush;
	int   nCellSize;
	int   nWidth, nHeight;
#endif
	CBManStat()
	{
		dwBuildTime = dwTraceTime = 0;
		nProvider = nBrush = nTraceBrush = 0;
		pHitBrush = NULL;
#ifdef BMAN_USE_GRID
		nOutOfRangeBrush = nCellSize = nWidth = nHeight = 0;
#endif
	}
protected:
private:
};
#endif



class CBrushCell
{
public:
	short   m_iX, m_iZ;  //maybe useless
	A3DAABB   m_AABB;
	vector <CCDBrush * > m_pBrushes;

protected:
private:
};

//@desc : using grid to manage the brushes. By Kuiwu[2/11/2005]
class CBrushGrid
{
public:
	CBrushGrid(int iCellSize = 80, int iW = 11, int iH = 11 );
	~CBrushGrid();  //non-virtual
	void AddProvider(CBrushProvider * pProvider);
	bool RemoveProvider(CBrushProvider * pProvider);
	void Build(const A3DVECTOR3& vCenter, bool bForce = false);
	bool  Trace(BrushTraceInfo * pInfo, bool bCheckFlag = true);

#if BMAN_VERBOSE_STAT
	CBManStat * GetStat()
	{
		return &m_Stat;
	}
#endif
protected:
private:
	int   m_iCellSize;
	int   m_iW, m_iH;   //in cell
	short m_iCenterX, m_iCenterZ;
	
	float    m_fTraceRange2;
	
	CBrushCell * m_pBrushCell;  //as a brush cell buffer
	//@note : use hash table to accelerate searching cell buffer. By Kuiwu[2/11/2005]
	typedef hashtab<CBrushCell* , DWORD, abase::_hash_function>  CellTable;
	CellTable	m_CellTbl;

	vector<CBrushProvider *> m_UnOrganizedProvider;  //to be built.
	vector<CCDBrush * > m_OutOfRangeBrush; 
	
	inline void _GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z);
	inline CBrushCell * _FindCell(short x, short z);

	bool _UpdateCenter(const A3DVECTOR3& vCenter, bool bForce);
	inline bool _AddBrush(CCDBrush* pBrush);
#if BMAN_VERBOSE_STAT
	CBManStat     m_Stat;
#endif	

};