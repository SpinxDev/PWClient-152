/*
 * FILE: A3DSurfaceMan.h
 *
 * DESCRIPTION: Managing class for surface object
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSURFACE_MAN_H_
#define _A3DSURFACE_MAN_H_

#include "A3DTypes.h"
#include "AString.h"

class A3DSurface;
class A3DDevice;

class A3DSurfaceMan
{
private:
	A3DDevice*	m_pA3DDevice;
	AString		m_strFolderName;

public:
	A3DSurfaceMan();
	virtual ~A3DSurfaceMan();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();
	bool Reset();

	bool LoadCursorSurfaceFromFile(int nWidth, int nHeight, const char* szFileName, A3DCOLOR colorKey, A3DSurface** ppSurface);
	bool LoadCursorSurfaceFromFile(const char* szFileName, A3DCOLOR colorKey, A3DSurface** ppSurface);

	bool LoadSurfaceFromFile(int nWidth, int nHeight, const char* szFileName, A3DCOLOR colorKey, A3DSurface** ppA3DSurface);
	bool LoadSurfaceFromFile(const char* szFileName, A3DCOLOR colorKey, A3DSurface** ppA3DSurface);

	bool LoadSurfaceFromFileInMemory(int nWidth, int nHeight, const char* szFileName, const void * pMemFile, DWORD dwFileLength, A3DCOLOR colorKey, A3DSurface** ppA3DSurface);
	bool LoadSurfaceFromFileInMemory(const char* szFileName, const void * pMemFile, DWORD dwFileLength, A3DCOLOR colorKey, A3DSurface** ppA3DSurface);
	bool ReleaseSurface(A3DSurface*& pSurface);

	void SetFolderName(const char* szFolderName) { m_strFolderName = szFolderName; }
};

typedef A3DSurfaceMan * PA3DSurfaceMan;

#endif
