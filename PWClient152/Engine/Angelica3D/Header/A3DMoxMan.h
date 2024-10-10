/*
 * FILE: A3DMoxMan.h
 *
 * DESCRIPTION: Managing Class for frame objects
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMOXMAN_H_
#define _A3DMOXMAN_H_

#include "A3DTypes.h"
#include "AList.h"
#include "AString.h"

class A3DDevice;
class A3DFrame;

class A3DMoxMan
{
public:		//	Types

	struct MOXRECORD
	{
		AString		strFileName;
		A3DFrame*	pFrame;
	};

private:
	
	AString		m_strFolderName;
	A3DDevice*  m_pA3DDevice;
	AList	    m_ListMox;

	bool FindMoxFile(const char * szFilename, A3DFrame ** ppFrame);
public:
	A3DMoxMan();
	virtual ~A3DMoxMan();

	bool Init(A3DDevice * pDevice);
	bool Release();
	bool Reset();

	bool LoadMoxFile(const char * szFilename, A3DFrame ** ppFrame);
	bool ReleaseFrame(A3DFrame *& pFrame);
	bool DuplicateFrame(A3DFrame * pOrgFrame, A3DFrame ** ppNewFrame);
					
	inline void SetFolderName(const char* szFolderName) { m_strFolderName = szFolderName; }
	inline const char* GetFolderName() { return m_strFolderName; }
	inline int GetMoxCount() { return m_ListMox.GetSize(); }
};

#endif