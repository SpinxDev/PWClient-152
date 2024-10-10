/********************************************************************
	created:	2006/07/10
	author:		kuiwu

	purpose:    an interface, manage a lua script file	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#pragma  once

#include <hashmap.h>

#ifndef MAX_PATH
#define MAX_PATH          260
#endif



#define	  MD5_LEN		16

class CMd5Hash
{
public:
	CMd5Hash();
	
	static void Digest(const unsigned char * input, unsigned int inputlen,
						unsigned char * output, unsigned int& outputlen);

private:
	void init();
	void transform (const unsigned char block[64]);
	void update(const unsigned char *input, unsigned int inputlen);

	void  final(unsigned char * output, unsigned int& outputlen);

	unsigned int state[4];
	unsigned int count[2];
	unsigned char buffer[64];

};


typedef void (* LuaModifyHandler) (const char * name, const char * buf, int len);

// void   LuaFileModifyHandler(const char * name, const char * buf, int len);



class CLuaState;

class CLuaScript  
{
public:
	enum
	{
		EXECUTE_FAIL,
		EXECUTE_SUCCEED,
		EXECUTE_UNCHANGE,
	};
	CLuaScript();
	CLuaScript(CLuaState * state);
	CLuaScript(const CLuaScript& scr);
	
	virtual ~CLuaScript();
	
	virtual CLuaScript * Clone() const;

	bool		FromFile(const char * fileName);
	bool		FromBuffer(const char * name, const char * buf, int bufLen);

	CLuaState *  GetLuaState() const { return m_pLuaState;}
	const char * GetName() const{return m_szName;}
	void         SetLuaState(CLuaState * state)	{m_pLuaState = state;}
	void		 SetModifyHandler(LuaModifyHandler  handler){m_ModifyHandler = handler;}
	LuaModifyHandler  GetModifyHandler() const {return m_ModifyHandler;}
	int          GetRefCount() const {return m_RefCount;}
	void         IncRefCount() { ++m_RefCount;}
	void         DecRefCount() { --m_RefCount;}
	
	
	//execute the script,
	virtual int Execute();

	int RetrieveBuf(char * buf);

	void ToggleBreakPoints(bool isAdd, int lineNum);	// toggle break point, isAdd = true for add, otherwise delete.

	bool hasBreakPoint(int lineNum);		// check whether line of lineNum has break point
	
protected:
	CLuaState * m_pLuaState;
	char        m_szName[MAX_PATH];  //path to lua file
	char      * m_pBuf;
	int         m_BufLen;
	int         m_RefCount;

	LuaModifyHandler  m_ModifyHandler;

	abase::hash_map<int, int> m_breakPoints;	// break points of this script, linenum -> bool 

	void        Release();
};


