/*
 * FILE: A3DIBLScene.h
 *
 * DESCRIPTION: Class representing the whole image-based-lighting scene in A3D Engine
 *
 * CREATED BY: Hedi, 2002/12/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DIBLSCENE_H_
#define _A3DIBLSCENE_H_

#include "A3DVertex.h"
#include "A3DScene.h"
#include "A3DIBLMesh.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define A3DSCENE_RENDER_SOLID		1
#define A3DSCENE_RENDER_ALPHA		2

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DLamp;
class A3DIBLLightGrid;
class A3DDevice;
class A3DTexture;
class A3DStream;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DPortal
//
///////////////////////////////////////////////////////////////////////////

class A3DIBLScene : public A3DScene
{
	friend int IBLFaceSortCompare(const void *arg1, const void *arg2);

private:

	struct TEXTURE_RECORD
	{
		int				nTexID;						// index in texture record buffer;
		char			szTextureName[MAX_PATH];	// Name of the texture;
		char			szLightMapName[MAX_PATH];   //Name of the detail texture;
		A3DMESHPROP		MeshProps;					// The mesh's property; not really used by IBL scene
	
		int				nFaceNum;					// Total face using this texture;
		int				nFaceVisible;				// Current Visible Face Count;
	
		DWORD			dwRenderFlag;				// Flag to say this texture is to be rendered or not;
		A3DTexture*		pTexture;					// Pointer of this texture;
		A3DTexture*		pLightMap;					// Pointer of this texture's lightmap texture;
		A3DIBLVERTEX*	pVerts;						// Pointer of vertex buffer;
	};
	
	struct FACE_RECORD
	{
		WORD	nTexID;			// The texture id of this face;
		int		nRefTicks;		// The ticks last time referenced this face (in Engine tick unit);
	};
	
	// We should write an abstract class to be the base class of all A3DScene classes
	// and define the common interface of each kind of scene class
	// but now we can not do that because the Great Qin Warrior has used A3DScene very 
	// directly, we must fix its usage in QW before we move A3DScene into an abstract class.
	struct OPTION
	{
		int		nVertexType;
	
		union
		{
			char	foo[64];
		};
	};
	
	struct SORTEDFACE
	{
		float			vDisToCam;
		TEXTURE_RECORD	*pTexRecord;
		WORD			wIndexInTexVisible;
	};

protected:

	A3DEngine*			m_pA3DEngine;
	A3DDevice*			m_pA3DDevice;
	A3DIBLLIGHTRANGE	m_IBLLightRange;
	char				m_szFileName[MAX_PATH];
	
	OPTION				m_Option;

	int					m_nNumFaces;		// Total faces of this scene;
	A3DIBLVERTEX*		m_pAllFaces;		// Faces buffer that consist of seperate vertex;
	FACE_RECORD*		m_pFaceRecords;		// Buffer stores each face's texture id and reference info;
	SORTEDFACE*			m_pSortedFaces;		// Buffer used for sorts;

	int					m_nNumVisibleFaces;	// Number of Visible Faces;

	int					m_nNumTextures;		// Texture number;
	TEXTURE_RECORD*		m_pTextureRecords;	// Texture list;

	A3DStream**			m_aStreams;			// streams;
	bool				m_bUseStream;		//	Use stream flag

	A3DAABB				m_SceneAABB;

	static A3DIBLLightGrid*		m_pGlobalLightGrid;		// a global pointer that dynamic light will use
	A3DIBLLightGrid*			m_pIBLLightGrid;		// light grid pointer;

protected:
	TEXTURE_RECORD* FindTextureRecord(char* szTextureName, char* szLightMapName, const A3DMESHPROP& MeshProps);
	bool RenderLamps(A3DViewport * pCurrentViewport);

public:
	A3DIBLScene();
	virtual ~A3DIBLScene();

	virtual bool Init(A3DEngine* pA3DEngine, bool bUseStream=true);
	virtual bool Release();

	// Serialize methods;
	virtual bool Load(A3DEngine* pA3DEngine, const char* szFile, bool bUseStream=true);
	virtual bool Load(A3DEngine* pA3DEngine, AFile* pFileToLoad, bool bUseStream=true);
	virtual bool Save(AFile* pFileToSave);

	bool AddTriFace(char* szTextureName, char* szLightMapName, A3DIBLVERTEX* v0, A3DIBLVERTEX* v1, A3DIBLVERTEX* v2, const A3DMESHPROP& MeshProps);
	bool LoadAllMaps();
	bool PrepareFirstRender(); // Do something before first render 

	virtual	bool UpdateVisibleSets(A3DViewport * pCurrentViewport);

	bool SetDeviceState();
	bool RestoreDeviceState();

	// Render current scene with the specified flag control;
	// See A3DScene.h for predefined flag
	virtual bool Render(A3DViewport * pCurrentViewport, DWORD dwFlag);
	virtual bool RenderSort(A3DViewport * pCurrentViewport, DWORD dwFlag, bool bNear2Far=false);

	// Import/Export methods;
	// We use import to make it compatible with A3DScene initialization methods!
	virtual bool Import(char * szMoxFileName);
	virtual bool Export(char * szMoxFileName) { return true; };

	virtual int GetFaceCount() { return m_nNumFaces; }
	A3DVERTEX * GetFaceBuffer() { return NULL; }
	virtual bool GetAllFaceVerts(A3DVECTOR3 * pVertsBuffer, int * pnBufferSize);
	virtual int GetTextureNum() { return m_nNumTextures; }
	virtual A3DAABB& GetSceneAABB() { return m_SceneAABB; }
	virtual int GetNumVisibleFaces() { return m_nNumVisibleFaces; }
	virtual bool IsOnGround(int nFaceIndex) { return false; }
	virtual bool IsUsedForPVS(int nFaceIndex) { return false; }
	virtual bool Is2Sided(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].MeshProps.Get2SidesFlag(); }
	virtual bool IsAlpha(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].dwRenderFlag == A3DSCENE_RENDER_ALPHA ? true : false; }

	inline A3DIBLLIGHTRANGE GetLightRange() { return m_IBLLightRange; }
	inline void SetLightRange(A3DIBLLIGHTRANGE iblLightRange) { m_IBLLightRange = iblLightRange; }

	inline A3DIBLLightGrid * GetLightGrid() { return m_pIBLLightGrid; }
	bool SetLightGrid(A3DIBLLightGrid * pLightGrid);

	static A3DIBLLightGrid * GetGobalLightGrid() { return m_pGlobalLightGrid; }
};

typedef class A3DIBLScene * PA3DIBLScene;

/*********************************************/
/*              Light Grid class             */
/*********************************************/
class A3DLightGridDLight;

enum A3DIBLLIGHTTYPE
{
	A3DIBLLIGHTTYPE_POINT = 0,
	A3DIBLLIGHTTYPE_SPOT,
	A3DIBLLIGHTTYPE_DIRECT
};

// structure for define a light source of Image Based lighting
typedef struct _A3DIBLLIGHTSOURCE
{
	A3DIBLLIGHTTYPE		type;	
	A3DVECTOR3			vecPos;
	A3DVECTOR3			vecDir;
	A3DCOLORVALUE		color;
	int					nLampStyle;
	int					nLampSize;
	float				vLampRange;
	float				vLampIntensity;

}A3DIBLLIGHTSOURCE, * PA3DIBLLIGHTSOURCE;

typedef struct _A3DIBLLIGHTSAMPLE
{
	// By now we only use one simulated direct light source, but in fact we can use more than one simulated direct light source here;
	WORD			wLightIndex;	// the index of the direct light in light source array
	WORD			wDLightIndex;	// the index of the dynamic light in dynamic light source array
	
	A3DCOLOR		colorDirect;	// the main direct light color at this point, if there are several direct light at this point, this is the combined color of all light sources
	A3DCOLOR		colorAmbient;	// the accumulate color values of all other lights (treated as ambient lights) at this point

} A3DIBLLIGHTSAMPLE, * PA3DIBLLIGHTSAMPLE;

#define A3DLIGHTGRID_MAX_DLIGHT	32
class A3DIBLLightGrid : public A3DObject
{
private:
	A3DDevice			* m_pA3DDevice;

	int					m_nLightSourceCount;
	A3DIBLLIGHTSOURCE	* m_pLightSources;

	int					m_nLampCount;
	A3DLamp				** m_pLampEffects;

	A3DIBLLIGHTSAMPLE	* m_pLightSamples;
	A3DTEXTUREOP		m_colorOP;
	
	float				m_vDirBrightness;		// the brightness factor of direct light
	float				m_vAmbientBrightness;	// the brightness factor of ambient light

	A3DVECTOR3			m_vecMin;			// minimum point of the light grid
	A3DVECTOR3			m_vecMax;			// maximum point of the light grid

	float				m_vXDF;				// x discreet factor
	float				m_vZDF;				// z discreet factor
	float				m_vYDF;				// y discreet factor

	int					m_nLength;			// grid cell number along x axis
	int					m_nWidth;			// grid cell number along z axis;
	int					m_nHeight;			// grid cell number along y axis;

	int					m_nXDim;			// sample points number along x axis
	int					m_nZDim;			// sample points number along z axis
	int					m_nYDim;			// sample points number along y axis
		
	// Dynamic Lights Section
	int					m_nDLightCount;		// count of dynamic lights;
	int					m_nNextDLightIndex; // after initialization it is 0
	A3DLightGridDLight	* m_aDLights[A3DLIGHTGRID_MAX_DLIGHT]; // dynamic light instance pointer

	bool				m_bOptimized;		// flag indicates whether this light grid has been optimized, such as compact by octree

protected:

public:
	A3DIBLLightGrid();
	virtual ~A3DIBLLightGrid();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool Load(A3DDevice* pA3DDevice, AFile* pFileToLoad);
	bool Save(AFile* pFileToSave);

	bool RenderLamps(A3DViewport * pCurrentViewport);
	bool AnimateLamps();

	bool PrepareFirstRender();

	// Dynamic lights sections;
	virtual bool GetNextDLight(int * pDLightID); // Get next dynamic light index that can be used; -1 means you can not add any more dynamic lights
	virtual bool SetFreeDLight(int nDLightID); // set a dynamic light free

	virtual bool UpdateDLight(int nDLightID, const A3DVECTOR3& vecPos, const A3DCOLORVALUE& color, const FLOAT& range);
	virtual bool UpdateDLightColorOnly(int nDLightID, const A3DCOLORVALUE& color); 

	// Optimize is used to do some optimizing code such as octree based light grid compact
	// but after optimized, a light grid is no longer easily modified.
	virtual bool Optimize(); 

	virtual bool AddLightSource(const A3DIBLLIGHTSOURCE& source);
	virtual bool GetLightSource(int nIndex, A3DIBLLIGHTSOURCE * pSource);

	virtual bool SetDimension(int nLength, int nWidth, int nHeight, const A3DVECTOR3& vecMin, const A3DVECTOR3& vecMax);
	virtual bool GetDimension(int * pnLength, int * pnWidth, int * pnHeight, A3DVECTOR3 * pvecMin, A3DVECTOR3 * pvecMax);

	// Get an equivalent light at some point using 3 direction linear interpolation
	// This function is used for main view weapons or the main actor in the scene
	// This function is a little bit slower than GetNearestLightInfo();
	virtual bool GetEquivalentLightInfo(const A3DVECTOR3& vecPoint, A3DVECTOR3 * pvecLightDir, A3DCOLORVALUE * pclLightDirect, A3DCOLOR * pclAmbient, A3DLIGHTPARAM * pDLightParam=NULL);
	virtual bool GetNearestLightInfo(const A3DVECTOR3& vecPoint, A3DVECTOR3 * pvecLightDir, A3DCOLORVALUE * pclLightDirect, A3DCOLOR * pclAmbient, A3DLIGHTPARAM * pDLightParam=NULL);

	virtual bool GetNearestLightSampleIndex(const A3DVECTOR3& vecPoint, int * pnIndex);
	virtual bool GetSamplePosition(int nIndex, A3DVECTOR3 * pvecPoint);

	virtual bool SetLightSample(int nIndex, const A3DIBLLIGHTSAMPLE& sample);
	virtual bool GetLightSample(int nIndex, A3DIBLLIGHTSAMPLE * pSample);

	inline void SetColorOP(A3DTEXTUREOP op) { m_colorOP = op; }
	inline A3DTEXTUREOP GetColorOP() { return m_colorOP; }

	inline void SetDirectBrightness(float vBrightness) { m_vDirBrightness = vBrightness; }
	inline float GetDirectBrightness() { return m_vDirBrightness; }

	inline void SetAmbientBrightness(float vBrightness) { m_vAmbientBrightness = vBrightness; }
	inline float GetAmbientBrightness() { return m_vAmbientBrightness; }
};

typedef class A3DIBLLightGrid * PA3DIBLLightGrid;

/*********************************************/
/*      Light Grid Dynamic Light Class       */
/*********************************************/
class A3DLightGridDLight
{
private:
	bool			m_bUsed;	// Flag indicate whether this dynamic light is now in use

	A3DVECTOR3		m_vecPos;
	A3DCOLORVALUE	m_color;
	float			m_vRange;

public:
	A3DLightGridDLight();
	~A3DLightGridDLight();

	inline bool IsUsed()			{ return m_bUsed; }
	inline void SetUsed(bool bUsed) { m_bUsed = bUsed; }

	inline A3DVECTOR3 GetPos()		{ return m_vecPos; }
	inline void SetPos(const A3DVECTOR3& vecPos) { m_vecPos = vecPos; }

	inline A3DCOLORVALUE GetColor() { return m_color; }
	inline void SetColor(const A3DCOLORVALUE& color) { m_color = color; }

	inline FLOAT GetRange()		{ return m_vRange; }
	inline void SetRange(const FLOAT& range) { m_vRange = range; }
};

typedef class A3DLightGridDLight * PA3DLightGridDLight;
#endif _A3DIBLSCENE_H_
