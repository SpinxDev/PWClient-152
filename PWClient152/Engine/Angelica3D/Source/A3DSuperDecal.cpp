/*
 * FILE: A3DSuperDecal.cpp
 *
 * DESCRIPTION: Routines for super decal which has more properties then normal decal
 *
 * CREATED BY: duyuxin, 2001/12/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DSuperDecal.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DGFXMan.h"
#include "A3DVertexCollector.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "AFile.h"

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DSuperDecal::A3DSuperDecal()
{
	m_dwClassID		= A3D_CID_SUPERDECAL;
	m_iTexInterval	= 1;
	m_iRepTime		= 1;
	m_bTexRepeat	= false;
	m_iNumKey		= 0;
	m_iTexRow		= 0;
	m_iTexCol		= 0;
	m_iCurTexRow	= 0;
	m_iCurTexCol	= 0;
	m_fTexU			= 0.0f;
	m_fTexV			= 0.0f;
	m_bFinish		= true;
	m_fScale		= 1.0f;
	m_fSize			= 0.0f;

	for (int i=0; i < MAXNUM_SDKEYPOINT; i++)
	{
		m_aKeys[i].fWidth	= 1.0f;
		m_aKeys[i].fHeight	= 1.0f;
		m_aKeys[i].a		= 255;
		m_aKeys[i].r		= 255;
		m_aKeys[i].g		= 255;
		m_aKeys[i].b		= 255;
		m_aKeys[i].fRotate	= 0.0f;
		m_aKeys[i].vPos		= A3DVECTOR3(0.0f);
		m_aKeys[i].iTickStamp	= 0;
	}

	m_Shader.SrcBlend	= A3DBLEND_SRCALPHA;
	m_Shader.DestBlend	= A3DBLEND_INVSRCALPHA;
}

A3DSuperDecal::~A3DSuperDecal()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pDevice: valid A3D device
	szTexName: texture file's name
*/
bool A3DSuperDecal::Init(A3DDevice* pDevice, char* szTexName)
{
	if (!A3DDecal::Init(DECALTYPE_PURE3D, pDevice, szTexName))
	{
		g_A3DErrLog.Log("A3DSuperDecal::Init(), A3DDecal::Init failed!");
		return false;
	}

	Stop();

	return true;
}

//	Release resource of object
void A3DSuperDecal::Release()
{
	A3DDecal::Release();
}

/*	Set texture properties.

	Return true for success, otherwise return false.

	iRow: rows of texture tile
	iCol: columns of texture tile
	iInterval: tick interval between two texture tile
*/
bool A3DSuperDecal::SetTextureProp(int iRow, int iCol, int iInterval)
{
	if (iRow <= 0 || iCol <= 0 || iInterval <= 0)
		return false;

	m_iTexRow		= iRow;
	m_iTexCol		= iCol;
	m_iTexInterval	= iInterval;
	m_fTexWid		= 1.0f / m_iTexCol;
	m_fTexHei		= 1.0f / m_iTexRow;
	m_fTexU			= 0.0f;
	m_fTexV			= 0.0f;

	return true;
}

/*	Get texture properties.

	piRow (out): used to receive rows of texture tile
	piCol (out): used to receive columns of texture tile
	piInterval (out): used to receive tick interval between two texture tile
*/	
void A3DSuperDecal::GetTextureProp(int* piRow, int* piCol, int* piInterval)
{
	*piRow		= m_iTexRow;
	*piCol		= m_iTexCol;
	*piInterval	= m_iTexInterval;
}

/*	Add a key point.

	Return true for success, otherwise return false.

	iIndex: index of key point whose properties will be set
	KeyPoint: key point's properties
*/
bool A3DSuperDecal::SetKeyPoint(int iIndex, SUPDECALKEYPOINT& KeyPoint)
{
	if (iIndex < 0 && iIndex >= m_iNumKey)
		return false;

	m_aKeys[iIndex] = KeyPoint;

	return true;
}

/*	Get specified key point's properties.

	Return true for success, otherwise return false. 

	iIndex: specified key point
	pKeyPoint (out): used to receive key point's properties
*/
bool A3DSuperDecal::GetKeyPoint(int iIndex, SUPDECALKEYPOINT* pKeyPoint)
{
	if (iIndex < 0 && iIndex >= m_iNumKey)
		return false;

	*pKeyPoint = m_aKeys[iIndex];

	return true;
}

void A3DSuperDecal::Play()
{
	if (m_iNumKey < 1)
		return;

	//	Set proper states
	Stop();

	m_iCurTexRow	= 0;
	m_iCurTexCol	= 0;
	m_fTexU			= 0.0f;
	m_fTexV			= 0.0f;
	m_iState		= ST_PLAY;
	m_bFinish		= false;

	m_CurProp.fWidth	= m_aKeys[0].fWidth;
	m_CurProp.fHeight	= m_aKeys[0].fHeight;
	m_CurProp.fRotate	= m_aKeys[0].fRotate;
	m_CurProp.a			= m_aKeys[0].a;
	m_CurProp.r			= m_aKeys[0].r;
	m_CurProp.g			= m_aKeys[0].g;
	m_CurProp.b			= m_aKeys[0].b;
	m_CurProp.vPos.x	= m_aKeys[0].vPos.x;
	m_CurProp.vPos.y	= m_aKeys[0].vPos.y;
	m_CurProp.vPos.z	= m_aKeys[0].vPos.z;

	//	Update decal's properties and prepare to render
	//  These parameter must be called when a call is made to Play(),
	//  or you will see the new decal appear at an old pos
	m_fWidth	 = m_CurProp.fWidth * m_fScale;
	m_fHeight	 = m_CurProp.fHeight * m_fScale;
	m_fRotateDeg = m_CurProp.fRotate;
	m_vCenter	 = m_CurProp.vPos * m_matParentTM; // We must transform center to world space;

	A3DCOLOR Col = A3DCOLORRGBA((BYTE)m_CurProp.r, (BYTE)m_CurProp.g, (BYTE)m_CurProp.b, (BYTE)m_CurProp.a);
	A3DDecal::SetColor(Col);

	if (m_iNumKey > 1)
	{
		m_iNextKey = GetNextKeyIndex();
		CalculateStepLength();
	}
}

//	Pause
void A3DSuperDecal::Pause(bool bPause)
{
	if (bPause && m_iState == ST_PLAY)
		m_iState = ST_PAUSE;
	else if (!bPause && m_iState == ST_PAUSE)
		m_iState = ST_PLAY;
}

//	Stop play and restore decal's properties to the first key point
void A3DSuperDecal::Stop()
{
	m_iState		= ST_STOP;
	m_iCurKey		= 0;
	m_iCurTick		= 0;
	m_iLifeCnt		= 0;
	m_iRepCnt		= 0;
	m_bFinish		= true;
}

/*	To play decal animation this function should be called every tick.

	Return true for success, otherwise return false
*/
bool A3DSuperDecal::TickAnimation()
{
	if (m_iState != ST_PLAY)
		return true;

	//	It's time to change texture ?
	if (m_iLifeCnt > 0 && !(m_iLifeCnt % m_iTexInterval))
	{
		if (++m_iCurTexCol >= m_iTexCol)
		{
			if (++m_iCurTexRow >= m_iTexRow)
			{
				if (m_bTexRepeat)//m_iRepTime != 1)
				{
					m_iCurTexCol = 0;
					m_iCurTexRow = 0;
				}
				else
				{
					//	Stop at the last tile
					m_iCurTexCol--;
					m_iCurTexRow--;
				}
			}
			else	//	Change a row
			{
				m_iCurTexCol = 0;
			}
		}

		m_fTexU = m_fTexWid * m_iCurTexCol;
		m_fTexV = m_fTexHei * m_iCurTexRow;
	}

	//	More than one key point ?
	if (m_iNumKey > 1)
	{
		//	Reach next key point ?
		if (m_iCurTick >= m_aKeys[m_iNextKey].iTickStamp)
		{
			m_iCurKey	= m_iNextKey;
			m_iNextKey	= GetNextKeyIndex();

			if (!m_iNextKey)	//	Loop ?
			{
				m_iRepCnt++;
				m_iCurKey	= 0;
				m_iNextKey	= GetNextKeyIndex();
				m_iCurTick	= 0;
			}

			m_CurProp.fWidth	= m_aKeys[m_iCurKey].fWidth;
			m_CurProp.fHeight	= m_aKeys[m_iCurKey].fHeight;
			m_CurProp.fRotate	= m_aKeys[m_iCurKey].fRotate;
			m_CurProp.a			= m_aKeys[m_iCurKey].a;
			m_CurProp.r			= m_aKeys[m_iCurKey].r;
			m_CurProp.g			= m_aKeys[m_iCurKey].g;
			m_CurProp.b			= m_aKeys[m_iCurKey].b;
			m_CurProp.vPos.x	= m_aKeys[m_iCurKey].vPos.x;
			m_CurProp.vPos.y	= m_aKeys[m_iCurKey].vPos.y;
			m_CurProp.vPos.z	= m_aKeys[m_iCurKey].vPos.z;

			if (m_iNextKey == -1)	//	No more key can be got
				Stop();
			else
				CalculateStepLength();
		}
		else
		{
			m_CurProp.fWidth	+= m_Step.fWidth;
			m_CurProp.fHeight	+= m_Step.fHeight;
			m_CurProp.fRotate	+= m_Step.fRotate;
			m_CurProp.a			+= m_Step.a;
			m_CurProp.r			+= m_Step.r;
			m_CurProp.g			+= m_Step.g;
			m_CurProp.b			+= m_Step.b;
			m_CurProp.vPos.x	+= m_Step.vPos.x;
			m_CurProp.vPos.y	+= m_Step.vPos.y;
			m_CurProp.vPos.z	+= m_Step.vPos.z;
		}
	}
	else	//	Always update current state by key point 0
	{
		m_CurProp.fWidth	= m_aKeys[0].fWidth;
		m_CurProp.fHeight	= m_aKeys[0].fHeight;
		m_CurProp.fRotate	= m_aKeys[0].fRotate;
		m_CurProp.a			= m_aKeys[0].a;
		m_CurProp.r			= m_aKeys[0].r;
		m_CurProp.g			= m_aKeys[0].g;
		m_CurProp.b			= m_aKeys[0].b;
		m_CurProp.vPos.x	= m_aKeys[0].vPos.x;
		m_CurProp.vPos.y	= m_aKeys[0].vPos.y;
		m_CurProp.vPos.z	= m_aKeys[0].vPos.z;
	}

	//	Update decal's properties and prepare to render
	m_fWidth	 = m_CurProp.fWidth * m_fScale;
	m_fHeight	 = m_CurProp.fHeight * m_fScale;
	m_fRotateDeg = m_CurProp.fRotate;
	m_vCenter	 = m_CurProp.vPos * m_matParentTM; // We must transform center to world space;

	A3DCOLOR Col = A3DCOLORRGBA((BYTE)m_CurProp.r, (BYTE)m_CurProp.g, (BYTE)m_CurProp.b, (BYTE)m_CurProp.a);
	A3DDecal::SetColor(Col);

	// Step to next frame;
	m_iLifeCnt++;
	m_iCurTick++;
	return true;
}

bool A3DSuperDecal::Render(A3DViewport * pView, bool bCheckVis/* true */)
{
	if (m_bFinish)
		return true;

	return A3DDecal::Render(pView, bCheckVis);
}

//	Calculate step length between current key point and next one
void A3DSuperDecal::CalculateStepLength()
{
	if (m_aKeys[m_iCurKey].iTickStamp >= m_aKeys[m_iNextKey].iTickStamp-1)
		return;

	float fTime = 1.0f / (m_aKeys[m_iNextKey].iTickStamp - m_aKeys[m_iCurKey].iTickStamp);

	m_Step.fWidth	= (m_aKeys[m_iNextKey].fWidth - m_aKeys[m_iCurKey].fWidth) * fTime;
	m_Step.fHeight	= (m_aKeys[m_iNextKey].fHeight - m_aKeys[m_iCurKey].fHeight) * fTime;
	m_Step.fRotate	= (m_aKeys[m_iNextKey].fRotate - m_aKeys[m_iCurKey].fRotate) * fTime;
	m_Step.a		= (m_aKeys[m_iNextKey].a - m_aKeys[m_iCurKey].a) * fTime;
	m_Step.r		= (m_aKeys[m_iNextKey].r - m_aKeys[m_iCurKey].r) * fTime;
	m_Step.g		= (m_aKeys[m_iNextKey].g - m_aKeys[m_iCurKey].g) * fTime;
	m_Step.b		= (m_aKeys[m_iNextKey].b - m_aKeys[m_iCurKey].b) * fTime;
	m_Step.vPos.x	= (m_aKeys[m_iNextKey].vPos.x - m_aKeys[m_iCurKey].vPos.x) * fTime;
	m_Step.vPos.y	= (m_aKeys[m_iNextKey].vPos.y - m_aKeys[m_iCurKey].vPos.y) * fTime;
	m_Step.vPos.z	= (m_aKeys[m_iNextKey].vPos.z - m_aKeys[m_iCurKey].vPos.z) * fTime;
}

/*	Get next key's index.

	Return -1 if no key can be got.
*/
int A3DSuperDecal::GetNextKeyIndex()
{
	if (m_iCurKey+1 >= m_iNumKey)
	{
		if (m_iRepTime <= 0)
			return 0;
		else if (m_iRepCnt+1 >= m_iRepTime)
			return -1;

		return 0;
	}

	return m_iCurKey+1;
}

bool A3DSuperDecal::Save(AFile * pFileToSave)
{
	//Save superdecal specification data first;
	if( pFileToSave->IsBinary() )
	{
		//Save Binary Data here;
	}
	else
	{
		//Save Text Data here;
		char szLineBuffer[AFILE_LINEMAXLEN];

		sprintf(szLineBuffer, "KeyPointNum: %d", m_iNumKey);
		pFileToSave->WriteLine(szLineBuffer);
		for(int i=0; i<m_iNumKey; i++)
		{
			sprintf(szLineBuffer, "KeyPoint: (%d, %d, %f, %f, %x, %f, %f, %f, %f",
				i, 
				m_aKeys[i].iTickStamp, 
				m_aKeys[i].fWidth, 
				m_aKeys[i].fHeight, 
				A3DCOLORRGBA(m_aKeys[i].r, m_aKeys[i].g, m_aKeys[i].b, m_aKeys[i].a), 
				m_aKeys[i].fRotate, 
				m_aKeys[i].vPos.x, 
				m_aKeys[i].vPos.y, 
				m_aKeys[i].vPos.z);
			pFileToSave->WriteLine(szLineBuffer);
		}
		sprintf(szLineBuffer, "RepeatTime: %d", GetRepeatTime());
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TexRow: %d", m_iTexRow);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TexCol: %d", m_iTexCol);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TexInterval: %d", m_iTexInterval);
		pFileToSave->WriteLine(szLineBuffer);
		
		// Blend flag;
		sprintf(szLineBuffer, "SrcBlend: %d", m_Shader.SrcBlend);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "DestBlend: %d", m_Shader.DestBlend);
		pFileToSave->WriteLine(szLineBuffer);

		if (m_pTexture)
		{
			AString strFileName;
			m_pTexture->GetMapFileInFolder(m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), strFileName);
			sprintf(szLineBuffer, "TextureMap: %s", strFileName);
		}
		else
			sprintf(szLineBuffer, "TextureMap: ");

		pFileToSave->WriteLine(szLineBuffer);
	}
	return true;
}

bool A3DSuperDecal::Load(A3DDevice * pDevice, AFile * pFileToLoad)
{
	char		szTextureMap[MAX_PATH];

	if (!Init(pDevice, NULL))
	{
		g_A3DErrLog.Log("A3DSuperDecal::Load(), failed to initialize decal!");
		return false;
	}

	 //Load superdecal specification data first;
	if( pFileToLoad->IsBinary() )
	{
		//Load Binary Data here;
	}
	else
	{
		//Load Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];
		char			szResult[AFILE_LINEMAXLEN];
		DWORD			dwReadLen;
		int				nval;
		A3DCOLOR		color;
		FLOAT			vSize;
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "KeyPointNum: %d", &m_iNumKey);
		for(int i=0; i<m_iNumKey; i++)
		{
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "KeyPoint: (%d, %d, %f, %f, %x, %f, %f, %f, %f",
				&nval, &m_aKeys[i].iTickStamp, &m_aKeys[i].fWidth, &m_aKeys[i].fHeight, 
				&color, 
				&m_aKeys[i].fRotate, &m_aKeys[i].vPos.x, &m_aKeys[i].vPos.y, &m_aKeys[i].vPos.z);
			m_aKeys[i].r = A3DCOLOR_GETRED(color);
			m_aKeys[i].g = A3DCOLOR_GETGREEN(color);
			m_aKeys[i].b = A3DCOLOR_GETBLUE(color);
			m_aKeys[i].a = A3DCOLOR_GETALPHA(color);

			vSize = (FLOAT) sqrt(m_aKeys[i].fWidth * m_aKeys[i].fWidth + m_aKeys[i].fHeight * m_aKeys[i].fHeight);
			if( m_fSize < vSize ) m_fSize = vSize;  
		}
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "RepeatTime: %d", &m_iRepTime);
		m_bTexRepeat = m_iRepTime < 0 ? true : false;
		m_iRepTime = abs(m_iRepTime);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "TexRow: %d", &m_iTexRow);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "TexCol: %d", &m_iTexCol);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "TexInterval: %d", &m_iTexInterval);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strstr(szLineBuffer, "SrcBlend: ") == szLineBuffer )
		{
			sscanf(szLineBuffer, "SrcBlend: %d", &m_Shader.SrcBlend);
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "DestBlend: %d", &m_Shader.DestBlend);
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		}
		
		a_GetStringAfter(szLineBuffer, "TextureMap: ", szResult);
		strncpy(szTextureMap, szResult, MAX_PATH);

		SetTextureProp(m_iTexRow, m_iTexCol, m_iTexInterval);

		if (szTextureMap[0] && !ChangeTexture(szTextureMap))
		{
			g_A3DErrLog.Log("A3DSuperDecal::Load(), failed to change texture!");
			return false;
		}

		// See if this decal is too short to use offset when it is shown;
		if( m_aKeys[m_iNumKey - 1].iTickStamp <= 15 )
			m_fSize = 0.0f;
	}

	if (m_bOptimized)
	{
		A3DVertexCollector* pCollector = m_pA3DDevice->GetA3DEngine()->GetVertexCollector();
		m_hTexInfo = pCollector->RegisterTexture(A3DVT_TLVERTEX, m_pTexture, NULL, &m_Shader);
	}

	return true;
}
