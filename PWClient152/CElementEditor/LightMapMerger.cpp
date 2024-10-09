/*
 * FILE: LightMapMerger.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "LightMapMerger.h"
#include "LightMapMergerB.h"
#include "LightMapMergerA.h"
#include "Bitmap.h"

#include "A3D.h"

//#define new A_DEBUG_NEW

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

extern int g_iTrnExport;

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
//	Implement CLightMapMerger
//	
///////////////////////////////////////////////////////////////////////////

CLightMapMerger::CLightMapMerger()
{
	m_iNumProject	= 0;
	m_iNumProjMask	= 0;
	m_iNumMask		= 0;
}

CLightMapMerger::~CLightMapMerger()
{
}

//	Initialize object
bool CLightMapMerger::Init(const INITDATA& InitData)
{
	m_strName		= InitData.szName;
	m_strDestDir	= InitData.szDestDir;	
	m_InitData		= InitData;

	m_iNumProject	= InitData.iNumRow * InitData.iNumCol;
	m_iNumProjMask	= InitData.iProjMaskCol * InitData.iProjMaskRow;
	m_iNumMask		= m_iNumProjMask * m_iNumProject;

	return true;
}

//	Release object
void CLightMapMerger::Release()
{
	//	Release all project name
	for (int i=0; i < m_aProjNames.GetSize(); i++)
		delete m_aProjNames[i];

	m_aProjNames.RemoveAll();
}

//	Add project name
void CLightMapMerger::AddProject(const char* szName)
{
	CString* pstrName = new CString(szName);
	if (!pstrName)
		return;

	m_aProjNames.Add(pstrName);
}

//	Do export
bool CLightMapMerger::DoExport()
{
	int iNumProj = m_InitData.iNumCol * m_InitData.iNumRow;
	if (!iNumProj || !m_aProjNames.GetSize() || m_aProjNames.GetSize() != iNumProj)
	{
		AUX_ProgressLog("CLightMapMerger::DoExport, Wrong number of project name.");
		return false;
	}

	//	Create destination directory
	if (!AUX_CreateDirectory(m_strDestDir))
	{
		AUX_ProgressLog("CLightMapMerger::DoExport, Failed to create export directory.");
		return false;
	}

	if (!g_iTrnExport)
	{
		AUX_ProgressLog("-- Export lightmaps for day ...");

		CLightMapMergerA ExpImp;
		if (!ExpImp.DoExport(this, true))
		{
			AUX_ProgressLog("CLightMapMerger::DoExport, Failed to export lightmap data");
			return false;
		}
	}
	else if (g_iTrnExport == 1)
	{
		AUX_ProgressLog("-- Export lightmaps for day ...");

		CLightMapMergerB ExpImp;
		if (!ExpImp.DoExport(this, true))
		{
			AUX_ProgressLog("CLightMapMerger::DoExport, Failed to export lightmap data");
			return false;
		}

		AUX_ProgressLog("-- Export lightmaps for night ...");
		if (!ExpImp.DoExport(this, false))
		{
			AUX_ProgressLog("CLightMapMerger::DoExport, Failed to export lightmap data");
			return false;
		}
	}
	else
	{
		ASSERT(0);
		AUX_ProgressLog("CLightMapMerger::DoExport, No lightmap export imp was found");
		return false;
	}

	//AUX_ProgressLog("-- All done!");

	return true;
}

//	Calculate lightmap size
int CLightMapMerger::CalcLightmapSize(int iMaskNum, int iMaskGrid)
{
	int iMaskCol = (int)(sqrt((float)iMaskNum) + 0.5f);
	return iMaskCol * iMaskGrid;
}

//	Fill lightmap with white color
void CLightMapMerger::FillWhiteLightmap(CELBitmap* pBmp)
{
	if (!pBmp)
		return;

	int iSize = pBmp->GetPitch() * pBmp->GetHeight();
	memset(pBmp->GetBitmapData(), 0xff, iSize);
}

//	Read lightmap data from .t2lm file
bool CLightMapMerger::ReadLightmapData(CELBitmap* pBmp, FILE* fp, int iMaskNum, int iMaskLMSize)
{
	ASSERT(pBmp);

	int iMaskLMDataLen = iMaskLMSize * iMaskLMSize * 3;

	BYTE* pMaskLMData = new BYTE [iMaskLMDataLen];
	if (!pMaskLMData)
	{
		AUX_ProgressLog("CLightMapMergerA::ReadLightmapData, Not enough memory !");
		return false;
	}

	DWORD* aOffsets = new DWORD [iMaskNum];
	if (!aOffsets)
	{
		delete [] pMaskLMData;
		AUX_ProgressLog("CLightMapMergerA::ReadLightmapData, Not enough memory !");
		return false;
	}
	
	//	Read lightmap offset
	fread(aOffsets, 1, iMaskNum * sizeof (DWORD), fp);

	int iMaskCol = (int)(sqrt((float)iMaskNum) + 0.5f);
	int iMaskRow = iMaskCol;
	int iLineDataLen = iMaskLMSize * 3;

	ASSERT(pBmp && pBmp->GetWidth() == iMaskCol * iMaskLMSize && pBmp->GetHeight() == iMaskRow * iMaskLMSize);

	ARectI rcLock;
	int r, c, iMaskArea;
	CELBitmap::LOCKINFO LockInfo;

	for (r=0; r < iMaskRow; r++)
	{
		rcLock.left		= 0;
		rcLock.right	= iMaskLMSize;
		rcLock.top		= r * iMaskLMSize;
		rcLock.bottom	= rcLock.top + iMaskLMSize;

		iMaskArea = r * iMaskCol;

		for (c=0; c < iMaskCol; c++, iMaskArea++)
		{
			//	Read lightmap data of this mask area from file
			fseek(fp, aOffsets[iMaskArea], SEEK_SET);
			fread(pMaskLMData, 1, iMaskLMDataLen, fp);

			pBmp->LockRect(rcLock, &LockInfo);

			//	Copy lightmap data
			BYTE* pDst = LockInfo.pData;
			BYTE* pSrc = pMaskLMData;

			for (int i=0; i < LockInfo.iHeight; i++)
			{
				memcpy(pDst, pSrc, iLineDataLen);

				pSrc += iLineDataLen;
				pDst += LockInfo.iPitch;
			}

			rcLock.left	 += iMaskLMSize;
			rcLock.right += iMaskLMSize;	
		}
	}

	delete [] pMaskLMData;
	delete [] aOffsets;

	//	test code
//	pBmp->SaveToFile("G:\\Develop\\Element\\Bin\\Temp\\src_lightmap.bmp");

	return true;
}
