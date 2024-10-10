/*
 * FILE: A3DMuscleOrgan.h
 *
 * DESCRIPTION: Muscle as an organ, this is the control interface and data defination
 *
 * CREATED BY: Hedi, 2004/7/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMUSCLEORGAN_H_
#define _A3DMUSCLEORGAN_H_

#include "A3DTypes.h"
#include "AArray.h"
#include "A3DObject.h"
#include "vector.h"

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

class A3DMuscleMeshImp;

enum MUSCLE_TYPE
{
	MUSCLE_TYPE_SINGLEDIR = 0,
	MUSCLE_TYPE_DOUBLEDIR = 1,
	MUSCLE_TYPE_CIRCLEDIR
};

struct MUSCLEDATA
{
	char			szName[32];		// muscle name

	A3DVECTOR3		vecCenter;		// muscle control point center
	A3DVECTOR3		vecMuscleDir;	// muscle end direction, this is vecCenter - vecEnd
	float			vLength;		// muscle length
	
	// parameter to describe the affect regions.
	A3DVECTOR3		vecXAxis;		// muscle orientation x
	A3DVECTOR3		vecYAxis;		// muscle orientation y
	A3DVECTOR3		vecZAxis;		// muscle orientation z
	A3DVECTOR3		vecRadius;		// radius at x, y and z axis
	float			vPitch;			// muscle affect region pitch value

	MUSCLE_TYPE		type;			// muscle type

	float			vTension;		// current tension of the muscle

	abase::vector<int>idLink;		// linked muscle id
};


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DMuscleData
//		the muscle's decription data which are exported from  3DS Max
//	
///////////////////////////////////////////////////////////////////////////

class A3DMuscleData	: public A3DObject
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DMuscleData();
	virtual ~A3DMuscleData();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();
	//	Load from disc or memory
	bool Load(AFile* pFile);
	bool Load(const char* szFile);
	//	Save to disc or memory
	bool Save(AFile* pFile);
	bool Save(const char* szFile);

	inline int GetNumMuscle()							{ return m_nNumMuscle; }
	inline const MUSCLEDATA& GetMuscleData(int nIndex) 	{ return m_pMuscles[nIndex]; }
	void SetMuscleTension(int nIndex, float vTension);

	const A3DMuscleData& operator = (const A3DMuscleData& data2);

protected:	//	Attributes
	int				m_nNumMuscle;	// number of muscles
	MUSCLEDATA*		m_pMuscles;		// muscle definition data

	// Create Muscle Buffer
	bool CreateMuscles();
	// Release muscle buffer
	bool ReleaseMuscles();

protected:	//	Operations
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DMuscleOrgan
//		muscle controller, it is one to one with A3DMuscleMeshImp, and make
//	use of the muscle data as well as the original information stored in
//	A3DMuscleMesh
//	
///////////////////////////////////////////////////////////////////////////

class A3DMuscleOrgan : public A3DObject
{
public:		//	Types
	
	// the affected region information for one muscle
	struct MUSCLEAFR
	{
		int							nNumVert;		// number of verts
		abase::vector<int>			idVert;			// id list of verts
		abase::vector<float>		weightVert;		// weight list of verts
		abase::vector<A3DVECTOR3>	vecDelta;		// muscle's dir * length in all bones
	};

public:		//	Constructor and Destructor

	A3DMuscleOrgan();
	virtual ~A3DMuscleOrgan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(int nNumMatrices, const A3DMATRIX4 * pInitMatrices, A3DMuscleMeshImp* pHostMesh, const A3DMuscleData& muscleData);
	//	Release object
	virtual void Release();

	//	update organ's data 
	virtual bool Update(int nDeltaTime);

	// calcualte all muscles' affected region information using curretn muscle data
	bool UpdateAffectedRegion();

	//	Deform the mesh using current afr informtion
	virtual bool DeformMesh();

	//	muscle tension control method
	//	get number of muscle
	int GetMuscleCount();
	//	get one muscle's name by its index
	const char * GetMuscleName(int nIndex);
	//	get muscle's tension
	float GetMuscleTension(int nIndex);
	float GetMuscleTensionByName(const char * szName);
	//	set muscle's tension
	void SetMuscleTension(int nIndex, float vTension);
	void SetMuscleTensionByName(const char * szName, float vTension);
	//	save current muscle tensions
	bool SaveTensions(const char * szFile);
	bool SaveTensions(AFile* pFile);
	//	load a set of muscle tensions from file
	bool LoadTensions(const char * szFile);
	bool LoadTensions(AFile* pFile);


	inline bool IsChanged()			{ return m_bChanged; }
	inline void ClearChangeFlag()	{ m_bChanged = false; }

protected:	//	Attributes
	A3DMuscleData		m_muscleData;		// current muscle data configuration
	int					m_nNumInitMatrices;	// number of init matrices
	A3DMATRIX4*			m_pInitMatrices;	// blend matrices when bind
	MUSCLEAFR*			m_pAffectedRegions;	// affected region for all muscles
	A3DMuscleMeshImp*	m_pHostMesh;		// muscle mesh which use this organ
	bool				m_bChanged;			// flag indicates if any muscle's tension has been adjusted

protected:	//	Operations
	// create init matrices
	bool CreateInitMatrices(int nNumMatrices, const A3DMATRIX4* pInitMatrices);
	// release init matrices
	bool ReleaseInitMatrices();
	// create an empty affected region buffer
	bool CreateAffectedRegion();
	// release curretn region information data
	void ReleaseAffectedRegion();
	// update the affected region information of one muscle
	bool UpdateMuscleAFR(int nIndex);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMUSCLEORGAN_H_
