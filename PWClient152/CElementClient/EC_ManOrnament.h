/*
 * FILE: EC_ManOrnament.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Manager.h"
#include "EC_Counter.h"
#include "hashtab.h"
#include "AArray.h"
#include "AString.h"
#include "CDWithCH.h"
#include "aabbcd.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define USING_BRUSH_MAN
#define USING_TRIANGLE_MAN

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////
class CECOrnament;
class CECHomeOrnament;
class CECModel;
class CECScene;
class CAutoScene;
class AFile;
class A3DGFXEx;
class AM3DSoundBuffer;
class CECBrushMan;
class CECTriangleMan;

using namespace CHBasedCD;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECOrnamentMan
//	
///////////////////////////////////////////////////////////////////////////

class CECOrnamentMan : public CECManager
{
public:		//	Types

	//	Loaded EC model
	struct LDECMODEL
	{
		DWORD		dwID;		//	Model ID
		CECModel*	pModel;		//	Model object
	};

	//	EC model node
	struct ECMODELNODE
	{
		CECModel*	pModel;		//	Model object
		int			nBrushes;	//	number of brush object used in collision
		CCDBrush**	ppBrushes;	//	Brush object used in collision
		A3DVECTOR3	vInitPos;
		A3DVECTOR3	vInitDir;
		A3DVECTOR3	vInitUp;
		AString		strActName;
		bool TraceWithBrush(BrushTraceInfo * pInfo);
	};

	//	Sound node
	struct SOUNDNODE
	{
		AString		strFile;	//	File name
		A3DVECTOR3	vPos;		//	Sound position
		float		fMinDist;	//	Sound distance
		float		fMaxDist;
		int			iValidTime;	//	0-in daylight; 1-in night; 2: 24-hour
		bool 		bLoaded;	//	loaded flag: true means the sound buffer has been loaded or loading failed. 

		AM3DSoundBuffer*	pSound;		//	Sound object
	};
	
	//	Gfx node
	struct GFXNODE
	{
		AString		strFile;	//	File name
		A3DVECTOR3	vPos;		//	position
		A3DVECTOR3	vDir;		//	direction
		A3DVECTOR3	vUp;		//	up
		float		fScale;		//	scale
		float		fSpeed;		//	play speed
		int			iValidTime;	//	0-in daylight; 1-in night; 2: 24-hour
		float		fAlpha;		//	alpha factor from scene editor
		bool		bAttenuation; // turn off when far then a predefined distance
		float		max_alpha;	//	alpha value store in gfx
		bool 		bLoaded;	//	loaded flag: true means the sound buffer has been loaded or loading failed. 

		A3DGFXEx *	pGfx;		//	A3DGFXEx object 
	};

	typedef abase::hashtab<CECOrnament*, int, abase::_hash_function> OnmtTable;
	typedef abase::hashtab<CECHomeOrnament*, int, abase::_hash_function> HomeOnmtTable;
	typedef abase::hashtab<GFXNODE*, int, abase::_hash_function> GFXTable;
	typedef abase::hashtab<ECMODELNODE*, int, abase::_hash_function> ECModelTable;
	typedef abase::hashtab<SOUNDNODE*, int, abase::_hash_function> SoundTable;

public:		//	Constructor and Destructor

	CECOrnamentMan(CECGameRun* pGameRun);
	virtual ~CECOrnamentMan();

public:		//	Attributes

public:		//	Operations

	//	Release manager
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	//	RenderForReflected routine
	virtual bool RenderForReflect(CECViewport * pViewport);
	//	RenderForRefract routine
	virtual bool RenderForRefract(CECViewport * pViewport);

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	On entering game world
	virtual bool OnEnterGameWorld();
	//	On leaving game world
	virtual bool OnLeaveGameWorld();

	//	Ray trace
	virtual bool RayTrace(ECRAYTRACE* pTraceInfo);
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, float * pFraction, A3DVECTOR3& vNormal);

	// Trace for CD
	virtual bool TraceWithBrush(BrushTraceInfo * pInfo);
	
	//	Load ornament from file
	CECOrnament* LoadOrnament(DWORD dwID, CECScene* pScene, const char* szMapPath, float fOffX, float fOffZ);
	//	Release ornament of specified ID
	void ReleaseOrnament(DWORD dwOnmtID);
	//	Get ornament through it's ID
	CECOrnament* GetOrnament(DWORD dwOnmtID);

	//	Load home ornament from file
	CECHomeOrnament* LoadHomeOrnament(DWORD dwID, CAutoScene* pScene, const char* szMoxFile, const char * szCHFFile, const A3DMATRIX4& matTM);
	//	Release home ornament of specified ID
	void ReleaseHomeOrnament(DWORD dwOnmtID);
	//	Get home ornament through it's ID
	CECHomeOrnament* GetHomeOrnament(DWORD dwOnmtID);

	//	Load effect from file
	DWORD LoadEffect(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ,int subTrn);
	//	Release effect of specified ID
	void ReleaseEffect(DWORD dwEffectID);
	//	Get effect through it's ID
	GFXNODE * GetEffect(DWORD dwEffectID);
	//	Load static EC model from file
	DWORD LoadECModel(AFile* pFile, float fOffX, float fOffZ,int subTrn);
	//	Release static EC model of specified ID
	void ReleaseECModel(DWORD dwModelID);
	//	Get static EC model through it's ID
	ECMODELNODE* GetECModel(DWORD dwModelID);
	//	Load sound object from file
	DWORD LoadSoundObject(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ,int subTrn);
	//	Release sound object
	void ReleaseSoundObject(DWORD dwSoundID);
	//	Get sound buffer object through it's ID
	SOUNDNODE* GetSoundObject(DWORD dwSoundID);

	//	Get Brush Man
#ifdef USING_BRUSH_MAN
	CECBrushMan* GetBrushMan() { return m_pBrushMan; }
#endif

	// Get Triangle Man
#ifdef USING_TRIANGLE_MAN
	CECTriangleMan* GetTriangleMan() { return m_pTriangleMan; }
	void LoadTriangleMan(const A3DVECTOR3& vCenter);
	void ReleaseTriangleMan(const A3DVECTOR3& vCenter);
	bool UsingTriangleMan() const { return m_bUsingTriMan; }
#endif

	//	Get object number
	int GetOrnamentNum() { return (int)m_OrnamentTab.size(); }
	int GetHomeOrnamentNum() { return (int)m_HomeOrnamentTab.size(); }
	int GetGFXNum() { return (int)m_GFXTab.size(); }
	int GetECModelNum() { return (int)m_ECModelTab.size(); }
	
	//	Load EC model in loading thread
	bool ThreadLoadECModel(DWORD dwModelID, const char* szFile);

	// temp for test only
	bool UpdateOrnamentsLight(A3DLIGHTPARAM paramDay, A3DLIGHTPARAM paramNight);

protected:	//	Attributes

	OnmtTable		m_OrnamentTab;		//	Ornament table
	HomeOnmtTable	m_HomeOrnamentTab;	//	Home ornament table
	GFXTable		m_GFXTab;			//	Gfx table
	ECModelTable	m_ECModelTab;		//	EC model table
	SoundTable		m_SoundTab;			//	Sound table
	CECCounter		m_SndTimeCnt;		//	Sound time counter

#ifdef USING_BRUSH_MAN
	CECBrushMan*	m_pBrushMan;
#endif

#ifdef USING_TRIANGLE_MAN
	CECTriangleMan*	m_pTriangleMan;
	bool m_bUsingTriMan;
#endif

	AArray<LDECMODEL, LDECMODEL&>	m_aLoadedECMs;	//	Loaded EC models
	CRITICAL_SECTION				m_csLoad;		//	m_aLoadedECMs lock

protected:	//	Operations

	//	Deliver loaded EC models
	void DeliverLoadedECModels();
	//	Load EC model from file
	CECModel* LoadECModelFromFile(const char* szFile);
	//	Delete EC model object
	void DeleteECModel(CECModel* pModel);

	//	Load sound from file
	bool LoadSoundFromFile(SOUNDNODE* pNode);
	//	Fade in/out sound
	void FadeSound(AM3DSoundBuffer * pSound, bool bFadeIn);
	//	Load gfx from file
	bool LoadGFXFromFile(GFXNODE * pNode);
	//	Fade in/out gfx
	void FadeGFX(GFXNODE * pNode, bool bFadeIn);

	//	Is valid time to load resource ?
	bool IsValidTime(float fDNFactor, int iValidTime)
	{
		if (iValidTime == 2 || (fDNFactor < 0.5f && !iValidTime) || (fDNFactor > 0.5f && iValidTime == 1))
			return true;
		else
			return false;
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

