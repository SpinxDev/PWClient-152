/*
 * FILE: AUIClockIcon.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "AUI.h"

#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DFlatCollector.h"
#include "AAssist.h"
#include "AUIClockIcon.h"

#define new A_DEBUG_NEW

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

//	Indices
WORD AUIClockIcon::m_aIndices[24] = 
{
	8, 7, 0, 8, 0, 1, 
	8, 1, 2, 8, 2, 3, 
	8, 3, 4, 8, 4, 5, 
	8, 5, 6, 8, 6, 7,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AUIClockIcon
//	
///////////////////////////////////////////////////////////////////////////

AUIClockIcon::AUIClockIcon()
{
	m_x		= 0;
	m_y		= 0;
	m_cx	= 16;
	m_cy	= 16;
	m_fz	= 0.0f;
	m_iPos	= 100;
	m_iMin	= 0;
	m_iMax	= 100;
	m_iStep	= 10;
	m_dwCol	= 0x80ffffff;
	m_pA3DEngine = NULL;
	m_pA3DTexture = NULL;
	m_pA3DStream = NULL;
	m_iClockType = AUICLOCK_ROTATE;
}

AUIClockIcon::~AUIClockIcon()
{
}

//	Advances the current position
int AUIClockIcon::OffsetProgressPos(int iOffset)
{
	m_iPos += iOffset;
	a_Clamp(m_iPos, m_iMin, m_iMax);
	return m_iPos;
}

//	Sets the current position
void AUIClockIcon::SetProgressPos(int iPos)
{
	m_iPos = iPos;
	a_Clamp(m_iPos, m_iMin, m_iMax);
}

//	Sets the minimum and maximum ranges 
void AUIClockIcon::SetProgressRange(int iMin, int iMax)
{
	ASSERT(iMin <= iMax);
	m_iMin = iMin;
	m_iMax = iMax;
	a_Clamp(m_iPos, m_iMin, m_iMax);
}

//	Advances the current position by the step increment
int AUIClockIcon::StepProgress()
{
	m_iPos += m_iStep;
	a_Clamp(m_iPos, m_iMin, m_iMax);
	return m_iPos;
}

//	Set icon position and size
void AUIClockIcon::MoveIcon(int x, int y, int cx, int cy)
{
	m_x		= x;
	m_y		= y;
	m_cx	= cx;
	m_cy	= cy;
	
	OnIconMove();
}

//	Update icon position and size
void AUIClockIcon::OnIconMove()
{
	float l = (float)m_x;
	float t = (float)m_y;
	float r = (float)m_x + m_cx - 1;
	float b = (float)m_y + m_cy - 1;
	float xc = (l + r) * 0.5f;
	float yc = (t + b) * 0.5f;

	m_aPos[0].x	= r;
	m_aPos[0].y	= t;
	m_aPos[0].z	= m_fz;

	m_aPos[1].x	= r;
	m_aPos[1].y	= yc;
	m_aPos[1].z	= m_fz;

	m_aPos[2].x	= r;
	m_aPos[2].y	= b;
	m_aPos[2].z	= m_fz;

	m_aPos[3].x	= xc;
	m_aPos[3].y	= b;
	m_aPos[3].z	= m_fz;

	m_aPos[4].x	= l;
	m_aPos[4].y	= b;
	m_aPos[4].z	= m_fz;

	m_aPos[5].x	= l;
	m_aPos[5].y	= yc;
	m_aPos[5].z	= m_fz;

	m_aPos[6].x	= l;
	m_aPos[6].y	= t;
	m_aPos[6].z	= m_fz;

	m_aPos[7].x	= xc;
	m_aPos[7].y	= t;
	m_aPos[7].z	= m_fz;

	m_aPos[8].x	= xc;
	m_aPos[8].y	= yc;
	m_aPos[8].z	= m_fz;
}

//	Render routine
bool AUIClockIcon::Render()
{
	ASSERT(m_pA3DEngine);

	bool bRet = false;
	switch (m_iClockType)
	{	
	case AUICLOCK_UP:
	case AUICLOCK_DOWN:
		bRet = RenderUpdown();
		break;
	case AUICLOCK_ROTATE:
	default:
		bRet = RenderRotate();
		break;
	}
	return bRet;
}

bool AUIClockIcon::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char* pszClockImage)
{
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	return SetClockImage(pszClockImage);
}

void AUIClockIcon::Release()
{
	A3DRELEASE(m_pA3DStream)
	A3DRELEASE(m_pA3DTexture)
}

bool AUIClockIcon::SetClockImage(const char* pszClockImage)
{
	A3DRELEASE(m_pA3DTexture)

	if( pszClockImage && strlen(pszClockImage) != 0 )
	{
		AString strPath;
		strPath.Format("Surfaces\\%s", pszClockImage);
		m_pA3DTexture = new A3DTexture;
		m_pA3DTexture->SetNoDownSample(true);
		if( !m_pA3DTexture->LoadFromFile(m_pA3DDevice, strPath, strPath) )
		{
			A3DRELEASE(m_pA3DTexture);
			return AUI_ReportError(__LINE__, 1, "AUIClockIcon::SetClockImage, failed to call m_pA3DTexture->LoadFromFile()");
		}

		if( !m_pA3DStream )
		{
			m_pA3DStream = new A3DStream;
			if( !m_pA3DStream->Init(m_pA3DDevice, sizeof(A3DTLVERTEX), A3DFVF_A3DTLVERTEX, 24, 0, A3DSTRM_REFERENCEPTR, 0) )
			{
				A3DRELEASE(m_pA3DStream);
				return AUI_ReportError(__LINE__, __FILE__);
			}
		}

	}
	else
	{
		A3DRELEASE(m_pA3DStream)
	}

	return true;
}

bool AUIClockIcon::RenderRotate()
{
	//	Handle special progress position
	if (m_iPos == m_iMax)
	{
		return true;
	}
	else if (m_iPos == m_iMin)
	{
		if( m_pA3DTexture )
		{
			m_pA3DEngine->FlushInternalSpriteBuffer();
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pA3DTexture->Appear(0);
			
			bool bval = true;
			A3DTLVERTEX * pVerts, vert[10];
			if( !m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**) &pVerts, 0) )
				return false;
			
			for( int j = 0; j < 8; j+=2 )
			{
				vert[j].x = m_aPos[j].x;
				vert[j].y = m_aPos[j].y;
				vert[j].z = 0.0f;
				vert[j].rhw = 1.0f;
				vert[j].diffuse = m_dwCol;
				vert[j].specular = 0xFF000000;
			}

			vert[0].tu = vert[2].tu = 1.0f;
			vert[4].tu = vert[6].tu = 0.0f;
			vert[0].tv = vert[6].tv = 0.0f;
			vert[2].tv = vert[4].tv = 1.0f;

			pVerts[0] = vert[0];
			pVerts[1] = vert[2];
			pVerts[2] = vert[4];
			pVerts[3] = vert[0];
			pVerts[4] = vert[4];
			pVerts[5] = vert[6];
			
			m_pA3DStream->UnlockVertexBufferDynamic();
			m_pA3DStream->Appear();
			
			bval = m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);
			if( !bval ) 
				return AUI_ReportError(__LINE__, __FILE__);
			
			m_pA3DTexture->Disappear(0);

			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetLightingEnable(true);
		}
		else
		{
			A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();
			ASSERT(pFC);

			A3DVECTOR3 aPos[4];
			aPos[0] = m_aPos[0];
			aPos[1] = m_aPos[2];
			aPos[2] = m_aPos[4];
			aPos[3] = m_aPos[6];

			static const WORD aIndices[6] = {0, 1, 2, 0, 2, 3};
			pFC->AddRenderData_2D(aPos, 4, aIndices, 6, m_dwCol);
		}
		
		return true;
	}

	float fSegLen = (m_iMax - m_iMin) * 0.125f;
	float fCurSeg = (m_iPos - m_iMin) / fSegLen;

	int iSeg = (int)fCurSeg;
	a_Clamp(iSeg, 0, 7);
	float fResidue = fCurSeg - iSeg;

	float l = (float)m_x;
	float t = (float)m_y;
	float r = (float)m_x + m_cx - 1;
	float b = (float)m_y + m_cy - 1;
	float xc = (l + r) * 0.5f;
	float yc = (t + b) * 0.5f;
	float xhl = m_cx * 0.5f;
	float yhl = m_cy * 0.5f;

	A3DVECTOR3& v = m_aPos[9];
	v.z = m_fz;
	
	switch (iSeg)
	{
	case 0:	v.x = xc + xhl * fResidue; v.y = t;		break;
	case 1: v.x = r; v.y = t + yhl * fResidue;		break;
	case 2: v.x = r; v.y = yc + yhl * fResidue;		break;
	case 3: v.x = r - xhl * fResidue; v.y = b;		break;
	case 4: v.x = xc - xhl * fResidue; v.y = b;		break;
	case 5: v.x = l; v.y = b - yhl * fResidue;		break;
	case 6: v.x = l; v.y = yc - yhl * fResidue;		break;
	case 7: v.x = l + xhl * fResidue; v.y = t;		break;
	}

	if( m_pA3DTexture )
	{
		m_pA3DEngine->FlushInternalSpriteBuffer();
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DTexture->Appear(0);
		
		bool bval = true;
		A3DTLVERTEX * pVerts, vert[10];
		if( !m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**) &pVerts, 0) )
			return false;
		
		for( int j = 0; j < 10; j++ )
		{
			vert[j].x = m_aPos[j].x;
			vert[j].y = m_aPos[j].y;
			vert[j].tu = (m_aPos[j].x - l) / (m_cx - 1);
			vert[j].tv = (m_aPos[j].y - t) / (m_cy - 1);
			vert[j].z = 0.0f;
			vert[j].rhw = 1.0f;
			vert[j].diffuse = m_dwCol;
			vert[j].specular = 0xFF000000;
		}

		pVerts[0] = vert[8];
		pVerts[1] = vert[9];
		pVerts[2] = vert[iSeg];
		int iDst = 3;
		for (int i=(iSeg+1)*3; i < 24; i++, iDst++)
			pVerts[iDst] = vert[m_aIndices[i]];
		
		m_pA3DStream->UnlockVertexBufferDynamic();
		m_pA3DStream->Appear();
		
		bval = m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, iDst / 3);
		if( !bval ) 
			return AUI_ReportError(__LINE__, __FILE__);
		
		m_pA3DTexture->Disappear(0);

		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetLightingEnable(true);
	}
	else
	{
		A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();
		ASSERT(pFC);

		WORD aIndices[24];
		aIndices[0] = 8;
		aIndices[1] = 9;
		aIndices[2] = iSeg;

		int iDst = 3;
		for (int i=(iSeg+1)*3; i < 24; i++, iDst++)
			aIndices[iDst] = m_aIndices[i];

		pFC->AddRenderData_2D(m_aPos, 10, aIndices, iDst, m_dwCol);
	}

	return true;
}

bool AUIClockIcon::RenderUpdown()
{
	if (m_iPos == m_iMax || m_iMin == m_iMax)
	{
		return true;
	}
	else if (m_iPos == m_iMin)
	{
		if( m_pA3DTexture )
		{
			m_pA3DEngine->FlushInternalSpriteBuffer();
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pA3DTexture->Appear(0);
			
			bool bval = true;
			A3DTLVERTEX * pVerts, vert[10];
			if( !m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**) &pVerts, 0) )
				return false;
			
			for( int j = 0; j < 8; j+=2 )
			{
				vert[j].x = m_aPos[j].x;
				vert[j].y = m_aPos[j].y;
				vert[j].z = 0.0f;
				vert[j].rhw = 1.0f;
				vert[j].diffuse = m_dwCol;
				vert[j].specular = 0xFF000000;
			}

			vert[0].tu = vert[2].tu = 1.0f;
			vert[4].tu = vert[6].tu = 0.0f;
			vert[0].tv = vert[6].tv = 0.0f;
			vert[2].tv = vert[4].tv = 1.0f;

			pVerts[0] = vert[0];
			pVerts[1] = vert[2];
			pVerts[2] = vert[4];
			pVerts[3] = vert[0];
			pVerts[4] = vert[4];
			pVerts[5] = vert[6];
			
			m_pA3DStream->UnlockVertexBufferDynamic();
			m_pA3DStream->Appear();
			
			bval = m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);
			if( !bval ) 
				return AUI_ReportError(__LINE__, __FILE__);
			
			m_pA3DTexture->Disappear(0);

			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetLightingEnable(true);
		}
		else
		{
			A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();
			ASSERT(pFC);

			A3DVECTOR3 aPos[4];
			aPos[0] = m_aPos[0];
			aPos[1] = m_aPos[2];
			aPos[2] = m_aPos[4];
			aPos[3] = m_aPos[6];

			static const WORD aIndices[6] = {0, 1, 2, 0, 2, 3};
			pFC->AddRenderData_2D(aPos, 4, aIndices, 6, m_dwCol);
		}
		
		return true;
	}

	float fPortion = m_iPos / float(m_iMax-m_iMin);
	if( m_pA3DTexture )
	{
		m_pA3DEngine->FlushInternalSpriteBuffer();
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DTexture->Appear(0);
		
		bool bval = true;
		A3DTLVERTEX * pVerts, vert[8];
		if( !m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**) &pVerts, 0) )
			return false;
		
		for( int j = 0; j < 8; j+=2 )
		{
			vert[j].x = m_aPos[j].x;
			vert[j].y = m_aPos[j].y;
			vert[j].z = 0.0f;
			vert[j].rhw = 1.0f;
			vert[j].diffuse = m_dwCol;
			vert[j].specular = 0xFF000000;
		}
		if (m_iClockType == AUICLOCK_DOWN)
		{
			vert[0].tu = 1.0f;
			vert[0].tv = 0;
			vert[2].tu = 1.0f;
			vert[2].tv = fPortion;
			vert[2].y = vert[0].y + fPortion * m_cy;
			vert[4].tu = 0;
			vert[4].tv = fPortion;
			vert[4].y = vert[6].y + fPortion * m_cy;
			vert[6].tv = 0;
			vert[6].tu = 0; 
		}
		else
		{
			vert[0].y += m_cy * (1.0f-fPortion);
			vert[0].tv = 1-fPortion;
			vert[0].tu = 1.0f;
			vert[2].tu = 1.0f;
			vert[2].tv = 1.0f;
			vert[4].tu = 0;
			vert[4].tv = 1.0f;
			vert[6].y += m_cy * (1.0f-fPortion);
			vert[6].tv = 1-fPortion;
			vert[6].tu = 0; 
		}
		

		pVerts[0] = vert[0];
		pVerts[1] = vert[2];
		pVerts[2] = vert[4];
		pVerts[3] = vert[0];
		pVerts[4] = vert[4];
		pVerts[5] = vert[6];
		
		m_pA3DStream->UnlockVertexBufferDynamic();
		m_pA3DStream->Appear();
		
		bval = m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);
		if( !bval ) 
			return AUI_ReportError(__LINE__, __FILE__);
		
		m_pA3DTexture->Disappear(0);

		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetLightingEnable(true);
	}
	else
	{
		A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();
			ASSERT(pFC);

			A3DVECTOR3 aPos[4];
			aPos[0] = m_aPos[0];
			aPos[1] = m_aPos[2];
			aPos[2] = m_aPos[4];
			aPos[3] = m_aPos[6];
			aPos[0].y += m_cy*(1-fPortion);
			aPos[3].y += m_cy*(1-fPortion);

			static const WORD aIndices[6] = {0, 1, 2, 0, 2, 3};
			pFC->AddRenderData_2D(aPos, 4, aIndices, 6, m_dwCol);
	}

	return true;
}