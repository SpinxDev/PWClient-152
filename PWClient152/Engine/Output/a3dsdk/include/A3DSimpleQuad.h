/*
 * FILE: A3DSimpleQuad.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: liyi 2012, 3, 15
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSIMPLEQUAD_H_
#define _A3DSIMPLEQUAD_H_

//#include "A3DColorValue.h"
#include "A3DVector.h"

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

class A3DStream;
class A3DVertexDecl;
class A3DEngine;
class A3DDevice;
class A3DHLSL;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSimpleQuad
//	
///////////////////////////////////////////////////////////////////////////

class A3DSimpleQuad
{
public:		//	Types

	//	Maximum of data bound with vertex
	enum
	{
		MAX_NUM_VDATA = 2,
	};

	//	Index of vertex
	enum
	{
		VERT_LT = 0,	//	Left-Top
		VERT_RT,		//	Right-Top
		VERT_LB,		//	Left-Bottom
		VERT_RB,		//	Right-Bottom
	};

	//	Direction light vertex
	struct QUAD_VERTEX
	{
		A3DVECTOR4	vPos;		//	Position in screen space
		float		uv[2];
		A3DVECTOR4	data[MAX_NUM_VDATA];	//	Data
	};

public:		//	Constructor and Destructor

	A3DSimpleQuad();
	virtual ~A3DSimpleQuad();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Draw quad with specified hlsl
	//	bApplyStream: true apply stream
	bool Draw(A3DHLSL* pShader);
	
	bool RawDraw();
	//	Draw with specified color, this can be used to clear render target
	bool FillColor(const A3DVECTOR4& col);

	//	Apply stream and vertex declaration to device
	bool ApplyStream();

	//	Set data that is bound with vertex
	//	vi: vertex index [0, 3]
	//	di: data index [0, MAX_NUM_VDATA-1]
	//	data: data
	void SetVertexData(int vi, int di, const A3DVECTOR4& data);
	const A3DVECTOR4& GetVertexData(int vi, int di);
	//	Set vertex z value
	void SetVertexZ(int vi, float z);
	//	Apply vertex data changes that caused by SetVertexXXX() functions to stream
	void ApplyVertexData();

	//	Set render target size
	void SetRTSize(int iWid, int iHei);
	//	Get stream
	A3DStream* GetStream() { return m_pStream; }
	//	Get vertex declaration
	A3DVertexDecl* GetVertexDecl() { return m_pvd; }

protected:	//	Attributes

	A3DEngine*			m_pA3DEngine;		//	Engine object
	A3DDevice*			m_pA3DDevice;		//	Device object
	A3DStream*			m_pStream;			//	Quad stream
	A3DVertexDecl*		m_pvd;				//	Vertex declaration

	QUAD_VERTEX			m_aQuadVerts[4];	//	Vertex

	A3DHLSL*			m_pShaderFillColor;
	
protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSIMPLEQUAD_H_
