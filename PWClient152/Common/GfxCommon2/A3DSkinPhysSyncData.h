/*
 * FILE: A3DSkinPhysSyncData.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSKINPHYSSYNCDATA_H_
#define _A3DSKINPHYSSYNCDATA_H_

#include <AString.h>
#include <A3DMatrix.h>
#include <AArray.h>
#include <hashtab.h>
#include "A3DPhysActor.h"

#ifdef _ANGELICA21
#include "A3DAdhereMesh.h"
#endif

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

class A3DSkinModel;
class A3DSkin;
class A3DMeshBase;
class APhysXActorDesc;
class APhysXMaterialDesc;
class APhysXShapeDesc;
class APhysXClothAttacherDesc;
class APhysXClothObjectDesc;
class APhysXScene;
class APhysXClothAttacher;
class NxStream;

struct APhysXClothParameters;
struct APhysXClothVertexConstraintParameters;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinPhysSyncData
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinPhysSyncData
{
public:		//	Types

	//	Error code
	enum
	{
		ERR_OK = 0,			//	No error
		ERR_NO_MEMORY,		//	Not enough memory
		ERR_NO_ACTOR,		//	Not find valid actor
		ERR_NO_MESH,		//	Not find valid mesh
		ERR_FAILED_CREATE,	//	Failed to create test object
		ERR_HANG_ACTOR,		//	Some actor isn't linked to any cc-type hook
		ERR_ACTOR_NOTLINK,	//	Some actor isn't linked to hh-type hook
		ERR_NO_HHHOOK,		//	Not find hh-type hook
		ERR_OTHERS,			//	All other error
	};

	//	Actor data
	struct ACTOR_DATA : public A3DPhysActor
	{
	protected:

		AString		strHHHook;		//	hh-type hook
		bool		bHHIsBone;		//	true, hh-type hook is a bone
		float		fBoundRad;		//	Radius of bounding ball that is made up by dragged vertices

		AArray<DWORD>	aDraggedVerts;	//	Index of vertex that is dragged by this actor

	public:

		friend class A3DSkinPhysSync;
		friend class A3DSkinPhysSyncData;

		//	Operations
		ACTOR_DATA();
		ACTOR_DATA(const ACTOR_DATA& rhs);
		virtual ~ACTOR_DATA();

		//	Operator =
		ACTOR_DATA& operator = (const ACTOR_DATA& rhs);
		//	Clone
		A3DPhysActor* Clone() const;
		//	Get HHHook Name
		const char* GetHookName() const { return strHHHook; }
		//	Get Hook Is Bone
		bool GetHHIsBone() const { return bHHIsBone; }
		//	Get bound radius
		float GetBoundRadius() const { return fBoundRad; }
	};

	//	Mesh data
	struct MESH_DATA
	{
	protected:

		AString			strName;		//	Mesh name
		int				iNumVert;		//	Number of vertex
		int				iNumIndex;		//	Number of index
		bool			bRigid;			//	true, rigid mesh; false, skin mesh
		int				iBoneIdx;		//	Bone index. valid when bRigid = true;
		A3DVECTOR3*		aVerts;			//	Vertex buffer
		int*			aIndices;		//	Index buffer
		AArray<int>		aBlendBones;	//	Bones that effect this mesh

#ifdef _ANGELICA21
		AString         strAdhereMeshName; // which mesh this mesh adhere to
		// it is driven by adhered mesh when strAdhereMeshName is not empty
		A3DAdhereMesh::AdhereParam*        aAdherParams;  // corresponding to vertex buffer one by one. validate when strAdhereMeshName is not empty
#endif
		

		APhysXClothVertexConstraintParameters* aClothVertexConstraints;	// corresponding to vertex buffer one by one

		APhysXClothObjectDesc*	pClothDesc;		//	Cloth properties

	public:

		friend class A3DSkinPhysSync;
		friend class A3DSkinPhysSyncData;

		//	Operations
		MESH_DATA();
		~MESH_DATA();

		//	Get mesh name
		const char* GetMeshName() const { return strName; }
		//	Get vertex number
		int GetVertexNum() const { return iNumVert; }
		//	Get index number
		int GetIndexNum() const { return iNumIndex; }
		//	Get vertex position by index
		bool GetVertexPos(int n, A3DVECTOR3& vPos) const;
		//	Set vertex position by index
		void SetVertexPos(int n, const A3DVECTOR3& vPos);
		//	Get vertex index
		int GetIndex(int n) const;
		//	Get rigid flag
		bool IsRigid() const { return bRigid; }
		//	Get blend bones
		AArray<int>& GetBlendBones() { return aBlendBones; }

		APhysXClothObjectDesc* GetClothObjectDesc() const { return pClothDesc; }

		//	Get cloth parameters
		APhysXClothParameters* GetClothParams() const;
		//	Set cloth parameters
		void SetClothParams(const APhysXClothParameters* pSrc);

		//	Get cloth vertex constraint parameters
		APhysXClothVertexConstraintParameters* GetClothVertexConstraints() const
		{
			return aClothVertexConstraints;
		}

		APhysXClothVertexConstraintParameters* GetClothVertexConstraints()
		{
			return aClothVertexConstraints;
		}


		//	Set cloth vertex constraint parameters
		void SetClothVertexConstraints(APhysXClothVertexConstraintParameters* pClothVertexConstraints)
		{
			if(aClothVertexConstraints)
			{
				delete [] aClothVertexConstraints;
				aClothVertexConstraints = NULL;
			}

			if(pClothVertexConstraints)
			{
				aClothVertexConstraints = new APhysXClothVertexConstraintParameters[iNumVert];
				memcpy(aClothVertexConstraints, pClothVertexConstraints, iNumVert * sizeof(APhysXClothVertexConstraintParameters));
			}
		}

#ifdef _ANGELICA21
		void SetAdhereParams(A3DAdhereMesh::AdhereParam* params);

		void SetAdhereMeshName(const char* szAdhereName) { if (szAdhereName) strAdhereMeshName = szAdhereName; else strAdhereMeshName.Empty();}
		const char* GetAdhereMeshName() const { return strAdhereMeshName; } 

		const A3DAdhereMesh::AdhereParam* GetAdhereParams() const { return aAdherParams; }
#endif
		

	};

	//	Source mesh info
	struct SRC_MESH
	{
		int			iIndex;		//	Mesh index in skin
		AString		strName;	//	Mesh name
	};

	friend class A3DSkinPhysSync;
	friend class A3DSkinPhysSyncDataMan;
	friend class A3DModelPhysics;

public:		//	Constructor and Destructor

	A3DSkinPhysSyncData();
	virtual ~A3DSkinPhysSyncData();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(const char* szFile);
	//	Save data to file
	bool Save(const char* szFile);

	//	Initialize bind data
	bool InitBind(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel);
	//	Check bind flag
	bool GetBindInitFlag() { return m_bBindInit; }
	//	Get last create error code
	int GetLastCreateError() { return m_iInitBindErr; }
	//	Check if specified skin can be bound with this physical data
	bool SkinBindCheck(A3DSkin* pSkin);
	//	Get soft link flag
	bool GetSoftLinkFlag() const { return m_bSoftLink; }
	//	Set soft link flag
	void SetSoftLinkFlag(bool bSoftLink) { m_bSoftLink = bSoftLink; }

	//	Link actor to a HH-type hook
	bool LinkActorToHook(const char* szActor, A3DSkinModel* pSkinModel, const char* szHookHH, bool bHHIsBone);
	//	Un-link actor from hook
	bool UnlinkActorFromHook(const char* szActor);

	//	Create a new actor
	ACTOR_DATA* CreateActor(const char* szName, const A3DVECTOR3& vPos);
	//	Remove a actor
	void RemoveActor(const char* szName);
	//	Change actor name
	bool ChangeActorName(const char* szOldName, const char* szNewName);
	//	Get actor number
	int GetActorNum() const { return m_aActors.GetSize(); }
	//	Get actor
	ACTOR_DATA* GetActorByName(const char* szName, int* piIndex=NULL) const;
	ACTOR_DATA* GetActorByIndex(int n) const { return m_aActors[n]; }

	//	Get mesh data number
	int GetMeshNum() const { return m_aMeshes.GetSize(); }
	//	Get mesh data
	MESH_DATA* GetMeshByName(const char* szName, int* piIndex=NULL) const;
	MESH_DATA* GetMeshByIndex(int n) const { return m_aMeshes[n]; }
	//	Get source mesh info
	APtrArray<SRC_MESH*>& GetSrcRigidMeshInfo() { return m_aRigidMeshInfo; }
	APtrArray<SRC_MESH*>& GetSrcSkinMeshInfo() { return m_aSkinMeshInfo; }

	//	Get file name
	const char* GetFileName() const { return m_strFileName; }
	//	Get skin's physical description
	APhysXClothAttacherDesc* GetSkinPhysDesc() const { return m_pSkinPhysDesc; }

protected:	//	Attributes

	int			m_iInitBindErr;		//	Error code of InitBind()
	bool		m_bBindInit;		//	true, physical data has been prepared
	AString		m_strFileName;		//	Data file name set by Load()
	int			m_iVersion;			//	File data version
	bool		m_bSoftLink;		//	true, use sofe link
	
	APhysXClothAttacherDesc*	m_pSkinPhysDesc;	//	Skin physical desc

	APtrArray<ACTOR_DATA*>		m_aActors;		//	Actor array
	APtrArray<MESH_DATA*>		m_aMeshes;		//	Mesh array

	APtrArray<SRC_MESH*>		m_aRigidMeshInfo;	//	Info of rigid meshes those will create cloth data
	APtrArray<SRC_MESH*>		m_aSkinMeshInfo;	//	Info of skin meshes those will create cloth data

protected:	//	Operations

	//	Create skin's physical description
	bool CreateSkinPhysDesc(APhysXScene* pPhysScene);
	bool CreateSkinPhysDesc_OnLoad();
	//	Create skin's physical description with new scale
	bool CreateScaledSkinPhysDesc(A3DSkinModel* pSkinModel, A3DSkin* pSkin, APhysXScene* pPhysScene, APhysXClothAttacherDesc* pPhysDesc);
	//	Create scaled mesh
	bool CreateScaledMesh(A3DSkin* pSkin, A3DMeshBase* pMeshBase, AArray<A3DMATRIX4>& aBlendMats, MESH_DATA* pMeshData, A3DVECTOR3* aOutVerts, AArray<int>* aBlendBones);

	//	Build meshes from skin
	bool BuildMeshFromSkin(A3DSkinModel* pSkinModel, A3DSkin* pSkin, const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx);
	//	Release all meshes
	void ReleaseMeshes();
	//	Check if mesh match
	bool CompareMesh(MESH_DATA* pMesh, A3DMeshBase* pMeshBase);
	//	Collect available skin mesh
	bool CollectSkinMesh(A3DSkin* pSkin, APtrArray<A3DMeshBase*>& aMeshes, bool bCheckName);
	//	Create mesh data from specified skinmesh
	MESH_DATA* CreateMeshData(A3DSkin* pSkin, A3DMeshBase* pMeshBase, AArray<A3DMATRIX4>& aBlendMats);
	//	Record info of skin meshes that will create cloth data
	void RecordSourceMeshInfo(A3DSkin* pSkin, const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx);

	//	Calculate dragged vertices for actors
	bool CalcActorDraggedVerts(APhysXClothAttacher* pAttacherObject);

	//	Write string to stream
	bool WriteString(NxStream* ps, AString& str);
	//	Read string from stream
	bool ReadString(NxStream* ps, AString& str);
	//	Write actor data to stream
	bool WriteActorData(NxStream* ps, ACTOR_DATA* pActor);
	//	Write mesh data to stream
	bool WriteMeshData(NxStream* ps, MESH_DATA* pMesh);
	//	Read actor data to stream
	bool ReadActorData(NxStream* ps, ACTOR_DATA* pActor);
	//	Read mesh data to stream
	bool ReadMeshData(NxStream* ps, MESH_DATA* pMesh);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinPhysSyncDataMan
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinPhysSyncDataMan
{
public:		//	Types

	//	Data node
	struct DATA_NODE
	{
		A3DSkinPhysSyncData*	pData;		//	Data object
		DWORD					dwRefCnt;	//	Reference counter

		DATA_NODE()
		{
			pData		= NULL;
			dwRefCnt	= 1;
		}
	};

	typedef abase::hashtab<DATA_NODE*, int, abase::_hash_function> SyncDataTable;

public:		//	Constructor and Destructor

	A3DSkinPhysSyncDataMan();
	virtual ~A3DSkinPhysSyncDataMan();

public:		//	Attributes

	static A3DSkinPhysSyncDataMan& GetGlobalMan();

public:		//	Operations

	//	Load skin physical data from file
	A3DSkinPhysSyncData* LoadSyncData(const char* szFile);
	//	Release sync data
	void ReleaseSyncData(A3DSkinPhysSyncData* pSyncData);

protected:	//	Attributes

	SyncDataTable		m_SyncDataTab;		//	Data table
	CRITICAL_SECTION	m_csSyncDataTab;	//	Lock of m_SyncDataTab

protected:	//	Operations

	//	Search data node
	DATA_NODE* SearchDataNode(const char* szFile, DWORD dwFileID);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSKINPHYSSYNCDATA_H_
