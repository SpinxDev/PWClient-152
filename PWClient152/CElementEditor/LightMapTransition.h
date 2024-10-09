// LightMapTransition.h: interface for the CLightMapTransition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTMAPTRANSITION_H__F3E1CEF5_4E3D_4E6F_9B97_2087B23644C9__INCLUDED_)
#define AFX_LIGHTMAPTRANSITION_H__F3E1CEF5_4E3D_4E6F_9B97_2087B23644C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "vector.h"
#include "A3DTerrain2File.h"

class CElMapCache;
class CELBitmap;
class CExpElementMap;

enum LIGHTMAP_TRANS
{
	TRANS_RIGHT = 0,
	TRANS_BOTTOM,
};

class CLightMapTransition  
{
public:
	CLightMapTransition();
	virtual ~CLightMapTransition();

	bool DoTrans();
	bool LoadProjList();
	bool GetProjectInfor(const CString strCurrentPrj, int pNeiIndices[], CString pNeiName[]);
protected:
	CELBitmap* LoadLightMap(int iProject,CString strProject,bool bNight = false);
	
	int  CalcLightmapSize(int iMaskNum, int iMaskGrid);
	
	bool ReadLightmapData(FILE* fp, int iMaskNum, int iMaskLMSize, CELBitmap *pBitmap);

	void ReleaseExportRes();

	bool ProecssBitmap(CELBitmap *pCurrentBitmap, CELBitmap *pNeiBitmap,int iNei,int iTansPixel);
	
	bool WriteLightMap(CELBitmap *pBitmap, CString strProject,bool bNight = false);
	
	void ConnectSubMap(CELBitmap *pBitmap, int iMaskSize, int iMaskRow, int iMaskCol);

	abase::vector<CString *> m_Array;
	
	DWORD	m_dwNumCol;
	
	int     m_iLMSize;
	DWORD*  m_aLMOffsets;
	bool    m_bLoaded;

	TRN2FILEIDVER  m_LMVersion;
	T2LMFILEHEADER2 m_Header2;
	int iNumMaskArea;
	int iMaskLMSize;
};

class CLightMapBatchProcess
{
public:
	CLightMapBatchProcess();
	~CLightMapBatchProcess();

	bool Process();

protected:
	abase::vector<CString *> m_Array;
	DWORD m_dwNumCol;

protected:
	int  GetQuadrant(CExpElementMap *pCurMap,bool bIsNight);
	bool LoadProjList();
	void ReleaseProjList();
	bool ProcessProject(CExpElementMap *pCurMap,CString strCurProj,CElMapCache *pCache,bool bIsNight,CString pNeiName[]);
	bool GetProjectInfor(const CString strCurrentPrj, int pNeiIndices[], CString pNeiName[]);
};

#endif // !defined(AFX_LIGHTMAPTRANSITION_H__F3E1CEF5_4E3D_4E6F_9B97_2087B23644C9__INCLUDED_)
