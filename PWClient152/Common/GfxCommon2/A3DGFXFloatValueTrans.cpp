#include "StdAfx.h"
#include "A3DGFXFloatValueTrans.h"
#include <AFile.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static const char* _format_animtype		= "AnimType: %d";
static const char* _format_destnum		= "DestNum: %d";
static const char* _format_starttime	= "StartTime: %d";
static const char* _format_destval		= "DestVal: %f";
static const char* _format_transtime	= "TransTime: %d";

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static float Lerp(float fLhs, float fRhs, float fFraction)
{
	return fLhs * (1.0f - fFraction) + fRhs * fFraction;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXFloatValueTrans
//	
///////////////////////////////////////////////////////////////////////////

static PROPERTY_ENTRY FloatValueCtrlPropEntries[] = 
{
	{ ID_ANIM_FLOAT_CONTROL_STARTTIME,	"初始时间" },
	{ ID_ANIM_FLOAT_CONTROL_INITVAL,	"初始值" },
	{ ID_ANIM_FLOAT_CONTROL_DESTNUM,	"目标数" },
	{ ID_ANIM_FLOAT_CONTROL_DESTVAL1,	"目标值1" },
	{ ID_ANIM_FLOAT_CONTROL_TRANSTIME1,	"间隔时间1" },
	{ ID_ANIM_FLOAT_CONTROL_DESTVAL2,	"目标值2" },
	{ ID_ANIM_FLOAT_CONTROL_TRANSTIME2,	"间隔时间2" },
	{ ID_ANIM_FLOAT_CONTROL_DESTVAL3,	"目标值3" },
	{ ID_ANIM_FLOAT_CONTROL_TRANSTIME3,	"间隔时间3" },
	{ ID_ANIM_FLOAT_CONTROL_DESTVAL4,	"目标值4" },
	{ ID_ANIM_FLOAT_CONTROL_TRANSTIME4,	"间隔时间4" },
	{ ID_ANIM_FLOAT_CONTROL_DESTVAL5,	"目标值5" },
	{ ID_ANIM_FLOAT_CONTROL_TRANSTIME5,	"间隔时间5" },
};

static PROPERTY_META FloatValueCtrlMeta(_countof(FloatValueCtrlPropEntries), FloatValueCtrlPropEntries);

const PROPERTY_META* A3DGFXFloatValueTrans::GetPropertyMeta() const
{
	return &FloatValueCtrlMeta;
}

bool A3DGFXFloatValueTrans::IsEqual(const A3DGFXAnimable* pRhs) const
{
	if (const A3DGFXFloatValueTrans* pFloatValueTrans = dynamic_cast<const A3DGFXFloatValueTrans*>(pRhs))
	{
		return pFloatValueTrans->m_iDestNum == m_iDestNum
			&& pFloatValueTrans->m_iStartTime == m_iStartTime
			&& memcmp(pFloatValueTrans->m_aDestVals, m_aDestVals, sizeof(m_aDestVals)) == 0
			&& memcmp(pFloatValueTrans->m_aTransTimes, m_aTransTimes, sizeof(m_aTransTimes)) == 0 
			&& pFloatValueTrans->m_bInterpolate == m_bInterpolate;
	}
	else
		return false;
}

A3DGFXAnimValue A3DGFXFloatValueTrans::GetValue(TimeValue t) const
{
	if (t <= m_iStartTime || m_iDestNum == 0)
		return A3DGFXAnimValue(m_aDestVals[0]);

	TimeValue tAccu = m_iStartTime;
	int iIdx = 0;
	for (iIdx = 0; iIdx < m_iDestNum; ++iIdx)
	{
		if (t > tAccu && t > tAccu + m_aTransTimes[iIdx])
		{
			tAccu += m_aTransTimes[iIdx];
			continue;
		}

		ASSERT( t > tAccu && t <= tAccu + m_aTransTimes[iIdx]);

		float fCurVal;
		if (m_bInterpolate)
			fCurVal = Lerp(m_aDestVals[iIdx], m_aDestVals[iIdx + 1], (t - tAccu) / (float)m_aTransTimes[iIdx]);
		else
			fCurVal = m_aDestVals[iIdx];

		return A3DGFXAnimValue(fCurVal);
	}

	return A3DGFXAnimValue(m_aDestVals[m_iDestNum]);
}

A3DGFXAnimable* A3DGFXFloatValueTrans::Clone() const
{
	return new A3DGFXFloatValueTrans(*this);
}

bool A3DGFXFloatValueTrans::Save(AFile* pFile)
{
	char szLine[AFILE_LINEMAXLEN];

	sprintf_s(szLine, _format_destnum, m_iDestNum);
	pFile->WriteLine(szLine);

	sprintf_s(szLine, _format_starttime, m_iStartTime);
	pFile->WriteLine(szLine);

	//	Dest value array has (DestNum + 1) values
	for (int iIdx = 0; iIdx <= m_iDestNum; ++iIdx)
	{
		sprintf_s(szLine, _format_destval, m_aDestVals[iIdx]);
		pFile->WriteLine(szLine);
	}

	for (int iIdx = 0; iIdx < m_iDestNum; ++iIdx)
	{
		sprintf_s(szLine, _format_transtime, m_aTransTimes[iIdx]);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXFloatValueTrans::Load(AFile* pFile, DWORD dwVersion)
{
	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_destnum, &m_iDestNum);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_starttime, &m_iStartTime);

	//	Dest value array has (DestNum + 1) values
	for (int iIdx = 0; iIdx <= m_iDestNum; ++iIdx)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_destval, &m_aDestVals[iIdx]);
	}

	for (int iIdx = 0; iIdx < m_iDestNum; ++iIdx)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_transtime, &m_aTransTimes[iIdx]);
	}

	return true;
}

GFX_PROPERTY A3DGFXFloatValueTrans::GetProperty(int nID)
{
	switch (nID)
	{
	case ID_ANIM_FLOAT_CONTROL_INITVAL:
		return GFX_PROPERTY(m_aDestVals[0]);
	case ID_ANIM_FLOAT_CONTROL_STARTTIME:
		return GFX_PROPERTY(m_iStartTime).SetType(GFX_VALUE_INT);
	case ID_ANIM_FLOAT_CONTROL_DESTNUM:
		return GFX_PROPERTY(m_iDestNum);
	case ID_ANIM_FLOAT_CONTROL_DESTVAL1:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL2:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL3:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL4:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL5:
		return GFX_PROPERTY(m_aDestVals[nID - ID_ANIM_FLOAT_CONTROL_DESTVAL1 + 1]);
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME1:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME2:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME3:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME4:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME5:
		return GFX_PROPERTY(m_aTransTimes[nID - ID_ANIM_FLOAT_CONTROL_TRANSTIME1]).SetType(GFX_VALUE_INT);
	default:
		return GFX_PROPERTY();
	}	
}

void A3DGFXFloatValueTrans::SetProperty(int nID, const GFX_PROPERTY& vIn)
{
	GFX_PROPERTY v(vIn);

	if (m_pConstrains)
		m_pConstrains->PropertyConstrain(nID, v);

	switch (nID)
	{
	case ID_ANIM_FLOAT_CONTROL_INITVAL:
		m_aDestVals[0] = (float)v;
		break;
	case ID_ANIM_FLOAT_CONTROL_DESTNUM:
		m_iDestNum = (int)v;
		break;
	case ID_ANIM_FLOAT_CONTROL_STARTTIME:
		m_iStartTime = (int)v;
		break;
	case ID_ANIM_FLOAT_CONTROL_DESTVAL1:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL2:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL3:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL4:
	case ID_ANIM_FLOAT_CONTROL_DESTVAL5:
		m_aDestVals[nID - ID_ANIM_FLOAT_CONTROL_DESTVAL1 + 1] = (float)v;
		break;
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME1:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME2:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME3:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME4:
	case ID_ANIM_FLOAT_CONTROL_TRANSTIME5:
		m_aTransTimes[nID - ID_ANIM_FLOAT_CONTROL_TRANSTIME1] = (DWORD)v;
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}