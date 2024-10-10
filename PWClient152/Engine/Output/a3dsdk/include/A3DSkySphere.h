/*
 * FILE: A3DSkySphere.h
 *
 * DESCRIPTION: Class that standing for the sphere sky method in A3D Engine
 *
 * CREATED BY: Hedi, 2003/1/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKYSPHERE_H_
#define _A3DSKYSPHERE_H_

#include "A3DVertex.h"
#include "A3DSky.h"

class A3DStream;
class A3DTexture;
class A3DDevice;
class A3DCamera;
class A3DHLSL;
class A3DVertexDecl;

class A3DSkySphere : public A3DSky
{
public:		//	Types

	//	Animation properties
	struct ANIM_PROP
	{
	public:

		friend class A3DSkySphere;

		float	fCapTile;		//	Cap tile
		float	fCapSpeedU;		//	Cap scroll speed on U axis
		float	fCapSpeedV;		//	Cap scroll speed on V axis
		float	fCapRotSpeed;	//	Cap rotate speed (radian per 1/30s)

		ANIM_PROP();

	protected:

		float	fCapOffsetU;	//	Cap scroll offset on U axis
		float	fCapOffsetV;	//	Cap scroll offset on V axis
		float	fCapRotate;		//	Cap rotated radian
	};

private:

	//Creation Param;
	WORD				m_nSegmentCount;
	FLOAT				m_vPitchRange;
	FLOAT				m_fRadius;

	ANIM_PROP			m_CurAnimProp;		//	Current animation properties
	ANIM_PROP			m_DstAnimProp;		//	Destination animation properties
	A3DMATRIX4			m_tmCurCapCloud;
	A3DMATRIX4			m_tmDstCapCloud;

	A3DStream *			m_pStreamSurround;
	A3DStream *			m_pStreamCap;

	int					m_nVertCountS;
	int					m_nIndexCountS;
	
	int					m_nVertCountF;
	int					m_nIndexCountF;
	
	int					m_nVertCountC;
	int					m_nIndexCountC;
	
	A3DTexture *		m_pTextureSurroundF;
	A3DTexture *		m_pTextureSurroundB;
	A3DTexture *		m_pTextureCap;

	// for a transition effect
	int					m_nTransTime;
	int 				m_nTransTickLeft;

	bool				m_bTransToDest;			// true to trans to dest, false to trans to src
	bool				m_bTextureLoading;		// true means texture is loading

	char				m_szCapToLoad[MAX_PATH];
	char				m_szSurroundFToLoad[MAX_PATH];
	char				m_szSurroundBToLoad[MAX_PATH];

	A3DTexture *		m_pTextureSurroundFDest;
	A3DTexture *		m_pTextureSurroundBDest;
	A3DTexture *		m_pTextureCapDest;

    A3DHLSL *           m_pHLSLCap;
    A3DHLSL *           m_pHLSLCapBlend;
    A3DHLSL *           m_pHLSLSurround;
    A3DHLSL *           m_pHLSLSurroundBlend;
    A3DVertexDecl *     m_pVertDecl;

protected:
	bool CreateSurround();
	bool CreateCap();
    bool RenderCapHLSL(A3DTexture * pTextureCur, A3DTexture * pTextureDst, float fAlpha);
    bool RenderSurroundHLSL(A3DTexture * pTextureCurF, A3DTexture * pTextureDstF,
        A3DTexture * pTextureCurB, A3DTexture * pTextureDstB, float fAlpha);

	bool SwitchClouds();

	//	Update cap texture tm
	void UpdateCapTexTM(A3DMATRIX4& matTM, ANIM_PROP& props);
    void RenderHLSL();
    A3DTexture* SafeTexture(A3DTexture* pTexture);

public:
	A3DSkySphere();
	virtual ~A3DSkySphere();

	bool Init(A3DDevice* pDevice, A3DCamera* pCamera, const char* szCap, const char* szSurroundF, const char* szSurroundB);
	bool TransSky(const char * szCap, const char * szSurroundF, const char * szSurroundB, int nTransTime/*in millisecond*/, const ANIM_PROP* pDstAnim=NULL);
	bool SetTimePassed(int nDeltaTime);
	virtual bool Release();

	virtual bool Render();

    virtual bool TickAnimation();

	virtual bool SetCamera(A3DCamera * pCamera);

	inline A3DTexture * GetTextureSurroundF() { return m_pTextureSurroundF; }
	inline A3DTexture * GetTextureSurroundB() { return m_pTextureSurroundB; }
	inline A3DTexture * GetTextureCap() { return m_pTextureCap; }

	bool SetTextureSurroundF(char * szTextureFile);
	bool SetTextureSurroundB(char * szTextureFile);
	bool SetTextureCap(char * szTextureFile);

	inline float GetTile() { return m_CurAnimProp.fCapTile; }
	inline void SetTile(float fTile) { m_CurAnimProp.fCapTile = fTile; }

	inline float GetFlySpeedU() { return m_CurAnimProp.fCapSpeedU; }
	inline void SetFlySpeedU(float u) { m_CurAnimProp.fCapSpeedU = u; }

	inline float GetFlySpeedV() { return m_CurAnimProp.fCapSpeedV; }
	inline void SetFlySpeedV(float v) { m_CurAnimProp.fCapSpeedV = v; }

	inline bool IsTransing() { return m_nTransTickLeft != 0; }
	inline bool IsTextureLoading() { return m_bTextureLoading; }

	bool NeedTrans(const char * szCap, const char * szSurroundF, const char * szSurroundB, const ANIM_PROP* pDstAnim=NULL);

	friend DWORD WINAPI SkyLoadTextures(LPVOID pArg);
};

typedef A3DSkySphere * PA3DSkySphere;

#endif//_A3D_SKYSPHERE_H_