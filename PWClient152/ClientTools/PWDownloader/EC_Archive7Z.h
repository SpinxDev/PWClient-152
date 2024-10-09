/*
 * FILE: EC_Archive7Z.h
 *
 * DESCRIPTION: This class is used to extract files from *.7z
 *
 * CREATED BY: Shizhenhua, 2013/7/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <stdio.h>
#include <string>
#include <windows.h>
#include <assert.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  class CECArchive7Z
//  
///////////////////////////////////////////////////////////////////////////

class IArchive7Z
{
public:

	enum
	{
		OP_GETTOTAL,		// ��ȡ��ǰ��������
		OP_GETCOMPLETED,	// ��ȡ��ǰ����
		OP_GETCURFILE,		// ��ȡ��ǰ��ѹ���ļ�
	};

	struct PROPERTY
	{
		union
		{
			unsigned __int64 pi;
			char ps[MAX_PATH];
		};

		PROPERTY(unsigned __int64 p) { pi = p; }
		PROPERTY(const char* str) { strncpy(ps, str, MAX_PATH); }
	};

	typedef void (*ExtractCallback) (int op, PROPERTY& prop);

public:
	// ���ٶ���
	virtual void Release() = 0;

	// ���ý�ѹ�ص�����
	virtual void SetExtractCB(ExtractCallback pfnCallback) = 0;

	// ����7zѹ����
	// Param: ѹ������·��
	// Param: ѹ����������
	virtual bool LoadPack(const char* filename, const char* password) = 0;

	// ��ѹ�����ļ���ָ��Ŀ¼
	virtual bool ExtractTo(const char* szDestDir) = 0;

	// ж��ѹ����
	virtual void Unload() = 0;
};

// ����һ��7z������
IArchive7Z* CreateArchive7Z();

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
