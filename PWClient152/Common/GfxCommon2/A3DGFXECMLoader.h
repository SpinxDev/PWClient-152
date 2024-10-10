/*
* FILE: A3DGFXECMLoader.h
*
* DESCRIPTION: Helper classes for A3DGFXECModel (to deal with different types of loading process)
*
* CREATED BY: Zhangyachuan, 2010/03/01
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _ECMLoader_H_
#define _ECMLoader_H_


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

class A3DGFXECModel;
class CECModel;
class IGFXECMInterface;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECMLoader
//	
///////////////////////////////////////////////////////////////////////////

class ECMLoader
{

public:		//	Types

	enum 
	{
		TYPE_NORMAL = 0,
		TYPE_CUSTOM_SKIN,
		TYPE_CUSTOM_ECM,
		TYPE_NUMBER,
	};

public:		//	Constructor and Destructor

	explicit ECMLoader(A3DGFXECModel* pECMEle);
	virtual ~ECMLoader() = 0 {}

	virtual ECMLoader* Clone(A3DGFXECModel* pECMEle) const = 0;
	static ECMLoader* Create(int iLdType, A3DGFXECModel* pECMEle);
	static void Destroy(ECMLoader* pLoader);

public:		//	Attributes

public:		//	Operations

	inline int GetLoaderType() const { return m_iECMLoaderType; }
	virtual bool OnInit(CECModel* pModel) = 0;
	virtual bool OnPlay(CECModel* pModel) = 0;
	virtual void Clean(CECModel* pModel) {}
	virtual void Update(CECModel* pModel, const AString& strActName) {}

protected:	//	Attributes

	A3DGFXECModel* m_pECMEle;
	int m_iECMLoaderType;

protected:	//	Operations

	void TransferProperties(CECModel* pModel);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class NormalLoader
//	
///////////////////////////////////////////////////////////////////////////

class NormalLoader : public ECMLoader
{
public:
	NormalLoader(A3DGFXECModel* pECMEle);

protected:
	virtual ECMLoader* Clone(A3DGFXECModel* pECMEle) const;
	virtual bool OnInit(CECModel* pModel);
	virtual bool OnPlay(CECModel* pModel);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CustomECMLoader
//	
///////////////////////////////////////////////////////////////////////////

class CustomECMLoader : public ECMLoader
{
public:
	explicit CustomECMLoader(A3DGFXECModel* pECMEle);
protected:
	bool m_bStartLoadECM;
	virtual ECMLoader* Clone(A3DGFXECModel* pECMEle) const;
	virtual bool OnInit(CECModel* pModel);
	virtual bool OnPlay(CECModel* pModel);
	virtual void Clean(CECModel* pModel);
	virtual void Update(CECModel* pModel, const AString& strActName);
	bool LoadCustomECModel(IGFXECMInterface* pGfxLoadECM, CECModel* pModel);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CustomSkinLoader
//	
///////////////////////////////////////////////////////////////////////////

class CustomSkinLoader : public ECMLoader
{
public:
	explicit CustomSkinLoader(A3DGFXECModel* pECMEle);
protected:
	virtual ECMLoader* Clone(A3DGFXECModel* pECMEle) const;
	virtual bool OnInit(CECModel* pModel);
	virtual bool OnPlay(CECModel* pModel);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_ECMLoader_H_


