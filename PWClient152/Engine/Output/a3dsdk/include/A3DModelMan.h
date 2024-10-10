/*
 * FILE: A3DModelMan.h
 *
 * DESCRIPTION: Managing Class for model objects
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMODELMAN_H_
#define _A3DMODELMAN_H_

#include "A3DTypes.h"
#include "AList.h"
#include "AString.h"

class A3DModelCollector;
class A3DModel;
class A3DDevice;

class A3DModelMan
{
public:		//	Types

	struct MODELRECORD
	{
		AString		strFileName;
		A3DModel*	pModel;
	};

private:
	
	AString		m_strFolderName;
	A3DDevice*	m_pA3DDevice;
	AList		m_ListModel;

	A3DModelCollector*	m_pModelCollector;

protected:
	bool FindModelFile(const char * szFilename, A3DModel ** ppFrame);

public:
	A3DModelMan();
	~A3DModelMan();

	bool Init(A3DDevice * pDevice);
	bool Release();
	bool Reset();

	bool LoadModelFile(const char* szFilename, A3DModel** ppModel, bool bChild=false);
	bool ReleaseModel(A3DModel*& pModel);

	inline void SetFolderName(const char * szFolderName) { m_strFolderName = szFolderName; }
	inline const char* GetFolderName() { return m_strFolderName; }
	inline int GetModelCount() { return m_ListModel.GetSize(); }
};

typedef A3DModelMan * PA3DModelMan;

#endif