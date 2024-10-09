/********************************************************************
	created:	2005/09/27
	created:	27:9:2005   17:08
	file name:	Param.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include <AAssist.h>
#include <vector.h>

//////////////////////////////////////////////////////////////////////////
// CParamArray
//////////////////////////////////////////////////////////////////////////

class CParam;
typedef abase::vector<CParam *> CParamArrayBase;
class CParamArray : public CParamArrayBase
{
public:
	CParamArray();
	CParamArray(const CParamArray & src);
	virtual ~CParamArray();
	void ClearParams();
	CParamArray & operator = (const CParamArray & src);
//	virtual CParamArray * Clone() const;
//	virtual void Assign(const CParamArray & src);
};


//////////////////////////////////////////////////////////////////////////
// CParam
//////////////////////////////////////////////////////////////////////////

class CParam  
{
protected:	
	ACString m_strName;
	int m_nDisplayType;
	CParamArray m_displayValues;
public:
	enum {TYPE_NORMAL, TYPE_LIST, TYPE_BOOL, TYPE_CUSTOM, TYPE_VOID};
	CParam(ACString strName, int nDisplayType = TYPE_NORMAL, const CParamArray *pDisplayValues = NULL);
	virtual ~CParam();
	ACString GetName() const;
	int GetDisplayType() const;
	void SetDisplayValues(const CParamArray& displayValues);
	const CParamArray & GetDisplayValues() const;
	CParamArray & GetDisplayValues();

	virtual void SetStringValue(ACString strVal) = 0;
	virtual ACString GetStringValue() const = 0;
	virtual CParam * Clone() const = 0;
	virtual void Assign(const CParam & src) = 0;
	virtual bool IsEqualTo(const CParam & src) const = 0;
	virtual void AssignValue(const CParam & src) = 0;
};

//////////////////////////////////////////////////////////////////////////
// CParamImpBase
//////////////////////////////////////////////////////////////////////////

// base

template<typename DataType> class CParamImpBase : public CParam 
{
protected:
	DataType m_Data;
public:
	CParamImpBase(
		ACString strName, 
		DataType valInit, 
		int nDisplayType = TYPE_NORMAL, 
		const CParamArray *pDisplayValues = NULL) 
		: CParam(strName, nDisplayType, pDisplayValues), m_Data(valInit) 
	{
	}
	DataType GetTrueValue() const { return m_Data; }
	void SetTrueValue(DataType val) { m_Data = val; }
	virtual void SetStringValue(ACString strVal) {}
	virtual ACString GetStringValue() const { return _AL(""); }
	virtual void Assign(const CParam & src)
	{
		m_strName = src.GetName();
		m_nDisplayType = src.GetDisplayType();
		m_displayValues = src.GetDisplayValues();
		m_Data = GetParamTrueValue<DataType>(&src);
	}
	virtual void AssignValue(const CParam & src)
	{
		m_Data = GetParamTrueValue<DataType>(&src);
	}
	virtual bool IsEqualTo(const CParam & src) const
	{
		return GetParamTrueValue<DataType>(&src) == m_Data;
	}
};

// derived template
template<typename DataType> class CParamImp : public CParamImpBase<DataType>
{
public:
	CParamImp(
		ACString strName, 
		DataType valInit, 
		int nDisplayType = TYPE_NORMAL, 
		const CParamArray *pDisplayValues = NULL) 
		: CParamImpBase<DataType>(strName, valInit, nDisplayType, pDisplayValues)
	{
	}
	virtual CParam * Clone() const 
	{ 
		CParamImp<DataType> *pNewParam =
			new CParamImp<DataType>(GetName(), GetTrueValue(), GetDisplayType(), &GetDisplayValues());
		return pNewParam;
	}
};

// some helper
template<typename DataType>
DataType GetParamTrueValue(const CParam *pParam)
{
	const CParamImp<DataType> *pParamImp = dynamic_cast<const CParamImp<DataType> *>(pParam);
	ASSERT(pParamImp && "param true value type not match");
	return pParamImp->GetTrueValue();
}

template<typename DataType>
void SetParamTrueValue(CParam *pParam, DataType val)
{
	CParamImp<DataType> *pParamImp = dynamic_cast<CParamImp<DataType> *>(pParam);
	ASSERT(pParamImp && "param true value type not match");
	pParamImp->SetTrueValue(val);
}

#define IMPLEMENT_PARAM_CONSTRUCTOR(dataType, defaultInitVal, defaultDisplayType) \
CParamImp( \
	ACString strName, \
	dataType valInit = defaultInitVal, \
	int nDisplayType = defaultDisplayType, \
	const CParamArray *pDisplayValues = NULL) \
	: CParamImpBase<dataType>(strName, valInit, nDisplayType, pDisplayValues) \
{ \
}

#define IMPLEMENT_PARAM_CLONE_INLINE(dataType) \
virtual CParam * Clone() const \
{ \
	CParamImp<dataType> *pNewParam = \
		new CParamImp<dataType>(GetName(), GetTrueValue(), GetDisplayType(), &GetDisplayValues()); \
	return pNewParam; \
}


// spciailiztion for each derived CParamImpBase
template<> class CParamImp<bool> : public CParamImpBase<bool>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(bool, false, TYPE_BOOL);
	IMPLEMENT_PARAM_CLONE_INLINE(bool);

	virtual void SetStringValue(ACString strVal) { m_Data = (strVal == _AL("0")) ? false : true; }
	virtual ACString GetStringValue() const { return m_Data ? _AL("1") : _AL("0"); }
};

template<> class CParamImp<int> : public CParamImpBase<int>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(int, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(int);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToInt(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%d"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<unsigned int> : public CParamImpBase<unsigned int>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(unsigned int, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(unsigned int);
	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToInt(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%u"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<long> : public CParamImpBase<long>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(long, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(long);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToInt(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%d"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<unsigned long> : public CParamImpBase<unsigned long>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(unsigned long, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(unsigned long);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToInt(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%u"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<short> : public CParamImpBase<short>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(short, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(short);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToInt(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%d"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<unsigned short> : public CParamImpBase<unsigned short>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(unsigned short, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(unsigned short);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToInt(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%u"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<float> : public CParamImpBase<float>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(float, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(float);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToFloat(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%f"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<double> : public CParamImpBase<double>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(double, 0, TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(double);

	virtual void SetStringValue(ACString strVal) { m_Data = strVal.ToFloat(); }
	virtual ACString GetStringValue() const 
	{ 
		ACString strVal;
		strVal.Format(_AL("%f"), m_Data);
		return strVal;
	}
};

template<> class CParamImp<AString> : public CParamImpBase<AString>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(AString, "", TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(AString);

	virtual void SetStringValue(ACString strVal) { m_Data = AC2AS(strVal); }
	virtual ACString GetStringValue() const { return AS2AC(m_Data); }
};


template<> class CParamImp<AWString> : public CParamImpBase<AWString>
{
public:
	IMPLEMENT_PARAM_CONSTRUCTOR(AWString, L"", TYPE_NORMAL);
	IMPLEMENT_PARAM_CLONE_INLINE(AWString);

	virtual void SetStringValue(ACString strVal) { m_Data = AC2WC(strVal); }
	virtual ACString GetStringValue() const { return WC2AC(m_Data); }
};
