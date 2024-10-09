/*
 * FILE: TerrainExpImpA.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "TerrainExpImpA.h"
#include "TerrainExportor.h"
#include "ExpElementMap.h"
#include "ExpTerrain.h"
#include "TerrainLayer.h"

#include "A3D.h"
#include "A3DTerrain2Loader.h"

//#define new A_DEBUG_NEW

using namespace A3DTRN2LOADERA;

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
//	Implement CTerrainExpImpA
//	
///////////////////////////////////////////////////////////////////////////

CTerrainExpImpA::CTerrainExpImpA()
{
	m_pe			= NULL;
	m_fpTerrain		= NULL;
	m_fpMaskMap		= NULL;
	m_aMaskAreas	= NULL;
	m_iProjMaskArea	= 0;
	m_iProjBlock	= 0;
	m_aBlockOffs	= NULL;
	m_aMaskAreaOffs	= NULL;
	m_aSubTerrains	= NULL;
	m_fProjTrnSize	= 0.0f;
	m_iProjHMSize	= 0;
	m_iProjMaskCol	= 0;
	m_iProjMaskRow	= 0;
}

CTerrainExpImpA::~CTerrainExpImpA()
{
}

//	Do export
bool CTerrainExpImpA::DoExport(CTerrainExportor* pExporter, TRN2FILEHEADER* pTrnInfo)
{
	m_pe = pExporter;

	if (!ExportTerrainData())
		return false;

	if (pTrnInfo)
		*pTrnInfo = m_T2FileHeader;

	return true;
}

//	Release resources used during exporting terrain
void CTerrainExpImpA::ReleaseTerrainExportRes()
{
	//	Release all texture file name
	for (int i=0; i < m_aTextureFile.GetSize(); i++)
		delete m_aTextureFile[i];

	m_aTextureFile.RemoveAll();

	if (m_aSubTerrains)
	{
		delete [] m_aSubTerrains;
		m_aSubTerrains = NULL;
	}

	if (m_aMaskAreas)
	{
		delete [] m_aMaskAreas;
		m_aMaskAreas = NULL;
	}

	if (m_aBlockOffs)
	{
		delete [] m_aBlockOffs;
		m_aBlockOffs = NULL;
	}

	if (m_aMaskAreaOffs)
	{
		delete [] m_aMaskAreaOffs;
		m_aMaskAreaOffs = NULL;
	}

	m_aTexIndices.RemoveAll();
	m_aSpecuMaps.RemoveAll();
	m_aMaskSizes.RemoveAll();
	m_aMaskMapOffs.RemoveAll();
	m_aLayerMaps.RemoveAll();
	m_aProjAxises.RemoveAll();
	m_aUVScales.RemoveAll();
	m_aLayerWeights.RemoveAll();
}

//	Add a texture file name and return texture's index
int CTerrainExpImpA::AddTextureFile(const char* szFile)
{
	if (!szFile || !szFile[0])
		return -1;
	
	//	Check whether this texture has existed
	for (int i=0; i < m_aTextureFile.GetSize(); i++)
	{
		if (!m_aTextureFile[i]->CompareNoCase(szFile))
			return i;
	}

	CString* pstrName = new CString(szFile);
	if (!pstrName)
		return -1;

	return m_aTextureFile.Add(pstrName);
}

//	Allocate exporting terrain resources
bool CTerrainExpImpA::AllocTerrainExportRes()
{
	//	Sub terrain information
	if (!(m_aSubTerrains = new TRN2FILESUBTRN [m_pe->m_iProjNum]))
	{
		AUX_ProgressLog("CTerrainExpImpA::AllocTerrainExportRes, Not enough memory !");
		return false;
	}

	//	Create layer map pairs
	if (!(m_aMaskAreas = new T2MKFILEMASKAREA [m_T2FileHeader.iNumMaskArea]))
	{
		AUX_ProgressLog("CTerrainExpImpA::AllocTerrainExportRes, Not enough memory !");
		return false;
	}

	//	Create block data offsets array
	if (!(m_aBlockOffs = new DWORD [m_T2FileHeader.iNumBlock]))
	{
		AUX_ProgressLog("CTerrainExpImpA::AllocTerrainExportRes, Not enough memory !");
		return false;
	}

	//	Create mask area data offsets array
	if (!(m_aMaskAreaOffs = new DWORD [m_T2FileHeader.iNumMaskArea]))
	{
		AUX_ProgressLog("CTerrainExpImpA::AllocTerrainExportRes, Not enough memory !");
		return false;
	}

	memset(m_aSubTerrains, 0, m_pe->m_iProjNum * sizeof (TRN2FILESUBTRN));
	memset(m_aMaskAreas, 0, m_T2FileHeader.iNumMaskArea * sizeof (T2MKFILEMASKAREA));
	memset(m_aBlockOffs, 0, m_T2FileHeader.iNumBlock * sizeof (DWORD));
	memset(m_aMaskAreaOffs, 0, m_T2FileHeader.iNumMaskArea * sizeof (DWORD));

	//	Create lumps
	int iEstimate = m_T2FileHeader.iNumMaskArea * 8;
	m_aTexIndices.SetSize(iEstimate, 500);
	m_aSpecuMaps.SetSize(iEstimate, 500);
	m_aMaskSizes.SetSize(iEstimate, 500);
	m_aMaskMapOffs.SetSize(iEstimate, 500);
	m_aLayerMaps.SetSize(iEstimate, 500);
	m_aProjAxises.SetSize(iEstimate, 500);
	m_aUVScales.SetSize(iEstimate, 500);
	m_aLayerWeights.SetSize(iEstimate, 500);

	//	Below lines only clear element number
	m_aTexIndices.RemoveAll(false);
	m_aSpecuMaps.RemoveAll(false);
	m_aMaskSizes.RemoveAll(false);
	m_aMaskMapOffs.RemoveAll(false);
	m_aLayerMaps.RemoveAll(false);
	m_aProjAxises.RemoveAll(false);
	m_aUVScales.RemoveAll(false);
	m_aLayerWeights.RemoveAll(false);
	
	return true;
}

//	Export terrain data
bool CTerrainExpImpA::ExportTerrainData()
{
	//AUX_ProgressLog("-- Export terrain data ...");

	bool bRet = true;

	//	Create terrain file
	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s.trn2", m_pe->m_strDestDir, m_pe->m_strName);

	if (!(m_fpTerrain = fopen(szFile, "wb+")))
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportTerrainData, Failed to create terrain file %s", szFile);
		return false;
	}

	//	Create mask map file
	AUX_ChangeExtension(szFile, "t2mk");

	if (!(m_fpMaskMap = fopen(szFile, "wb+")))
	{
		fclose(m_fpTerrain);
		AUX_ProgressLog("CTerrainExpImpA::ExportTerrainData, Failed to create mask map file %s", szFile);
		return false;
	}

	//	Write file version and identity of terrain file
	TRN2FILEIDVER IdVer;
	IdVer.dwIdentify = TRN2FILE_IDENTIFY;
	IdVer.dwVersion	 = TRN2FILE_VERSION;
	fwrite(&IdVer, 1, sizeof (IdVer), m_fpTerrain);

	//	Fill terrain file header
	memset(&m_T2FileHeader, 0, sizeof (m_T2FileHeader));
	m_T2FileHeader.iBlockGrid	= m_pe->m_InitData.iBlockGrid;
	m_T2FileHeader.iMaskGrid	= m_pe->m_InitData.iMaskTile;
	m_T2FileHeader.iMaskBlock	= m_pe->m_InitData.iMaskTile / m_pe->m_InitData.iBlockGrid;
	m_T2FileHeader.iNumSubTrn	= m_pe->m_aProjNames.GetSize();
	m_T2FileHeader.iSubTrnRow	= m_pe->m_InitData.iNumRow;
	m_T2FileHeader.iSubTrnCol	= m_pe->m_InitData.iNumCol;
	m_T2FileHeader.iLoaderID	= A3DTerrain2Loader::LOADER_A;
	//	Write file header this time is only to hold place
	fwrite(&m_T2FileHeader, 1, sizeof (m_T2FileHeader), m_fpTerrain);

	//	write file version and identity of mask map file
	IdVer.dwIdentify = T2MKFILE_IDENTIFY;
	IdVer.dwVersion	 = T2MKFILE_VERSION;
	fwrite(&IdVer, 1, sizeof (IdVer), m_fpMaskMap);

	//	Fill mask map file header
	memset(&m_MKFileHeader, 0, sizeof (m_MKFileHeader));
	//	Write file header this time is only to hold place
	fwrite(&m_MKFileHeader, 1, sizeof (m_MKFileHeader), m_fpMaskMap);

	m_iProjectCnt = 0;

	for (m_iProjRowCnt=0; m_iProjRowCnt < m_pe->m_InitData.iNumRow; m_iProjRowCnt++)
	{
		for (m_iProjColCnt=0; m_iProjColCnt < m_pe->m_InitData.iNumCol; m_iProjColCnt++)
		{
			if (!ExportProject(m_iProjectCnt))
			{
				AUX_ProgressLog("CTerrainExpImpA::ExportTerrainData, Failed to export project %d", m_iProjectCnt);
				bRet = false;
				goto End;
			}

			m_iProjectCnt++;
		}
	}

	//	Write lumps in mask map file
	if (!ExportMaskMapLumps())
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportTerrainData, Failed to export mask map lumps");
		bRet = false;
		goto End;
	}

	//	End export mask map file
	if (!EndExportMaskMapFile())
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportTerrainData, Failed to call EndExportMaskMapFile()");
		bRet = false;
		goto End;
	}

	if (!EndExportTerrainFile())
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportTerrainData, Failed to call EndExportTerrainFile()");
		bRet = false;
	}

End:

	//	Release resources
	ReleaseTerrainExportRes();

	fclose(m_fpTerrain);
	fclose(m_fpMaskMap);

	return bRet;
}

//	End export mask map file
bool CTerrainExpImpA::EndExportMaskMapFile()
{
	//	Rewrite mask map file header
	fseek(m_fpMaskMap, sizeof (TRN2FILEIDVER), SEEK_SET);
	fwrite(&m_MKFileHeader, 1, sizeof (m_MKFileHeader), m_fpMaskMap);

	//	Rewrite mask area information offsets
	fwrite(m_aMaskAreaOffs, 1, m_T2FileHeader.iNumMaskArea * sizeof (DWORD), m_fpMaskMap);

	return true;
}

//	End export terrain file
bool CTerrainExpImpA::EndExportTerrainFile()
{
	m_T2FileHeader.iNumTexture	= m_aTextureFile.GetSize();
	m_T2FileHeader.dwTexOffset	= ftell(m_fpTerrain);

	//	Write texture file names
	for (int i=0; i < m_aTextureFile.GetSize(); i++)
		AUX_WriteString(m_fpTerrain, *m_aTextureFile[i]);

	//	Record loader section offset
	m_T2FileHeader.dwLoaderOff	= ftell(m_fpTerrain);

	//	Rewrite terrain file header
	fseek(m_fpTerrain, sizeof (TRN2FILEIDVER), SEEK_SET);
	fwrite(&m_T2FileHeader, 1, sizeof (m_T2FileHeader), m_fpTerrain);

	//	Rewrite sub-terrain infromation
	fwrite(m_aSubTerrains, 1, m_pe->m_iProjNum * sizeof (TRN2FILESUBTRN), m_fpTerrain);

	//	Rewrite block data offsets
	fwrite(m_aBlockOffs, 1, m_T2FileHeader.iNumBlock * sizeof (DWORD), m_fpTerrain);

	return true;
}

//	Export terrain data of a project
bool CTerrainExpImpA::ExportProject(int iProject)
{
	CString strProject = *m_pe->m_aProjNames[iProject];

	AUX_ProgressLog("-- Exporting terrain data of project [%d/%d] %s ...", iProject+1, m_pe->m_iProjNum, strProject);
	//AUX_ProgressLog("-- Loading terrain data ...");

	//	Make project's file name
	CString strFile;
	strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProject, strProject);

	//	Load project
	CExpElementMap Map;
	if (!Map.Load(strFile, LOAD_EXPTERRAIN))
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportProject, Failed to call CExpElementMap::Load");
		return false;
	}

	CExpTerrain* pExpTerrain = (CExpTerrain*)Map.GetTerrain();
	ASSERT(pExpTerrain);

	//	Is the first project ?
	if (!iProject)
	{
		//	Fill corresponding information in file header
		CTerrain::HEIGHTMAP* pHeightMap = pExpTerrain->GetHeightMap();
		
		m_fProjTrnSize	= pExpTerrain->GetTerrainSize();
		m_iProjHMSize	= pHeightMap->iWidth;

		m_T2FileHeader.fTerrainWid	= m_fProjTrnSize * m_pe->m_InitData.iNumCol;
		m_T2FileHeader.fTerrainHei	= m_fProjTrnSize * m_pe->m_InitData.iNumRow;
		m_T2FileHeader.fBlockSize	= pExpTerrain->GetTileSize() * m_pe->m_InitData.iBlockGrid;
		m_T2FileHeader.iTrnBlkRow	= (pHeightMap->iHeight - 1) / m_pe->m_InitData.iBlockGrid * m_pe->m_InitData.iNumRow;
		m_T2FileHeader.iTrnBlkCol	= (pHeightMap->iWidth - 1) / m_pe->m_InitData.iBlockGrid * m_pe->m_InitData.iNumCol;
		m_T2FileHeader.iNumBlock	= m_T2FileHeader.iTrnBlkRow * m_T2FileHeader.iTrnBlkCol;
		m_T2FileHeader.iSubTrnGrid	= pHeightMap->iWidth - 1;
		m_T2FileHeader.fGridSize	= pExpTerrain->GetTileSize();

		m_iProjBlock	= (pHeightMap->iHeight - 1) / m_pe->m_InitData.iBlockGrid * (pHeightMap->iWidth - 1) / m_pe->m_InitData.iBlockGrid;
		m_iProjMaskCol	= (pHeightMap->iWidth - 1) / m_pe->m_InitData.iMaskTile;
		m_iProjMaskRow	= (pHeightMap->iHeight - 1) / m_pe->m_InitData.iMaskTile;
		m_iProjMaskArea	= m_iProjMaskCol * m_iProjMaskRow;

		m_T2FileHeader.iNumMaskArea = m_iProjMaskArea * m_pe->m_InitData.iNumCol * m_pe->m_InitData.iNumRow;
		m_MKFileHeader.iNumMaskArea	= m_T2FileHeader.iNumMaskArea;

		//	Allocate exporting terrain resources
		if (!AllocTerrainExportRes())
		{
			AUX_ProgressLog("CTerrainExpImpA::ExportProject, Failed to call AllocExportRes() !");
			goto Error;
		}

		//	Write data to file to hold place
		fwrite(m_aSubTerrains, 1, m_pe->m_iProjNum * sizeof (TRN2FILESUBTRN), m_fpTerrain);
		fwrite(m_aBlockOffs, 1, m_T2FileHeader.iNumBlock * sizeof (DWORD), m_fpTerrain);
		fwrite(m_aMaskAreaOffs, 1, m_T2FileHeader.iNumMaskArea * sizeof (DWORD), m_fpMaskMap);
	}
	else	//	Isn't the first project
	{
		//	Check project information
		CTerrain::HEIGHTMAP* pHeightMap = pExpTerrain->GetHeightMap();

		if (pExpTerrain->GetTerrainSize() != m_fProjTrnSize ||
			pHeightMap->iWidth != m_iProjHMSize)
		{
			AUX_ProgressLog("CTerrainExpImpA::ExportProject, Project %d terrain has different size !");
			goto Error;
		}
	}

	m_aSubTerrains[iProject].fMinHei	= pExpTerrain->GetYOffset();
	m_aSubTerrains[iProject].fMaxHei	= pExpTerrain->GetYOffset() + pExpTerrain->GetMaxHeight();

	//AUX_ProgressLog("-- Export mask areas ...");
	AUX_ResetProgressBar(m_iProjMaskArea);

	//	Export mask areas
	if (!ExportProjectMaskAreas(pExpTerrain))
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportProject, Project %d failed to call ExportProjectMaskAreas() !");
		goto Error;
	}

	//AUX_ProgressLog("-- Export terrain blocks ...");
	AUX_ResetProgressBar(m_iProjBlock);

	//	Export blocks
	if (!ExportProjectBlocks(pExpTerrain))
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportProject, Project %d failed to call ExportProjectBlocks() !");
		goto Error;
	}

	Map.Release();

	return true;

Error:

	Map.Release();

	return false;
}

//	Export all mask map areas in a project terrain
bool CTerrainExpImpA::ExportProjectMaskAreas(CExpTerrain* pTerrain)
{
	int iProjMask = (m_iProjHMSize-1) / m_pe->m_InitData.iMaskTile;
	int iTrnMaskPitch = iProjMask * m_pe->m_InitData.iNumCol;
	int iBaseRow = m_iProjRowCnt * iProjMask;
	int iBaseCol = m_iProjColCnt * iProjMask;

	for (int r=0; r < iProjMask; r++)
	{
		int iMaskArea = (iBaseRow + r) * iTrnMaskPitch + iBaseCol;

		for (int c=0; c < iProjMask; c++, iMaskArea++)
		{
			if (!ExportProjectMaskArea(pTerrain, iMaskArea, r, c))
				return false;

			//	Step progress bar
			AUX_StepProgressBar(1);
		}
	}

	return true;
}

/*	Export one mask map area in a project terrain

	pTerrain: current project's terrain
	iMaskArea: mask area's index in whole exporting terrain
	iRow, iCol: row and column in current project
*/
bool CTerrainExpImpA::ExportProjectMaskArea(CExpTerrain* pTerrain, int iMaskArea,
											 int iRow, int iCol)
{
	ARectI rcMap;
	rcMap.left		= iCol * m_pe->m_InitData.iMaskTile;
	rcMap.top		= iRow * m_pe->m_InitData.iMaskTile;
	rcMap.right		= rcMap.left + m_pe->m_InitData.iMaskTile;
	rcMap.bottom	= rcMap.top + m_pe->m_InitData.iMaskTile;

	//	Get all mask map data in this mask area
	CExpTerrain::MASKMAPDATA* aMaskData = pTerrain->GetMaskAreaInfo(rcMap);
	if (!aMaskData)
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportProjectMaskArea, failed to call CExpTerrain::GetMaskAreaInfo !");
		return false;
	}

	//	Record mask area data's offset in file
	DWORD dwOffset = ftell(m_fpMaskMap);
	m_aMaskAreaOffs[iMaskArea] = dwOffset;

	int i;
	CString strFile;

	//	Throw off layers which have pure black mask map
	T2MKFILEMASKAREA& FileMaskArea = m_aMaskAreas[iMaskArea];
	FileMaskArea.iNumLayer		= 0;
	FileMaskArea.iFirstIndex	= m_aTexIndices.GetSize();

	//	Skip information block
	dwOffset += sizeof (FileMaskArea);

	for (i=0; i < pTerrain->GetLayerNum(); i++)
	{
		const CExpTerrain::MASKMAPDATA& LayerMask = aMaskData[i];
		CTerrainLayer* pLayer = pTerrain->GetLayer(i);

		if (pTerrain->IsPureBlackMask(LayerMask))
			continue;	//	Throw layer which has pure black mask
		
		m_aLayerMaps.Add(i);
		m_aMaskSizes.Add(LayerMask.iWidth);
		m_aTexIndices.Add(AddTextureFile(pLayer->GetTextureFile()));
		m_aSpecuMaps.Add(AddTextureFile(pLayer->GetSpecularMapFile()));
		m_aMaskMapOffs.Add(dwOffset);
		m_aProjAxises.Add((BYTE)pLayer->GetProjAxis());
		m_aLayerWeights.Add((BYTE)pLayer->GetLayerWeight());
		
		T2MKFILEUVSCALE uvScale;
		uvScale.su = pLayer->GetScaleU();
		uvScale.sv = pLayer->GetScaleV();
		m_aUVScales.Add(uvScale);

		dwOffset += LayerMask.iWidth * LayerMask.iHeight;

		FileMaskArea.iNumLayer++;
	}

	//	Write mask map area information to file
	fwrite(&FileMaskArea, 1, sizeof (FileMaskArea), m_fpMaskMap);

	//	Write mask map image data
	for (i=0; i < FileMaskArea.iNumLayer; i++)
	{
		//	Get real layer index
		int iLayer = m_aLayerMaps[FileMaskArea.iFirstIndex+i];
		const CExpTerrain::MASKMAPDATA& LayerMask = aMaskData[iLayer];
		if (!LayerMask.pData)	//	Default layer doesn't has mask
			continue;

		BYTE* pLineData = LayerMask.pData;

		for (int r=0; r < LayerMask.iHeight; r++)
		{
			fwrite(pLineData, 1, LayerMask.iWidth * sizeof (BYTE), m_fpMaskMap);
			pLineData += LayerMask.iPitch;
		}
	}

	//	Release resources
	pTerrain->ReleaseMaskAreaInfo(aMaskData);

	return true;
}

//	Export lumps in mask map file
bool CTerrainExpImpA::ExportMaskMapLumps()
{
	DWORD dwSize;

	//	Write texture index lump
	dwSize = m_aTexIndices.GetSize() * sizeof (int);
	m_MKFileHeader.aLumps[T2MKLUMP_TEXTURE].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_TEXTURE].dwSize	= dwSize;
	if (fwrite(m_aTexIndices.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	//	Write specular map index lump
	dwSize = m_aSpecuMaps.GetSize() * sizeof (int);
	m_MKFileHeader.aLumps[T2MKLUMP_SPECUMAP].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_SPECUMAP].dwSize	= dwSize;
	if (fwrite(m_aSpecuMaps.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	//	Write mask image size lump
	dwSize = m_aMaskSizes.GetSize() * sizeof (int);
	m_MKFileHeader.aLumps[T2MKLUMP_MASKSIZE].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_MASKSIZE].dwSize	= dwSize;
	if (fwrite(m_aMaskSizes.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	//	Write mask image offset lump
	dwSize = m_aMaskMapOffs.GetSize() * sizeof (DWORD);
	m_MKFileHeader.aLumps[T2MKLUMP_OFFSET].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_OFFSET].dwSize	= dwSize;
	if (fwrite(m_aMaskMapOffs.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	//	Write projection axis lump
	dwSize = m_aProjAxises.GetSize() * sizeof (BYTE);
	m_MKFileHeader.aLumps[T2MKLUMP_PROJAXIS].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_PROJAXIS].dwSize	= dwSize;
	if (fwrite(m_aProjAxises.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	//	Write uv-scale axis lump
	dwSize = m_aUVScales.GetSize() * sizeof (T2MKFILEUVSCALE);
	m_MKFileHeader.aLumps[T2MKLUMP_UVSCALE].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_UVSCALE].dwSize	= dwSize;
	if (fwrite(m_aUVScales.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	//	Write layer weight lump
	dwSize = m_aLayerWeights.GetSize() * sizeof (BYTE);
	m_MKFileHeader.aLumps[T2MKLUMP_WEIGHT].dwOff	= ftell(m_fpMaskMap);
	m_MKFileHeader.aLumps[T2MKLUMP_WEIGHT].dwSize	= dwSize;
	if (fwrite(m_aLayerWeights.GetData(), 1, dwSize, m_fpMaskMap) != dwSize)
		return false;

	return true;
}

//	Export blocks in a project terrain
bool CTerrainExpImpA::ExportProjectBlocks(CExpTerrain* pTerrain)
{
	int iProjBlock = (m_iProjHMSize-1) / m_pe->m_InitData.iBlockGrid;
	int iBaseRow = m_iProjRowCnt * iProjBlock;
	int iBaseCol = m_iProjColCnt * iProjBlock;

	for (int r=0; r < iProjBlock; r++)
	{
		int iBlock = (iBaseRow + r) * m_T2FileHeader.iTrnBlkCol + iBaseCol;

		for (int c=0; c < iProjBlock; c++, iBlock++)
		{
			if (!ExportProjectBlock(pTerrain, iBaseRow+r, iBaseCol+c, r, c))
				return false;

			//	Step progress bar
			AUX_StepProgressBar(1);
		}
	}

	return true;
}

/*	Export one block in a project terrain

	iTrnRow, iTrnCol: block row and column in whole exporting terrain
	iRow, iCol: row and column in current project
*/
bool CTerrainExpImpA::ExportProjectBlock(CExpTerrain* pTerrain, int iTrnRow, int iTrnCol,
										  int iRow, int iCol)
{
	int iMaskBlock = m_pe->m_InitData.iMaskTile / m_pe->m_InitData.iBlockGrid;
	int iBlock = iTrnRow * m_T2FileHeader.iTrnBlkCol + iTrnCol;

	//	Calculate index of the mask area which this block blongs to
	int iMaskPitch = m_T2FileHeader.iTrnBlkCol / iMaskBlock;
	int iMaskRow = iTrnRow / iMaskBlock;
	int iMaskCol = iTrnCol / iMaskBlock;
	int iMaskArea = iMaskRow * iMaskPitch + iMaskCol;

	T2BKFILEBLOCK FileBlock;
	FileBlock.iMaskArea		= iMaskArea;
	FileBlock.iRowInTrn		= iTrnRow;
	FileBlock.iColInTrn		= iTrnCol;
	FileBlock.dwLayerFlags	= 0;

	ARectI rcMap;
	rcMap.left		= iCol * m_pe->m_InitData.iBlockGrid;
	rcMap.top		= iRow * m_pe->m_InitData.iBlockGrid;
	rcMap.right		= rcMap.left + m_pe->m_InitData.iBlockGrid;
	rcMap.bottom	= rcMap.top + m_pe->m_InitData.iBlockGrid;

	//	Get all mask map data in this mask area
	CExpTerrain::MASKMAPDATA* aMaskData = pTerrain->GetMaskAreaInfo(rcMap);
	if (!aMaskData)
	{
		AUX_ProgressLog("CTerrainExpImpA::ExportProjectBlock, failed to call CExpTerrain::GetMaskAreaInfo !");
		return false;
	}

	int i, j, iFirstLayer;
	T2MKFILEMASKAREA& MaskArea = m_aMaskAreas[iMaskArea];

	//	Use the first pure white layer as block's first layer
	iFirstLayer = pTerrain->GetFirstPureWriteLayer(aMaskData);

	for (i=iFirstLayer; i < pTerrain->GetLayerNum(); i++)
	{
		const CExpTerrain::MASKMAPDATA& LayerMask = aMaskData[i];
		if (pTerrain->IsPureBlackMask(LayerMask))
			continue;	//	Throw layer which has pure black mask

		//	Re-map layer index
		for (j=0; j < MaskArea.iNumLayer; j++)
		{
			if (m_aLayerMaps[MaskArea.iFirstIndex + j] == i)
				break;
		}

		FileBlock.dwLayerFlags |= (1 << j);
	}

	//	Release resources
	pTerrain->ReleaseMaskAreaInfo(aMaskData);

	//	Record block information offset in file
	DWORD dwOffset = ftell(m_fpTerrain);
	m_aBlockOffs[iBlock] = dwOffset;

	//	Get vertex height data and save it
	CExpTerrain::HEIGHTINFO HeightInfo;
	pTerrain->GetHeightData(rcMap, HeightInfo);
	BYTE* pLineData = (BYTE*)HeightInfo.pData;

	float* aHeis = new float [HeightInfo.iWidth * HeightInfo.iHeight];
	int iCount = 0;

	for (i=0; i < HeightInfo.iHeight; i++)
	{
		float* aScales = (float*)pLineData;
		pLineData += HeightInfo.iPitch;

		for (j=0; j < HeightInfo.iWidth; j++)
			aHeis[iCount++] = pTerrain->GetYOffset() + aScales[j] * pTerrain->GetMaxHeight();
	}

	FileBlock.fLODScale = CalcBlockLODScale(aHeis, HeightInfo.iWidth, HeightInfo.iHeight);

	//	Write block information
	fwrite(&FileBlock, 1, sizeof (FileBlock), m_fpTerrain);

	//	Write height data
	fwrite(aHeis, 1, HeightInfo.iWidth * HeightInfo.iHeight * sizeof (float), m_fpTerrain);
	delete [] aHeis;

	//	Get vertex normal data and save it
	pTerrain->GetNormalData(rcMap, HeightInfo);
	pLineData = (BYTE*)HeightInfo.pData;

	float* aNormals = new float [HeightInfo.iWidth * 3];

	for (i=0; i < HeightInfo.iHeight; i++)
	{
		iCount = 0;
		A3DVECTOR3* pSrcLine = (A3DVECTOR3*)pLineData;
		pLineData += HeightInfo.iPitch;

		for (j=0; j < HeightInfo.iWidth; j++, iCount+=3)
		{
			aNormals[iCount+0] = pSrcLine[j].x;
			aNormals[iCount+1] = pSrcLine[j].y;
			aNormals[iCount+2] = pSrcLine[j].z;
		}

		//	Write vertex normal data to file
		fwrite(aNormals, 1, HeightInfo.iWidth * 3 * sizeof (float), m_fpTerrain);
	}

	delete [] aNormals;

	return true;
}

//	Calculate block's LOD scale
float CTerrainExpImpA::CalcBlockLODScale(float* aHeis, int iWid, int iHei)
{
	float fBlockSize = m_T2FileHeader.fBlockSize;

	/*	v0----v1
		|\	  |		Z
		| \   |		|
		|  \  |		|
		|	\ |		|
		v2----v3	------- X
	*/
	A3DVECTOR3 v0(0.0f, aHeis[0], 0.0f);
	A3DVECTOR3 v1(fBlockSize, aHeis[iWid-1], 0.0f);
	A3DVECTOR3 v2(0.0f, aHeis[(iWid-1)*iHei], -fBlockSize);
	A3DVECTOR3 v3(fBlockSize, aHeis[iWid*iHei-1], -fBlockSize);

	A3DPLANE p0, p1;
	p0.CreatePlane(v0, v1, v3);
	p1.CreatePlane(v0, v3, v2);

	//	0.866 = cos(30 deg)
	if (DotProduct(p0.vNormal, p1.vNormal) < 0.866f)
		return 1.0f;

	float fDelta0=0.0f, fDelta1=0.0f;
	float fStep = fBlockSize / iWid;
	A3DVECTOR3 vPos;
	int iCount = 0;

	for (int i=0; i < iHei; i++)
	{
		vPos.z = -i * fStep;

		for (int j=0; j < iWid; j++, iCount++)
		{
			vPos.x = j * fStep;
			vPos.y = aHeis[iCount];

			float fDelta;

			if (j < i)
			{
				fDelta = (float)fabs(DotProduct(vPos - v0, p1.vNormal));
				if (fDelta > fDelta1)
					fDelta1 = fDelta;
			}
			else
			{
				fDelta = (float)fabs(DotProduct(vPos - v0, p0.vNormal));
				if (fDelta > fDelta0)
					fDelta0 = fDelta;
			}
		}
	}

	float fLimit = fBlockSize * 0.4f;
	float fScale = a_Max(fDelta0, fDelta1);
	if (fScale > fLimit)
		return 1.0f;

	fScale = 1.0f - (fLimit - fScale) / fLimit;

	return fScale;
}


