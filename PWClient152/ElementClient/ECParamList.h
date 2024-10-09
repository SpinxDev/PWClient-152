/********************************************************************
	created:	2005/08/30
	created:	30:8:2005   17:34
	file name:	ECParamList.h
	author:		yaojun
	
	  purpose:	CECParamList represent a list of parameters.
				parameters in the list do not hold any memory, 
				but should be "connected" to some varialble 
				define by user before use.

				use AddParam()/InsertParam()/RemoveParam() to 
				build param list.
				use PreparePushParam()/PushParam() or 
				SetParam() to assign value to a param.
				param value can only be assigned from a string.
				
				supported data type : 
				bool / int / short / unsigned short / long / unsigned long /
				float / double / AString

*********************************************************************/

#pragma once

#include <AString.h>
#include <AWString.h>
#include <vector.h>
#include <ABaseDef.h>
#include <AAssist.h>
#include <Windows.h>

class CECParamList
{
private:
	// helper for convert string to any type
	struct ParamBase 
	{
		virtual void ConvertFrom(AString str) = 0;
	};
	template<typename DataType> struct Param : public ParamBase {};
	template<> struct Param<bool> : public ParamBase 
	{
		bool & data;
		Param(bool & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = (str == "true") ? true : false; }
	};
	template<> struct Param<int> : public ParamBase 
	{
		int & data;
		Param(int & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = str.ToInt(); }
	};
	template<> struct Param<short> : public ParamBase 
	{
		short & data;
		Param(short & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = static_cast<short>(str.ToInt()); }
	};
	template<> struct Param<unsigned short> : public ParamBase 
	{
		unsigned short & data;
		Param(unsigned short & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = static_cast<unsigned short>(str.ToInt()); }
	};
	template<> struct Param<long> : public ParamBase 
	{
		long & data;
		Param(long & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = static_cast<long>(str.ToInt()); }
	};
	template<> struct Param<unsigned long> : public ParamBase 
	{
		unsigned long & data;
		Param(unsigned long & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = static_cast<unsigned long>(str.ToInt()); }
	};
	template<> struct Param<float> : public ParamBase 
	{
		float & data;
		Param(float & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = str.ToFloat(); }
	};
	template<> struct Param<double> : public ParamBase 
	{
		double & data;
		Param(double & host) : data(host) {}
		virtual void ConvertFrom(AString str) { data = static_cast<double>(str.ToFloat()); }
	};
	template<> struct Param<AString> : public ParamBase 
	{
		AString & data;
		Param(AString & host) : data(host) {}
		virtual void ConvertFrom(AString str) 
		{ 
			data = str; 
		}
	};
	template<> struct Param<AWString> : public ParamBase 
	{
		AWString & data;
		Param(AWString & host) : data(host) {}
		virtual void ConvertFrom(AString str) 
		{ 
			data = AS2WC(CP_UTF8, str); 
		}
	};
	
	// param vector
	typedef abase::vector<ParamBase *> ParamArray;
	ParamArray m_vecParams;
	int m_nPushParamIndex;
public:
	// contructor/destructor
	CECParamList() : m_nPushParamIndex(0) {}
	virtual ~CECParamList()
	{
		ParamArray::iterator iter = m_vecParams.begin();
		for (; iter != m_vecParams.end(); ++iter)
		{
			ParamBase *p = *iter;
			delete p;
		}
	}

	// build param list
	template<typename DataType>
	void AddParam(DataType & data) { m_vecParams.push_back(new Param<DataType>(data)); }
	void RemoveParam(int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < (int)m_vecParams.size());
		
		ParamBase * p = m_vecParams[nIndex];
		delete p;

		ParamArray::iterator iter1, iter2;
		iter1 = m_vecParams.begin() + nIndex;
		iter2 = iter1 + 1;
		m_vecParams.erase(iter1, iter2);
	}
	template<typename DataType>
	void InsertParam(int nIndex, DataType &data)
	{
		ASSERT(nIndex >= 0 && nIndex <= (int)m_vecParams.size());

		ParamArray::iterator iter = m_vecParams.begin() + nIndex;
		m_vecParams.insert(iter, new Param<DataType>(data));
	}

	// assign value to param
	void PreparePushParamValue() { m_nPushParamIndex = 0; }
	void PushParamValue(AString strParam) 
	{ 
		ASSERT(m_nPushParamIndex >= 0 && m_nPushParamIndex < (int)m_vecParams.size());
		SetParamValue(m_nPushParamIndex++, strParam);	
	}
	
	void SetParamValue(int nIndex, AString strParam) 
	{ 
		ASSERT(nIndex >= 0 && nIndex < (int)m_vecParams.size());
		m_vecParams[nIndex]->ConvertFrom(strParam); 
	}
};
