/*
 * FILE: EL_Precinct.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/6/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "el_region.h"
#include <memory.h>

#ifdef _ELEMENTCLIENT
#include "AWScriptFile.h"
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define INVALID_LEN		999999.0f

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CELRegion
//	
///////////////////////////////////////////////////////////////////////////

CELRegion::CELRegion()
{
	m_iType		= -1;
	m_fLeft		= INVALID_LEN;
	m_fTop		= INVALID_LEN;
	m_fRight	= -INVALID_LEN;
	m_fBottom	= -INVALID_LEN;
}

CELRegion::~CELRegion()
{
}

#ifdef _ELEMENTCLIENT

bool CELRegion::Load(AWScriptFile* pFile, int iVersion)
{
	if (iVersion >= 2)
	{
		if (!pFile->MatchToken(L"[region]", true))
			return false;
	}

	//	Read name
	pFile->GetNextToken(true);
	m_strName = pFile->m_szToken;

	//	Read region type
	m_iType = pFile->GetNextTokenAsInt(true);
	//	Read point number
	int iNumPoint = pFile->GetNextTokenAsInt(false);

	int i;

	//	Read vertices ...
	for (i=0; i < iNumPoint; i++)
	{
		VECTOR3 v;

		//	Get position
		v.x = pFile->GetNextTokenAsFloat(true);
		v.y = pFile->GetNextTokenAsFloat(false);
		v.z = pFile->GetNextTokenAsFloat(false);
		
		m_aPoints.push_back(v);
	}

	//	Build bound box
	BuildBoundBox();

	return true;
}

#else	//	_ELEMENTCLIENT

//	Load data from file
bool CELRegion::Load(FILE* fp, int iVersion)
{
	int i, iNumPoint;
	float v[3];

	//	Read number of point
	fread(&iNumPoint, sizeof (int), 1, fp);

	for (i=0; i < iNumPoint; i++)
	{
		fread(v, sizeof (v), 1, fp);
		
		VECTOR3 v1 = {v[0], v[1], v[2]};
		m_aPoints.push_back(v1);
	}

	//	Build bound box
	BuildBoundBox();

	return true;
}

#endif	//	_ELEMENTCLIENT

//	Build precinct bound box
void CELRegion::BuildBoundBox()
{
	m_fLeft		= INVALID_LEN;
	m_fTop		= INVALID_LEN;
	m_fRight	= -INVALID_LEN;
	m_fBottom	= -INVALID_LEN;

	for (int i=0; i < (int)m_aPoints.size(); i++)
	{
		const VECTOR3& v = m_aPoints[i];

		if (v.x < m_fLeft)		m_fLeft = v.x;
		if (v.x > m_fRight)		m_fRight = v.x;
		if (v.z < m_fTop)		m_fTop = v.z;
		if (v.z > m_fBottom)	m_fBottom = v.z;
	}
}

//	Is specified position in this precinct ?
bool CELRegion::IsPointIn(float x, float z)
{
	//	Check bound box at first
	if (x < m_fLeft || x > m_fRight || z < m_fTop || z > m_fBottom)
		return false;

	int iNumCross = 0;// 点与多边形交点个数

	for (int i=0; i < (int)m_aPoints.size(); i++)
	{
		//	从检测点向右侧引出的射线是否与该边相交
		if (IsCrossLine(x, z, i))
			iNumCross++;
	}

	//	共有偶数个交点，则不在多边形内; 共有奇数个交点，在多边形内
	return (iNumCross & 1) ? true : false;
}

//	判断射线是否与边相交
bool CELRegion::IsCrossLine(float x, float z, int iIndex)
{
	int iNumLine = m_aPoints.size();
	const VECTOR3& v1 = m_aPoints[iIndex];
	const VECTOR3& v2 = m_aPoints[(iIndex+1) % iNumLine];

	if (v1.x < x && v2.x < x)	//	如果线段在射线左边
		return false;

	if (v1.z < z && v2.z < z)	//	如果线段在射线上边
		return false;

	if (v1.z > z && v2.z > z)	//	如果线段在射线下边
		return false;

	if (v1.z == v2.z)	//	如果线段与射线在同一水平线上
		return false;

	if (z == v1.z)	//	如果射线穿过线段顶点1
	{
		//	找到顶点 1 之前最近且与射线不在同一水平线上的点
		VECTOR3* pvPre = NULL;
		bool bFind = false;

		for (int i=1; i < iNumLine; i++)
		{
			pvPre = &m_aPoints[(iIndex + iNumLine - i) % iNumLine];
			if (pvPre->z != z)	// 不在扫描线上
			{
				bFind = true;
				break;
			}
		}

		if (!bFind)	//	没找到符合条件的点（多边形所有点在同一水平线上）
			return false;

		//	如果顶点1之前点与顶点2在射线两侧
		if ((pvPre->z < z && v2.z > z) || (pvPre->z > z && v2.z < z))
			return false;
	}

	//	计算射线与边的交点
	float fSlope = (v2.x - v1.x) / (v2.z - v1.z);
	float fCrossX = (z - v1.z) * fSlope + v1.x;
	
	//	交点在测试点左侧 return false
	return fCrossX <= x ? false : true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CELTransportBox
//	
///////////////////////////////////////////////////////////////////////////

CELTransportBox::CELTransportBox()
{
	m_idInst	= 0;
	m_idSrcInst	= 0;
	m_iLevelLmt	= 1;
	m_vTarget.x	= 0.0f;
	m_vTarget.y	= 0.0f;
	m_vTarget.z	= 0.0f;
	m_vPos.x	= 0.0f;
	m_vPos.y	= 0.0f;
	m_vPos.z	= 0.0f;
	m_vExts.x	= 0.0f;
	m_vExts.y	= 0.0f;
	m_vExts.z	= 0.0f;
	m_iIndex	= -1;
}

CELTransportBox::~CELTransportBox()
{
}

#ifdef _ELEMENTCLIENT

bool CELTransportBox::Load(AWScriptFile* pFile, int iVersion)
{
	if (iVersion >= 2)
	{
		if (!pFile->MatchToken(L"[trans]", true))
			return false;
	}

	m_idInst = pFile->GetNextTokenAsInt(true);

	if (iVersion >= 3)
		m_idSrcInst = pFile->GetNextTokenAsInt(false);
	else
		m_idSrcInst = 1;

	if (iVersion >= 5)
		m_iLevelLmt = pFile->GetNextTokenAsInt(false);
	else
		m_iLevelLmt = 1;

	m_vPos.x = pFile->GetNextTokenAsFloat(true);
	m_vPos.y = pFile->GetNextTokenAsFloat(false);
	m_vPos.z = pFile->GetNextTokenAsFloat(false);

	m_vExts.x = pFile->GetNextTokenAsFloat(true);
	m_vExts.y = pFile->GetNextTokenAsFloat(false);
	m_vExts.z = pFile->GetNextTokenAsFloat(false);

	m_vTarget.x = pFile->GetNextTokenAsFloat(true);
	m_vTarget.y = pFile->GetNextTokenAsFloat(false);
	m_vTarget.z = pFile->GetNextTokenAsFloat(false);

	return true;
}

#else	//	_ELEMENTCLIENT

//	Load data from file
bool CELTransportBox::Load(FILE* fp, int iVersion)
{
	float v[3];

	//	Read ID of instance
	fread(&m_idInst, sizeof (int), 1, fp);

	if (iVersion >= 3)
		fread(&m_idSrcInst, sizeof (int), 1, fp);
	else
		m_idSrcInst = 1;

	if (iVersion >= 5)
		fread(&m_iLevelLmt, sizeof (int), 1, fp);
	else
		m_iLevelLmt = 1;

	//	Area position
	fread(v, sizeof (v), 1, fp);
	m_vPos.x = v[0];
	m_vPos.y = v[1];
	m_vPos.z = v[2];

	//	Area extents
	fread(v, sizeof (v), 1, fp);
	m_vExts.x = v[0];
	m_vExts.y = v[1];
	m_vExts.z = v[2];

	//	Target position
	fread(v, sizeof (v), 1, fp);
	m_vTarget.x = v[0];
	m_vTarget.y = v[1];
	m_vTarget.z = v[2];

	return true;
}

#endif	//	_ELEMENTCLIENT

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CELRegionSet
//	
///////////////////////////////////////////////////////////////////////////

CELRegionSet::CELRegionSet()
{
	m_dwTimeStamp	= 0;
}

CELRegionSet::~CELRegionSet()
{
	int i;

	//	Release all precincts
	for (i=0; i < (int)m_aRegions.size(); i++)
		delete m_aRegions[i];
	
	for (i=0; i < (int)m_aTransBoxes.size(); i++)
		delete m_aTransBoxes[i];
}

//	Get the precinct specified position is in
CELRegion* CELRegionSet::IsPointInRegion(float x, float z)
{
	for (int i=0; i < (int)m_aRegions.size(); i++)
	{
		CELRegion* p = m_aRegions[i];
		if (p->IsPointIn(x, z))
			return p;
	}

	return NULL;
}

//	Get the PK region specified position is in
CELRegion* CELRegionSet::IsPointInPKRegion(float x, float z)
{
	for (int i=0; i < (int)m_aPKRegions.size(); i++)
	{
		CELRegion* p = m_aPKRegions[i];
		if (p->IsPointIn(x, z))
			return p;
	}

	return NULL;
}

//	Get the peace region specified position is in
CELRegion* CELRegionSet::IsPointInSanctuary(float x, float z)
{
	for (int i=0; i < (int)m_aPeaceRegions.size(); i++)
	{
		CELRegion* p = m_aPeaceRegions[i];
		if (p->IsPointIn(x, z))
			return p;
	}

	return NULL;
}

//	Get the transport box specified position is in
CELTransportBox* CELRegionSet::IsPointInTransport(float x, float y, float z, int idSrcInst)
{
	for (int i=0; i < (int)m_aTransBoxes.size(); i++)
	{
		CELTransportBox* p = m_aTransBoxes[i];
		if (p->GetSrcInstanceID() == idSrcInst && p->IsPointIn(x, y, z))
			return p;
	}

	return NULL;
}

#ifdef _ELEMENTCLIENT

//	Load data from file
bool CELRegionSet::Load(const char* szFileName)
{
	AWScriptFile File;
	if (!File.Open(szFileName))
		return false;

	//	Get version
	if (!File.MatchToken(L"version", false))
		return false;

	int iVersion = File.GetNextTokenAsInt(false);
	if (iVersion > ELRGNFILE_VERSION)
		return false;

	if (iVersion >= 4)
		m_dwTimeStamp = (unsigned int)File.GetNextTokenAsInt(true);
	else
		m_dwTimeStamp = 0;

	while (File.PeekNextToken(true))
	{
		if (iVersion == 1 || !wcsicmp(File.m_szToken, L"[region]"))
		{
			CELRegion* pRegion = new CELRegion;
			if (!pRegion->Load(&File, iVersion))
				return false;

			m_aRegions.push_back(pRegion);
			
			if (pRegion->GetType() == CELRegion::REGION_PK)
				m_aPKRegions.push_back(pRegion);
			else if (pRegion->GetType() == CELRegion::REGION_SANCTUARY)
				m_aPeaceRegions.push_back(pRegion);
		}
		else if (!wcsicmp(File.m_szToken, L"[trans]"))
		{
			CELTransportBox* pTrans = new CELTransportBox;
			if (!pTrans->Load(&File, iVersion))
				return false;

			pTrans->SetIndex((int)m_aTransBoxes.size());
			m_aTransBoxes.push_back(pTrans);
		}
		else
			return false;
	}

	File.Close();
	return true;
}

#else	//	_ELEMENTCLIENT

//	Load data from file
bool CELRegionSet::Load(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "rb");
	if (!fp)
		return false;
	
	//	Read version at first
	unsigned int dwVersion;
	fread(&dwVersion, 1, sizeof (unsigned int), fp);

	if (dwVersion > ELRGNFILE_VERSION)
		return false;

	//	Reset file pointer
	fseek(fp, 0, SEEK_SET);

	//	Read file header
	REGIONFILEHEADER4 Header;
	if (dwVersion == 1)
	{
		REGIONFILEHEADER1 Temp;
		fread(&Temp, 1, sizeof (Temp), fp);

		memcpy(&Header, &Temp, sizeof (Temp));

		Header.iNumTrans	= 0;
		Header.dwTimeStamp	= 0;
	}
	else if (dwVersion < 4)
	{
		REGIONFILEHEADER2 Temp;
		fread(&Temp, 1, sizeof (Temp), fp);

		memcpy(&Header, &Temp, sizeof (Temp));

		Header.dwTimeStamp	= 0;
	}
	else	//	dwVersion >= 4 
	{
		fread(&Header, 1, sizeof (Header), fp);
	}
	
	m_dwTimeStamp	= Header.dwTimeStamp;

	int iRegionCnt=0, iTransCnt=0, iDataType;

	//	Read data
	while (iRegionCnt < Header.iNumRegion || iTransCnt < Header.iNumTrans)
	{
		fread(&iDataType, sizeof (int), 1, fp);

		if (!iDataType)
		{
			CELRegion* pRegion = new CELRegion;
			if (!pRegion->Load(fp, (int)dwVersion))
				return false;

			m_aRegions.push_back(pRegion);

		/*	if (pRegion->GetType() == CELRegion::REGION_PK)
				m_aPKRegions.push_back(pRegion);
			else if (pRegion->GetType() == CELRegion::REGION_SANCTUARY)
				m_aPeaceRegions.push_back(pRegion);
		*/
			iRegionCnt++;
		}
		else if (iDataType == 1)
		{
			CELTransportBox* pTrans = new CELTransportBox;
			if (!pTrans->Load(fp, (int)dwVersion))
				return false;

			pTrans->SetIndex((int)m_aTransBoxes.size());
			m_aTransBoxes.push_back(pTrans);

			iTransCnt++;
		}
		else
		{
		//	ASSERT(0);
			return false;
		}
	}

	fclose(fp);
	return true;
}

#endif	//	_ELEMENTCLIENT
