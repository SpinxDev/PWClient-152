/*
 * FILE: EC_RTDebug.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AList2.h"
#include "AAssist.h"
#include <hashmap.h>
#include <set>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Color of run-time debug strings
enum
{
	RTDCOL_ERROR		= 0xffff0000,	//	Error
	RTDCOL_WARNING		= 0xffff8040,	//	Warning
	RTDCOL_NETWORK		= 0xff00ff00,	//	Network
	RTDCOL_CHATLOCAL	= 0xffffff00,	//	Local chat
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECViewport;
class A3DFont;

namespace GNET
{
	class Protocol;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECRTDebug
//	
///////////////////////////////////////////////////////////////////////////

class CECRTDebug
{
public:		//	Types

	//	Debug string
	struct DBGSTR
	{
		ACString	strMsg;		//	String content
		DWORD		dwCol;		//	String color
		int			iLine;		//	Line number from bottom to top
		int			iLevel;		//	Message level
	};

	typedef std::set<AString> HideProtoList;

public:		//	Constructor and Destructor

	CECRTDebug();
	virtual ~CECRTDebug();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport);

	//	Output a debug string
	void OutputDebugInfo(DWORD dwCol, const ACHAR* szMsg);
	//	Output a notify message
	void OutputNotifyMessage(DWORD dwCol, const ACHAR* szMsg);

	AString GetProtocolName(int iProtocol);
	AString GetGamedataSendName(int iCmd, bool bFromServer);
	AString GetProtocolName(const GNET::Protocol &p, bool bFromServer);

	//  Hide or show some protocols
	void HideProtocol(const AString& str);
	void ShowProtocol(const AString& str);
	bool IsProtocolHide(const GNET::Protocol& p, bool bFromServer);
	bool IsGameDataHide(int iCmd, bool bFromServer);
	const HideProtoList& GetHideProtos() const { return m_HideProtos; }

protected:	//	Attributes

	A3DFont*	m_pA3DFont;			//	A3D font object
	int			m_iMaxVisStr;		//	Maximum visible string number
	int			m_iLineSpace;		//	Line space

	DWORD		m_dwRTimeCnt;		//	Roll time counter
	DWORD		m_dwRTimeInter;		//	Time between two roll

	APtrList<DBGSTR*>	m_DbgStrList;	//	Debug strings

	typedef abase::hash_map<int, AString>	NameMap;
	NameMap mProtocol;
	NameMap mCGamedata;
	NameMap mSGamedata;

	HideProtoList m_HideProtos;		//  Some protocols do not need to show, such as 'OBJECT_MOVE'

	void InitPNameMap();
	void InitCGNameMap();
	void InitSGNameMap();
	void InitHideProto();

protected:	//	Operations

	//	Roll strings up
	void RollStringsUp(bool bClearCnt);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECException
//	
///////////////////////////////////////////////////////////////////////////

class CECException
{
public:		//	Types

	//	Exception type
	enum
	{
		TYPE_UNKNOWN = 0,
		TYPE_DATAERR,			//	Data error
		TYPE_OVERBOUND,			//	Over array or data buffer bound
	};

public:		//	Constructor and Destructor

	CECException(int iType)
	{
		m_iType = iType;
	}

public:		//	Attributes

public:		//	Operations

	int GetType() { return m_iType; }

protected:	//	Attributes

	int		m_iType;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECDataReader
//	
///////////////////////////////////////////////////////////////////////////

class CECDataReader
{
public:		//	Types

public:		//	Constructor and Destructor

	CECDataReader(void* pDataBuf, int iDataLen)
	{
		if (!pDataBuf || iDataLen <= 0)
		{
			throw CECException(CECException::TYPE_DATAERR);
		}

		m_pStart	= (BYTE*)pDataBuf;
		m_pEnd		= m_pStart + iDataLen;
		m_pCur		= (BYTE*)m_pStart;
	}

public:		//	Attributes

public:		//	Operations

	//	Offset reader
	void Offset(int iOffset, int iSeekFlag)
	{
		BYTE* pCur = NULL;

		switch (iSeekFlag)
		{
		case SEEK_SET:	pCur = m_pStart + iOffset;	break;
		case SEEK_CUR:	pCur = m_pCur + iOffset;	break;
		case SEEK_END:	pCur = m_pEnd + iOffset;	break;
		}

		m_pCur = pCur;

		BoundCheck(0);
	}

	//	Read a int data
	int Read_int() { BoundCheck(sizeof (int)); int r = *(int*)m_pCur; m_pCur += sizeof (int); return r; }
	//	Read a DWORD data
	DWORD Read_DWORD() { BoundCheck(sizeof (DWORD)); DWORD r = *(DWORD*)m_pCur; m_pCur += sizeof (DWORD); return r; }
	//	Read a float data
	float Read_float() { BoundCheck(sizeof (float)); float r = *(float*)m_pCur; m_pCur += sizeof (float); return r; }
	//	Read a short data
	short Read_short() { BoundCheck(sizeof (short)); short r = *(short*)m_pCur; m_pCur += sizeof (short); return r; }
	//	Read a WORD data
	WORD Read_WORD() { BoundCheck(sizeof (WORD)); WORD r = *(WORD*)m_pCur; m_pCur += sizeof (WORD); return r; }
	//	Read a char data
	char Read_char() { BoundCheck(sizeof (char)); char r = *(char*)m_pCur; m_pCur += sizeof (char); return r; }
	//	Read a BYTE data
	BYTE Read_BYTE() { BoundCheck(sizeof (BYTE)); BYTE r = *(BYTE*)m_pCur; m_pCur += sizeof (BYTE); return r; }
	//	Read a double data
	double Read_double() { BoundCheck(sizeof (double)); double r = *(double*)m_pCur; m_pCur += sizeof (double); return r; }
	//	Read a data block of specified size
	void* Read_Data(DWORD dwSize) { BoundCheck(dwSize); BYTE* r = m_pCur; m_pCur += dwSize; return r; }

protected:	//	Attributes

	BYTE*	m_pStart;	//	Data buffer start address
	BYTE*	m_pEnd;		//	Data buffer end address
	BYTE*	m_pCur;		//	Current data cursor

protected:	//	Operations

	//	Bound check
	void BoundCheck(DWORD dwSize)
	{
		if (m_pCur < m_pStart || m_pCur + dwSize > m_pEnd)
		{
			throw CECException(CECException::TYPE_OVERBOUND);
		}
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

