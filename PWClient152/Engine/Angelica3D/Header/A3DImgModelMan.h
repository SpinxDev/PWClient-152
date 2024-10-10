/*
 * FILE: A3DImgModelMan.h
 *
 * DESCRIPTION: Routines used to management Image models
 *
 * CREATED BY: duyuxin, 2002/11/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DIMGMODELMAN_H_
#define _A3DIMGMODELMAN_H_

#include "A3DVertex.h"
#include "A3DImgModel.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DTexture;
class A3DImgModel;
class A3DDevice;
class A3DViewport;
class AScriptFile;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DImageModelMan
//
///////////////////////////////////////////////////////////////////////////

class A3DImageModelMan
{
public:		//	Types

	enum
	{
		MAXNUM_MODELGROUP	= 128,		//	Maximum number of model group
		MAXNUM_RENDERMODEL	= 256,		//	Maximum nubmer of image model can be rendered together,
										//		Note: ensure this value be factorial of 2
		RM_FACTORIAL		= 8			//	MAXNUM_RENDERMODEL = 2 ^ RM_FACTORIAL
	};

	//	Texture type
	enum
	{
		TEXTURE_UNKNOWN		= 0,
		TEXTURE_BMP			= 1,
		TEXTURE_TGA			= 2,
	};

	struct TEXTURE
	{
		A3DTexture*		pTexture;		//	Texture pointer
		int				iFirstCell;		//	First cell's index
		int				iLastCell;		//	Last cell's index

		//	Used for render
		A3DImgModel**	aRModels;		//	Model rendered queue
		int				iMaxRModel;		//	Buffer length (in pointers) of aRModels
		int				iNumRModel;		//	Current number of model in aRModels
	};

	//	Mark point
	struct MARKPOINT
	{
		char			szName[32];		//	Name
		DWORD			dwID;			//	ID of mark point
		float			fOffx;			//	Offset in metres
		float			fOffy;
	};

	struct MODELGROUP
	{
		bool			bValid;			//	Valid flag
		DWORD			dwID;			//	Group ID
		int				iTexType;		//	Texture type of this group
		TEXTURE*		aTextures;		//	Texture array
		IMGMODELACTION*	aActions;		//	Action array
		IMGMODELCELL*	aCells;			//	Cell array
		A3DImgModel**	aModels;		//	Model array
		MARKPOINT*		aMarkPoints;	//	Mark point array
		int				iNumTexture;	//	Number of texture
		int				iNumAction;		//	Number of action
		int				iNumCell;		//	Number of cell
		int				iNumModel;		//	Number of model
		int				iNumMarkPt;		//	Number of mark points
	};

public:		//	Constructors and Destructors

	A3DImageModelMan();
	virtual ~A3DImageModelMan();

public:		//	Attributes

public:		//	Operations

	bool			Init(A3DDevice *pDevice);	//	Initialize object
	void			Release();					//	Release
	bool			Reset();					//	Reset manager

	int				LoadModelGroup(char* szDescFile);		//	Load a model group
	void			ReleaseModelGroup(int iGroup);			//	Release model group

	int				GetAction(int iGroup, DWORD dwID, IMGMODELACTION* pAction);			//	Get action through ID
	int				GetAction(int iGroup, char* szName, IMGMODELACTION* pAction);		//	Get action through name
	bool			GetActionByIndex(int iGroup, int iIndex, IMGMODELACTION* pAction);	//	Get action through index
	bool			GetMarkPoint(int iGroup, char* szName, float* pfx, float* pfy);	//	Get mark point's offset
	bool			GetMarkPoint(int iGroup, DWORD dwID, float* pfx, float* pfy);	//	Get mark point's offset

	A3DImgModel*	CreateImageModel(int iGroup, char* szName);		//	Create a image model object
	void			ReleaseImageModel(A3DImgModel* pModel);			//	Release image model object
	bool			CopyImageModel(A3DImgModel* pDest, int iGroup, char* szName);		//	Copy a image model object
	bool			BitCopyImageModel(A3DImgModel* pDest, int iGroup, char* szName);	//	Bit-copy a image model object

	bool			AddRenderModel(A3DImgModel* pModel);	//	Add a model which will be rendered
	bool			Render(A3DViewport* pViewport);			//	Render image models

	A3DDevice*		GetA3DDevice()		{	return m_pA3DDevice;	}

	inline bool		GetFrameCell(int iGroup, int iCell, IMGMODELCELL* pShape);		//	Get frame shape information

protected:	//	Attributes

	A3DDevice*		m_pA3DDevice;

	MODELGROUP		m_aGroups[MAXNUM_MODELGROUP];		//	Model group array
	MODELGROUP*		m_aValidGroups[MAXNUM_MODELGROUP];	//	Valid group pointer
	int				m_iNumGroup;						//	Number of valid model group

	A3DLVERTEX		m_aVertBuf[MAXNUM_RENDERMODEL * 4];	//	Vertex buffer used to render
	WORD			m_aIdxBuf[MAXNUM_RENDERMODEL * 6];	//	Index buffer used to render
	
	int				m_iAlphaRef;		//	Alpha compare value
	A3DCMPFUNC		m_AlphaFunc;		//	Alpha compare function when rendering

protected:	//	Operations

	void			ReleaseModelGroup(MODELGROUP* pGroup);		//	Release model group
	void			ReleaseAllGroups();			//	Release all groups
	MODELGROUP*		AllocEmptyGroup();			//	Allocate a empty group
	TEXTURE*		AllocTextureSlot(MODELGROUP* pGroup, int iCount);	//	Allocate a texture slot for specified slot
	A3DImgModel*	LoadImageModel(char* szModelFile, int iGroup);		//	Load a image model from file
	A3DImgModel*	FindImageModel(int iGroup, char* szName);			//	Find a image model
	bool			AddRenderModelToQueue(TEXTURE* pSlot, A3DImgModel* pModel);		//	Add a model which will be rendered to queue

	bool			ParseTextureSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile, char* szTexPath);	//	Parse 'sec:textures' section in image model group file
	bool			ParseActionSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile);		//	Parse 'sec:actions' section in image model group file
	bool			ParseCellSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile);		//	Parse 'sec:cells' section in image model group file
	bool			ParseModelSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile, char* szModelPath);	//	Parse 'sec:models' section in image model group file
	bool			ParseTexInfoSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile);		//	Parse 'sec:textureinfo' section in image model group file
	bool			ParseMarkPtSectionInGroupFile(MODELGROUP* pGroup, AScriptFile* pFile);		//	Parse 'sec:markpoints' section in image model group file

	bool			CheckActionIDs(MODELGROUP* pGroup);		//	Check action ID collision
	bool			CheckMarkPtIDs(MODELGROUP* pGroup);		//	Check mark point ID collision

	inline bool		IsValidGroup(int iGroup);		//	Check whether a group is valid group
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

//	Check whether a group is valid group
bool A3DImageModelMan::IsValidGroup(int iGroup)
{
	if (iGroup < 0 || iGroup >= MAXNUM_MODELGROUP || 
		!m_aGroups[iGroup].bValid)
		return false;

	return true;
}

/*	Get frame shape information.

	Return true for success, otherwise return false.

	iGroup: model group index.
	iCell: frame (image cell) index
	pCell: used to receive frame cell information, can't be NULL.
*/
bool A3DImageModelMan::GetFrameCell(int iGroup, int iCell, IMGMODELCELL* pCell)
{
	assert(IsValidGroup(iGroup));
	MODELGROUP* pGroup = &m_aGroups[iGroup];

	assert(iCell >= 0 && iCell < pGroup->iNumCell);
	*pCell = pGroup->aCells[iCell];

	return true;
}


#endif	//	_A3DIMGMODELMAN_H_


