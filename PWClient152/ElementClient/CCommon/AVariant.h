/*
 * FILE: AVariant.h
 *
 * DESCRIPTION: 实现万用数据类AVariant
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY: By JiangLi。使用模板构造函数以及模板类型转换操作符应该是不可行的。
 *               因此重新定义了相关部分。
 *
 * MODIFY: By 崔铭。去除了
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */


#ifndef _AVARIANT_H__
#define _AVARIANT_H__

#include <astring.h>
#include <a3dtypes.h>
#include <a3dmacros.h>
#include <a3DFontMan.h>
#include "AFile.h"

#define DEFINEOPERATOR(type) 	operator type() {return *(type *)(&m_avData);}

class ASet;
class AObject;
class AClassInfo;
class AScriptCodeData;
class AVariant  
{
public:

	enum AVARTYPE
	{
		AVT_INVALIDTYPE,	// 非法类型，用于默认构造,此时任何数据成员都不可用
		AVT_BOOL,			// 布尔量				m_avData.bV;
		AVT_CHAR,			// 字符					m_avData.cV;	
		AVT_UCHAR,			// 无符号字符			m_avData.ucV;	
		AVT_SHORT,			// 短整型				m_avData.sV;	
		AVT_USHORT,			// 无符号短整型			m_avData.usV;	
		AVT_INT,			// 整型					m_avData.iV;	
		AVT_UINT,			// 无符号整型			m_avData.uiV;	
		AVT_LONG,			// 长整型				m_avData.lV;	
		AVT_ULONG,			// 无符号长整型			m_avData.ulV;	
		AVT_INT64,			// 64Bit整型			m_avData.i64V;	
		AVT_UINT64,			// 无符号64Bit整型		m_avData.ui64V;
		AVT_FLOAT,			// 浮点数				m_avData.fV;	
		AVT_DOUBLE,			// 双精度浮点数			m_avData.dV;	
		AVT_POBJECT,		// 对象指针				m_avData.paV;
		AVT_PSCRIPT,		//脚本代码对象				m_avData.pascdV
		AVT_A3DVECTOR3,		// 向量					m_avData.a3dvV;
		AVT_A3DMATRIX4,		// 矩阵					m_avData.pa3dmV;
		AVT_STRING,			// 字符串				m_avData.pstrV;
		AVT_PABINARY,		// 二进制块				m_avData.binV;
		AVT_PACLASSINFO,	// 类串行化信息指针		m_avData.paciV;
		AVT_PASET,			// 集合指针				m_avData.pasetV;
		AVT_FONT,			// 字体					m_avData.font;
	};


public:

	AVariant();
	AVariant(const AVariant & v);
	AVariant(bool bV);
	AVariant(char cV);
	AVariant(unsigned char ucV);
	AVariant(short sV);
	AVariant(unsigned short usV);
	AVariant(int iV);
	AVariant(unsigned int uiV);
	AVariant(long lV);
	AVariant(unsigned long ulV);
	AVariant(__int64 i64V);
	AVariant(unsigned __int64 ui64V);
	AVariant(float fV);
	AVariant(double dV);
	AVariant(AObject & object);
	AVariant(AScriptCodeData & object);
	AVariant(A3DVECTOR3 & avec3);
	AVariant(A3DMATRIX4 &amat4);
	AVariant(AString & str);
//	AVariant(const ABinary & binV);
	AVariant(AClassInfo * paciV);
//	AVariant(ASet * pSet);
	AVariant(A3DFontMan::FONTTYPE font);

	int GetType() const {return m_iType;}

	DEFINEOPERATOR(bool)
	DEFINEOPERATOR(char)
	DEFINEOPERATOR(unsigned char)
	DEFINEOPERATOR(short)
	DEFINEOPERATOR(unsigned short)
	DEFINEOPERATOR(int)
	DEFINEOPERATOR(unsigned int)
	DEFINEOPERATOR(long)
	DEFINEOPERATOR(unsigned long)
	DEFINEOPERATOR(__int64)
	DEFINEOPERATOR(unsigned __int64)

	// 对float的转换做特殊设计
	operator float()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		if(m_iType == AVT_DOUBLE)
			return (float)m_avData.dV;
		if(m_iType == AVT_FLOAT)
			return m_avData.fV;
		return (float)(*(__int64 *)(&m_avData));
	}

	// 对double的转换做特殊设计
	operator double()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		if(m_iType == AVT_DOUBLE)
			return m_avData.dV;
		if(m_iType == AVT_FLOAT)
			return (double)m_avData.fV;
		return (double)(*(__int64 *)(&m_avData));
	}

	operator AString()
	{
		return m_string;
	}

	AVariant& operator= (const AVariant& src)
	{
		m_string = src.m_string;
		memcpy(&m_avData, &src.m_avData, sizeof(src.m_avData));
		m_iType = src.m_iType;
		return *this;
	}

	DEFINEOPERATOR(AObject *)
	DEFINEOPERATOR(A3DVECTOR3)
	DEFINEOPERATOR(A3DMATRIX4)
	DEFINEOPERATOR(AClassInfo *)
	DEFINEOPERATOR(AScriptCodeData*)
	DEFINEOPERATOR(A3DFontMan::FONTTYPE)
	operator AScriptCodeData&()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		return *(AScriptCodeData*)(&m_avData);
	}
//	DEFINEOPERATOR(ASet *)
	
	void Save(AFile* pFile);
	void Load(AFile* pFile);

private:

	AString m_string;
	union AVARIANT
	{
		bool				bV;			// 布尔量
		char				cV;			// 字符
		unsigned char			ucV;		// 无符号字符
		short				sV;			// 短整型
		unsigned short			usV;		// 无符号短整型
		int				iV;			// 整型
		unsigned int			uiV;		// 无符号整型
		long				lV;			// 长整型
		unsigned long			ulV;		// 无符号长整型
		__int64				i64V;		// 64Bit整型
		unsigned __int64		ui64V;		// 无符号64Bit整型
		float				fV;			// 浮点数
		double				dV;			// 双精度浮点数
		AObject *			paV;		// 对象指针
		char				a3dvV[sizeof(A3DVECTOR3)];	// 向量数据区
		char				a3dmV[sizeof(A3DMATRIX4)];	// 矩阵数据区
//		AString *			pstrV;		// 字符串 
//		const ABinary *			pbinV;		// 二进制块对象指针
		AClassInfo *			paciV;		// 串行化信息指针
		ASet *				pasetV;		// 集合指针
		AScriptCodeData*		pascdV;		//脚本的指针
		A3DFontMan::FONTTYPE font;
	} m_avData;
	int m_iType;	// 数据类型
};

#define		CODE_PAGE			CP_ACP

class CSafeString
{
	LPSTR m_szBuf;
	LPWSTR m_wszBuf;

public:
	CSafeString(LPCSTR lpsz, int nLen) : m_wszBuf(NULL)
	{
		assert(lpsz);
		if (nLen < 0) nLen = strlen(lpsz);
		m_szBuf = new char[nLen+1];
		strncpy(m_szBuf, lpsz, nLen);
		m_szBuf[nLen] = '\0';
	}

	CSafeString(LPCSTR lpsz) : m_wszBuf(NULL)
	{
		assert(lpsz);
		int n = strlen(lpsz);
		m_szBuf = new char[n+1];
		strcpy(m_szBuf, lpsz);
	}
	
	CSafeString(LPCWSTR lpwsz, int nLen) : m_szBuf(NULL)
	{
		assert(lpwsz);
		if (nLen < 0) nLen = wcslen(lpwsz);
		m_wszBuf = new wchar_t[nLen+1];
		wcsncpy(m_wszBuf, lpwsz, nLen);
		m_wszBuf[nLen] = L'\0';
	}

	CSafeString(LPCWSTR lpwsz) : m_szBuf(NULL)
	{
		assert(lpwsz);
		int n = wcslen(lpwsz);
		m_wszBuf = new wchar_t[n+1];
		wcscpy(m_wszBuf, lpwsz);
	}

	operator LPCSTR() { return GetAnsi();}
	operator LPCWSTR() { return GetUnicode(); }
	
	CSafeString& operator= (const CSafeString& str)
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;

		m_szBuf = NULL;
		m_wszBuf = NULL;

		if (str.m_szBuf)
		{
			m_szBuf = new char[strlen(str.m_szBuf)+1];
			strcpy(m_szBuf, str.m_szBuf);
		}

		if (str.m_wszBuf)
		{
			m_wszBuf = new wchar_t[wcslen(str.m_wszBuf)+1];
			wcscpy(m_wszBuf, str.m_wszBuf);
		}
		return *this;
	}

	LPCSTR GetAnsi()
	{
		if (m_szBuf) return m_szBuf;
		assert(m_wszBuf);
		
		int nCount = WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			NULL,
			0,
			NULL,
			NULL);
	
		m_szBuf = new char[nCount];

		WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			m_szBuf,
			nCount,
			NULL,
			NULL);
		return m_szBuf;
	}
	
	LPCWSTR GetUnicode()
	{
		if (m_wszBuf) return m_wszBuf;
		assert(m_szBuf);
		
		int nCount = MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			NULL,
			0);

		m_wszBuf = new wchar_t[nCount];

		MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			m_wszBuf,
			nCount);

		return m_wszBuf;
	}

	virtual ~CSafeString() 
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;
	}
};

#endif
