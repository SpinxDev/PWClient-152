/*
 * FILE: A3DSkinModelMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSKINMODELMAN_H_
#define _A3DSKINMODELMAN_H_

#include "A3DPlatform.h"
#include "AArray.h"
#include "hashtab.h"
#include "AString.h"

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

class AFile;
class A3DSkinModel;
class A3DEngine;
class A3DDevice;
class A3DSkinModelActionCore;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinModelMan
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinModelMan
{
public:		//	Types

	//	Skin model share data
	struct SKINMODEL
	{
		DWORD		dwRefCnt;		//	Reference counter
		AString		strModelFile;	//	Model file
		DWORD		dwActEndOff;	//	Offset of action data in .smd file, used to accelate model loading speed

		APtrArray<A3DSkinModelActionCore*>	aActions;	//	Action data

		SKINMODEL()
		{
			dwRefCnt	= 0;
			dwActEndOff = 0;
		}
	};

	typedef abase::hashtab<SKINMODEL*, int, abase::_hash_function> ModelTable;

public:		//	Constructor and Destructor

	A3DSkinModelMan();
	virtual ~A3DSkinModelMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Tick animation
	bool TickAnimation();

	//	Load skin model data from file
	bool LoadModelFromFile(A3DSkinModel* pModel, AFile* pFile, int iSkinFlag);
	//	When a model is going to be released, this function is called
	void OnModelRelease(A3DSkinModel* pModel);

	//	Create a A3DSkinModelActionCore object
	A3DSkinModelActionCore* NewActionCore(int iFPS, const char* szName);
	//	Add an action to specified model (only used by SkeletonEditor)
	bool AddAction(A3DSkinModel* pModel, A3DSkinModelActionCore* pActionCore);
	//	Remove an action from specified model (only used by SkeletonEditor)
	void RemoveActionByName(A3DSkinModel* pModel, const char* szName);
	//	Enable / Disable action data cache
	void EnableActionCache(bool bEnable) { m_bUseActCache = bEnable; }
	//	Is action cache enabled ?
	bool IsActionCacheEnabled() { return m_bUseActCache; }

	//	Get A3D engine object
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	//	Get A3D device object
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }

protected:	//	Attributes
	
	A3DEngine*			m_pA3DEngine;	//	A3D engine object
	A3DDevice*			m_pA3DDevice;	//	A3D device object
	ModelTable			m_ModelTab;		//	Model table
	CRITICAL_SECTION	m_cs;			//	Skin model table lock

	DWORD	m_dwActCacheCnt;	//	Time counter used to check action cache
	DWORD	m_dwLastActCache;	//	Last time to check action cache
	bool	m_bUseActCache;		//	true, enable action data cache

protected:	//	Operations

	//	Search a skin model by it's file name and id
	SKINMODEL* SearchModel(const char* szFile, DWORD dwModelID);
	//	Release model node
	void ReleaseModelNode(SKINMODEL* pNode);
	//	Arrange outdated model actions
	void ArrangeOutdatedModelActions(DWORD dwCurTime);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSKINMODELMAN_H_
