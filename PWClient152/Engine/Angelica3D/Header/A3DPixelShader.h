/*
 * FILE: A3DPixelShader.h
 *
 * DESCRIPTION: Routines for pixel shader
 *
 * CREATED BY: duyuxin, 2002/3/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DPIXELSHADER_H_
#define _A3DPIXELSHADER_H_

#include "A3DTypes.h"
#include "A3DDevObject.h"
#include "AString.h"
#include "A3DPlatform.h"

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

class A3DDevice;
	
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DPixelShader
//
///////////////////////////////////////////////////////////////////////////

class A3DPixelShader : public A3DDevObject
{
public:		//	Types

	friend class A3DShaderMan;

public:		//	Constructors and Destructors

	A3DPixelShader();
	virtual ~A3DPixelShader();

public:		//	Attributes

	static A3DPixelShader*	m_pCurShader;	//	Current applied pixel shader

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pDevice);
	//	Release object
	void Release();	

	//	Load pixel shader from file
	bool Load(const char* szFile, bool bBinary);

	//	Apply this pixel shader
	bool Appear();
	//	Abolish pixel shader
	bool Disappear();

	//	Before device reset
	virtual bool BeforeDeviceReset() { return true; }
	//	After device reset
	virtual bool AfterDeviceReset() { return true; }

	//	Get pixel shader file name
	const char* GetFileName() { return m_strFileName; }
	//	Get pixel shader ID
	DWORD GetShaderID() { return m_dwShaderID; }

protected:	//	Attributes

	A3DDevice*					m_pA3DDevice;			//	Device pointer
	AString						m_strFileName;			//	File name relative to shader path
	LPD3DXBUFFER				m_pD3DXShaderBuf;		//	Pixel shader code buffer
	IDirect3DPixelShader9*		m_pD3DShader;			//	D3D Pixel shader handle
	DWORD						m_dwShaderID;			//	Pixel shader ID
	DWORD						m_dwPosInMan;			//	Position in manager

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DPIXELSHADER_H_


