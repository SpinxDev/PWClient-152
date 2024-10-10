/*
 * FILE: A3DFrame.h
 *
 * DESCRIPTION: Class standing for a coordinate system in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DFRAME_H_
#define _A3DFRAME_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include "AList.h"

class A3DDevice;
class A3DMesh;
class A3DViewport;
class A3DMaterial;

//	This struct is out of date and should be replaced by A3DFRAMEOBB. 
//	It's only used for loading old version .mox file
struct A3DFRAMEOBB_PROP
{
	bool			bCanShootThrough;
	bool			bNoMark;
	bool			bCanPickThrough;
	bool			bCanNotDestroy;
	A3DMATERIALTYPE	nMaterialType;
	char			foo[32];//This is used to make some reserved room for expanding new proerpty;
};

//	This struct is out of date and should be replaced by A3DFRAMEOBB. 
//	It's only used for loading old version .mox file
struct A3DFRAME_OBB
{
	char				szName[32];
	A3DOBB				a3dOBB;
	A3DFRAMEOBB_PROP	property;
};

class A3DFRAMEOBB
{
public:		//	Types

	//	Property flags
	enum
	{
		FG_SHOOTTHROUGH		= 0x0001,		//	Can be shoot through
		FG_NOMARK			= 0x0002,		//	No mark
		FG_PICKTHROUGH		= 0x0004,		//	Can be picked through
		FG_CANNOTDESTROY	= 0x0008,		//	Can not be destroyed
	};

public:		//	Constructors and Destructors

	A3DFRAMEOBB() { szName[0] = '\0'; dwProps = 0; }
	A3DFRAMEOBB(const A3DFRAMEOBB& fo)
	{ 
		memcpy(szName, fo.szName, 32);
		a3dOBB	= fo.a3dOBB;
		dwProps = fo.dwProps; 
		MtlType = fo.MtlType;
	}

public:		//	Attributes

	char			szName[32];
	A3DOBB			a3dOBB;
	DWORD			dwProps;
	A3DMATERIALTYPE	MtlType;

public:		//	Operations

	void SetShootThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_SHOOTTHROUGH; else dwProps &= ~FG_SHOOTTHROUGH; }
	bool GetShootThroughFlag() const { return (dwProps & FG_SHOOTTHROUGH) ? true : false; }
	void SetNoMarkFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOMARK; else dwProps &= ~FG_NOMARK; }
	bool GetNoMarkFlag() const { return (dwProps & FG_NOMARK) ? true : false; }
	void SetPickThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_PICKTHROUGH; else dwProps &= ~FG_PICKTHROUGH; }
	bool GetPickThroughFlag() const { return (dwProps & FG_PICKTHROUGH) ? true : false; }
	void SetCanNotDestroyFlag(bool bTrue) { if (bTrue) dwProps |= FG_CANNOTDESTROY; else dwProps &= ~FG_CANNOTDESTROY; }
	bool GetCanNotDestroyFlag() const { return (dwProps & FG_CANNOTDESTROY) ? true : false; }
};

class A3DFrame : public A3DObject
{
protected:
	A3DDevice *			m_pA3DDevice;

	A3DFrame *			m_pParent;
	AList				m_ChildList;
	AList				m_MeshList;
	
	ALISTELEMENT *		m_pLastFrameElement;
	ALISTELEMENT *		m_pLastMeshElement;

	A3DMATRIX4 *		m_pRelativeTM;
	A3DMATRIX4 			m_matAbsoluteTM;

	// The sum of child mesh's vert and face count;
	int					m_nVertCount;
	int					m_nIndexCount;

	int					m_nFrameCountRecursive;
	int					m_nFrameCount;
	int					m_nFrameRecursive;
	int					m_nFrame;

	A3DOBB *			m_pAutoOBBs; // Obbs calculated automatically from child meshes;
	A3DAABB *			m_pAutoAABBs; // AABBs calculated automatically from child meshes;

	int					m_nBoundingBoxNum;
	A3DFRAMEOBB *		m_pBoundingBox;
	int					m_nBoundingBoxNumRecursive;

	A3DVECTOR3			m_vecPos;

	bool				m_bDuplicated;
public:
	A3DFrame(); 
	~A3DFrame();
	
	A3DFrame * GetFirstChildFrame();
	A3DFrame * GetNextChildFrame();
	A3DMesh * GetFirstMesh();
	A3DMesh * GetNextMesh();

	bool FindChildByName(const char * szChildName, A3DFrame ** ppFrame);

	inline int GetNumChilds() { return m_ChildList.GetSize(); }
	inline int GetNumMeshes() { return m_MeshList.GetSize(); }
	inline int GetFrameCount() { return m_nFrameCount; }
	inline int GetFrameCountRecursive() { return m_nFrameCountRecursive; }

	inline A3DFrame * GetParent() { return m_pParent; }
	inline void SetParent(A3DFrame * pParent) { m_pParent = pParent; }
	A3DMATRIX4& GetRelativeTM(int nFrame);
	A3DMATRIX4& GetRelativeTM();
	A3DMATRIX4& GetAbsoluteTM();
	inline A3DMATRIX4* GetRelativeTMPointer() { return m_pRelativeTM; }

	bool SetTM(A3DMATRIX4 TM, int nFrame=0);

	virtual bool SetPos(A3DFrame * pRefFrame, A3DVECTOR3 vecPos, int nFrame=0);
	virtual bool SetOrientation(A3DFrame * pRefFrame, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, int nFrame=0);
	virtual bool AddRotation(A3DFrame * pRefFrame, A3DVECTOR3 vecAxis, FLOAT fDegree, int nFrame=0);
	virtual bool AddTranslation(A3DFrame * pRefFrame, A3DVECTOR3 vecOffset, int nFrame=0);
	virtual bool Scale(FLOAT fScale, int nFrame=0);
	
	virtual A3DVECTOR3 GetPos();

	A3DVECTOR3 Transform(A3DVECTOR3 vec);
	A3DVECTOR3 InverseTransform(A3DVECTOR3 vec);

	virtual bool UpdateToFrame(int nFrame, A3DMATRIX4 * pMatParent = NULL);
	virtual bool Init(A3DDevice * pDevice, int nFrameCount=1);
	virtual bool Duplicate(A3DDevice * pA3DDevice, A3DFrame * pOriginFrame);

	virtual bool Release();
	virtual bool Render(A3DViewport * pCurrentViewport, bool bNeedSort, bool bNeedCollect);

	int CalculateFrameCountRecursive();

	bool AddChild(A3DFrame * pChild);
	bool AddMesh(A3DMesh * pMesh);
	bool AddBoundingBox(const A3DFRAMEOBB& FrameOBB);

	inline bool IsEmptyFrame() { return m_MeshList.GetSize() == 0 && m_ChildList.GetSize() == 0; }

	A3DOBB GetChildOBB();
	bool BuildAutoOBB(); //Calculate an auto obb from there child meshes;
	A3DAABB GetChildAABB();
	bool BuildAutoAABB(); //Calculate an auto aabb from there child meshes;

	// Ray trace model's mesh triangles;
	bool RayTraceMesh(A3DVECTOR3& vecStart, A3DVECTOR3& vecDelta, A3DVECTOR3 * pvecPos, A3DVECTOR3 * pvecNormal, float * pvFraction);

	inline A3DAABB GetFrameAutoAABB(int index) { return m_pAutoAABBs[index]; }
	inline A3DOBB GetFrameAutoOBB(int index) { return m_pAutoOBBs[index]; }
	inline A3DAABB * GetFrameAutoAABBPointer() { return m_pAutoAABBs; }
	inline A3DOBB * GetFrameAutoOBBPointer() { return m_pAutoOBBs; }

	inline int GetFrameOBBNum() { return m_nBoundingBoxNum; }
	inline A3DFRAMEOBB& GetFrameOBB(int i) { return m_pBoundingBox[i]; }
	inline A3DFRAMEOBB* GetFrameOBBPointer() { return m_pBoundingBox; }
	inline int GetFrameOBBNumRecursive() { return m_nBoundingBoxNumRecursive; }

	inline int GetVertCount() { return m_nVertCount; }
	inline int GetIndexCount() { return m_nIndexCount; }

	bool SetExtraMaterial(A3DMaterial * pMaterial);

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pDevice, AFile * pFileToLoad);

protected:

	//	Load version 1 frame obb
	bool LoadFrameOBBV1(int iBoxNum, AFile* pFile, bool bBinary);
	//	Load version 2 frame obb
	bool LoadFrameOBBV2(int iBoxNum, AFile* pFile, bool bBinary);
	//	Save version 2 frame obb
	bool SaveFrameOBBV2(AFile* pFile, bool bBinary);
};

typedef A3DFrame * PA3DFrame;

#endif
