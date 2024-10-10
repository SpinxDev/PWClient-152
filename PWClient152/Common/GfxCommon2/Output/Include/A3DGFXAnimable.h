/*
* FILE: A3DGFXAnimable.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/8/12
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXAnimable_H_
#define _A3DGFXAnimable_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define ID_ANIM_FLOAT_CONTROL_INITVAL		100
#define ID_ANIM_FLOAT_CONTROL_DESTNUM		101
#define ID_ANIM_FLOAT_CONTROL_STARTTIME		102
#define ID_ANIM_FLOAT_CONTROL_DESTVAL1		103
#define ID_ANIM_FLOAT_CONTROL_DESTVAL2		104
#define ID_ANIM_FLOAT_CONTROL_DESTVAL3		105
#define ID_ANIM_FLOAT_CONTROL_DESTVAL4		106
#define ID_ANIM_FLOAT_CONTROL_DESTVAL5		107
#define ID_ANIM_FLOAT_CONTROL_TRANSTIME1	108
#define ID_ANIM_FLOAT_CONTROL_TRANSTIME2	109
#define ID_ANIM_FLOAT_CONTROL_TRANSTIME3	110
#define ID_ANIM_FLOAT_CONTROL_TRANSTIME4	111
#define ID_ANIM_FLOAT_CONTROL_TRANSTIME5	112

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

typedef DWORD TimeValue;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

struct PROPERTY_ENTRY
{
	int m_nID;
	const char* m_szName;
};

struct PROPERTY_META
{
	PROPERTY_META(int num, const PROPERTY_ENTRY* entrys)
		: m_iEntryNum(num)
		, m_aEntries(entrys)
	{

	}

	int GetEntryNum() const { return m_iEntryNum; }
	const PROPERTY_ENTRY* GetEntries() const { return m_aEntries; }

private:

	int m_iEntryNum;
	const PROPERTY_ENTRY* m_aEntries;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXAnimValue
//	
///////////////////////////////////////////////////////////////////////////

class A3DGFXAnimValue
{
public:

	enum ValueType
	{
		VT_INT,
		VT_FLOAT,
		VT_VECTOR3,
		VT_VECTOR4,
		VT_QUATERNION,
	};

private:

	ValueType m_ValueType;

	union 
	{
		INT m_iVal;
		FLOAT m_fVal[4];
	};

public:

	A3DGFXAnimValue(ValueType vt) : m_ValueType(vt) {}
	A3DGFXAnimValue(int v) : m_ValueType(VT_INT) { SetValue(v); }
	A3DGFXAnimValue(float v) : m_ValueType(VT_FLOAT) { SetValue(v); }
	A3DGFXAnimValue(const A3DVECTOR3& v) : m_ValueType(VT_VECTOR3) { SetValue(v); }
	A3DGFXAnimValue(const A3DVECTOR4& v) : m_ValueType(VT_VECTOR4) { SetValue(v); }
	A3DGFXAnimValue(const A3DQUATERNION& v) : m_ValueType(VT_QUATERNION) { SetValue(v); }
		

	ValueType GetType() const { return m_ValueType; }
	void GetValue(int& iVal) const { iVal = m_iVal; }
	void GetValue(float& fVal) const { fVal = m_fVal[0]; }
	void GetValue(A3DVECTOR3& vVal) const { memcpy(vVal.m, m_fVal, sizeof(float) * 3); }
	void GetValue(A3DVECTOR4& vVal) const { memcpy(vVal.m, m_fVal, sizeof(float) * 4); }
	void GetValue(A3DQUATERNION& qVal) const { memcpy(qVal.m, m_fVal, sizeof(float) * 4); }

	void SetValue(int iVal) { ASSERT( m_ValueType == VT_INT ); m_iVal = iVal; }
	void SetValue(float fVal) { ASSERT( m_ValueType == VT_FLOAT); m_fVal[0] = fVal; }
	void SetValue(const A3DVECTOR3& vVal) { ASSERT( m_ValueType == VT_VECTOR3 ); memcpy(m_fVal, vVal.m, sizeof(float) * 3); }
	void SetValue(const A3DVECTOR4& vVal) { ASSERT( m_ValueType == VT_VECTOR4 ); memcpy(m_fVal, vVal.m, sizeof(float) * 4); }
	void SetValue(const A3DQUATERNION& qVal) { ASSERT( m_ValueType == VT_QUATERNION ); memcpy(m_fVal, qVal.m, sizeof(float) * 4); }
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXAnimable
//	
///////////////////////////////////////////////////////////////////////////

class A3DGFXAnimable
{

public:		//	Types

	enum AnimableType
	{
		ANIMTYPE_FLOATTRANS = 0,
		ANIMTYPE_FLOATTRANS_NOINTERPOLATE,
		ANIMTYPE_NUM,
	};

	class PropertyConstrains
	{
	public:
		virtual void PropertyConstrain(int nID, GFX_PROPERTY& v) const = 0;
	};

protected:	//	Constructor and Destructor

	explicit A3DGFXAnimable(AnimableType at) 
		: m_at(at)
		, m_pConstrains(NULL) 
	{
	}

	virtual ~A3DGFXAnimable(void) = 0 {}

public:		//	Attributes

public:		//	Operations

	static A3DGFXAnimable* CreateAnimable(AnimableType at);
	static void DestroyAnimable(A3DGFXAnimable* pAnimable);
	static bool SaveAnimable(AFile* pFile, A3DGFXAnimable* pAnimable);
	static bool LoadAnimable(AFile* pFile, A3DGFXAnimable** pAnimable, DWORD dwVersion);

	virtual A3DGFXAnimValue GetValue(TimeValue t) const = 0;
	virtual A3DGFXAnimable* Clone() const = 0;
	virtual bool Save(AFile* pFile) = 0;
	virtual bool Load(AFile* pFile, DWORD dwVersion) = 0;
	virtual const PROPERTY_META* GetPropertyMeta() const = 0;
	virtual GFX_PROPERTY GetProperty(int nID) = 0;
	virtual void SetProperty(int nID, const GFX_PROPERTY& vIn) = 0;
	virtual bool IsEqual(const A3DGFXAnimable* pRhs) const = 0;
	
	//	Get type
	inline AnimableType GetAnimableType() const { return m_at; }
	void SetPropertyConstrains(const PropertyConstrains* pConstrains);

protected:	//	Attributes

	AnimableType m_at;
	const PropertyConstrains* m_pConstrains;

protected:	//	Operations

};


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

inline bool operator == (const A3DGFXAnimable& lhs, const A3DGFXAnimable& rhs)
{
	return lhs.IsEqual(&rhs);
}


#endif	//	_A3DGFXAnimable_H_


