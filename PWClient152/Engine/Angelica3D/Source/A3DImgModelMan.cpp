/*
 * FILE: A3DImgModelMan.cpp
 *
 * DESCRIPTION: Routines used to management Image models
 *
 * CREATED BY: duyuxin, 2002/11/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DImgModelMan.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DTextureMan.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "AScriptFile.h"
#include "A3DTexture.h"
#include "A3DViewport.h"
#include "AFileImage.h"
#include "AScriptFile.h"
#include "AFI.h"

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

//	Action sort compare function
static int _IMActionSortCompareFunc(const void *arg1, const void *arg2)
{
	IMGMODELACTION* pAct1 = (IMGMODELACTION*)arg1;
	IMGMODELACTION* pAct2 = (IMGMODELACTION*)arg2;

	if (pAct1->dwID < pAct2->dwID)
		return -1;
	else if (pAct1->dwID > pAct2->dwID)
		return 1;
	else
		return 0;
}

//	Mark point compare function
static int _IMMarkPtSortCompareFunc(const void *arg1, const void *arg2)
{
	A3DImageModelMan::MARKPOINT* pMarkPt1 = (A3DImageModelMan::MARKPOINT*)arg1;
	A3DImageModelMan::MARKPOINT* pMarkPt2 = (A3DImageModelMan::MARKPOINT*)arg2;

	if (pMarkPt1->dwID < pMarkPt2->dwID)
		return -1;
	else if (pMarkPt1->dwID > pMarkPt2->dwID)
		return 1;
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DImageModelMan
//
///////////////////////////////////////////////////////////////////////////

A3DImageModelMan::A3DImageModelMan()
{
	m_pA3DDevice	= NULL;
	m_iNumGroup		= 0;
	m_iAlphaRef		= 128;
	m_AlphaFunc		= A3DCMP_GREATEREQUAL;

	memset(m_aGroups, 0, sizeof (m_aGroups));
	memset(m_aValidGroups, 0, sizeof (m_aValidGroups));

	memset(m_aVertBuf, 0, sizeof (m_aVertBuf));
	memset(m_aIdxBuf, 0, sizeof (m_aIdxBuf));
}

A3DImageModelMan::~A3DImageModelMan()
{
}

/*	Initialize image model object.

	Return true for success, otherwise return false.

	pDevice: A3D Device object
*/
bool A3DImageModelMan::Init(A3DDevice *pDevice)
{
	m_pA3DDevice = pDevice;

	//	Initialize index buffer used to render image model
	int i, iCnt, iBase;

	for (i=0, iCnt=0; i < MAXNUM_RENDERMODEL; i++, iCnt+=6)
	{
		iBase = i << 2;

		m_aIdxBuf[iCnt+0] = iBase + 0;
		m_aIdxBuf[iCnt+1] = iBase + 1;
		m_aIdxBuf[iCnt+2] = iBase + 3;
		m_aIdxBuf[iCnt+3] = iBase + 1;
		m_aIdxBuf[iCnt+4] = iBase + 2;
		m_aIdxBuf[iCnt+5] = iBase + 3;
	}

	return true;
}

//	Release
void A3DImageModelMan::Release()
{
	ReleaseAllGroups();
}

//	Reset manager
bool A3DImageModelMan::Reset()
{
	ReleaseAllGroups();
	return true;
}

//	Release all groups
void A3DImageModelMan::ReleaseAllGroups()
{
	for (int i=0; i < m_iNumGroup; i++)
	{
		MODELGROUP* pGroup = m_aValidGroups[i];
		assert(pGroup);
		ReleaseModelGroup(pGroup);
	}

	m_iNumGroup = 0;
	memset(m_aGroups, 0, sizeof (m_aGroups));
	memset(m_aValidGroups, 0, sizeof (m_aValidGroups));
}

//	Release model group
void A3DImageModelMan::ReleaseModelGroup(int iGroup)
{
	assert(IsValidGroup(iGroup));
	
	MODELGROUP* pGroup = &m_aGroups[iGroup];

	//	Remove group resources
	ReleaseModelGroup(pGroup);

	//	Remove it from valid groups
	m_iNumGroup--;
	int i = 0;
	for (i=0; i < m_iNumGroup; i++)
	{
		if (m_aValidGroups[i] == pGroup)
			break;
	}

	if (i < m_iNumGroup)
		m_aValidGroups[i] = m_aValidGroups[m_iNumGroup];
		
	m_aValidGroups[m_iNumGroup] = NULL;
}

//	Release model group
void A3DImageModelMan::ReleaseModelGroup(MODELGROUP* pGroup)
{
	int j;

	if (pGroup->aTextures && m_pA3DDevice)
	{
		A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
		
		for (j=0; j < pGroup->iNumTexture; j++)
		{
			TEXTURE* pSlot = &pGroup->aTextures[j];
			if (pSlot->pTexture)
				pTexMan->ReleaseTexture(pSlot->pTexture);
			
			if (pSlot->aRModels)
				free(pSlot->aRModels);
		}
		
		free(pGroup->aTextures);
	}
	
	if (pGroup->aModels)
	{
		for (j=0; j < pGroup->iNumModel; j++)
		{
			if (pGroup->aModels[j])
			{
				pGroup->aModels[j]->Release();
				delete pGroup->aModels[j];
			}
		}
		
		free(pGroup->aModels);
	}
	
	if (pGroup->aActions)
		free(pGroup->aActions);
	
	if (pGroup->aCells)
		free(pGroup->aCells);

	if (pGroup->aMarkPoints)
		free(pGroup->aMarkPoints);

	pGroup->bValid = false;
}

/*	Load a model group

	Return model group's index for success, otherwise return -1

	szDescFile: group description file's (made by ImgModelEditor.exe) full
				path file name
*/
int A3DImageModelMan::LoadModelGroup(char* szDescFile)
{
	AFileImage File;
	AScriptFile Script;
	char szPath[MAX_PATH], *pTemp;
	
	sprintf(szPath, "%s\\%s", af_GetBaseDir(), szDescFile);

	if (!File.Open(szPath, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to open file %s", szDescFile);
		return -1;
	}

	if (!Script.Open(&File))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to open script file.");
		File.Close();
		return -1;
	}

	File.Close();

	if ((pTemp = strrchr(szPath, '\\')))
		*(pTemp+1) = '\0';
	else
		szPath[0] = '\0';

	MODELGROUP* pGroup = AllocEmptyGroup();
	if (!pGroup)
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to allocate new group");
		Script.Close();
		return -1;
	}

	pGroup->dwID = a_MakeIDFromString(szDescFile);

	//	First, load textures section
	if (!Script.MatchToken("sec:textures", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to find 'sec:textures' section.");
		goto Error;
	}
	else
	{
		char szTexPath[MAX_PATH];
		sprintf(szTexPath, "%stextures\\", szPath);
		
		if (!ParseTextureSectionInGroupFile(pGroup, &Script, szTexPath))
		{
			g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to parse textures section");
			goto Error;
		}
	}

//	Script.ResetScriptFile();

	//	Then, load actions section
	if (!Script.MatchToken("sec:actions", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to find 'sec:actions' section.");
		goto Error;
	}
	else if (!ParseActionSectionInGroupFile(pGroup, &Script))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to parse actions section");
		goto Error;
	}

//	Script.ResetScriptFile();

	//	Then, load cells section
	if (!Script.MatchToken("sec:cells", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to find 'sec:cells' section.");
		goto Error;
	}
	else if (!ParseCellSectionInGroupFile(pGroup, &Script))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to parse cells section");
		goto Error;
	}

//	Script.ResetScriptFile();

	//	Then, image models section
	if (!Script.MatchToken("sec:models", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to find 'sec:models' section.");
		goto Error;
	}
	else
	{
		char szModelPath[MAX_PATH];
		sprintf(szModelPath, "%smodeldescs\\", szPath);

		if (!ParseModelSectionInGroupFile(pGroup, &Script, szModelPath))
		{
			g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to parse image models section");
			goto Error;
		}
	}

//	Script.ResetScriptFile();

	//	Then, texture information section
	if (!Script.MatchToken("sec:textureinfo", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to find 'sec:textureinfo' section.");
		goto Error;
	}
	else if (!ParseTexInfoSectionInGroupFile(pGroup, &Script))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to parse textureinfo section");
		goto Error;
	}

//	Script.ResetScriptFile();

	//	Then, mark point section
	if (!Script.MatchToken("sec:markpoints", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to find 'sec:markpoints' section.");
		goto Error;
	}
	else if (!ParseMarkPtSectionInGroupFile(pGroup, &Script))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadModelGroup, Failed to parse mark points section");
		goto Error;
	}

	Script.Close();

	return pGroup - m_aGroups;

Error:

	ReleaseModelGroup(pGroup);
	m_iNumGroup--;

	Script.Close();
	return -1;
}

/*	Allocate a empty group

	Return group structure's address for success, otherwse return NULL.
*/
A3DImageModelMan::MODELGROUP* A3DImageModelMan::AllocEmptyGroup()
{
	if (m_iNumGroup >= MAXNUM_MODELGROUP)
	{
		g_A3DErrLog.Log("A3DImageModelMan::AllocEmptyGroup, Too many model groups");
		return NULL;
	}

	//	Search a empty group
	int i;
	for (i=0; i < MAXNUM_MODELGROUP; i++)
	{
		if (!m_aGroups[i].bValid)
			break;
	}

	assert(i < MAXNUM_MODELGROUP);

	MODELGROUP* pGroup = &m_aGroups[i];
	memset(pGroup, 0, sizeof (MODELGROUP));

	pGroup->bValid = true;

	m_aValidGroups[m_iNumGroup++] = pGroup;

	return pGroup;
}

/*	Parse 'textures' section in image model group file

	Return true for success, otherwise return false.

	pGroup: used to receive textures infromation
	pFile: image model group file
	szTexPath: path of texture files (having '\' at tail)
*/
bool A3DImageModelMan::ParseTextureSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile,
													  char* szTexPath)
{
	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::ParseTextureSectionInGroupFile, Failed to match {");
		return false;
	}
	
	assert(m_pA3DDevice);
	A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

	char szFile[MAX_PATH];
	int iCount = 0;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(pFile->m_szToken, "texturenumber"))
		{
			pFile->GetNextToken(true);	//	Skip 'texturenumber'

			pFile->GetNextToken(true);
			pGroup->iNumTexture = atoi(pFile->m_szToken);

			if (pGroup->iNumTexture)
			{
				if (!(pGroup->aTextures = (TEXTURE*)malloc(sizeof (TEXTURE) * pGroup->iNumTexture)))
				{
					g_A3DErrLog.Log("A3DImageModelMan::ParseTextureSectionInGroupFile, not enough memory!");
					return false;
				}
				
				memset(pGroup->aTextures, 0, sizeof (TEXTURE) * pGroup->iNumTexture);
			}
		}
		else
		{
			TEXTURE* pSlot = AllocTextureSlot(pGroup, iCount++);

			//	Texture file name
			pFile->GetNextToken(true);

			A3DTexture* pTexture = NULL;
			sprintf(szFile, "%s%s", szTexPath, pFile->m_szToken);

			if (!pTexMan->LoadTextureFromFile(szFile, &pTexture, A3DTF_MIPLEVEL, 1))
				g_A3DErrLog.Log("A3DImageModelMan::ParseTextureSectionInGroupFile, Failed to load texture %s", szFile);
			else
				pSlot->pTexture = pTexture;

			if (pGroup->iTexType == TEXTURE_UNKNOWN)
			{
				if (af_CheckFileExt(szFile, ".bmp", 4))
					pGroup->iTexType = TEXTURE_BMP;
				else
					pGroup->iTexType = TEXTURE_TGA;
			}
		}

		pFile->SkipLine();
	}

	if (pGroup->iNumTexture != iCount)
	{
		assert(0);
		pGroup->iNumTexture = iCount;
	}

	return true;
}

/*	Parse 'actions' section in image model group file

	Return true for success, otherwise return false.

	pGroup: used to receive actions infromation
	pFile: image model group file
*/
bool A3DImageModelMan::ParseActionSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile)
{
	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::ParseActionSectionInGroupFile, Failed to match {");
		return false;
	}
	
	assert(m_pA3DDevice);

	int iCount = 0;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(pFile->m_szToken, "actionnumber"))
		{
			pFile->GetNextToken(true);	//	Skip 'actionnumber'

			pFile->GetNextToken(true);
			pGroup->iNumAction = atoi(pFile->m_szToken);

			if (pGroup->iNumAction)
			{
				if (!(pGroup->aActions = (IMGMODELACTION*)malloc(sizeof (IMGMODELACTION) * pGroup->iNumAction)))
				{
					g_A3DErrLog.Log("A3DImageModelMan::ParseActionSectionInGroupFile, not enough memory!");
					return false;
				}
				
				memset(pGroup->aActions, 0, sizeof (IMGMODELACTION) * pGroup->iNumAction);
			}
		}
		else
		{
			IMGMODELACTION* pAct = &pGroup->aActions[iCount++];

			//	Frame number
			pFile->GetNextToken(true);
			pAct->iNumFrame = atoi(pFile->m_szToken);

			//	The first cell
			pFile->GetNextToken(false);
			pAct->iFirstCell = atoi(pFile->m_szToken);

			//	The last cell
			pFile->GetNextToken(false);
			pAct->iLastCell = atoi(pFile->m_szToken);

			//	Loop
			pFile->GetNextToken(false);
			pAct->bLoop = atoi(pFile->m_szToken) ? true : false;

			//	Start frame
			pFile->GetNextToken(false);
			pAct->iStart = atoi(pFile->m_szToken);

			//	Action name
			pFile->GetNextToken(false);
			strcpy_s(pAct->szName, pFile->m_szToken);

			//	ID
			pAct->dwID = a_MakeIDFromString(pAct->szName);

			if (pAct->iFirstCell <= pAct->iLastCell)
				pAct->bForward = true;
			else
				pAct->bForward = false;
		}

		pFile->SkipLine();
	}

	if (pGroup->iNumAction != iCount)
	{
		assert(0);
		pGroup->iNumAction = iCount;
	}

#ifdef _DEBUG
	
	if (!CheckActionIDs(pGroup))
		assert(0);

#endif

	//	Sort actions by ID here...
	qsort(pGroup->aActions, pGroup->iNumAction, sizeof (IMGMODELACTION), _IMActionSortCompareFunc);

	return true;
}

/*	Parse 'cells' section in image model group file

	Return true for success, otherwise return false.

	pGroup: used to receive actions infromation
	pFile: image model group file
*/
bool A3DImageModelMan::ParseCellSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile)
{
	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::ParseCellSectionInGroupFile, Failed to match {");
		return false;
	}
	
	assert(m_pA3DDevice);

	int iCount = 0;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(pFile->m_szToken, "cellnumber"))
		{
			pFile->GetNextToken(true);	//	Skip 'cellnumber'

			pFile->GetNextToken(true);
			pGroup->iNumCell = atoi(pFile->m_szToken);

			if (pGroup->iNumCell)
			{
				if (!(pGroup->aCells = (IMGMODELCELL*)malloc(sizeof (IMGMODELCELL) * pGroup->iNumCell)))
				{
					g_A3DErrLog.Log("A3DImageModelMan::ParseCellSectionInGroupFile, not enough memory!");
					return false;
				}
				
				memset(pGroup->aCells, 0, sizeof (IMGMODELCELL) * pGroup->iNumCell);
			}
		}
		else
		{
			IMGMODELCELL* pCell = &pGroup->aCells[iCount++];

			//	Texture index
			pFile->GetNextToken(true);
			pCell->iTexture = atoi(pFile->m_szToken);

			//	Left, top, right and bottom
			pFile->GetNextToken(false);
			int x1 = atoi(pFile->m_szToken);

			pFile->GetNextToken(false);
			int y1 = atoi(pFile->m_szToken);

			pFile->GetNextToken(false);
			int x2 = atoi(pFile->m_szToken);

			pFile->GetNextToken(false);
			int y2 = atoi(pFile->m_szToken);

			//	u1
			pFile->GetNextToken(false);
			pCell->u1 = (float)atof(pFile->m_szToken);

			//	v1
			pFile->GetNextToken(false);
			pCell->v1 = (float)atof(pFile->m_szToken);

			//	u2
			pFile->GetNextToken(false);
			pCell->u2 = (float)atof(pFile->m_szToken);

			//	v2
			pFile->GetNextToken(false);
			pCell->v2 = (float)atof(pFile->m_szToken);

			//	Skip offsets in pixels
			pFile->GetNextToken(false);
			pFile->GetNextToken(false);

			//	width in metres
			pFile->GetNextToken(false);
			pCell->fWidth = (float)atof(pFile->m_szToken);

			//	height in metres
			pFile->GetNextToken(false);
			pCell->fHeight = (float)atof(pFile->m_szToken);

			//	x offset in metres
			pFile->GetNextToken(false);
			pCell->fOffx = (float)atof(pFile->m_szToken);

			//	y offset in metres
			pFile->GetNextToken(false);
			pCell->fOffy = (float)atof(pFile->m_szToken);
		}

		pFile->SkipLine();
	}

	if (pGroup->iNumCell != iCount)
	{
		assert(0);
		pGroup->iNumCell = iCount;
	}

	return true;
}

/*	Parse 'sec:models' section in image model group file

	Return true for success, otherwise return false.

	pGroup: used to receive actions infromation
	pFile: image model group file
	szModelPath: path of image model files
*/
bool A3DImageModelMan::ParseModelSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile,
													char* szModelPath)
{
	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::ParseModelSectionInGroupFile, Failed to match {");
		return false;
	}

	A3DImgModel* pModel;
	char szFile[MAX_PATH];
	int iCount = 0;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(pFile->m_szToken, "modelnumber"))
		{
			pFile->GetNextToken(true);	//	Skip 'modelnumber'

			pFile->GetNextToken(true);
			pGroup->iNumModel = atoi(pFile->m_szToken);

			if (pGroup->iNumModel)
			{
				if (!(pGroup->aModels = (A3DImgModel**)malloc(sizeof (A3DImgModel*) * pGroup->iNumModel)))
				{
					g_A3DErrLog.Log("A3DImageModelMan::ParseModelSectionInGroupFile, not enough memory!");
					return false;
				}
				
				memset(pGroup->aModels, 0, sizeof (A3DImgModel*) * pGroup->iNumModel);
			}
		}
		else
		{
			//	Model name
			pFile->GetNextToken(true);
			sprintf(szFile, "%s%s.im", szModelPath, pFile->m_szToken);

			if (!(pModel = LoadImageModel(szFile, pGroup-m_aGroups)))
				g_A3DErrLog.Log("A3DImageModelMan::ParseModelSectionInGroupFile, Failed to load image model file %s", szFile);
			else
				pGroup->aModels[iCount++] = pModel;
		}

		pFile->SkipLine();
	}

	if (pGroup->iNumModel != iCount)
	{
		assert(0);
		pGroup->iNumModel = iCount;
	}

	return true;
}

/*	Parse 'sec:textureinfo' section in image model group file

	Return true for success, otherwise return false.

	pGroup: used to receive actions infromation
	pFile: image model group file
*/
bool A3DImageModelMan::ParseTexInfoSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile)
{
	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::ParseTexInfoSectionInGroupFile, Failed to match {");
		return false;
	}

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(pFile->m_szToken, "origin") ||
			!_stricmp(pFile->m_szToken, "scale"))
		{
			//	Don't handle these
		}

		pFile->SkipLine();
	}

	return true;
}

/*	Parse 'sec:markpoints' section in image model group file

	Return true for success, otherwise return false.

	pGroup: used to receive actions infromation
	pFile: image model group file
*/
bool A3DImageModelMan::ParseMarkPtSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile)
{
	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImageModelMan::ParseMarkPtSectionInGroupFile, Failed to match {");
		return false;
	}
	
	assert(m_pA3DDevice);

	int iCount = 0;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(pFile->m_szToken, "marknumber"))
		{
			pFile->GetNextToken(true);	//	Skip 'marknumber'

			pFile->GetNextToken(true);
			pGroup->iNumMarkPt = atoi(pFile->m_szToken);

			if (pGroup->iNumMarkPt)
			{
				if (!(pGroup->aMarkPoints = (MARKPOINT*)malloc(sizeof (MARKPOINT) * pGroup->iNumMarkPt)))
				{
					g_A3DErrLog.Log("A3DImageModelMan::ParseMarkPtSectionInGroupFile, not enough memory!");
					return false;
				}
				
				memset(pGroup->aMarkPoints, 0, sizeof (MARKPOINT) * pGroup->iNumMarkPt);
			}
		}
		else
		{
			MARKPOINT* pMarkPt = &pGroup->aMarkPoints[iCount++];

			//	Skip x and y
			pFile->GetNextToken(true);
			pFile->GetNextToken(false);

			//	Offset in metres
			pFile->GetNextToken(false);
			pMarkPt->fOffx = (float)atof(pFile->m_szToken);
			
			pFile->GetNextToken(false);
			pMarkPt->fOffy = (float)atof(pFile->m_szToken);
			
			//	Skip texture index
			pFile->GetNextToken(false);

			//	Name
			pFile->GetNextToken(false);
			strcpy_s(pMarkPt->szName, pFile->m_szToken);
			
			pMarkPt->dwID = a_MakeIDFromString(pMarkPt->szName);
		}

		pFile->SkipLine();
	}

	if (pGroup->iNumMarkPt != iCount)
	{
		assert(0);
		pGroup->iNumMarkPt = iCount;
	}

#ifdef _DEBUG
	
	if (!CheckMarkPtIDs(pGroup))
		assert(0);

#endif

	//	Sort mark points by ID here...
	qsort(pGroup->aMarkPoints, pGroup->iNumMarkPt, sizeof (MARKPOINT), _IMMarkPtSortCompareFunc);

	return true;
}

/*	Check action ID collision

	Return true if no same ID was found.

	pGroup: group whose actions will be checked
*/
bool A3DImageModelMan::CheckActionIDs(MODELGROUP* pGroup)
{
	int i, j;
	IMGMODELACTION* pAct;

	for (i=0; i < pGroup->iNumAction-1; i++)
	{
		pAct = &pGroup->aActions[i++];

		for (j=i+1; j < pGroup->iNumAction; j++)
		{
			if (pAct->dwID == pGroup->aActions[j].dwID)
				return false;
		}
	}

	return true;
}

/*	Check mark point ID collision

	Return true if no same ID was found.

	pGroup: group whose actions will be checked
*/
bool A3DImageModelMan::CheckMarkPtIDs(MODELGROUP* pGroup)
{
	int i, j;
	MARKPOINT* pMarkPt;

	for (i=0; i < pGroup->iNumMarkPt-1; i++)
	{
		pMarkPt = &pGroup->aMarkPoints[i++];

		for (j=i+1; j < pGroup->iNumMarkPt; j++)
		{
			if (pMarkPt->dwID == pGroup->aMarkPoints[j].dwID)
				return false;
		}
	}

	return true;
}

/*	Allocate a texture slot for specified slot.

	Return texture slot's address for success, otherwise return NULL.

	pGroup: specified group.
	iCount: texture slot's index.
*/
A3DImageModelMan::TEXTURE* A3DImageModelMan::AllocTextureSlot(MODELGROUP* pGroup, int iCount)
{
	TEXTURE* pSlot = &pGroup->aTextures[iCount];

	pSlot->aRModels	= (A3DImgModel**)malloc(16 * sizeof (A3DImgModel*));
	if (!pSlot->aRModels)
	{
		g_A3DErrLog.Log("A3DImageModelMan::AllocTextureSlot, not enough memory!");
		return NULL;
	}

	memset(pSlot->aRModels, 0, 16 * sizeof (A3DImgModel*));

	pSlot->iMaxRModel = 16;
	pSlot->iNumRModel = 0;

	return pSlot;
}

/*	Load a image model from file

	Return image model object's address for success, otherwise return NULL

	szModelFile: Image model file's name
	iGroup: current group's index
*/
A3DImgModel* A3DImageModelMan::LoadImageModel(char* szModelFile, int iGroup)
{
	A3DImgModel* pModel = new A3DImgModel;
	if (!pModel)
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadImageModel, not enough memory!");
		return NULL;
	}

	if (!pModel->Load(this, szModelFile, iGroup))
	{
		g_A3DErrLog.Log("A3DImageModelMan::LoadImageModel, Failed to initiaize image model!");
		return NULL;
	}

	return pModel;
}

/*	Find a image model

	Return image model object's address for success, otherwise return NULL

	szModelFile: Image model file's name
	iGroup: current group's index
*/
A3DImgModel* A3DImageModelMan::FindImageModel(int iGroup, char* szName)
{
	assert(IsValidGroup(iGroup));

	MODELGROUP* pGroup = &m_aGroups[iGroup];
	DWORD dwID = a_MakeIDFromString(szName);

	for (int i=0; i < pGroup->iNumModel; i++)
	{
		if (pGroup->aModels[i]->GetID() == dwID)
			return pGroup->aModels[i];
	}

	return NULL;
}

/*	Create a image model object

	Return image model's address for success, otherwise return NULL.

	iGroup: image model group's ID, which is returned by LoadModelGroup().
	szName: image model's name.
*/
A3DImgModel* A3DImageModelMan::CreateImageModel(int iGroup, char* szName)
{
	A3DImgModel* pDest = new A3DImgModel;
	if (!pDest)
	{
		g_A3DErrLog.Log("A3DImageModelMan::CreateImageModel, not enough memory!");
		return NULL;
	}
		
	if (!CopyImageModel(pDest, iGroup, szName))
	{
		pDest->Release();
		delete pDest;
		g_A3DErrLog.Log("A3DImageModelMan::CreateImageModel, Failed to copy image model!");
		return NULL;
	}

	return pDest;
}

/*	Release image model object

	pModel: image model object that was allocated by CreateImageModel()
*/
void A3DImageModelMan::ReleaseImageModel(A3DImgModel* pModel)
{
	if (pModel)
	{
		pModel->Release();
		delete pModel;
	}
}

/*	Copy a image model object. This is no special requirement for pDest except
	that it should be a valid image model object address.

	Return true for success, otherwise return false.

	pDest: image model whose data will be set
	iGroup: ID of image model group, in which source image model exists.
	szName: source image model's name.
*/
bool A3DImageModelMan::CopyImageModel(A3DImgModel* pDest, int iGroup, char* szName)
{
	assert(pDest);

	A3DImgModel* pSrc = FindImageModel(iGroup, szName);
	if (!pSrc)
	{
		g_A3DErrLog.Log("A3DImageModelMan::CopyImageModel, Failed to find iamge mode %s!", szName);
		return NULL;
	}

	return pDest->Copy(pSrc);
}

/*	Bit-copy a image model object. This function duplicate specified image
	model (source image model) to pDest. This function just do bit-copy 
	operations, so destination image model must be exactly same as source 
	image model and all needed memory in pDest must have been prepared.

	pDest: image model whose data will be set
	iGroup: ID of image model group, in which source image model exists.
	szName: source image model's name.
*/
bool A3DImageModelMan::BitCopyImageModel(A3DImgModel* pDest, int iGroup, char* szName)
{
	assert(pDest);

	A3DImgModel* pSrc = FindImageModel(iGroup, szName);
	if (!pSrc)
	{
		g_A3DErrLog.Log("A3DImageModelMan::BitCopyImageModel, Failed to find iamge mode %s!", szName);
		return NULL;
	}

	return pDest->BitCopy(pSrc);
}

/*	Get action through ID

	Return action's index in specified image model group for success, 
	otherwise return -1.

	iGroup: index of image model group, which the action belongs to
	dwID: action's ID.
	pAction (out): used to receive action data. can't be NULL
*/
int A3DImageModelMan::GetAction(int iGroup, DWORD dwID, IMGMODELACTION* pAction)
{
	assert(IsValidGroup(iGroup));

	MODELGROUP* pGroup = &m_aGroups[iGroup];

	if (!pGroup->iNumAction)
		return -1;

	IMGMODELACTION* aActions = pGroup->aActions;

	//	Because we have sorted actions by ID in LoadModelGroup(), we can 
	//	use half-search algorithm here.
	int b = 0;
	int e = pGroup->iNumAction - 1;

	if (aActions[b].dwID == dwID)
	{
		*pAction = aActions[b];
		return b;
	}

	if (aActions[e].dwID == dwID)
	{
		*pAction = aActions[e];
		return e;
	}

	while (b < e-1)
	{
		int m = (b + e) >> 1;

		if (aActions[m].dwID == dwID)
		{
			*pAction = aActions[m];
			return m;
		}
		else if (aActions[m].dwID > dwID)
			e = m;
		else
			b = m;
	}

	return -1;
}

/*	Get action through name

	Return action's index in specified image model group for success, 
	otherwise return -1.

	iGroup: index of image model group, which the action belongs to
	szName: action's name.
	pAction (out): used to receive action data. can't be NULL
*/
int A3DImageModelMan::GetAction(int iGroup, char* szName, IMGMODELACTION* pAction)
{
	DWORD dwID = a_MakeIDFromString(szName);
	return GetAction(iGroup, dwID, pAction);
}

/*	Get action through index

	Return true for success, otherwise return false.

	iGroup: index of image model group, which the action belongs to
	iIndex: action's index.
	pAction (out): used to receive action data. can't be NULL
*/
bool A3DImageModelMan::GetActionByIndex(int iGroup, int iIndex, IMGMODELACTION* pAction)
{
	assert(IsValidGroup(iGroup));

	MODELGROUP* pGroup = &m_aGroups[iGroup];

	if (iIndex < 0 || iIndex >= pGroup->iNumAction)
	{
		assert(0);
		return false;
	}

	*pAction = pGroup->aActions[iIndex];

	return true;
}

/*	Get mark point's offset

	Return true for success, otherwise return false.

	iGroup: index of image model group, which the action belongs to
	szName: mark point's name.
	pfx, pfy (out): used to receive mark point's offset, cannot be NULL
*/
bool A3DImageModelMan::GetMarkPoint(int iGroup, char* szName, float* pfx, float* pfy)
{
	DWORD dwID = a_MakeIDFromString(szName);
	return GetMarkPoint(iGroup, dwID, pfx, pfy);
}

/*	Get mark point's offset

	Return true for success, otherwise return false.

	iGroup: index of image model group, which the action belongs to
	dwID: mark point's ID.
	pfx, pfy (out): used to receive mark point's offset, cannot be NULL
*/
bool A3DImageModelMan::GetMarkPoint(int iGroup, DWORD dwID, float* pfx, float* pfy)
{
	assert(IsValidGroup(iGroup));

	MODELGROUP* pGroup = &m_aGroups[iGroup];

	if (!pGroup->iNumMarkPt)
		return false;

	MARKPOINT* aMarkPts = pGroup->aMarkPoints;

	//	Because we have sorted mark points by ID in LoadModelGroup(), we can 
	//	use half-search algorithm here.
	int b = 0;
	int e = pGroup->iNumMarkPt - 1;

	if (aMarkPts[b].dwID == dwID)
	{
		*pfx = aMarkPts[b].fOffx;
		*pfy = aMarkPts[b].fOffy;
		return true;
	}

	if (aMarkPts[e].dwID == dwID)
	{
		*pfx = aMarkPts[e].fOffx;
		*pfy = aMarkPts[e].fOffy;
		return true;
	}

	while (b < e-1)
	{
		int m = (b + e) >> 1;

		if (aMarkPts[m].dwID == dwID)
		{
			*pfx = aMarkPts[m].fOffx;
			*pfy = aMarkPts[m].fOffy;
			return true;
		}
		else if (aMarkPts[m].dwID > dwID)
			e = m;
		else
			b = m;
	}

	return false;
}

/*	Add a model to render pool

	Return true for success, otherwise return false.

	pModel: iamge model object's address
*/
bool A3DImageModelMan::AddRenderModel(A3DImgModel* pModel)
{
	assert(pModel);
	
	int iGroup = pModel->GetMGIndex();
	assert(IsValidGroup(iGroup));
	MODELGROUP* pGroup = &m_aGroups[iGroup];

	int iActIndex = pModel->GetCurActIndex();
	assert(iActIndex >= 0 && iActIndex < pGroup->iNumAction);
	IMGMODELACTION* pAct = &pGroup->aActions[iActIndex];

	int iCell = pModel->GetCurCell();
	assert(iCell >= 0 && iCell < pGroup->iNumCell);
	IMGMODELCELL* pCell = &pGroup->aCells[iCell];

	if (!AddRenderModelToQueue(&pGroup->aTextures[pCell->iTexture], pModel))
		return false;

	return true;
}

/*	Add a model which will be rendered to queue

	Return true for success, otherwise return false.

	pSlot: texture slot. 
	pModel: image model will be rendered
*/
bool A3DImageModelMan::AddRenderModelToQueue(TEXTURE* pSlot, A3DImgModel* pModel)
{
	if (!pSlot)
		return false;

	if (pSlot->iNumRModel >= pSlot->iMaxRModel)
	{
		int iNewSize = pSlot->iMaxRModel + 32;
		
		if (!(pSlot->aRModels = (A3DImgModel**)realloc(pSlot->aRModels, sizeof (A3DImgModel*) * iNewSize)))
			//	Not enough memory to extend buffer, so discard pModel
			return false;
		
		pSlot->iMaxRModel = iNewSize;
	}
	
	pSlot->aRModels[pSlot->iNumRModel++] = pModel;
	
	return true;
}

/*	Render image models. In most occasions, in order to get a correct rendering
	result for alpha images, application should set a proper alpha reference
	value and alpha compare function before call this routine.
*/
bool A3DImageModelMan::Render(A3DViewport* pViewport)
{
	if (!m_iNumGroup)
		return true;

	int i, j, k, n, iPass, iTail, iVertCnt, iModelCnt;
	MODELGROUP* pGroup;
	TEXTURE* pTexture;

	m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetAlphaRef(m_iAlphaRef);
	m_pA3DDevice->SetAlphaFunction(m_AlphaFunc);

	//	Set world matrix to identity matrix
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	int iTexType = TEXTURE_UNKNOWN;

	for (i=0; i < m_iNumGroup; i++)
	{
		pGroup = m_aValidGroups[i];
		assert(pGroup);

		//	Set different blend method for bmp and tga textures
		if (pGroup->iTexType != iTexType)
		{
			iTexType = pGroup->iTexType;

			if (pGroup->iTexType == TEXTURE_BMP)
			{
				m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
				m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCCOLOR);
				m_pA3DDevice->SetAlphaTestEnable(false);
			}
			else
			{
				m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
				m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
				m_pA3DDevice->SetAlphaTestEnable(true);
			}
		}

		//	Render models in every texture slot
		for (j=0; j < pGroup->iNumTexture; j++)
		{
			pTexture = &pGroup->aTextures[j];
			if (!pTexture->iNumRModel)
				continue;

			iPass = pTexture->iNumRModel >> RM_FACTORIAL;
			iTail = pTexture->iNumRModel & (MAXNUM_RENDERMODEL-1);

			if (pTexture->pTexture)
				pTexture->pTexture->Appear(0);
			else
				m_pA3DDevice->ClearTexture(0);

			iModelCnt = 0;

			for (k=0; k < iPass; k++)
			{
				iVertCnt = 0;
				for (n=0; n < MAXNUM_RENDERMODEL; n++)
				{
					pTexture->aRModels[iModelCnt+n]->PushVertsToRenderBuffer(&m_aVertBuf[iVertCnt]);
					iVertCnt += 4;
				}

				iModelCnt += MAXNUM_RENDERMODEL;

				//	Render...
				m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, MAXNUM_RENDERMODEL << 2,
													 MAXNUM_RENDERMODEL << 1, m_aIdxBuf, A3DFMT_INDEX16, 
													 m_aVertBuf, sizeof (A3DLVERTEX));
			}

			//	The last models
			if (iTail)
			{
				iVertCnt = 0;
				for (n=0; n < iTail; n++)
				{
					pTexture->aRModels[iModelCnt+n]->PushVertsToRenderBuffer(&m_aVertBuf[iVertCnt]);
					iVertCnt += 4;
				}

				//	Render...
				m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, iTail << 2,
													 iTail << 1, m_aIdxBuf, A3DFMT_INDEX16, 
													 m_aVertBuf, sizeof (A3DLVERTEX));
			}

			//	Clear render model counter of texture for next render tick
			pTexture->iNumRModel = 0;
		}
	}

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetAlphaTestEnable(false);

	return true;
}


