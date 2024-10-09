/*
 * FILE: Render.h
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2004/4/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_RENDER_H_
#define _RENDER_H_

#ifdef AUTO_TERRAIN_EDITOR
#include <WTypes.h>
#include "AChar.h"
#include <ABaseDef.h>
#include <ARect.h>
#include <A3DVector.h>
#include <AString.h>
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////
#ifdef AUTO_TERRAIN_EDITOR
const float ORTHO_RANGE			= 3000.0f;
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;	
class A3DLight;	
class A3DCamera;
class A3DOrthoCamera;
class A3DViewport;
#ifdef AUTO_TERRAIN_EDITOR
class A3DFont;
class A3DStringSet;
#else
class CECViewport;
#endif
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CRender
//
///////////////////////////////////////////////////////////////////////////

class CRender
{
public:		//	Types

public:		//	Constructor and Destructor

	CRender();
	virtual ~CRender() {}

public:		//	Attributes

public:		//	Operations

#ifdef AUTO_TERRAIN_EDITOR
	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd);
	//	Release game
	void Release();
	//	Resize device
	bool ResizeDevice(int iWid, int iHei);
	//	Begin render
	bool BeginRender();
	//	End render
	bool EndRender();
	//	Output text
	void TextOut(int x, int y, const ACHAR* szText, DWORD color);
	//	Draw 2D rectangle
	void Draw2DRect(const ARectI& rc, DWORD dwCol, bool bFrame);
	//	Set ambient color and diretional light's direction and color
	void ChangeLight(const A3DVECTOR3& vDir, DWORD dwDirCol, DWORD dwAmbient);
	
	A3DOrthoCamera* GetA3DOrthoCamera() { return m_pA3DOrthoCamera; }
	A3DViewport* GetA3DViewport() { return m_pA3DViewport; }
	A3DCamera* GetA3DCamera() { return m_pA3DCamera; }
#else
	//	Initialize game
	bool Init();
	CECViewport* GetECViewport() { return m_pECViewport; }
	A3DCamera* GetA3DCamera();
	A3DOrthoCamera* GetA3DOrthoCamera();
	A3DViewport* GetA3DViewport();
#endif
	
	//	Get interface
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() {	return m_pA3DDevice; }
	A3DLight* GetDirLight() { return m_pDirLight; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;		//	A3D engine
	A3DDevice*		m_pA3DDevice;		//	A3D device
	A3DLight*		m_pDirLight;		//	Directional light

#ifdef AUTO_TERRAIN_EDITOR
	A3DCamera*		m_pA3DCamera;		//	A3D camera
	A3DOrthoCamera* m_pA3DOrthoCamera;	//  A3D ortho camear;
	A3DViewport*	m_pA3DViewport;		//	A3D viewport
	A3DFont*		m_pFont;			//	Font object

	HINSTANCE		m_hInstance;		//	Handle of instance
	HWND			m_hRenderWnd;		//	Render window's handle
	int				m_iRenderWid;		//	Render size
	int				m_iRenderHei;

	bool			m_bEngineOK;		//	Engine is ready
#else
	CECViewport*	m_pECViewport;		//	ECViewport
#endif
	
protected:	//	Operations

#ifdef AUTO_TERRAIN_EDITOR
	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
#endif
};

extern CRender	g_Render;

#ifdef AUTO_TERRAIN_EDITOR
///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////
inline bool glb_CreateDirectory(const char* szDir)
{
	AString strDir = szDir;
	int iLen = strDir.GetLength();

	if (iLen <= 3)
		return true;

	//	Clear the last '\\'
	if (strDir[iLen-1] == '\\')
		strDir[iLen-1] = '\0';

	//	Save current directory
	char szCurDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurDir);

	AString strTemp;

	int iPos = strDir.Find('\\', 0);
	while (iPos > 0)
	{
		strTemp = strDir.Mid(0, iPos);
		CreateDirectoryA(strTemp, NULL);
		iPos = strDir.Find('\\', iPos+1);
	}

	CreateDirectoryA(szDir, NULL);

	//	Restore current directory
	SetCurrentDirectoryA(szCurDir);

	return true;
}

inline bool glb_WriteString(FILE* fp, const char* str)
{
	ASSERT(str);

	//	Write length of string
	int iLen = strlen(str);
	fwrite(&iLen, 1, sizeof (int), fp);

	//	Write string data
	if (iLen)
		fwrite(str, 1, iLen, fp);

	return true;
}

//	Change file's extension
inline void glb_ChangeExtension(char* szFile, char* szNewExt)
{
	char* pTemp = strrchr(szFile, '.');
	if (!pTemp)
	{
		strcat(szFile, ".");
		strcat(szFile, szNewExt);
	}
	else
	{
		strcpy(pTemp+1, szNewExt);
	}
}
#endif

#endif	//	_RENDER_H_
