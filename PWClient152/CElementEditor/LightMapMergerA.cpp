/*
 * FILE: LightMapMergerA.cpp
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
#include "LightMapMergerA.h"
#include "Bitmap.h"
#include "dds\\ddstrans.h"

#include "A3D.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

using namespace A3DTRN2LOADERA;

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
//	Implement CLightMapMergerA
//	
///////////////////////////////////////////////////////////////////////////

CLightMapMergerA::CLightMapMergerA()
{
	m_pm			= NULL;
	m_fpLM			= NULL;
	m_aLMOffsets	= NULL;
	m_pLMBmp		= NULL;
	m_iLMSize		= 0;
	m_iMaskLMSize	= 0;
	m_pLMBuf		= NULL;
}

CLightMapMergerA::~CLightMapMergerA()
{
}

//	Do export
bool CLightMapMergerA::DoExport(CLightMapMerger* pMerger, bool bDay)
{
	m_pm = pMerger;

	//	Merge lightmaps of all projects
	if (!ExportLightmaps())
	{
		AUX_ProgressLog("CLightMapMergerA::DoExport, Failed to call ExportTerrainData");
		return false;
	}

	return true;
}

//	Export light map
bool CLightMapMergerA::ExportLightmaps()
{
	bool bRet = true;

	//	Create lightmap file
	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s.t2lm", m_pm->m_strDestDir, m_pm->m_strName);

	if (!(m_fpLM = fopen(szFile, "wb+")))
	{
		AUX_ProgressLog("CLightMapMergerA::ExportLightmaps, Failed to create lightmap file %s", szFile);
		return false;
	}

	//	Write lightmap file version and identity
	TRN2FILEIDVER IdVer;
	IdVer.dwIdentify = T2LMFILE_IDENTIFY;
	IdVer.dwVersion	 = T2LMFILE_VERSION;
	fwrite(&IdVer, 1, sizeof (IdVer), m_fpLM);

	//	Fill lightmap file header
	memset(&m_LMFileHeader, 0, sizeof (m_LMFileHeader));
	//	Write file header this time is only to hold place
	fwrite(&m_LMFileHeader, 1, sizeof (m_LMFileHeader), m_fpLM);

	m_iProjectCnt = 0;
	AUX_ResetProgressBar(m_pm->m_iNumProject);

	for (m_iProjRowCnt=0; m_iProjRowCnt < m_pm->m_InitData.iNumRow; m_iProjRowCnt++)
	{
		for (m_iProjColCnt=0; m_iProjColCnt < m_pm->m_InitData.iNumCol; m_iProjColCnt++)
		{
			if (!ExportProject(m_iProjectCnt))
			{
				AUX_ProgressLog("CLightMapMergerA::ExportLightmaps, Failed to export lightmap of project %d", m_iProjectCnt);
				bRet = false;
				goto End;
			}

			m_iProjectCnt++;
		}
	}

	if (!EndExportLightmapFile())
	{
		AUX_ProgressLog("CLightMapMergerA::ExportLightmaps, Failed to call EndExportLightmapFile()");
		bRet = false;
	}

End:

	//	Release resources
	ReleaseExportRes();

	fclose(m_fpLM);

	return bRet;
}

//	Allocate exporting resources
bool CLightMapMergerA::AllocExportRes()
{
	//	Create lightmap object
	if (!(m_pLMBmp = new CELBitmap))
	{
		AUX_ProgressLog("CLightMapMergerA::AllocExportRes, Not enough memory !");
		return false;
	}

	if (!m_pLMBmp->CreateBitmap(m_iLMSize, m_iLMSize, 24, NULL))
	{
		AUX_ProgressLog("CLightMapMergerA::AllocExportRes, Not enough memory !");
		return false;
	}

	m_pLMBmp->Clear();

	if (!(m_pLMBuf = new BYTE [m_iMaskLMSize * m_iMaskLMSize * 3]))
	{
		AUX_ProgressLog("CLightMapMergerA::AllocExportRes, Not enough memory !");
		return false;
	}

	if (!(m_aLMOffsets = new DWORD [m_pm->m_iNumMask]))
	{
		AUX_ProgressLog("CLightMapMergerA::AllocExportRes, Not enough memory !");
		return false;
	}

	memset(m_aLMOffsets, 0, m_pm->m_iNumMask * sizeof (DWORD));

	return true;
}

//	Release resources used during exporting terrain
void CLightMapMergerA::ReleaseExportRes()
{
	if (m_aLMOffsets)
	{
		delete [] m_aLMOffsets;
		m_aLMOffsets = NULL;
	}

	if (m_pLMBuf)
	{
		delete [] m_pLMBuf;
		m_pLMBuf = NULL;
	}

	if (m_pLMBmp)
	{
		m_pLMBmp->Release();
		delete m_pLMBmp;
		m_pLMBmp = NULL;
	}
}

//	Export lightmap data of a project
bool CLightMapMergerA::ExportProject(int iProject)
{
	CString strProject = *m_pm->m_aProjNames[iProject];

	//	Make project lightmap file name
	CString strFile;
	strFile.Format("%s%s\\%s\\%srender.t2lm", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	
	bool bHasLightmap = true;
	FILE* pLMFile = fopen(strFile, "rb");
	int iNumMaskArea, iMaskLMSize;

	if (!pLMFile)
	{
		AUX_ProgressLog("CLightMapMergerA::ExportProject, Couldn't open project (%d) lightmap file", iProject);
		bHasLightmap = false;
	}
	else
	{
		//	Read lightmap file version and Identity
		TRN2FILEIDVER IdVer;
		if (fread(&IdVer, 1, sizeof (IdVer), pLMFile) != sizeof (IdVer))
		{
			AUX_ProgressLog("CLightMapMergerA::ExportProject, project (%d) lightmap file is corrupt", iProject);
			bHasLightmap = false;
			goto DoExport;
		}

		if (IdVer.dwIdentify != T2LMFILE_IDENTIFY || IdVer.dwVersion > T2LMFILE_VERSION)
		{
			AUX_ProgressLog("CLightMapMergerA::ExportProject, project (%d) lightmap file identity or version error", iProject);
			bHasLightmap = false;
			goto DoExport;
		}

		if (IdVer.dwVersion == 1)
		{
			T2LMFILEHEADER Header;
			fread(&Header, 1, sizeof (Header), pLMFile);
			iNumMaskArea = Header.iNumMaskArea;
			iMaskLMSize	 = Header.iMaskLMSize;
		}
		else	//	IdVer.dwVersion >= 2
		{
			T2LMFILEHEADER2 Header;
			fread(&Header, 1, sizeof (Header), pLMFile);
	
			//	Source lightmap file shouldn't be DDS format
			if (Header.dwFlags & T2LMFILE_DDS)
			{
				AUX_ProgressLog("CLightMapMergerA::ExportProject, project (%d) lightmap is DDS format", iProject);
				bHasLightmap = false;
				goto DoExport;
			}

			iNumMaskArea = Header.iNumMaskArea;
			iMaskLMSize	 = Header.iMaskLMSize;
		}

		if (iProject)
		{
			//	Isn't the first project, check lightmap size
			if (m_iLMSize != CLightMapMerger::CalcLightmapSize(iNumMaskArea, iMaskLMSize))
			{
				AUX_ProgressLog("CLightMapMergerA::ExportProject, project (%d) mask area number error", iProject);
				bHasLightmap = false;
			}
		}
	}

DoExport:

	//	This is the first project ?
	if (!iProject)
	{
		if (!bHasLightmap)
		{
			if (pLMFile)
				fclose(pLMFile);

			AUX_ProgressLog("CLightMapMergerA::ExportProject, First project's lightmap error, couldn't continue");
			return false;
		}

		//	This is the first project
		m_iLMSize		= CLightMapMerger::CalcLightmapSize(iNumMaskArea, iMaskLMSize);
		m_iMaskLMSize	= m_iLMSize / m_pm->m_InitData.iProjMaskCol;

		m_LMFileHeader.iNumMaskArea	= m_pm->m_iNumMask;
		m_LMFileHeader.iMaskLMSize	= m_iMaskLMSize;
		m_LMFileHeader.dwFlags		= T2LMFILE_DDS;
		m_LMFileHeader.iLMDataSize	= 0;

		//	Allocate resource used to exporting
		if (!AllocExportRes())
		{
			fclose(pLMFile);
			return false;
		}

		//	Write data to file to hold place
		fwrite(m_aLMOffsets, 1, m_pm->m_iNumMask * sizeof (DWORD), m_fpLM);
	}
	else
	{
		if (!bHasLightmap)
		{
			if (pLMFile)
			{
				fclose(pLMFile);
				pLMFile = NULL;
			}
		}
	}

	//	If we havn't lightmap or load lightmap failed, fill it with white color
	if (!bHasLightmap || !CLightMapMerger::ReadLightmapData(m_pLMBmp, pLMFile, iNumMaskArea, iMaskLMSize))
		m_pm->FillWhiteLightmap(m_pLMBmp);

	if (pLMFile)
		fclose(pLMFile);

	//	Generate this sub-terrain's lightmap
	GenProjectLightmap(iProject);

	//	Step progress bar
	AUX_StepProgressBar(1);

	return true;
}

//	Generate project lightmap
bool CLightMapMergerA::GenProjectLightmap(int iProject)
{
	int iMaskCol = m_iProjColCnt * m_pm->m_InitData.iProjMaskCol;
	int iMaskRow = m_iProjRowCnt * m_pm->m_InitData.iProjMaskRow;
	int iMaskPitch = m_pm->m_InitData.iNumCol * m_pm->m_InitData.iProjMaskCol;

	ARectI rcLock;
	CELBitmap::LOCKINFO LockInfo;
	int r, c, iLineDataLen = m_iMaskLMSize * 3;

	for (r=0; r < m_pm->m_InitData.iProjMaskRow; r++)
	{
		int iMaskArea = (iMaskRow + r) * iMaskPitch + iMaskCol;

		rcLock.left		= 0;
		rcLock.right	= m_iMaskLMSize;
		rcLock.top		= r * m_iMaskLMSize;
		rcLock.bottom	= rcLock.top + m_iMaskLMSize;

		for (c=0; c < m_pm->m_InitData.iProjMaskCol; c++, iMaskArea++)
		{
			m_pLMBmp->LockRect(rcLock, &LockInfo);

			//	Pick-up mask lightmap data from whole sub-terrain lightmap
			BYTE* pData1 = LockInfo.pData;
			BYTE* pData2 = m_pLMBuf;
			for (int i=0; i < LockInfo.iHeight; i++)
			{
				memcpy(pData2, pData1, iLineDataLen);
				pData1 += LockInfo.iPitch;
				pData2 += iLineDataLen;
			}

			//	Compress lightmap
			int iDataSize;
			BYTE* pDDSData = DdsCompressBGR(m_pLMBuf, iDataSize, m_iMaskLMSize, 
				m_iMaskLMSize, 3, m_iMaskLMSize, kDXT1, dNoMipMaps, 1);
			if (!pDDSData)
			{
				AUX_ProgressLog("CLightMapMergerA::ExportLightmaps, Failed to compress texture to dds format");
				return false;
			}

			if (!m_LMFileHeader.iLMDataSize)
				m_LMFileHeader.iLMDataSize = iDataSize;
			else
			{
				ASSERT(m_LMFileHeader.iLMDataSize == iDataSize);
			}

			//	Record lightmap offset
			m_aLMOffsets[iMaskArea] = ftell(m_fpLM);
			//	Save lightmap data
			fwrite(pDDSData, iDataSize, 1, m_fpLM);
			ReleaseData(pDDSData);

			rcLock.left	 += m_iMaskLMSize;
			rcLock.right += m_iMaskLMSize;
		}
	}

	return true;
}

//	End exporting lightmap file
bool CLightMapMergerA::EndExportLightmapFile()
{
	//	Rewrite file header
	fseek(m_fpLM, sizeof (TRN2FILEIDVER), SEEK_SET);
	fwrite(&m_LMFileHeader, 1, sizeof (m_LMFileHeader), m_fpLM);

	//	Rewrite mask area lightmap data offsets
	fwrite(m_aLMOffsets, 1, m_pm->m_iNumMask * sizeof (DWORD), m_fpLM);

	return true;
}

