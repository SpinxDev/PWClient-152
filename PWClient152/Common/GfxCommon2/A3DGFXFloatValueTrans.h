#ifndef _A3DGFXFloatValueTrans_H_
#define _A3DGFXFloatValueTrans_H_

#include "A3DGFXAnimable.h"


class A3DGFXFloatValueTrans : public A3DGFXAnimable
{

public:		//	Types

	enum 
	{
		DEST_VALUE_NUM = 5,
	};

public:		//	Constructor and Destructor

	A3DGFXFloatValueTrans(bool bInterpolate = true)
		: A3DGFXAnimable(ANIMTYPE_FLOATTRANS)
	{
		m_iDestNum = 1;
		m_iStartTime = 0;
		memset(m_aDestVals, 0, sizeof(m_aDestVals));
		memset(m_aTransTimes, 0, sizeof(m_aTransTimes));
		m_bInterpolate = bInterpolate;
	}

	A3DGFXFloatValueTrans(const A3DGFXFloatValueTrans& rhs)
		: A3DGFXAnimable(rhs.GetAnimableType())
		, m_iDestNum(rhs.m_iDestNum)
		, m_iStartTime(rhs.m_iStartTime)
	{
		memcpy(m_aDestVals, rhs.m_aDestVals, sizeof(m_aDestVals));
		memcpy(m_aTransTimes, rhs.m_aTransTimes, sizeof(m_aTransTimes));
		m_bInterpolate = rhs.m_bInterpolate;
	}

	virtual ~A3DGFXFloatValueTrans(void)
	{

	}

public:		//	Attributes

public:		//	Operations

	virtual A3DGFXAnimValue GetValue(TimeValue t) const;
	virtual A3DGFXAnimable* Clone() const;
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual GFX_PROPERTY GetProperty(int nID);
	virtual void SetProperty(int nID, const GFX_PROPERTY& v);
	virtual const PROPERTY_META* GetPropertyMeta() const;
	virtual bool IsEqual(const A3DGFXAnimable* pRhs) const;

	inline void SetInitValue(float val) { m_aDestVals[0] = val; }
	inline void SetDestNum(int num) { m_iDestNum = num; }
	
protected:	//	Attributes

	int m_iDestNum;
	TimeValue m_iStartTime;
	float m_aDestVals[DEST_VALUE_NUM + 1];
	TimeValue m_aTransTimes[DEST_VALUE_NUM];
	bool	m_bInterpolate;

protected:	//	Operations

};


































#endif