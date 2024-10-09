/*
 * FILE: EC_FullGlowRender.h
 *
 * DESCRIPTION: Fullscreen glow for the Element Client
 *
 * CREATED BY: Hedi, 2004/10/9 
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_FULLGLOWRENDER_H_
#define _EC_FULLGLOWRENDER_H_

#include <A3DTypes.h>
#include <A3DVertex.h>

class A3DDevice;
class A3DRenderTarget;
class A3DStream;
class A3DPixelShader;
class A3DTexture;
class A3DViewport;

// Glow types
enum FULLGLOW_TYPE
{
	FULLGLOW_TYPE_NULL = 0,
	FULLGLOW_TYPE_DAYLIGHT = 1,
	FULLGLOW_TYPE_NIGHT = 2,
	FULLGLOW_TYPE_UNDERWATER = 3,
	FULLGLOW_TYPE_DEAD = 4,
	FULLGLOW_TYPE_LOGIN = 5,
};

// Vertex for glow area
#define A3DGLOWVERT_FVF		D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX5

struct A3DGLOWVERTEX
{
	A3DGLOWVERTEX() {}
	A3DGLOWVERTEX(const A3DVECTOR4& _pos, A3DCOLOR _diffuse, float _u1, float _v1, 
		float _u2, float _v2, float _u3, float _v3, float _u4, float _v4, float _u5, float _v5)
	{
		pos = _pos;
		diffuse = _diffuse;
		u1 = _u1;
		v1 = _v1;
		u2 = _u2;
		v2 = _v2;
		u3 = _u3;
		v3 = _v3;
		u4 = _u4;
		v4 = _v4;
		u5 = _u5;
		v5 = _v5;
	}
	 
	A3DVECTOR4	pos;			//	Position on screen
	A3DCOLOR	diffuse;		//	Color
	float		u1, v1;
	float		u2, v2;
	float		u3, v3;
	float		u4, v4;
	float		u5, v5;
};

// screen present controller
class CECFullGlowPresenter
{
private:
	float				m_vGlowPower;
	float				m_vGlowPowerSet;
	float				m_vGlowPowerStep;
	int					m_nGlowPowerTime;

	float				m_vGlowLevel;
	float				m_vGlowLevelSet;
	float				m_vGlowLevelStep;
	int					m_nGlowLevelTime;

	float				m_vFadeLevel;
	float				m_vFadeLevelSet;
	float				m_vFadeStep;
	int					m_nFadeTime;

	float				m_vMotionBlurLevel;
	float				m_vMotionBlurLevelSet;
	float				m_vMotionBlurStep;
	int					m_nMotionBlurTime;

	float				m_vMonoRenderLevel;
	float				m_vMonoRenderLevelSet;
	float				m_vMonoRenderStep;
	int					m_nMonoRenderTime;
	A3DCOLOR			m_MonoHue;

	A3DPixelShader *	m_pGlow1Shader;
	A3DPixelShader *	m_pGlow2Shader;

	A3DPixelShader *	m_pMonoGlowShader;

public:
	CECFullGlowPresenter();

	void SetData(A3DPixelShader * pGlow1Shader, A3DPixelShader * pGlow2Shader, A3DPixelShader * pMonoGlowShader);
	
	void SetGlowPower(float vStart, float vEnd, int nTime);
	void SetGlowLevel(float vStart, float vEnd, int nTime);

	void SetMotionBlur(float vStart, float vEnd, int nTime);
	void SetFade(float vStart, float vEnd, int nTime);
	void SetMonoRender(float vStart, float vEnd, int nTime, A3DCOLOR monoHue);

	void StopMotionBlur();
	void StopFade(bool bToWhite=true);
	void StopMonoRender();

	void Reset();	// reset all parameters to default
	void Update(int nDeltaTime);

	// render states and pixelshader consts managing
	void PrepareGlow1Pass(float vBlur0, float vBlur1, float vCenter);
	void AfterGlow1Pass();
	void PrepareGlow2Pass(A3DCOLOR colorNow, float vBlur0, float vBlur1, float vCenter);
	void AfterGlow2Pass();

public:
	bool IsFading();
	bool IsMonoRendering();
	bool IsMontionBluring();
};

class CECFullGlowRender
{
private:
	bool					m_bRenderToBack;		// flag indicates we are rendering into back target of the device instead of glow target
	bool					m_bCanDoFullGlow;		// flag indicates whether we can do full screen glow
	bool					m_bResourceLoaded;		// flag indicates whether resource has been loaded
	bool					m_bGlowOn;				// flag indicates we do glow now
	bool					m_bWarpOn;				// flag indicates we do space warp
	bool					m_bFlareOn;				// flag indicates we do sun flare rendering
	bool					m_bSharpenOn;			// flag indicates we do sharpen or not

	A3DDevice *				m_pA3DDevice;
	int						m_nWidth;
	int						m_nHeight;
	
	FULLGLOW_TYPE			m_glowType;
	A3DCOLOR				m_color;

	CECFullGlowPresenter *	m_pPresenter;			// copy controller

	A3DRenderTarget *		m_pBackTarget;			//	render target same size as back buffer
	A3DRenderTarget *		m_pGlowTarget1;			//	render target 1/4 size of back buffer
	A3DRenderTarget *		m_pGlowTarget2;			//	render target 1/4 size of back buffer

	A3DPixelShader *		m_pGlow1Shader;			// glow1 rendering shader
	A3DPixelShader *		m_pGlow2Shader;			// glow2 rendering shader
	A3DPixelShader *		m_pMonoGlowShader;		// mono color glow rendering shader

	A3DStream *				m_pStreamStretchCopy;	// stream used to stretch copy
	A3DTLVERTEX				m_vertsStretchCopy[4];		

	A3DStream *				m_pStreamBlurX1;		// stream used to do x-axis bluring from target1 to target2
	A3DStream *				m_pStreamBlurY2;		// stream used to do y-axis bluring from target2 to screen
	
	A3DGLOWVERTEX			m_vertsBlurX1[4];
	A3DGLOWVERTEX			m_vertsBlurY2[4];

	WORD					m_indices[6];

	// space warp objects.
	A3DPixelShader *		m_pTLWarpShader;		// TL warp render pixel shader
	A3DStream *				m_pTLWarpStream;		// stream used to show space warps using TL verts
	int						m_nMaxTLWarpVerts;		// max verts number of TL warp stream
	int						m_nMaxTLWarpIndices;	// max indices number of TL warp stream

	A3DPixelShader *		m_pWarpShader;			// warp render pixel shader
	A3DStream *				m_pWarpStream;			// stream used to show space warps using 3d verts
	int						m_nMaxWarpVerts;		// max verts number of warp stream
	int						m_nMaxWarpIndices;		// max indices number of warp stream

	A3DPixelShader *		m_pSharpenShader;		// sharpen render pixel shader
	A3DStream *				m_pStreamSharpen;		// stream used to do sharpen

	A3DTexture *			m_pBumpUnderWater;

public:
	inline bool IsRenderToBack()					{ return m_bRenderToBack; }
	inline bool CanDoFullGlow()						{ return m_bCanDoFullGlow; }
	inline bool IsGlowOn()							{ return m_bGlowOn; }
	inline bool IsWarpOn()							{ return m_bWarpOn; }
	inline bool IsFlareOn()							{ return m_bFlareOn; }
	inline bool IsSharpenOn()						{ return m_bSharpenOn; }
	inline int GetWidth()							{ return m_nWidth; }
	inline int GetHeight() 							{ return m_nHeight; }
	A3DRenderTarget * GetBackTarget()				{ return m_pBackTarget; }

protected:
	void CalculateGaussian(double halfsize);

	bool LoadResource();
	bool ReleaseResource();

	bool CreateBumpMap();

	bool DoWarps(A3DViewport * pViewport);
	bool DoGlow();
	bool DoSharpen();

	inline bool GetNeedResource() { return m_bGlowOn || m_bFlareOn || m_bWarpOn || m_bSharpenOn; }

public:
	CECFullGlowRender();
	~CECFullGlowRender();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool SetGlowOn(bool bFlag);
	bool SetWarpOn(bool bFlag);
	bool SetFlareOn(bool bFlag);
	bool SetSharpenOn(bool bFlag);

	bool ReloadResource();

	bool Update(int nDeltaTime);

	bool BeginGlow();
	bool EndGlow(A3DViewport * pViewport);

	bool SetGlowType(FULLGLOW_TYPE type, DWORD value);

	void SetMotionBlur(float vStart, float vEnd, int nTime);
	void SetFade(float vStart, float vEnd, int nTime);
	void SetMonoRender(float vStart, float vEnd, int nTime, A3DCOLOR monoHue=0xffffffff);
};

#endif //_EC_FULLGLOWRENDER_H_