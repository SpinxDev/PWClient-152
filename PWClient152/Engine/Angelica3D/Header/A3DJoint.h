/*
 * FILE: A3DJoint.h
 *
 * DESCRIPTION: a set of classes which define the connection information between two bones.
 *				Joint is the object that a controller will control directly.
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DJOINT_H_
#define _A3DJOINT_H_

#include "A3DObject.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////////
// Joint ID
///////////////////////////////////////////////////////////////////////////////

class A3DSkeleton;
class A3DBone;
class A3DController;

///////////////////////////////////////////////////////////////////////////////
//
// class A3DJoint is the base class of joint and it contains both the sliding
// information (position) and the rotational information (orientation).
//
// A3DJoint is a free joint, and A3DControlledJoint is a limited controlled joint.
// A3DAnimJoint is a joint that will acting according to a predefined trackdata.
//
///////////////////////////////////////////////////////////////////////////////
class A3DJoint : public A3DObject
{
public:

	//	Joint type
	enum
	{
		JOINT_UNKNOWN = -1,
		JOINT_ANIM = 0,
		JOINT_CONTROL,
		JOINT_SPRING,
		JOINT_AXISSPRING,
	};
	
	//	Struct used to save and load bone
	struct JOINTDATA
	{
		int		iParentBone;
		int		iChildBone;
		int		iSiblingJoint;
	};

protected:

	DWORD			m_dwFileNameID;	//	ID calculated from joint file name
	A3DSkeleton*	m_pSkeleton;	//	Skeleton this bone belongs to

	int		m_iJointType;			//	Joint type
	int		m_iParentBone;			// parent bone of the joint
	int		m_iChildBone;			// child bone of the joint
	int		m_iSiblingJoint;		// the joints controlled same two bones.

public:
	//////////////////////////////////////////////////////////////////////////////
	//
	// Inline get and set functions.
	//
	//////////////////////////////////////////////////////////////////////////////
	inline DWORD GetFileNameID() { return m_dwFileNameID; }
	inline int GetJointType() { return m_iJointType; }
	virtual void SetName(const char* szName);

	inline int GetParentBone() { return m_iParentBone; }
	inline int GetChildBone() { return m_iChildBone; }
	inline int GetSiblingJoint() { return m_iSiblingJoint; }
	A3DBone* GetParentBonePtr();
	A3DBone* GetChildBonePtr();
	A3DJoint* GetSiblingJointPtr();

	inline A3DSkeleton* GetSkeleton() { return m_pSkeleton; }
	inline void SetSkeleton(A3DSkeleton* pSkeleton) { m_pSkeleton = pSkeleton; }

	inline void SetBones(int iParentBone, int iChildBone) 
	{
		m_iParentBone	= iParentBone;
		m_iChildBone	= iChildBone;
	}
	
	inline void SetSiblingJoint(int iJoint) { m_iSiblingJoint = iJoint; }

public:
	//////////////////////////////////////////////////////////////////////////////
	//
	// Constructor and Destructor
	//
	//////////////////////////////////////////////////////////////////////////////
	A3DJoint();
	virtual ~A3DJoint();

	//	Initialize bone
	virtual bool Init(A3DSkeleton* pSkeleton);
	//	Release bone
	virtual void Release();
	//	Duplicate joint data
	virtual A3DJoint* Duplicate(A3DSkeleton* pDstSkeleton) { return NULL; }

	//////////////////////////////////////////////////////////////////////////////
	//
	// Update methods.
	//
	// bool Update(int nDeltaTime)
	//	update this joint a little time specified as nDeltaTime
	//
	//	nDeltaTime		IN			time delta to update
	//	return true if succeed, or else false.
	//
	// A3DMATRIX4 GetMatrix()
	//	Get this joint's equivalent transform matrix
	// 
	// return equivalent tm.
	//////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
	virtual A3DMATRIX4 GetMatrix() = 0;

	/////////////////////////////////////////////////////////////////////////////
	// Serialize methods.
	/////////////////////////////////////////////////////////////////////////////
	virtual bool Load(AFile* pFile);
	virtual bool Save(AFile* pFile);

	static A3DJoint* NewJoint(DWORD dwType, bool bClassID);
};

///////////////////////////////////////////////////////////////////////////////
//
// A3DAnimJoint is the joint that will get its data from a track not a controller
//
///////////////////////////////////////////////////////////////////////////////

class A3DAnimJoint : public A3DJoint
{
public:		//	Types

protected:

public:

public:
	//////////////////////////////////////////////////////////////////////////////
	//
	// Constructor and Destructor
	//
	//////////////////////////////////////////////////////////////////////////////
	A3DAnimJoint();
	virtual ~A3DAnimJoint();

	//	Duplicate joint data
	virtual A3DJoint* Duplicate(A3DSkeleton* pDstSkeleton);

	//////////////////////////////////////////////////////////////////////////////
	//
	// Update methods.
	//
	//////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
	virtual A3DMATRIX4 GetMatrix();

	/////////////////////////////////////////////////////////////////////////////
	// Serialize methods.
	/////////////////////////////////////////////////////////////////////////////
	virtual bool Load(AFile* pFile);
	virtual bool Save(AFile* pFile);
};

///////////////////////////////////////////////////////////////////////////////
//
// A3DCtrlJoint is the joint that will get its data from a controller not a track
//
///////////////////////////////////////////////////////////////////////////////
class A3DControlledJoint : public A3DJoint
{
protected:
	A3DController *		m_pController;		// controller which will control this joint fully
											// each joint can have only one controller on it
											// but this controller can be of any kind

	bool				m_bAbsoluteControl;	// flag indicates the pos and angle will
											// replace original bone state 

	FLOAT				m_vXPos;			// x coordinates.
	FLOAT				m_vYPos;			// y coordinates.
	FLOAT				m_vZPos;			// z coordinates.

	FLOAT				m_vXAngle;			// x angle.
	FLOAT				m_vYAngle;			// y angle.
	FLOAT				m_vZAngle;			// z angle.

	BOOL				m_bXActive;			// flag indicates whether moving/rotating with x axis is allowed
	BOOL				m_bXLimited;		// flag indicates whether moving/rotating with x axis is limited by min and max values
	FLOAT				m_vXMin;			// min value of x axis movement
	FLOAT				m_vXMax;			// max value of x axis movement
	FLOAT				m_vXPrefered;		// prefered value of x axis movement

	BOOL				m_bYActive;			// flag indicates whether moving/rotating with y axis is allowed
	BOOL				m_bYLimited;		// flag indicates whether moving/rotating with y axis is limited by min and max values
	FLOAT				m_vYMin;			// min value of y axis movement
	FLOAT				m_vYMax;			// max value of y axis movement
	FLOAT				m_vYPrefered;		// prefered value of y axis movement

	BOOL				m_bZActive;			// flag indicates whether moving/rotating with z axis is allowed
	BOOL				m_bZLimited;		// flag indicates whether moving/rotating with z axis is limited by min and max values
	FLOAT				m_vZMin;			// min value of z axis movement
	FLOAT				m_vZMax;			// max value of z axis movement
	FLOAT				m_vZPrefered;		// prefered value of z axis movement

public:
	inline FLOAT GetXPos()			{ return m_vXPos; }
	inline FLOAT GetYPos()			{ return m_vYPos; }
	inline FLOAT GetZPos()			{ return m_vZPos; }

	inline void SetXPos(FLOAT x)	{ m_vXPos = x; }
	inline void SetYPos(FLOAT y)	{ m_vYPos = y; }
	inline void SetZPos(FLOAT z)	{ m_vZPos = z; }

	inline FLOAT GetXAngle()		{ return m_vXAngle; }
	inline FLOAT GetYAngle()		{ return m_vYAngle; }
	inline FLOAT GetZAngle()		{ return m_vZAngle; }

	inline void SetXAngle(FLOAT x)	{ m_vXAngle = x; }
	inline void SetYAngle(FLOAT y)	{ m_vYAngle = y; }
	inline void SetZAngle(FLOAT z)	{ m_vZAngle = z; }

	inline FLOAT GetXMin()			{ return m_vXMin; }
	inline FLOAT GetXMax()			{ return m_vXMax; }
	inline FLOAT GetXPrefered()		{ return m_vXPrefered; }

	inline void SetXMin(FLOAT vMin)				{ m_vXMin = vMin; }
	inline void SetXMax(FLOAT vMax)				{ m_vXMax = vMax; }
	inline void SetXPrefered(FLOAT vPrefered)	{ m_vXPrefered = vPrefered; }

	inline FLOAT GetYMin()			{ return m_vYMin; }
	inline FLOAT GetYMax()			{ return m_vYMax; }
	inline FLOAT GetYPrefered()		{ return m_vYPrefered; }

	inline void SetYMin(FLOAT vMin)				{ m_vYMin = vMin; }
	inline void SetYMax(FLOAT vMax)				{ m_vYMax = vMax; }
	inline void SetYPrefered(FLOAT vPrefered)	{ m_vYPrefered = vPrefered; }

	inline FLOAT GetZMin()			{ return m_vZMin; }
	inline FLOAT GetZMax()			{ return m_vZMax; }
	inline FLOAT GetZPrefered()		{ return m_vZPrefered; }

	inline void SetZMin(FLOAT vMin)				{ m_vZMin = vMin; }
	inline void SetZMax(FLOAT vMax)				{ m_vZMax = vMax; }
	inline void SetZPrefered(FLOAT vPrefered)	{ m_vZPrefered = vPrefered; }

	inline BOOL IsXLimited()			{ return m_bXLimited; }
	inline BOOL IsYLimited()			{ return m_bYLimited; }
	inline BOOL IsZLimited()			{ return m_bZLimited; }

	inline void SetXLimited(BOOL bFlag)	{ m_bXLimited = bFlag; }
	inline void SetYLimited(BOOL bFlag)	{ m_bYLimited = bFlag; }
	inline void SetZLimited(BOOL bFlag)	{ m_bZLimited = bFlag; }

public:
	//////////////////////////////////////////////////////////////////////////////
	//
	// Constructor and Destructor
	//
	//////////////////////////////////////////////////////////////////////////////
	A3DControlledJoint();
	virtual ~A3DControlledJoint();

	//////////////////////////////////////////////////////////////////////////////
	//
	// Update methods.
	//
	//////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
	virtual A3DMATRIX4 GetMatrix();

	/////////////////////////////////////////////////////////////////////////////
	// Serialize methods.
	/////////////////////////////////////////////////////////////////////////////
	virtual bool Load(AFile* pFile);
	virtual bool Save(AFile* pFile);
};

///////////////////////////////////////////////////////////////////////////////
//
// Spring joint describe a joint between 2 bones connected via a single spring.
//
///////////////////////////////////////////////////////////////////////////////
class A3DSpringJoint : public A3DControlledJoint
{
protected:
	FLOAT			m_vSpringRest;		// spring rest value in measure of 
										// distance between the two bone's 
										// pivot pos or two bone's angle
	FLOAT			m_vSpringTension;	// spring tension value

public:
	///////////////////////////////////////////////////////////////////////////
	// 
	// Inline get and set functions.
	//
	///////////////////////////////////////////////////////////////////////////
	inline FLOAT GetSpringRest()			{ return m_vSpringRest; }
	inline FLOAT GetSpringTension()			{ return m_vSpringTension; }

	inline void SetSpringRest(FLOAT vRest)			{ m_vSpringRest = vRest; }
	inline void SetSpringTension(FLOAT vTension)	{ m_vSpringTension = vTension; }	

public:
	A3DSpringJoint();
	virtual ~A3DSpringJoint();

	//////////////////////////////////////////////////////////////////////////////
	//
	// Update methods.
	//
	//////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
	virtual A3DMATRIX4 GetMatrix();

	/////////////////////////////////////////////////////////////////////////////
	// Serialize methods.
	/////////////////////////////////////////////////////////////////////////////
	virtual bool Load(AFile* pFile);
	virtual bool Save(AFile* pFile);
};

///////////////////////////////////////////////////////////////////////////////
//
// Spring axis joint describe a joint between 2 bones connected via a set of 
// springs on each axis, it can be used in sliding joints and rotation joints.
//
///////////////////////////////////////////////////////////////////////////////
class A3DAxisSpringJoint : public A3DControlledJoint
{
protected:
	FLOAT			m_vXSpringRest;		// spring rest value of x axis
	FLOAT			m_vXSpringTension;	// spring tension of x axis
	FLOAT			m_vYSpringRest;		// spring rest value of y axis
	FLOAT			m_vYSpringTension;	// spring tension of y axis
	FLOAT			m_vZSpringRest;		// spring rest value of z axis
	FLOAT			m_vZSpringTension;	// spring tension of z axis

public:
	///////////////////////////////////////////////////////////////////////////
	// 
	// Inline get and set functions.
	//
	///////////////////////////////////////////////////////////////////////////
	inline FLOAT GetXSpringRest()				{ return m_vXSpringRest; }
	inline FLOAT GetXSpringTension()			{ return m_vXSpringTension; }
	inline FLOAT GetYSpringRest()				{ return m_vYSpringRest; }
	inline FLOAT GetYSpringTension()			{ return m_vYSpringTension; }
	inline FLOAT GetZSpringRest()				{ return m_vZSpringRest; }
	inline FLOAT GetZSpringTension()			{ return m_vZSpringTension; }

	inline void SetXSpringRest(FLOAT vRest)			{ m_vXSpringRest = vRest; }
	inline void SetXSpringTension(FLOAT vTension)	{ m_vXSpringTension = vTension; }	
	inline void SetYSpringRest(FLOAT vRest)			{ m_vYSpringRest = vRest; }
	inline void SetYSpringTension(FLOAT vTension)	{ m_vYSpringTension = vTension; }	
	inline void SetZSpringRest(FLOAT vRest)			{ m_vZSpringRest = vRest; }
	inline void SetZSpringTension(FLOAT vTension)	{ m_vZSpringTension = vTension; }	

public:
	A3DAxisSpringJoint();
	virtual ~A3DAxisSpringJoint();

	//////////////////////////////////////////////////////////////////////////////
	//
	// Update methods.
	//
	//////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
	virtual A3DMATRIX4 GetMatrix();

	/////////////////////////////////////////////////////////////////////////////
	// Serialize methods.
	/////////////////////////////////////////////////////////////////////////////
	virtual bool Load(AFile* pFile);
	virtual bool Save(AFile* pFile);
};

#endif//_A3DJOINT_H_