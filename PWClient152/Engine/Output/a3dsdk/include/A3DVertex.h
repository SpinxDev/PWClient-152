/*
 * FILE: A3DVertex.h
 *
 * DESCRIPTION: Structures representing the Vertex, LVerex or TLVertex
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DVERTEX_H_
#define _A3DVERTEX_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define A3DFVF_A3DVERTEX	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define A3DFVF_A3DLVERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define A3DFVF_A3DTLVERTEX  (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define A3DFVF_A3DTLVERTEX2	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)
#define A3DFVF_A3DIBLVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)
#define A3DFVF_A3DBVERTEX1	(D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define A3DFVF_A3DBVERTEX2	(D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define A3DFVF_A3DBVERTEX3	(D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define A3DFVF_A3DMCVERTEX	(D3DFVF_XYZ | D3DFVF_NORMAL)
#define A3DFVF_A3DTLWARPVERT	(D3DFVF_XYZRHW | D3DFVF_TEX3)
#define A3DFVF_A3DWARPVERT	(D3DFVF_XYZ | D3DFVF_TEX3)

enum A3DVERTEXTYPE
{
	A3DVT_UNKNOWN = -1,
	A3DVT_VERTEX = 0,
	A3DVT_LVERTEX,
	A3DVT_TLVERTEX,
	A3DVT_IBLVERTEX,
	A3DVT_BVERTEX1,		//	Blending vertex with 1 weight
	A3DVT_BVERTEX2,		//	Blending vertex with 2 weight
	A3DVT_BVERTEX3,		//	Blending vertex with 3 weight
	A3DVT_BVERTEX3TAN,	//	Blending vertex with 3 weight and Tangent
	A3DVT_VERTEXTAN,	//	Tangent
	A3DVT_MCVERTEX,		//	Morph channel vertex
	A3DVT_TLVERTEX2,	//	TL vertex with 2 texture coord.
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct A3DVERTEX
{
    FLOAT x, y, z;
	FLOAT nx, ny, nz;
	FLOAT tu, tv;

public:

	A3DVERTEX() {}
	A3DVERTEX(const A3DVECTOR3& vPos, const A3DVECTOR3& vNormal, FLOAT _tu, FLOAT _tv)
	{
		x = vPos.x;		y = vPos.y;		z = vPos.z;
		nx = vNormal.x;	ny = vNormal.y;	nz = vNormal.z;
		tu = _tu;		tv = _tv;
	}
	
	void Set(const A3DVECTOR3& vPos, const A3DVECTOR3& vNormal, FLOAT _tu, FLOAT _tv)
	{
		x = vPos.x;		y = vPos.y;		z = vPos.z;
		nx = vNormal.x;	ny = vNormal.y;	nz = vNormal.z;
		tu = _tu;		tv = _tv;
	}
};

struct A3DLVERTEX
{
	FLOAT x, y, z;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu, tv;

public:

	A3DLVERTEX() {}
	A3DLVERTEX(const A3DVECTOR3& _vecPos, A3DCOLOR _diffuse, A3DCOLOR _specular, FLOAT _tu, FLOAT _tv) 
	{ 
		x = _vecPos.x; y = _vecPos.y; z = _vecPos.z; diffuse = _diffuse; specular = _specular; tu = _tu; tv = _tv;
	}

	void Set(const A3DVECTOR3& _vecPos, A3DCOLOR _diffuse, A3DCOLOR _specular, FLOAT _tu, FLOAT _tv) 
	{ 
		x = _vecPos.x; y = _vecPos.y; z = _vecPos.z; diffuse = _diffuse; specular = _specular; tu = _tu; tv = _tv;
	}
};

struct A3DTLVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu, tv;

public:

	A3DTLVERTEX() {}
	A3DTLVERTEX(const A3DVECTOR4& vPos, DWORD _diffuse, DWORD _specular, FLOAT _tu, FLOAT _tv)
	{
		x = vPos.x;	y = vPos.y;	z = vPos.z;	rhw = vPos.w; diffuse = _diffuse; specular = _specular; tu = _tu; tv = _tv;
	}

	void Set(const A3DVECTOR4& vPos, DWORD _diffuse, DWORD _specular, FLOAT _tu, FLOAT _tv)
	{
		x = vPos.x;	y = vPos.y;	z = vPos.z;	rhw = vPos.w; diffuse = _diffuse; specular = _specular;	tu = _tu; tv = _tv;
	}
};

struct A3DTLVERTEX2
{
	FLOAT x, y, z, rhw;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu1, tv1;
	FLOAT tu2, tv2;

public:

	A3DTLVERTEX2() {}
	A3DTLVERTEX2(const A3DVECTOR4& vPos, DWORD _diffuse, DWORD _specular, FLOAT _tu1, FLOAT _tv1, FLOAT _tu2, FLOAT _tv2)
	{
		x = vPos.x;	y = vPos.y;	z = vPos.z;	rhw = vPos.w; diffuse = _diffuse; specular = _specular; tu1 = _tu1; tv1 = _tv1;	tu2 = _tu2; tv2 = _tv2;
	}
};

struct A3DIBLVERTEX
{
	FLOAT x, y, z;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu1, tv1;
	FLOAT tu2, tv2;

public:

	A3DIBLVERTEX() {}
	A3DIBLVERTEX(const A3DVECTOR3& _vecPos, const A3DCOLOR& _diffuse, const A3DCOLOR& _specular, FLOAT _tu1, FLOAT _tv1, FLOAT _tu2, FLOAT _tv2)
	{
		x = _vecPos.x; y = _vecPos.y; z = _vecPos.z; diffuse = _diffuse; specular = _specular;
		tu1 = _tu1; tv1 = _tv1;
		tu2 = _tu2; tv2 = _tv2;
	}

	void Set(const A3DVECTOR3& _vecPos, const A3DCOLOR& _diffuse, const A3DCOLOR& _specular, FLOAT _tu1, FLOAT _tv1, FLOAT _tu2, FLOAT _tv2)
	{
		x = _vecPos.x; y = _vecPos.y; z = _vecPos.z; diffuse = _diffuse; specular = _specular;
		tu1 = _tu1; tv1 = _tv1;
		tu2 = _tu2; tv2 = _tv2;
	}
};

struct A3DBVERTEX1
{
	FLOAT vPos[3];			//	Position
	FLOAT aWeights[1];
	DWORD dwMatIndices;		//	Matrix indices
	FLOAT vNormal[3];
	FLOAT tu, tv;			//	Texture coordinates
};

struct A3DBVERTEX2
{
	FLOAT vPos[3];			//	Position
	FLOAT aWeights[2];
	DWORD dwMatIndices;		//	Matrix indices
	FLOAT vNormal[3];
	FLOAT tu, tv;			//	Texture coordinates
};

struct A3DBVERTEX3
{
	FLOAT vPos[3];			//	Position
	FLOAT aWeights[3];
	DWORD dwMatIndices;		//	Matrix indices
	FLOAT vNormal[3];
	FLOAT tu, tv;			//	Texture coordinates
};

struct A3DBVERTEX3TAN : public A3DBVERTEX3
{
	A3DVECTOR4 tangents;	//	tangent axis
};

//	Morph channel vertex
struct A3DMCVERTEX
{
	FLOAT vPos[3];			//	Position delta
	FLOAT vNormal[3];		//	Normal delta
};


// Vertex for space warp
struct A3DTLWARPVERTEX
{
	A3DTLWARPVERTEX();
	A3DTLWARPVERTEX(const A3DVECTOR4& _pos, float _u1, float _v1, float _u2, float _v2, float _u3, float _v3)
	{
		pos		= _pos;
		u1		= _u1;
		v1		= _v1;
		u2		= _u2;
		v2		= _v2;
		u3		= _u3;
		v3		= _v3;
	}

	A3DVECTOR4	pos;			//	Position on screen
	float		u1, v1;			//	u, v coord of the normal map
	float		u2, v2;			//	u, v coord of render target map
	float		u3,	v3;			//	bump offset scale along u, v coordinate
};

// Vertex for space warp
struct A3DWARPVERTEX
{
	A3DWARPVERTEX();
	A3DWARPVERTEX(const A3DVECTOR3& _pos, float _u1, float _v1, float _u2, float _v2, float _u3, float _v3)
	{
		pos		= _pos;
		u1		= _u1;
		v1		= _v1;
		u2		= _u2;
		v2		= _v2;
		u3		= _u3;
		v3		= _v3;
	}

	A3DVECTOR3	pos;			//	Position on screen
	float		u1, v1;			//	u, v coord of the normal map
	float		u2, v2;			//	u, v coord of render target map, this will be projected texture coords.
	float		u3,	v3;			//	bump offset scale along u, v coordinate
};

// light map coord
struct A3DLIGHTMAPCOORD
{
	A3DLIGHTMAPCOORD(){}
	float u, v;
};
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


#endif
