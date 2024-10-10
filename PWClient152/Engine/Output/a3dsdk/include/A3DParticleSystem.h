/*
 * FILE: A3DParticleSystem.h
 *
 * DESCRIPTION: Base class of particle system for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DPARTICLESYSTEM_H_
#define _A3DPARTICLESYSTEM_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DObject.h"
#include "A3DFuncs.h"
#include "AAssist.h"

class A3DModel;
class A3DGraphicsFX;
class A3DGFXMan;
class A3DTextureMan;
class A3DViewport;
class A3DEngine;
class A3DDevice;
class A3DFrame;
class A3DStream;
class A3DTexture;

#define A3DPARTICLESYSTEM_MAXPARTICLES				128

//Max particles in this particle system;
#define A3DPARTICLESYSTEM_MAXPARTICLES_TOTAL		1024
//Max particles that rendering at the same time in this particle system;
#define A3DPARTICLESYSTEM_MAXPARTICLES_RENDER		256
//Max color maps which are used in determine the color of each particles;
//This color map can only be projected onto axis-plane;
#define A3DPARTICLESYSTEM_MAXCOLORMAPS				5
//Max object textures used in object fragment mode;
#define A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE 32
//Max instanced objects;
#define A3DPARTICLESYSTEM_INSTANCED_MAXMODEL		16

enum A3DPARTICLE_TYPE
{
	A3DPARTICLE_TYPE_UNKNOWN = -1,
	A3DPARTICLE_STANDARD_PARTICLES = 0,
	A3DPARTICLE_META_PARTICLES,
	A3DPARTICLE_OBJECT_FRAGMENTS,
	A3DPARTICLE_INSTANCED_GEOMETRY
};

enum A3DPARTICLE_STANDARD_PARTICLE_TYPE
{
	A3DPARTICLE_STANDARD_PARTICLE_TRIANGLE = 0,
	A3DPARTICLE_STANDARD_PARTICLE_CUBE,
	A3DPARTICLE_STANDARD_PARTICLE_SPECIAL,
	A3DPARTICLE_STANDARD_PARTICLE_FACING,
	A3DPARTICLE_STANDARD_PARTICLE_CONSTANT,
	A3DPARTICLE_STANDARD_PARTICLE_TETRA,
	A3DPARTICLE_STANDARD_PARTICLE_SIXPOINT,
	A3DPARTICLE_STANDARD_PARTICLE_SPHERE
};

enum A3DPARTICLE_FRAGMENT_TYPE
{
	A3DPARTICLE_FRAGMENT_TYPE_ALLFACES = 0,
	A3DPARTICLE_FRAGMENT_TYPE_NUMBEROFCHUNCKS,
	A3DPARTICLE_FRAGMENT_TYPE_SMOOTHINGANGLE
};

enum A3DPARTICLE_SPAWN_TYPE
{
	A3DPARTICLE_SPAWN_NONE = 0,
	A3DPARTICLE_SPAWN_DIEONCOLLISION,
	A3DPARTICLE_SPAWN_SPAWNONCOLLISION,
	A3DPARTICLE_SPAWN_SPAWNONDEATH,
	A3DPARTICLE_SPAWN_TRAILS
};

enum A3DPARTICLE_SPINAXIS_TYPE
{
	A3DPARTICLE_SPINAXIS_RANDOM = 0,
	A3DPARTICLE_SPINAXIS_TRAVELDIR,
	A3DPARTICLE_SPINAXIS_DEFINEDDIR
};

// In which color space the interpolation of color is done;
enum A3DPARTICLE_COLORSPACE
{
	A3DPARTICLE_COLORSPACE_RGB = 0,
	A3DPARTICLE_COLORSPACE_HSV
};

enum A3DPARTICLE_COLORMAP_PLANE
{
	A3DPARTICLE_COLORMAP_PLANE_XZ = 0,
	A3DPARTICLE_COLORMAP_PLANE_XY,
	A3DPARTICLE_COLORMAP_PLANE_YZ
};

// A common particle parameter which are used in all kinds of particles;
typedef struct _COMMON_PARTICLE
{
	A3DVECTOR3  vecPos;		// Position of the particle;
	int			nLive;		// How long this particle has existed;
	int			nLife;		// How long can this particle still exist;

	A3DCOLOR	color;		// The diffuse color of this particle, including alpha channel;
							
	A3DVECTOR3  vecSpeed; 	// Moving direction and its velocity;
	A3DVECTOR3  vecDir;		// Moving direction; Just be the original direction of travellying, will not change during its life, not like speed;
	A3DVECTOR3	vecUp;		// The perpendicular directin of vecDir;

	A3DVECTOR3	vecSpinAxis;// The axis around which this particle rotates;
	int			nSpinTime;  // The interval of the spinning;
	FLOAT		vSpinPhase; // The starting phase of this particle;

	FLOAT		vSize;		// How big the particle is;

	FLOAT		vBubbleAmplitude; // The amplitude factor of this particle's bubble behavior;
	int			nBubblePeriod; // The period that this particle spin on the pertendular plane;
	FLOAT		vBubblePhase; // The starting phase of the particle rotation;
} COMMON_PARTICLE, * PCOMMON_PARTICLE;

// A standard particle;
typedef struct _STANDARD_PARTICLE
{
	COMMON_PARTICLE		common;	
} STANDARD_PARTICLE, * PSTANDARD_PARTICLE;

typedef struct _INSTANCED_GEOMETRY_PARTICLE
{
	COMMON_PARTICLE		common;
	A3DModel *			pModel;
	int					nSunkTicks;	// if > 0, this particle is sinking;
} INSTANCED_GEOMETRY_PARTICLE, * PINSTANCED_GEOMETRY_PARTICLE;

// An object fragment particle;
typedef struct _OBJECT_FRAGMENT_PARTICLE
{
	COMMON_PARTICLE		common;
	int					nVertCount;
	int					nIndexCount;
	int					nTextureID;
	A3DLVERTEX			*pVertex;		
	WORD				*pIndex;
	
	A3DGraphicsFX		*pGFX; // Some fragment will has its own smoke effects;
} OBJECT_FRAGMENT_PARTICLE, * POBJECT_FRAGMENT_PARTICLE;

class A3DParticleSystem : public A3DObject
{
protected:
	A3DDevice *				m_pA3DDevice;
	A3DFrame *				m_pParentFrame;

	static A3DVECTOR3		m_EmittingAxis;

	FLOAT					m_vGravity;

	FLOAT					m_vScale;
	FLOAT					m_vSpeedScale;

	// Position and orientation of the emitter;
	A3DVECTOR3				m_EmitterPos;
	A3DVECTOR3				m_EmitterDir;
	A3DVECTOR3				m_EmitterUp;

	A3DMATRIX4				m_EmitterRelativeTM; // Construct from pos, dir and up
	A3DMATRIX4				m_EmitterParentTM; // Emitter's parent transform matrix
	A3DMATRIX4				m_EmitterAbsoluteTM; // Multiply my parent absolute TM and my relative TM;
	A3DVECTOR3				m_EmitterAbsoluteVelocity; // Velocity in world space. This will be calculated from difference of absolute TM

	A3DPARTICLE_TYPE		m_ParticleType;	 // Type of the particle
	int						m_nParticleSize; // Size of one particle structure

	LPBYTE					m_pParticleBuffer; // The pre-malloc array to hold particles;
	int						m_nNumParticles; // How many particles are there in the pre-malloc array;
	int						m_nMaxParticles; // Max particles can be held in the pre-malloc array;

	bool					m_bEmitting;
	int						m_nMaxVisibleParticles;

	// This section variables are for standard particle system;
	int						m_nVertCount;
	int						m_nIndexCount;
	int						m_nMaxVertCount;
	int						m_nMaxIndexCount;
	A3DTLVERTEX *			m_pVertexBuffer;
	WORD *					m_pIndexBuffer;
	A3DStream *				m_pA3DStream;
	A3DTexture *			m_pA3DTexture;
	char					m_szTextureMap[MAX_PATH];

	A3DBLEND				m_SrcBlend;
	A3DBLEND				m_DestBlend;

	// This section variables are for object fragment system;
	int						m_nTextureNum;
	int						m_pnVertCount[A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE];
	int						m_pnIndexCount[A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE];
	A3DLVERTEX *			m_ppVertexBuffers[A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE];
	WORD *					m_ppIndexBuffers[A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE];
	A3DTexture *			m_ppA3DTextures[A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE];
	
	// This section variables are used for instanced geomemtry system;
	int						m_nInstancedModelNum;
	char					m_szInstancedModelName[A3DPARTICLESYSTEM_INSTANCED_MAXMODEL][MAX_PATH];
	A3DModel *				m_pInstancedModels[A3DPARTICLESYSTEM_INSTANCED_MAXMODEL];
	
	// Color Maps Section;
	int						m_nColorNum; // How many colors are used in color map;
	A3DCOLOR				m_pColors[A3DPARTICLESYSTEM_MAXCOLORMAPS];
	A3DHSVCOLORVALUE		m_pHSVColors[A3DPARTICLESYSTEM_MAXCOLORMAPS];
	FLOAT					m_vColorMapRange;
	A3DPARTICLE_COLORSPACE  m_ColorSpace;
	A3DPARTICLE_COLORMAP_PLANE	m_ColorMapPlane;

	// Timing ruler;
	int						m_nTicks;

	// Generation Control Sets;
	// Particle Quantity Control;
	bool					m_bUseRate;
	int						m_nUseRate;
	bool					m_bUseTotal;
	int						m_nUseTotal;

	A3DCOLOR GetParticleColor(FLOAT vHeight);

	inline int GetNumberToNew()
	{
		if( m_bUseRate )
			return m_nUseRate;
		else
		{
			if( m_nEmitEnd < m_nEmitStart )
				return 0;
			else
				return m_nUseTotal / (m_nEmitEnd - m_nEmitStart + 1);
		}
	}

	// Particle Timing Control;
	int		m_nEmitStart;
	int		m_nEmitEnd;
	int		m_nDisplayUntil;
	bool	m_bExpired;
	int		m_nLife;
	int		m_nLifeVariation;

	inline int GetNextParticleLife() { return (int)(m_nLife * RandFloat(1.0f, m_nLifeVariation * 1.0f / m_nLife)); }
	inline int GetLongestParticleLife() { return (int)(m_nLife * (1.0f + m_nLifeVariation * 1.0f / m_nLife)); }

	// Particle Motion Control;
	FLOAT	m_vSpeed;
	FLOAT	m_vSpeedVariation; //%
	FLOAT	m_vDivergence;

	inline FLOAT GetNextParticleSpeedValue() { return RandFloat(m_vSpeed, m_vSpeed * m_vSpeedVariation * 0.01f) * m_vSpeedScale; }

	// Particle Size Control;
	FLOAT	m_vSize;
	FLOAT	m_vSizeVariation; //%

	inline FLOAT GetNextParticleSize() { return RandFloat(m_vSize, m_vSize * m_vSizeVariation * 0.01f) * m_vScale; }

	int		m_nGrowFor;	// At first m_nGrowFor frame, the particle will grow up;
	int		m_nFadeFor; // At last m_nFadeFor frame, the particle will fade out;
	
	// Uniqueness
	DWORD	m_dwSeed;

	// Object motion inheritance;
	FLOAT	m_vInheritInfluence; //%
	FLOAT	m_vInheritInfluenceVariation; //%

	inline FLOAT GetNextParticleInheritInfluence() { return RandFloat(m_vInheritInfluence, m_vInheritInfluence * m_vInheritInfluenceVariation * 0.01f); }

	FLOAT	m_vInheritMultiplier;
	
	// Particle Type Control Sets;
	// Standard Particle Control;
	A3DPARTICLE_STANDARD_PARTICLE_TYPE m_StandardParticleType;

	// Meta Particle Control;
	FLOAT	m_vMetaParticleTension; 
	FLOAT	m_vMetaParticleTensionVariation; //%

	inline FLOAT GetNextParticleTension() { return RandFloat(m_vMetaParticleTension, m_vMetaParticleTension * m_vMetaParticleTensionVariation * 0.01f); }

	// Object fragment Control;
	FLOAT	m_vFragmentThickness;
	int		m_nObjectFragmentMinimum;
	FLOAT	m_vObjectFragmentSmoothAngle;

	A3DPARTICLE_FRAGMENT_TYPE m_ObjectFragmentType;

	// Particle Spawn Control Sets;
	A3DPARTICLE_SPAWN_TYPE	m_SpawnType;

	int		m_nSpawns;
	FLOAT	m_vSpawnAffects;
	int		m_nSpawnMultiplier;
	FLOAT	m_vSpawnVariation;

	// Particle Rotation Control Sets;
	int		m_nSpinTime; // How many frame it will take to let particle rotate 360 degree;
	FLOAT	m_vSpinTimeVariation; //%

	inline int GetNextParticleSpinTime()
	{
		int nSpinTime = (int)(m_nSpinTime * RandFloat(1.0f, m_vSpinTimeVariation * 0.01f));
		if( nSpinTime < 0 )
			return 0;
		else
			return nSpinTime;
	}

	FLOAT	m_vSpinPhase; //Deg
	FLOAT	m_vSpinPhaseVariation; //%

	inline FLOAT GetNextParticleSpinPhase() { return RandFloat(m_vSpinPhase, m_vSpinPhase * m_vSpinPhaseVariation * 0.01f); }

	A3DPARTICLE_SPINAXIS_TYPE m_SpinAxis;
	
	A3DVECTOR3		m_vecSpinDefinedAxis;
	FLOAT			m_vSpinAxisVariation; //deg
	
	inline A3DMATRIX4 GetNextParticleSpinVariation()
	{
		FLOAT		vSpinX = RandFloat(0.0f, m_vSpinAxisVariation);
		FLOAT		vSpinY = RandFloat(0.0f, m_vSpinAxisVariation);
		FLOAT		vSpinZ = RandFloat(0.0f, m_vSpinAxisVariation);

		return a3d_RotateX(DEG2RAD(vSpinX)) * a3d_RotateY(DEG2RAD(vSpinY)) * a3d_RotateZ(DEG2RAD(vSpinZ));
	}

	// Input, the travel direction of this particle;
	inline A3DVECTOR3 GetNextParticleSpinAxis(A3DVECTOR3& vecDir)
	{
		A3DVECTOR3  vecAxis = A3DVECTOR3(0.0f);
		A3DMATRIX4	matRotate = GetNextParticleSpinVariation();
	
		switch(m_SpinAxis)
		{
		case A3DPARTICLE_SPINAXIS_RANDOM:
			vecAxis = Normalize(A3DVECTOR3(RandFloat(0.0f, 1.0f), RandFloat(0.0f, 1.0f), RandFloat(0.0f, 1.0f)));
			break;
		case A3DPARTICLE_SPINAXIS_TRAVELDIR:
			vecAxis = vecDir * matRotate;
			break;
		case A3DPARTICLE_SPINAXIS_DEFINEDDIR:
			vecAxis = m_vecSpinDefinedAxis * matRotate;
			break;
		}
		return vecAxis;
	}

	// Bubble Motion Control Sets, there are for bubbles only;
	FLOAT	m_vBubbleAmplitude;
	FLOAT	m_vBubbleAmplitudeVariation; //%

	inline FLOAT GetNextParticleBubbleAmplitude() { return RandFloat(m_vBubbleAmplitude, m_vBubbleAmplitude * m_vBubbleAmplitudeVariation * 0.01f); }

	int		m_nBubblePeriod;
	FLOAT	m_vBubblePeriodVariation; //%

	inline int	 GetNextParticleBubblePeriod() { return (int)(m_nBubblePeriod * RandFloat(1.0f, m_vBubblePeriodVariation * 0.01f)); }

	FLOAT	m_vBubblePhase;
	FLOAT	m_vBubblePhaseVariation; //%

	inline FLOAT GetNextParticleBubblePhase() { return RandFloat(m_vBubblePhase, m_vBubblePhase * m_vBubblePhaseVariation * 0.01f); }

protected:

	// Construct up vector;
	bool UpdateUp();
	// Set all members to its default values;
	bool DefaultValues();
	// Generate one random float number ranges [a, b];
	inline FLOAT RandFloat(FLOAT average, FLOAT variation) { return a_Random(average - variation, average + variation); }

	bool AddParticle(LPBYTE pParticle);
	bool DeleteParticle(int nIndex);

	virtual bool MakeDead(LPVOID pParticle) = 0;
	virtual bool EmitParticles() = 0;
	bool FillCommonParticle(COMMON_PARTICLE * pCommonParticle); // Fill one particle's common parameter;

	virtual bool UpdateParticles();
	virtual bool UpdateStandardParticles();
	virtual bool UpdateMetaParticles();
	virtual bool UpdateObjectFragmentParticles();
	virtual bool UpdateInstancedGeometryParticles();

	bool UpdateCommonParticle(COMMON_PARTICLE * pCommonParticle); // Update one particle's common parameter;
	
	virtual bool ReleaseParticles();

public:
	A3DParticleSystem();
	virtual ~A3DParticleSystem();

	virtual bool Init(A3DDevice * pA3DDevice);
	virtual bool Release();
	
	virtual bool Save(AFile * pFileToSave);
	virtual bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	virtual bool CreateStandard();
	virtual bool CreateMeta();
	virtual bool CreateObjectFragment(int nTextureNum, A3DTexture ** ppTextures);
	virtual bool CreateInstancedGeometry();

	bool UpdateRelativeTM();
	bool UpdateParentTM(A3DMATRIX4& matParent);
	bool UpdateAbsoluteTM();

	bool Start(int nEmitLasting=-1, int nRenderLasting=-1); // negative nLasting means never stops automatically;
	bool Stop(bool bDeleteCurrent=false);

	virtual bool TickEmitting();
	virtual bool RenderParticles(A3DViewport * pCurrentViewport);
	virtual bool RenderStandardParticles(A3DViewport * pCurrentViewport);
	virtual bool RenderMetaParticles(A3DViewport * pCurrentViewport);
	virtual bool RenderObjectFragmentParticles(A3DViewport * pCurrentViewport);
	virtual bool RenderInstancedGeometryParticles(A3DViewport * pCurrentViewport);

public:
	inline int GetNumParticles() { return m_nNumParticles; }

	inline bool IsExpired() { return m_bExpired; }
	void SetParticleType(A3DPARTICLE_TYPE type);
	inline A3DPARTICLE_TYPE GetParticleType() { return m_ParticleType; }

	inline int GetInstancedModelNum() { return m_nInstancedModelNum; }
	inline char * GetInstancedModelName(int index) 
	{
		if( index >= m_nInstancedModelNum )
			return NULL;
		else
			return m_szInstancedModelName[index];
	}
	bool AddInstancedModel(char * szInstancedModel);
	bool RemoveInstancedModel(int index);
	
	inline void SetGravity(FLOAT vGravity) { m_vGravity = vGravity; }
	inline FLOAT GetGravity() { return m_vGravity; }

	// Interface for adjusting emitter's direction and up and position;
	inline void SetEmitPosX(FLOAT x) { m_EmitterPos.x = x; UpdateRelativeTM(); }
	inline void SetEmitPosY(FLOAT y) { m_EmitterPos.y = y; UpdateRelativeTM(); }
	inline void SetEmitPosZ(FLOAT z) { m_EmitterPos.z = z; UpdateRelativeTM(); }
	inline FLOAT GetEmitPosX() { return m_EmitterPos.x; }
	inline FLOAT GetEmitPosY() { return m_EmitterPos.y; }
	inline FLOAT GetEmitPosZ() { return m_EmitterPos.z; }

	inline void SetEmitDir(FLOAT vDeg, FLOAT vPitch) 
	{ 
		m_EmitterDir.x = (FLOAT)(sin(DEG2RAD(vDeg)) * cos(DEG2RAD(vPitch)));
		m_EmitterDir.y = (FLOAT)sin(DEG2RAD(vPitch));
		m_EmitterDir.z = (FLOAT)(cos(DEG2RAD(vDeg)) * cos(DEG2RAD(vPitch)));
		UpdateUp(); 
		UpdateRelativeTM();
	}
	inline FLOAT GetEmitDirDeg() 
	{
		return (FLOAT) RAD2DEG(atan2(m_EmitterDir.x, m_EmitterDir.z));
	}
	inline FLOAT GetEmitDirPitch()
	{
		return (FLOAT) RAD2DEG(asin(m_EmitterDir.y));
	}

	inline void SetEmitUpX(FLOAT x) { m_EmitterUp.x = x; UpdateRelativeTM(); }
	inline void SetEmitUpY(FLOAT y) { m_EmitterUp.y = y; UpdateRelativeTM(); }
	inline void SetEmitUpZ(FLOAT z) { m_EmitterUp.z = z; UpdateRelativeTM(); }
	inline FLOAT GetEmitUpX() { return m_EmitterUp.x; }
	inline FLOAT GetEmitUpY() { return m_EmitterUp.y; }
	inline FLOAT GetEmitUpZ() { return m_EmitterUp.z; }

	inline void SetStandardParticleType(A3DPARTICLE_STANDARD_PARTICLE_TYPE type) { m_StandardParticleType = type; }
	inline A3DPARTICLE_STANDARD_PARTICLE_TYPE GetStandardParticleType() { return m_StandardParticleType; }

	// Change current using texture;
	bool ChangeTextureMap(char * pszTextureMap);
	// Set texture's filename;
	inline void SetTextureMap(char * pszTextureMap) { strncpy_s(m_szTextureMap, pszTextureMap, MAX_PATH); }
	// Get texture's filename;
	inline char * GetTextureMap() { return m_szTextureMap; }

	inline void SetColorNum(int nColorNum) { m_nColorNum = min2(nColorNum, A3DPARTICLESYSTEM_MAXCOLORMAPS); }
	inline int  GetColorNum() { return m_nColorNum; }

	inline bool SetColor(int i, A3DCOLOR rgbColor)
	{
		if( i > m_nColorNum ) return false;
		m_pColors[i] = rgbColor;
		a3d_RGBToHSV(rgbColor, &m_pHSVColors[i]);
		return true;
	}
	inline A3DCOLOR GetColor(int i) { return m_pColors[i]; }

	inline void SetColorMapRange(FLOAT vColorMapRange) { m_vColorMapRange = vColorMapRange; }
	inline FLOAT GetColorMapRange() { return m_vColorMapRange; }

	inline void SetColorSpace(A3DPARTICLE_COLORSPACE colorSpace) { m_ColorSpace = colorSpace; }
	inline A3DPARTICLE_COLORSPACE GetColorSpace() { return m_ColorSpace; }

	inline void SetColorMapPlane(A3DPARTICLE_COLORMAP_PLANE plane) { m_ColorMapPlane = plane; }
	inline A3DPARTICLE_COLORMAP_PLANE GetColorMapPlane() { return m_ColorMapPlane; }

	//---------------------------------------------------------------//

	inline void SetUseRateMode(int nUseRate, bool bUseRate=true){ m_bUseRate = bUseRate; m_bUseTotal = !bUseRate; m_nUseRate = nUseRate;	}
	inline bool IsUseRate(){ return m_bUseRate; }
	inline int  GetUseRate() { return m_nUseRate; }

	inline void SetUseTotalMode(int nUseTotal, bool bUseTotal=true){ m_bUseRate = !bUseTotal; m_bUseTotal = bUseTotal; m_nUseTotal = nUseTotal; }
	inline bool IsUseTotal() { return m_bUseTotal; }
	inline int  GetUseTotal() { return m_nUseTotal; }

	inline void SetEmitStart(int nEmitStart) { m_nEmitStart = nEmitStart; }
	inline int  GetEmitStart() { return m_nEmitStart; }
	inline void SetEmitEnd(int nEmitEnd) { m_nEmitEnd = nEmitEnd; }
	inline int  GetEmitEnd() { return m_nEmitEnd; }
	inline void SetDisplayUntil(int nDisplayUntil) { m_nDisplayUntil = nDisplayUntil; }
	inline int  GetDisplayUntil() { return m_nDisplayUntil; }
	inline void SetLife(int nLife) { m_nLife = nLife; }
	inline int  GetLife() { return m_nLife; } 
	inline void SetLifeVariation(int nLifeVariation) { m_nLifeVariation = nLifeVariation; }
	inline int  GetLifeVariation() { return m_nLifeVariation; }

	inline void SetSpeed(FLOAT vSpeed) { m_vSpeed = vSpeed; }
	inline FLOAT GetSpeed() { return m_vSpeed; }
	inline void SetSpeedVariation(FLOAT vSpeedVariation) { m_vSpeedVariation = vSpeedVariation; }
	inline FLOAT GetSpeedVariation() { return m_vSpeedVariation; }
	inline void SetDivergence(FLOAT vDivergence) { m_vDivergence = vDivergence; }
	inline FLOAT GetDivergence() { return m_vDivergence; }

	inline void SetSize(FLOAT vSize) { m_vSize = vSize; }
	inline FLOAT GetSize() { return m_vSize; }
	inline void SetSizeVariation(FLOAT vSizeVariation) { m_vSizeVariation = vSizeVariation; }
	inline FLOAT GetSizeVariation() { return m_vSizeVariation; }
	inline void SetGrowFor(int nGrowFor) { m_nGrowFor = nGrowFor; }
	inline int  GetGrowFor() { return m_nGrowFor; }
	inline void SetFadeFor(int nFadeFor) { m_nFadeFor = nFadeFor; }
	inline int  GetFadeFor() { return m_nFadeFor; }

	inline A3DPARTICLE_SPINAXIS_TYPE  GetSpinAxisType() { return m_SpinAxis;}
	inline void  SetSpinAxisType(A3DPARTICLE_SPINAXIS_TYPE spintype) { m_SpinAxis = spintype; }
	inline FLOAT GetSpinAxisX() {return m_vecSpinDefinedAxis.x;}
	inline FLOAT GetSpinAxisY() {return m_vecSpinDefinedAxis.y;}
	inline FLOAT GetSpinAxisZ() {return m_vecSpinDefinedAxis.z;}
	inline FLOAT GetSpinAxisVariation() {return m_vSpinAxisVariation;}
	inline void  SetSpinAxisX(FLOAT x){m_vecSpinDefinedAxis.x = x;}
	inline void  SetSpinAxisY(FLOAT y){m_vecSpinDefinedAxis.y = y;}
	inline void  SetSpinAxisZ(FLOAT z){m_vecSpinDefinedAxis.z = z;}
	inline void  SetSpinAxisVariation(FLOAT v) {m_vSpinAxisVariation = v;}

	inline void SetInheritInfluence(FLOAT vInfluence) { m_vInheritInfluence = vInfluence; }
	inline FLOAT GetInheritInfluence() { return m_vInheritInfluence; }
	inline void SetInheritInfluenceVariation(FLOAT vInfluenceVariation) { m_vInheritInfluenceVariation = vInfluenceVariation; }
	inline FLOAT GetInheritInfluenceVariation() { return m_vInheritInfluenceVariation; }
	inline void SetInheritMultiplier(FLOAT vMultiplier) { m_vInheritMultiplier = vMultiplier; }
	inline FLOAT GetInheritMultiplier() { return m_vInheritMultiplier; }


	inline void SetFragmentThickness(FLOAT vThickness) { m_vFragmentThickness = vThickness; }
	inline FLOAT GetFragmentThickness() { return m_vFragmentThickness; }
	inline void SetFragmentType(A3DPARTICLE_FRAGMENT_TYPE type) { m_ObjectFragmentType = type; }
	inline A3DPARTICLE_FRAGMENT_TYPE GetFragmentType() { return m_ObjectFragmentType; }
	inline void SetFragmentMinimum(int nMinimum) { m_nObjectFragmentMinimum = nMinimum; }
	inline int  GetFragmentMinimum() { return m_nObjectFragmentMinimum; }
	inline void SetFragmentSmoothAngle(FLOAT vSmoothAngle) { m_vObjectFragmentSmoothAngle = vSmoothAngle; }
	inline FLOAT GetFragmentSmoothAngle() { return m_vObjectFragmentSmoothAngle; }

	inline void SetSpinTime(int nSpinTime) { m_nSpinTime = nSpinTime; }
	inline int  GetSpinTime() { return m_nSpinTime; }
	inline void SetSpinTimeVariation(FLOAT vSpinTimeVariation) { m_vSpinTimeVariation = vSpinTimeVariation; }
	inline FLOAT GetSpinTimeVariation() { return m_vSpinTimeVariation; }
	inline void SetSpinPhase(FLOAT vSpinPhase) { m_vSpinPhase = vSpinPhase; }
	inline FLOAT GetSpinPhase() { return m_vSpinPhase; }
	inline void SetSpinPhaseVariation(FLOAT vSpinPhaseVariation) { m_vSpinPhaseVariation = vSpinPhaseVariation; }
	inline FLOAT GetSpinPhaseVariation() { return m_vSpinPhaseVariation; }

	inline void SetSpawnType(A3DPARTICLE_SPAWN_TYPE spawnType) { m_SpawnType = spawnType; }
	inline A3DPARTICLE_SPAWN_TYPE GetSpawnType() { return m_SpawnType; }
	inline void SetSpawns(int nSpawns) { m_nSpawns = nSpawns; }
	inline int  GetSpawns() { return m_nSpawns; }
	inline void SetSpawnAffects(FLOAT vSpawnAffects) { m_vSpawnAffects = vSpawnAffects; }
	inline FLOAT GetSpawnAffects() { return m_vSpawnAffects; }
	inline void SetSpawnMultiplier(int nSpawnMultiplier) { m_nSpawnMultiplier = nSpawnMultiplier; }
	inline int  GetSpawnMultiplier() { return m_nSpawnMultiplier; }
	inline void SetSpawnVariation(FLOAT vSpawnVariation) { m_vSpawnVariation = vSpawnVariation; }
	inline FLOAT GetSpawnVariation() { return m_vSpawnVariation; }

	inline void SetBubbleAmplitude(FLOAT vBubbleAmplitude) { m_vBubbleAmplitude = vBubbleAmplitude; }
	inline FLOAT GetBubbleAmplitude() { return m_vBubbleAmplitude; }
	inline void SetBubbleAmplitudeVariation(FLOAT vBubbleAmplitudeVariation) { m_vBubbleAmplitudeVariation = vBubbleAmplitudeVariation; }
	inline FLOAT GetBubbleAmplitudeVariation() { return m_vBubbleAmplitudeVariation; }
	inline void SetBubblePeriod(int nBubblePeriod) { m_nBubblePeriod = nBubblePeriod; }
	inline int  GetBubblePeriod() { return m_nBubblePeriod; }
	inline void SetBubblePeriodVariation(FLOAT vVariation) { m_vBubblePeriodVariation = vVariation; }
	inline FLOAT GetBubblePeriodVariation() { return m_vBubblePeriodVariation; }
	inline void SetBubblePhase(FLOAT vPhase) { m_vBubblePhase = vPhase; }
	inline FLOAT GetBubblePhase() { return m_vBubblePhase; }
	inline void SetBubblePhaseVariation(FLOAT vVariation) { m_vBubblePhaseVariation = vVariation; }
	inline FLOAT GetBubblePhaseVariation() { return m_vBubblePhaseVariation; }
	inline void ClearAbsoluteVelocity() { m_EmitterAbsoluteVelocity.x = m_EmitterAbsoluteVelocity.y = m_EmitterAbsoluteVelocity.z = 0.0f; }

	inline void SetScale(FLOAT vScale) { m_vScale = vScale; m_vSpeedScale = (float)sqrt(m_vScale);	}
	inline FLOAT GetScale() { return m_vScale; }

	inline void SetSrcBlend(A3DBLEND blend) { m_SrcBlend = blend; }
	inline A3DBLEND GetSrcBlend() { return m_SrcBlend; }
	inline void SetDestBlend(A3DBLEND blend) { m_DestBlend = blend; }
	inline A3DBLEND GetDestBlend() { return m_DestBlend; }
};


#endif //_A3DPARTICLESYSTEM_H_
