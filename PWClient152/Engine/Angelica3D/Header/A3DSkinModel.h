/*
 * FILE: A3DSkinModel.h
 *
 * DESCRIPTION: A3D skin model class
 *
 * CREATED BY: duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINMODEL_H_
#define _A3DSKINMODEL_H_

#include "A3DTypes.h"
#include "A3DCoordinate.h"
#include "A3DTrace.h"
#include "A3DMaterial.h"
#include "AArray.h"
#include "AList2.h"
#include "AString.h"
#include "hashtab.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Identity and version of skin model file
#define	SKMDFILE_IDENTIFY	(('A'<<24) | ('S'<<16) | ('M'<<8) | 'D')
#define SKMDFILE_VERSION	9

//	Identity and version of skin physique file
#define	SPHYFILE_IDENTIFY	(('A'<<24) | ('P'<<16) | ('H'<<8) | 'Y')
#define SPHYFILE_VERSION	1

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

#pragma pack (push, 1)

//	Skin model file header
struct A3DSKINMODELFILEHEADER
{
	DWORD	dwFlags;		//	File's identity, SKMDFILE_IDENTIFY
	DWORD	dwVersion;		//	File's version, SKMDFILE_VERSION
	int		iNumSkin;		//	Number of skin
	int		iNumAction;		//	Number of action
	int		iNumHanger;		//	Number of hanger
	int		iNumProp;		//	Number of property
	BYTE	aReserved[60];	//	Reserved
};

//	Skin physique file header
struct A3DSKINPHYFILEHEADER
{
	DWORD	dwFlags;		//	File's identity, SKMDFILE_IDENTIFY
	DWORD	dwVersion;		//	File's version, SKMDFILE_VERSION
	int		iNumHitBox;		//	Number of hit boxes
	int		iNumPhyShape;	//	Number of physique shapes
	BYTE	aReserved[64];	//	Reserved
};

#pragma pack (pop)

class A3DEngine;
class A3DDevice;
class A3DSkeleton;
class A3DSkin;
class A3DViewport;
class A3DCameraBase;
class A3DGraphicsFX;
class A3DSkinModelAction;
class A3DSkinModelActionCore;
class A3DSMActionChannel;
class A3DHanger;
class A3DGFXHanger;
class A3DSkinModelHanger;
class A3DSkinHitBox;
class A3DSkinPhyShape;
class A3DIBLScene;
class A3DLight;
class A3DAnimJoint;
class A3DSkeletonHook;
class A3DVertexShader;
class AFile;
class A3DOcclusionProxy;
class A3DHLSL;
class A3DReplaceHLSL;
class A3DSkinModel;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

//	Render modifier
class A3DSkinModelRenderModifier
{
public:
	//在ForwardRender之前对该Model做一些修改
	virtual bool BeforeForwardRender(A3DSkinModel* pModel) = 0;
	//在ForwardRender之后对该Model再做一些修改
	virtual bool AfterForwardRender(A3DSkinModel* pModel) = 0;
	//在一帧渲染完之后做一些清理工作.
	virtual bool ClearAfterAllRender() = 0;
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinModel
//
///////////////////////////////////////////////////////////////////////////
class A3DSkinModel : public A3DCoordinate
{
public:		//	Types

	//	Trace flag
	enum
	{
		TRACE_RAY		= 0x0001,	//	Enable ray trace
		TRACE_AABB		= 0x0002,	//	Enable aabb trace
		TRACE_RAYTOBOX	= 0x0004,	//	Check bone hit box when ray trace
		TRACE_AABBTOBOX	= 0x0008,	//	Check bone hit box when aabb trace
	};

	//	Material using method
	enum
	{
		MTL_ORIGINAL = 0,		//	Use meshes' original material
		MTL_UNIFORM,			//	Use uniform material for all meshes
		MTL_SCALED,				//	Use scaled original material
	};

	//	RenderAtOnce flag
	enum
	{
		RAO_NOTEXTURE		= 0x01,
		RAO_NOMATERIAL		= 0x02,
		RAO_NOALPHA			= 0x04,
		RAO_NOPIXELSHADER	= 0x08,
		RAO_NOEFFECT		= RAO_NOTEXTURE,
		RAO_NOVERTEXSHADER	= 0x10,
		RAO_NOUPDATEBLENDMAT= 0x20,
		RAO_NOALL			= RAO_NOTEXTURE|RAO_NOALPHA|RAO_NOPIXELSHADER|RAO_NOVERTEXSHADER,
	};

	//	Auto model AABB type
	enum
	{
		AUTOAABB_SKELETON = 0,	//	Build auto model AABB using skeleton
		AUTOAABB_INITMESH,		//	Build auto model AABB using initial mesh
	};
	
	//	Load Skin Flag, Skin type of Load() function
	enum
	{
		LSF_DEFAULT = 0,	//	Load skin by Skin Manager
		LSF_NOSKIN,			//	Don't load skin
		LSF_UNIQUESKIN,		//	Load a unique skin
	};

	//	Act channels
	enum
	{
		ACTCHA_DEFAULT	= 0,
		ACTCHA_MAX		= 16,	//	Keep this value same with A3DSkinModelActionCore::ACTCHA_MAX
	};

	//	Ray trace or aabb trace result
	struct TRACERT
	{
		A3DSkinModel*	pModel;		//	The model which was hit
		A3DSkinHitBox*	pHitBox;	//	The bone bone which was hit
	};

	//struct REPLACEPARAM{
	//	AString SemaGFX;
	//	AString SemaEffect;
	//	LPVOID	ptr;
	//};

	//	Light information
	struct LIGHTINFO : public A3DCOMMCONSTTABLE
	{
		A3DCOLORVALUE	colAmbient;		//	Ambient color
		A3DVECTOR3		vLightDir;		//	Direction of directional light
		A3DCOLORVALUE	colDirDiff;		//	Directional light's diffuse color
		A3DCOLORVALUE	colDirSpec;		//	Directional light's specular color
		bool			bPtLight;		//	true, enable Point light, 
		A3DVECTOR3		vPtLightPos;	//	Position of spot light
		A3DCOLORVALUE	colPtAmb;		//	Point light's ambient color
		A3DCOLORVALUE	colPtDiff;		//	Point light's diffuse color
		A3DVECTOR3		vPtAtten;		//	Point light's attenuation
		float			fPtRange;		//	Point light's range

		LIGHTINFO();
	};

	//	Skin
	struct SKIN
	{
		A3DSkin*	pA3DSkin;		//	A3D skin object
		bool		bOwnSkin;		//	True, model can free the skin when necessary
		bool		bRender;		//	Render flag
	};

	typedef abase::hashtab<AString, AString, abase::_hash_function>	PropTable;

	friend class A3DSkinMan;
	friend class A3DSkinModelHanger;
	friend class A3DSkinModelMan;
	friend class A3DSkeleton;

	//typedef abase::vector<REPLACEPARAM> ReplaceParamArray;

public:		//	Constructors and Destructors

	A3DSkinModel();
	virtual ~A3DSkinModel();
	
public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Load model from file
	virtual bool Load(AFile* pFile, int iSkinFlag=LSF_DEFAULT);
	bool Load(const char* szFile, int iSkinFlag=LSF_DEFAULT);
	//	Save model from file
	virtual bool Save(AFile* pFile);
	bool Save(const char* szFile);

	//	Add a new skin
	int AddSkin(A3DSkin* pA3DSkin, bool bAutoFree);
	int AddSkinFile(const char* szSkinFile, bool bAutoFree);
	int AddSkinFile(AFile* pSkinFile, bool bAutoFree);
	//	Replace a skin
	bool ReplaceSkin(int iIndex, A3DSkin* pA3DSkin, bool bAutoFree);
	bool ReplaceSkinFile(int iIndex, const char* szSkinFile, bool bAutoFree);
	bool ReplaceSkinFile(int iIndex, AFile* pSkinFile, bool bAutoFree);
	//	Remove a skin item, this operation release both skin and skin item
	void RemoveSkinItem(int iItem);
	//	Release all skins
	void ReleaseAllSkins();
	//	Show / Hide a skin
	void ShowSkin(int iIndex, bool bShow);
	//	Bind hit boxes and physique shapes with model
	bool BindPhysique(const char* szFile);
	bool BindPhysique(AFile* pFile);
	//	Save hit boxes and physique shapes data to file
	bool SavePhysique(const char* szFile);
	bool SavePhysique(AFile* pFile);
	//	Remove hit boxes and physique shapes
	void RemovePhysique();

	//	Set vertex shader for all skin meshes and rigid meshes
	void SetVertexShader(A3DVertexShader* pvsSkinMesh, A3DVertexShader* pvsRigidMesh);
	//	Restore vertex shader for all skin meshes and rigid meshes
	void RestoreVertexShader();

	//	Update model
	bool Update(int iDeltaTime, bool bNoAnim=false);
	//	Render routine
	bool Render(A3DViewport* pViewport, bool bCheckVis=true, A3DSkinModelRenderModifier* pRenderModifier = NULL);
	//	Render model at once
	bool RenderAtOnce(A3DViewport* pViewport, DWORD dwFlags, bool bCheckVis=true, const A3DReplaceHLSL* pRepHLSL = NULL, bool bReplaceChildModelShader = true);
	//	Update skeleton and animation
	bool UpdateSkeletonAndAnim(int iDeltaTime, bool bNoAnim);

	//	Get action count
	int GetActionNum() const { return m_ActionList.GetCount(); }
	//	Get action
	A3DSkinModelActionCore* GetAction(const char* szName);
	//	Get first action
	A3DSkinModelActionCore* GetFirstAction();
	//	Get next action
	A3DSkinModelActionCore* GetNextAction();
	//	Play action
	bool PlayActionByName(const char* szActName, int iChannel, int iNumLoop=1, int iTransTime=200, bool bRestart=true, bool bChildToo=false, bool bAbsTrack=false);
	bool PlayActionByNameDC(const char* szActName, int iNumLoop=1, int iTransTime=200, bool bRestart=true, bool bChildToo=false, bool bAbsTrack=false)
	{ 
		return PlayActionByName(szActName, ACTCHA_DEFAULT, iNumLoop, iTransTime, bRestart, bChildToo, bAbsTrack);
	}
	//	Stop action
	void StopAction(int iChannel, bool bChildToo=false);
	//	Stop all joint actions, set all joint action to No.0 frame
	void StopAllActions(bool bChildToo=false);
	//	Get current animation time of morph action bound to specified bone
	int GetMorphActionTime(int iBone);
	//	Build action channel
	A3DSMActionChannel* BuildActionChannel(int iChannel);
	A3DSMActionChannel* BuildActionChannel(int iChannel, int iNumBone, int* aBones, bool bExclude=false);
	//	Get action channel
	A3DSMActionChannel* GetActionChannel(int iChannel);
	//	Get all bones states at specified action and frame
	bool GetBonesStateAtFrame(const char* szAct, int iRelFrame, AArray<A3DMATRIX4>& aOutMats, bool bBoneScale=true);
	//	Get a bone's state at specified action and frame
	bool GetBoneStateAtFrame(const char* szAct, int iRelFrame, const char* szBone, A3DMATRIX4& matOut, bool bBoneScale=true);

	//	Below interfaces only remained for compatibility with old applications
	bool PlayActionByName(const char* szActName, float fWeight, int iTransTime=200, bool bRestart=true, bool bChildToo=false);
	void StopActionByName(const char* szActName, bool bChildToo=false);
	//	Stop all actions and reset model to initial pose
	void ResetToInitPose();

	//	Get number of blend matrix
	int GetBlendMatrixNum() { return m_aBlendMats.GetSize(); }
	//	Get all blend matrices
	const A3DMATRIX4* GetBlendMatrices() { return (const A3DMATRIX4*)m_aBlendMats.GetData(); }
	//	Get all blend matrices for software processing
	const A3DMATRIX4* GetSWBlendMatrices() { return (const A3DMATRIX4*)m_aSWBlendMats.GetData(); }
	//	Get all transposed matrices for vertex shader
	const A3DMATRIX4* GetTVSBlendMatrices() { return (const A3DMATRIX4*)m_aTVSBlendMats.GetData(); }
	//	Get specified blend matrix
	const A3DMATRIX4& GetBlendMatrix(int iIndex) { return m_aBlendMats[iIndex]; }
	//	Get specified transposed blend matrix for vertex shader
	const A3DMATRIX4& GetTVSBlendMatrix(int iIndex) { return m_aTVSBlendMats[iIndex]; }
	//	Get Blend mat updated
	bool GetBlendMatUpdated() const { return m_bBlendMatUpdated; }
	//	Apply rigid mesh blend matrices
	bool ApplyVSRigidMeshMat(A3DViewport* pViewport, int iBone, int iVSConstIdx);

	//	Get hanger number
	int GetHangerNum() const { return m_aHangers.GetSize(); }
	//	Get hanger through index
	A3DHanger* GetHanger(int n) const { return m_aHangers[n]; }
	//	Find a hanger through it's name
	A3DHanger* GetHanger(const char* szName);
	//	Get a model hanger through it's name
	A3DSkinModelHanger* GetSkinModelHanger(const char* szName) { return (A3DSkinModelHanger*)SearchHanger(szName, A3D_CID_SKINMODELHANGER); }
	//	Get a GFX hanger through it's name
	A3DGFXHanger* GetGFXHanger(const char* szName) { return (A3DGFXHanger*)SearchHanger(szName, A3D_CID_GFXHANGER); }
	
	//	Get skeleton hook object by name
	A3DSkeletonHook* GetSkeletonHook(const char* szName, bool bNoChild);
	//	Bind a child model
	A3DSkinModelHanger* AddChildModel(const char* szHangerName, bool bOnBone, const char* szBindTo, const char* szModelFile, const char* szCCName);
	A3DSkinModelHanger* AddChildModel(const char* szHangerName, bool bOnBone, const char* szBindTo, A3DSkinModel* pModel, const char* szCCName);
	//	Get child model number
	int GetChildModelNum() { return m_aChildModels.GetSize(); }
	//	Get child model by index
	A3DSkinModel* GetChildModel(int n) { return m_aChildModels[n]; }
	//	Unbind child model of specified hanger but don't release it
	A3DSkinModel* UnbindChildModel(const char* szHangerName);
	//	Remove child model
	void RemoveChildModel(const char* szHangerName);
	//	Set / Get auto update child model flag
	void SetAutoUpdateChildFlag(bool bAutoUpdate) { m_bAutoUpdateChild = bAutoUpdate; }
	bool GetAutoUpdateChildFlag() { return m_bAutoUpdateChild; }
	//	Check if this model is a inborn child
	bool IsInbornChild() { return m_bInbornChild; }
	//	Get parent model
	A3DSkinModel* GetParent() { return m_pParent; }

	//	Set / Get trace flags
	void SetTraceFlag(DWORD dwFlags) { m_dwTraceFlag = dwFlags; }
	DWORD GetTraceFlag() const { return m_dwTraceFlag; }
	//	Get trace result
	const TRACERT& GetRayTraceResult() const { return m_RayTraceRt; }
	const TRACERT& GetAABBTraceResult() const { return m_AABBTraceRt; }
	//	Ray trace routine
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, RAYTRACERT* pTraceRt);
	//	AABB trace routine
	bool AABBTrace(AABBTRACEINFO* pInfo, AABBTRACERT* pTraceRt);

	//	Get skeleton
	A3DSkeleton* GetSkeleton() const { return m_pA3DSkeleton; }
	//	Get skin number
	int GetSkinNum() { return m_aSkins.GetSize(); }
	//	Get skin
	A3DSkin* GetA3DSkin(int n) { return m_aSkins[n] ? m_aSkins[n]->pA3DSkin : NULL;	}
	//	Get skin item
	SKIN* GetSkinItem(int n) { return m_aSkins[n]; }
	//	Get model file name
	const char* GetFileName() const { return m_strFileName; }
	//	Get track data directory
	const char* GetTrackDataDir() const { return m_strTcksDir; }
	//	Set track data directory
	void SetTrackDataDir(const char* szDir) { m_strTcksDir = szDir; }
	//	Get model ID
	DWORD GetModelID() const { return m_dwModelID; }

	//	Change skin own flag
	void SetSkinOwnFlag(int n, bool bOwn) { if (m_aSkins[n]) m_aSkins[n]->bOwnSkin = bOwn; }

	//	Get / Set model's light information
	void SetLightInfo(const LIGHTINFO& Info);
	void SetLightInfo(A3DIBLScene* pIBLScene, bool Equivalent);
	const LIGHTINFO& GetLightInfo() { return m_LightInfo; }
	//	Set / Get specular flag
	void EnableSpecular(bool bEnable);
	bool IsSpecularEnable() const { return m_bSpecular; }
	//	Set / Get material using method
	void SetMaterialMethod(int iMethod) { m_iMtlMethod = iMethod; }
	int GetMaterialMethod() const { return m_iMtlMethod; }
	//	Set / Get uniform material
	void SetUniformMaterial(const A3DMaterial& mtl) { m_UniformMtl = mtl; }
	A3DMaterial& GetUniformMaterial() { return m_UniformMtl; }
	//	Set / Get material scale factor
	void SetMaterialScale(const A3DCOLORVALUE& Scale) { m_MtlScale = Scale; }
	void SetMaterialScale(float r, float g, float b, float a) { m_MtlScale.Set(r, g, b, a); }
	A3DCOLORVALUE& GetMaterialScale() { return m_MtlScale; }
	//	Set / Get transparent value of model
	void SetTransparent(float fTransparent);
	float GetTransparent() { return m_fTransparent; }
	//	Set / Get transparence from parent model
	void SetInheritTransFlag(bool bTrue) { m_bInheritTrans = bTrue; }
	bool GetInheritTransFlag() { return m_bInheritTrans; }
	//	Set / Get blend states
	void SetSrcBlend(A3DBLEND b) { m_SrcBlend = b; }
	void SetDstBlend(A3DBLEND b) { m_DstBlend = b; }
	inline A3DBLEND GetSrcBlend();
	inline A3DBLEND GetDstBlend();
	//	Set / Get alpha compare switch
	void EnableAlphaComp(bool bEnable) { m_bAlphaComp = bEnable; }
	bool GetAlphaCompFlag() { return m_bAlphaComp; }
	//	Set / Get extra emissive light
	void SetExtraEmissive(const A3DCOLORVALUE& Emissive) { m_colEmissive = Emissive; }
	A3DCOLORVALUE& GetExtraEmissive() { return m_colEmissive; }
	//	Set / Get alpha sort id
	void SetAlphaSortID(int id) { m_idAlphaSort = id; }
	int GetAlphaSortID() const { return m_idAlphaSort; }
	//	Set / Get alpha sort weight
	void SetAlphaSortWeight(int iWeight) { m_iAlphaWeight = iWeight; }
	int GetAlphaSortWeight() const { return m_iAlphaWeight; }
	//	Get version
	DWORD GetVersion() { return m_dwVersion; }

	//	Get model's AABB
	const A3DAABB& GetModelAABB() const { return m_ModelAABB; }
	//	Set auto model AABB type
	void SetAutoAABBType(int iType);
	//	Get auto model AABB type
	int GetAutoAABBType() { return m_iAutoAABB; }
	//	Add a skin hit box
	int AddSkinHitBox(const char* szName, int iBone, const A3DOBB& obb);
	//	Get skin hit box by name and index
	A3DSkinHitBox* GetSkinHitBox(const char* szName, int* piIndex);
	//	Get skin hit box by index
	A3DSkinHitBox* GetSkinHitBox(int n) { return m_aHitBoxes[n]; }
	//	Get number of skin hit boxes
	int GetSkinHitBoxNum() const { return m_aHitBoxes.GetSize(); }

	//	Add a skin physique shape
	int AddSkinPhyShape(const char* szName, int iBone, const A3DCAPSULE& cc);
	//	Get skin physique shape by name and index
	A3DSkinPhyShape* GetSkinPhyShape(const char* szName, int* piIndex);
	//	Get skin physique shape by index
	A3DSkinPhyShape* GetSkinPhyShape(int n) { return m_aPhyShapes[n]; }
	//	Get number of skin physique shape
	int GetSkinPhyShapeNum() { return m_aPhyShapes.GetSize(); }

	//	Model is visible ?
	bool IsVisible(A3DCameraBase* pCamera);
	//	Get m_bNoAnimUpdate flag
	bool GetNoAnimUpdateFlag() { return m_bNoAnimUpdate; }
	//	Set / Get hide flag
	void Hide(bool bHide) { m_bHide = bHide; }
	bool IsHidden() { return m_bHide; }

	//	Add property
	bool AddProperty(const AString& strName, const AString& strValue);
	//	Set property value
	void SetProperty(const AString& strName, const AString& strValue);
	//	Get property value
	const char* GetProperty(const AString& strName);
	//	Delete property
	void DeleteProperty(const AString& strName);
	//	Get property number
	int GetPropertyNum() const { return static_cast<int>(m_PropTable.size()); }
	//	Get property table
	PropTable& GetPropertyTable() { return m_PropTable; }

	//	Reset absolute track
	void ResetAbsTrack() { m_aAbsTracks.RemoveAll(); }
	//	Register absolute track
	void RegisterAbsTrack(int iNumTrack, int* aTrackIDs);
	//	Register absolute track of specified bone
	bool RegisterAbsTrackOfBone(const char* szBoneName);
	//	Get registered absolute track
	int GetAbsTrackNum() { return m_aAbsTracks.GetSize(); }
	int GetAbsTrack(int n) { return m_aAbsTracks[n]; }
	AArray<int, int>& GetAbsTracks() { return m_aAbsTracks; }

	//	Get A3D engine
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	//	Get A3D device
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }

	//	Below function are used by model editor or exportor !
	//	Set blend matrix
	void SetBlendMatrix(int i, const A3DMATRIX4& mat) { m_aBlendMats[i] = mat; }
	//	Save empty model
	bool EmptySave(const char* szFile, const char* szSkeleton, const char* aSkinFiles[], int iNumSkin, const char* szPhysique);
	APtrList<A3DSkinModelActionCore*>& GetActionList() { return m_ActionList; }
	APtrArray<SKIN*>& GetSkins() { return m_aSkins; }

	bool CheckTangentError();	// 调用后如果法线和切线错误,纹理将被替换.避免多线程调用

	//	Affected by dynamic point light ?
	bool HasDynamicLight() const { return m_LightInfo.bPtLight; }
	//inline ReplaceParamArray& GetReplaceParam();

	//	Update model's hit boxes
	bool UpdateHitBoxes(int iDeltaTime);

    const A3DVECTOR4& GetBorderColor() const { return m_vBorderColor; }

    // vColor : RGBA order
    void EnableBorder(float fWidth, const A3DVECTOR4& vColor) { m_fBorderWidth = fWidth, m_vBorderColor = vColor; }
    void DisableBorder() { m_fBorderWidth = 0; }

    float GetBorderWidth() const { return m_fBorderWidth; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;
	A3DDevice*		m_pA3DDevice;
	A3DSkeleton*	m_pA3DSkeleton;

	AString			m_strFileName;		//	Model file name
	AString			m_strPhyFile;		//	Physique file name
	DWORD			m_dwModelID;		//	Model ID
	int				m_iTickTime;		//	Time time passed by Update()
	bool			m_bHide;			//	true, don't render
	bool			m_bModelMan;		//	true, this model is loaded by A3DSkinModelMan
	DWORD			m_dwVersion;		//	Loaded file version
	AString			m_strTcksDir;		//	Track set file directory which is relative to smd file's directory
	A3DSkinModel*	m_pParent;			//	Parent model
    
	APtrArray<SKIN*>			m_aSkins;			//	Skin array
	APtrArray<A3DHanger*>		m_aHangers;			//	Hanger object array
	APtrArray<A3DSkinModel*>	m_aChildModels;		//	Child models
	A3DSMActionChannel*			m_aActChannels[ACTCHA_MAX];		//	Action channels
	AArray<int, int>			m_aAbsTracks;		//	Absolute tracks

	APtrList<A3DSkinModelActionCore*>	m_ActionList;		//	Action list
	ALISTPOSITION	m_ActionPos;		//	Used to enumerate actions

	bool			m_bNoAnimUpdate;			//	true, last update is caused by UpdateWithoutAnim()

	bool			m_bSpecular;		//	true, consider specular effect
	LIGHTINFO		m_LightInfo;		//	Light information of model
	int				m_iMtlMethod;		//	Material using method
	A3DMaterial		m_UniformMtl;		//	Uniform material
	A3DCOLORVALUE	m_MtlScale;			//	Material color scale factor
	A3DCOLORVALUE	m_colEmissive;		//	Extra emissive light of whole model which effect all meshes
	float			m_fTransparent;		//	Transparent.< 0: isn't transparent; > 0: transparent; 
											//	0: isn't transparent but rendered in alpha list
	bool			m_bInheritTrans;	//	true, inherit transparence from parent
	A3DBLEND		m_SrcBlend;			//	Souce blend state
	A3DBLEND		m_DstBlend;			//	Destination blend state
	bool			m_bAlphaComp;		//	Alpha compare switch
	bool			m_bAutoUpdateChild;	//	true, update child models automatically, true is default value
	bool			m_bInbornChild;		//	true, this model is a inborn child of other model
	
	int				m_idAlphaSort;		//	ID used to do alpha sort, 0 is invalid value
	int				m_iAlphaWeight;		//	Alpha sort weight, more bigger more closer

	A3DAABB			m_ModelAABB;		//	Model's AABB
	int				m_iAutoAABB;		//	Auto AABB type
	A3DOBB			m_obbInitMesh;		//	Initial mesh OBB

	DWORD			m_dwTraceFlag;		//	Trace flag
	TRACERT			m_RayTraceRt;		//	Result of last ray trace
	TRACERT			m_AABBTraceRt;		//	Result of last aabb trace
	bool			m_bCalcBevels;		//	true, need recalculating obb bevels

	PropTable		m_PropTable;		//	Property table
	bool			m_bBlendMatUpdated;	//	flag indicates whether blend matrices has been recalculated after last update call
    A3DVECTOR4      m_vBorderColor;     //  Border Color;
    float           m_fBorderWidth;     //  Border Width;

	AArray<A3DMATRIX4>				m_aBlendMats;		//	Normal blend matrices
	AArray<A3DMATRIX4>				m_aSWBlendMats;		//	Software processing blend matrices, which consider world center offset
	AArray<A3DMATRIX4>				m_aTVSBlendMats;	//	Transposed blend matrices for vertex shader
	APtrArray<A3DSkinHitBox*>		m_aHitBoxes;		//	Skin Hit boxes
	AArray<OBBBEVELS>				m_aOBBBevels;		//	OBB bevels
	APtrArray<A3DSkinPhyShape*>		m_aPhyShapes;		//	Physique shapes
    A3DOcclusionProxy*              m_pOccProxy;        //  Proxy for occlusion culling
	//ReplaceParamArray				m_aReplaceParam;

protected:	//	Operations

	//	Bind skeleton with model
	A3DSkeleton* BindSkeletonFile(const char* szSkeletonFile);
	//	Remove current bound skeleton
	void RemoveSkeleton(bool bRemoveCoord);

	//	Search action
	ALISTPOSITION SearchAction(const char* szName, DWORD dwNameID);
	//	Update skeleton in idle state
	void UpdateIdleState(int iDeltaTime);
	//	Build specified skin item
	bool BuildSkinItem(SKIN* pSkinItem, A3DSkin* pA3DSkin, bool bAutoFree);
	//	Clean a skin item
	void CleanSkinItem(SKIN* pSkinItem);
	//	Add a unique skin
	int AddUniqueSkinFile(const char* szSkinFile);

	//	Add a hanger
	bool AddHanger(A3DHanger* pHanger);
	//	Load a hanger from file
	bool LoadHanger(AFile* pFile);
	//	Release all hangers
	void ReleaseAllHangers();
	//	Remove a hanger
	void RemoveHanger(int iIndex);
	void RemoveHanger(A3DHanger* pHanger);
	//	Release a hanger
	void ReleaseHanger(A3DHanger* pHanger);
	//	Search a hanger of specified type
	A3DHanger* SearchHanger(const char* szName, DWORD dwClassID);

	//	Update routine without time counting
	bool UpdateInternal(int iDeltaTime, bool bNoAnim);
	//	Update all blend matrices
	bool UpdateBlendMatrices();
	//	Update physique shape
	bool UpdatePhysiqueShapes(int iDeltaTime);
	//	Update initial mesh OBB
	void UpdateInitMeshOBB();
	//	Update model in child mode.
	bool UpdateAsChild(int iDeltaTime);
	//	Apply model light
	void ApplyModelLight(A3DLight* pDirLight, A3DLight* pDPtLight);

	//	Called when a bone's whole scale matrix changes
	void OnBoneWholeScaleChanges();
	//	Pass bone's whole scale to a child model
	void PassWholeScaleToChild(A3DSkinModelHanger* pHanger);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

A3DBLEND A3DSkinModel::GetSrcBlend() 
{ 
	return m_SrcBlend;	
}

A3DBLEND A3DSkinModel::GetDstBlend() 
{ 
	return m_DstBlend;	
}


#endif	//	_A3DSKINMODEL_H_

