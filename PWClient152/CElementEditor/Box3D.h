/*
 * FILE: Box3D.h
 *
 * DESCRIPTION: 3D box class
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
//	Class CBox3D
//
///////////////////////////////////////////////////////////////////////////

class CBox3D
{
public:		//	Types

public:		//	Constructor and Destructor

	CBox3D();
	virtual ~CBox3D() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize box
	bool Init(CRender* pRender, float fSize, bool bFlat=true);
	//	Release box
	void Release();

	//	Render sector
	bool Render();

	//	Set / Get color
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

	//	Set / Get Size
	void SetSize(float fSize) { m_aSizes[0] = m_aSizes[1] = m_aSizes[2] = fSize; }
	void SetSize(float x, float y, float z) { m_aSizes[0] = x; m_aSizes[1] = y; m_aSizes[2] = z; }
	float GetSize(int iAxis) { return m_aSizes[iAxis]; }

	//	Set / Get flat flag
	void SetFlatFlag(bool bFlat) { m_bFlat = bFlat; }
	bool GetFlatFlag() { return m_bFlat; }

	//	Set / Get transform matrix
	void SetTM(const A3DMATRIX4& mat) { m_matTM = mat; }
	const A3DMATRIX4& GetTM() { return m_matTM; }

protected:	//	Attributes

	CRender*	m_pRender;		//	Render object
	DWORD		m_dwColor;		//	Box's color
	A3DVECTOR3	m_vPos;			//	Box's position
	A3DVECTOR3	m_vDir;			//	Direction
	A3DVECTOR3	m_vUp;			//	Up
	A3DVECTOR3	m_vRight;		//	Right
	float		m_aSizes[3];	//	Box's size
	bool		m_bFlat;		//	true, flat box; false, wire box
	A3DMATRIX4	m_matTM;		//	Transform matrix

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


