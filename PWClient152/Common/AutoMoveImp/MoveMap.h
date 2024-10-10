/********************************************************************
	created:	2006/05/14
	author:		kuiwu
	
	purpose:	map for pathfinding
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _MOVE_MAP_H_
#define _MOVE_MAP_H_




/************************************************************************/
// some global function and variables

//class A3DVECTOR3;
#include <A3DVector.h>
#include <A3DTypes.h>
#include <stdio.h>
#include <vector.h>
using namespace abase;
#include <AString.h>



namespace AutoMove
{



class CLayerMap;
class CJointList;
class CMultiCluGraph;

class CMoveMap
{
public:
	CMoveMap();
	~CMoveMap();
	void Release();
	bool Load(const char * szPath, vector<AString> * pLayerNames = NULL);
	bool Save(const char * szPath, const vector<AString>& layerNames, const AString& mluName);
	CLayerMap * CreateLayer();
	CJointList * CreateJointList();
	CMultiCluGraph * CreateMultiCluGraph(int iPfAlg);

	int	 GetLayerCount() const
	{
		return (int) m_aLayers.size();
	}
	CLayerMap * GetLayer(int index) const
	{
		if (index >= 0 && index < GetLayerCount())
		{
			return m_aLayers[index];
		}

		return NULL;
	}
	
	int      WhichLayer(const A3DPOINT2& ptMap, float dH, float *pLayerDist = NULL) const;

	//return min passable layer, if no passable layer return 0.
	int  GetMinPassableLayer(const A3DPOINT2& ptMap) const ;
	int  GetAllPassableLayers(const A3DPOINT2& ptMap,int* pLayers = NULL)const ;
	int GetMapWidth() const
	{
		return m_iMapWidth;
	}

	int GetMapLength() const
	{
		return m_iMapLength;
	}

	APointF TransMap2Wld(const A3DPOINT2& ptMap) const;
	APointF TransMap2Wld(const APointF& ptMap) const;
	A3DPOINT2 TransWld2Map(const APointF& ptWld) const;
	void CalcOrigin();
	void SetOrigin(const A3DVECTOR3 &vOrigin);
	
	CJointList * GetJointList() const
	{
		return	m_pJointList;
	}
	CMultiCluGraph * GetMultiCluGraph() const
	{
		return m_pMultiCluGraph;
	}
	
	void               ReleaseMultiCluGraph();
	float GetDH(int iLayer, const A3DPOINT2& ptMap) const;
	
private:
	void _SetMapCenterAsOrigin();

private:

	// /\
	// | |----------|
	// | |			|	
	// | |			|
	// | |----------|
	// |_ _ _ _ _ _>
	A3DVECTOR3  m_vOrigin;   // 搜索地图左下角对应世界中三维位置

	vector<CLayerMap *>  m_aLayers;
	CJointList		*    m_pJointList;
	CMultiCluGraph  *    m_pMultiCluGraph;
	int					 m_iMapWidth;
	int					 m_iMapLength;
	float				 m_fPixelSize;
};



};	// end of the namespace

#endif




