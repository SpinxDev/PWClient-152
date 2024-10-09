/*
 * FILE: EC_SunMoon.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/3/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_SUNMOON_H_
#define _EC_SUNMOON_H_

#include "A3DTypes.h"
#include "A3DVertex.h"

#define SUN_UNIT_SIZE			10.0f
#define MOON_UNIT_SIZE			20.0f

#define SUN_MAX_PITCH			(DEG2RAD(70.0f))
#define MOON_MAX_PITCH			(DEG2RAD(60.0f))

#define DAWN_TIME				(3.5f / 24.0f)
#define MID_NOON				(12.0f / 24.0f)
#define SUN_SET					(21.0f / 24.0f)
#define MID_NIGHT				(24.0f / 24.0f)

#define NIGHT_DAY_START			(3.0f / 24.0f)
#define NIGHT_SUN_RISE_MIN		(4.0f / 24.0f)
#define NIGHT_DAY_END			(7.0f / 24.0f)
#define DAY_NIGHT_START			(18.0f / 24.0f)
#define DAY_SUN_SET_MAX			(19.5f / 24.0f)
#define DAY_NIGHT_END			(21.0f / 24.0f)

class A3DPixelShader;
class A3DTexture;
class A3DStream;
class CECViewport;
class A3DViewport;
class A3DRenderTarget;

#define FLARE_ALPHA_FVF			(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX6)
struct FLARE_ALPHA_VERT
{
	A3DVECTOR4		pos;
	A3DCOLOR		diffuse;

	struct 
	{
		float		u, v;
	} tex_coords[6];
};

class CECSunMoon
{
private:
	A3DTexture *		m_pSunTexture;			// texture of the sun
	A3DTexture *		m_pSunShineTexture;		// texture of the sun shine
	A3DTexture *		m_pMoonTexture;			// texture of the moon

	bool				m_bCanDoFlare;			// flag indicates whether we can do flare or not
	A3DRenderTarget *	m_pFlareAlphaTarget;	// render target to store flare's alpha
	A3DPixelShader *	m_pFlareShader;			// pixel shader for render solar flare
	A3DStream *			m_pStreamFlareAlpha;	// stream stand for alpha accumulate
	A3DTexture *		m_pFlaresTexture;		// textures for all lens flares
	A3DStream *			m_pStreamFlares;		// stream stand for all lens flares

	A3DTexture *		m_pSunFlareTexture;		// texture for sun flare
	A3DTexture *		m_pMoonFlareTexture;	// texture for moon flare
	A3DStream *			m_pStreamSunPoint;		// stream used to test flare visibility
	A3DStream *			m_pStreamSun;			// stream stand for the sun
	A3DStream *			m_pStreamSunShine;		// stream stand for the sun shine
	A3DStream *			m_pStreamMoon;			// stream stand for the moon

	float				m_vSunSize;				// the size of the sun
	float				m_vMoonSize;			// the size of the moon

	A3DCOLOR			m_colorSun;				// color of sun
	A3DCOLOR			m_colorMoon;			// color of moon

	float				m_vSunPitch;			// the pitch of the sun
	float				m_vMoonPitch;			// the pitch of the moon
	A3DVECTOR3			m_vecLightDir;			// light direction

	float				m_vDeg;					// the degree of the light source

	double				m_vTimeOfTheDay;		// time of the day 0.0f means 00:00, 1.0f means 24:00
	float				m_fDNFactor;			// day or night factor
	float				m_fDNFactorDest;		// day or night factor dest

	bool				m_bTimeHasBeenSet;		// flag indicates time has been set
	bool				m_bDirHasBeenSet;		// flag indicates dir has been set
	bool				m_bFirstAlpha;			// flag indicates alpha target has not been cleared yet.

	float				m_vSunVisibility;		// sun visibility factor
	float				m_vMoonVisibility;		// moon visibility factor

	bool				m_bSunVisible;			// sun visible flag
	bool				m_bMoonVisible;			// moon visible flag

protected:
	bool InitFlare();
	bool ReleaseFlare();

	bool UpdateWithTime();
	bool UpdateLensFlares(A3DViewport * pViewport, const A3DVECTOR3 vecCamPos, const A3DVECTOR3 vecCamDir, const A3DVECTOR3 vecSunPos);
	
public:
	// Constructor and Destructor
	CECSunMoon();
	~CECSunMoon();

	// Initialize and finalize
	bool Init();
	bool Release();

	// render and tick
	bool Render(CECViewport * pViewport);
	bool RenderSolarFlare(CECViewport * pViewport);
	bool Tick(DWORD dwDeltaTime);

	// time adjust
	bool SetTimeOfTheDay(float vTime);
	float GetTimeOfTheDay() { return (float)m_vTimeOfTheDay; }

	// sun moon alpha adjust
	bool SetSunColor(A3DCOLOR color);
	bool SetMoonColor(A3DCOLOR color);

	bool SetLightDir(const A3DVECTOR3& vecLightDir);
	const A3DVECTOR3& GetLightDir() { return m_vecLightDir; }

	inline A3DCOLOR GetSunColor()	{ return m_colorSun; }
	inline A3DCOLOR GetMoonColor()	{ return m_colorMoon; }

	inline float GetSunPitch()		{ return m_vSunPitch; }
	inline float GetMoonPitch()		{ return m_vMoonPitch; }

	inline float GetDNFactor()		{ return m_fDNFactor; }
	inline float GetDNFactorDest()	{ return m_fDNFactorDest; }

	inline bool IsSunVisible()		{ return m_bSunVisible; }
	inline bool IsMoonVisible()		{ return m_bMoonVisible; }

	inline void SetSunVisible(bool bFlag) { m_bSunVisible = bFlag; }
	inline void SetMoonVisible(bool bFlag) { m_bMoonVisible = bFlag; }

	inline bool IsSunTime()			{ return m_vSunPitch > 0.0f; }

	// get day night trans time in millisecond.
	int GetDNTransTime();
};

#endif
