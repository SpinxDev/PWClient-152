/*
 * FILE: Staff.h
 *
 * DESCRIPTION: Staff class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#pragma once

#include "A3DTypes.h"

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

class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CStaff
//
///////////////////////////////////////////////////////////////////////////

class CStaff
{
public:		//	Types

public:		//	Constructor and Destructor

	CStaff();
	virtual ~CStaff() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize box
	bool Init(CRender* pRender, int iSegment, float fLength, float fRadius);
	//	Release box
	void Release();

	//	Render sector
	bool Render();

	//	Set/Get color
	void SetColor(DWORD dwCol) { m_dwColor = dwCol; }
	DWORD GetColor() { return m_dwColor; }
	//	Set / Get position
	void SetPos(const A3DVECTOR3& vPos) { m_vPos = vPos; }
	A3DVECTOR3 GetPos() { return m_vPos; }
	//	Set direction and up
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Get direction
	A3DVECTOR3 GetDir() { return m_vDir; }
	//	Get up
	A3DVECTOR3 GetUp() { return m_vUp; }
	//	Set / Get length
	void SetLength(float fLength) { m_fLength = fLength; }
	float GetLength() { return m_fLength; }
	//	Set / Get radius
	void SetRadius(float fRadius) { m_fRadius = fRadius; }
	float GetRadius() { return m_fRadius; }

	//	Set / Get transform matrix
	void SetTM(const A3DMATRIX4& mat) { m_matTM = mat; }
	const A3DMATRIX4& GetTM() { return m_matTM; }

protected:	//	Attributes

	CRender*	m_pRender;		//	Render object
	DWORD		m_dwColor;		//	Staff's color
	int			m_iNumSeg;		//	Number of segment
	A3DVECTOR3	m_vPos;			//	Staff's position
	A3DVECTOR3	m_vDir;			//	Direction
	A3DVECTOR3	m_vUp;			//	Up
	A3DVECTOR3	m_vRight;		//	Right
	float		m_fLength;		//	Staff's length
	float		m_fRadius;		//	Staff's radius
	A3DMATRIX4	m_matTM;		//	Transform matrix

	WORD*		m_aIndices;		//	Indices
	int			m_iNumIdx;		//	Number of index
	A3DVECTOR3*	m_aVerts;		//	Vertex buffer

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


