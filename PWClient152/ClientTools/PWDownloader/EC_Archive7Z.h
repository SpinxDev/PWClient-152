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
		OP_GETTOTAL,		// 获取当前进度总量
		OP_GETCOMPLETED,	// 获取当前进度
		OP_GETCURFILE,		// 获取当前解压的文件
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
	// 销毁对象
	virtual void Release() = 0;

	// 设置解压回调函数
	virtual void SetExtractCB(ExtractCallback pfnCallback) = 0;

	// 加载7z压缩包
	// Param: 压缩包的路径
	// Param: 压缩包的密码
	virtual bool LoadPack(const char* filename, const char* password) = 0;

	// 解压所有文件到指定目录
	virtual bool ExtractTo(const char* szDestDir) = 0;

	// 卸载压缩包
	virtual void Unload() = 0;
};

// 创建一个7z包对象
IArchive7Z* CreateArchive7Z();

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
