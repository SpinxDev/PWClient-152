/*
 * FILE: TerrainExportor.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"

#include "TerrainExportor.h"
#include "ExpElementMap.h"
#include "Bitmap.h"
#include "LightMapGen.h"
#include "TerrainExpImpA.h"
#include "TerrainExpImpB.h"
#include "AC.h"
#include "A3D.h"
#include "SceneObjectManager.h"
#include "TerrainSquare.h"
#include "af.h"

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
//	Implement CTerrainExportor
//	
///////////////////////////////////////////////////////////////////////////

CTerrainExportor::CTerrainExportor()
{
	m_fpLightmap		= NULL;
	m_iProjectCnt		= 0;
	m_iProjRowCnt		= 0;
	m_iProjColCnt		= 0;
	m_iProjMaskArea		= 0;
	m_fProjTrnSize		= 0.0f;
	m_bLightNeighbor    = false;
	m_bLightMapTans     = false;
	m_aLMOffs			= NULL;
	m_iProjNum			= 0;
	m_pCache            = 0;

	memset(&m_InitData, 0, sizeof (m_InitData));
}

CTerrainExportor::~CTerrainExportor()
{
}

//	Initialize object
bool CTerrainExportor::Init(const INITDATA& InitData)
{
	m_strName		= InitData.szName;
	m_strDestDir	= InitData.szDestDir;
	m_InitData		= InitData;
	return true;
}

//	Release object
void CTerrainExportor::Release()
{
	//	Release all project name
	for (int i=0; i < m_aProjNames.GetSize(); i++)
		delete m_aProjNames[i];

	m_aProjNames.RemoveAll();
}

//	Add project name
void CTerrainExportor::AddProject(const char* szName)
{
	CString* pstrName = new CString(szName);
	if (!pstrName)
		return;

	m_aProjNames.Add(pstrName);
}

//	Do export
bool CTerrainExportor::DoExport()
{
	AUX_ProgressLog("-- Exporting terrain data to %s ...", m_strDestDir);

	m_iProjNum = m_InitData.iNumCol * m_InitData.iNumRow;
	if (!m_iProjNum || !m_aProjNames.GetSize() || m_aProjNames.GetSize() != m_iProjNum)
	//if (!iNumProj || !m_aProjNames.GetSize() || m_aProjNames.GetSize() < iNumProj) //Modified by XQF
	{
		AUX_ProgressLog("CTerrainExportor::DoExport, Wrong number of project name.");
		return false;
	}

	//	Create destination directory
	if (!AUX_CreateDirectory(m_strDestDir))
	{
		AUX_ProgressLog("CTerrainExportor::DoExport, Failed to create export directory.");
		return false;
	}

	//	Export terrain data
	if (!g_iTrnExport)
	{
		CTerrainExpImpA ExpImp;
		if (!ExpImp.DoExport(this, &m_TrnInfo))
		{
			AUX_ProgressLog("CTerrainExportor::DoExport, Failed to call export terrain data");
			return false;
		}

		m_iProjMaskArea = m_TrnInfo.iNumMaskArea / m_iProjNum;
		m_fProjTrnSize	= m_TrnInfo.fTerrainWid / m_TrnInfo.iSubTrnCol;
		m_iProjHMSize	= ExpImp.GetProjHMSize();
		m_iProjMaskCol	= ExpImp.GetProjMaskCol();
		m_iProjMaskRow	= ExpImp.GetProjMaskRow();
	}
	else if (g_iTrnExport == 1)
	{
		CTerrainExpImpB ExpImp;
		if (!ExpImp.DoExport(this, &m_TrnInfo))
		{
			AUX_ProgressLog("CTerrainExportor::DoExport, Failed to call export terrain data");
			return false;
		}

		m_iProjMaskArea = m_TrnInfo.iNumMaskArea / m_iProjNum;
		m_fProjTrnSize	= m_TrnInfo.fTerrainWid / m_TrnInfo.iSubTrnCol;
		m_iProjHMSize	= ExpImp.GetProjHMSize();
		m_iProjMaskCol	= ExpImp.GetProjMaskCol();
		m_iProjMaskRow	= ExpImp.GetProjMaskRow();
	}
	else
	{
		ASSERT(0);
		AUX_ProgressLog("CTerrainExportor::DoExport, No terrain export imp was found");
		return false;
	}
	
	if(m_InitData.pLMGen)
	{
		// I will copy terrain light file to editer temp folder
		char szFile[MAX_PATH];
		char szDestFile[MAX_PATH];
		sprintf(szDestFile,"%s%s\\%s\\%sRender.trn2",g_szWorkDir,g_szEditMapDir,g_szTempMap,g_szTempMap);
		sprintf(szFile, "%s\\%s.trn2", m_strDestDir, m_strName);
		if(!CopyFile(szFile,szDestFile,false))
			g_Log.Log("Copy file %s to %s failed!",szFile,szDestFile);
		//	Change file
		AUX_ChangeExtension(szFile, "t2mk");
		AUX_ChangeExtension(szDestFile, "t2mk");
		if(!CopyFile(szFile,szDestFile,false))
			g_Log.Log("Copy file %s to %s failed!",szFile,szDestFile);
	}

	//	Export light map data for day
	if (!ExportLightMaps(false))
	{
		AUX_ProgressLog("CTerrainExportor::DoExport, Failed to call ExportLightMaps for day");
		return false;
	}
	
	// Export light map data for night
	if(!g_bOnlyCalMiniMap)
	{
		if (!ExportLightMaps(true))
		{
			AUX_ProgressLog("CTerrainExportor::DoExport, Failed to call ExportLightMaps for night");
			return false;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	//process terrain square
	if(m_InitData.bGameExp)
	{
		char szFile[MAX_PATH];
		sprintf(szFile, "%s\\%s.trn2", m_strDestDir, m_strName);
		CTerrainSquare TrnSquare;
		if (!TrnSquare.Build(szFile, NULL))
		{
			g_Log.Log("CTerrainSquare::Build(), Faild to build square for terrain %s!",szFile);
			return false;
		}
		
		//	Save data
		af_ChangeFileExt(szFile, MAX_PATH, ".t2hc");
		if (!TrnSquare.Save(szFile))
		{
			g_Log.Log("CTerrainSquare::Build(), Faild to save square terrain file %s!",szFile);
			TrnSquare.Release();
			return false;
		}
		
		TrnSquare.Release();
	}
	
	//AUX_ProgressLog("-- All done!");
	
	return true;
}

//	Export light maps
bool CTerrainExportor::ExportLightMaps(bool bIsNight)
{
	if (!m_InitData.pLMGen || !m_InitData.pLMNightGen)
		return true;

	//AUX_ProgressLog("-- Export light map data ...");

	LIGHTINGPARAMS LtParams;

	//	Create light map file
	char szFile[MAX_PATH];
	if(!bIsNight)
	{
		sprintf(szFile, "%s\\%s.t2lm", m_strDestDir, m_strName);
		LtParams = m_InitData.pLMGen->GetLightingParams();
	}else 
	{
		sprintf(szFile, "%s\\%s.t2lm1", m_strDestDir, m_strName);
		LtParams = m_InitData.pLMNightGen->GetLightingParams();
	}

	if (!(m_fpLightmap = fopen(szFile, "wb+")))
	{
		AUX_ProgressLog("CTerrainExportor::ExportLightMaps, Failed to create lightmap file %s", szFile);
		return false;
	}

	//	Write version and identity of lightmap file
	TRN2FILEIDVER IdVer;
	IdVer.dwIdentify = T2LMFILE_IDENTIFY;
	IdVer.dwVersion	 = T2LMFILE_VERSION;
	fwrite(&IdVer, 1, sizeof (IdVer), m_fpLightmap);

	//	Write file header to hold place
	memset(&m_LMFileHeader, 0, sizeof (m_LMFileHeader));
	m_LMFileHeader.iNumMaskArea	= m_TrnInfo.iNumMaskArea;
	m_LMFileHeader.iMaskLMSize	= LtParams.iMapSize / (m_TrnInfo.iSubTrnGrid / m_TrnInfo.iMaskGrid);
	m_LMFileHeader.iLMDataSize	= m_LMFileHeader.iMaskLMSize * m_LMFileHeader.iMaskLMSize * 3;

	fwrite(&m_LMFileHeader, 1, sizeof (m_LMFileHeader), m_fpLightmap);

	if (!AllocLightmapExportRes())
	{
		AUX_ProgressLog("CTerrainExportor::ExportLightMaps, Failed to create resources");
		return false;
	}

	//	Write light map offsets to hold place
	fwrite(m_aLMOffs, 1, m_TrnInfo.iNumMaskArea * sizeof (DWORD), m_fpLightmap);

	m_iProjectCnt = 0;

	for (m_iProjRowCnt=0; m_iProjRowCnt < m_InitData.iNumRow; m_iProjRowCnt++)
	{
		for (m_iProjColCnt=0; m_iProjColCnt < m_InitData.iNumCol; m_iProjColCnt++)
		{
			if (!ExportProjectLightMap(m_iProjectCnt,bIsNight))
			{
				AUX_ProgressLog("CTerrainExportor::ExportLightMaps, Failed to export light map of project %d", m_iProjectCnt);
				return false;
			}
			m_iProjectCnt++;
		}
	}

	//	Rewrite light map offsets to file
	fseek(m_fpLightmap, sizeof (TRN2FILEIDVER) + sizeof (m_LMFileHeader), SEEK_SET);
	fwrite(m_aLMOffs, 1, m_TrnInfo.iNumMaskArea * sizeof (DWORD), m_fpLightmap);

	fclose(m_fpLightmap);

	ReleaseLightmapExportRes();

	return true;
}

//	Allocate exporting light map resources
bool CTerrainExportor::AllocLightmapExportRes()
{
	//	Create mask area light map data offsets array
	if (!(m_aLMOffs = new DWORD [m_TrnInfo.iNumMaskArea]))
	{
		AUX_ProgressLog("CTerrainExportor::AllocLightmapExportRes, Not enough memory !");
		return false;
	}

	memset(m_aLMOffs, 0, m_TrnInfo.iNumMaskArea * sizeof (DWORD));

	return true;
}

//	Release resources used during exporting light map
void CTerrainExportor::ReleaseLightmapExportRes()
{
	if (m_aLMOffs)
	{
		delete [] m_aLMOffs;
		m_aLMOffs = NULL;
	}
}
#define _EDEBUG
//	Export a project's light map
bool CTerrainExportor::ExportProjectLightMap(int iProject,bool bIsNight)
{
	AUX_ProgressLog("-- Exporting light map of project [%d/%d] ...", iProject+1, m_aProjNames.GetSize());

	bool bRet = true;

	bRet &= SetLightMapProject(CLightMapGen::MAP_CURRENT, m_iProjRowCnt, m_iProjColCnt,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_LEFTTOP, m_iProjRowCnt-1, m_iProjColCnt-1,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_TOP, m_iProjRowCnt-1, m_iProjColCnt,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_RIGHTTOP, m_iProjRowCnt-1, m_iProjColCnt+1,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_LEFT, m_iProjRowCnt, m_iProjColCnt-1,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_RIGHT, m_iProjRowCnt, m_iProjColCnt+1,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_LEFTBOTTOM, m_iProjRowCnt+1, m_iProjColCnt-1,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_BOTTOM, m_iProjRowCnt+1, m_iProjColCnt,bIsNight);
	bRet &= SetLightMapProject(CLightMapGen::MAP_RIGHTBOTTOM, m_iProjRowCnt+1, m_iProjColCnt+1,bIsNight);
	if (!bRet)
	{
		AUX_ProgressLog("CTerrainExportor::ExportProjectLightMap, Failed to load project");
		return false;
	}
	
	if(bIsNight)
	{
		if (!m_InitData.pLMNightGen->DoGenerate(bIsNight))
		{	
			if(!m_bLightMapTans)
			{
				//	Release projects
				for (int i=0; i < CLightMapGen::NUM_MAP; i++)
				{
					CElementMap* pMap = NULL;
					if(bIsNight)
						pMap = m_InitData.pLMNightGen->GetMap(i);
					else pMap = m_InitData.pLMGen->GetMap(i);
					if (pMap)
					{
						pMap->Release();
						delete pMap;
					}
				}
			}
			return false;
		}
	}else
	{
		if (!m_InitData.pLMGen->DoGenerate(bIsNight))
		{
			if(!m_bLightMapTans)
			{
				//	Release projects
				for (int i=0; i < CLightMapGen::NUM_MAP; i++)
				{
					CElementMap* pMap = NULL;
					if(bIsNight)
						pMap = m_InitData.pLMNightGen->GetMap(i);
					else pMap = m_InitData.pLMGen->GetMap(i);
					if (pMap)
					{
						pMap->Release();
						delete pMap;
					}
				}
			}
			return false;
		}
	}
	CELBitmap* listLightMap[4];
	for(int n = 0; n < 4; n ++) listLightMap[n] = NULL;
	
	CString strFile;

	//如果需要在灯光图之间做过度，就保留前面算出的灯光图，
	//同时计算2个相邻图的灯光在该图上产生的阴影图，然后做过渡操作
	if(m_bLightMapTans && m_pCache)
	{
		//	1		
		//2 0 3
		//  4
		//保留当前图到数组第0号
		if(bIsNight)
			listLightMap[0] = CopyELBitmap(m_InitData.pLMNightGen->GetLightMapBitmap());
		else listLightMap[0] = CopyELBitmap(m_InitData.pLMGen->GetLightMapBitmap());
#ifdef _EDEBUG		
		strFile.Format("%sTemp\\ccccccccccccccccccccc%d.bmp", g_szWorkDir,0);
		if(listLightMap[0]) listLightMap[0]->SaveToFile(strFile);
#endif		
		//取得相邻图灯光信息，并重新计算光照后保留到相应的灯光图列表中
		CString tempProj[3];
		tempProj[0] = m_strExtProj[5];
		tempProj[1] = m_strExtProj[7];
		tempProj[2] = m_strExtProj[8];
		for(int i = 0; i < 3; i++)
		{
			if(bIsNight)
			{
				CExpElementMap *pTempMap = m_pCache->GetMap(tempProj[i]);
				if(pTempMap==NULL) continue;
				m_InitData.pLMNightGen->Release();
				if(g_bOnlyCalMiniMap)
				{
					LIGHTINGPARAMS lpa = pTempMap->GetLightingParams();
					lpa.dwAmbient = A3DCOLORRGB(192,192,192);
					lpa.dwSunCol  = A3DCOLORRGB(192,192,192);
					lpa.fSunPower = 1.0f;
					m_InitData.pLMGen->Init(lpa,true);
				}else m_InitData.pLMNightGen->Init(pTempMap->GetNightLightingParams(),true);
				m_InitData.pLMNightGen->DoGenerate(bIsNight);
				listLightMap[i+1] = CopyELBitmap(m_InitData.pLMNightGen->GetLightMapBitmap());
			}else
			{
				CExpElementMap *pTempMap = m_pCache->GetMap(tempProj[i]);
				if(pTempMap==NULL) continue;
				m_InitData.pLMGen->Release();
				if(g_bOnlyCalMiniMap)
				{
					LIGHTINGPARAMS lpa = pTempMap->GetLightingParams();
					lpa.dwAmbient = A3DCOLORRGB(192,192,192);
					lpa.dwSunCol  = A3DCOLORRGB(192,192,192);
					lpa.fSunPower = 1.0f;
					m_InitData.pLMGen->Init(lpa,true);
				}else m_InitData.pLMGen->Init(pTempMap->GetLightingParams(),true);
				
				m_InitData.pLMGen->DoGenerate(bIsNight);
				listLightMap[i+1] = CopyELBitmap(m_InitData.pLMGen->GetLightMapBitmap());
#ifdef _EDEBUG
				strFile.Format("%sTemp\\ccccccccccccccccccccc%d.bmp", g_szWorkDir,i+1);
				if(listLightMap[i+1]) listLightMap[i+1]->SaveToFile(strFile);
#endif
			}
		}
		//计算过渡后的灯光图把它保存在0号图中
		ProcessELBitmap(listLightMap);
		m_InitData.pLMGen->Release();
		m_InitData.pLMNightGen->Release();

		m_InitData.pLMGen->SetLightMapBitmap(listLightMap[0]);
		m_InitData.pLMNightGen->SetLightMapBitmap(listLightMap[0]);
	}

#ifdef _EDEBUG
	//	Save light map to temporary directory for debugging
	if(bIsNight)
	{
		strFile.Format("%sTemp\\lm_night%s_d.bmp", g_szWorkDir, m_strExtProj[4]);
		if(m_bLightMapTans) 
		{
			if(listLightMap[0]) listLightMap[0]->SaveToFile(strFile);
		}else m_InitData.pLMNightGen->GetLightMapBitmap()->SaveToFile(strFile);
	}else
	{
		strFile.Format("%sTemp\\lm_day%s_d.bmp", g_szWorkDir, m_strExtProj[4]);
		if(m_bLightMapTans)
		{
			if(listLightMap[0]) listLightMap[0]->SaveToFile(strFile);
		}else m_InitData.pLMGen->GetLightMapBitmap()->SaveToFile(strFile);
	}
#endif	
	
	//AUX_ProgressLog("-- Exporting light map of each mask area ...");
	AUX_ResetProgressBar(m_iProjMaskArea);

	int iProjMask = (m_iProjHMSize-1) / m_InitData.iMaskTile;
	int iTrnMaskPitch = iProjMask * m_InitData.iNumCol;
	int iBaseRow = m_iProjRowCnt * iProjMask;
	int iBaseCol = m_iProjColCnt * iProjMask;

	for (int r=0; r < iProjMask; r++)
	{
		int iMaskArea = (iBaseRow + r) * iTrnMaskPitch + iBaseCol;

		for (int c=0; c < iProjMask; c++, iMaskArea++)
		{
			if (!ExportProjectMaskLightMap(iMaskArea, r, c,bIsNight))
				return false;

			//	Step progress bar
			AUX_StepProgressBar(1);
		}
	}

	if(!m_bLightMapTans)
	{
		//	Release projects
		for (int i=0; i < CLightMapGen::NUM_MAP; i++)
		{
			CElementMap* pMap = NULL;
			if(bIsNight)
				pMap = m_InitData.pLMNightGen->GetMap(i);
			else pMap = m_InitData.pLMGen->GetMap(i);
			if (pMap)
			{
				pMap->Release();
				delete pMap;
			}
		}
	}else
	{
		m_InitData.pLMGen->SetLightMapBitmap(NULL);
		m_InitData.pLMNightGen->SetLightMapBitmap(NULL);
		for( int i = 0; i < 3; i++)
		{
			if(listLightMap[i]) 
			{
				listLightMap[i]->Release();
				delete listLightMap[i];
			}
		}
	}
	return true;
}

//对5张图做过渡处理，处理结果放到0号图中
void CTerrainExportor::ProcessELBitmap(CELBitmap* listLightMap[])
{
	//    1
	//  2 0 3
	//    4
	CELBitmap *pTemp = CopyELBitmap(listLightMap[0]);
	
	
	//01
	TansBottomEdge2(pTemp,listLightMap[2],100);
	//02
	TansRightEdge2(pTemp,listLightMap[1],100);
	//03
	TansRightEdge(listLightMap[0],listLightMap[1],100);
	//04
	TansBottomEdge(listLightMap[0],listLightMap[2],100);
	//05
	TansRightBottomCorner2(listLightMap[0],pTemp,100);
	//06
	TansRightBottomCorner(listLightMap[0],listLightMap[3],100);

	pTemp->Release();
	delete pTemp;
}

void CTerrainExportor::TansUpEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left	 = 0;
	rcArea1.top		 = 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();

	rcArea2.left     = 0;
	rcArea2.top      = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,w,h;
	for(h = 0; h < iTansPixel; h++)
	{
		for(w = 0; w < LockInfo1.iWidth; w++)
		{
			int idx = h * LockInfo1.iWidth*3 + w*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)h/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)h/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)h/(double)iTansPixel);
		}
	}
}

void CTerrainExportor::TansLeftEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left		= 0;
	rcArea1.top		= 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();

	rcArea2.left    = 0;
	rcArea2.top     = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,w,h;
	for(w = 0; w < iTansPixel; w++)
	{
		for(h = 0; h < LockInfo1.iHeight; h++)
		{
			int idx = h * LockInfo1.iWidth*3 + w*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)w/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)w/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)w/(double)iTansPixel);
		}
	}
}

void CTerrainExportor::TansRightEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left	 = 0;
	rcArea1.top		 = 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();

	rcArea2.left     = 0;
	rcArea2.top      = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,w,h;
	for(w = 0; w < iTansPixel; w++)
	{
		for(h = 0; h < LockInfo1.iHeight - w; h++)
		{
			int idx = h * LockInfo1.iWidth*3 + (LockInfo1.iWidth - w - 1)*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)w/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)w/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)w/(double)iTansPixel);
		}
	}

}

void CTerrainExportor::TansRightEdge2(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left	 = 0;
	rcArea1.top		 = 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();

	rcArea2.left     = 0;
	rcArea2.top      = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,w,h;
	for(w = 0; w < iTansPixel; w++)
	{
		for(h = 0; h < LockInfo1.iHeight - iTansPixel; h++)
		{
			int idx = h * LockInfo1.iWidth*3 + (LockInfo1.iWidth - w - 1)*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)w/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)w/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)w/(double)iTansPixel);
		}
	}

}

void CTerrainExportor::TansBottomEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left		= 0;
	rcArea1.top		= 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();

	rcArea2.left    = 0;
	rcArea2.top     = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,w,h;
	for(h = 0; h < iTansPixel; h++)
	{
		for(w = 0; w < LockInfo1.iHeight - h; w++)
		{
			int idx = (LockInfo1.iHeight - h - 1) * LockInfo1.iWidth*3 + w*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)h/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)h/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)h/(double)iTansPixel);
		}
	}	
}
void CTerrainExportor::TansBottomEdge2(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left		= 0;
	rcArea1.top		= 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();

	rcArea2.left    = 0;
	rcArea2.top     = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,w,h;
	for(h = 0; h < iTansPixel; h++)
	{
		for(w = 0; w < LockInfo1.iHeight; w++)
		{
			int idx = (LockInfo1.iHeight - h - 1) * LockInfo1.iWidth*3 + w*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)h/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)h/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)h/(double)iTansPixel);
		}
	}	
}

void CTerrainExportor::TansRightBottomCorner2(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap,int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left		= 0;
	rcArea1.top		= 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();
	
	rcArea2.left    = 0;
	rcArea2.top     = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();
	
	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);
	
	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;
	
	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,h,w;
	
	for(h = 0; h < 2*iTansPixel; h++)
	{
		for(int i = 0; i < h; i++)
		{
			if(LockInfo1.iHeight - h + i < LockInfo1.iHeight - iTansPixel) continue;
			
			int idx = (LockInfo1.iHeight - h + i) * LockInfo1.iWidth*3 + (LockInfo1.iWidth - i - 1)*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			if(h <= iTansPixel)
			{
				int test = h/2;
				if(test==0) continue;
				if(i <= test )
				{
					pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)(test-i)/(double)test);
					pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)(test-i)/(double)test);
					pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)(test-i)/(double)test);
				}
			}
		}
	}
	
	for( w = 0; w < iTansPixel; w ++)
	{
		for( h = 0; h < iTansPixel - w; h++)
		{
			int idx = (LockInfo1.iHeight - iTansPixel + h) * LockInfo1.iWidth*3 + (LockInfo1.iWidth - w - h - 2)*3;	
			
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];

			int test = (iTansPixel - w)/2;
			int dx = h;
			if(dx>test) dx = test; 
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)(test-dx)/(double)test);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)(test-dx)/(double)test);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)(test-dx)/(double)test);
		}
	}
}

void CTerrainExportor::TansRightBottomCorner(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel)
{
	if(pDestBitmap==NULL || pSrcBitmap==NULL) return;
	
	ARectI rcArea1,rcArea2;
	rcArea1.left		= 0;
	rcArea1.top		= 0;
	rcArea1.right    = pDestBitmap->GetWidth();
	rcArea1.bottom   = pDestBitmap->GetHeight();
	
	rcArea2.left    = 0;
	rcArea2.top     = 0;
	rcArea2.right    = pSrcBitmap->GetWidth();
	rcArea2.bottom   = pSrcBitmap->GetHeight();
	
	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pDestBitmap->LockRect(rcArea1, &LockInfo1);
    pSrcBitmap->LockRect(rcArea2,&LockInfo2);
	
	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth) return;
	
	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) return;
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;
	int pr,pg,pb,h;
	for(h = 0; h < iTansPixel; h++)
	{
		for(int i = 0; i < h; i++)
		{
			int idx = (LockInfo1.iHeight - h + i) * LockInfo1.iWidth*3 + (LockInfo1.iWidth - i - 1)*3;
			pr = pSrcData1[idx + 0] - pSrcData2[idx + 0];
			pg = pSrcData1[idx + 1] - pSrcData2[idx + 1];
			pb = pSrcData1[idx + 2] - pSrcData2[idx + 2];
			
			pSrcData1[idx + 0] = pSrcData2[idx + 0] + (int)(pr * (double)h/(double)iTansPixel);
			pSrcData1[idx + 1] = pSrcData2[idx + 1] + (int)(pg * (double)h/(double)iTansPixel);
			pSrcData1[idx + 2] = pSrcData2[idx + 2] + (int)(pb * (double)h/(double)iTansPixel);
			
		}
	}		
}



CELBitmap * CTerrainExportor::CopyELBitmap(CELBitmap*pSrcBitmap)
{
	if(pSrcBitmap==NULL) return NULL;
	int iMapSize = pSrcBitmap->GetWidth();
	CELBitmap *pNewBitmap = new CELBitmap();
	if(!pNewBitmap) return NULL;
	
	if (!pNewBitmap->CreateBitmap(iMapSize, iMapSize, 24, NULL))
	{
		g_Log.Log("CTerrainExportor::CopyELBitmap, Failed to create bitmap !");
		return NULL;
	}
	
	//	Clear bitmap with black color
	pNewBitmap->Clear();

	//	Fill bitmap data
	BYTE* pDstLine = pNewBitmap->GetBitmapData();
	BYTE* pSrcLine = pSrcBitmap->GetBitmapData();
	
	for (int i=0; i < iMapSize; i++)
	{
		memcpy(pDstLine, pSrcLine, iMapSize*3);
		pDstLine += iMapSize*3;
		pSrcLine += iMapSize*3;
	}
	return pNewBitmap;
}

//	Set project used to calculate light map
bool CTerrainExportor::SetLightMapProject(int iPos, int r, int c,bool bIsNight)
{
	ASSERT(m_InitData.pLMGen);
	ASSERT(m_InitData.pLMNightGen);
	
	int iProject = 4 + r * 3 + c;
	int nQuad = GetQuadrant(bIsNight);
	
	bool bLoad = false;
	if(nQuad==1) // 1 quadrant 
		if(iProject == 3 || iProject==6 || iProject== 7) bLoad = true;
	if(nQuad==2) // 2 quadrant
		if(iProject == 5 || iProject==7 || iProject== 8) bLoad = true;
	if(nQuad==3) // 3 quadrant
		if(iProject == 1 || iProject==2 || iProject== 5) bLoad = true;
	if(nQuad==4) // 4 quadrant
		if(iProject == 0 || iProject==1 || iProject== 3) bLoad = true;

	if(iProject==4) bLoad = true;

	if(!m_bLightNeighbor)
	{
		if(iProject!=4)
		{
			m_InitData.pLMGen->SetMap(iPos, NULL);
			m_InitData.pLMNightGen->SetMap(iPos,NULL);
			return true;
		}
	}
	
	if(iProject < 0 || iProject > 9 || !bLoad)
	{
		m_InitData.pLMGen->SetMap(iPos, NULL);
		m_InitData.pLMNightGen->SetMap(iPos,NULL);
		return true;
	}

	CString strProject;
	if(iProject==4 && !m_bLightMapTans) strProject = *m_aProjNames[0];
	else strProject = m_strExtProj[iProject];

	if(strProject.IsEmpty()) 
	{
		m_InitData.pLMGen->SetMap(iPos, NULL);
		m_InitData.pLMNightGen->SetMap(iPos,NULL);
		return true;
	}
	
	//	Make project's file name
	CString strFile;
	strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProject, strProject);

	//	Set map offset
	float fx, fz;

	switch (iPos)
	{
	case CLightMapGen::MAP_CURRENT:		fx = fz = 0.0f;	break;
	case CLightMapGen::MAP_LEFTTOP:		fx = -m_fProjTrnSize; fz = m_fProjTrnSize; break;
	case CLightMapGen::MAP_TOP:			fx = 0.0f; fz = m_fProjTrnSize; break;
	case CLightMapGen::MAP_RIGHTTOP:	fx = m_fProjTrnSize; fz = m_fProjTrnSize; break;
	case CLightMapGen::MAP_LEFT:		fx = -m_fProjTrnSize; fz = 0.0f; break;
	case CLightMapGen::MAP_RIGHT:		fx = m_fProjTrnSize; fz = 0.0f; break;
	case CLightMapGen::MAP_LEFTBOTTOM:	fx = -m_fProjTrnSize; fz = -m_fProjTrnSize; break;
	case CLightMapGen::MAP_BOTTOM:		fx = 0.0f; fz = -m_fProjTrnSize; break;
	case CLightMapGen::MAP_RIGHTBOTTOM:	fx = m_fProjTrnSize; fz = -m_fProjTrnSize; break;
	default:
		ASSERT(0);
		break;
	}
	
	
	CExpElementMap* pMap = NULL;
	if(m_bLightMapTans && m_pCache)
	{
		pMap = m_pCache->GetMap(strProject);
		if(!pMap) return false;
		pMap->SetMapOffset(fx, fz);
	}else
	{
		//	Load project
		pMap = new CExpElementMap;
		if (!pMap) return false;
		if (!pMap->Load(strFile, LOAD_EXPLIGHTMAP))
		{
			AUX_ProgressLog("CTerrainExportor::SetLightMapProject, Failed to call CExpElementMap::Load");
			return false;
		}
		pMap->SetMapOffset(fx, fz);
	}
	
	if (bIsNight) 
	{
		m_InitData.pLMNightGen->SetMap(iPos,pMap);
	}
	else 
	{
		m_InitData.pLMGen->SetMap(iPos, pMap);
	}
	
	return true;
}


/*
	Get quadrant of sun's direction
	2 z|  1
	___|___x
	   |
	3  |  4
*/
int CTerrainExportor::GetQuadrant(bool bIsNight)
{
	LIGHTINGPARAMS litParams;
	if(bIsNight) litParams = m_InitData.pLMNightGen->GetLightingParams();
	else litParams = m_InitData.pLMGen->GetLightingParams();
	
	//quadrant
	if(litParams.vSunDir.x>=0)
	{//1,4
		if(litParams.vSunDir.z>=0)
		{//1
			return 1;
		}else
		{//4
			return 4;
		}
	}else
	{//2,3
		if(litParams.vSunDir.z>=0)
		{//2
			return 2;
		}else
		{//3
			return 3;
		}
	}
	return 0;
}

/*	Export a project mask area's light map

	iMaskArea: mask area's index in whole exporting terrain
	iRow, iCol: row and column in current project
*/
bool CTerrainExportor::ExportProjectMaskLightMap(int iMaskArea, int iRow, int iCol, bool bIsNight)
{
	ARectI rcArea;
	rcArea.left		= iCol * m_LMFileHeader.iMaskLMSize;
	rcArea.top		= iRow * m_LMFileHeader.iMaskLMSize;
	rcArea.right	= rcArea.left + m_LMFileHeader.iMaskLMSize;
	rcArea.bottom	= rcArea.top + m_LMFileHeader.iMaskLMSize;

	//	Record mask area data's offset in file
	DWORD dwOffset = ftell(m_fpLightmap);
	m_aLMOffs[iMaskArea] = dwOffset;

	CELBitmap::LOCKINFO LockInfo;
	if(bIsNight)
		m_InitData.pLMNightGen->GetLightMapBitmap()->LockRect(rcArea, &LockInfo);
	else m_InitData.pLMGen->GetLightMapBitmap()->LockRect(rcArea, &LockInfo);

	BYTE* pDstData = new BYTE[m_LMFileHeader.iMaskLMSize * 3];
	if (!pDstData)
	{
		AUX_ProgressLog("CTerrainExportor::ExportProjectMaskLightMap, Not enough memory !");
		return false;
	}

	//	Write mask map light map data file
	BYTE* pSrcData = LockInfo.pData;

	for (int i=0; i < LockInfo.iHeight; i++)
	{
		//	Windows bitmap has the same color order as D3D
		for (int j=0, n=0; j < LockInfo.iWidth; j++, n+=3)
		{
			pDstData[n]		= pSrcData[n];
			pDstData[n+1]	= pSrcData[n+1];
			pDstData[n+2]	= pSrcData[n+2];
		}

		fwrite(pDstData, 1, LockInfo.iWidth * 3, m_fpLightmap);
		pSrcData += LockInfo.iPitch;
	}

	delete [] pDstData;

	return true;
}



////////////////////////////////////////////////////////////////////////////////////
// class CElMapCache

CElMapCache::CElMapCache()
{
	for(int i = 0; i < 9; i++)
	{	
		m_pMapList[i].m_pMap = NULL;
		m_pMapList[i].strProjName = "";
	}
}

CElMapCache::~CElMapCache()
{
	Release();
}

void CElMapCache::Release()
{
	for(int i = 0; i < 9; i++)
	{	
		if(m_pMapList[i].m_pMap)
		{
			m_pMapList[i].m_pMap->Release();
			delete m_pMapList[i].m_pMap;
			m_pMapList[i].m_pMap = NULL;
		}
		m_pMapList[i].strProjName = "";
	}
}

void CElMapCache::ReleaseOthers(const CString& strProject)
{
	for(int i = 0; i < 9; i++)
	{	
		if(0 != strcmp(m_pMapList[i].strProjName,strProject))
		{
			
			if(m_pMapList[i].m_pMap)
			{
				m_pMapList[i].m_pMap->Release();
				delete m_pMapList[i].m_pMap;
				m_pMapList[i].m_pMap = NULL;
			}
			m_pMapList[i].strProjName = "";
		}
	}
}

CExpElementMap * CElMapCache::GetMap(const CString& strProjName)
{
	for(int i = 0; i < 9; i++)
	{
		if(0==strcmp(m_pMapList[i].strProjName,strProjName))
		{
			return m_pMapList[i].m_pMap;
		}
	}
	return NULL;
}

void CElMapCache::AddProject(const CString *strProjName)
{
	
	MAPCACHE_ITEM temp[9];
	// 清除
	for(int i = 0; i < 9; i++)
	{
		
		bool bDelete = true;
		for(int j = 0; j < 9; j++)
		{
			if(0==strcmp(m_pMapList[i].strProjName,strProjName[j]))
			{
				bDelete = false;
				break;
			}
		}
		if(bDelete)
		{
			if(m_pMapList[i].m_pMap)
			{
				m_pMapList[i].m_pMap->Release();
				delete m_pMapList[i].m_pMap;
				m_pMapList[i].m_pMap = NULL;
				m_pMapList[i].strProjName = "";
			}
		}
	}
	
	for( i = 0; i < 9; i++)
	{
		temp[i].strProjName = strProjName[i];
		temp[i].m_pMap = GetMap(strProjName[i]);
		if(temp[i].m_pMap==NULL)
		{
		   if(temp[i].strProjName.IsEmpty()) continue;
			CString strFile;
		   strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, temp[i].strProjName, temp[i].strProjName);
		   temp[i].m_pMap = new CExpElementMap;
		   if(temp[i].m_pMap==NULL) continue;
		   g_bOnlyLoadNearBuildForLight = true;
		   if (!temp[i].m_pMap->Load(strFile, LOAD_EXPLIGHTMAP))
		   {
			   g_Log.Log("CElMapCache::AddProject, Failed to call CExpElementMap::Load");
			   delete temp[i].m_pMap;
			   temp[i].m_pMap = NULL;
			   g_bOnlyLoadNearBuildForLight = false;
			   continue;
		   }
		   g_bOnlyLoadNearBuildForLight = false;
		}
	}

	for( i = 0; i < 9; i++)
	{
		m_pMapList[i].m_pMap = temp[i].m_pMap;
		m_pMapList[i].strProjName = temp[i].strProjName;
	}

	for( i = 0; i < 9; i++)
	{
		for( int j = i+1; j < 9; j++)
		{
			if(strcmp(m_pMapList[i].strProjName,m_pMapList[j].strProjName)==0)
			{	
				if(m_pMapList[i].m_pMap != m_pMapList[j].m_pMap)
				{
					if(m_pMapList[j].m_pMap)
					{
						m_pMapList[j].m_pMap->Release();
						delete m_pMapList[j].m_pMap;
					}
				}
				m_pMapList[j].strProjName = "";
				m_pMapList[j].m_pMap = NULL;
			}
		}
	}
}

CExpElementMap* CElMapCache::AddStartMap(const CString& strProjName)
{ 
	for( int i = 0; i < 9; i++)
	{
		if(strcmp(m_pMapList[i].strProjName,strProjName)==0)
		{
			if(m_pMapList[i].m_pMap) 
			{
				m_pMapList[i].m_pMap->Release();
				delete m_pMapList[i].m_pMap;
				m_pMapList[i].m_pMap = NULL;
				m_pMapList[i].strProjName = "";
			}
		}
	}
	
	for( i = 0; i < 9; i++)
	{
		if(m_pMapList[i].m_pMap==NULL)
		{
			CString strFile;
			strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProjName, strProjName);
			m_pMapList[i].strProjName = strProjName;
			m_pMapList[i].m_pMap = new CExpElementMap;
			if (!m_pMapList[i].m_pMap->Load(strFile, LOAD_EXPLIGHTMAP))
			{
				g_Log.Log("CElMapCache::AddStartMap, Failed to call CExpElementMap::Load");
				delete m_pMapList[i].m_pMap;
				m_pMapList[i].m_pMap = NULL;
				return NULL;
			}
			return m_pMapList[i].m_pMap;
		}
	}
	return NULL;
}
