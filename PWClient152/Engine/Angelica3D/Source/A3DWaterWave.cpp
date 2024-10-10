/*
* FILE: A3DWaterWave.cpp
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi, 2011, 04, 12
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/

#include "A3DVector.h"
#include "A3DTypes.h"
#include "APerlinNoise1D.h"
#include "A3DFuncs.h"
//#include "A3DEnvironment.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "AFile.h"
#include "A3DPI.h"

#include "A3DWaterWave.h"
#include "A3DWaterArea.h"
#include "A3DTerrainWater.h"

A3DWaterWave::A3DWaterWave()
{
	m_pArea			= NULL;

	m_pVerts		= NULL;
	m_pIndices		= NULL;

	m_nMaxSegments	= 0;
	m_nNumSegments	= 0;
	m_pSegmentPos	= NULL;
	m_pDayColors	= NULL;
	m_pNightColors	= NULL;
	m_fDNFactor		= -1.0f;

	m_vEdgeSize		= 1.0f;
}

A3DWaterWave::~A3DWaterWave()
{
	Release();
}

bool A3DWaterWave::Init(A3DWaterArea * pArea, float vEdgeSize, int nMaxSegments)
{
	m_pArea		= pArea;
	m_vEdgeSize = vEdgeSize;

	m_nMaxSegments = nMaxSegments;
	m_pSegmentPos = new A3DVECTOR3[m_nMaxSegments];
	m_pDayColors = new A3DCOLOR[m_nMaxSegments * 3];
	m_pNightColors = new A3DCOLOR[m_nMaxSegments * 3];
	memset(m_pDayColors, 0x80, sizeof(A3DCOLOR) * m_nMaxSegments * 3);
	memset(m_pNightColors, 0x80, sizeof(A3DCOLOR) * m_nMaxSegments * 3);
	m_pVerts = new A3DWAVEVERTEX[m_nMaxSegments * 3];
	m_pIndices = new WORD[(m_nMaxSegments - 1) * 12];

	m_nNumSegments = 0;
	return true;
}

bool A3DWaterWave::Release()
{
	if( m_pDayColors )
	{
		delete [] m_pDayColors;
		m_pDayColors = NULL;
	}

	if( m_pNightColors )
	{
		delete [] m_pNightColors;
		m_pNightColors = NULL;
	}

	if( m_pSegmentPos )
	{
		delete [] m_pSegmentPos;
		m_pSegmentPos = NULL;
	}

	if( m_pVerts )
	{
		delete [] m_pVerts;
		m_pVerts = NULL;
	}

	if( m_pIndices )
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
	}

	return true;
}

bool A3DWaterWave::AddSegment(const A3DVECTOR3& vecPos)
{
	if( m_nNumSegments == m_nMaxSegments )
		return false;

	m_pSegmentPos[m_nNumSegments] = vecPos;
	m_nNumSegments ++;

	return true;
}

bool A3DWaterWave::CopyToBuffer(A3DWAVEVERTEX * pVerts, WORD * pIndices, int nStartVert)
{
	float fDNFactor = m_pArea->GetTerrainWater()->GetDNFactor();
	float delta = (float)fabs(m_fDNFactor - fDNFactor);

	if( delta > 1.0f / 256.0f )
	{
		m_fDNFactor = fDNFactor;
		UpdateColors();
	}

	// first copy vertex into dest buffer
	memcpy(pVerts, m_pVerts, sizeof(A3DWAVEVERTEX) * m_nVertCount);

	// then set correct index information
	int index;
	for(index=0; index<m_nFaceCount * 3; index++)
	{
		pIndices[index] = nStartVert + m_pIndices[index];
	}

	return true;
}

bool A3DWaterWave::UpdateColors()
{
	int d, n;
	n = (int)(m_fDNFactor * 255.0f);
	d = 255 - n;

	for(int i=0; i<m_nVertCount; i++)
	{
		A3DCOLOR c0, c1, c2, c3;
		c0 = m_pDayColors[i] & 0x00ff00ff;
		c1 = m_pDayColors[i] & 0x0000ff00;
		c2 = m_pNightColors[i] & 0x00ff00ff;
		c3 = m_pNightColors[i] & 0x0000ff00;

		c0 = ((c0 * d + c2 * n) >> 8) & 0x00ff00ff;
		c1 = ((c1 * d + c3 * n) >> 8) & 0x0000ff00;
		m_pVerts[i].diffuse = (m_pVerts[i].diffuse & 0xff000000) | c0 | c1;
	}

	return true;
}

bool A3DWaterWave::UpdateVB()
{
	m_nVertCount = m_nNumSegments * 3;
	m_nFaceCount = (m_nNumSegments - 1) * 4;

	A3DVECTOR3 vecOut, vecOutLast;
	A3DVECTOR3 vecOutDir;
	A3DVECTOR3 vecVertOut;
	float vTotalLength = 0.0f;

	APerlinNoise1D noise;
	noise.Init(64, 1.0f, 23, 0.995f, 5, 0x1794);
	float			dv;

	// first we determine the last segment's outside vector
	A3DVECTOR3 vecEdge;
#define EDGE_LEN	15.0f

	A3DVECTOR3 vecDeltaY;
	for(int i=0; i<m_nNumSegments; i++)
	{
		if( i != m_nNumSegments - 1 )
		{
			vecEdge = m_pSegmentPos[i + 1] - m_pSegmentPos[i];
			vecOut = a3d_Normalize(CrossProduct(A3DVECTOR3(0.0f, 1.0f, 0.0f), vecEdge));
		}
		if( i == 0 )
			vecOutDir = a3d_Normalize(vecOut);
		else if( i == m_nNumSegments - 1 )
			vecOutDir = a3d_Normalize(vecOutLast);
		else
			vecOutDir = a3d_Normalize(vecOut + vecOutLast);

		noise.GetValue((float)i, &dv, 1);
		dv = (dv - 0.5f) * 0.1f;

		float vEdgeSize = m_vEdgeSize;
		if( vEdgeSize > 4.0f )
			vEdgeSize = 4.0f;

		vecDeltaY = A3DVECTOR3(0.0f, (rand() % 11) / 100.0f, 0.0f);
		vecVertOut = vecOutDir * (float)(vEdgeSize / fabs(sin(acos(DotProduct(vecOutDir, Normalize(vecEdge))))));
		m_pVerts[i * 3] = A3DWAVEVERTEX(m_pSegmentPos[i] + vecDeltaY, 0x00ffffff, vTotalLength / EDGE_LEN, 1.0f + dv);
		m_pVerts[i * 3 + 1] = A3DWAVEVERTEX(m_pSegmentPos[i]- vecVertOut * 0.35f, 0xffffffff, vTotalLength / EDGE_LEN, 0.75f + dv);
		m_pVerts[i * 3 + 2] = A3DWAVEVERTEX(m_pSegmentPos[i] - vecVertOut * 1.0f - vecDeltaY, 0x00ffffff, vTotalLength / EDGE_LEN, 0.5f + dv);

		if( i != m_nNumSegments - 1 )
		{
			m_pIndices[i * 12 +  0] = i * 3;
			m_pIndices[i * 12 +  1] = i * 3 + 1;
			m_pIndices[i * 12 +  2] = i * 3 + 3;

			m_pIndices[i * 12 +  3] = m_pIndices[i * 12 +  2];
			m_pIndices[i * 12 +  4] = m_pIndices[i * 12 +  1];
			m_pIndices[i * 12 +  5] = i * 3 + 4;

			m_pIndices[i * 12 +  6] = m_pIndices[i * 12 +  5];
			m_pIndices[i * 12 +  7] = m_pIndices[i * 12 +  4];
			m_pIndices[i * 12 +  8] = i * 3 + 2;

			m_pIndices[i * 12 +  9] = m_pIndices[i * 12 +  6];
			m_pIndices[i * 12 + 10] = m_pIndices[i * 12 +  8];
			m_pIndices[i * 12 + 11] = i * 3 + 5;
		}

		vecOutLast = vecOut;
		vTotalLength += Magnitude(vecEdge);
	}

	return true;
}

bool A3DWaterWave::SetDeltaPos(const A3DVECTOR3& vecDelta)
{
	for(int i=0; i<m_nNumSegments; i++)
	{
		m_pSegmentPos[i] = m_pSegmentPos[i] + vecDelta;
	}

	return UpdateVB();
}

bool A3DWaterWave::Load(A3DWaterArea * pArea, AFile * pFileToLoad)
{
	DWORD dwReadLen;
	DWORD dwVersion;

	// first write current version of the area
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10003000 )
	{
		// then read basic information of the area
		// then write basic information of the area
		if( !pFileToLoad->Read(&m_nMaxSegments, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vEdgeSize, sizeof(float), &dwReadLen) )
			goto READFAIL;

		if( !Init(pArea, m_vEdgeSize, m_nMaxSegments) )
		{
			g_A3DErrLog.Log("A3DWaterWave::Load(), failed to init!");
			return false;
		}

		if( !pFileToLoad->Read(&m_nNumSegments, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pSegmentPos, sizeof(A3DVECTOR3) * m_nNumSegments, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nNumSegments * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nNumSegments * 3, &dwReadLen) )
			goto READFAIL;

		// now update all necessary data for rendering
		UpdateVB();

		m_fDNFactor = -1.0f;
	}
	else if( dwVersion == 0x10002000 )
	{
		// then read basic information of the area
		// then write basic information of the area
		if( !pFileToLoad->Read(&m_nMaxSegments, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vEdgeSize, sizeof(float), &dwReadLen) )
			goto READFAIL;

		if( !Init(pArea, m_vEdgeSize, m_nMaxSegments) )
		{
			g_A3DErrLog.Log("A3DWaterWave::Load(), failed to init!");
			return false;
		}

		if( !pFileToLoad->Read(&m_nNumSegments, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pSegmentPos, sizeof(A3DVECTOR3) * m_nNumSegments, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nNumSegments * 3, &dwReadLen) )
			goto READFAIL;

		// now update all necessary data for rendering
		UpdateVB();

		for(int i=0; i<m_nNumSegments * 3; i++)
		{
			m_pNightColors[i] = a3d_ColorValueToColorRGBA(a3d_ColorRGBAToColorValue(m_pDayColors[i]) * 0.3f);
		}

		m_fDNFactor = -1.0f;
	}
	else if( dwVersion == 0x10001000 )
	{
		// then read basic information of the area
		// then write basic information of the area
		if( !pFileToLoad->Read(&m_nMaxSegments, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vEdgeSize, sizeof(float), &dwReadLen) )
			goto READFAIL;

		if( !Init(pArea, m_vEdgeSize, m_nMaxSegments) )
		{
			g_A3DErrLog.Log("A3DWaterWave::Load(), failed to init!");
			return false;
		}

		if( !pFileToLoad->Read(&m_nNumSegments, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pSegmentPos, sizeof(A3DVECTOR3) * m_nNumSegments, &dwReadLen) )
			goto READFAIL;

		// now update all necessary data for rendering
		UpdateVB();
	}
	else
	{
		g_A3DErrLog.Log("A3DWaterWave::Load(), version [%x] is not supported now!", dwVersion);
		return false;
	}

	return true;

READFAIL:
	g_A3DErrLog.Log("A3DWaterWave::Load(), read from the file failed!");
	return false;
}

bool A3DWaterWave::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;
	DWORD dwVersion = WAVE_VER;

	// first write current version of the area
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then write basic information of the area
	if( !pFileToSave->Write(&m_nMaxSegments, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vEdgeSize, sizeof(float), &dwWriteLen) )
		goto WRITEFAIL;

	if( !pFileToSave->Write(&m_nNumSegments, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pSegmentPos, sizeof(A3DVECTOR3) * m_nNumSegments, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pDayColors, sizeof(A3DCOLOR) * m_nNumSegments * 3, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pNightColors, sizeof(A3DCOLOR) * m_nNumSegments * 3, &dwWriteLen) )
		goto WRITEFAIL;

	return true;

WRITEFAIL:
	g_A3DErrLog.Log("A3DWaterWave::Save(), Write to the file failed!");
	return false;
}


A3DWaveMoveCtrl::A3DWaveMoveCtrl()
{
	m_vTimeNow = 0.0f;

	memset(m_vTimeStart, 0, sizeof(float) * 4);
	memset(m_vTimeEnd, 0, sizeof(float) * 4);
	memset(m_vValue, 0, sizeof(float) * 4);
	memset(m_vA, 0, sizeof(float) * 4);
	memset(m_vB, 0, sizeof(float) * 4);
	memset(m_vC, 0, sizeof(float) * 4);

	m_nCurSegment = 0;
}

bool A3DWaveMoveCtrl::SetCtrlValues(float t1, float t2, float t3, float t4, float v1, float v2, float v3, float v4, float v5)
{
	m_vTimeStart[0] = 0;
	m_vTimeStart[1] = t1;
	m_vTimeStart[2] = t1 + t2;
	m_vTimeStart[3] = t1 + t2 + t3;

	m_vTimeEnd[0] = m_vTimeStart[0] + t1;
	m_vTimeEnd[1] = m_vTimeStart[1] + t2;
	m_vTimeEnd[2] = m_vTimeStart[2] + t3;
	m_vTimeEnd[3] = m_vTimeStart[3] + t4;

	// start value of each segment
	m_vValue[0] = v1;
	m_vValue[1] = v3;
	m_vValue[2] = v5;
	m_vValue[3] = v3;

	float x0, x1, x2, x3;

	// now caluclate these 4 segment's coeffients
	// segment 1
	x0 = v1;
	x1 = v1;
	x2 = v2;
	x3 = v3;
	m_vC[0] = 3 * (x1 - x0);
	m_vB[0] = 3 * (x2 - x1) - m_vC[0];
	m_vA[0] = x3 - x0 - m_vC[0] - m_vB[0];

	// segment 2
	x0 = v3;
	x1 = v4;
	x2 = v5;
	x3 = v5;
	m_vC[1] = 3 * (x1 - x0);
	m_vB[1] = 3 * (x2 - x1) - m_vC[1];
	m_vA[1] = x3 - x0 - m_vC[1] - m_vB[1];

	// segment 3
	x0 = v5;
	x1 = v5;
	x2 = v4;
	x3 = v3;
	m_vC[2] = 3 * (x1 - x0);
	m_vB[2] = 3 * (x2 - x1) - m_vC[2];
	m_vA[2] = x3 - x0 - m_vC[2] - m_vB[2];

	// segment 4
	x0 = v3;
	x1 = v2;
	x2 = v1;
	x3 = v1;
	m_vC[3] = 3 * (x1 - x0);
	m_vB[3] = 3 * (x2 - x1) - m_vC[3];
	m_vA[3] = x3 - x0 - m_vC[3] - m_vB[3];

	return true;
}

float A3DWaveMoveCtrl::UpdateValue(float vDeltaTime)
{
	m_vTimeNow += vDeltaTime;

CHECKTIME:
	if( m_vTimeNow > m_vTimeEnd[m_nCurSegment] )
	{
		m_nCurSegment ++;
		if( m_nCurSegment == 4 )
		{
			m_nCurSegment = 0;
			// clear some time
			m_vTimeNow -= m_vTimeEnd[3];
		}

		goto CHECKTIME;
	}

	float t = (m_vTimeNow - m_vTimeStart[m_nCurSegment]) / (m_vTimeEnd[m_nCurSegment] - m_vTimeStart[m_nCurSegment]);
	return m_vA[m_nCurSegment] * t * t * t + m_vB[m_nCurSegment] * t * t + m_vC[m_nCurSegment] * t + m_vValue[m_nCurSegment];
}