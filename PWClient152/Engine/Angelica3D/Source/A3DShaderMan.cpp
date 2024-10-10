/*
 * FILE: A3DShaderMan.cpp
 *
 * DESCRIPTION: Routines used to management shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DShaderMan.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DShader.h"
#include "A3DVertexShader.h"
#include "A3DPixelShader.h"
#include "A3DFuncs.h"
#include "AFileImage.h"
#include "AFI.h"
#include "AMemory.h"
#include "ACSWrapper.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DShaderMan
//
///////////////////////////////////////////////////////////////////////////

A3DShaderMan::A3DShaderMan()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_strFolderName	= "Shaders\\2.2";

	InitializeCriticalSection(&m_csPS);
	InitializeCriticalSection(&m_csVS);
}

A3DShaderMan::~A3DShaderMan()
{
	DeleteCriticalSection(&m_csPS);
	DeleteCriticalSection(&m_csVS);
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pA3DEngine: address of A3D engine object
*/
bool A3DShaderMan::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);

	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();

	return true;
}

//	Release
void A3DShaderMan::Release()
{
	ACSWrapper csa(&m_csPS);

	//	Release all pixel shader
	ALISTPOSITION pos = m_PSList.GetHeadPosition();
	while (pos)
	{
		SHADERNODE* pNode = m_PSList.GetNext(pos);
		A3DPixelShader* pShader = (A3DPixelShader*)pNode->pShader;
		pShader->Release();
		delete pShader;
		delete pNode;
	}

	m_PSList.RemoveAll();

	csa.Detach(true);

	csa.Attach(&m_csVS);

	//	Release all vertex shader
	pos = m_VSList.GetHeadPosition();
	while (pos)
	{
		SHADERNODE* pNode = m_VSList.GetNext(pos);
		A3DVertexShader* pShader = (A3DVertexShader*)pNode->pShader;
		pShader->Release();
		delete pShader;
		delete pNode;
	}

	m_PSList.RemoveAll();

	csa.Detach(true);

	m_pA3DDevice = NULL;
}

/*	Load shader file.

	Return shader object's address for success, otherwise return NULL.

	szFileName: shader file's name relative to shader folder
*/
A3DShader* A3DShaderMan::LoadShaderFile(const char* szFileName)
{
	AFileImage File;
	A3DShader* pShader;
	char szShaderTitle[MAX_PATH];
	
	af_GetFileTitle(szFileName, szShaderTitle, MAX_PATH);
	if (!(pShader = new A3DShader()))
	{
		g_A3DErrLog.Log("A3DShaderMan::LoadShaderFile, not enough memory");
		return NULL;
	}

	if (!pShader->Init(m_pA3DEngine))
	{
		delete pShader;
		g_A3DErrLog.Log("A3DShaderMan::LoadShaderFile, failed to initialize shader object");
		return NULL;
	}
	
	if (!File.Open(m_strFolderName, szShaderTitle, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		//	No shader file was found, only output a error log and continue
		//	We should not release pShader, because we must use it without effects
		g_A3DErrLog.Log("A3DShaderMan::LoadShaderFile, [%s] can not be found", szShaderTitle);
		return pShader;
	}

	if (!pShader->Load(&File))
	{
		//	Load shader file fail, only output a error log and continue
		delete pShader;
		File.Close();
		g_A3DErrLog.Log("A3DShaderMan::LoadShaderFile, failed to load shader %s", szShaderTitle);
        pShader = NULL;
	}

	File.Close();
	
	return pShader;
}

/*	Release A3D shader.

	ppShader: address of A3DShader object's pointer
*/
void A3DShaderMan::ReleaseShader(A3DShader** ppShader)
{
	A3DShader* pShader = *ppShader;

	if (pShader)
	{
		pShader->Release();
		delete pShader;
		*ppShader = NULL;
	}
}

/*	Search a pixel shader with specified name.

	Return shader's position in list.

	szFile: pixel shader file name used to compare
*/
ALISTPOSITION A3DShaderMan::FindPixelShader(const char* szFile)
{
	DWORD dwID = a_MakeIDFromLowString(szFile);

	ACSWrapper csa(&m_csPS);

	ALISTPOSITION pos = m_PSList.GetHeadPosition();
	while (pos)
	{
		SHADERNODE* pNode = m_PSList.GetAt(pos);
		A3DPixelShader* pShader = (A3DPixelShader*)pNode->pShader;
		if (pShader->GetShaderID() == dwID)
			return pos;

		m_PSList.GetNext(pos);
	}

	return 0;
}

//	Search a vertex shader with specified name
ALISTPOSITION A3DShaderMan::FindVertexShader(const char* szFile)
{
	DWORD dwID = a_MakeIDFromLowString(szFile);

	ACSWrapper csa(&m_csVS);

	ALISTPOSITION pos = m_VSList.GetHeadPosition();
	while (pos)
	{
		SHADERNODE* pNode = m_VSList.GetAt(pos);
		A3DVertexShader* pShader = (A3DVertexShader*)pNode->pShader;
		if (pShader->GetShaderID() == dwID)
			return pos;

		m_VSList.GetNext(pos);
	}

	return 0;
}

/*	Load pixel shader from file

	Return pixel shader object's address for success, otherwise return NULl.
	
	szFile: absolute or relative file name of shader
	bBinary: true, load binary pixel file. false, load ascii pixel file
*/
A3DPixelShader* A3DShaderMan::LoadPixelShader(const char* szFile, bool bBinary)
{
	if (!szFile || !szFile[0])
	{
		g_A3DErrLog.Log("A3DShaderMan::LoadPixelShader, invalid file name");
		return NULL;
	}

	AString strFullFile, strRelativeFile;
	af_GetFullPath(strFullFile, szFile);
	af_GetRelativePath(strFullFile, strRelativeFile);

	ACSWrapper csa(&m_csPS);

	ALISTPOSITION pos = FindPixelShader(strRelativeFile);
	if (pos)
	{
		//	Shader with same name has been found
		SHADERNODE* pNode = m_PSList.GetAt(pos);
		pNode->dwRefCnt++;
		return (A3DPixelShader*)pNode->pShader;
	}

	A3DPixelShader* pNewShader = new A3DPixelShader;
	if (!pNewShader)
	{
		g_A3DErrLog.Log("A3DShaderMan::LoadPixelShader, Not enough memory!");
		return NULL;
	}

	if (!pNewShader->Init(m_pA3DDevice))
	{
		delete pNewShader;
		g_A3DErrLog.Log("A3DShaderMan::LoadPixelShader, Failed to initialize pixel shader!");
		return NULL;
	}

	//	Load pixel shader from file
	if (!pNewShader->Load(strRelativeFile, bBinary))
	{
		delete pNewShader;
	//	g_A3DErrLog.Log("A3DShaderMan::LoadPixelShader, Fail to load pixel shader from file %s!", strFullFile);
		return NULL;
	}

	//	Add new node to list
	SHADERNODE* pNode = new SHADERNODE;
	if (!pNode)
	{
		pNewShader->Release();
		delete pNewShader;
		return NULL;
	}

	pNode->dwRefCnt = 1;
	pNode->pShader	= pNewShader;

	pNewShader->m_dwPosInMan = (DWORD)m_PSList.AddTail(pNode);

	return pNewShader;
}

/*	Release a pixel shader 

	Return true for success, otherwise return false.

	ppShader: pixel shader which will be released
*/
void A3DShaderMan::ReleasePixelShader(A3DPixelShader** ppShader)
{
	A3DPixelShader* pShader = *ppShader;
	if (!pShader || !pShader->m_dwPosInMan)
		return;

	ALISTPOSITION pos = (ALISTPOSITION)pShader->m_dwPosInMan;

	ACSWrapper csa(&m_csPS);

	SHADERNODE* pNode = m_PSList.GetAt(pos);
	if (!(--pNode->dwRefCnt))
	{
		//	Release shader
		pShader->Release();
		delete pShader;

		//	Remove node
		delete pNode;
		m_PSList.RemoveAt(pos);
	}

	*ppShader = NULL;
}

//	Load vertex shader
A3DVertexShader* A3DShaderMan::LoadVertexShader(const char* szFile, bool bBinary, D3DVERTEXELEMENT9* pdwDecl)
{
	if (!szFile || !szFile[0])
	{
		g_A3DErrLog.Log("A3DShaderMan::LoadVertexShader, invalid file name");
		return NULL;
	}

	AString strFullFile, strRelativeFile;
	af_GetFullPath(strFullFile, szFile);
	af_GetRelativePath(strFullFile, strRelativeFile);

	ACSWrapper csa(&m_csVS);

	ALISTPOSITION pos = FindVertexShader(strRelativeFile);
	if (pos)
	{
		//	Shader with same name has been found
		SHADERNODE* pNode = m_VSList.GetAt(pos);
		pNode->dwRefCnt++;
		return (A3DVertexShader*)pNode->pShader;
	}

	A3DVertexShader* pNewShader = new A3DVertexShader;
	if (!pNewShader)
	{
		g_A3DErrLog.Log("A3DShaderMan::LoadVertexShader, Not enough memory!");
		return NULL;
	}

	if (!pNewShader->Init(m_pA3DDevice))
	{
		delete pNewShader;
		g_A3DErrLog.Log("A3DShaderMan::LoadVertexShader, Failed to initialize vertex shader!");
		return NULL;
	}

	//	Load vertex shader from file
	if (!pNewShader->Load(strRelativeFile, bBinary, pdwDecl))
	{
		delete pNewShader;
	//	g_A3DErrLog.Log("A3DShaderMan::LoadVertexShader, Fail to load vertex shader from file %s!", strFullFile);
		return NULL;
	}

	//	Add new node to list
	SHADERNODE* pNode = new SHADERNODE;
	if (!pNode)
	{
		pNewShader->Release();
		delete pNewShader;
		return NULL;
	}

	pNode->dwRefCnt = 1;
	pNode->pShader	= pNewShader;

	pNewShader->m_dwPosInMan = (DWORD)m_VSList.AddTail(pNode);

	return pNewShader;
}

A3DVertexShader* A3DShaderMan::LoadVertexShader(const char* szFile, bool bBinary, A3DVERTEXTYPE VertexType, int iStreamIdx/* 0 */)
{
	D3DVERTEXELEMENT9 aDecl[MAX_FVF_DECL_SIZE];
	a3d_MakeVSDeclarator(VertexType, iStreamIdx, aDecl);
	return LoadVertexShader(szFile, bBinary, aDecl);
}

A3DVertexShader* A3DShaderMan::LoadVertexShader(const char* szFile, bool bBinary, DWORD dwFVF)
{
	D3DVERTEXELEMENT9 aDecl[MAX_FVF_DECL_SIZE];
	a3d_MakeVSDeclarator(dwFVF, aDecl);
	return LoadVertexShader(szFile, bBinary, aDecl);
}

//	Release vertex shader
void A3DShaderMan::ReleaseVertexShader(A3DVertexShader** ppShader)
{
	A3DVertexShader* pShader = *ppShader;
	if (!pShader || !pShader->m_dwPosInMan)
		return;

	ALISTPOSITION pos = (ALISTPOSITION)pShader->m_dwPosInMan;

	ACSWrapper csa(&m_csVS);

	SHADERNODE* pNode = m_VSList.GetAt(pos);
	if (!(--pNode->dwRefCnt))
	{
		//	Release shader
		pShader->Release();
		delete pShader;

		//	Remove node
		delete pNode;
		m_VSList.RemoveAt(pos);
	}

	*ppShader = NULL;
}

