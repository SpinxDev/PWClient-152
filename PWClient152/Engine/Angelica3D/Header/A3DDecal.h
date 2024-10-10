/*
 * FILE: A3DDecal.h
 *
 * DESCRIPTION: Routines for decal
 *
 * CREATED BY: duyuxin, 2001/11/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DDECAL_H_
#define _A3DDECAL_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DObject.h"
#include "AString.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Types of decal
enum
{
	DECALTYPE_PURE3D	= 0,	//	All vertices need transforming
	DECALTYPE_PURE2D	= 1,	//	All vertices needn't transforming
	DECALTYPE_CENTER3D	= 2		//	Center point need transforming and others needn't
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class A3DStream;
class A3DTexture;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DDecal
//
///////////////////////////////////////////////////////////////////////////

class A3DDecal : public A3DObject
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DDecal();
	virtual ~A3DDecal();

public:		//	Attributes

public:		//	Operations

	bool		Init(int iType, A3DDevice* pDevice, char* szTexName, bool bForceRender=false);	//	Initialize object
	void		Release();		//	Release object

	void		SetPos(FLOAT x, FLOAT y, FLOAT z)		{	m_vCenter = A3DVECTOR3(x, y, z);			}
	void		SetSize(FLOAT fWidth, FLOAT fHeight)	{	m_fWidth = fWidth; m_fHeight = fHeight;		}
	void		SetColor(A3DCOLOR colDecal)				{	m_aColors[0] = m_aColors[1] = m_aColors[2] = m_aColors[3] = colDecal;	}
	void		SetColor(int iVert, A3DCOLOR colVert)	{	m_aColors[iVert] = colVert;		}
	void		UpdateParentTM(const A3DMATRIX4& matParentTM);

	inline void	SetTextureArea(float u, float v, float fWidth, float fHeight);	//	Set texture area used by this decal
	inline void Set3DDecalMinSize(float fMinWid, float fMinHei);	//	Set minimum size for DECALTYPE_PURE3D type decal 
	inline void Set3DDecalMaxSize(float fMaxWid, float fMaxHei);	//	Set maximum size for DECALTYPE_PURE3D type decal

	const char * GetTextureMap()			{	return m_strTextureMap;		}
	A3DTexture*	GetTexture()				{	return m_pTexture;			}
	void		Rotate(FLOAT fDegree)		{	m_fRotateDeg = fDegree;		}
	void		SetShader(A3DSHADER Shader)	{	m_Shader = Shader;			}
	A3DSHADER	GetShader()					{	return m_Shader;			}

	bool		ChangeTexture(char* szTexName);	//	Change decal's texture
	virtual bool Render(A3DViewport* pView, bool bCheckVis=true);	//	Render flares

protected:	//	Attributes

	AString		m_strTextureMap; // The texture's map file name;

	A3DDevice*	m_pA3DDevice;	//	A3D device object
	A3DStream*	m_pA3DStream;	//	A3D stream object, used when m_bOptimized = false
	A3DTexture*	m_pTexture;		//	Texture of decal
	A3DSHADER	m_Shader;		//	Shader

	bool		m_bOptimized;	//	Whether A3D engine use vertex colloector
	A3DTLVERTEX	m_aVerts[4];	//	Vertex buffer used when m_bOptimized = true
	WORD		m_aIndices[6];	//	Indices buffer used when m_bOptimized = true
	DWORD		m_hTexInfo;		//	Handle of texture infromation got from vertex collector

	A3DMATRIX4	m_matParentTM;	//  Parent's transform matrix;
	int			m_iType;		//	Type of decal
	bool		m_bVisible;		//	Visible flag
	A3DVECTOR3	m_vCenter;		//	Decal center's position.
	FLOAT		m_fWidth;		//	Half width
	FLOAT		m_fHeight;		//	Half height
	A3DCOLOR	m_aColors[4];	//	Color for each vertex
	FLOAT		m_fMinWidth;	//	Minimum half size of DECALTYPE_PURE3D type decal 
	FLOAT		m_fMinHeight;
	FLOAT		m_fMaxWidth;	//	Maximum half size of DECALTYPE_PURE3D type decal
	FLOAT		m_fMaxHeight;

	float		m_fTexU;		//	Texture U coordinates
	float		m_fTexV;		//	Texture V coordinates
	float		m_fTexWid;		//	Texture width (0.0f - 1.0f)
	float		m_fTexHei;		//	Texture height (0.0f - 1.0f)

	float		m_fRotateDeg;	//	Rotate degree

protected:	//	Operations

	bool		Update(A3DViewport* pView);		//	Update decal with specified viewport
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Set texture area used by this decal, by default, whole texture will be used.

	u, v: area's left corner in texture (0.0f -- 1.0f)
	fWidth, fHeight: area's size (0.0f -- 1.0f)
*/
void A3DDecal::SetTextureArea(float u, float v, float fWidth, float fHeight)
{
	m_fTexU		= u;
	m_fTexV		= v;
	m_fTexWid	= fWidth;
	m_fTexHei	= fHeight;
}

//	Set minimum size for DECALTYPE_PURE3D type decal 
void A3DDecal::Set3DDecalMinSize(float fMinWid, float fMinHei)
{
	m_fMinWidth		= fMinWid;
	m_fMinHeight	= fMinHei;
}

//	Set maximum size for DECALTYPE_PURE3D type decal
void A3DDecal::Set3DDecalMaxSize(float fMaxWid, float fMaxHei)
{
	m_fMaxWidth		= fMaxWid;
	m_fMaxHeight	= fMaxHei;
}


#endif	//	_A3DDECAL_H_


