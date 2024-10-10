/*
* FILE: A3DWaterWave2.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi, 2012, 3, 5
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3D_WATER_WAVE_2_H_
#define _A3D_WATER_WAVE_2_H_

#include "ABaseDef.h"

class A3DWaterArea2;
class A3DVECTOR3;
class AFile;
class A3DStream;

//define WaveVertex
struct A3DWAVEVERTEX2
{
	A3DWAVEVERTEX2() {}
	A3DWAVEVERTEX2(const A3DVECTOR3& _pos, A3DCOLOR _diffuse1, A3DCOLOR _diffuse2, float _tu, float _tv, float _tw)
	{
		pos = _pos;
		diffuse1 = _diffuse1;
		diffuse2 = _diffuse2;
		u = _tu;
		v = _tv;
		w = _tw;
	}

	A3DVECTOR3	pos;			//	Position
	float		u, v;			//	Texture coordinates
	float		w;				//  alpha for wave
	A3DCOLOR	diffuse1;		//	Diffuse
	A3DCOLOR	diffuse2;		//	Diffuse
};

// class that represent a whole enclosed wave chain
class A3DWaterWave2
{
	friend class A3DWaterArea2;

private:
	A3DWaterArea2 *	m_pArea;				// water area object this wave resides.

	int				m_nMaxSegments;			// max number of segments
	int				m_nNumSegments;			// number of segments
	A3DVECTOR3 *	m_pSegmentPos;			// position of each segment
	A3DCOLOR *		m_pDayColors;			// colors of each vertex in day
	A3DCOLOR *		m_pNightColors;			// colors of each vertex at night

	float			m_fDNFactor;			// factor of day night in m_pVerts;

	int				m_nVertCount;
	int				m_nFaceCount;
	float			m_vEdgeSize;

	A3DWAVEVERTEX2 *	m_pVerts;
	WORD *			m_pIndices;
	A3DStream*		m_pStream;

protected:
	bool UpdateVB();

	// shift these water waves to a new position, this will
	// be called when A3DWaterArea::ShiftTo() is called
	bool SetDeltaPos(const A3DVECTOR3& vecDelta);

public:
	A3DWaterWave2();
	~A3DWaterWave2();

	bool Init(A3DWaterArea2 * pArea, float vEdgeSize, int nMaxSegments);
	bool Release();

	bool Load(A3DWaterArea2 * pArea, AFile * pFileToLoad);
	bool Save(AFile * pFileToSave);

	bool AddSegment(const A3DVECTOR3& vecPos);
	//FIXME!!
	//bool CopyToBuffer(A3DWAVEVERTEX * pVerts, WORD * pIndices, int nStartVert);
	//bool UpdateColors();

	inline int GetVertCount()				{ return m_nVertCount; }
	inline int GetFaceCount()				{ return m_nFaceCount; }
	inline A3DWAVEVERTEX2 * GetVerts()		{ return m_pVerts; }
	inline A3DCOLOR * GetDayColors()		{ return m_pDayColors; }
	inline A3DCOLOR * GetNightColors()		{ return m_pNightColors; }

	inline A3DWaterArea2 * GetWaterArea()	{ return m_pArea; }
	inline A3DStream*	GetStream();
};

inline A3DStream* A3DWaterWave2::GetStream()
{
	return m_pStream;
}

// we use four looped back bezier control point to control the movement of the wave
// in fact it's 3 different point
// but two of them has zero length control point
//
//        5---4======3======2---1
//
class A3DWaveMoveCtrl2
{
private:
	float			m_vTimeNow;			// current time

	int				m_nCurSegment;		// current moving segment id
	float			m_vTimeStart[4];	// start time of each segment
	float			m_vTimeEnd[4];		// end time of each segment
	float			m_vValue[4];		// value of each point

	float			m_vA[4];			// bezier coefficients
	float			m_vB[4];
	float			m_vC[4];

protected:
public:
	A3DWaveMoveCtrl2();
	~A3DWaveMoveCtrl2() {}

	bool SetCtrlValues(float t1, float t2, float t3, float t4, float v1, float v2, float v3, float v4, float v5);
	float UpdateValue(float vDeltaTime);
};

#endif //_A3D_WATER_WAVE_2_H_