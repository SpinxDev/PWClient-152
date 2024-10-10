#include "StdAfx.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "A3DDecalEx.h"
#include "A3DTrail.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DGFXPhysDataMan.h"
#include "A3DGFXShakeCamera.h"
#include "A3DGFXModel.h"
#include "A3DGFXECModel.h"
#include "A3DGFXModelProxy.h"
#include "GfxRequestQueue.h"

#ifdef A3DPROFILE
#	include "A3DProfile.h"
#else
#	define A3DPROFILE_EVENT( VARIABLE, COLOR, NAME )
#endif

static const char _gfx_file_version[]	= "Version: %d";
static const char _gfx_is_angelica3[]	= "IsAngelica3: %d";
static const char _gfx_load_format[]	= "GFXELEMENTCOUNT: %d";
static const char _gfx_def_scale[]		= "DedaultScale: %f";
static const char _gfx_play_speed[]		= "PlaySpeed: %f";
static const char _gfx_def_alpha[]		= "DefaultAlpha: %f";
static const char _gfx_raytrace[]		= "Raytrace: %d";
static const char _gfx_face_to_viewer[]	= "FaceToViewer: %d";
static const char _gfx_shake_cam[]		= "ShakeCam: %d";
static const char _gfx_vec[]			= "Vec: %f, %f, %f";
static const char _fade_by_dist[]		= "FadeByDist: %d";
static const char _fade_start[]			= "FadeStart: %f";
static const char _fade_end[]			= "FadeEnd: %f";
static const char _no_change_dir[]		= "NoChangeDir: %d";
static const char _2d_render[]			= "2DRender: %d";
static const char _2d_back_layer[]		= "2DBackLayer: %d";
static const char _use_aabb[]			= "UseAABB: %d";
static const char _phys_exist_num[]		= "PhysExist: %d";
static const char _gfx_shake_damping_mode[] = "ShakeDamp: %d";
static const char _gfx_shake_direction[] = "ShakeDir: %d";
static const char _accurate_aabb[]		= "AccurateAABB: %d";
static const char _skip_time[]	= "SkipTime: %d";

/*
 * Version History
 * V2 04.11.16
 * Remark: emitor, scale has min max value
 * V3 04.11.18
 * Remark: add ctrl method, noise scale
 * V4 04.11.18
 * Remark: add ele move bind
 * V5 04.11.22
 * Remark: add bezier curve control
 * V6 04.11.24
 * Remark: lightning add start, end width
 * V7 04.11.25
 * Remark: lightning add alpha start, end
 * V8 04.12.30
 * Remark: can enable or disable ztest
 * V9 05.1.12
 * Remark: all elements can do tex animation
 * V10 05.1.18
 * Remark: particle rand angle initially
 * V11 05.1.31
 * Remark: update cl trans
 * V12 05.2.17
 * Remark: correct cl trans
 * V13 05.2.25
 * Remark: update scale noise
 * V14 05.2.28
 * Remark: update ring
 * V15 05.2.28
 * Remark: update ring
 * V16 05.3.7
 * Remark: add default scale
 * V17 05.3.7
 * Remark: add default play speed
 * V18 05.3.9
 * Remark: add alpha control, trail bind control
 * V19 05.3.10
 * Remark: add gfx model action
 * V20 05.3.15
 * Remark: add particle acc
 * V21 05.3.25
 * Remark: update key point ctrl
 * V22 05.4.15
 * Remark: add match ground
 * V23 05.4.18
 * Remark: add ray trace
 * V24 05.4.21
 * Remark: add shake camera control
 * V25 05.4.28
 * Remark: add bound box
 * V26 05.5.20
 * Remark: only use ground norm
 * V27 05.5.21
 * Remark: fade out by dist
 * V28 05.6.8
 * Remark: add priority ctrl
 * V29 05.7.21
 * Remark: add model alpha compare
 * V30 05.7.26
 * Remark: add scale no offset flag
 * V31 05.8.4
 * Remark: add model action loops
 * V32 05.8.11
 * Remark: add dummy ele
 * V33 05.9.29
 * Remark: add specail element
 * V34 05.11.3
 * Remark: add texture tile mode
 * V35 05.11.28
 * Remark: lightning mid width and alpha
 * V36 05.12.7
 * Remark: update decal scale algorithm
 * V37 05.12.7
 * Remark: update par sys scale algorithm
 * V38 06.2.17
 * Remark: gfx face to viewer
 * V39 06.2.17
 * Remark: lightning use normal
 * V40 06.3.29
 * Remark: add gfx container element type
 * V41 06.4.20
 * Remark: texture uv interchange
 * V42 06.7.14
 * Remark: add 2d decal z offset
 * V43 06.7.17
 * Remark: add par emitter drag prop
 * V44 07.1.12
 * Remark: add curve move dir
 * V45 07.1.30
 * Remark: add render layer
 * V46 07.5.9
 * Remark: add no change dir property
 * V47 07.5.25
 * Remark: add gfx container use outer color flag
 * V48 07.5.31
 * Remark: add drag power
 * V49 07.6.6
 * Remark: model z write enable flag
 * V50 07.6.7
 * Remark: add 2d render
 * V51 07.8.6
 * Remark: add multiplane particle system ref model
 * V52 07.9.12
 * Remark: add multiplane particle use plane normal dir
 * V53 07.10.15
 * Remark: add using aabb flag
 * V54 07.11.12
 * Remark: add match surface
 * V55 07.11.13
 * Remark: optimise match surface
 * V56 07.11.30
 * Remark: gfx container add sub gfx loop flag
 * V57 07.12.6
 * Remark: move match surface property to base class
 * V58 07.12.26
 * Remark: add texture no down sample prop
 * V59 08.8.27
 * Remark: add GridDecalEx : Decal with points editable
 * V60 08.9.11
 * Remark: add ZOffset to GridDecalEx
 * V61 08.9.26
 * Remark: add YawEffect when m_bGroundNormal is set to true on A3DDecalEx
 * V62 08.9.27
 * Remark: add Particles' Init Dir, using this direction to affect the result direction
 * V63 08.10.8
 * Remark: add GFXPhysElement, add A3DGfxPhysDataMan to A3DGFXEx for managing physelements, 
 * And if physelement exists, there would be a same-named PhysDataFile in the same path of .gfx file with EXT: .gphy
 * V64 08.10.16
 * Remark: GFXPhysEmitter add particle width and height parameters
 * V65 08.11.26
 * Remark: 
 *		1. Add class A3DPhysFluidEmitter derived from A3DPhysElement, it would create a APhysXFluidObject in runtime, which will set the particles' positions
 *		2. Add class A3DPhysPointEmitter derived from A3DPhysFluidEmitter : Using A3DPT_POINTLIST to render primitives
 *		3. Add class A3DPhysEmitter derived from A3DPhysFluidEmitter : Using A3DPT_TRIANGLELIST to render primitives and support the dummy objects
 *		4. Add Function RenderPointList to A3DGFXRenderSlot which provide the ability to draw Point Primitives
 *		5. Add Function GetPointSpriteInfo to A3DGFXElement which has a parameter PointSpriteInfo*, and would be called to fill the struct when Point Primitives are being drawed
 *		6. Add struct A3DPOINTVERTEX to A3DGFXStreamMan.h for PointPrimitives' drawing
 *		7. Remove the old version of A3DPhysEmitter which was for experimental purpose from A3DPhysElement.h & A3DPhysElement.cpp
 * V66 08.12.9
 * Remark: Add Average generation method to CurveEmitter
 * V67 08.12.12
 * Remark: Add NewGFXElement : A3DGFXLightningEx which is derived from A3DGFXLightning and provide the appending generation ability
 * V68 08.12.15
 * Remark: Add Tail Fade out to A3DGFXLightningEx (behavior looks like A3DTrail), Using RotList to save the lightning's verts' indice
 * V69 08.12.16
 * Remark: A3DGFXLightningEx : change original tail fade out to is use verts life, add new tail fade out, change a bug which makes the dead verts unremoved
 * V70 08.12.18
 * Remark: A3DParticleEmitter add m_bUseHSVInterpolation, which controls the GenColor's behavior : generate color from RGB or HSV
 * added runtime variables m_hsvMin, m_hsvMax to A3DParticleEmitter which is corresponding to m_colorMin, m_colorMax
 * V71 08.12.24
 * Remark: A3DEllipsoidEmitter add average generate particle mode
 * V72 09.1.19
 * Remark: A3DParticleSystemEx could support A3DParticle's self UV, which is for the texture frame animation, add IsUseParUV to A3DParticleSystemEx
 * V73 09.2.26
 * Remark: A3DGFXLightning Add Lightning Filter to control the lightning's amplitude
 * V74 09.6.3
 * Remark: Fix a bug when the A3DGFXElement's loop to end. it should call the ResumeLoop() in order to do some reset thing (Older version do not call this)
 *		   The problem is there may be some gfx was made rely on this bug. so add m_bResetWhenResumeLoop to A3DGFXElement to let the user decide whether to reset the state.
 * V75 09.7.3
 * Remark: Add m_dwTexTimeTotal to A3DGFXElement, in order to control the max texture animation time, when the time is up, the texture animation would stop at the last frame
 * V76 09.8.17
 * Remark: A3DGFXElement::m_bZEnable before this version all changed no effect. load this value, test the version, if < 76, then set it to true (default).
			Then use this member in all RegisterSlot operation. we make the A3DGFXElement object's z-test property be controled by this value from this version.
 * V77 09.9.1
 * Remark: A3DGFXModel add m_bIsUse3DCamera which could force the the gfxmodel be rendered in a 3D camera while in 2D-rendering.
 * V78 09.9.7
 * Remark: A3DGFXContainer add m_fPlaySpeed for the sub gfx.
 * V79 09.9.11
 * Remark: A3DParticleSystemEx add m_bIsStartWithGroundHeight to make the particle be started using the height of AfxGetGrndNorm();
 * V80 09.12.24
 * Remark: A3DTrail space warp will take effect from now on, version under 80's warp property would be set to false.
 * V81 09.12.28
 * Remark: color trans add alpha only flag means it only affects the alpha part of the color
 * V82 09.12.29
 * Remark: add shake damping affectors (linear, quadratic)
 * V83 10.1.15
 * Remark: add new type of gfx element : A3DGFXECModel
 * V84 10.1.22
 * Remark: add runtime accurate aabb calculation
 * V85 10.3.2
 * Remark: add multiple loading method for A3DGFXECModel (Normal / Custom ECM / Custom Skin)
 * V86 10.4.12
 * Remark: add decal match ground surface use gfx dir property
 * V87 10.4.23
 * Remark: add A3DTrail spline mode
 * V88 10.5.13
 * Remark: A3DGfxSound change to multiple sound files, could be selected randomly
 * V89 10.5.26
 * Remark: From Version 89, we add one m_bIsAngelica3GFX to recognize different type of file
 * V90 10.8.19
 * Remark: Add Shake Period, Shake by Distance
 * V91 11.2.14
 * Remark: Now A3DShader supported
 * V92 11.2.16
 * Remark: add gfx element can do fade out flag, and particle system stop emit when fade out flag
 * V93 11.3.18
 * Remark: Add shader consts loop property
 * V94 11.4.29
 * Remark: Add gfx container use gfx scale when is dummy ele flag
 * V95 11.5.31
 * Remark: Shader info can be loaded from file
 * V96 11.7.27
 * Remark: Add Audio mode to A3DGFXSound
 * V97 11.11.21
 * Remark: Add shaking camera blur
 * V98 11.11.22
 * Remark: add shaking camera radial blur
 * V99 12.1.4
 * Remark: add ParticleSystem init random texture, cylinder emitter segments, gfx model move dir, multiplane emitter smd support
 * V100 12.2.24
 * Remark: add grid decal Affect By Scale, Rot From View
 * V101 12.3.8
 * Remark: add grid decal Use Ground Normal, Offset Height
 * V102	12.4.6
 * Remark: add Lightning amplitude, wave move
 * V103	12.4.9
 * Remark: add gfx ribbon element
 * V104	12.4.12
 * Remark: add gfx element soft edge flag
 * V105 12.4.17
 * Remark: add gfx element hlsl info
 * V106 12.4.19
 * Remark: add gfx proxy model element (use gfx's id to get skin model)
 * V107 12.4.24
 * Remark: add skeleton emitter type particle (use gfx's id to get model skeleton)
 * V108 12.4.24
 * Remark: add particle system z offset
 * V109 12.5.10
 * Remark: element add m_AnimatedProperty
 * V110 12.5.18
 * Remark: model proxy add use static frame property (render skinmodel with the start frame blend matrices)
 * V111 12.6.12
 * Remark: gfx trail change spline mode to new implementation, and add sampler frequency property
 * V112 12.8.5
 * Remark: gfx add skip time when start
 * V113 12.8.28
 * Remark: add new element type: A3DTrailEx
 * V114 12.8.30
 * Remark: add grid decal always on ground property
 * V115 12.11.20
 * Remark: add 2d decal, screen space dimension
 * V116 13.1.11
 * Remark: phys particle, add stop emit when fade out property
 * V117 13.1.16
 * Remark: phys emitter m_affectors load & save
 * V118 13.5.13
 * Remark: gfx element use inner light info
 * V119 13.5.13
 * Remark: light element inner use, not use outside
 * V120 13.7.10
 * Remark: model proxy include children option
 * V121 13.9.3
 * Remark: add shaking camera attenu dist param
 * V122 14.3.7
 * Remark: A3DTrail add following parameters for trail shape perturbation:
 *			m_iPerturbMode,  m_fDisappearSpeed, m_fSpreadSpeed
 * V123 14.3.20
 * Remark: A3DTrail add property: m_bFaceCamera	
 */

#define GFX_CUR_VERSION 123
#define GFX_DEBUG_VERSION_BEGIN 100000		// if fileVersion >= GFX_DEBUG_VERSION_BEGIN && fileVersion%2==0, save file with fileVersion+1
#define GFXEX_CHECK_VISIBLE_TICK 5
#define GFX_MIN_TICK_TIME 2

static const float _match_grnd_max_delta = 0.2f;
extern float _gfx_stat_tick_time;
extern int _gfx_total_count;
extern int _gfx_render_count;

#ifdef _DEBUG
	static bool s_bEnableDumpGfx = true;
#else
	static bool s_bEnableDumpGfx = false;
#endif

void glb_EnableDumpGfx(bool b)
{
	s_bEnableDumpGfx = b;
}

typedef abase::hash_map<A3DGFXEx*, DWORD> DumpGFXMap;
static DumpGFXMap s_GFXMap;

class DumpExistGFXsWrapper
{
private:

	CRITICAL_SECTION m_cs;

public:

	DumpExistGFXsWrapper()
	{
		::InitializeCriticalSection(&m_cs);
	}

	~DumpExistGFXsWrapper()
	{
		::DeleteCriticalSection(&m_cs);
	}

	void Lock()
	{
		::EnterCriticalSection(&m_cs);
	}

	void Unlock()
	{
		::LeaveCriticalSection(&m_cs);
	}
};

static DumpExistGFXsWrapper s_DumpExistGFXsWrapper;

void DumpExistGFXs()
{
	FILE * fp = fopen("dump_gfx.txt", "w");
	if (!fp)
		return;

	s_DumpExistGFXsWrapper.Lock();

	for (DumpGFXMap::iterator itr = s_GFXMap.begin()
		; itr != s_GFXMap.end()
		; ++itr)
	{
		A3DGFXEx* pGFX = itr->first;
		fprintf(fp, "%s 0x%p\n", pGFX->GetPath(), pGFX);
	}

	s_DumpExistGFXsWrapper.Unlock();

	fclose(fp);
}

void LogExistGFXs()
{
	s_DumpExistGFXsWrapper.Lock();
	abase::hash_map<AString, DWORD> sort_map;

	for (DumpGFXMap::iterator itr = s_GFXMap.begin()
		; itr != s_GFXMap.end()
		; ++itr)
	{
		A3DGFXEx* pGFX = itr->first;
		const char* sz = pGFX->GetPath();

		abase::hash_map<AString, DWORD>::iterator itSort = sort_map.find(sz);

		if (itSort != sort_map.end())
			itSort->second ++;
		else
			sort_map[sz] = 1;
	}

	abase::hash_map<AString, DWORD>::iterator itSort = sort_map.begin();

	for (; itSort != sort_map.end(); ++itSort)
	{
		const char* sz = itSort->first;

		if (sz[0])
		{
			a_LogOutput(1, "GFX: %d, %s", itSort->second, sz);
		}
		else
		{
			char buf[1024];
			sprintf(buf, "No name GFX: %d", itSort->second);
			a_LogOutput(1, buf);
		}
	}

	s_DumpExistGFXsWrapper.Unlock();
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DGFXEx
//
//////////////////////////////////////////////////////////////////////////

A3DGFXEx::A3DGFXEx() :
	m_pDev(NULL),
	m_iState(ST_STOP),
	m_dwTimeSpan(0),
	m_fDefPlaySpeed(1.0f),
	m_fDefScale(1.0f),
	m_fDefAlpha(1.0f),
	m_fPlaySpeed(1.0f),
	m_fScale(1.0f),
	m_fAlpha(1.0f),
	m_fActualSpeed(1.0f),
	m_fActualScale(1.0f),
	m_fActualAlpha(1.0f),
	m_bMatchGround(false),
	m_bUseRaytrace(false),
	m_bFaceToViewer(false),
	m_pShakeNoise(NULL),
	m_pShakeAffector(NULL),
	m_iShakeAffectorType(ShakeAffector::SA_NONE),
	m_iShakeType(SHAKECAM_NONE),
	m_pShakeCamera(NULL),
	m_bCalcAABB(false),
	m_vPos(0),
	m_vOldPos(1e6),
	m_vGroundNorm(_unit_y),
	m_fGroundHeight(0),
	m_qDir(_unit_dir),
	m_qMatchGrndDir(_unit_dir),
	m_bTransDirty(true),
	m_AABBOrg(_unit_zero, _unit_zero),
	m_pAABBVerts(NULL),
	m_bUseAABB(true),
	m_bAccurateAABB(false),
	m_bFadeByDist(false),
	m_fFadeStart(10.f),
	m_fFadeEnd(50.f),
	m_bLightVisible(false),
	m_dwFileID(0),
	m_bCloseToGrnd(false),
	m_fDeltaAlpha(0),
	m_fTargetAlpha(0),
	m_dwTransTime(0),
	m_bVisible(true),
	m_fExtent(0),
	m_fDistToCam(0),
	m_bUseLOD(false),
	m_bUseOuterColor(false),
	m_bNoChangeDir(false),
	m_b2DRender(false),
	m_b2DBackLayer(false),
	m_bZTestEnable(true),
	m_bDisableCamShake(false),
	m_bEnableCamBlur(false),
	m_bIsAngelica3GFX(false),
	m_bUseParentModelPtr(false),
	m_nSfxPriority(0),
	m_pPhysDataMan(NULL),
	m_bCreatedByGFXECM(false),
	m_bEmitterStopped(true),
	m_nId(0),
	m_hostModel(NULL),
	m_bChild(false),
	m_pTickTime(NULL),
	m_bFadingOut(false),
	m_bResourceReady(true),
	m_bLoadingInThread(false),
	m_bStateInit(true),
	m_bToRelease(false),
	m_dwSkipTime(0)
{
	m_dwVersion = GFX_CUR_VERSION;
	m_dwRenderTick = rand();
	m_matScale.Identity();
	m_matParent.Identity();
	m_matNoScale.Identity();
	m_AABB.Clear();

#ifdef A3D_PHYSX
	m_pPhysDataMan = new A3DGFXPhysDataMan(this);
#endif
	_gfx_total_count++;

	if (s_bEnableDumpGfx)
	{
		s_DumpExistGFXsWrapper.Lock();
		s_GFXMap[this] = 1;
		s_DumpExistGFXsWrapper.Unlock();
	}
}

A3DGFXEx::~A3DGFXEx()
{
	if (m_bLoadingInThread)
		AfxGetGFXExMan()->CancelGfxMultiThreadLoad(this);

	if (s_bEnableDumpGfx)
	{
		s_DumpExistGFXsWrapper.Lock();
		s_GFXMap.erase(this);
		s_DumpExistGFXsWrapper.Unlock();
	}

	if (m_pAABBVerts)
	{
		delete[] m_pAABBVerts;
		m_pAABBVerts = NULL;
	}

	if (m_pShakeAffector)
	{
		delete m_pShakeAffector;
		m_pShakeAffector = NULL;
	}

	if (m_pShakeNoise)
	{
		delete m_pShakeNoise;
		m_pShakeNoise = NULL;
	}

#ifdef A3D_PHYSX
	if (m_pPhysDataMan)
	{
		delete m_pPhysDataMan;
		m_pPhysDataMan = NULL;
	}
#endif

	assert(!m_EleLst.size());
	A3DGFXExMan* pGfxMan = AfxGetGFXExMan();
	if (NULL != pGfxMan)
	{
		pGfxMan->RemoveFromQueue(this);
	}

	// ensume stop shaking camera
	if (m_pShakeCamera)
		m_pShakeCamera->Stop();

	_gfx_total_count--;

	if ( m_pShakeCamera )
	{
		delete m_pShakeCamera;
		m_pShakeCamera = NULL;
	}
}

bool A3DGFXEx::Init(A3DDevice* pDev)
{
	m_pDev = pDev;

#ifdef A3D_PHYSX
	if (m_pPhysDataMan)
		m_pPhysDataMan->Init(gGetAPhysXScene());
#endif
	return true;
}

bool A3DGFXEx::Release()
{
	if (m_bLoadingInThread)
		AfxGetGFXExMan()->CancelGfxMultiThreadLoad(this);

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->Release();
		delete m_EleLst[i];
	}

	m_EleLst.clear();
	m_EleMap.clear();

#ifdef A3D_PHYSX
	if (m_pPhysDataMan)
		m_pPhysDataMan->Release();
#endif
	
	return true;
}

bool A3DGFXEx::Load(A3DDevice* pDev, AFileImage* pFile)
{
	if (!Init(pDev))
		return false;
	
	int nEleCount = 0;
	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_dwVersion, sizeof(m_dwVersion), &dwReadLen);
		pFile->Read(&m_fDefScale, sizeof(m_fDefScale), &dwReadLen);
		pFile->Read(&m_fDefPlaySpeed, sizeof(m_fDefPlaySpeed), &dwReadLen);
		pFile->Read(&m_fDefAlpha, sizeof(m_fDefAlpha), &dwReadLen);
		pFile->Read(&m_bUseRaytrace, sizeof(m_bUseRaytrace), &dwReadLen);
		pFile->Read(&m_bFaceToViewer, sizeof(m_bFaceToViewer), &dwReadLen);
		pFile->Read(&m_bFadeByDist, sizeof(m_bFadeByDist), &dwReadLen);
		pFile->Read(&m_fFadeStart, sizeof(m_fFadeStart), &dwReadLen);
		pFile->Read(&m_fFadeEnd, sizeof(m_fFadeEnd), &dwReadLen);
		pFile->Read(m_AABBOrg.Mins.m, sizeof(m_AABBOrg.Mins.m), &dwReadLen);
		pFile->Read(m_AABBOrg.Maxs.m, sizeof(m_AABBOrg.Maxs.m), &dwReadLen);
		//pFile->Read(&m_bShakeCam, sizeof(m_bShakeCam), &dwReadLen);

		if (GetShakeCam())
		{
			m_pShakeNoise = static_cast<A3DGFXCtrlNoiseBase*>(A3DGFXKeyPointCtrlBase::LoadFromFile(pFile, m_dwVersion));

			if (!m_pShakeNoise)
			{
				Release();
				return false;
			}

			m_pShakeNoise->Resume();
		}

		pFile->Read(&nEleCount, sizeof(nEleCount), &dwReadLen);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];
		int nRead;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _gfx_file_version, &m_dwVersion);

		if (m_dwVersion >= 89)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_is_angelica3, &nRead);
			m_bIsAngelica3GFX = (nRead != 0);
			
			if (m_bIsAngelica3GFX)
			{
				a_LogOutput(1, "ERROR! Try to load angelica 3 gfx using the angelica2 gfx library.");
				return false;
			}
		}

		if (m_dwVersion >= 16)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_def_scale, &m_fDefScale);
		}

		if (m_dwVersion >= 17)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_play_speed, &m_fDefPlaySpeed);
		}

		if (m_dwVersion >= 18)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_def_alpha, &m_fDefAlpha);
		}

		if (m_dwVersion >= 23)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_raytrace, &nRead);
			m_bUseRaytrace = (nRead != 0);
		}
		else
			m_bUseRaytrace = false;

		if (m_dwVersion >= 38)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_face_to_viewer, &nRead);
			m_bFaceToViewer = (nRead != 0);
		}

		if (m_dwVersion >= 27)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _fade_by_dist, &nRead);
			m_bFadeByDist = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _fade_start, &m_fFadeStart);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _fade_end, &m_fFadeEnd);
		}

		if (m_dwVersion >= 25)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_vec, VECTORADDR_XYZ(m_AABBOrg.Mins));

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_vec, VECTORADDR_XYZ(m_AABBOrg.Maxs));
		}

		if (m_dwVersion >= 53)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _use_aabb, &nRead);
			m_bUseAABB = (nRead != 0);
		}

		if (m_dwVersion >= 84)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _accurate_aabb, &nRead);
			m_bAccurateAABB = (nRead != 0);
		}

		if (m_dwVersion >= 24)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_shake_cam, &m_iShakeType);

			if (GetShakeCam())
			{
				m_pShakeNoise = static_cast<A3DGFXCtrlNoiseBase*>(A3DGFXKeyPointCtrlBase::LoadFromFile(pFile, m_dwVersion));

				if (!m_pShakeNoise)
				{
					Release();
					return false;
				}

				m_pShakeNoise->Resume();
			}
		}

		if (GetShakeCam())
		{
			m_pShakeCamera = IShakeCamera::LoadFromFile(pFile, m_dwVersion, this);
			
			if (!m_pShakeCamera)
			{
				Release();
				return false;
			}
		}

		if (m_dwVersion >= 82)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _gfx_shake_damping_mode, &nRead);
			m_iShakeAffectorType = nRead;
			if (m_pShakeAffector)
				delete m_pShakeAffector;

			m_pShakeAffector = ShakeAffector::CreateAffector(m_iShakeAffectorType, m_pShakeNoise);
		}

		if (m_dwVersion >= 46)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _no_change_dir, &nRead);
			m_bNoChangeDir = (nRead != 0);
		}

		if (m_dwVersion >= 50)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _2d_render, &nRead);
			m_b2DRender = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _2d_back_layer, &nRead);
			m_b2DBackLayer = (nRead != 0);

			m_bZTestEnable = !m_b2DRender;
		}

		if (m_dwVersion >= 112)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _skip_time, &m_dwSkipTime);
		}

		if (m_dwVersion >= 63)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _phys_exist_num, &nRead);
			bool bIsPhysExist = (nRead != 0);

#ifdef A3D_PHYSX
			if (bIsPhysExist)
			{
				AString strPhysDataFile = pFile->GetFileName();
				strPhysDataFile = A3DGFXPhysDataMan::GfxFileExt2PhysFileExt(strPhysDataFile);
				if (!m_pPhysDataMan->LoadPhys(strPhysDataFile))
				{
					a_LogOutput(1, "A3DGFXEx::Load, Failed to load phys file %s", strPhysDataFile);
					return false;
				}
			}
#endif
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _gfx_load_format, &nEleCount);
	}

	// 判断版本
	if (GFX_CUR_VERSION < m_dwVersion && m_dwVersion < GFX_DEBUG_VERSION_BEGIN)
	{
		Release();
		return false;
	}

	m_AABBOrg.CompleteCenterExts();
	m_fExtent = m_AABBOrg.Extents.Magnitude();

	if (m_AABBOrg.Mins.x > m_AABBOrg.Maxs.x ||
		m_AABBOrg.Mins.y > m_AABBOrg.Maxs.y ||
		m_AABBOrg.Mins.z > m_AABBOrg.Maxs.z)
	{
		m_bUseAABB = false;
		m_bAccurateAABB = false;
	}

#ifndef GFX_EDITOR

	if (m_bUseAABB && m_bAccurateAABB)
	{
		m_pAABBVerts = new A3DVECTOR3[8];
		const A3DVECTOR3& v1 = m_AABBOrg.Mins;
		const A3DVECTOR3& v2 = m_AABBOrg.Maxs;
		m_pAABBVerts[0].Set(v1.x, v1.y, v1.z);
		m_pAABBVerts[1].Set(v1.x, v2.y, v1.z);
		m_pAABBVerts[2].Set(v1.x, v1.y, v2.z);
		m_pAABBVerts[3].Set(v1.x, v2.y, v2.z);
		m_pAABBVerts[4].Set(v2.x, v1.y, v1.z);
		m_pAABBVerts[5].Set(v2.x, v1.y, v2.z);
		m_pAABBVerts[6].Set(v2.x, v2.y, v1.z);
		m_pAABBVerts[7].Set(v2.x, v2.y, v2.z);
	}

#endif

	for (int i = 0; i < nEleCount; i++)
	{
		A3DGFXElement* pEle = A3DGFXElement::LoadElementFromFile(m_pDev, this, pFile, m_dwVersion);

		if (pEle)
		{
			if (pEle->UseGroundNormal() || pEle->UseGroundHeight())
				m_bMatchGround = true;

			AddGFXElement(pEle);
		}
#ifdef GFX_EDITOR
		// 如果pEle未能成功Load
		// 如果是编辑器中，则报错
		// 否则跳过
		// 编辑器打开gfx的情况下 如果gfx中包含物理粒子 而物理引擎没有被初始化
		// 则报错
		// 非编辑器的情况下 如果gfx中包含物理粒子 而物理引擎没有被初始化
		// 则跳过该物理粒子
		else
		{
			Release();
			return false;
		}
#endif
	}

	UpdateBindEle();
	Stop(false);

	if (m_bResourceReady)
	{
		SetPlaySpeed(1.0f);
		SetScale(1.0f);
		SetAlpha(1.0f);
	}

	return true;
}

bool A3DGFXEx::Load(A3DDevice* pDev, const char* szFile)
{
	AFileImage file;
	if (!file.Open("", szFile, AFILE_OPENEXIST | AFILE_TEMPMEMORY))
	{
		//gfx is exist? 
#ifdef _ANGELICA21
		glb_GetGfxRequestQueue()->PushGfxResRequest(AString(szFile));
#endif
		return false;
	}

	if (!Load(pDev, &file))
	{
		file.Close();
		return false;
	}

	file.Close();
	
	m_strFile = szFile;
	return true;
}

bool A3DGFXEx::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		DWORD dwVersion = GFX_CUR_VERSION;

		pFile->Write(&dwVersion, sizeof(dwVersion), &dwWrite);
		pFile->Write(&m_fDefScale, sizeof(m_fDefScale), &dwWrite);
		pFile->Write(&m_fDefPlaySpeed, sizeof(m_fDefPlaySpeed), &dwWrite);
		pFile->Write(&m_fDefAlpha, sizeof(m_fDefAlpha), &dwWrite);
		pFile->Write(&m_bUseRaytrace, sizeof(m_bUseRaytrace), &dwWrite);
		pFile->Write(&m_bFaceToViewer, sizeof(m_bFaceToViewer), &dwWrite);
		pFile->Write(&m_bFadeByDist, sizeof(m_bFadeByDist), &dwWrite);
		pFile->Write(&m_fFadeStart, sizeof(m_fFadeStart), &dwWrite);
		pFile->Write(&m_fFadeEnd, sizeof(m_fFadeEnd), &dwWrite);
		pFile->Write(m_AABBOrg.Mins.m, sizeof(m_AABBOrg.Mins.m), &dwWrite);
		pFile->Write(m_AABBOrg.Maxs.m, sizeof(m_AABBOrg.Maxs.m), &dwWrite);
		//pFile->Write(&m_bShakeCam, sizeof(m_bShakeCam), &dwWrite);
		if (GetShakeCam()) m_pShakeNoise->SaveToFile(pFile);
		size_t sz = m_EleLst.size();
		pFile->Write(&sz, sizeof(sz), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		DWORD dwVersion = GFX_CUR_VERSION;
		if (m_dwVersion >= GFX_DEBUG_VERSION_BEGIN)		// debug version, set an odd version number
			dwVersion = m_dwVersion%2==0 ? m_dwVersion+1 : m_dwVersion;

		sprintf(szLine, _gfx_file_version, dwVersion);
		pFile->WriteLine(szLine);

		//	不论我们读取进来的文件是由哪个版本的引擎下的编辑器创建的，用本版本的编辑器存储时都做如此标记
		sprintf(szLine, _gfx_is_angelica3, (int)FALSE);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_def_scale, m_fDefScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_play_speed, m_fDefPlaySpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_def_alpha, m_fDefAlpha);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_raytrace, (int)m_bUseRaytrace);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_face_to_viewer, (int)m_bFaceToViewer);
		pFile->WriteLine(szLine);

		sprintf(szLine, _fade_by_dist, (int)m_bFadeByDist);
		pFile->WriteLine(szLine);

		sprintf(szLine, _fade_start, m_fFadeStart);
		pFile->WriteLine(szLine);

		sprintf(szLine, _fade_end, m_fFadeEnd);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_vec, VECTOR_XYZ(m_AABBOrg.Mins));
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_vec, VECTOR_XYZ(m_AABBOrg.Maxs));
		pFile->WriteLine(szLine);

		sprintf(szLine, _use_aabb, m_bUseAABB);
		pFile->WriteLine(szLine);

		sprintf(szLine, _accurate_aabb, m_bAccurateAABB);
		pFile->WriteLine(szLine);

		sprintf(szLine, _gfx_shake_cam, (int)m_iShakeType);
		pFile->WriteLine(szLine);

		if (GetShakeCam()) m_pShakeNoise->SaveToFile(pFile);
		if (GetShakeCam()) IShakeCamera::SaveToFile(m_pShakeCamera, pFile);

		_snprintf(szLine, sizeof(szLine), _gfx_shake_damping_mode, m_iShakeAffectorType);
		pFile->WriteLine(szLine);

		sprintf(szLine, _no_change_dir, m_bNoChangeDir);
		pFile->WriteLine(szLine);

		sprintf(szLine, _2d_render, m_b2DRender);
		pFile->WriteLine(szLine);

		sprintf(szLine, _2d_back_layer, m_b2DBackLayer);
		pFile->WriteLine(szLine);

		sprintf(szLine, _skip_time, m_dwSkipTime);
		pFile->WriteLine(szLine);

		// added in version 63
		// when m_pPhysDataMan is invalid, just take the number as 0
		sprintf(szLine, _phys_exist_num, m_pPhysDataMan ? m_pPhysDataMan->GetPhysEleNum() : 0);
		pFile->WriteLine(szLine);
		
		sprintf(szLine, _gfx_load_format, m_EleLst.size());
		pFile->WriteLine(szLine);
	}

	size_t i;

	for (i = 0; i < m_EleLst.size(); i++)
		if (!m_EleLst[i]->GetBindEle())
			m_EleLst[i]->SaveElementToFile(pFile);

	for (i = 0; i < m_EleLst.size(); i++)
		if (m_EleLst[i]->GetBindEle())
			m_EleLst[i]->SaveElementToFile(pFile);


#ifdef A3D_PHYSX
	if (m_pPhysDataMan && m_pPhysDataMan->GetPhysEleNum())
	{
		AString strPhysDataFile = pFile->GetFileName();
		strPhysDataFile = A3DGFXPhysDataMan::GfxFileExt2PhysFileExt(strPhysDataFile);
		m_pPhysDataMan->SavePhys(strPhysDataFile);
	}
#endif

	return true;
}

A3DGFXEx* A3DGFXEx::Clone() const
{
	A3DGFXEx* pGfx = new A3DGFXEx();
	*pGfx = *this;
	return pGfx;
}

A3DGFXEx& A3DGFXEx::operator = (const A3DGFXEx& src)
{
	if (&src == this)
		return *this;

	if (!src.m_bResourceReady)
	{
		assert(false);
		return *this;
	}

#ifndef _ANGELICA21
	Release();
#endif

	Init(src.m_pDev);

	for (size_t i = 0; i < src.m_EleLst.size(); i++)
	{
		if (NULL == src.m_EleLst[i])
			continue;
		AddGFXElement(src.m_EleLst[i]->Clone(this));
	}

	m_fDefPlaySpeed	= src.m_fDefPlaySpeed;
	m_fDefScale		= src.m_fDefScale;
	m_fDefAlpha		= src.m_fDefAlpha;

	if (m_bResourceReady)
	{
		SetPlaySpeed(1.0f);
		SetScale(1.0f);
		SetAlpha(1.0f);
	}

	m_bMatchGround = src.m_bMatchGround;
	m_bUseRaytrace = src.m_bUseRaytrace;
	m_bFaceToViewer = src.m_bFaceToViewer;
	m_bNoChangeDir = src.m_bNoChangeDir;
	m_b2DRender = src.m_b2DRender;
	m_b2DBackLayer = src.m_b2DBackLayer;
	m_bZTestEnable = !m_b2DRender;
	m_iShakeType = src.m_iShakeType;
	m_dwSkipTime = src.m_dwSkipTime;

	if (GetShakeCam())
	{
		//	不知道自己是不是已经有这个对象了，如果有了，应该删掉
		if (m_pShakeNoise)
			delete m_pShakeNoise;

		ASSERT( src.m_pShakeNoise );
		m_pShakeNoise = new A3DGFXCtrlNoiseBase();
		m_pShakeNoise->_CloneBase(src.m_pShakeNoise);
		
		if (m_pShakeCamera)
			delete m_pShakeCamera;

		ASSERT( src.m_pShakeCamera );
		m_pShakeCamera = src.m_pShakeCamera->Clone(this);
	}

	m_AABBOrg		= src.m_AABBOrg;
	m_bUseAABB		= src.m_bUseAABB;
	m_bAccurateAABB	= src.m_bAccurateAABB;
	m_fExtent		= src.m_fExtent;
	m_bFadeByDist	= src.m_bFadeByDist;
	m_fFadeStart	= src.m_fFadeStart;
	m_fFadeEnd		= src.m_fFadeEnd;
	m_dwFileID		= src.m_dwFileID;
	m_bDisableCamShake = src.m_bDisableCamShake;
	m_bEnableCamBlur = src.m_bEnableCamBlur;
	m_iShakeAffectorType = src.m_iShakeAffectorType;

	if (src.m_pAABBVerts)
	{
		if (!m_pAABBVerts)
			m_pAABBVerts = new A3DVECTOR3[8];

		memcpy(m_pAABBVerts, src.m_pAABBVerts, sizeof(A3DVECTOR3) * 8);
	}
	else
	{
		if (m_pAABBVerts)
		{
			delete m_pAABBVerts;
			m_pAABBVerts = NULL;
		}
	}

	if (m_pShakeAffector)
		delete m_pShakeAffector;
		
	m_pShakeAffector = ShakeAffector::CreateAffector(m_iShakeAffectorType, m_pShakeNoise);

	m_strFile = src.m_strFile;
	m_pTickTime = src.m_pTickTime;
	UpdateBindEle();
	return *this;
}

void A3DGFXEx::AddNewEleMap(A3DGFXElement* pEle)
{
	if (strcmp(pEle->GetName(), "") == 0)
	{
		AString strName;
		strName.Format("Unnamed%d", A3DGFXElement::m_nUnnamedCount++);
		pEle->SetName(strName);
	}

	while (true)
	{
		if (m_EleMap.find(AString(pEle->GetName())) == m_EleMap.end()) break;
		AString strName;
		strName.Format("Unnamed%d", A3DGFXElement::m_nUnnamedCount++);
		pEle->SetName(strName);
	}

	m_EleMap[AString(pEle->GetName())] = pEle;
}

void A3DGFXEx::UpdateBindEle()
{
	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		A3DGFXElement*& pEle = m_EleLst[i];

		if (!pEle->GetBindEleName().IsEmpty())
			pEle->SetBindEle(GetElementByName(pEle->GetBindEleName()));

		if (!pEle->GetDummyName().IsEmpty())
			pEle->SetDummyEle(GetElementByName(pEle->GetDummyName()));
	}
}

// GFXEditor interface
A3DGFXElement* A3DGFXEx::AddEmptyElement(int nEleId)
{
#ifndef _ANGELICA21

	if (nEleId == ID_ELE_TYPE_LIGHT)
	{
		for (size_t i = 0; i < m_EleLst.size(); i++)
		{
			if (NULL == m_EleLst[i])
				continue;
			if (m_EleLst[i]->GetEleTypeId() == ID_ELE_TYPE_LIGHT)
				return NULL;
		}
	}

#endif

	A3DGFXElement* pEle = A3DGFXElement::CreateEmptyElement(m_pDev, this, nEleId);
	if (pEle)
	{
		pEle->SetName("");
		AddGFXElement(pEle);
	}
	return pEle;
}

void A3DGFXEx::AddGFXElement(A3DGFXElement* pEle)
{
	if (NULL == pEle)
		return;
	m_EleLst.push_back(pEle);
	AddNewEleMap(pEle);
	
#ifdef A3D_PHYSX
	if (m_pPhysDataMan && pEle->IsPhysEle())
		m_pPhysDataMan->RegisterElement(pEle);
#endif
	
}

void A3DGFXEx::RemoveElement(A3DGFXElement* pEle)
{
#ifdef A3D_PHYSX
	if (m_pPhysDataMan && pEle->IsPhysEle())
		m_pPhysDataMan->RemoveElement(pEle);
#endif

	m_EleMap.erase(AString(pEle->GetName()));
	int iToDel = -1;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i] && m_EleLst[i] != pEle)
			continue;
		if (m_EleLst[i] == pEle)
			iToDel = i;
		else if (m_EleLst[i]->GetBindEleName() == pEle->GetName())
			m_EleLst[i]->SetBindEle(NULL);
		else if (m_EleLst[i]->GetDummyEle() == pEle)
			m_EleLst[i]->SetDummyEle(NULL);
	}

	if (iToDel >= 0)
	{
		m_EleLst[iToDel]->Release();
		delete m_EleLst[iToDel];
		m_EleLst.erase(m_EleLst.begin()+iToDel);
	}
}

bool A3DGFXEx::ChangeEleName(A3DGFXElement* pEle, AString strNewName)
{
	if (m_EleMap.find(strNewName) != m_EleMap.end()) return false;
	m_EleMap.erase(AString(pEle->GetName()));
	pEle->SetName(strNewName);
	m_EleMap[strNewName] = pEle;
	return true;
}

void A3DGFXEx::UpdateEleParam(const AString& strEleName, int nParamId, const GFX_PROPERTY& gp)
{
	if (!m_bResourceReady)
		return;

	EleMap::iterator it = m_EleMap.find(strEleName);
	if (it == m_EleMap.end()) return;
	it->second->UpdateParam(nParamId, gp);
}

void A3DGFXEx::GetParamInfoList(GfxParamList& ParamInfoList)
{
	if (!m_bResourceReady)
		return;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		A3DGFXElement* pEle = m_EleLst[i];
		if (NULL == pEle) continue;
		const GFX_PARAM_INFO* pInfo = gfx_param_info(pEle->GetEleTypeId());
		if (pInfo == NULL) continue;
		for (int j = 0; j < pInfo->m_nPropCount; j++)
		{
			const GFX_PROP_INFO* pSub = &pInfo->m_pPropInfo[j];
			if (pEle->IsParamEnable(pSub->m_OpId))
			{
				GfxParamInfo* p = new GfxParamInfo;
				p->m_nParamId = pSub->m_OpId;
				p->m_strEleName = pEle->GetName();
				p->m_eumType = pEle->GetParamType(pSub->m_OpId);
				ParamInfoList.push_back(p);
			}
		}
	}
}

bool A3DGFXEx::IsParamEnable(const AString& strEleName, int nParamId) const
{
	if (!m_bResourceReady)
		return true;

	EleMap::const_iterator it = m_EleMap.find(strEleName);
	if (it == m_EleMap.end()) return false;
	return it->second->IsParamEnable(nParamId);
}


bool A3DGFXEx::IsTexUsed(const char* szTex)
{
	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		AString strTex = "Gfx\\Textures\\" + m_EleLst[i]->GetTexPath();
		if (strTex.CompareNoCase(szTex) == 0)
			return true;

		const AString& strShaderTex = m_EleLst[i]->GetShaderTexture();
		if (!strShaderTex.IsEmpty())
		{
			strTex = "Gfx\\Textures\\" + strShaderTex;
			if (strTex.CompareNoCase(szTex) == 0)
				return true;
		}

		if (m_EleLst[i]->GetHLSLInfo().HasPixelShader())
		{
			A3DHLPixelShader* pHLPS = m_EleLst[i]->GetHLSLInfo().PixelShader();
			std::vector<AString> samplerNames;
			pHLPS->ListSamplers(&samplerNames);
			for (std::vector<AString>::const_iterator itr = samplerNames.begin()
				; itr != samplerNames.end()
				; ++itr)
			{
				const AString& strTex = pHLPS->GetTextureFile(*itr);
				if (strTex.CompareNoCase(szTex) == 0)
					return true;
			}
		}
	}

	return false;
}

bool A3DGFXEx::Pause()
{
	m_iState = ST_PAUSE;

	if (!m_bResourceReady)
		return true;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->Pause();
	}

	return true;
}

bool A3DGFXEx::Stop(bool bResetState)
{
	if (bResetState)
		m_iState = ST_STOP;

	if (!m_bResourceReady)
		return true;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->Stop();
	}

	if (m_pShakeNoise)
		m_pShakeCamera->Stop();

	m_bLightVisible = false;
	m_fDeltaAlpha = 0;
	m_fTargetAlpha = 0;
	m_dwTransTime = 0;
	m_bUseOuterColor = false;
	m_nSfxPriority = 0;
	m_bEmitterStopped = true;

	return true;
}

void A3DGFXEx::StopParticleEmit()
{
	if (!m_bResourceReady)
		return;

	for (size_t i = 0; i < m_EleLst.size(); ++i)
	{
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->StopParticleEmit();
	}

	m_bEmitterStopped = true;
}

void A3DGFXEx::SetFadingOut(bool b)
{
	m_bFadingOut = b;

	if (!m_bResourceReady)
		return;

	if (b)
	{
		for (size_t i = 0; i < m_EleLst.size(); ++i)
		{
			if (NULL == m_EleLst[i])
				continue;
			m_EleLst[i]->DoFadeOut();
		}
	}
}

void A3DGFXEx::SetToEditMode()
{
	m_iState = ST_EDITMODE;
	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->SetToEditMode();
	}
}

bool A3DGFXEx::IsInfinite() const
{
	if (!m_bResourceReady)
		return true;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		if (m_EleLst[i]->IsInfinite())
			return true;
	}
	return false;
}

void A3DGFXEx::UpdateAllTexs()
{
	size_t i;

	for (i = 0; i < m_EleLst.size(); i++)
	{
		A3DGFXElement* pEle = m_EleLst[i];
		if (NULL == pEle)
			continue;
		pEle->ReleaseTex();
	}

	AfxGetGFXTexMan().Release();

	for (i = 0; i < m_EleLst.size(); i++)
	{
		A3DGFXElement* pEle = m_EleLst[i];
		if (NULL == pEle)
			continue;
		pEle->UpdateTex();
	}
}

bool A3DGFXEx::TickAnimation(DWORD dwTickTime)
{
	AGPA_RECORD_FUNCTION_TIME;
	if (!m_bResourceReady || m_iState != ST_PLAY)
		return true;

#ifdef FUNCTION_ANALYSER
	ATTACH_FUNCTION_ANALYSER(true, 5, 0, m_strFile);
#endif

	if (!m_bStateInit)
	{
		m_bStateInit = true;
		Start(false);
		SetPlaySpeed(m_fPlaySpeed);
		SetScale(m_fScale);
		SetAlpha(m_fAlpha);
	}

#ifdef GFX_STAT
	__int64 _start, _end;
	_start = ACounter::GetMicroSecondNow();
#endif

	if (m_dwTransTime > dwTickTime)
	{
		m_dwTransTime -= dwTickTime;
		m_fAlpha += m_fDeltaAlpha * dwTickTime;
		CalcActualAlpha();
	}
	else if (m_dwTransTime)
	{
		m_fAlpha = m_fTargetAlpha;
		m_dwTransTime = 0;
		CalcActualAlpha();
	}

	A3DCamera* pCamera = AfxGetA3DCamera();
	A3DVECTOR3 vView = pCamera->GetPos() - GetPos();
	m_fDistToCam = vView.Normalize();

	if (m_bFaceToViewer)
	{
		m_matNoScale = _build_matrix(vView, m_vPos);
		m_qDir.ConvertFromMatrix(m_matNoScale);
		m_bTransDirty = true;
	}

	if (m_bTransDirty)
	{
		CalcParentTM();
		m_bTransDirty = false;
	}

	if (dwTickTime)
	{
		dwTickTime = static_cast<DWORD>(dwTickTime * m_fActualSpeed);

		if (dwTickTime < GFX_MIN_TICK_TIME)
			dwTickTime = GFX_MIN_TICK_TIME;
	}

	m_dwTimeSpan += dwTickTime;

	if (!m_bDisableCamShake && GetShakeCam())
	{
		ASSERT( m_pShakeNoise );
		if (NULL != m_pShakeCamera)
		{
			m_pShakeCamera->SetEnableCamBlur(m_bEnableCamBlur);
			m_pShakeCamera->Tick(dwTickTime, m_dwTimeSpan, pCamera, m_fDistToCam);
		}
	}

	if (TickElements(dwTickTime))
		Stop();

#ifdef GFX_STAT
	_end = ACounter::GetMicroSecondNow();
	__int64 _delta = (_end - _start);

	if (m_pTickTime && !m_bChild)
	{
		m_pTickTime->dwCount++;
		m_pTickTime->dwTickTime += (DWORD)_delta;
	}

	float _tick_time = _delta * 0.001f;
	_gfx_stat_tick_time += _tick_time;
#endif

	return true;
}

bool A3DGFXEx::TickElements(DWORD dwTickTime)
{
	bool bFinished = true;
	size_t ele_count = m_EleLst.size();
	size_t i;

	if (m_bFadingOut)
	{
		for (i = 0; i < ele_count; i++)
		{
			A3DGFXElement* pEle = m_EleLst[i];
			if (NULL == pEle)
				continue;
			if (pEle->IsDummyEle() || !pEle->CanDoFadeOut())
				continue;

			pEle->TickAnimation(dwTickTime);

			if (!pEle->IsFinished())
				bFinished = false;
		}
	}
	else
	{
		for (i = 0; i < ele_count; i++)
		{
			A3DGFXElement* pEle = m_EleLst[i];
			if (NULL == pEle)
				continue;
			if (pEle->IsDummyEle())
				continue;
			
			pEle->TickAnimation(dwTickTime);
			
			if (!pEle->IsFinished())
				bFinished = false;
		}
	}

	return bFinished;
}

bool A3DGFXEx::Render(A3DViewport* pView)
{
	A3DPROFILE_EVENT( GFXExRender, 0xFF3030, L"GFXExRender" );
	AGPA_RECORD_FUNCTION_TIME;
	if (!m_bStateInit)
		return true;

#ifndef GFX_EDITOR

	if (m_iState == ST_STOP)
		return true;

#endif

#ifdef FUNCTION_ANALYSER
	ATTACH_FUNCTION_ANALYSER(true, 5, 0, m_strFile);
#endif

	m_dwRenderTick++;

	if (m_bFadeByDist)
	{
		float fDist = (m_pDev->GetA3DEngine()->GetActiveCamera()->GetPos() - m_vPos).Magnitude();
		float fAlpha = (m_fFadeEnd - fDist) / (m_fFadeEnd - m_fFadeStart);
		a_Clamp(fAlpha, 0.0f, 1.0f);
		SetAlpha(fAlpha);
	}

	if (m_fActualAlpha <= 0.05f)
		return true;

	if (m_bUseAABB && !pView->GetCamera()->AABBInViewFrustum(GetAABB()))
	{
		m_bLightVisible = false;
		return true;
	}

	_gfx_render_count++;
	int nPriority = AfxGetGFXExMan()->GetPriority();
	size_t ele_count = m_EleLst.size();
	size_t i;

	if (m_bFadingOut)
	{
		for (i = 0; i < ele_count; i++)
		{
			A3DGFXElement* pEle = m_EleLst[i];
			if (NULL == pEle)
				continue;
			if (pEle->IsDummyEle() || pEle->GetPriority() > nPriority || !pEle->CanDoFadeOut())
				continue;

			if (pEle->IsVisible())
				pEle->Render(pView);
		}
	}
	else
	{
		for (i = 0; i < ele_count; i++)
		{
			A3DGFXElement* pEle = m_EleLst[i];
			if (NULL == pEle)
				continue;
			if (pEle->IsDummyEle() || pEle->GetPriority() > nPriority)
				continue;

			if (pEle->IsVisible())
				pEle->Render(pView);
		}
	}

	return true;
}

/*
void A3DGFXEx::CalcShakeOffset()
{
	// If already shaking, then user set a disable camera shake
	// we must let shaking stops here
	const float fTimeSpan = m_dwTimeSpan / 1000.f;
	if (!GetShakeCam()
	 || !m_pShakeNoise
	 || (!m_pShakeNoise->Tick(fTimeSpan)) )
	{
		StopShaking();
		return;
	}

	if (!m_bShaking)
	{
		m_bShaking = true;
		m_nShakeCount = 0;
		AfxBeginShakeCam();
	}
	
	if ((m_nShakeCount++ % 2) == 0)
	{
		if (m_fDistToCam >= _shake_attenu_dist)
			m_vShakeOffset.Clear();
		else
		{
			const A3DVECTOR3 v = GenerateDirection(static_cast<SHAKECAM>(m_iShakeType));
			const float fCo = 1.0f - m_fDistToCam / _shake_attenu_dist;
			m_vShakeOffset = (m_pShakeNoise->GetNoiseVal(m_dwTimeSpan) * fCo) * v;
		}
		
		if (m_pShakeAffector)
			m_pShakeAffector->AffectShakeOffset(fTimeSpan, m_vShakeOffset);
	}
	else
		m_vShakeOffset = -m_vShakeOffset;
	
	AfxSetCamOffset(m_vShakeOffset);

}

void A3DGFXEx::StopShaking()
{
	if (!m_bShaking) return;
	if ((m_nShakeCount % 2) != 0) AfxSetCamOffset(-m_vShakeOffset);
	AfxEndShakeCam();
	
	m_bShaking = false;
}
*/

//	When Sub elements' property (GroundNormal, GroundHeight) changed, then this property should be updated
//	This function would only be called in editor(through A3DGFXElement::SetProperty)
void A3DGFXEx::UpdateMatchGround()
{
	if (!m_bResourceReady)
		return;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		A3DGFXElement* pEle = m_EleLst[i];

		if (!pEle)
			continue;

		if (pEle->UseGroundNormal() || pEle->UseGroundHeight())
			m_bMatchGround = true;
	}
}

void A3DGFXEx::Reset()
{
	m_dwVersion = GFX_CUR_VERSION;
	m_fDefPlaySpeed	= 1.0f;
	m_fDefScale		= 1.0f;
	m_fDefAlpha		= 1.0f;
	m_fPlaySpeed	= 1.0f;
	m_fScale		= 1.0f;
	m_fAlpha		= 1.0f;
	m_fActualSpeed	= 1.0f;
	m_fActualScale	= 1.0f;
	m_fActualAlpha	= 1.0f;
	m_bFaceToViewer	= false;
	m_bMatchGround	= false;
	m_bUseRaytrace	= false;
	m_iShakeType	= SHAKECAM_NONE;
	m_bNoChangeDir	= false;
	m_b2DRender		= false;
	m_b2DBackLayer	= false;
	m_bZTestEnable	= true;
	m_nSfxPriority	= 0;
	m_dwSkipTime = 0;
	
	if (m_pShakeNoise)
	{
		delete m_pShakeNoise;
		m_pShakeNoise = NULL;
	}
	
	m_iShakeAffectorType = ShakeAffector::SA_NONE;
	if (m_pShakeAffector)
	{
		delete m_pShakeAffector;
		m_pShakeAffector = NULL;
	}

	m_bFadeByDist = false;
	m_fFadeStart = 0;
	m_fFadeEnd = 0;
	m_bLightVisible = false;
	m_matGroundNormal.Identity();
	m_matGroundHeight.Identity();

	if (m_pShakeCamera)
		m_pShakeCamera->Reset();
}

void A3DGFXEx::StartCalcAABB()
{
	m_AABBOrg.Clear();

	m_bCalcAABB = true;
}

void A3DGFXEx::EndCalcAABB()
{
	m_bCalcAABB = false;
	A3DAABB aabb(m_AABBOrg.Mins, m_AABBOrg.Maxs);
	m_AABBOrg = aabb;
}

void A3DGFXEx::SetShakeAffectorType(int iType)
{
	m_iShakeAffectorType = iType;
	if (m_pShakeAffector)
		delete m_pShakeAffector;

	m_pShakeAffector = ShakeAffector::CreateAffector(iType, m_pShakeNoise);
} 

bool A3DGFXEx::GetShakeCam() const
{
	return m_iShakeType != SHAKECAM_NONE;
}

void A3DGFXEx::SetShakeCam(bool bShake)
{
	SetShakeCamType(bShake ? SHAKECAM_DIR_RAND : SHAKECAM_NONE);
}

void A3DGFXEx::SetShakeCamType(SHAKECAM sc)
{
	ASSERT(sc >= SHAKECAM_NONE && sc < SHAKECAM_NUM);
	m_iShakeType = (int)sc;
}

void A3DGFXEx::SetPos(const A3DVECTOR3& vPos)
{
	m_vPos = vPos;
	m_matNoScale.SetRow(3, vPos);
	m_bTransDirty = true;
}

void A3DGFXEx::SetDirAndUp(const A3DVECTOR3 vDir, const A3DVECTOR3& vUp)
{
	if (m_bNoChangeDir)
		return;

	A3DVECTOR3 vRight = CrossProduct(vUp, vDir);
	m_matNoScale.SetRow(0, vRight);
	m_matNoScale.SetRow(1, vUp);
	m_matNoScale.SetRow(2, vDir);
	m_qDir.ConvertFromMatrix(m_matNoScale);
	m_bTransDirty = true;
}

bool A3DGFXEx::SetParentTM(const A3DMATRIX4& matParent)
{
	m_vPos = matParent.GetRow(3);
	m_matNoScale.SetRow(3, m_vPos);

	if (!m_bNoChangeDir)
	{
		m_matNoScale.SetRow(0, Normalize(matParent.GetRow(0)));
		m_matNoScale.SetRow(1, Normalize(matParent.GetRow(1)));
		m_matNoScale.SetRow(2, Normalize(matParent.GetRow(2)));
		m_qDir.ConvertFromMatrix(m_matNoScale);
	}

	m_bTransDirty = true;
	return true;
}

void A3DGFXEx::CalcParentTM()
{
	m_matParent = m_matScale * m_matNoScale;

	if (m_bMatchGround)
	{
		float dx = m_vPos.x - m_vOldPos.x;
		float dz = m_vPos.z - m_vOldPos.z;

		if (dx * dx + dz * dz > 1e-6)
		{
			m_vOldPos = m_vPos;
			m_fGroundHeight = AfxGetGrndNorm(m_vPos, &m_vGroundNorm);
		}

		if (fabs(m_vPos.y - m_fGroundHeight) < _match_grnd_max_delta)
			m_bCloseToGrnd = true;
		else
			m_bCloseToGrnd = false;

		m_qMatchGrndDir = m_qDir;
		GetMatchGrndQuat(m_qMatchGrndDir, m_vGroundNorm);
		m_qMatchGrndDir.ConvertToMatrix(m_matGroundNormal);
		m_matGroundNormal.SetRow(3, A3DVECTOR3(m_vPos.x, m_fGroundHeight, m_vPos.z));
		m_matGroundNormal = m_matScale * m_matGroundNormal;
		m_matGroundHeight = m_matNoScale;
		m_matGroundHeight._42 = m_fGroundHeight;
		m_matGroundHeight = m_matScale * m_matGroundHeight;
	}

#ifdef GFX_EDITOR
	
	m_AABB			= m_AABBOrg;
	m_AABB.Mins		*= m_fScale;
	m_AABB.Maxs		*= m_fScale;
	m_AABB.Mins		+= m_vPos;
	m_AABB.Maxs		+= m_vPos;
	m_AABB.CompleteCenterExts();

#else

	if (m_bResourceReady)
	{
		if (m_bUseAABB && m_bAccurateAABB)
		{
			if (!m_pAABBVerts)
			{
				m_pAABBVerts = new A3DVECTOR3[8];
				const A3DVECTOR3& v1 = m_AABBOrg.Mins;
				const A3DVECTOR3& v2 = m_AABBOrg.Maxs;
				m_pAABBVerts[0].Set(v1.x, v1.y, v1.z);
				m_pAABBVerts[1].Set(v1.x, v2.y, v1.z);
				m_pAABBVerts[2].Set(v1.x, v1.y, v2.z);
				m_pAABBVerts[3].Set(v1.x, v2.y, v2.z);
				m_pAABBVerts[4].Set(v2.x, v1.y, v1.z);
				m_pAABBVerts[5].Set(v2.x, v1.y, v2.z);
				m_pAABBVerts[6].Set(v2.x, v2.y, v1.z);
				m_pAABBVerts[7].Set(v2.x, v2.y, v2.z);
			}

			m_AABB.Clear();

			for (int i = 0; i < 8; i++)
			{
				A3DVECTOR3 v = RotateVec(m_qDir, m_pAABBVerts[i] * m_fScale);
				m_AABB.AddVertex(v);
			}

			m_AABB.Mins	+= m_vPos;
			m_AABB.Maxs	+= m_vPos;
		}
		else
		{
			m_AABB			= m_AABBOrg;
			m_AABB.Mins		*= m_fScale;
			m_AABB.Maxs		*= m_fScale;
			m_AABB.Mins		+= m_vPos;
			m_AABB.Maxs		+= m_vPos;
		}

		m_AABB.CompleteCenterExts();
	}
	else
		m_AABB.Clear();

#endif
}

const A3DMATRIX4& A3DGFXEx::GetParentTM()
{
	if (m_bTransDirty)
	{
		CalcParentTM();
		m_bTransDirty = false;
	}

	return m_matParent;
}

bool A3DGFXEx::Start(bool)
{
	if (!m_bResourceReady)
	{
		m_iState = ST_PLAY;
		return true;
	}
	else
		m_bStateInit = true;

	Stop();
	m_iState = ST_PLAY;
	StartElements();
	m_dwTimeSpan = 0;

	if (m_pShakeNoise)
		m_pShakeCamera->Start();

	m_bTransDirty = true;
	m_bEmitterStopped = false;
	m_bFadingOut = false;

	if(m_dwSkipTime > 0)
	{
		int nLoop = m_dwSkipTime / 50;
		int nLastTick = m_dwSkipTime % 50;

		for (int i = 0 ; i < nLoop ; ++i) {
			TickAnimation(50);
		}
		TickAnimation(nLastTick);
	}

	return true;
}

void A3DGFXEx::StartElements()
{
	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
#ifdef GFX_EDITOR
		m_EleLst[i]->SetInit(false);
#endif
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->Play();
	}
}

void A3DGFXEx::ResumeLoop()
{
	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		if (NULL == m_EleLst[i])
			continue;
		m_EleLst[i]->ResumeLoop();
	}
}

void A3DGFXEx::DeriveParentProperty(A3DGFXEx* pParent)
{
	SetSfxPriority(pParent->GetSfxPriority());
	SetZTestEnable(pParent->IsZTestEnable());
	SetId(pParent->GetId(), pParent->GetHostModel());
	SetDisableCamShake(pParent->IsDisableCamShake());
	SetUseLOD(pParent->GetUseLOD());
	SetCreatedByGFXECM(pParent->IsCreatedByGFXECM());
	SetAccurateAABB(pParent->IsAccurateAABB());
	m_bChild = true;
}

void A3DGFXEx::SetShakePeriod( DWORD dwPeriod )
{
	if (!m_pShakeCamera)
		return;

	bool bShakeByDistance = m_pShakeCamera->GetShakeByDistance();
	A3DGFX_CAMERA_BLUR_INFO BlurInfo = m_pShakeCamera->GetCameraBlurInfo();
	float fShakeAttenuDist = m_pShakeCamera->GetAttenuDist();
	delete m_pShakeCamera;
	m_pShakeCamera = IShakeCamera::CreateShakeCam(this, dwPeriod, bShakeByDistance, BlurInfo, fShakeAttenuDist);
}

DWORD A3DGFXEx::GetShakePeriod() const
{
	if (!m_pShakeCamera)
		return 0;

	return m_pShakeCamera->GetShakePeriod();
}

void A3DGFXEx::SetShakeByDistance( bool bShakeByDis )
{
	if (!m_pShakeCamera)
		return;

	m_pShakeCamera->SetShakeByDistance(bShakeByDis);
}

bool A3DGFXEx::GetShakeByDistance() const
{
	if (!m_pShakeCamera)
		return true;

	return m_pShakeCamera->GetShakeByDistance();
}

A3DAABB A3DGFXEx::GetModelAABB()
{
	A3DAABB outAABB = m_AABB;

	for (size_t i = 0; i < m_EleLst.size(); i++)
	{
		A3DGFXElement* pEle = m_EleLst[i];
		if (!pEle)
			continue;
		switch (pEle->GetEleTypeId())
		{
		case ID_ELE_TYPE_MODEL:
			{
				A3DSkinModel* pSkinModel = ((A3DGFXModel*)pEle)->GetSkinModel();
				if (pSkinModel)
				{
					outAABB.Merge(pSkinModel->GetModelAABB());
				}
			}
			break;
		case ID_ELE_TYPE_ECMODEL:
			{
				A3DSkinModel* pSkinModel = ((A3DGFXECModel*)pEle)->GetSkinModel();
				if (pSkinModel)
				{
					outAABB.Merge(pSkinModel->GetModelAABB());
				}
			}
			break;
		case ID_ELE_TYPE_MODEL_PROXY:
			{
				A3DSkinModel* pSkinModel = ((A3DGFXModelProxy*)pEle)->GetSkinModel();
				if (pSkinModel)
				{
					outAABB.Merge(pSkinModel->GetModelAABB());
				}
			}
			break;
		}
	}

	return outAABB;
}