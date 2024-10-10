#ifndef		_A3DGFXEX_H_
#define		_A3DGFXEX_H_

#include <A3DObject.h>
#include <hashmap.h>
#include <A3DVector.h>
#include <A3DMatrix.h>
#include <A3DQuaternion.h>
#include <A3DGeometry.h>
#include "GfxCommonTypes.h"

enum GfxValueType;
enum GFX_STATE;
enum SHAKECAM;

class GfxParamInfo
{
public:
	AString			m_strEleName;
	int				m_nParamId;
	GfxValueType	m_eumType;
};

struct GFX_TICK_TIME
{
	DWORD dwCount;
	DWORD dwTickTime;
};

class A3DDevice;
class A3DViewport;
class AFile;
class AFileImage;
class A3DGFXElement;
class A3DGFXPhysDataMan;
class A3DGFXCtrlNoiseBase;
class ShakeAffector;
class IShakeCamera;
struct GFX_PROPERTY;

typedef abase::vector<GfxParamInfo*> GfxParamList;
typedef abase::vector<A3DGFXElement*> GfxElementList;

class A3DGFXEx : public A3DObject
{
public:
	A3DGFXEx();
	virtual ~A3DGFXEx();

	friend class A3DGFXExMan;
	friend class A3DGFXContainer;

protected:

	typedef abase::hash_map<AString, A3DGFXElement*> EleMap;

	GfxElementList	m_EleLst;
	EleMap			m_EleMap;
	A3DDevice*		m_pDev;
	GFX_STATE		m_iState;
	A3DMATRIX4		m_matScale;
	A3DMATRIX4		m_matParent;
	A3DMATRIX4		m_matNoScale;
	A3DMATRIX4		m_matGroundNormal;
	A3DMATRIX4		m_matGroundHeight;
	A3DVECTOR3		m_vPos;
	A3DVECTOR3		m_vOldPos;
	A3DVECTOR3		m_vGroundNorm;
	float			m_fGroundHeight;
	A3DQUATERNION	m_qDir;
	A3DQUATERNION	m_qMatchGrndDir;
	bool			m_bTransDirty;
	DWORD			m_dwVersion;
	DWORD			m_dwTimeSpan;
	float			m_fDefPlaySpeed;
	float			m_fDefScale;
	float			m_fDefAlpha;
	float			m_fPlaySpeed;
	float			m_fScale;
	float			m_fAlpha;
	float			m_fActualSpeed;
	float			m_fActualScale;
	float			m_fActualAlpha;
	bool			m_bMatchGround;
	bool			m_bUseRaytrace;
	
	//	Camera shake properties
	A3DGFXCtrlNoiseBase* m_pShakeNoise;
	ShakeAffector*	m_pShakeAffector;
	int				m_iShakeAffectorType;
	int				m_iShakeType;			// None, Rand, Up-Down

	A3DAABB			m_AABBOrg;
	A3DAABB			m_AABB;
	A3DVECTOR3*		m_pAABBVerts;
	bool			m_bUseAABB;
	bool			m_bCalcAABB;
	bool			m_bAccurateAABB;
	bool			m_bFadeByDist;
	float			m_fFadeStart;
	float			m_fFadeEnd;
	DWORD			m_dwRenderTick;
	bool			m_bLightVisible;
	DWORD			m_dwFileID;
	bool			m_bCloseToGrnd;
	float			m_fDeltaAlpha;
	float			m_fTargetAlpha;
	DWORD			m_dwTransTime;
	bool			m_bVisible;
	float			m_fExtent;
	float			m_fDistToCam;
	bool			m_bDisableCamShake;
	bool			m_bEnableCamBlur;
	bool			m_bUseLOD;
	bool			m_bFaceToViewer;
	bool			m_bUseOuterColor;
	A3DCOLORVALUE	m_OuterColor;
	bool			m_bNoChangeDir;
	bool			m_b2DRender;
	bool			m_b2DBackLayer;
	bool			m_bZTestEnable;			// only manually set
	bool			m_bCreatedByGFXECM;		// this GFX is created by a ECModel that is created by a GFX
	bool			m_bEmitterStopped;		// called StopParticleEmit after played
	bool			m_bIsAngelica3GFX;		// is this file created in angelica3 gfx editor
	bool			m_bUseParentModelPtr;
	int				m_nSfxPriority;
	clientid_t		m_nId;
	modelh_t		m_hostModel;
	A3DGFXPhysDataMan *m_pPhysDataMan;
	IShakeCamera*	m_pShakeCamera;			// Shake camera module
	bool			m_bChild;
	GFX_TICK_TIME*	m_pTickTime;
	bool			m_bFadingOut;
	DWORD			m_dwSkipTime;

	// Multi-thread loading mode
	volatile bool	m_bResourceReady;
	volatile bool	m_bLoadingInThread;
	bool			m_bStateInit;
	volatile bool	m_bToRelease;

	AString			m_strFile;

protected:

	void AddNewEleMap(A3DGFXElement* pEle);
	bool TickElements(DWORD dwTickTime);
	bool Render(A3DViewport* pView);

public:

	bool Init(A3DDevice* pDev);
	bool Load(A3DDevice* pDev, AFileImage* pFile);
	bool Load(A3DDevice* pDev, const char* szFile);
	bool Save(AFile* pFile);
	virtual A3DGFXEx* Clone() const;
	A3DGFXEx& operator = (const A3DGFXEx& src);
	bool Release();
	float GetExtent() const { return m_fExtent; }
	float GetDistToCam() const { return m_fDistToCam; }
	void SetUseLOD(bool b) { m_bUseLOD = b; }
	bool GetUseLOD() const { return m_bUseLOD; }
	A3DDevice* GetA3DDevice() const { return m_pDev; }
	DWORD GetVersion() const { return m_dwVersion; }

	//	Start function will first call stop and then restart the GFX
	//	parameter bForceStop is never used
	//	keep the interface parameter for compatibility
	bool Start(bool bForceStop = false);
	bool Pause();
	//	bForceStopPhys will cause all phys object released immediately
	bool Stop(bool bResetState = true);
	bool TickAnimation(DWORD dwTickTime);
	void StartElements();
	
	//	Stop particle emitting
	void StopParticleEmit();

	GFX_STATE GetState() const { return m_iState; }

	bool IsParticleEmitStopped() const { return m_bEmitterStopped; }
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool IsVisible() const { return m_bVisible; }
	float GetDefPlaySpeed() const { return m_fDefPlaySpeed; }
	float GetPlaySpeed() const { return m_fPlaySpeed; }
	float GetActualSpeed() const { return m_fActualSpeed; }
	void CalcActualSpeed() { m_fActualSpeed = m_fDefPlaySpeed * m_fPlaySpeed; }
	void SetDefPlaySpeed(float fSpeed)
	{
		m_fDefPlaySpeed = fSpeed;
		CalcActualSpeed();
	}
	void SetPlaySpeed(float fSpeed)
	{
		m_fPlaySpeed = fSpeed;
		CalcActualSpeed();
	}

	float GetDefScale() const { return m_fDefScale; }
	float GetScale() const { return m_fScale; }
	float GetActualScale() const { return m_fActualScale; }
	void CalcActualScale()
	{
		m_fActualScale = m_fDefScale * m_fScale;
		m_matScale.Scale(m_fActualScale, m_fActualScale, m_fActualScale);
		m_bTransDirty = true;
	}
	void SetDefScale(float fScale)
	{
		m_fDefScale = fScale;
		CalcActualScale();
	}
	void SetScale(float fScale)
	{
		m_fScale = fScale;
		CalcActualScale();
	}

	float GetDefAlpha() const { return m_fDefAlpha; }
	float GetAlpha() const { return m_fAlpha; }
	float GetActualAlpha() const { return m_fActualAlpha; }
	void CalcActualAlpha() { m_fActualAlpha = m_fDefAlpha * m_fAlpha; }
	void SetDefAlpha(float fAlpha)
	{
		m_fDefAlpha = fAlpha;
		CalcActualAlpha();
	}
	void SetAlpha(float fAlpha)
	{
		m_fAlpha = fAlpha;
		CalcActualAlpha();
	}

	void SetAlphaTransition(float fTargetAlpha, DWORD dwTransTime);
	bool InAlphaTransition() const { return m_dwTransTime != 0; }
	bool AlphaIsInc() const { return m_fDeltaAlpha > 0.0f; }

	bool IsUsingOuterColor() const { return m_bUseOuterColor; }
	const A3DCOLORVALUE& GetOuterColor() const { return m_OuterColor; }
	void SetOuterColor(A3DCOLORVALUE cl)
	{
		m_bUseOuterColor = true;
		m_OuterColor = cl;
	}
	void ClearOuterColor() { m_bUseOuterColor = false; }

	bool GetFaceToViewer() const { return m_bFaceToViewer; }
	void SetFaceToViewer(bool b) { m_bFaceToViewer = b; }
	bool GetFadeByDist() const { return m_bFadeByDist; }
	void SetFadeByDist(bool bFade) { m_bFadeByDist = bFade; }
	float GetFadeStart() const { return m_fFadeStart; }
	void SetFadeStart(float fStart) { m_fFadeStart = fStart; }
	float GetFadeEnd() const { return m_fFadeEnd; }
	void SetFadeEnd(float fEnd) { m_fFadeEnd = fEnd; }

	//	Has shake camera property ?
	bool GetShakeCam() const;
	//	Deprecated. set camera shake property, camera will shake in random direction
	void SetShakeCam(bool bShake);
	//	New interface, using this function is encouraged
	//	See reference for enum SHAKECAM
	void SetShakeCamType(SHAKECAM iShakeType);
	//	Get the type of shaking camera
	int GetShakeCamType() const { return m_iShakeType; }
	//	Get Shake noise
	A3DGFXCtrlNoiseBase* GetShakeNoise() { return m_pShakeNoise; }
	//	Set Shake noise
	void SetShakeNoise(A3DGFXCtrlNoiseBase* pNoise) { m_pShakeNoise = pNoise; }
	//	Get Shake camera
	IShakeCamera* GetShakeCamera() const { return m_pShakeCamera; }
	//	Set Shake camera
	void SetShakeCamera(IShakeCamera* pShakeCam) { m_pShakeCamera = pShakeCam; }
	//	Get accumulated shake offset
	const A3DVECTOR3& GetShakeOffset() const;

	//	Get is no change direction gfx (do not use this in 2D gfx)
	bool IsNoChangeDir() const { return m_bNoChangeDir; }
	//	Set is no change direction gfx (do not use this in 2D gfx)
	void SetNoChangeDir(bool b) { m_bNoChangeDir = b; }

	void SetPos(const A3DVECTOR3& vPos);
	void SetDirAndUp(const A3DVECTOR3 vDir, const A3DVECTOR3& vUp);
	bool SetParentTM(const A3DMATRIX4& matParent);
	void CalcParentTM();
	const A3DMATRIX4& GetParentTM();
	const A3DMATRIX4& GetGrndNormalTM() const { return m_matGroundNormal; }
	const A3DMATRIX4& GetGrndHeightTM() const { return m_matGroundHeight; }
	const A3DVECTOR3& GetPos() const { return m_vPos; }
	const A3DQUATERNION& GetDir() const { return m_qDir; }
	const A3DQUATERNION& GetMatchGrndDir() const { return m_qMatchGrndDir; }
	const A3DMATRIX4& GetScaleMat() const { return m_matScale; }
	void UpdateEleParam(const AString& strEleName, int nParamId, const GFX_PROPERTY& gp);
	void GetParamInfoList(GfxParamList& ParamInfoList);
	bool IsParamEnable(const AString& strEleName, int nParamId) const;

	void UpdateBindEle();

	// GFXEditor interface
	A3DGFXElement* AddEmptyElement(int nEleId);
	void AddGFXElement(A3DGFXElement* pEle);
	void RemoveElement(A3DGFXElement* pEle);
	bool ChangeEleName(A3DGFXElement* pEle, AString strNewName);
	int GetElementCount() const { return (int)m_EleLst.size(); }
	A3DGFXElement* GetElement(int nIndex) { return m_EleLst[nIndex]; }
	const A3DGFXElement* GetElement(int nIndex) const { return m_EleLst[nIndex]; }
	A3DGFXElement* GetElementByName(const AString& strName)
	{
		EleMap::iterator it = m_EleMap.find(strName);

		if (it == m_EleMap.end())
			return NULL;

		return it->second;
	}
	void InterchangeEle(int i1, int i2)
	{
		A3DGFXElement* tmp = m_EleLst[i1];
		m_EleLst[i1] = m_EleLst[i2];
		m_EleLst[i2] = tmp;
	}
	void SetToEditMode();
	void UpdateAllTexs();
	bool IsTexUsed(const char* szTex);
	bool MatchGrnd() const { return m_bMatchGround; }
	void SetMatchGround(bool b) { m_bMatchGround = b; }	
	bool IsUseRaytrace() const { return m_bUseRaytrace; }
	void SetUseRaytrace(bool b) { m_bUseRaytrace = b; }
	//	When Sub elements' property (GroundNormal, GroundHeight) changed, then this property should be updated
	//	This function would only be called in editor(through A3DGFXElement::SetProperty)
	void UpdateMatchGround();
	void Reset();
	bool IsInfinite() const;
	void StartCalcAABB();
	void EndCalcAABB();
	bool IsCalcingAABB() const { return m_bCalcAABB; }
	void ResetCalcingAABB() { m_bCalcAABB = false; }
	const A3DAABB& GetAABBOrg() const { return m_AABBOrg; }
	A3DAABB& GetAABBOrgRef() { return m_AABBOrg; }
	void SetAABBOrg(const A3DAABB& aabb) { m_AABBOrg = aabb; }
	bool UsingAABB() const { return m_bUseAABB; }
	void SetUseAABB(bool b) { m_bUseAABB = b; }
	bool IsAccurateAABB() const { return m_bAccurateAABB; }
	void SetAccurateAABB(bool b) {	m_bAccurateAABB = b; }
	const A3DAABB& GetAABB() const { return m_AABB; }
	A3DAABB GetModelAABB();			// aabb including ecmodel and skinmodel
	DWORD GetFileID() const { return m_dwFileID; }
	void SetFileID(DWORD dwFileID) { m_dwFileID = dwFileID; }
	bool IsCloseToGrnd() const { return m_bCloseToGrnd; }
	DWORD GetTimeElapse() const { return m_dwTimeSpan; }
	bool Is2DRender() const { return m_b2DRender; }
	void Set2DRender(bool b) { m_b2DRender = b; m_bZTestEnable = !b; }
	bool Is2DBackLayer() const { return m_b2DBackLayer; }
	void Set2DBackLayer(bool b) { m_b2DBackLayer = b; }
	bool IsZTestEnable() const { return m_bZTestEnable; }
	void SetZTestEnable(bool b) { m_bZTestEnable = b; }
	int GetSfxPriority() const { return m_nSfxPriority; }
	void SetSfxPriority(int n) { m_nSfxPriority = n; }

	bool IsDisableCamShake() const { return m_bDisableCamShake; }
	void SetDisableCamShake(bool b) { m_bDisableCamShake = b; }

	bool IsEnableCamBlur() const { return m_bEnableCamBlur; }
	void SetEnableCamBlur(bool b) { m_bEnableCamBlur = b; }

	clientid_t GetId() const { return m_nId; }
	modelh_t GetHostModel() const { return m_hostModel; }
	void SetId(clientid_t nId, modelh_t hostModel) { m_nId = nId; m_hostModel = hostModel; }
	int GetSkipTime() const { return m_dwSkipTime; }
	void SetSkipTime(DWORD skipTime) { m_dwSkipTime = skipTime; }

	int	GetShakeAffectorType() const { return m_iShakeAffectorType; }
	void SetShakeAffectorType(int iType);
	bool IsCreatedByGFXECM() const { return m_bCreatedByGFXECM; }	// this GFX is created by ECM that is created by a GFX
	void SetCreatedByGFXECM(bool b) { m_bCreatedByGFXECM = b; }
	bool IsAngelica3GFX() const { return m_bIsAngelica3GFX; }		// is this file created in angelica3 gfx editor
	void DeriveParentProperty(A3DGFXEx* pParent);
	void ResumeLoop();
	A3DGFXPhysDataMan * GetPhysDataMan() { return m_pPhysDataMan; }

	void SetShakePeriod( DWORD dwPeriod ); 
	DWORD GetShakePeriod() const;

	void SetShakeByDistance( bool bShakeByDis );
	bool GetShakeByDistance() const;

	ShakeAffector* GetShakeAffector() const { return m_pShakeAffector; }

	// begin multi-thread part
	bool IsResourceReady() const { return m_bResourceReady; }

	void SetResourceReady(bool b)
	{
		if (b)
			m_bResourceReady = true;
		else
		{
			m_bResourceReady = false;
			m_bStateInit = false;
		}
	}

	bool IsLoadingInThread() const { return m_bLoadingInThread; }
	void SetLoadingInThread(bool b) { m_bLoadingInThread = b; }
	bool IsStateInit() const { return m_bStateInit; }

	bool IsFadingOut() const { return m_bFadingOut; }
	void SetFadingOut(bool b);

	// end multi-thread part

	const char* GetPath() const { return m_strFile; }

	bool GetUseParentModelPtr() const { return m_bUseParentModelPtr; }
	void SetUseParentModelPtr(bool b) { m_bUseParentModelPtr = b; }

	bool IsChild() const { return m_bChild; }
};


inline void A3DGFXEx::SetAlphaTransition(float fTargetAlpha, DWORD dwTransTime)
{
	if (dwTransTime == 0) return;
	m_fTargetAlpha = fTargetAlpha;
	m_dwTransTime = dwTransTime;
	m_fDeltaAlpha = (fTargetAlpha - m_fAlpha) / dwTransTime;
}

#endif
