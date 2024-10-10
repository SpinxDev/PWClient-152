/*
 * FILE: A3DPixelShader.cpp
 *
 * DESCRIPTION: Routines for pixel shader
 *
 * CREATED BY: duyuxin, 2002/3/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPixelShader.h"
#include "A3DPI.h"
#include "A3DDevice.h"
#include "AFileImage.h"
#include "AFI.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


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
//	Implement A3DPixelShader
//
///////////////////////////////////////////////////////////////////////////

A3DPixelShader* A3DPixelShader::m_pCurShader = NULL;

A3DPixelShader::A3DPixelShader()
{
	m_dwClassID			= A3D_CID_PIXELSHADER;
	m_pA3DDevice		= NULL;
	m_pD3DXShaderBuf	= NULL;
	m_pD3DShader		= NULL;
	m_dwShaderID		= 0;
	m_dwPosInMan		= 0;
}

A3DPixelShader::~A3DPixelShader()
{
}

/*	Initialize object

	Return true for success, otherwise return false;
*/
bool A3DPixelShader::Init(A3DDevice* pDevice)
{
	m_pA3DDevice = pDevice;
	return true;
}

//	Release object
void A3DPixelShader::Release()
{
	if (m_pD3DShader)
	{
		//m_pD3DShader->Release();
		if( m_pA3DDevice)
			m_pA3DDevice->ReleaseResource(m_pD3DShader); //multiThread render

		m_pD3DShader = NULL;
	}

	if (m_pD3DXShaderBuf)
	{
		//m_pD3DXShaderBuf->Release();
		if(m_pA3DDevice)
			m_pA3DDevice->ReleaseResource(m_pD3DXShaderBuf); //multiThread render
		m_pD3DXShaderBuf = NULL;
	}
}

/*	Load pixel shader from file

	Return true for success, otherwise return false;

	szFile: file name
	bBinary: true, load binary pixel file. false, load ascii pixel file
*/
bool A3DPixelShader::Load(const char* szFile, bool bBinary)
{
	char szFullFileName[MAX_PATH];
	af_GetFullPath(szFullFileName, szFile);

	HRESULT hr;

	if (bBinary)
	{
		AFileImage File;

		if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		{
			File.Close();
			g_A3DErrLog.Log("A3DPixelShader::Load, Cannot open file %s!", szFullFileName);
			return false;
		}

		if (D3DXCreateBuffer(File.GetFileLength(), &m_pD3DXShaderBuf) != D3D_OK)
		{
			File.Close();
		//	g_A3DErrLog.Log("A3DPixelShader::Load, Failed to create D3DXBuffer!");
			return false;
		}

		//	Copy file data
		memcpy(m_pD3DXShaderBuf->GetBufferPointer(), File.GetFileBuffer(), File.GetFileLength());

		File.Close();
	}
	else	//	Load ascii file
	{
		AFileImage File;

		if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		{
			File.Close();
			g_A3DErrLog.Log("A3DPixelShader::Load, Cannot open file %s!", szFullFileName);
			return false;
		}

		LPD3DXBUFFER lpErrorBuf = NULL;
		if ((hr = D3DXAssembleShader((LPCSTR)File.GetFileBuffer(), File.GetFileLength(), NULL, NULL,0, &m_pD3DXShaderBuf, &lpErrorBuf)) != D3D_OK)
		{
			File.Close();
#ifdef _DEBUG
			g_A3DErrLog.Log("A3DPixelShader::Load, Failed to load pixel shader. \n%s", lpErrorBuf->GetBufferPointer());
#endif // _DEBUG
			if(lpErrorBuf != NULL)
			{
				lpErrorBuf->Release();
				lpErrorBuf = NULL;
			}
			return false;
		}
		if(lpErrorBuf != NULL)
		{
			lpErrorBuf->Release();
			lpErrorBuf = NULL;
		}
		File.Close();
	}

	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	if ((hr = pD3DDevice->CreatePixelShader((DWORD*)m_pD3DXShaderBuf->GetBufferPointer(),
											&m_pD3DShader)) != D3D_OK)
	{
	//	g_A3DErrLog.Log("A3DPixelShader::Load, Failed to create pixel shader.");
		return false;
	}

	//	Record file name
	af_GetRelativePath(szFullFileName, m_strFileName);
	m_dwShaderID = a_MakeIDFromLowString(m_strFileName);

	//	After DX 8.1, pixel shaders needn't to be recreated when device reset, 
	//	they will be remebered by DX automatically.	so we release pixel buffer here.
	m_pD3DXShaderBuf->Release();
	m_pD3DXShaderBuf = NULL;

	return true;
}

/*	Apply pixel shader

	Return true for success, otherwise return false.
*/
bool A3DPixelShader::Appear()
{
	if (!m_pD3DShader)
		return false;

	if (m_pCurShader == this)
		return true;

	//m_pA3DDevice->SetHLSLShader(NULL);
	if (FAILED(m_pA3DDevice->raw_SetPixelShader(m_pD3DShader)))
	{
		g_A3DErrLog.Log("A3DPixelShader::Appear, Failed to apply pixel shader.");
		return false;
	}

	m_pCurShader = this;

	return true;
}

//	Abolish pixel shader
bool A3DPixelShader::Disappear()
{
	if (m_pCurShader == this)
	{
		m_pA3DDevice->raw_SetPixelShader(NULL);
		m_pCurShader = NULL;
	}

	return true;
}

