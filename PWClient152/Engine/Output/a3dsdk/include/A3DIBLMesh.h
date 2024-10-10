/*
 * FILE: A3DIBLMesh.h
 *
 * DESCRIPTION: Class representing a mesh using image based lighting
 *
 * CREATED BY: Hedi, 2002/11/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DIBLMESH_H_
#define _A3DIBLMESH_H_

#include "A3DVertex.h"
#include "A3DObject.h"
#include "AList.h"

class A3DTexture;
class A3DStream;
class A3DDevice;
class A3DIBLMesh;
class A3DViewport;

// interface IAnimObj
//		interface that represent a sub mesh of the A3DMesh
class IAnimObj
{
protected:
	A3DDevice		* m_pA3DDevice;

public:
	IAnimObj();
	~IAnimObj();

	virtual bool Render(A3DViewport * pCurrentViewport) = 0;
	virtual bool TickAnimation(float vDeltaTime/*in second*/) = 0;

	virtual bool Init(A3DDevice * pDevice) = 0;
	virtual bool Release() = 0;
};

// class A3DSubMesh
//		class represent a sub mesh
class A3DSubMesh : public IAnimObj
{
protected:
	int				m_nVertCount;
	int				m_nFaceCount;

public:
	A3DSubMesh();
	~A3DSubMesh();

	bool Init(A3DDevice * pDevice) { return true; }
	bool Release() { return true; }

public:
	virtual bool AddVerts(void* pVerticesBuffer, int nVertCount, A3DVERTEXTYPE vertexType) = 0;
	virtual bool AddFaces(void* pIndicesBuffer, int nFaceCount, A3DFORMAT indexFormat) = 0;

	virtual bool Render(A3DViewport * pCurrentViewport) { return true; }
	virtual bool TickAnimation(float vDeltaTime) { return true; }

public:
	inline int GetVertCount() { return m_nVertCount; }
	inline int GetFaceCount() { return m_nFaceCount; }
};

// class A3DVSubMesh
//		class represetn a sub mesh that using A3DVERTEX streams
class A3DVSubMesh : public A3DSubMesh
{
protected:
	A3DVERTEX			* m_pVertices;
	WORD				* m_pIndices;

public:
	virtual bool AddVerts(void* pVertices, int nVertCount, A3DVERTEXTYPE vertexType) = 0;
	virtual bool AddFaces(void* pIndices, int nFaceCount, A3DFORMAT indexFormat) = 0;

	virtual bool Render(A3DViewport * pCurrentViewport) { return true; }
	virtual bool TickAnimation(float vDeltaTime) { return true; }
public:
	A3DVSubMesh();
	~A3DVSubMesh();

};

// class A3DLSubMesh
//		class represent a pre-lighted mesh
class A3DLSubMesh : public A3DSubMesh
{
protected:
	A3DLVERTEX			* m_pLVertices;
	WORD				* m_pLIndices;

public:
	virtual bool AddVerts(void* pVertices, int nVertCount, A3DVERTEXTYPE vertexType);
	virtual bool AddFaces(void* pIndices, int nFaceCount, A3DFORMAT indexFormat);

	virtual bool Init(A3DDevice * pDevice);
	virtual bool Release();

	virtual bool Render(A3DViewport * pCurrentViewport);
	virtual bool TickAnimation(float vDeltaTime);
public:
	A3DLSubMesh();
	~A3DLSubMesh();
};

// class A3DTLSubMesh
//		class represent a pre-transformed-lighted mesh
class A3DTLSubMesh : public A3DSubMesh
{
protected:
	A3DTLVERTEX			* m_pTLVertices;
	WORD				* m_pTLIndices;

public:
	virtual bool AddVerts(void* pVertices, int nVertCount, A3DVERTEXTYPE vertexType) = 0;
	virtual bool AddFaces(void* pIndices, int nFaceCount, A3DFORMAT indexFormat) = 0;

	virtual bool Render(A3DViewport * pCurrentViewport) { return true; }
	virtual bool TickAnimation(float vDeltaTime) { return true; }

public:
	A3DTLSubMesh();
	~A3DTLSubMesh();
};
  
// class A3DIBLSubMesh
//		class represent a image based sub mesh
class A3DIBLSubMesh : public A3DSubMesh
{
protected:
	bool			m_bManTexture; // true when m_pLightMap is loaded through A3DTextureMan
	char			m_szLightMapPath[MAX_PATH];
	A3DTexture		* m_pLightMap;
	A3DIBLVERTEX	* m_pIBLVertices;
	WORD			* m_pIBLIndices;

public:				  
	// We may use multi-pass to show the image based lighting;
	// Light pass we should render the light maps onto the screen
	virtual bool LightPass(A3DViewport * pCurrentViewport);
	// In Extra pass we should render some extra lighting object onto the screen to accumulate the lights
	virtual bool ExtraPass(A3DViewport * pCurrentViewport);
	// This is the render pass that draw base textured mesh onto the screen
	virtual bool BasePass(A3DViewport * pCurrentViewport);

	// We may use just one-pass to show the image based lighting;
	virtual bool OnePass(A3DViewport * pCurrentViewport);

	// We may only render the vertex colored mesh for maximum speed;
	virtual bool VertexColorPass(A3DViewport * pCurrentViewport);

	virtual bool Render(A3DViewport * pCurrentViewport) { return true; }
	virtual bool TickAnimation(float vDeltaTime) { return true; }

public:
	virtual bool AddVerts(void* pVertices, int nVertCount, A3DVERTEXTYPE vertexType);
	virtual bool AddFaces(void* pIndices, int nFaceCount, A3DFORMAT indexFormat);

public:
	A3DIBLSubMesh();
	~A3DIBLSubMesh();

	bool Init(A3DDevice * pDevice);
	bool Release();

	inline void SetLightMap(char * szPath) { strncpy(m_szLightMapPath, szPath, MAX_PATH); }
	bool LoadLightMap();

	bool SetLightMap(int nLMWidth, int nLMHeight, LPBYTE pLMImage);
};

class A3DIBLTriSubMesh : public A3DIBLSubMesh
{
public:
	A3DIBLTriSubMesh();
	~A3DIBLTriSubMesh();

	bool AddIBLFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, int n0, int n1, int n2);
	bool AddRectFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, const A3DIBLVERTEX& v3, int n0, int n1, int n2, int n3, int n4, int n5);
};

class A3DIBLBBSubMesh : public A3DIBLSubMesh
{
public:
	A3DIBLBBSubMesh();
	~A3DIBLBBSubMesh();

	bool AddIBLFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, int n0, int n1, int n2);
};	

class A3DIBLPlaneSubMesh : public A3DIBLSubMesh
{
public:
	A3DIBLPlaneSubMesh();
	~A3DIBLPlaneSubMesh();

	bool AddIBLFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, int n0, int n1, int n2);
};

enum A3DIBLLIGHTRANGE
{
	A3DIBLLR_1X = 0,
	A3DIBLLR_2X,
	A3DIBLLR_4X
};

class A3DIBLMesh : public A3DObject
{
private:
	A3DDevice			* m_pA3DDevice;
	A3DTexture			* m_pBaseTexture;
	AList				m_ListIBLSubMesh;
	AList				m_ListLSubMesh;		// we must put LSubMesh and IBLSubMesh together for efficience, this can be done by abstract A3DIBLSubMesh with mehtod same as A3DLSubMesh

	A3DIBLLIGHTRANGE	m_IBLLightRange;
	bool				m_bUseLightMaps;	// Flag indicate whether we should use Lightmap shading, or just vertex color shading

protected:
	// We may use multi-pass to show the image based lighting and extra lighting;
	// And we may use one-pass to show the image based lighting result only;
	bool RenderByOnePass(A3DViewport * pCurrentViewport);
	bool RenderByMultiPass(A3DViewport * pCurrentViewort);

	// We may turn off lightmap shading, and using only vertex color shading for 
	// maximum speed goal
	bool RenderByVertexColorOnly(A3DViewport * pCurrentViewport);
		
public:
	A3DIBLMesh();
	~A3DIBLMesh();

	bool Init(A3DDevice * pDevice, char * pszBaseTexture, A3DIBLLIGHTRANGE iblLightRange, bool bUseLightMaps=true);
	bool Release();

	bool Render(A3DViewport * pCurrentViewport);

	// Sub mesh manage section;
	bool AddIBLTriSubMesh(A3DIBLTriSubMesh ** ppNewIBLTriSubMesh);
	bool AddIBLBBSubMesh(A3DIBLBBSubMesh ** ppNewIBLBBSubMesh);	
	bool AddIBLPlaneSubMesh(A3DIBLPlaneSubMesh ** ppNewIBLPlaneSubMesh);
	bool AddLSubMesh(A3DLSubMesh ** ppNewLSubMesh);

	bool LoadLightMaps();

	inline bool GetUseLightMaps() { return m_bUseLightMaps; }
};

#endif//_A3DIBLMESH_H_
