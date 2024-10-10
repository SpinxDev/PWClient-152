/*
 * FILE: A3DCoordinate.cpp
 *
 * DESCRIPTION: A3D coordinate class
 *
 * CREATED BY: duyuxin, 2003/10/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DCoordinate.h"
#include "A3DPI.h"
#include "A3DFuncs.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DCoordinate
//
///////////////////////////////////////////////////////////////////////////

A3DCoordinate::A3DCoordinate() : 
m_matAbsoluteTM(A3DMATRIX4::IDENTITY),
m_matRelativeTM(A3DMATRIX4::IDENTITY)
{
	m_dwClassID		= A3D_CID_COORDINATE;
	m_pParentCoord	= NULL;
	m_dwPosHandle	= 0;
}

A3DCoordinate::~A3DCoordinate()
{
	//	Unlink from parent
	if (m_pParentCoord && m_dwPosHandle)
		m_pParentCoord->RemoveChildCoord(this);

	//	Unlink all children
	A3DCoordinate::RemoveAllChildrenCoords();
}

/*	Set position

	vPos: position in world coordinates
*/
void A3DCoordinate::SetPos(const A3DVECTOR3& vPos)
{
	A3DVECTOR3 vOffset = vPos - GetPos();
	
	m_matAbsoluteTM._41 = vPos.x;
	m_matAbsoluteTM._42 = vPos.y;
	m_matAbsoluteTM._43 = vPos.z;

	//	Update relative TM
	UpdateRelativeTM();

	//	Handle all children
	Bound_MoveChildren(vOffset);
}

/*	Set forward and up direction

	vDir: real forward direction in world coordinates, needn't be normalized
	vUp: real up direction in world coordinates, needn't be normalized
*/
void A3DCoordinate::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	SetAbsoluteTM(a3d_TransformMatrix(vDir, vUp, GetPos()));
}

//	Set position and direction
void A3DCoordinate::SetPosAndDir(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	SetAbsoluteTM(a3d_TransformMatrix(vDir, vUp, vPos));
}

//	Set relative position
void A3DCoordinate::SetRelativePos(const A3DVECTOR3& vPos)
{
	m_matRelativeTM._41 = vPos.x;
	m_matRelativeTM._42 = vPos.y;
	m_matRelativeTM._43 = vPos.z;

	SetRelativeTM(m_matRelativeTM);
}

//	Set relative forward and up direction
void A3DCoordinate::SetRelativeDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	A3DVECTOR3 vPos = m_matRelativeTM.GetRow(3);
	SetRelativeTM(a3d_TransformMatrix(vDir, vUp, vPos));
}

//	Set relative position and direction
void A3DCoordinate::SetRelativePosAndDir(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	SetRelativeTM(a3d_TransformMatrix(vDir, vUp, vPos));
}

//	Set absolute transform matrix
void A3DCoordinate::SetAbsoluteTM(const A3DMATRIX4& mat)
{
	m_matAbsoluteTM = mat;

	//	Update relative TM
	UpdateRelativeTM();

	//	Handle all children
	Bound_SetChildrenAbsoluteTM();
}

//	Set relative transform matrix
void A3DCoordinate::SetRelativeTM(const A3DMATRIX4& mat)
{
	m_matRelativeTM = mat;

	if (!m_pParentCoord)
		m_matAbsoluteTM = m_matRelativeTM;
	else
		m_matAbsoluteTM = m_matRelativeTM * m_pParentCoord->GetAbsoluteTM();

	//	Handle all children
	Bound_SetChildrenAbsoluteTM();
}

//	Update relative TM
void A3DCoordinate::UpdateRelativeTM()
{
	if (!m_pParentCoord)
	{
		m_matRelativeTM = m_matAbsoluteTM;
		return;
	}
	
	m_matRelativeTM = m_matAbsoluteTM * a3d_InverseTM(m_pParentCoord->GetAbsoluteTM());
}

void A3DCoordinate::RotateX(float fDeltaRad, bool bInLocal/* true */)
{
	A3DMATRIX4 matTurn = a3d_RotateX(fDeltaRad);

	if (bInLocal)
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	SetAbsoluteTM(m_matAbsoluteTM);
}

void A3DCoordinate::RotateY(float fDeltaRad, bool bInLocal/* true */)
{
	A3DMATRIX4 matTurn = a3d_RotateY(fDeltaRad);

	if (bInLocal)
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	SetAbsoluteTM(m_matAbsoluteTM);
}

void A3DCoordinate::RotateZ(float fDeltaRad, bool bInLocal/* true */)
{
	A3DMATRIX4 matTurn = a3d_RotateZ(fDeltaRad);

	if (bInLocal)
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	SetAbsoluteTM(m_matAbsoluteTM);
}

void A3DCoordinate::RotateAxis(const A3DVECTOR3& vAxis, float fDeltaRad, bool bInLocal/* true */)
{
	A3DMATRIX4 matTurn = a3d_RotateAxis(vAxis, fDeltaRad);
	
	if (bInLocal)
		m_matAbsoluteTM = matTurn * m_matAbsoluteTM;
	else
		m_matAbsoluteTM = m_matAbsoluteTM * matTurn;

	SetAbsoluteTM(m_matAbsoluteTM);
}

//	Move all children
void A3DCoordinate::Bound_MoveChildren(const A3DVECTOR3& vOffset)
{
	ALISTPOSITION pos = m_ChildCoordList.GetHeadPosition();
	while (pos)
	{
		A3DCoordinate* pChild = m_ChildCoordList.GetNext(pos);
		pChild->Bound_Move(vOffset);
	}
}

//	Set all children's forward and direction
void A3DCoordinate::Bound_SetChildrenAbsoluteTM()
{
	A3DMATRIX4 mat;

	ALISTPOSITION pos = m_ChildCoordList.GetHeadPosition();
	while (pos)
	{
		A3DCoordinate* pChild = m_ChildCoordList.GetNext(pos);
		mat = pChild->GetRelativeTM() * m_matAbsoluteTM;
		pChild->Bound_SetAbsoluteTM(mat);
	}
}

//	Move position
void A3DCoordinate::Bound_Move(const A3DVECTOR3& vOffset)
{
	A3DVECTOR3 vPos = GetPos() + vOffset;

	m_matAbsoluteTM._41 = vPos.x;
	m_matAbsoluteTM._42 = vPos.y;
	m_matAbsoluteTM._43 = vPos.z;

	//	Handle all children
	Bound_MoveChildren(vOffset);
}

//	Set forward and up direction
void A3DCoordinate::Bound_SetAbsoluteTM(const A3DMATRIX4& mat)
{
	m_matAbsoluteTM = mat;

	//	Handle all children
	Bound_SetChildrenAbsoluteTM();
}

//	Add child coordinate
bool A3DCoordinate::AddChildCoord(A3DCoordinate* pChild)
{
	if (!pChild)
		return false;

	ALISTPOSITION pos = m_ChildCoordList.AddTail(pChild);
	pChild->m_dwPosHandle  = (DWORD)pos;
	pChild->m_pParentCoord = this;
	return true;
}

//	Remove child coordinate
void A3DCoordinate::RemoveChildCoord(A3DCoordinate* pChild)
{
	if (!pChild || !pChild->m_dwPosHandle)
		return;

	ASSERT(pChild->m_pParentCoord == this);

	m_ChildCoordList.RemoveAt((ALISTPOSITION)pChild->m_dwPosHandle);
	pChild->m_dwPosHandle  = 0;
	pChild->m_pParentCoord = NULL;
}

//	Remove all child coordinates
void A3DCoordinate::RemoveAllChildrenCoords()
{
	ALISTPOSITION pos = m_ChildCoordList.GetHeadPosition();
	while (pos)
	{
		A3DCoordinate* pChild = m_ChildCoordList.GetNext(pos);

		pChild->m_pParentCoord	= NULL;
		pChild->m_dwPosHandle	= 0;
	}

	m_ChildCoordList.RemoveAll();
}


