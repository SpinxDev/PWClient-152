/*
 * FILE: A3DSkinModelAux.h
 *
 * DESCRIPTION: A3D skin model aux class
 *
 * CREATED BY: duyuxin, 2003/11/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINMODELAUX_H_
#define _A3DSKINMODELAUX_H_

#include "A3DTypes.h"
#include "A3DObject.h"

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

class A3DGraphicsFX;
class A3DSkinModel;
class AFile;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DHanger
//
///////////////////////////////////////////////////////////////////////////

class A3DHanger : public A3DObject
{
public:		//	Types

	//	Hanger type
	enum
	{
		TYPE_UNKNOWN = -1,
		TYPE_GFX = 0,
		TYPE_SKINMODEL,
	};

	//	Used to save and load hangers
	struct HANGERDATA
	{
		int		iType;		//	Hanger type
		bool	bOnBone;	//	On bone flag
	};

	friend class A3DSkinModel;

public:		//	Constructors and Destructors

	A3DHanger(A3DSkinModel* pSkinModel);
	virtual ~A3DHanger() {}
	
public:		//	Attributes

public:		//	Operations

	//	Load hanger from file
	virtual bool Load(AFile* pFile);
	//	Save hanger from file
	virtual bool Save(AFile* pFile);

	//	Load hanger object
	virtual bool LoadHangerObject() { return true; }
	//	Release hanger object
	virtual void ReleaseHangerObject() {}

	//	Update routine
	virtual bool Update(int iDeltaTime);

	//	Set bind info
	void SetBindInfo(bool bOnBone, const char* szBindTo) { m_bOnBone = bOnBone; m_strBindTo = szBindTo; }
	//	Is bound on bone ?
	bool IsBoundOnBone() { return m_bOnBone; }
	//	Get bind target name
	const char* GetBindName() { return m_strBindTo; }
	//	Get offset matrix
	const A3DMATRIX4& GetOffsetMat() { return m_matOffset; }
	//	Set offset matrix
	void SetOffsetMat(const A3DMATRIX4& mat) { m_matOffset = mat; }

	//	Set / Get hanger type
	void SetType(int iType) { m_iType = iType; }
	int GetType() { return m_iType; }
	//	Enable / disable inherit scale factor from hook
	void EnableInheritScale(bool bEnable) { m_bInheritScale = bEnable; }
	bool GetInheritScaleFlag() { return m_bInheritScale; }

protected:	//	Attributes

	A3DSkinModel*	m_pA3DSkinModel;	//	Skin model object

	int			m_iType;		//	Hanger type
	bool		m_bOnBone;		//	true, hanger is bound on bone, false, it is bound on hook
	AString		m_strBindTo;	//	Name of bone or hook, this depends on bOnBone
	int			m_iBindIndex;	//	Index of bind target, may be bone or hook, this depends on bOnBone

	A3DMATRIX4	m_matTM;		//	Transform matrix
	A3DMATRIX4	m_matOffset;	//	Offset matrix

	bool		m_bInheritScale;	//	true, inherit scale factor from hook
	float		m_fInheritScale;	//	Inherit scale factor

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DGFXHanger
//
///////////////////////////////////////////////////////////////////////////

class A3DGFXHanger : public A3DHanger
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DGFXHanger(A3DSkinModel* pSkinModel) : A3DHanger(pSkinModel)
	{ 
		m_iType		= TYPE_GFX;
		m_dwClassID	= A3D_CID_GFXHANGER;
		m_pGFX		= NULL;
	}

	virtual ~A3DGFXHanger();
	
public:		//	Attributes

public:		//	Operations

	//	Load hanger from file
	virtual bool Load(AFile* pFile);
	//	Save hanger from file
	virtual bool Save(AFile* pFile);

	//	Load hanger object
	virtual bool LoadHangerObject();
	//	Release hanger object
	virtual void ReleaseHangerObject();

	//	Update routine
	virtual bool Update(int iDeltaTime);

	//	Set / Get GFX file name
	void SetGFXFile(const char* szFile) { m_strGFXFile = szFile; }
	const char* GetGFXFile() { return m_strGFXFile; }

	//	Get GFX object
	A3DGraphicsFX* GetGFXObject() { return m_pGFX; }
	
protected:	//	Attributes

	AString			m_strGFXFile;	//	GFX file name
	A3DGraphicsFX*	m_pGFX;			//	GFX object

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinModelHanger
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinModelHanger : public A3DHanger
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DSkinModelHanger(A3DSkinModel* pSkinModel);
	virtual ~A3DSkinModelHanger();
	
public:		//	Attributes

public:		//	Operations

	//	Load hanger from file
	virtual bool Load(AFile* pFile);
	//	Save hanger from file
	virtual bool Save(AFile* pFile);

	//	Load hanger object
	virtual bool LoadHangerObject();
	//	Release hanger object
	virtual void ReleaseHangerObject();

	//	Update routine
	virtual bool Update(int iDeltaTime);

	//	Set hanger object
	bool SetHangerObject(A3DSkinModel* pModel);
	//	Unbind hanger object and this hanger. This function doesn't release hanger object
	A3DSkinModel* UnbindHangerObject();
	//	Get model object
	A3DSkinModel* GetModelObject() { return m_pModel; }

	//	Set / Get Model file name
	void SetModelFile(const char* szFile) { m_strModelFile = szFile; }
	const char* GetModelFile() { return m_strModelFile; }
	//	Set / Get connection center name
	void SetCCName(const char* szName) { m_strCCName = szName; }
	const char* GetCCName() { return m_strCCName; }

protected:	//	Attributes

	AString			m_strModelFile;		//	Model file name
	AString			m_strCCName;		//	Connection Center name
	A3DSkinModel*	m_pModel;			//	Model object
	int				m_iCCIndex;			//	CC hook index

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinHitBox
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinHitBox : public A3DObject
{
public:		//	Types

	//	Struct used to save and load box
	struct BOXDATA
	{
		int		iBone;	//	Index of bone
		A3DOBB	obb;	//	obb in bone space
	};

public:		//	Constructors and Destructors

	A3DSkinHitBox(A3DSkinModel* pA3DSkinModel)
	{ 
		m_dwClassID		= A3D_CID_SKINHITBOX;
		m_iBone			= -1;
		m_pA3DSkinModel = pA3DSkinModel;
	}

public:		//	Attributes

public:		//	Operations

	//	Load hit box from file
	virtual bool Load(AFile* pFile);
	//	Save hit box from file
	virtual bool Save(AFile* pFile);

	//	Update routine
	bool Update(int iDeltaTime);

	//	Set / Get index of bone to which the hit box attaches
	void SetBone(int iBone) { m_iBone = iBone; }
	int GetBone() { return m_iBone; }

	//	Set / Get obb in bone space
	void SetOBB(const A3DOBB& obb) { m_obb = obb; }
	const A3DOBB& GetOBB() { return m_obb; }

	//	Get transformed obb in world space
	const A3DOBB& GetWorldOBB() { return m_obbWorld; }

protected:	//	Attributes

	A3DSkinModel*	m_pA3DSkinModel;	//	Skin model object

	int		m_iBone;		//	Bone's index
	A3DOBB	m_obb;			//	obb in bone space
	A3DOBB	m_obbWorld;		//	Transformed obb in world space

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinPhyShape
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinPhyShape : public A3DObject
{
public:		//	Types

	//	Struct used to save and load physique shape
	struct PHYSHAPDEDATA
	{
		int			iBone;	//	Index of bone
		A3DCAPSULE	cc;		//	Capsule(Capped-Cylinder) in bone space
	};

public:		//	Constructors and Destructors

	A3DSkinPhyShape(A3DSkinModel* pA3DSkinModel)
	{ 
		m_dwClassID		= A3D_CID_SKINPHYSHAPE;
		m_iBone			= -1;
		m_pA3DSkinModel	= pA3DSkinModel;
	}

public:		//	Attributes

public:		//	Operations

	//	Load physique shape from file
	virtual bool Load(AFile* pFile);
	//	Save physique shape from file
	virtual bool Save(AFile* pFile);

	//	Update routine
	bool Update(int iDeltaTime);

	//	Set / Get index of bone to which the physique shape attaches
	void SetBone(int iBone) { m_iBone = iBone; }
	int GetBone() { return m_iBone; }

	//	Set / Get capsule in bone space
	void SetCapsule(const A3DCAPSULE& cc) { m_cc = cc; }
	const A3DCAPSULE& GetCapsule() { return m_cc; }

	//	Get transformed capsule in world space
	const A3DCAPSULE& GetWorldCapsule() { return m_ccWorld; }

protected:	//	Attributes

	A3DSkinModel*	m_pA3DSkinModel;	//	Skin model object

	int				m_iBone;	//	Bone's index
	A3DCAPSULE		m_cc;		//	Capsule in bone space
	A3DCAPSULE		m_ccWorld;	//	Transformed capsule in world space

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSKINMODELAUX_H_