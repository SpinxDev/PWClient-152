/********************************************************************
	created:	2006/05/14
	author:		kuiwu
	
	purpose:	map for pathfinding
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#include <A3DMacros.h>
#include <assert.h>

#include "MoveMap.h"
#include "LayerMap.h"
#include "bitimage.h"
#include "JointList.h"
#include "MultiCluGraph.h"
#include "CluGraphConnector.h"

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//define&const
#define	   PFMAP_CFG_FILE_MAGIC      (DWORD)(('c'<<24)| ('f'<<16)|('g'<<8)|('f'))
#define    PFMAP_CFG_FILE_VERSION     1

const     float      SAME_LAYER_DH   = 1.0f;

//////////////////////////////////////////////////////////////////////////
//local vars


//////////////////////////////////////////////////////////////////////////
//local funcs
	
//////////////////////////////////////////////////////////////////////////
//static

//////////////////////////////////////////////////////////////////////////
//class CMoveMap
CMoveMap::CMoveMap()
:m_pJointList(NULL), m_pMultiCluGraph(NULL)
{
}

CMoveMap::~CMoveMap()
{
}

void CMoveMap::Release()
{
	int i;
	for (i = 0; i < (int)m_aLayers.size(); ++i)
	{
		A3DRELEASE(m_aLayers[i]);
	}
	m_aLayers.clear();
	A3DRELEASE(m_pJointList);
	A3DRELEASE(m_pMultiCluGraph);

}

void CMoveMap::ReleaseMultiCluGraph()
{
	A3DRELEASE(m_pMultiCluGraph);
}

CJointList * CMoveMap::CreateJointList()
{
	if (!m_pJointList)
	{
		m_pJointList = new CJointList;
	}
	return GetJointList();
}

bool CMoveMap::Load(const char * szPath, vector<AString> * pLayerNames)
{
	Release();
	AFileImage   fileimg;
	if (!fileimg.Open(szPath, AFILE_BINARY|AFILE_OPENEXIST))
	{
		return false;
	}

	char drive[MAX_PATH];
	char dir[MAX_PATH];
	_splitpath(szPath, drive, dir, NULL, NULL);
	
	unsigned long readlen;
	DWORD flag;
	fileimg.Read(&flag, sizeof(DWORD), &readlen);
	if (flag != PFMAP_CFG_FILE_MAGIC)
	{
		return false;
	}
	fileimg.Read(&flag, sizeof(DWORD), &readlen);  //version
	if (flag != PFMAP_CFG_FILE_VERSION)
	{
		return false;
	}
	
	int count;
	fileimg.Read(&count, sizeof(int), &readlen);
	int i;
	char buf[1024];
	int  len;
	for (i = 0; i < count; ++i)
	{
		CLayerMap * pLayerMap = new CLayerMap;
		m_aLayers.push_back(pLayerMap);

		fileimg.Read(&len, sizeof(int), &readlen);
		fileimg.Read(buf, sizeof(char)*len, &readlen);
		buf[len] ='\0';

		if (pLayerNames)
		{
			pLayerNames->push_back(AString(buf));
		}

		fileimg.Read(&flag, sizeof(DWORD), &readlen);
		if (flag == 1)
		{
			AString path;
			path += drive;
			path += dir;
			path += buf;
			path += ".prmap";
			if (!pLayerMap->LoadRMap(path))
			{
				return false;
			}
		}
		
		fileimg.Read(&flag, sizeof(DWORD), &readlen);
		if (flag == 1)
		{
			AString path;
			path += drive;
			path += dir;
			path += buf;
			path += ".pdhmap";
			if (!pLayerMap->LoadDHMap(path))
			{
				return false;
			}
		}
		
		fileimg.Read(&flag, sizeof(DWORD), &readlen);
		if (flag == 1)
		{
			AString path;
			path += drive;
			path += dir;
			path += buf;
			path += ".isl";
			if (!pLayerMap->LoadIsl(path))
			{
				return false;
			}
		}
	}
	//mlu
	fileimg.Read(&len, sizeof(int), &readlen);
	fileimg.Read(buf, sizeof(char)*len, &readlen);
	buf[len] ='\0';
	AString path;
	path += drive;
	path += dir;
	path += buf;
	path += ".mlu";

	m_pMultiCluGraph = new CMultiCluGraph;

	if (!m_pMultiCluGraph->Load(path))
	{
		return false;
	}


	return true;
}

bool CMoveMap::Save(const char * szPath, const vector<AString>& layerNames, const AString& mluName)
{

	if (!m_pMultiCluGraph || m_aLayers.empty())
	{
		return false;
	}

	AFile   fileimg;
	if (!fileimg.Open(szPath, AFILE_BINARY|AFILE_CREATENEW))
	{
		return false;
	}
	DWORD  writelen;

	DWORD  flag = PFMAP_CFG_FILE_MAGIC;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);
	flag = PFMAP_CFG_FILE_VERSION;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);

	int count = (int) m_aLayers.size();
	fileimg.Write(&count, sizeof(int), &writelen);

	int i;
	char buf[1024];
	int  len;
	for (i = 0; i < count; ++i)
	{
		strcpy(buf, layerNames[i]);
		len = strlen(buf);
		fileimg.Write(&len, sizeof(int), &writelen);
		fileimg.Write(buf, sizeof(char)*len, &writelen);
		
		flag = (m_aLayers[i]->GetRMap() != NULL)? (1):(0);
		fileimg.Write(&flag, sizeof(DWORD), &writelen);

		flag = (m_aLayers[i]->GetDHMap() != NULL)? (1):(0);
		fileimg.Write(&flag, sizeof(DWORD), &writelen);
		
		flag = (m_aLayers[i]->GetIslandList() != NULL)? (1):(0);
		fileimg.Write(&flag, sizeof(DWORD), &writelen);
	}

	//mlu
	strcpy(buf, mluName);
	len = strlen(buf);
	fileimg.Write(&len, sizeof(int), &writelen);
	fileimg.Write(buf, sizeof(char)*len, &writelen);


	return true;
}


void CMoveMap::_SetMapCenterAsOrigin()
{
	m_vOrigin.Clear();
	m_vOrigin.x = -m_iMapWidth * m_fPixelSize * 0.5f;
	m_vOrigin.z = -m_iMapLength * m_fPixelSize * 0.5f;
}

CLayerMap * CMoveMap::CreateLayer()
{
	CLayerMap * pLayerMap = new CLayerMap;
	m_aLayers.push_back(pLayerMap);
	return pLayerMap;
}

void CMoveMap::CalcOrigin()
{
	CLayerMap * pLayerMap;
	if (!m_aLayers.empty())
	{
		pLayerMap = m_aLayers[0];
	}
	else
	{
		assert(0);
		return;
	}

	pLayerMap->GetRMap()->GetImageSize(m_iMapWidth, m_iMapLength);
	m_fPixelSize = pLayerMap->GetRMap()->GetPixelSize();
	_SetMapCenterAsOrigin();
}

void CMoveMap::SetOrigin(const A3DVECTOR3 &vOrigin)
{
	//	直接设置地图中心对应的三维位置，以支持部分地图块寻径的实现
	m_vOrigin.Clear();
	m_vOrigin.x = vOrigin.x;
	m_vOrigin.z = vOrigin.z;
}

APointF CMoveMap::TransMap2Wld(const A3DPOINT2& ptMap) const
{
	return TransMap2Wld(APointF(ptMap.x, ptMap.y));
}

APointF CMoveMap::TransMap2Wld(const APointF& ptMap) const
{
	float x, y;
	x  =m_vOrigin.x + (ptMap.x + 0.5f) * m_fPixelSize;
	y = m_vOrigin.z + (ptMap.y + 0.5f) * m_fPixelSize;

	return APointF(x, y);
}

A3DPOINT2 CMoveMap::TransWld2Map(const APointF& ptWld)const
{
	int x, y;
	x = (int)floor ((ptWld.x - m_vOrigin.x) / m_fPixelSize );
 	y = (int)floor ((ptWld.y - m_vOrigin.z) / m_fPixelSize );
 	return A3DPOINT2(x,y);
}

CMultiCluGraph * CMoveMap::CreateMultiCluGraph(int iPfAlg)
{
	A3DRELEASE(m_pMultiCluGraph);
	
	CCluGraphConnector cluConnector(iPfAlg);
	if (cluConnector.Connect(m_aLayers, m_pJointList))
	{
		m_pMultiCluGraph = new CMultiCluGraph;
		cluConnector.Export(m_pMultiCluGraph);
	}
	cluConnector.Release();
	return GetMultiCluGraph();
}

int CMoveMap::WhichLayer(const A3DPOINT2& ptMap, float dH, float *pLayerDist) const
{
	int i; 
	
	int which = -1;
	float min_layer_dh = 100000.0f;
	for (i = 0; i < (int)m_aLayers.size(); ++i)
	{
		CLayerMap* pLayer = m_aLayers[i];
		if (pLayer)
		{
			CBitImage* pRMap = pLayer->GetRMap();
			if (pRMap && pRMap->GetPixel(ptMap.x,ptMap.y))
			{
				float layer_dh = GetDH(i, ptMap);
				float diff_dh = (float)fabs(layer_dh - dH);

				if (diff_dh < SAME_LAYER_DH)
				{
					which = i;
					min_layer_dh = diff_dh;
					break;
				}

				if (diff_dh < min_layer_dh)
				{
					min_layer_dh = diff_dh;
					which = i;
				}
			}
		}
	}
	if (pLayerDist){
		*pLayerDist = min_layer_dh;
	}

	return which;
}

int CMoveMap::GetMinPassableLayer(const A3DPOINT2& ptMap) const 
{
	int i; 
	int which = 0;
	for (i = 0; i < (int)m_aLayers.size(); ++i)
	{
		CLayerMap* pLayer = m_aLayers[i];
		if (pLayer)
		{
			CBitImage* pRMap = pLayer->GetRMap();
			if (pRMap && pRMap->GetPixel(ptMap.x,ptMap.y))
			{
				which = i;
				break;
			}
		}
	}

	return which;
}

int CMoveMap::GetAllPassableLayers(const A3DPOINT2& ptMap,int* pLayers) const 
{
	int pos = 0;
	for (int i = 0; i < (int)m_aLayers.size(); ++i)
	{
		CLayerMap* pLayer = m_aLayers[i];
		if (pLayer)
		{
			CBitImage* pRMap = pLayer->GetRMap();
			if (pRMap && pRMap->GetPixel(ptMap.x,ptMap.y))
			{				
				if(pLayers!=NULL)
				{
					pLayers[pos] = i;
				}
				pos++;
			}
		}
	}
	return pos;
}

float CMoveMap::GetDH(int iLayer, const A3DPOINT2& ptMap) const
{
	if (iLayer<0 || iLayer>=m_aLayers.size()) 
	{
		ASSERT(0);
		return 0.0f;
	}
	CLayerMap* pLayerMap = m_aLayers[iLayer];
	if (!pLayerMap || !pLayerMap->GetDHMap()) 
	{
		ASSERT(0);
		return 0.0f;
	}
	FIX16  fixDh = pLayerMap->GetDHMap()->GetPixel(ptMap.x, ptMap.y);
	return AM_FIX16TOFLOAT(fixDh);
}

}	// end of the namespace


