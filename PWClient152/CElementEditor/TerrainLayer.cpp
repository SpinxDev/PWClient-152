/*
 * FILE: TerrainLayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "TerrainLayer.h"
#include "Terrain.h"
#include "EL_Archive.h"
#include "ElementMap.h"
#include "Bitmap.h"
#include "AutoMask.h"
#include "DataChunkFile.h"

#include "AF.h"
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


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack (push, 1)

//	Terrain layer file data struct
struct LAYERFILEDATA
{
	bool	bDefault;		//	true, this is default layer
	float	fScaleU;		//	Scale on U axis
	float	fScaleV;		//	Scale on V axis
	int		iProjAxis;		//	Projection axis
	bool	bEnable;		//	Enable flag
	int		iMaskType;		//	Mask map type
	int		iAutoMaskSize;	//	Auto mask texture size
	float	fAltiStart;		//	Altitude start
	float	fAltiEnd;		//	Altitude end
	float	fSlopeStart;	//	Slope start
	float	fSlopeEnd;		//	Slope end
	DWORD	dwAutoMaskAxis;	//	Axis flag used to generate auto mask
	int		iWeight;		//	Layer's weight
	char	aReserved[64];	//	Reserved
};

#pragma pack (pop)

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CTerrainLayer
//	
///////////////////////////////////////////////////////////////////////////

CTerrainLayer::CTerrainLayer(CTerrain* pTerrain)
{
	m_pTerrain			= pTerrain;
	m_fScaleU			= 1.0f;
	m_fScaleV			= 1.0f;
	m_iProjAxis			= 1;
	m_bEnable			= true;
	m_bDefault			= false;
	m_iMaskMapType		= MASK_NONE;
	m_iAutoMaskSize		= DEFMASK_SIZE;
	m_pMaskBmp			= NULL;
	m_fAltiStart		= 0.0f;
	m_fAltiEnd			= 1.0f;
	m_fSlopeStart		= 0.0f;
	m_fSlopeEnd			= 1.0f;
	m_bAutoMaskSmth		= true;	
	m_iWeight			= 0;
	m_dwAutoMaskAxis	= AXIS_X | AXIS_Y | AXIS_Z;
}

CTerrainLayer::~CTerrainLayer()
{
}

//	Initalize object
bool CTerrainLayer::Init(const char* szName, bool bDefault)
{
	if (bDefault)
		m_strName = "Default";
	else
		m_strName = szName;

	m_bDefault = bDefault;

	if (!bDefault)
	{
	}

	return true;
}

//	Release object
void CTerrainLayer::Release()
{
	//	Release mask map
	ReleaseMaskMap();

	m_strTexFile	= "";
	m_strSpecuMap	= "";
}

//	Create mask map object
bool CTerrainLayer::CreateMaskMap(int iSize)
{
	//	Release current mask at first
	ReleaseMaskMap();

	if (!(m_pMaskBmp = new CELBitmap))
	{
		g_Log.Log("CTerrainLayer::CreateMaskMap, Not enough memory!");
		return false;
	}

	if (!m_pMaskBmp->CreateBitmap(iSize, iSize, 8, NULL))
	{
		g_Log.Log("CTerrainLayer::CreateMaskMap, Failed to create bitmap!");
		return false;
	}

	return true;
}

//	Release auto mask texture
void CTerrainLayer::ReleaseMaskMap()
{
	if (m_pMaskBmp)
	{
		m_pMaskBmp->Release();
		delete m_pMaskBmp;
		m_pMaskBmp = NULL;
	}
}

/*	Change texture

	Return true for success, otherwise return false.

	szFile: new texture file name. NULL means release current texture
*/
bool CTerrainLayer::ChangeTexture(const char* szFile)
{
	if (szFile && szFile[0])
		af_GetRelativePath(szFile, m_strTexFile);
	else
		m_strTexFile = "";

	return true;
}

//	Change specular map
//	szFile: new specular map file name. NULL means release current specular map
bool CTerrainLayer::ChangeSpecularMap(const char* szFile)
{
	if (szFile && szFile[0])
		af_GetRelativePath(szFile, m_strSpecuMap);
	else
		m_strSpecuMap = "";

	return true;
}

/*	Change mask map type

	iType: new type of mask map
*/
void CTerrainLayer::ChangeMaskType(int iType)
{
	//	Auto mask has no effect on default layer
	if (m_bDefault)
		return;

	if (m_iMaskMapType == iType)
		return;

	m_iMaskMapType = iType;

	if (iType == MASK_NONE)
	{
		ReleaseMaskMap();
	}
	else if (iType == MASK_FROMFILE)
	{
		ReleaseMaskMap();
	}
	else if (iType == MASK_AUTO)
	{
		if (!UpdateAutoMaskMap(m_iAutoMaskSize))
			g_Log.Log("CTerrainLayer::ChangeMaskType, Failed to call UpdateAutoMaskMap!");
	}
	else	//	iType == MASK_EDIT
	{
		if (!m_pMaskBmp && !CreateMaskMap(DEFMASK_SIZE))
			g_Log.Log("CTerrainLayer::ChangeMaskType, Failed to call CreateMaskMap!");
	}
}

//	Update auto mask texture content
bool CTerrainLayer::UpdateAutoMaskMap(int iNewSize)
{
	if (m_iMaskMapType != MASK_AUTO)
		return false;

	if (!m_pMaskBmp || m_iAutoMaskSize != iNewSize)
	{
		//	Create new auto mask map
		if (!CreateMaskMap(iNewSize))
		{
			g_Log.Log("CTerrainLayer::UpdateAutoMaskMap, Failed to create auto mask map!");
			return false;
		}

		m_iAutoMaskSize	= iNewSize;
	}

	ASSERT(m_pMaskBmp);

	float fAltiStart = m_pTerrain->GetYOffset() + m_fAltiStart * m_pTerrain->GetMaxHeight();
	float fAltiEnd = m_pTerrain->GetYOffset() + m_fAltiEnd * m_pTerrain->GetMaxHeight();
	float fSlopeStart = m_fSlopeStart * A3D_PI * 0.5f;
	float fSlopeEnd = m_fSlopeEnd * A3D_PI * 0.5f;

	//	Create temporary height buffer
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	ASSERT(pHeightMap->pAbsHeight);

	BYTE* aMaskBytes = m_pMaskBmp->GetBitmapData();
	if (m_pMaskBmp->GetWidth() != m_pMaskBmp->GetPitch())
	{
		if (!(aMaskBytes = new BYTE [m_pMaskBmp->GetWidth() * m_pMaskBmp->GetHeight()]))
		{
			g_Log.Log("CTerrainLayer::UpdateAutoMaskMap, Not enough memory !");
			return false;
		}
	}

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	GenerateMask(pHeightMap->iWidth, pHeightMap->pAbsHeight, pHeightMap->aNormals, m_pTerrain->GetTileSize(),
				fAltiStart,	fAltiEnd, fSlopeStart, fSlopeEnd, m_dwAutoMaskAxis, m_pMaskBmp->GetWidth(),
				aMaskBytes, m_bAutoMaskSmth ? (float)m_pMaskBmp->GetWidth() * 2.0f / (pHeightMap->iWidth-1) : 0.0f);
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if (aMaskBytes != m_pMaskBmp->GetBitmapData())
	{
		BYTE* pDstLine = m_pMaskBmp->GetBitmapData();
		BYTE* pSrcLine = aMaskBytes;
			
		for (int i=0; i < m_pMaskBmp->GetHeight(); i++)
		{
			memcpy(pDstLine, pSrcLine, m_pMaskBmp->GetWidth());
			pDstLine += m_pMaskBmp->GetPitch();
			pSrcLine += m_pMaskBmp->GetWidth();
		}

		delete [] aMaskBytes;
	}

	return true;
}

//	Load mask map from file
bool CTerrainLayer::LoadMaskMap(const char* szFile)
{
	//	Release current mask at first
	ReleaseMaskMap();

	if (!(m_pMaskBmp = new CELBitmap))
	{
		g_Log.Log("CTerrainLayer::LoadMaskMap, Not enough memory!");
		return false;
	}

	if (!m_pMaskBmp->LoadAsGrayBitmap(szFile, 0, 0))
	{
		g_Log.Log("CTerrainLayer::LoadMaskMap, Failed to create bitmap!");
		return false;
	}

	return true;
}

//	Load data
bool CTerrainLayer::Load(CELArchive& ar, int iLayerIdx)
{
	//	Load layer information
	LAYERFILEDATA Data;
	ar.Read(&Data, sizeof (Data));

	m_bDefault			= Data.bDefault;
	m_iMaskMapType		= Data.iMaskType;
	m_bEnable			= Data.bEnable;
	m_iProjAxis			= Data.iProjAxis;
	m_iAutoMaskSize		= Data.iAutoMaskSize;
	m_fScaleU			= Data.fScaleU;
	m_fScaleV			= Data.fScaleV;
	m_fAltiStart		= Data.fAltiStart;
	m_fAltiEnd			= Data.fAltiEnd;
	m_fSlopeStart		= Data.fSlopeStart;
	m_fSlopeEnd			= Data.fSlopeEnd;
	m_dwAutoMaskAxis	= Data.dwAutoMaskAxis;
	m_iWeight			= Data.iWeight;

	//	Read layer's name
	m_strName = ar.ReadString();

	//	Read texture file name
	CString strFile = ar.ReadString();
	ChangeTexture(strFile);

	//	Read specular map file name
	strFile = ar.ReadString();
	ChangeSpecularMap(strFile);

	//	Read mask map file name
	strFile = ar.ReadString();
	if (strFile.GetLength())
	{
		DWORD dwVersion = m_pTerrain->GetVersion();
		if (dwVersion < 2)
		{
			CString strFullFile;
			strFullFile.Format("%s%s\\%s", g_szWorkDir, m_pTerrain->GetMap()->GetResDir(), strFile);

			if (!LoadMaskMap(strFullFile))
			{
				g_Log.Log("CTerrainLayer::Load: Failed to load mask map %s", strFullFile);
				return false;
			}
		}
		else	//	dwVersion >= 2
		{
			CDataChunkFile* pMaskMapFile = m_pTerrain->GetMaskMapFile();

			if (!(m_pMaskBmp = new CELBitmap))
			{
				g_Log.Log("CTerrainLayer::Load, Not enough memory!");
				return false;
			}

			if (!pMaskMapFile->LoadGrayBitmap(iLayerIdx, m_pMaskBmp))
			{
				g_Log.Log("CTerrainLayer::Load: Failed to load mask map");
				return false;
			}
		}
	}

	return true;
}

//	Save data
bool CTerrainLayer::Save(CELArchive& ar, int iLayerIdx)
{
	//	Fill layer info.
	LAYERFILEDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.bDefault		= m_bDefault;
	Data.iMaskType		= m_iMaskMapType;
	Data.bEnable		= m_bEnable;
	Data.iProjAxis		= m_iProjAxis;
	Data.iAutoMaskSize	= m_iAutoMaskSize;
	Data.fScaleU		= m_fScaleU;
	Data.fScaleV		= m_fScaleV;
	Data.fAltiStart		= m_fAltiStart;
	Data.fAltiEnd		= m_fAltiEnd;
	Data.fSlopeStart	= m_fSlopeStart;
	Data.fSlopeEnd		= m_fSlopeEnd;
	Data.dwAutoMaskAxis	= m_dwAutoMaskAxis;
	Data.iWeight		= m_iWeight;
	
	//	Write layer info
	if (!ar.Write(&Data, sizeof (Data)))
	{
		g_Log.Log("CTerrainLayer::Save, Failed to write layer info.");
		return false;
	}

	//	Write layer's name
	ar.WriteString(m_strName);

	//	Write texture file name
	ar.WriteString(m_strTexFile);

	//	Write specular file name
	ar.WriteString(m_strSpecuMap);

	//	Save mask map data
	if (!SaveMaskMap(ar, iLayerIdx))
	{
		g_Log.Log("CTerrainLayer::Save, Failed to save mask map data.");
		return false;
	}

	return true;
}

//	Save mask map data
bool CTerrainLayer::SaveMaskMap(CELArchive& ar, int iLayerIdx)
{
//	TERRAINFILE_VERSION < 2
// 	if (!m_pMaskBmp)
// 	{
// 		ar.WriteString("");
// 		return true;
// 	}
// 
// 	int iIndex = m_pTerrain->GetLayerIndex(this);
// 	ASSERT(iIndex >= 0);
// 
// 	//	Mask mask file name
// 	char szFile[MAX_PATH], szFullPath[MAX_PATH];
// 	sprintf(szFile, "LayerMask%03d.bmp", iIndex);
// 	sprintf(szFullPath, "%s%s\\%s", g_szWorkDir, m_pTerrain->GetMap()->GetResDir(), szFile);
// 
// 	//	Write mask map file name
// 	ar.WriteString(szFile);
// 
// 	if (!m_pMaskBmp->SaveToFile(szFullPath))
// 	{
// 		g_Log.Log("CTerrainLayer::SaveMaskMap, Failed to save bitmap data to file.");
// 		return false;
// 
// 	}


	//	Use below codes when TERRAINFILE_VERSION >= 2 !!!
	if (!m_pMaskBmp)
	{
		ar.WriteString("");
		return true;
	}

	//	Write mask map file name
	char szFile[MAX_PATH];
	sprintf(szFile, "LayerMask%03d.bmp", iLayerIdx);
	ar.WriteString(szFile);

	CDataChunkFile* pMaskMapFile = m_pTerrain->GetMaskMapFile();

	if (!pMaskMapFile->SaveGrayBitmap(iLayerIdx, m_pMaskBmp))
	{
		g_Log.Log("CTerrainLayer::SaveMaskMap: Failed to load mask map");
		return false;
	}

	return true;
}

/*	Edit mask content directly

	rcArea: pixel area in mask map
	pModifyData: modify data of mask map
*/
bool CTerrainLayer::EditMaskMap(const ARectI& rcArea, int* pModifyData)
{
	if (m_bDefault)
		return false;
	
	ChangeMaskType(MASK_EDIT);

	ASSERT(rcArea.left >= 0 && rcArea.top >= 0 && 
		rcArea.right <= m_pMaskBmp->GetWidth() &&
		rcArea.bottom <= m_pMaskBmp->GetHeight());

	CELBitmap::LOCKINFO LockInfo;
	m_pMaskBmp->LockRect(rcArea, &LockInfo);

	BYTE* pSrcLine = (BYTE*)pModifyData;
	BYTE* pDstLine = LockInfo.pData;
	int iSrcPitch = rcArea.Width() * sizeof (int);

	for (int i=0; i < rcArea.Height(); i++)
	{
		int* pSrc = (int*)pSrcLine;

		for (int j=0; j < rcArea.Width(); j++)
		{
			int c = pDstLine[j] + pSrc[j];
			a_Clamp(c, 0, 255);
			pDstLine[j] = (BYTE)c;
		}

		pSrcLine += iSrcPitch;
		pDstLine += LockInfo.iPitch;
	}

	m_pTerrain->SetUpdateRenderFlag(true);

	return true;
}

