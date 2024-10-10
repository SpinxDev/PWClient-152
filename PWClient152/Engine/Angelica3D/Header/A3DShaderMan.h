/*
 * FILE: A3DShaderMan.h
 *
 * DESCRIPTION: Routines used to management shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSHADERMAN_H_
#define _A3DSHADERMAN_H_

#include "A3DVertex.h"
#include "AList2.h"
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

class A3DShader;
class A3DPixelShader;
class A3DVertexShader;
class A3DDevice;
class A3DEngine;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DShaderMan
//
///////////////////////////////////////////////////////////////////////////

class A3DShaderMan
{
public:		//	Types

	struct SHADERNODE
	{
		DWORD	dwRefCnt;	//	Reference count
		void*	pShader;	//	Shader object
	};

public:		//	Constructors and Destructors

	A3DShaderMan();
	virtual ~A3DShaderMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release
	void Release();

	//	Load shader file
	A3DShader* LoadShaderFile(const char* szFileName);
	//	Release shader
	void ReleaseShader(A3DShader** ppShader);

	//	Load pixel shader
	A3DPixelShader* LoadPixelShader(const char* szFile, bool bBinary);
	//	Release pixel shader 
	void ReleasePixelShader(A3DPixelShader** ppShader);

	//	Load vertex shader
	A3DVertexShader* LoadVertexShader(const char* szFile, bool bBinary, D3DVERTEXELEMENT9* pdwDecl);
	A3DVertexShader* LoadVertexShader(const char* szFile, bool bBinary, A3DVERTEXTYPE VertexType, int iStreamIdx=0);
	A3DVertexShader* LoadVertexShader(const char* szFile, bool bBinary, DWORD dwFVF);
	//	Release vertex shader
	void ReleaseVertexShader(A3DVertexShader** ppShader);

	//	Set shader folder name
	void SetFolderName(char* szFolderName) { m_strFolderName = szFolderName; }
	//	Get shader folder name
	const char* GetFolderName() { return m_strFolderName; }

	//	Get pixel shader number
	int GetPixelShaderNum() { m_PSList.GetCount(); }
	//	Get vertex shader number
	int GetVertexShaderNum() { m_VSList.GetCount(); }

protected:	//	Attributes

	A3DEngine*	m_pA3DEngine;		
	A3DDevice*	m_pA3DDevice;
	AString		m_strFolderName;

	CRITICAL_SECTION		m_csPS;
	CRITICAL_SECTION		m_csVS;
	APtrList<SHADERNODE*>	m_PSList;	//	Pixel shader list
	APtrList<SHADERNODE*>	m_VSList;	//	Vertex shader list

protected:	//	Operations

	//	Search a pixel shader with specified name
	ALISTPOSITION FindPixelShader(const char* szFile);
	//	Search a vertex shader with specified name
	ALISTPOSITION FindVertexShader(const char* szFile);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSHADERMAN_H_


