/*
* FILE: TrailList.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/23
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _TrailList_H_
#define _TrailList_H_

#include "RotList.h"
#include <vector.h>
#include <A3DVector.h>
#include <A3DTypes.h>
#include <A3DHermiteSpline.h>
extern float g_fTempTrailMag;
extern float g_fTempTrailLerp;

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DHermiteSpline;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Struct TRAIL_DATA
//	
///////////////////////////////////////////////////////////////////////////

struct TRAIL_DATA
{
	A3DVECTOR3	m_vSeg1;
	A3DVECTOR3	m_vSeg2;
	int			m_nSegLife;
	A3DCOLOR	m_Color;

	TRAIL_DATA() 
	{
		m_nSegLife = 0;
	}
	TRAIL_DATA& operator = (const TRAIL_DATA& src);
	TRAIL_DATA(const A3DVECTOR3& v1, const A3DVECTOR3& v2, int nSegLife, A3DCOLOR cl)
		: m_vSeg1(v1), m_vSeg2(v2), m_nSegLife(nSegLife), m_Color(cl) {}
};

struct GRAVITY_DATA
{
	float	m_fGravity;
	float	m_fVerticalNoise;
	float	m_fVSpeed;
	float	m_fAmplitude;
	float	m_fHSpeed;
	bool	m_bXNoise;
	bool	m_bZNoise;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class TrailList
//	
///////////////////////////////////////////////////////////////////////////

class ITrailList
{
public:
	virtual ~ITrailList() = 0 {}
	virtual int GetCount() const = 0;
	virtual void AddKeyData(const TRAIL_DATA& keyData) = 0;
	virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData) = 0;
	virtual void Tick(DWORD dwTickDelta) = 0;
	virtual void Reset() = 0;
	virtual void SetSampleFreq(int iSampleFreq) {}
	virtual void PrepareRenderData() {}
	virtual int GetRenderCount() const = 0;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const = 0;
	static ITrailList* CreateTrailList(bool bIsSplineMode);
	static void DestroyTrailList(ITrailList* pTrailImpl);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class SplineTrailList
//	
///////////////////////////////////////////////////////////////////////////

class SplineTrailList : public ITrailList
{
public:

	struct KPData
	{
		KPData(int nSegLife, A3DCOLOR Color)
			: m_nSegLife(nSegLife)
            , m_nSegMaxLife(nSegLife)
			, m_Color(Color)
		{
			
		}
		int m_nSegLife;
        int m_nSegMaxLife;
		A3DCOLOR m_Color;
	};

	SplineTrailList();
	virtual ~SplineTrailList();

protected:

	virtual int GetCount() const;
	virtual void AddKeyData(const TRAIL_DATA& keyData);
	virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData);
	virtual void Tick(DWORD dwTickDelta);
	virtual void Reset();
	virtual int GetRenderCount() const;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const;
	virtual void SetSampleFreq(int iSampleFreq) 
	{ 
		m_iSampleFreq = iSampleFreq; 
		// 1 ms is the minimal sample interval
		if (m_iSampleFreq == 0) 
			m_iSampleFreq = 1; 
	}
	virtual void PrepareRenderData();

protected:

	void AddTrailKeyPoint(const TRAIL_DATA& keyData);
	size_t GetKPNum() const;
	int GetKPLife(int nKPIdx) const { return m_aKPData[nKPIdx].m_nSegLife; }
    float GetKPLifeRate(int nKPIdx) const;
	A3DVECTOR3 GetKPPosition(int nSpline, size_t nKPIndex) const;
	A3DVECTOR3 InterpPos(A3DHermiteSpline* pSpline, size_t nKPIndex, FLOAT t) const;
	A3DCOLOR InterpColor(size_t nKPIndex, FLOAT t) const;
	int InterpSegLife(size_t nKPIndex, FLOAT t) const;
    float InterpSegLifeRate(size_t nKPIndex, FLOAT t) const;
	virtual void GenerateRenderData(int i, int j);

protected:

	abase::vector<TRAIL_DATA> m_aRenderData;
	abase::vector<KPData> m_aKPData;
	A3DHermiteSpline* m_pTrailSpline[2];
	size_t m_nStartIdx;
	int m_iSampleFreq;
    abase::vector<A3DVECTOR3> m_aKp;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class LineTrailList
//	
///////////////////////////////////////////////////////////////////////////

class LineTrailList : public ITrailList
{
public:

	LineTrailList();

protected:
	
	virtual int GetCount() const;
	virtual void AddKeyData(const TRAIL_DATA& keyData);
	virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData);
	virtual void Tick(DWORD dwTickDelta);
	virtual void Reset();
	virtual int GetRenderCount() const;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const;

protected:
	RotList<TRAIL_DATA> m_TrailLst;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class RadianInterpList
//	
///////////////////////////////////////////////////////////////////////////
class RadianInterpList : public LineTrailList
{
public:
	RadianInterpList()
		: m_iSampleFreq(0)
	{}

protected:

	virtual void Tick(DWORD dwTickDelta);
	virtual int GetRenderCount() const;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const;
	virtual void PrepareRenderData();

protected:
	abase::vector<TRAIL_DATA> m_aRenderData;
	int m_iSampleFreq;

private:
	virtual void SetSampleFreq(int iSampleFreq) { m_iSampleFreq = iSampleFreq; }
	void GenerateRenderDataByTrailData(const TRAIL_DATA& td0, const TRAIL_DATA& td1);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class LineTrailGravityList
//	
///////////////////////////////////////////////////////////////////////////
class LineTrailGravityList : public LineTrailList
{
public:
	LineTrailGravityList();
	virtual ~LineTrailGravityList();
public:
	virtual void AddKeyData(const TRAIL_DATA& keyData);
	virtual void Tick(DWORD dwTickDelta);

	virtual void SetGravityData(const GRAVITY_DATA& gravityData);
	void UseGravity(bool bUse);

protected:
	void TickGravity(DWORD dwTickDelta);
	void AffectGravity(int nIdx,float fGravity);
private:
	bool	m_bGravity;
	GRAVITY_DATA	m_GravityData;
};


class DirectionalSplineTrailList : public SplineTrailList
{
public:
    DirectionalSplineTrailList();
    virtual ~DirectionalSplineTrailList();
    virtual void PrepareRenderData();

protected:

    virtual void AddKeyData(const TRAIL_DATA& keyData);
    virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData);
    void GenerateRenderData(int i, int j);
    virtual void Tick(DWORD dwTickDelta);
    virtual void Reset();

private:
    A3DHermiteSpline* m_pTrailSplineRef[2];

    void CalcSegTangents(
        const A3DVECTOR3& vLast1, const A3DVECTOR3& vLast2, const A3DVECTOR3& vCur1, const A3DVECTOR3& vCur2,
        A3DVECTOR3& vTangent1, A3DVECTOR3& vTangent2);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_TrailList_H_


