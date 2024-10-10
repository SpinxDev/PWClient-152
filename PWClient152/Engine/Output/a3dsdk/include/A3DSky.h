/*
 * FILE: A3DSky.h
 *
 * DESCRIPTION: Class that standing for the sky in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKY_H_
#define _A3DSKY_H_

#include "A3DVertex.h"
#include "A3DObject.h"

class A3DStream;
class A3DTexture;
class A3DFrame;
class A3DCameraBase;
class A3DDevice;

//	Vertex format used by sky for transition capability
#define A3DSKYVERT_FVF		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2
struct A3DSKYVERTEX
{
	float	x, y, z;		//	Position
	DWORD	diffuse;		//	Diffuse color
	float	u1, v1;			//	Texture coordinates for normal sky texture
	float	u2, v2;			//	Texture coordinates for transition alpha map
};

class A3DSky : public A3DObject
{
protected:
	A3DDevice *			m_pA3DDevice;
	A3DCameraBase *		m_pCamera;
	A3DMATRIX4			m_WorldMatrix;

	A3DTexture *		m_pTextureTrans;

private:
	//Creation Param;
	WORD				m_nLevelNum;
	WORD				m_nConeNum;
	FLOAT				m_fRadius;

	A3DStream *			m_pStreamSurround;
	A3DStream *			m_pStreamFlat;
	A3DStream *			m_pStreamCap;

	int					m_nVertCountS;
	int					m_nIndexCountS;
	A3DLVERTEX *		m_pVBSurround;
	WORD *				m_pIndicesSurround;
	
	int					m_nVertCountF;
	int					m_nIndexCountF;
	A3DLVERTEX *		m_pVBFlat;
	WORD *				m_pIndicesFlat;

	int					m_nVertCountC;
	int					m_nIndexCountC;
	A3DLVERTEX *		m_pVBCap;
	WORD *				m_pIndicesCap;

	char				m_szCap[MAX_PATH];
	char				m_szSurroundF[MAX_PATH];
	char				m_szSurroundB[MAX_PATH];
	char				m_szFlat[MAX_PATH];

	A3DTexture *		m_pTextureSurroundF;
	A3DTexture *		m_pTextureSurroundB;
	A3DTexture *		m_pTextureFlat;
	A3DTexture *		m_pTextureCap;
			
protected:
	A3DCOLOR			m_colorSkyBottom;

public:
	A3DSky();
	virtual ~A3DSky();

	bool Init(A3DDevice* pDevice, A3DCameraBase* pCamera, const char* szCap, const char* szSurroundF, const char* szSurroundB, const char* szFlat);
	virtual bool Release();

	virtual bool Render();
	virtual bool TickAnimation();

	virtual bool SetCamera(A3DCameraBase * pCamera);

	inline A3DTexture * GetTextureSurroundF() { return m_pTextureSurroundF; }
	inline A3DTexture * GetTextureSurroundB() { return m_pTextureSurroundB; }
	inline A3DTexture * GetTextureFlat() { return m_pTextureFlat; }
	inline A3DTexture * GetTextureCap() { return m_pTextureCap; }
	inline A3DCOLOR	GetSkyBottomColor()	{ return m_colorSkyBottom; }

	bool SetTextureSurroundF(char * szTextureFile);
	bool SetTextureSurroundB(char * szTextureFile);
	bool SetTextureFlat(char * szTextureFile);
	bool SetTextureCap(char * szTextureFile);
};

typedef A3DSky * PA3DSky;

#endif