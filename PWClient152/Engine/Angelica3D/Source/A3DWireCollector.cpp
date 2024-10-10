/*
 * FILE: WireCollector.cpp
 *
 * DESCRIPTION: Wire object collector class
 *
 * CREATED BY: Duyuxin, 2003/10/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DWireCollector.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "AArray.h"
#include "AMemory.h"
#include <vector.h>

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement A3DWireCollector
//
///////////////////////////////////////////////////////////////////////////

A3DWireCollector::A3DWireCollector()
{
	m_pA3DEngine	= NULL;
	m_iVertBufSize	= 0;
	m_iIdxBufSize	= 0;

	memset(&m_3dBuf, 0, sizeof (m_3dBuf));
	memset(&m_2dBuf, 0, sizeof (m_2dBuf));
}

//	Initialize wire collector
bool A3DWireCollector::Init(A3DEngine* pA3DEngine, int iVertBufSize/* 2048 */, int iIdxBufSize/* 8192 */)
{
	m_pA3DEngine	= pA3DEngine;
	m_iVertBufSize	= iVertBufSize;
	m_iIdxBufSize	= iIdxBufSize;

	//	Create stream
	if (!(m_3dBuf.pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DWireCollector::Init, Failed to create stream object.");
		return false;
	}

	if (!(m_3dBuf.pA3DStream->Init(pA3DEngine->GetA3DDevice(), A3DVT_LVERTEX, m_iVertBufSize, 
		m_iIdxBufSize, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR)))
	{
		g_A3DErrLog.Log("A3DWireCollector::Init, Failed to create stream object.");
		return false;
	}

    m_3dBuf.aVertBufRef = new A3DLVERTEX[m_iVertBufSize];
    m_3dBuf.aIdxBufRef = new WORD[m_iIdxBufSize];

	//	Create stream
	if (!(m_2dBuf.pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DWireCollector::Init, Failed to create stream object.");
		return false;
	}

	if (!(m_2dBuf.pA3DStream->Init(pA3DEngine->GetA3DDevice(), A3DVT_TLVERTEX, m_iVertBufSize, 
		m_iIdxBufSize, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR)))
	{
		g_A3DErrLog.Log("A3DWireCollector::Init, Failed to create stream object.");
		return false;
	}

    m_2dBuf.aVertBufRef = new A3DTLVERTEX[m_iVertBufSize];
    m_2dBuf.aIdxBufRef = new WORD[m_iIdxBufSize];

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (pA3DDevice)
		pA3DDevice->AddUnmanagedDevObject(this);
	
	return true;
}

//	Release wire collector
void A3DWireCollector::Release()
{
	if (m_pA3DEngine)
	{
		A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
		if (pA3DDevice)
			pA3DDevice->RemoveUnmanagedDevObject(this);
	}

	if (m_3dBuf.pA3DStream)
	{
		if (m_3dBuf.aVertBuf)		//	Stream still be locked ! this shouldn't happen
			LockStream_3D(false);
	
		A3DRELEASE(m_3dBuf.pA3DStream);
        delete m_3dBuf.aVertBufRef;
        m_3dBuf.aVertBufRef = NULL;
        delete m_3dBuf.aIdxBufRef;
        m_3dBuf.aIdxBufRef = NULL;

	}

	if (m_2dBuf.pA3DStream)
	{
		if (m_2dBuf.aVertBuf)		//	Stream still be locked ! this shouldn't happen
			LockStream_2D(false);
	
		A3DRELEASE(m_2dBuf.pA3DStream);
        delete m_2dBuf.aVertBufRef;
        m_2dBuf.aVertBufRef = NULL;
        delete m_2dBuf.aIdxBufRef;
        m_2dBuf.aIdxBufRef = NULL;
	}

	m_pA3DEngine = NULL;

	memset(&m_3dBuf, 0, sizeof (m_3dBuf));
	memset(&m_2dBuf, 0, sizeof (m_2dBuf));
}

//	Before device reset
bool A3DWireCollector::BeforeDeviceReset()
{
	m_3dBuf.aIdxBuf		= NULL;
	m_3dBuf.aVertBuf	= NULL;
	m_3dBuf.iIdxCnt		= 0;
	m_3dBuf.iVertCnt	= 0;

	m_2dBuf.aIdxBuf		= NULL;
	m_2dBuf.aVertBuf	= NULL;
	m_2dBuf.iIdxCnt		= 0;
	m_2dBuf.iVertCnt	= 0;

	return true;
}

//	Lock/Unlock stream
bool A3DWireCollector::LockStream_3D(bool bLock)
{
    if (bLock)
    {
        //if (!m_3dBuf.pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**)&m_3dBuf.aVertBuf, 0))
        //	return false;

        //if (!m_3dBuf.pA3DStream->LockIndexBufferDynamic(0, 0, (BYTE**)&m_3dBuf.aIdxBuf, 0))
        //	return false;

        m_3dBuf.aVertBuf = m_3dBuf.aVertBufRef;
        m_3dBuf.aIdxBuf = m_3dBuf.aIdxBufRef;
        m_3dBuf.iVertCnt	= 0;
        m_3dBuf.iIdxCnt		= 0;
    }
    else
    {
        A3DLVERTEX* aVertBuf;
        if (!m_3dBuf.pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**)&aVertBuf, 0))
            return false;

        memcpy(aVertBuf, m_3dBuf.aVertBuf, sizeof(A3DLVERTEX) * m_iVertBufSize);

        if (!m_3dBuf.pA3DStream->UnlockVertexBufferDynamic())
            return false;

        WORD* aIdxBuf;
        if (!m_3dBuf.pA3DStream->LockIndexBufferDynamic(0, 0, (BYTE**)&aIdxBuf, 0))
            return false;

        memcpy(aIdxBuf, m_3dBuf.aIdxBuf, sizeof(WORD) * m_iIdxBufSize);


        if (!m_3dBuf.pA3DStream->UnlockIndexBufferDynamic())
            return false;

        m_3dBuf.aVertBuf	= NULL;
        m_3dBuf.aIdxBuf		= NULL;
    }

	return true;
}

//	Lock/Unlock stream
bool A3DWireCollector::LockStream_2D(bool bLock)
{
    if (bLock)
    {
        //if (!m_2dBuf.pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**)&m_2dBuf.aVertBuf, 0))
        //	return false;

        //if (!m_2dBuf.pA3DStream->LockIndexBufferDynamic(0, 0, (BYTE**)&m_2dBuf.aIdxBuf, 0))
        //	return false;

        m_2dBuf.aVertBuf = m_2dBuf.aVertBufRef;
        m_2dBuf.aIdxBuf = m_2dBuf.aIdxBufRef;

        m_2dBuf.iVertCnt	= 0;
        m_2dBuf.iIdxCnt		= 0;
    }
    else
    {
        A3DTLVERTEX* aVertBuf;
        if (!m_2dBuf.pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**)&aVertBuf, 0))
            return false;

        memcpy(aVertBuf, m_2dBuf.aVertBuf, sizeof(A3DTLVERTEX) * m_iVertBufSize);

        if (!m_2dBuf.pA3DStream->UnlockVertexBufferDynamic())
            return false;

        WORD* aIdxBuf;
        if (!m_2dBuf.pA3DStream->LockIndexBufferDynamic(0, 0, (BYTE**)&aIdxBuf, 0))
            return false;

        memcpy(aIdxBuf, m_2dBuf.aIdxBuf, sizeof(WORD) * m_iIdxBufSize);


        if (!m_2dBuf.pA3DStream->UnlockIndexBufferDynamic())
            return false;

        m_2dBuf.aVertBuf	= NULL;
        m_2dBuf.aIdxBuf		= NULL;
    }

	return true;
}

//	Add vertex and index to render buffer
bool A3DWireCollector::AddRenderData_3D(A3DVECTOR3* aVerts, int iNumVert, WORD* aIndices, 
								   int iNumIdx, DWORD dwCol)
{
	ASSERT(m_pA3DEngine && m_3dBuf.pA3DStream);

	if (iNumVert > m_iVertBufSize || iNumIdx > m_iIdxBufSize)
	{
		ASSERT(iNumVert <= m_iVertBufSize && iNumIdx <= m_iIdxBufSize);
		return false;
	}

	//	Vertex buffer has been full ?
	if (m_3dBuf.aVertBuf)
	{
		if (m_3dBuf.iVertCnt+iNumVert > m_iVertBufSize || m_3dBuf.iIdxCnt+iNumIdx > m_iIdxBufSize)
			Flush_3D();
	}

	//	Stream hasn't been locked, lock it.
	if (!m_3dBuf.aVertBuf)
	{
		if (!LockStream_3D(true))
		{
			g_A3DErrLog.Log("A3DWireCollector::AddRenderData_3D(), Failed to lock stream!");
			return false;
		}
	}

	A3DLVERTEX* pVert;
	WORD* pIndex;
	int i;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	pVert	= &m_3dBuf.aVertBuf[m_3dBuf.iVertCnt];
	pIndex	= &m_3dBuf.aIdxBuf[m_3dBuf.iIdxCnt];

	//	Fill vertex buffer
	for (i=0; i < iNumVert; i++, pVert++)
	{
		pVert->x	= aVerts[i].x;
		pVert->y	= aVerts[i].y;
		pVert->z	= aVerts[i].z;
		pVert->tu	= 0.0f;
		pVert->tv	= 0.0f;

		pVert->specular	= Specular;
		pVert->diffuse	= dwCol;
	}

	//	Fill index buffer
	for (i=0; i < iNumIdx; i++, pIndex++)
		*pIndex = m_3dBuf.iVertCnt + aIndices[i];

	m_3dBuf.iVertCnt	+= iNumVert;
	m_3dBuf.iIdxCnt		+= iNumIdx;

	return true;
}

//	Add vertex and index to render buffer
bool A3DWireCollector::AddRenderData_2D(A3DVECTOR3* aVerts, int iNumVert, WORD* aIndices, 
								   int iNumIdx, DWORD dwCol)
{
	ASSERT(m_pA3DEngine && m_2dBuf.pA3DStream);

	if (iNumVert > m_iVertBufSize || iNumIdx > m_iIdxBufSize)
	{
		ASSERT(iNumVert <= m_iVertBufSize && iNumIdx <= m_iIdxBufSize);
		return false;
	}

	//	Vertex buffer has been full ?
	if (m_2dBuf.aVertBuf)
	{
		if (m_2dBuf.iVertCnt+iNumVert > m_iVertBufSize || m_2dBuf.iIdxCnt+iNumIdx > m_iIdxBufSize)
			Flush_2D();
	}

	//	Stream hasn't been locked, lock it.
	if (!m_2dBuf.aVertBuf)
	{
		if (!LockStream_2D(true))
		{
			g_A3DErrLog.Log("A3DWireCollector::AddRenderData_2D(), Failed to lock stream!");
			return false;
		}
	}

	A3DTLVERTEX* pVert;
	WORD* pIndex;
	int i;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	pVert	= &m_2dBuf.aVertBuf[m_2dBuf.iVertCnt];
	pIndex	= &m_2dBuf.aIdxBuf[m_2dBuf.iIdxCnt];

	//	Fill vertex buffer
	for (i=0; i < iNumVert; i++, pVert++)
	{
		pVert->x	= aVerts[i].x;
		pVert->y	= aVerts[i].y;
		pVert->z	= aVerts[i].z;
		pVert->rhw	= 1.0f;
		pVert->tu	= 0.0f;
		pVert->tv	= 0.0f;

		pVert->specular	= Specular;
		pVert->diffuse	= dwCol;
	}

	//	Fill index buffer
	for (i=0; i < iNumIdx; i++, pIndex++)
		*pIndex = m_2dBuf.iVertCnt + aIndices[i];

	m_2dBuf.iVertCnt	+= iNumVert;
	m_2dBuf.iIdxCnt		+= iNumIdx;

	return true;
}

//	Flush sector
bool A3DWireCollector::Flush_3D()
{
	if (m_3dBuf.aVertBuf)
		LockStream_3D(false);

	if (!m_3dBuf.iVertCnt || !m_3dBuf.iIdxCnt)
		return true;

	//	Set render states
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);
	pA3DDevice->ClearTexture(0);
    pA3DDevice->ClearVertexShader();
    pA3DDevice->ClearPixelShader();

    DWORD dwArg1Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLORARG1);
    DWORD dwArg2Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLORARG2);
    DWORD dwOpOld = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLOROP);
    DWORD dwAlphaArg1Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAARG1);
    DWORD dwAlphaArg2Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAARG2);
    DWORD dwAlphaOpOld = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAOP);
    DWORD dwOpOld1 = pA3DDevice->GetDeviceTextureStageState(1, D3DTSS_COLOROP);
    pA3DDevice->SetTextureColorArgs(0, A3DTA_DIFFUSE, A3DTA_CURRENT);
    pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
    pA3DDevice->SetTextureAlphaArgs(0, A3DTA_DIFFUSE, A3DTA_CURRENT);
    pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
    pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);

	pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_3dBuf.pA3DStream->Appear();

	//	Render
	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, m_3dBuf.iVertCnt, 0, m_3dBuf.iIdxCnt / 2))
		return false;

	//	Restore render state
	pA3DDevice->SetLightingEnable(true);

	m_3dBuf.iVertCnt	= 0;
	m_3dBuf.iIdxCnt		= 0;

    pA3DDevice->SetTextureColorArgs(0, dwArg1Old, dwArg2Old);
    pA3DDevice->SetTextureColorOP(0, (A3DTEXTUREOP)dwOpOld);
    pA3DDevice->SetTextureAlphaArgs(0, dwAlphaArg1Old, dwAlphaArg2Old);
    pA3DDevice->SetTextureAlphaOP(0, (A3DTEXTUREOP)dwAlphaOpOld);
    pA3DDevice->SetTextureColorOP(1, (A3DTEXTUREOP)dwOpOld1);

    pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	return true;
}

//	Flush sector
bool A3DWireCollector::Flush_2D()
{
	if (m_2dBuf.aVertBuf)
		LockStream_2D(false);

	if (!m_2dBuf.iVertCnt || !m_2dBuf.iIdxCnt)
		return true;

	//	Set render states
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetFVF(A3DFVF_A3DTLVERTEX);
	pA3DDevice->ClearTexture(0);

    pA3DDevice->ClearVertexShader();
    pA3DDevice->ClearPixelShader();

    DWORD dwArg1Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLORARG1);
    DWORD dwArg2Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLORARG2);
    DWORD dwOpOld = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLOROP);
    DWORD dwAlphaArg1Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAARG1);
    DWORD dwAlphaArg2Old = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAARG2);
    DWORD dwAlphaOpOld = pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAOP);
    DWORD dwOpOld1 = pA3DDevice->GetDeviceTextureStageState(1, D3DTSS_COLOROP);
    pA3DDevice->SetTextureColorArgs(0, A3DTA_DIFFUSE, A3DTA_CURRENT);
    pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
    pA3DDevice->SetTextureAlphaArgs(0, A3DTA_DIFFUSE, A3DTA_CURRENT);
    pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
    pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);

	m_2dBuf.pA3DStream->Appear();

	//	Render
	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, m_2dBuf.iVertCnt, 0, m_2dBuf.iIdxCnt / 2))
		return false;

	//	Restore render state
	pA3DDevice->SetLightingEnable(true);

    pA3DDevice->SetTextureColorArgs(0, dwArg1Old, dwArg2Old);
    pA3DDevice->SetTextureColorOP(0, (A3DTEXTUREOP)dwOpOld);
    pA3DDevice->SetTextureAlphaArgs(0, dwAlphaArg1Old, dwAlphaArg2Old);
    pA3DDevice->SetTextureAlphaOP(0, (A3DTEXTUREOP)dwAlphaOpOld);
    pA3DDevice->SetTextureColorOP(1, (A3DTEXTUREOP)dwOpOld1);

	m_2dBuf.iVertCnt	= 0;
	m_2dBuf.iIdxCnt		= 0;

	return true;
}

//	Add a line to render buffer
bool A3DWireCollector::Add3DLine(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD dwCol)
{
	WORD aIndices[2] = {0, 1};
	A3DVECTOR3 aVerts[2];
	aVerts[0] = v1;
	aVerts[1] = v2;
	
	return AddRenderData_3D(aVerts, 2, aIndices, 2, dwCol);
}

//	Add a box to render buffer
bool A3DWireCollector::Add3DBox(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, 
								const A3DVECTOR3& vRight, const A3DVECTOR3& vExts, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	//	Indices for wire box
	static WORD aWireIndices[24] = 
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7,
	};

	float fx2 = vExts.x * 2.0f;
	float fy2 = vExts.y * 2.0f;
	float fz2 = vExts.z * 2.0f;

	A3DVECTOR3 aVerts[8];

	aVerts[0] = vPos + vDir * vExts.z + vUp * vExts.y - vRight * vExts.x;
	aVerts[1] = aVerts[0] + vRight * fx2;
	aVerts[2] = aVerts[1] - vDir * fz2;
	aVerts[3] = aVerts[0] - vDir * fz2;
	aVerts[4] = aVerts[0] - vUp * fy2;
	aVerts[5] = aVerts[4] + vRight * fx2;
	aVerts[6] = aVerts[5] - vDir * fz2;
	aVerts[7] = aVerts[4] - vDir * fz2;

	if (pMat)
	{
		for (int i=0; i < 8; i++)
			aVerts[i] = aVerts[i] * (*pMat);
	}

	//	Push to render buffer
	return AddRenderData_3D(aVerts, 8, aWireIndices, 24, dwCol);
}

bool A3DWireCollector::AddSphere(const A3DVECTOR3 vPos, float fRadius, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{

	float r = fRadius;
	
	int nRow;

	if(r < 3.f/20) 
		nRow = 3;
	else
		nRow = (int)(log(r*20+0.2)*1.8f + 1);  

	int nCol = nRow * 2 + 2;
	if(nCol < 3) nCol = 3;

	float fStep = r / nRow;

	int verCount = (nRow * 2 - 1) * nCol + 2;

	abase::vector<A3DVECTOR3> vertList(verCount,A3DVECTOR3());
	abase::vector<WORD> IndicesList;
	IndicesList.reserve(verCount*2);

	float angelStep = A3D_2PI / nCol;	
	int i,vIdx = 0;
	int idx1 = -1,idx2= -1;
	int saveIdx = -1;
	for(i = 0; i < nRow ; i++)
	{
		if(i == 0)
		{
			A3DVECTOR3 vec(0,r,0);
			vertList[vIdx++] = vec+vPos;
			vec.y = -r;
			vertList[vIdx++] = vec+vPos;
			idx1 = 0;
			idx2 = 1;
			saveIdx = vIdx;
			continue;
		}

		int j;
		saveIdx = vIdx;
		float tmp = (float)(sin(3.1415926535/2.0f/nRow* i));
		tmp *= r;
		for(j = 0; j < nCol; j++)
		{

			float z = sin(A3D_2PI/nCol*j)*tmp;
			float x = cos(A3D_2PI/nCol*j)*tmp;
			float y = sqrt(r*r- x*x - z*z);
			A3DVECTOR3 vec(x,y,z);
			vertList[vIdx++] = vec + vPos;
			vec.y = -y;
			vertList[vIdx++] = vec + vPos;
			//和上一层连接，考虑顶点
			if(i == 1)
			{
				IndicesList.push_back(idx1);
				IndicesList.push_back(saveIdx + j*2);
				IndicesList.push_back(idx2);
				IndicesList.push_back(saveIdx + j*2+1);
			}
			else
			{
				IndicesList.push_back(saveIdx +(j - nCol)*2);
				IndicesList.push_back(saveIdx + j*2);
				IndicesList.push_back(saveIdx + (j - nCol)*2+1);
				IndicesList.push_back(saveIdx + j*2 +1);
			}

			//和本层连接
			if(j == 0)
			{
				//事先和最后一个顶点连接
				IndicesList.push_back(saveIdx);
				IndicesList.push_back(saveIdx + (nCol-1)*2);
				IndicesList.push_back(saveIdx + 1);
				IndicesList.push_back(saveIdx + (nCol-1)*2 + 1);
			}
			else
			{
				IndicesList.push_back(saveIdx + (j-1)*2);
				IndicesList.push_back(saveIdx + j*2);
				IndicesList.push_back(saveIdx + (j-1)*2 + 1);
				IndicesList.push_back(saveIdx + j*2 + 1);
			}
		}
	}

	saveIdx = vIdx;
	for(i = 0; i < nCol; i++)
	{

		float z = sin(angelStep*i)*r;
		float x = cos(angelStep*i)*r;
		float y = 0;
		A3DVECTOR3 vec(x,y,z);
		vertList[vIdx++] = vec + vPos;
		//和上一层连接，考虑顶点
		if(nRow == 1)
		{
			IndicesList.push_back(0);
			IndicesList.push_back(saveIdx + i);
			IndicesList.push_back(1);
			IndicesList.push_back(saveIdx + i);
		}
		else
		{
			IndicesList.push_back(saveIdx + (i - nCol)*2);
			IndicesList.push_back(saveIdx + i);
			IndicesList.push_back(saveIdx + (i - nCol)*2 + 1);
			IndicesList.push_back(saveIdx + i);
		}

		//和本层连接
		if(i == 0)
		{
			//事先和最后一个顶点连接
			IndicesList.push_back(saveIdx);
			IndicesList.push_back(saveIdx + (nCol-1));
		}
		else
		{
			IndicesList.push_back(saveIdx + (i-1));
			IndicesList.push_back(saveIdx + i);
		}
	}

	assert(vIdx == verCount);

	if (pMat)
	{
		for (int i=0; i < verCount; i++)
			vertList[i] = vertList[i] * (*pMat);
	}

	return AddRenderData_3D(vertList.begin(), vertList.size(), IndicesList.begin(), IndicesList.size(), dwCol);	
}

//	Add a AABB to render buffer
bool A3DWireCollector::AddAABB(const A3DAABB& aabb, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	return Add3DBox(aabb.Center, A3D::g_vAxisZ, A3D::g_vAxisY, A3D::g_vAxisX, aabb.Extents, dwCol, pMat);
}

//	Add a OBB to render buffer
bool A3DWireCollector::AddOBB(const A3DOBB& obb, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	return Add3DBox(obb.Center, obb.ZAxis, obb.YAxis, obb.XAxis, obb.Extents, dwCol, pMat);
}

//	Add a capsule to render buffer
bool A3DWireCollector::AddCapsule(const A3DCAPSULE& cc, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	if (cc.fHalfLen == 0.0f)
		return AddSphere(cc.vCenter, cc.fRadius, dwCol, pMat);

	AArray<A3DVECTOR3, A3DVECTOR3&> aVerts(0, 256);
	AArray<WORD, WORD> aIndices(0, 256);

	int i, j, iNumRow=4, iNumCol=8;
	const A3DVECTOR3& vLenAxis = cc.vAxisY;

	//	Top hemisphere's center
	A3DVECTOR3 vCenter = cc.vCenter + vLenAxis * cc.fHalfLen;

	//	Top vertex of top hemisphere
	aVerts.Add(vCenter + vLenAxis * cc.fRadius);

	A3DVECTOR3 vPos, vOff;

	//	Vertices of top hemisphere
	for (i=0; i < iNumRow; i++)
	{
		float fPitch = DEG2RAD(90.0f - (i+1) * 90.0f / iNumRow);
		vOff.y = cc.fRadius * sin(fPitch);

		float fYawStep = DEG2RAD(360.0f / iNumCol);
		float fRadius = cc.fRadius * cos(fPitch);

		for (j=0; j < iNumCol; j++)
		{
			float fYaw = j * fYawStep;

			vOff.x = fRadius * cos(fYaw);
			vOff.z = fRadius * sin(fYaw);

			vPos = vCenter + vOff.x * cc.vAxisX + vOff.y * cc.vAxisY + vOff.z * cc.vAxisZ;
			aVerts.Add(vPos);
		}
	}

	//	Bottom hemisphere's center
	vCenter = cc.vCenter - vLenAxis * cc.fHalfLen;

	//	Vertices of bottom hemisphere
	for (i=0; i < iNumRow; i++)
	{
		float fPitch = DEG2RAD(i * 90.0f / iNumRow);
		vOff.y = cc.fRadius * sin(fPitch);

		float fYawStep = DEG2RAD(360.0f / iNumCol);
		float fRadius = cc.fRadius * cos(fPitch);

		for (j=0; j < iNumCol; j++)
		{
			float fYaw = j * fYawStep;

			vOff.x = fRadius * cos(fYaw);
			vOff.z = fRadius * sin(fYaw);

			vPos = vCenter + vOff.x * cc.vAxisX - vOff.y * cc.vAxisY + vOff.z * cc.vAxisZ;
			aVerts.Add(vPos);
		}
	}

	//	Top vertex of bottom hemisphere
	aVerts.Add(vCenter - vLenAxis * cc.fRadius);

	WORD wBaseIdx = 1;
	
	for (i=0; i < iNumCol; i++)
	{
		aIndices.Add(0);
		aIndices.Add(wBaseIdx + i);

		if (i == iNumCol - 1)
		{
			aIndices.Add(wBaseIdx + iNumCol - 1);
			aIndices.Add(wBaseIdx);
		}
		else
		{
			aIndices.Add(wBaseIdx + i);
			aIndices.Add(wBaseIdx + i + 1);
		}
	}

	wBaseIdx += iNumCol;

	int iLoop = iNumRow * 2 - 2 + 1;
	for (j=0; j < iLoop; j++)
	{
		for (i=0; i < iNumCol; i++)
		{	
			aIndices.Add(wBaseIdx + i - iNumCol);
			aIndices.Add(wBaseIdx + i);

			if (i == iNumCol - 1)
			{
				aIndices.Add(wBaseIdx + iNumCol - 1);
				aIndices.Add(wBaseIdx);
			}
			else
			{
				aIndices.Add(wBaseIdx + i);
				aIndices.Add(wBaseIdx + i + 1);
			}
		}

		wBaseIdx += iNumCol;
	}

	for (i=0; i < iNumCol; i++)
	{
		aIndices.Add(wBaseIdx + i - iNumCol);
		aIndices.Add(wBaseIdx);
	}

	//	Transform vertices
	if (pMat)
	{
		for (i=0; i < aVerts.GetSize(); i++)
			aVerts[i] = aVerts[i] * (*pMat);
	}

	return AddRenderData_3D(aVerts.GetData(), aVerts.GetSize(), aIndices.GetData(), aIndices.GetSize(), dwCol);
}

//	Add a cylinder to render buffer
bool A3DWireCollector::AddCylinder(const A3DCYLINDER& c, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	if (c.fHalfLen == 0.0f || c.fRadius == 0.0f)
		return true;

	AArray<A3DVECTOR3, A3DVECTOR3&> aVerts(0, 256);
	AArray<WORD, WORD> aIndices(0, 256);

	const int iNumCol = 8;
	A3DVECTOR3 vCenter = c.vCenter + c.vAxisY * c.fHalfLen;

	int i;
	float fYawStep = DEG2RAD(360.0f / iNumCol);
	A3DVECTOR3 vPos, vOff;

	//	Add vertices
	for (i=0; i < iNumCol; i++)
	{
		float fYaw = i * fYawStep;

		vOff.x = c.fRadius * cos(fYaw);
		vOff.z = c.fRadius * sin(fYaw);

		vPos = vCenter + vOff.x * c.vAxisX + vOff.z * c.vAxisZ;
		aVerts.Add(vPos);
		vPos -= c.vAxisY * (c.fHalfLen * 2.0f);
		aVerts.Add(vPos);
	}

	//	Add cap borders
	for (i=0; i < iNumCol; i++)
	{
		int i1 = i * 2;
		int i2 = (i + 1) % iNumCol * 2;
	
		aIndices.Add(i1);
		aIndices.Add(i2);

		aIndices.Add(i1 + 1);
		aIndices.Add(i2 + 1);
	}

	//	Add side faces
	for (i=0; i < iNumCol; i++)
	{
		aIndices.Add(i * 2);
		aIndices.Add(i * 2 + 1);
	}

	//	Transform vertices
	if (pMat)
	{
		for (i=0; i < aVerts.GetSize(); i++)
			aVerts[i] = aVerts[i] * (*pMat);
	}

	return AddRenderData_3D(aVerts.GetData(), aVerts.GetSize(), aIndices.GetData(), aIndices.GetSize(), dwCol);;
}

//	Add 2D line
bool A3DWireCollector::AddLine_2D(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD dwCol)
{
	WORD aIndices[2] = {0, 1};
	A3DVECTOR3 aVerts[2];
	aVerts[0] = v1;
	aVerts[1] = v2;
	
	return AddRenderData_2D(aVerts, 2, aIndices, 2, dwCol);
}

//	Add 2D rectangle
bool A3DWireCollector::AddRect_2D(int l, int t, int r, int b, DWORD dwCol, float fz/* 0.0f */)
{
	WORD aIndices[8] = {0, 1, 1, 2, 2, 3, 3, 0};
	A3DVECTOR3 aVerts[4];

	aVerts[0].Set((float)l, (float)t, fz);
	aVerts[1].Set((float)r, (float)t, fz);
	aVerts[2].Set((float)r, (float)b, fz);
	aVerts[3].Set((float)l, (float)b, fz);

	return AddRenderData_2D(aVerts, 4, aIndices, 8, dwCol);
}


