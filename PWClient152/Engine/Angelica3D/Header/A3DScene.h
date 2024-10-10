/*
 * FILE: A3DScene.h
 *
 * DESCRIPTION: Class representing the whole scene in A3D Engine
 *
 * CREATED BY: Hedi, 2002/3/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSCENE_H_
#define _A3DSCENE_H_

#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DDevObject.h"
#include "A3DSceneVisible.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define A3DSCENE_USESTREAM

#define A3DSCENE_RENDER_SOLID		1
#define A3DSCENE_RENDER_ALPHA		2

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DStream;
class A3DEngine;
class A3DDevice;
class A3DTexture;
class A3DSceneVisible;
class A3DMesh;
class A3DViewport;

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

class A3DScene : public A3DDevObject
{
	friend int FaceSortCompare(const void *arg1, const void *arg2);

private:

	struct TEXTURE_RECORD
	{
		int					nTexID;						// index in texture record buffer;
		char				szTextureName[MAX_PATH];	// Name of the texture;
		char				szDetailTextureName[MAX_PATH]; //Name of the detail texture;
		A3DMATERIALPARAM	material;					// Material parameter of this texture;
		A3DMATRIX4			matDetailTM;				// Texture Transformation Matrix for detail texture;
		bool				b2Sided;					// If this material is 2 sided;
		A3DMESHPROP			MeshProps;					// The mesh's property;

		int					nFaceNum;					// Total face using this texture;
		int					nFaceVisible;				// Current Visible Face Count;
	
		DWORD				dwRenderFlag;				// Flag to say this texture is to be rendered or not;
		A3DTexture*			pTexture;					// Pointer of this texture;
		A3DTexture*			pDetailTexture;				// Pointer of this texture's detail texture;
		A3DVERTEX*			pVerts;						// Pointer of vertex buffer;

	#ifdef A3DSCENE_USESTREAM
		A3DStream*			pStream;					// The predefined stream which can contain all vertex of this texture;
	#endif
	};
	
	struct FACE_RECORD
	{
		WORD	nTexID;			// The texture id of this face;
		int		nRefTicks;		// The ticks last time referenced this face (in Engine tick unit);
	};
	
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
	A3DSceneVisible*	m_pA3DVisHandler;

	bool				m_bRenderAll;		//	true, force to render all surfaces
	bool				m_bLastRenderAll;	//	true, last frame is rendering all
	OPTION				m_Option;

	int					m_nNumFaces;		// Total faces of this scene;
	A3DVERTEX*			m_pAllFaces;		// Faces buffer that consist of seperate vertex;
	FACE_RECORD*		m_pFaceRecords;		// Buffer stores each face's texture id and reference info;
	SORTEDFACE*			m_pSortedFaces;		// Buffer used for sorts;
	int					m_nNumVisibleFaces;	// Number of Visible Faces;
	int					m_nNumTextures;		// Texture number;
	TEXTURE_RECORD*		m_pTextureRecords;	// Texture list;
	A3DAABB				m_SceneAABB;

protected:

	bool AddMesh(A3DMesh * pMesh);
	TEXTURE_RECORD* FindTextureRecord(const char* szTextureName, const A3DMATERIALPARAM& material, bool b2Sided, const A3DMESHPROP& MeshProp);

public:
	A3DScene();
	virtual ~A3DScene();

	virtual bool Init(A3DEngine* pA3DEngine);
	virtual bool Release();

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Set visible handler
	void SetVisibleHandler(A3DSceneVisible* pVisHandler) { m_pA3DVisHandler = pVisHandler; }
	//	Get visible handler
	A3DSceneVisible* GetVisibleHandler() { return m_pA3DVisHandler; }

	//	Check position's visibility
	inline bool PosIsVisible(const A3DVECTOR3& vecPos);
	//	Check AABB's visibility
	inline bool AABBIsVisible(const A3DAABB& aabb);
	inline bool AABBIsVisible(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);

	// Serialize methods;
	virtual bool Load(A3DEngine* pA3DEngine, const char* szFile);
	virtual bool Load(A3DEngine* pA3DEngine, AFile* pFileToLoad);
	virtual bool Save(AFile* pFileToSave);

	virtual	bool UpdateVisibleSets(A3DViewport * pCurrentViewport);

	// Render current scene with the specified flag control;
	// See A3DScene.h for predefined flag
	virtual bool Render(A3DViewport * pCurrentViewport, DWORD dwFlag);
	virtual bool RenderSort(A3DViewport * pCurrentViewport, DWORD dwFlag, bool bNear2Far=false);

	// Import/Export methods;
	// For import, we will add this mox into current scene data, NOT REPLACE!
	virtual bool Import(char * szMoxFileName);
	virtual bool Export(char * szMoxFileName);

	virtual int GetFaceCount() { return m_nNumFaces; }
	A3DVERTEX * GetFaceBuffer() { return m_pAllFaces; }
	virtual bool GetAllFaceVerts(A3DVECTOR3 * pVertsBuffer, int * pnBufferSize);
	virtual int GetTextureNum() { return m_nNumTextures; }
	virtual A3DAABB& GetSceneAABB() { return m_SceneAABB; }
	virtual int GetNumVisibleFaces() { return m_nNumVisibleFaces; }
	virtual bool IsOnGround(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].MeshProps.GetOnGroundFlag(); }
	virtual bool IsUsedForPVS(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].MeshProps.GetBSPPVSFlag(); }
	virtual bool Is2Sided(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].b2Sided; }
	virtual bool IsAlpha(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].dwRenderFlag == A3DSCENE_RENDER_ALPHA ? true : false; }
	
	//	Force to render all surfaces
	virtual void ForceToRenderAll(bool bTrue) { m_bRenderAll = bTrue; }
	//	Get force render flag
	bool GetRenderAllFlag() { return m_bRenderAll; }
};

bool A3DScene::PosIsVisible(const A3DVECTOR3& vecPos)
{
	if (!m_pA3DVisHandler)
		return true;

	return m_pA3DVisHandler->PointIsVisible(vecPos);
}

//	Check AABB's visibility
bool A3DScene::AABBIsVisible(const A3DAABB& aabb)
{
	if (!m_pA3DVisHandler)
		return true;

	return m_pA3DVisHandler->AABBIsVisible(aabb);
}

bool A3DScene::AABBIsVisible(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	A3DAABB aabb(vMins, vMaxs);
	return AABBIsVisible(aabb);
}


#endif _A3DSCENE_H_
