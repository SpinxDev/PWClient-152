/********************************************************************
	created:	2006/09/07
	author:		kuiwu
	
	purpose:	the value used to communicate with script(eg. lua)
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#pragma  once

#include <vector.h>
#include <lua.hpp>
#include <AString.h>
#include <AWString.h>
#include <ABaseDef.h>

//Null-terminated script string, the buffer is utf8
//note: the string with given length is poorly supportted. 
class CScriptString
{
public:
	CScriptString();
	CScriptString(const CScriptString& str);
	~CScriptString(); //non-virtual
	CScriptString& operator=(const CScriptString& str);

	void SetUtf8(const char * pBuf, int nLen);
	void SetAString(const char * str);
	void SetAWString(const wchar_t * str);

	
	const char* GetUtf8() const { return m_pBuf; }
	//utf8 string len
	int         GetLen() const {return m_nLen;}
	void RetrieveAString(AString& out) const;
	void RetrieveAWString(AWString& out) const ;

	
	void Release();
private:
	char * m_pBuf;  
	int    m_nLen;  //strlen, exclude  '\0'
	CScriptString& operator=(const char * str);
	void AWStringToUtf8(const AWString& in);
	void Utf8ToAWString(AWString& out) const;

	void AStringToUtf8(const AString& in);
	void Utf8ToAString(AString& out) const;
};

struct _64BITS_ID
{
	__int64 val;

	explicit _64BITS_ID(__int64 v) : val(v) {}
};

class CScriptValue  
{
public:
	enum  SV_TYPE
	{
		SVT_INVALID,
		SVT_BOOL,
		SVT_NUMBER,
		SVT_STRING,
		SVT_ARRAY,
		SVT_USERDATA,
	};
	int   m_Type;

	
	CScriptString  m_String;
	abase::vector<CScriptValue> m_ArrVal;
	abase::vector<CScriptValue> m_ArrKey;

	union
	{
		bool    m_Bool;
		double  m_Number;
		//	Change the type of m_UserData from DWORD to void* (zhangyachuan 09.6.19)
		//	In order to avoid the VC++ 2005's complain of "scriptvalue.h(130) : warning C4312: 'type cast' : conversion from 'const DWORD' to 'void *' of greater size"
		//	Because the void* might be a 64bit pointer in an 64bit enviroment, as DWORD might be anything since it is just a typedef
		//	Also because the lua's interface requires a void* style pointer instead of a DWORD value when it comes to the UserData
		//	See:
		//		LUA_API void	       *(lua_touserdata) (lua_State *L, int idx);
		//		LUA_API void	 (lua_pushlightuserdata) (lua_State *L, void *p);
		void*   m_UserData;		//pointer,  not set an int
		__int64 m_64BitsId;
	};

	CScriptValue();
	CScriptValue(const CScriptValue& val);
	CScriptValue(double val);
	CScriptValue(bool val);
	//CScriptValue(DWORD val);  //for clearity
	CScriptValue(void* val);
	CScriptValue(const CScriptString& val);

	explicit CScriptValue(_64BITS_ID val);

	~CScriptValue();  //non-virtual

	double GetDouble() const
	{
		assert(m_Type == SVT_NUMBER);
		return  m_Number;
	}
	int   GetInt() const
	{
		assert(m_Type == SVT_NUMBER);
		int i;
		double d;
		d = m_Number;
		lua_number2int(i, d);
		//i = (int)d;
		return i;
	}
	bool GetBool() const
	{
		assert(m_Type == SVT_BOOL);
		return m_Bool;
	}
	__int64 Get64BitsId() const;

	void    RetrieveAString(AString& out) const
	{
		assert(m_Type == SVT_STRING);
		m_String.RetrieveAString(out);
	}

	void    RetrieveAWString(AWString& out) const
	{
		assert(m_Type == SVT_STRING);
		m_String.RetrieveAWString(out);
	}
	
	void RetrieveUtf8(AString& out) const
	{
		assert(m_Type == SVT_STRING);
		out = m_String.GetUtf8();
	}

	void *  GetUserData() const
	{
		assert(m_Type == SVT_USERDATA);
		return m_UserData;
	}

	void SetVal(bool val)
	{
		m_Type = SVT_BOOL;
		m_Bool = val;
	}

	void SetVal(int val)
	{
		m_Type = SVT_NUMBER;
		m_Number = (double)val;
	}

	void Set64BitsId(__int64 val);

	void SetVal(double val)
	{
		m_Type = SVT_NUMBER;
		m_Number = val;
	}

	//note: for compatibility, not recommended
	//astring
	void SetVal(const char * val)
	{
		m_Type = SVT_STRING;
		m_String.SetAString(val);
	}
	//note: for compatibility, not recommended
	//awstring
	void SetVal(const wchar_t * val)
	{
		m_Type = SVT_STRING;
		m_String.SetAWString(val);
	}
	//set string, recommended 
	void SetVal(const CScriptString& val)
	{
		m_Type = SVT_STRING;
		m_String = val;
	}

	

	void SetVal(void * val)
	{
		m_Type = SVT_USERDATA;
		m_UserData = val;
	}



	/**
	 * \brief set the script value as table
	 * \param[in]
	 * \param[out]
	 * \return
	 * \note  NOT check the keys
	 * \warning  currently  keys MUST be simple type(typically: string, number)
	 * \todo   
	 * \author kuiwu 
	 * \date 28/4/2007
	 * \see 
	 */
	void SetArray(abase::vector<CScriptValue>& vals, abase::vector<CScriptValue>& keys)
	{
		m_Type = SVT_ARRAY;
		assert(keys.size() == vals.size());
		m_ArrKey = keys;
		m_ArrVal = vals;
	}
	//number as key, that is a "real" array.
	void SetArray(abase::vector<CScriptValue>& vals)
	{
		m_Type = SVT_ARRAY;
		m_ArrKey.clear();
		m_ArrVal = vals;
	}

};

//Todo: move the following to other place later
typedef struct lua_State lua_State;
namespace LuaBind
{
	void GetStack(lua_State * L, abase::vector<CScriptValue>& values, int start);
	void SetStack(lua_State * L, const abase::vector<CScriptValue>& values);
	
	bool Call(lua_State*L, const char * szTblName, const char * szMethod, 
		const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);

	//for debug
	void DumpValue(const CScriptValue& val);




}
