/********************************************************************
	created:	2006/10/08
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#ifndef _MOVE_AGENT_H_
#define _MOVE_AGENT_H_



#include <vector.h>
using namespace abase;
#include <A3DTypes.h>

#include "PfConstant.h"

namespace AutoMove
{


class CMoveMap;
class COptimizePath;
class CPfClu;

class CMoveAgent  
{
public:
	//	场景中凸包检测，用于在搜索位置附近选择合适搜索起点
	class BrushTest
	{
	public:
		virtual ~BrushTest(){}

		//	from.y、to.y 高度为 CLayerMap::GetDHMap 中存储的与地形的高度差
		virtual bool Collide(const A3DVECTOR3& from, const A3DVECTOR3& to)const = 0;
	};

public:
	CMoveAgent();
	~CMoveAgent();
	bool IsReady() const;
	bool Load(const char * szPath, const A3DVECTOR3 *pvOrigin=NULL);
	void Release();
	CMoveMap * GetMoveMap() const
	{
		return m_pMoveMap;
	}
	void SetBrushTest(BrushTest *p);
	bool SetStartEnd(const A3DPOINT2& ptStart, int iLayerStart, const A3DPOINT2& ptGoal, int iLayerGoal);	
	bool SetStartEnd(const A3DVECTOR3& vStart, int iLayerStart, const A3DVECTOR3& vGoal, int iLayerGoal, BrushTest *pBrushTest=NULL);
	int  WhichLayer(const A3DPOINT2& ptMap, float dH, float *pLayerDist = NULL);
	int  WhichLayer(const A3DVECTOR3& vWld, float dH, float *pLayerDist = NULL);
	int  GetMinPassableLayer(const A3DPOINT2& ptMap) const;
	int  GetAllPassableLayers(const A3DPOINT2& ptMap,int* pLayers = NULL)const;
	A3DPOINT2 TransWld2Map(const A3DVECTOR3& vWld);
	bool IsContain(const A3DVECTOR3& vWld);

	bool Search(abase::vector<PathNode> * pPath = NULL, int nMaxExpand=-1);
	void CreateOptimizer();
	abase::vector<PathNode> * Get2DPath() const;
	A3DVECTOR3 Get2DPathNode(int index)const;
	/**
	 * \brief retrieve the 3d path
	 * \param[in]
	 * \param[out]
	 * \return
	 * \note  the path node's y is the delta height from the terrain
	 * \warning
	 * \todo   
	 * \author kuiwu 
	 * \date 6/6/2008
	 * \see 
	 */
	void Retrieve3DPath(abase::vector<A3DVECTOR3>& path, int from = 0) const;
	int  GetPathCount() const;
	A3DVECTOR3 Get3DPathNode(const int index) const;

	/**
	 * \brief  optimize the path if necessary
	 * \param[in] moveIndex,  the current move index in the global path
	 * \param[in,out] remain3DPath, the remaining 3d path(excluding  moveIndex)after optimization if need optimize and the input param is not null.
	 * \return  true if need optimize
	 * \note   the 2d path will be updated if need optimize
	 * \warning
	 * \todo   
	 * \author kuiwu 
	 * \date 21/3/2007
	 * \see 
	 */
	bool Optimize(const int moveIndex, abase::vector<A3DVECTOR3>* remain3DPath = NULL);
	int  GetState() const
	{
		return m_iStat;
	}
	void SetPfAlg(int alg);
	int GetPfAlg() const
	{
		return m_iPfAlg;
	}

	int  GetOptimizeCatchCount() const;

	void ResetSearch();
	
private:
	bool          _CheckStart();
	bool          _CheckEnd();
	bool          _CheckThrough();
private:
	CMoveMap   * m_pMoveMap;
	CPfClu     * m_pPfAlg;
	int          m_iStat;
	COptimizePath * m_pPathOptimizer;
	int			 m_iPfAlg;
	A3DPOINT2    m_ptStart;
	int          m_iLayerStart;
	A3DPOINT2    m_ptGoal;
	int          m_iLayerGoal;
	BrushTest*  m_pBrushTest;
};

}

#endif