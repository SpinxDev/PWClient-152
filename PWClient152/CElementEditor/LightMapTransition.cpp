// LightMapTransition.cpp: implementation of the CLightMapTransition class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "LightMapTransition.h"
#include "AF.h"
#include "A3D.h"
#include "Bitmap.h"
#include "ExpElementMap.h"
#include "TerrainExportor.h"
#include "LightMapGen.h"
#include "SceneObjectManager.h"
#include "Render.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightMapTransition::CLightMapTransition()
{
	m_dwNumCol		= 1;
	m_bLoaded       = false;
	m_aLMOffsets    = 0;
}

CLightMapTransition::~CLightMapTransition()
{
	// Release array
	for(int i = 0; i< (int)m_Array.size();i++)
	{
		CString* p = m_Array.at(i);
		delete p;
	}
	m_Array.clear();

	ReleaseExportRes();
}

bool CLightMapTransition::LoadProjList()
{
	for(int i = 0; i< (int)m_Array.size();i++)
	{
		CString* p = m_Array.at(i);
		delete p;
	}
	m_Array.clear();
	
	// Load project file list
	AScriptFile File;
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	if (!File.Open(szProj))
	{
		g_Log.Log("Open file %s failed!",szProj);
		return false;
	}
	
	if(File.GetNextToken(true))
	{
		CString strCol(File.m_szToken);
		if(strCol.Find("col=")!=-1)
		{
			strCol.Replace("col=","");
			m_dwNumCol = (int)atof(strCol);
		}else
		{
			File.ResetScriptFile();
			m_dwNumCol = 1;
		}
			
	}

	while (File.GetNextToken(true))
	{
		CString *pNew = new CString(File.m_szToken);
		ASSERT(pNew);
		m_Array.push_back(pNew);
	}
	File.Close();
	return true;
}

bool CLightMapTransition::GetProjectInfor(const CString strCurrentPrj, int pNeiIndices[], CString pNeiName[])
{
int nCurrent = -1;
	
	// Release array
	for(int i = 0; i< (int)m_Array.size();i++)
	{
		CString* p = m_Array.at(i);
		if(0==strcmp(*p,strCurrentPrj))
			nCurrent = i;
	}

	if(nCurrent==-1) return false;
	
	// 0 1 2
	// 3 4 5
	// 6 7 8
	if(nCurrent+1 <= m_dwNumCol || (nCurrent+1)%m_dwNumCol==1) pNeiIndices[0] = -1;
	else pNeiIndices[0] = (nCurrent - m_dwNumCol) - 1;
	
	if(nCurrent+1 <= m_dwNumCol) pNeiIndices[1] = -1;
	else pNeiIndices[1] = (nCurrent - m_dwNumCol);
	
	if((nCurrent+1)%m_dwNumCol==0 || nCurrent+1 <= m_dwNumCol) pNeiIndices[2] = -1;
	else pNeiIndices[2] = (nCurrent - m_dwNumCol) + 1;

	if((nCurrent+1)%m_dwNumCol==1) pNeiIndices[3] = -1;
	else pNeiIndices[3] = (nCurrent) - 1;
	
	pNeiIndices[4] = (nCurrent);
	
	if((nCurrent+1)%m_dwNumCol==0) pNeiIndices[5] = -1;
	else pNeiIndices[5] = (nCurrent) + 1;

	if((nCurrent+1)%m_dwNumCol==1 || (nCurrent + m_dwNumCol) - 1 == nCurrent ||(nCurrent+1)/m_dwNumCol >= (int)m_Array.size()/m_dwNumCol) pNeiIndices[6] = -1;
	else pNeiIndices[6] = (nCurrent + m_dwNumCol) - 1;
	
	if((nCurrent+1)/m_dwNumCol >= (int)m_Array.size()/m_dwNumCol) pNeiIndices[7] = -1;
	else pNeiIndices[7] = (nCurrent + m_dwNumCol);
	
	if((nCurrent+1)%m_dwNumCol==0 || (nCurrent+1)/m_dwNumCol >= (int)m_Array.size()/m_dwNumCol) pNeiIndices[8] = -1;
	else pNeiIndices[8] = (nCurrent + m_dwNumCol) + 1;

	for(i = 0; i < 9; i++)
	{
		if(pNeiIndices[i] < 0 || pNeiIndices[i] >= (int)m_Array.size())
			pNeiIndices[i] = -1;
		
		int index = pNeiIndices[i];
		
		if(index==-1) pNeiName[i] = "";
		else pNeiName[i] = *m_Array.at(index);
	}
	return true;
}

bool CLightMapTransition::DoTrans()
{
	LoadProjList();

	for(int i = 0; i< (int)m_Array.size();i++)
	{
		CString* proj = m_Array.at(i);
		int pNeiIndices[9];
		CString pNeiName[9];
		
		if(!GetProjectInfor(*proj,pNeiIndices,pNeiName)) return false;
		
		for( int k = 0; k < 2; k++)
		{
			CELBitmap *pCurBitmap = LoadLightMap(i,*proj,(bool)k);
			CELBitmap *pNeiBitmap1 = NULL;
			CELBitmap *pNeiBitmap2 = NULL;
			if(pCurBitmap)
			{	
				
				// Right
				pNeiBitmap1 = LoadLightMap(pNeiIndices[5],pNeiName[5],(bool)k);
				// Bottom
				pNeiBitmap2 = LoadLightMap(pNeiIndices[7],pNeiName[7],(bool)k);
				
				ProecssBitmap(pCurBitmap,pNeiBitmap1,TRANS_RIGHT,20);
				ProecssBitmap(pCurBitmap,pNeiBitmap2,TRANS_BOTTOM,20);
				
				WriteLightMap(pCurBitmap,*proj,(bool)k);
				WriteLightMap(pNeiBitmap1,pNeiName[5],(bool)k);
				WriteLightMap(pNeiBitmap2,pNeiName[7],(bool)k);
				
#ifdef _EDEBUG			
				CString strTempPath;
				strTempPath.Format("TransLight%d%d.bmp",i,k);
				if(pCurBitmap)pCurBitmap->SaveToFile(strTempPath);
#endif
				if(pCurBitmap) 
				{
					pCurBitmap->Release();
					delete pCurBitmap;
					pCurBitmap = NULL;
				}
				
				if(pNeiBitmap1) 
				{
					pNeiBitmap1->Release();
					delete pNeiBitmap1;
					pNeiBitmap1 = NULL;
				}
				
				if(pNeiBitmap2) 
				{
					pNeiBitmap2->Release();
					delete pNeiBitmap2;
					pNeiBitmap2 = NULL;
				}
			}
		}
		
	}
	return true;
}

bool CLightMapTransition::ProecssBitmap(CELBitmap *pCurrentBitmap, CELBitmap *pNeiBitmap,int iNei,int iTansPixel)
{
	if(pCurrentBitmap==NULL || pNeiBitmap==NULL) return false;
	
	
	ARectI rcArea1,rcArea2;
	rcArea1.left		= 0;
	rcArea1.top		= 0;
	rcArea1.right    = pCurrentBitmap->GetWidth();
	rcArea1.bottom   = pCurrentBitmap->GetHeight();

	rcArea2.left    = 0;
	rcArea2.top     = 0;
	rcArea2.right    = pNeiBitmap->GetWidth();
	rcArea2.bottom   = pNeiBitmap->GetHeight();

	CELBitmap::LOCKINFO LockInfo1,LockInfo2;
	pCurrentBitmap->LockRect(rcArea1, &LockInfo1);
    pNeiBitmap->LockRect(rcArea2,&LockInfo2);

	if(LockInfo1.iHeight != LockInfo2.iHeight || LockInfo1.iWidth != LockInfo2.iWidth)
	{
		return false;
	}

	if(LockInfo1.iHeight < iTansPixel || LockInfo1.iWidth < iTansPixel) 
	{
		return false;
	}
	
	BYTE* pSrcData1 = LockInfo1.pData;
	BYTE* pSrcData2 = LockInfo2.pData;

	if(iNei == TRANS_RIGHT)
	{
		int h;
		for(h = 0; h < LockInfo1.iHeight; h++)
		{
			// 插值后重新写入数据
			int idx = h * LockInfo1.iWidth*3 + (LockInfo1.iWidth-1)*3;
			int ir = pSrcData1[idx + 0];
			int ig = pSrcData1[idx + 1];
			int ib = pSrcData1[idx + 2];
			
			idx = h * LockInfo2.iWidth*3;
			pSrcData2[idx + 0] = ir;
			pSrcData2[idx + 1] = ig;
			pSrcData2[idx + 2] = ib;
			
		}
		
	}else if(iNei == TRANS_BOTTOM)
	{
		int w;
		for(w = 0; w < LockInfo1.iWidth; w++)
		{
			int idx = (LockInfo1.iHeight-1) * LockInfo1.iWidth*3 + w*3;
			int ir =  pSrcData1[idx + 0];
			int ig =  pSrcData1[idx + 1];
			int ib =  pSrcData1[idx + 2];
			
			idx = w*3;
			pSrcData2[idx + 0] = ir;
			pSrcData2[idx + 1] = ig;
			pSrcData2[idx + 2] = ib;
			
		}
	}
	return true;
}

CELBitmap* CLightMapTransition::LoadLightMap(int iProject,CString strProject,bool bNight)
{
	
	if(iProject==-1) return NULL;
	CELBitmap *pBitmap = NULL;
	//	Make project lightmap file name
	CString strFile;
	if(bNight) strFile.Format("%s%s\\%s\\%srender.t2lm1", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	else strFile.Format("%s%s\\%s\\%srender.t2lm", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	
	FILE* pLMFile = fopen(strFile, "rb");
	if (!pLMFile)
	{
		AUX_ProgressLog("CLightMapTransition::LoadLightMap, Couldn't open project (%d) lightmap file", iProject);
		return NULL;
	}
	else
	{
		//	Read lightmap file version and Identity
		TRN2FILEIDVER IdVer;
		if (fread(&IdVer, 1, sizeof (IdVer), pLMFile) != sizeof (IdVer))
		{
			AUX_ProgressLog("CLightMapMergerB::ExportProject, project (%d) lightmap file is corrupt", iProject);
			goto failed;
		}

		if (IdVer.dwIdentify != T2LMFILE_IDENTIFY || IdVer.dwVersion > T2LMFILE_VERSION)
		{
			AUX_ProgressLog("CLightMapMergerB::ExportProject, project (%d) lightmap file identity or version error", iProject);
			goto failed;
		}

		if (IdVer.dwVersion <= 1)
		{
			AUX_ProgressLog("CLightMapMergerB::ExportProject, project (%d) lightmap file identity or version error", iProject);
			goto failed;
		}
		else	//	IdVer.dwVersion >= 2
		{
			fread(&m_Header2, 1, sizeof (m_Header2), pLMFile);
			iNumMaskArea = m_Header2.iNumMaskArea;
			iMaskLMSize	 = m_Header2.iMaskLMSize;
			//	Source lightmap file shouldn't be DDS format
			if (m_Header2.dwFlags & T2LMFILE_DDS)
			{
				AUX_ProgressLog("CLightMapMergerB::ExportProject, project (%d) lightmap is DDS format", iProject);
				goto failed;
			}

		}

		if(!m_bLoaded)
		{
			m_iLMSize = CalcLightmapSize(iNumMaskArea, iMaskLMSize);
			m_LMVersion = IdVer;
			if(m_aLMOffsets) delete []m_aLMOffsets;
			m_aLMOffsets = new DWORD[iNumMaskArea];
			m_bLoaded = true;
		}

		//	Create lightmap object
		if (!(pBitmap = new CELBitmap))
		{
			AUX_ProgressLog("CLightMapTransition::AllocExportRes, Not enough memory !");
			goto failed;
		}
	
		if (!pBitmap->CreateBitmap(m_iLMSize, m_iLMSize, 24, NULL))
		{
			AUX_ProgressLog("CLightMapTransition::AllocExportRes, Not enough memory !");
			goto failed;
		}
		pBitmap->Clear();

		if(!ReadLightmapData(pLMFile,iNumMaskArea,iMaskLMSize,pBitmap))
		{
			AUX_ProgressLog("CLightMapTransition::ReadLightmapData, Read data error %s!",strProject);
			goto failed;
		}
		
	}
	fclose(pLMFile);
	return pBitmap;

failed:
	if(pBitmap)
	{
		pBitmap->Release();
		delete pBitmap;
	}
	fclose(pLMFile);
	return NULL;	
}

//	Calculate lightmap size
int CLightMapTransition::CalcLightmapSize(int iMaskNum, int iMaskGrid)
{
	int iMaskCol = (int)(sqrt((float)iMaskNum) + 0.5f);
	return iMaskCol * iMaskGrid;
}

//	Read lightmap data from .t2lm file
bool CLightMapTransition::ReadLightmapData(FILE* fp, int iMaskNum, int iMaskLMSize,CELBitmap *pBitmap)
{
	ASSERT(pBitmap);

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

	if(!pBitmap || pBitmap->GetWidth() != iMaskCol * iMaskLMSize || pBitmap->GetHeight() != iMaskRow * iMaskLMSize)
	{	
		delete [] pMaskLMData;
		AUX_ProgressLog("CLightMapTransition::ReadLightmapData, light map size is different(%d != %d)!",pBitmap->GetWidth(), iMaskRow * iMaskLMSize);
		return false;
	}

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
			pBitmap->LockRect(rcLock, &LockInfo);

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
	
	ConnectSubMap(pBitmap,iMaskLMSize,iMaskRow,iMaskCol);
	delete [] pMaskLMData;
	delete [] aOffsets;
	return true;
}

CELBitmap::LOCKINFO GetSubMap(CELBitmap *pBitmap, int r, int c, int iMaskSize, int iMaskRow, int iMaskCol)
{
	CELBitmap::LOCKINFO LockInfo;
	LockInfo.pData = NULL;

	if(r >= iMaskRow || r < 0) return LockInfo;
	if(c >= iMaskCol || c < 0) return LockInfo;
	
	ARectI rcLock;
	rcLock.left		= iMaskSize * c;
	rcLock.right	= rcLock.left + iMaskSize;
	rcLock.top		= r * iMaskSize;
	rcLock.bottom	= rcLock.top + iMaskSize;

	pBitmap->LockRect(rcLock, &LockInfo);

	return LockInfo;
}

void CLightMapTransition::ConnectSubMap(CELBitmap *pBitmap, int iMaskSize, int iMaskRow, int iMaskCol)
{
	
	ARectI rcLock;
	int r, c;
	CELBitmap::LOCKINFO LockInfo1;
	CELBitmap::LOCKINFO LockInfo2;
	CELBitmap::LOCKINFO LockInfo3;
	int iLineDataLen = iMaskSize * 3;

	for (r=0; r < iMaskRow; r++)
	{
		
		for (c=0; c < iMaskCol; c++)
		{
			LockInfo1 = GetSubMap(pBitmap,r,c,iMaskSize, iMaskRow, iMaskCol);
			LockInfo2 = GetSubMap(pBitmap,r,c+1,iMaskSize, iMaskRow, iMaskCol);
			LockInfo3 = GetSubMap(pBitmap,r+1,c,iMaskSize, iMaskRow, iMaskCol);
			if(LockInfo2.pData)
			{
				BYTE* pDst = LockInfo1.pData;
				BYTE* pSrc = LockInfo2.pData;
				for( int i = 0; i < LockInfo1.iHeight; i ++)
				{
					memcpy(pDst + iLineDataLen - 3, pSrc, 3);
					pSrc += LockInfo2.iPitch;
					pDst += LockInfo1.iPitch;
				}
			}
			if(LockInfo3.pData)
			{
				BYTE* pDst = LockInfo1.pData;
				BYTE* pSrc = LockInfo3.pData;
				pDst += LockInfo1.iPitch * (LockInfo1.iHeight -1);
				memcpy(pDst, pSrc, iLineDataLen);
			}
		}
	}
}

//	Release resources used during exporting terrain
void CLightMapTransition::ReleaseExportRes()
{
	if (m_aLMOffsets)
	{
		delete [] m_aLMOffsets;
		m_aLMOffsets = NULL;
	}
}

bool CLightMapTransition::WriteLightMap(CELBitmap *pBitmap, CString strProject,bool bNight)
{
	if(pBitmap==NULL || !m_bLoaded) return false;
	//	Make project lightmap file name
	CString strFile;
	if(bNight) strFile.Format("%s%s\\%s\\%srender.t2lm1", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	else strFile.Format("%s%s\\%s\\%srender.t2lm", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	FILE* pLMFile;
	if (!(pLMFile = fopen(strFile, "wb+")))
	{
		AUX_ProgressLog("CLightMapTransition::WriteLightMap, Failed to create lightmap file %s", strFile);
		return false;
	}
	
	fwrite(&m_LMVersion, 1, sizeof (m_LMVersion), pLMFile);
	fwrite(&m_Header2, 1, sizeof (m_Header2), pLMFile);
	fwrite(m_aLMOffsets, 1, iNumMaskArea * sizeof (DWORD), pLMFile);

	int iMaskCol = (int)(sqrt((float)iNumMaskArea) + 0.5f);
	int iMaskRow = iMaskCol;
	int iLineDataLen =iMaskLMSize * 3;

	ARectI rcLock;
	int r, c, iMaskArea;
	CELBitmap::LOCKINFO LockInfo;
	BYTE* pDst = new BYTE[iLineDataLen];
	for (r=0; r < iMaskRow; r++)
	{
		rcLock.left		= 0;
		rcLock.right	= iMaskLMSize;
		rcLock.top		= r * iMaskLMSize;
		rcLock.bottom	= rcLock.top + iMaskLMSize;

		iMaskArea = r * iMaskCol;

		for (c=0; c < iMaskCol; c++, iMaskArea++)
		{
			pBitmap->LockRect(rcLock, &LockInfo);

			//	Copy lightmap data
			BYTE* pSrc = LockInfo.pData;
			DWORD dwOffset = ftell(pLMFile);
			m_aLMOffsets[iMaskArea] = dwOffset;
			
			for (int i=0; i < LockInfo.iHeight; i++)
			{
				memcpy(pDst, pSrc, iLineDataLen);
				fwrite(pDst, 1, iLineDataLen, pLMFile);
				pSrc += LockInfo.iPitch;
			}
			rcLock.left	 += iMaskLMSize;
			rcLock.right += iMaskLMSize;	
		}
	}
	//	Rewrite light map offsets to file
	fseek(pLMFile, sizeof (TRN2FILEIDVER) + sizeof (m_Header2), SEEK_SET);
	fwrite(m_aLMOffsets, 1, iNumMaskArea * sizeof (DWORD), pLMFile);
	if(pDst) delete []pDst;
	fclose(pLMFile);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// class CLightMapBatchProcess
	
CLightMapBatchProcess::CLightMapBatchProcess()
{
	m_dwNumCol = 1;
}

CLightMapBatchProcess::~CLightMapBatchProcess()
{
	ReleaseProjList();
}

bool CLightMapBatchProcess::LoadProjList()
{
	ReleaseProjList();
	
	// Load project file list
	AScriptFile File;
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	if (!File.Open(szProj))
	{
		g_Log.Log("Open file %s failed!", szProj);
		return false;
	}
	
	if(File.GetNextToken(true))
	{
		CString strCol(File.m_szToken);
		if(strCol.Find("col=")!=-1)
		{
			strCol.Replace("col=","");
			m_dwNumCol = (int)atof(strCol);
		}else
		{
			File.ResetScriptFile();
			m_dwNumCol = 1;
		}
			
	}

	while (File.GetNextToken(true))
	{
		CString *pNew = new CString(File.m_szToken);
		ASSERT(pNew);
		m_Array.push_back(pNew);
	}
	File.Close();
	return true;
}

void CLightMapBatchProcess::ReleaseProjList()
{
	for( int i = 0; i < m_Array.size(); ++i)
		delete m_Array[i];

	m_Array.clear();
}

bool CLightMapBatchProcess::GetProjectInfor(const CString strCurrentPrj, int pNeiIndices[], CString pNeiName[])
{
	int nCurrent = -1;
	
	// Release array
	for(int i = 0; i< (int)m_Array.size();i++)
	{
		CString* p = m_Array.at(i);
		if(0==strcmp(*p,strCurrentPrj))
			nCurrent = i;
	}

	if(nCurrent==-1) return false;
	
	// 0 1 2
	// 3 4 5
	// 6 7 8
	if(nCurrent+1 <= m_dwNumCol || (nCurrent+1)%m_dwNumCol==1) pNeiIndices[0] = -1;
	else pNeiIndices[0] = (nCurrent - m_dwNumCol) - 1;
	
	if(nCurrent+1 <= m_dwNumCol) pNeiIndices[1] = -1;
	else pNeiIndices[1] = (nCurrent - m_dwNumCol);
	
	if((nCurrent+1)%m_dwNumCol==0 || nCurrent+1 <= m_dwNumCol) pNeiIndices[2] = -1;
	else pNeiIndices[2] = (nCurrent - m_dwNumCol) + 1;

	if((nCurrent+1)%m_dwNumCol==1) pNeiIndices[3] = -1;
	else pNeiIndices[3] = (nCurrent) - 1;
	
	pNeiIndices[4] = (nCurrent);
	
	if((nCurrent+1)%m_dwNumCol==0) pNeiIndices[5] = -1;
	else pNeiIndices[5] = (nCurrent) + 1;

	if((nCurrent+1)%m_dwNumCol==1 || (nCurrent + m_dwNumCol) - 1 == nCurrent ||(nCurrent+1)/m_dwNumCol >= (int)m_Array.size()/m_dwNumCol) pNeiIndices[6] = -1;
	else pNeiIndices[6] = (nCurrent + m_dwNumCol) - 1;
	
	if((nCurrent+1)/m_dwNumCol >= (int)m_Array.size()/m_dwNumCol) pNeiIndices[7] = -1;
	else pNeiIndices[7] = (nCurrent + m_dwNumCol);
	
	if((nCurrent+1)%m_dwNumCol==0 || (nCurrent+1)/m_dwNumCol >= (int)m_Array.size()/m_dwNumCol) pNeiIndices[8] = -1;
	else pNeiIndices[8] = (nCurrent + m_dwNumCol) + 1;

	for(i = 0; i < 9; i++)
	{
		if(pNeiIndices[i] < 0 || pNeiIndices[i] >= (int)m_Array.size())
			pNeiIndices[i] = -1;
		
		int index = pNeiIndices[i];
		
		if(index==-1) pNeiName[i] = "";
		else pNeiName[i] = *m_Array.at(index);
	}
	return true;
}

bool CLightMapBatchProcess::Process()
{
	AUX_ProgressLog("-- 开始处理灯光图之间的过渡 ...");
	g_Render.GetA3DDevice()->Reset();
	if(!LoadProjList()) 
	{
		AfxMessageBox("不能取得地图排列信息数据!");
		return false;
	}
	CElMapCache cache;
	for(int i = 0; i< (int)m_Array.size();i++)
	{
		CString* proj = m_Array.at(i);
		g_Log.Log("CLightMapBatchProcess::Process(),Start to process project %s !",*proj);
		int pNeiIndices[9];
		CString pNeiName[9];
		
		AUX_ProgressLog("-- 正处理地图 (%d/%d) ...",i+1,(int)m_Array.size());
		
		if(!GetProjectInfor(*proj,pNeiIndices,pNeiName)) 
		{
			AfxMessageBox("不能取得当前地图的九个相邻地图!");
			return false;
		}
	
		CExpElementMap *pCurMap = cache.AddStartMap(*proj);
		if(pCurMap==NULL) 
		{
			AfxMessageBox("在Cache中不能取得当前地图指针!");
			return false;
		}
		
		int nQuad = GetQuadrant(pCurMap,false);
		if(nQuad==1) // 1 quadrant 
		{
			pNeiName[0] = ""; pNeiName[2] = "";
			pNeiName[1] = "";
		}
		if(nQuad==2) // 2 quadrant
		{	
			pNeiName[0] = ""; pNeiName[2] = ""; pNeiName[6] = "";
			pNeiName[1] = ""; pNeiName[3] = "";
		}

		if(nQuad==3) // 3 quadrant
		{
			pNeiName[0] = ""; pNeiName[6] = "";
			pNeiName[3] = "";
		}

		if(nQuad==4) // 4 quadrant
		{
			pNeiName[2] = ""; pNeiName[6] = "";
		}
		//因为在同时存在多张地图时内存可能不够用，而内存占用最大的是模型的凸包
		//所以我把模型的凸包按时存放到一个临时文件中，算完光后我再把他读回来:(
		if(!pCurMap->GetSceneObjectMan()->ExportConvexHullData("temp.chfs"))
			return false;

		cache.AddProject(pNeiName);
		if(!ProcessProject(pCurMap,*proj,&cache,false,pNeiName))
			g_Log.Log("CLightMapBatchProcess::Process(),Failed to process project%d light!",i);
	}
	
	AUX_ProgressLog("-- 开始做灯光图边缘对齐处理 ...");
	CLightMapTransition trans;
	trans.DoTrans();
	
	AUX_ProgressLog("-- 所有灯光处理已经完成!");
	return true;
}

/*
	Get quadrant of sun's direction
	2 z|  1
	___|___x
	   |
	3  |  4
*/
int CLightMapBatchProcess::GetQuadrant(CExpElementMap *pCurMap,bool bIsNight)
{
	LIGHTINGPARAMS litParams;
	if(bIsNight) litParams = pCurMap->GetNightLightingParams();
	else litParams = pCurMap->GetLightingParams();
	
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

bool CLightMapBatchProcess::ProcessProject(CExpElementMap *pCurMap,CString strCurProj,CElMapCache *pCache,bool bIsNight,CString pNeiName[])
{
	//	Export terrain data to temporary directory
	CString strName = strCurProj;
	CString strPath;
	//	Fill terrain exporter initial data
	strName += "Render";
	strPath.Format("%s%s", g_szWorkDir, pCurMap->GetResDir());
	
	CTerrainExportor::INITDATA InitData;
	memset(&InitData, 0, sizeof (InitData));

	InitData.szName		= strName;
	InitData.szDestDir	= strPath;
	InitData.iBlockGrid	= g_Configs.iTExpBlkGrid;
	InitData.iMaskTile	= AUX_GetMaskTileSize();
	InitData.iNumCol	= 1;
	InitData.iNumRow	= 1;
	InitData.pLMGen		= NULL;

	CLightMapGen LMGen;         // Light for day
	CLightMapGen LMNightGen;    // Light for night
	
	
	//	Create a light map generator
	if(g_bOnlyCalMiniMap)
	{
		LIGHTINGPARAMS lpa = pCurMap->GetLightingParams();
		lpa.dwAmbient = A3DCOLORRGB(192,192,192);
		lpa.dwSunCol  = A3DCOLORRGB(192,192,192);
		lpa.fSunPower = 1.0f;
		if(!LMGen.Init(lpa))
		{
			g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize light map generator");
			return false;
		}
	}else if (!LMGen.Init(pCurMap->GetLightingParams()))
	{
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize light map generator");
		return false;
	}
	//  A temp solution, we want LightGen can adjust the light result of this map's scene objects
	LMGen.SetCurrentMap(pCurMap);
	
	if(g_bOnlyCalMiniMap)
	{
		LIGHTINGPARAMS lpa = pCurMap->GetLightingParams();
		lpa.dwAmbient = A3DCOLORRGB(192,192,192);
		lpa.dwSunCol  = A3DCOLORRGB(192,192,192);
		lpa.fSunPower = 1.0f;
		if(!LMNightGen.Init(lpa))
		{
			g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize light map generator");
			return false;
		}
	}else if(!LMNightGen.Init(pCurMap->GetNightLightingParams()))
	{
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize night light map generator");
		return false;
	}
	LMNightGen.SetCurrentMap(pCurMap);
	InitData.pLMGen	= &LMGen;
	InitData.pLMNightGen = &LMNightGen;
	//  I will remove this case, put it to other where!
	pCurMap->GetSceneObjectMan()->UpdateLighting(false);
	
	
	//	Create terrain exporter
	CTerrainExportor Exportor;
	Exportor.m_bLightNeighbor = true;
	Exportor.m_bLightMapTans  = true;
	Exportor.m_pCache = pCache;
	if (!Exportor.Init(InitData))
	{
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize terrain exporter");
		return false;
	}

	//	Add project names ...
	for(int i = 0; i < 9; i++)
		Exportor.m_strExtProj[i] = pNeiName[i];//Add by XQF
	
	Exportor.AddProject(strCurProj);

	bool bRet;
	if (!(bRet = Exportor.DoExport()))
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to export terrain data");

	Exportor.Release();
	LMGen.Release();
	LMNightGen.Release();
	
	//先把其他地图释放掉再加载回凸包数据，以免内存不够
	pCache->ReleaseOthers(strCurProj);
	if(!pCurMap->GetSceneObjectMan()->ImportConvexHullData("temp.chfs"))
		return false;
	pCurMap->SaveLitModel();
	return bRet;
}
