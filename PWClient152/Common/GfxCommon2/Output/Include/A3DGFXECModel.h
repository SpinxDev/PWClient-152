/*
* FILE: A3DGFXECModel.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/01/13
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXECModel_H_
#define _A3DGFXECModel_H_

#include "A3DGFXElement.h"

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

class ECMLoader;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXECModel
//	
///////////////////////////////////////////////////////////////////////////

class A3DGFXECModel : public A3DGFXElement
{

public:		//	Types
	
	friend class ECMLoader;
	friend class NormalLoader;
	friend class CustomECMLoader;
	friend class CustomSkinLoader;

public:		//	Constructor and Destructor

	A3DGFXECModel(A3DGFXEx* pGfx);
	virtual ~A3DGFXECModel(void);

public:		//	Attributes

public:		//	Operations

	const char* GetECMFileName() const { return m_strECMFilename; }
	const char* GetUserCmd() const { return m_strUserCmd; }

protected:	//	Attributes

	ECMLoader* m_pLoader;
	AString m_strECMFilename;
	AString m_strActionName;
	AString m_strUserCmd;
	CECModel* m_pECModel;
	bool	m_bUseCasterSkin;

public:
	virtual A3DSkinModel* GetSkinModel();

protected:	//	Operations

	A3DGFXECModel& operator = (const A3DGFXECModel& src);
	bool InitECModelFile(const char* szFilename);
	void UpdateECModel(A3DMATRIX4 matTran, DWORD dwDiffuse, DWORD dwTickTime);
	
	virtual bool Play();
	virtual bool Stop();
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Render(A3DViewport* pA3DViewport);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual void DummyTick(DWORD dwTick);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual void PrepareRenderSkinModel();
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGFXECModel_H_


