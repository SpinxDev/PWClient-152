/*
 * FILE: EL_Precinct.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "el_precinct.h"
#include <memory.h>

#ifdef _ELEMENTCLIENT
#include "AWScriptFile.h"
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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
//	Implement CELPrecinct
//	
///////////////////////////////////////////////////////////////////////////

CELPrecinct::CELPrecinct()
{
	m_iPriority	= 0;
	m_idDstInst	= 0;
	m_idSrcInst	= 0;
	m_idDomain	= 0;
	m_fLeft		= 999999.0f;
	m_fTop		= 999999.0f;
	m_fRight	= -999999.0f;
	m_fBottom	= -999999.0f;
	
	m_vCityPos.x = 0.0f;
	m_vCityPos.y = 0.0f;
	m_vCityPos.z = 0.0f;
	
#ifdef _ELEMENTCLIENT

	m_dwID			= 0;
	m_iMusicInter	= 0;
	m_iMusicLoop	= 0;
	m_bNightSFX		= false;

#endif
}

CELPrecinct::~CELPrecinct()
{
#ifdef _ELEMENTCLIENT

	int i;

	for (i=0; i < m_aMarks.GetSize(); i++)
		delete m_aMarks[i];

	for (i=0; i < m_aMusicFiles.GetSize(); i++)
		delete m_aMusicFiles[i];

#endif
}

#ifdef _ELEMENTCLIENT

bool CELPrecinct::Load(AWScriptFile* pFile, int iVersion)
{
	//	Read name
	pFile->GetNextToken(true);
	m_strName = pFile->m_szToken;

	int iNumPoint;

	if (iVersion < 2)
	{
		m_dwID = 0;

		//	Read point number
		iNumPoint = pFile->GetNextTokenAsInt(true);
	}
	else	//	dwVersion >= 2
	{
		//	Read precinct ID
		m_dwID = (unsigned int)pFile->GetNextTokenAsInt(true);
		//	Read point number
		iNumPoint = pFile->GetNextTokenAsInt(false);
	}

	//	Read mark number
	int iNumMark = pFile->GetNextTokenAsInt(false);
	//	Read priority
	m_iPriority = pFile->GetNextTokenAsInt(false);
	//	Read destination instance ID
	m_idDstInst = pFile->GetNextTokenAsInt(false);
	//	Read music file number
	int iNumMusic = pFile->GetNextTokenAsInt(false);
	//	Read music interval
	m_iMusicInter = pFile->GetNextTokenAsInt(false);
	//	Read music loop type
	m_iMusicLoop = pFile->GetNextTokenAsInt(false);

	//	Source instance ID
	if (iVersion >= 4)
		m_idSrcInst = pFile->GetNextTokenAsInt(false);
	else
		m_idSrcInst = 1;

	//	ID of domain
	if (iVersion >= 6)
		m_idDomain = pFile->GetNextTokenAsInt(false);
	else
		m_idDomain = 0;

	//	pk protect
	if (iVersion >= 7)
		m_bPKProtect = pFile->GetNextTokenAsInt(false) > 0 ? true : false;
	else
		m_bPKProtect = false;

	//	Read city position
	m_vCityPos.x = pFile->GetNextTokenAsFloat(true);
	m_vCityPos.y = pFile->GetNextTokenAsFloat(false);
	m_vCityPos.z = pFile->GetNextTokenAsFloat(false);

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

	//	Read mark ...
	for (i=0; i < iNumMark; i++)
	{
		MARK* pMark = new MARK;

		pFile->GetNextToken(true);
		pMark->strName = pFile->m_szToken;

		pMark->vPos.x = pFile->GetNextTokenAsFloat(false);
		pMark->vPos.y = pFile->GetNextTokenAsFloat(false);
		pMark->vPos.z = pFile->GetNextTokenAsFloat(false);

		m_aMarks.Add(pMark);
	}

	//	Read sound file
	pFile->GetNextToken(true);
	m_strSound = AString("music\\") + WC2AS(pFile->m_szToken);

	//	Read music files ...
	for (i=0; i < iNumMusic; i++)
	{
		if (!pFile->GetNextToken(true))
		{
			ASSERT(0);
			return false;
		}

		AString* pstr = new AString;
		*pstr = WC2AS(pFile->m_szToken);
		m_aMusicFiles.Add(pstr);
	}

	//	Read sound file at night
	if (iVersion >= 3)
	{
		pFile->GetNextToken(true);
		m_strSound_n = AString("music\\") + WC2AS(pFile->m_szToken);
	}

	//	Build bound box
	BuildBoundBox();

	return true;
}

#else	//	_ELEMENTCLIENT

//	Load data from file
bool CELPrecinct::Load(FILE* fp, int iVersion)
{
	int i, iNumPoint;
	float v[3];

	//	Read number of point
	fread(&iNumPoint, sizeof (int), 1, fp);
	//	Read priority
	fread(&m_iPriority, sizeof (int), 1, fp);
	//	Read destination instance ID
	fread(&m_idDstInst, sizeof (int), 1, fp);

	//	Source instance ID
	if (iVersion >= 4)
		fread(&m_idSrcInst, sizeof (int), 1, fp);
	else
		m_idSrcInst = 1;

	//	Domain ID
	if (iVersion >= 6)
		fread(&m_idDomain, sizeof (int), 1, fp);
	else
		m_idDomain = 0;

	if (iVersion >= 7)
		fread(&m_bPKProtect, sizeof (bool), 1, fp);
	else
		m_bPKProtect = false;

	//	Read city position
	fread(v, sizeof (v), 1, fp);
	m_vCityPos.x = v[0];
	m_vCityPos.y = v[1];
	m_vCityPos.z = v[2];

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
void CELPrecinct::BuildBoundBox()
{
	m_fLeft		= 999999.0f;
	m_fTop		= 999999.0f;
	m_fRight	= -999999.0f;
	m_fBottom	= -999999.0f;

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
bool CELPrecinct::IsPointIn(float x, float z)
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
bool CELPrecinct::IsCrossLine(float x, float z, int iIndex)
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
//	Implement CELPrecinctSet
//	
///////////////////////////////////////////////////////////////////////////

CELPrecinctSet::CELPrecinctSet()
{
	m_dwTimeStamp	= 0;
}

CELPrecinctSet::~CELPrecinctSet()
{
	//	Release all precincts
	for (int i=0; i < (int)m_aPrecincts.size(); i++)
		delete m_aPrecincts[i];
}

//	Get the precinct specified position is in
CELPrecinct* CELPrecinctSet::IsPointIn(float x, float z, int idSrcInst)
{
	CELPrecinct* pPrecinct = NULL;

	for (int i=0; i < (int)m_aPrecincts.size(); i++)
	{
		CELPrecinct* p = m_aPrecincts[i];
		if (p->GetSrcInstanceID() != idSrcInst || !p->IsPointIn(x, z))
			continue;

		if (!pPrecinct || p->GetPriority() <= pPrecinct->GetPriority())
			pPrecinct = p;
	}

	return pPrecinct;
}

#ifdef _ELEMENTCLIENT

//	Load data from file
bool CELPrecinctSet::Load(const char* szFileName)
{
	AWScriptFile File;
	if (!File.Open(szFileName))
		return false;

	//	Get version
	if (!File.MatchToken(L"version", false))
		return false;

	int iVersion = File.GetNextTokenAsInt(false);
	if (iVersion > ELPCTFILE_VERSION)
		return false;

	if (iVersion >= 5)
		m_dwTimeStamp = (unsigned int)File.GetNextTokenAsInt(true);
	else
		m_dwTimeStamp = 0;

	while (File.PeekNextToken(true))
	{
		CELPrecinct* pPrecinct = new CELPrecinct;
		if (!pPrecinct->Load(&File, iVersion))
			return false;

		m_aPrecincts.push_back(pPrecinct);
	}

	File.Close();
	return true;
}

#else	//	_ELEMENTCLIENT

//	Load data from file
bool CELPrecinctSet::Load(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "rb");
	if (!fp)
		return false;
	
	//	Read version at first
	unsigned int dwVersion;
	fread(&dwVersion, 1, sizeof (unsigned int), fp);

	if (dwVersion > ELPCTFILE_VERSION)
		return false;

	//	Reset file pointer
	fseek(fp, 0, SEEK_SET);

	//	Read file header
	PRECINCTFILEHEADER5 Header;
	if (dwVersion < 5)
	{
		PRECINCTFILEHEADER Temp;
		fread(&Temp, 1, sizeof (Temp), fp);

		memcpy(&Header, &Temp, sizeof (Temp));

		Header.dwTimeStamp	= 0;
	}
	else	//	dwVersion >= 5
	{
		fread(&Header, 1, sizeof (Header), fp);
	}

	m_dwTimeStamp = Header.dwTimeStamp;

	for (int i=0; i < Header.iNumPrecinct; i++)
	{
		CELPrecinct* pPrecinct = new CELPrecinct;
		if (!pPrecinct->Load(fp, (int)dwVersion))
			return false;

		m_aPrecincts.push_back(pPrecinct);
	}

	fclose(fp);
	return true;
}

#endif	//	_ELEMENTCLIENT
