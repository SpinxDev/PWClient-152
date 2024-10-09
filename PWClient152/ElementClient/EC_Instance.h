/*
 * FILE: EC_Instance.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/8/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AString.h"
#include "AAssist.h"
#include <ARect.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class AWScriptFile;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECInstance
//	
///////////////////////////////////////////////////////////////////////////

class CECInstance
{
public:		//	Types
	typedef std::vector<AString> AStringArray;

public:		//	Constructor and Destructor

	CECInstance();

public:		//	Attributes

public:		//	Operations

	//	Load instance information from file
	bool Load(AWScriptFile* psf);

	//	Get instance ID
	int GetID() const { return m_id; }
	//	Get instance name
	const ACHAR* GetName() const { return m_strName; }
	//	Get instance data path
	const char* GetPath() const { return m_strPath; }
	//	Get row and column number of map
	int GetRowNum()const{ return m_iRowNum; }
	int GetColNum()const{ return m_iColNum; }
	bool GetLimitJump()const{ return m_bLimitJump; }
	const AStringArray & GetRouteFiles()const{ return m_routeFiles; }
	bool GetPositionRelatedTexture(float x, float z, AString &filePath)const;

protected:	//	Attributes

	int			m_id;		//	Instance ID
	ACString	m_strName;	//	Instance name
	AString		m_strPath;	//	Path
	int			m_iRowNum;	//	Number of map row
	int			m_iColNum;	//	Number of map column
	bool		m_bLimitJump;	//	是否限跳

	AStringArray	m_routeFiles;

	struct PositionRelatedTexture{
		ARectF	rect;
		AString	filePath;
	};
	typedef std::vector<PositionRelatedTexture>	PositionRelatedTextureArray;
	PositionRelatedTextureArray	m_positionRelatedTextures;	//	M图根据位置显示不同图

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

