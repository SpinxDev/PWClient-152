/*
 * FILE: A3DCoordinate.h
 *
 * DESCRIPTION: A3D coordinate class
 *
 * CREATED BY: duyuxin, 2003/10/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCOORDINATE_H_
#define _A3DCOORDINATE_H_

#include "A3DObject.h"
#include "A3DTypes.h"
#include "AList2.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DCoordinate
//
///////////////////////////////////////////////////////////////////////////

class A3DCoordinate : public A3DObject
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DCoordinate();
	virtual ~A3DCoordinate();

public:		//	Attributes

public:		//	Operations

	//	Set absolute transform matrix
	virtual void SetAbsoluteTM(const A3DMATRIX4& mat);
	//	Set relative transform matrix
	virtual void SetRelativeTM(const A3DMATRIX4& mat);

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set absolute forward and up direction
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Set absolute position and direction
	virtual void SetPosAndDir(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Move in absolute coordinates
	virtual void Move(const A3DVECTOR3& vOffset) { SetPos(GetPos() + vOffset); }

	//	Set relative position
	virtual void SetRelativePos(const A3DVECTOR3& vPos);
	//	Set relative forward and up direction
	virtual void SetRelativeDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Set relative position and direction
	virtual void SetRelativePosAndDir(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Move in relative coordinates
	virtual void RelativeMove(const A3DVECTOR3& vOffset) { SetRelativePos(m_matRelativeTM.GetRow(3) + vOffset); }

	//	Rotate routines
	virtual void RotateX(float fDeltaRad, bool bInLocal=true);
	virtual void RotateY(float fDeltaRad, bool bInLocal=true);
	virtual void RotateZ(float fDeltaRad, bool bInLocal=true);
	virtual void RotateAxis(const A3DVECTOR3& vAxis, float fDeltaRad, bool bInLocal=true);

	//	Get position
	A3DVECTOR3 GetPos() const { return m_matAbsoluteTM.GetRow(3); }
	//	Get forward direction
	A3DVECTOR3 GetDir() const { return m_matAbsoluteTM.GetRow(2); }
	//	Get up direction
	A3DVECTOR3 GetUp() const { return m_matAbsoluteTM.GetRow(1); }
	//	Get right dirction
	A3DVECTOR3 GetRight() const { return m_matAbsoluteTM.GetRow(0); } 

	//	Set parent coordinate
	void SetParentCood(A3DCoordinate* pParent) { m_pParentCoord = pParent; }
	//	Get parent coordinate
	A3DCoordinate* GetParentCoord() { return m_pParentCoord; }

	//	Add child coordinates
	bool AddChildCoord(A3DCoordinate* pChild);
	//	Remove child coordinate
	void RemoveChildCoord(A3DCoordinate* pChild);
	//	Remove all child coordinates
	void RemoveAllChildrenCoords();
	//	Get children coordinates number
	int GetChildrenCoordNum() { return m_ChildCoordList.GetCount(); }

	const A3DMATRIX4& GetAbsoluteTM() { return m_matAbsoluteTM; }
	const A3DMATRIX4& GetRelativeTM() { return m_matRelativeTM; }

protected:	//	Attributes

	A3DMATRIX4		m_matAbsoluteTM;	//	Absolute TM, from this coordinate to world
	A3DMATRIX4		m_matRelativeTM;	//	Relative TM, from this coordinate to parent
	A3DCoordinate*	m_pParentCoord;		//	Parent coordinate
	DWORD			m_dwPosHandle;		//	Position in parent, set by AddChildCoord()

	APtrList<A3DCoordinate*>	m_ChildCoordList;	//	Child coordinate list

protected:	//	Operations

	//	Update relative TM
	void UpdateRelativeTM();

	//	Below position and direction routines don't effect relative matrix
	//	Move position
	void Bound_Move(const A3DVECTOR3& vOffset);
	//	Rotate round specified axis
	void Bound_Rotate(const A3DVECTOR3& vAxis, float fRad);
	//	Set forward and up direction
	void Bound_SetAbsoluteTM(const A3DMATRIX4& mat);

	//	Move all children
	void Bound_MoveChildren(const A3DVECTOR3& vOffset);
	//	Set all children's forward and direction
	void Bound_SetChildrenAbsoluteTM();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DCOORDINATE_H_

