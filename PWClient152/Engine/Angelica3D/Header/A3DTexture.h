/*
 * FILE: A3DTexture.h
 *
 * DESCRIPTION: Class representing one texture in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTEXTURE_H_
#define _A3DTEXTURE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DObject.h"
#include "AString.h"

class A3DSurface;
class A3DDevice;
class A3DEngine;
class AFile;

#define A3DTF_DETAILMAP			0x00000001
#define A3DTF_LIGHTMAP			0x00000002
#define A3DTF_MIPLEVEL			0x00000004
#define A3DTF_NODOWNSAMPLE		0x00000008
//#define A3DTF_DONOTLOADASSHADER	0x00000100	// 不用了, 参考A3DTF_LOADASMTL
#define A3DTF_CREATEHLSLSHADER	0x00000200
#define A3DTF_TEXTURE2D         0x00000000	// 默认以2D方式载入
#define A3DTF_TEXTURE3D         0x00001000
#define A3DTF_TEXTURECUBE       0x00002000
#define A3DTF_LOADASMTL			0x00004000

class A3DTexture : public A3DObject
{
public:		//	Types

	friend class A3DTextureMan;

public:		//	Operations

	A3DTexture();
	virtual ~A3DTexture();

	virtual bool Init(A3DDevice* pDevice);
	virtual bool Release();
	virtual	bool Reload(bool bForceReload);

	// bool Init(A3DDevice* pDevice, IDirect3DTexture8* pDXTexture);

	virtual bool TickAnimation();
	virtual bool Appear(int nLayer=0);
	virtual bool Disappear(int nLayer=0);

	bool LoadFromFile(A3DDevice* pA3DDevice, const char* szFullpath, const char* szRelativePath);
	bool ExportToFile(const char * szFullpath); 
	//	Load texture file with specified size and format
	bool LoadFromFile(A3DDevice* pA3DDevice, const char* szFile, int iWidth, int iHeight, A3DFORMAT Fmt, int iMipLevel=1, PALETTEENTRY* aPalEntries=NULL);
	//	Load texture form memory with specified size and format
	bool LoadFromMemory(A3DDevice* pA3DDevice, BYTE* pDataBuf, int iDataSize, int iWidth, int iHeight, A3DFORMAT Fmt, int iMipLevel=1, PALETTEENTRY* aPalEntries=NULL);
	
	//	Create a pure color texture whose format is A3DFMT_A8R8G8B8
	static A3DTexture* CreateColorTexture(A3DDevice* pDevice, int iWidth, int iHeight, A3DCOLOR col);
	
	bool Create(A3DDevice * pDevice, int iWidth, int iHeight, A3DFORMAT Format, int nMipLevel=1, DWORD dwUsage = 0);
	bool LockRect(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel=0);
	bool UnlockRect();

	//	Init with specified width height and use the format of a surface, finally, copy 
	//	that part of the surface onto the texture using copy rects;
	bool CreateFromSurface(A3DDevice * pA3DDevice, A3DSurface * pA3DSurface, bool bDynamicTex,A3DRECT& rect, int * pTexWidth, int * pTexHeight, A3DFORMAT fmtSurface=A3DFMT_UNKNOWN);

	//	Get texture dimension
	bool GetDimension(int* piWidth, int* piHeight, int iLevel=0);
	//	Get texture format
	A3DFORMAT GetFormat(int iLevel=0);

	//	Update the texture's content from an A3DSurface;
	bool UpdateFromSurface(A3DSurface * pA3DSurface, A3DRECT& rect);
	void GetMapFileInFolder(const char* szFolderName, AString& strName);

	inline const char* GetMapFile() { return m_strMapFile; }
	inline bool IsAlphaTexture() { return m_bAlphaTexture; }
	inline bool IsShaderTexture() { return m_bShaderTexture; }
	inline IDirect3DTexture9* GetD3DTexture() { return m_pDXTexture; }
	inline void SetIsDetailTexture(bool bFlag) { m_bDetailTexture = bFlag; }
	inline bool IsDetailTexture() { return m_bDetailTexture; }
	inline void SetIsLightMap(bool bFlag) { m_bLightMap = bFlag; }
	inline bool IsLightMap() { return m_bLightMap; }
	inline int GetDesiredMipLevel() { return m_nDesiredMipLevel; }
	inline void SetDesciredMipLevel(int nLevel) { m_nDesiredMipLevel = nLevel; }
	inline DWORD GetTextureID() { return m_dwTextureID; }
	inline void SetNoDownSample(bool bFlag) { m_bNoDownSample = bFlag; }
	inline DWORD GetTimeStamp() { return m_dwTimeStamp; }
	
	//	This function should be used carefully !
	void SetMapFile(const char* szFile);
	virtual IDirect3DBaseTexture9* GetD3DBaseTexture() const;

    //multiThread Render --- begin
    bool LockRectDynamic(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel=0);
    bool UnlockRectDynamic();
    //multiThread Render --- end

protected:	//	Attributes

	IDirect3DTexture9* m_pDXTexture;

	A3DDevice*	m_pA3DDevice;

	// Flag to say whether this is a detail texture;
	bool		m_bDetailTexture;
	int			m_nMipLevel;			// How many mip map really created;
	int			m_nDesiredMipLevel;		// The maximum mip level want to be created;	
	bool		m_bNoDownSample;		// flag indicates don't downsample this texture

	// Flag to say whether this is a lightmap texture;
	bool		m_bLightMap;

	AString		m_strMapFile;		//	Map's file name relative to Angelica's work directory
	DWORD		m_dwTextureID;		//	Texture ID
	DWORD		m_dwPosInMan;		//	List position in texture manager
	int			m_iTexDataSize;		//	Texture data size in bytes

	bool		m_bHWITexture;		//	flag to indicate whether the device is not create at all;

	bool		m_bAlphaTexture;
	bool		m_bShaderTexture;	//	Flag indicates whether this is a multi-stage shader
	bool		m_bDDSTexture;		//	true, is DDS texture

	DWORD		m_dwTimeStamp;		//	time stamp of the file from which the texture loaded

    int			m_nWidth;
    int			m_nHeight;
    DWORD       m_dwBitsPerPixel;


	UINT		m_nLockLevel;
	IDirect3DSurface9* m_pLockSurface;
	RECT		m_rcLock;
	BOOL		m_bUseRect;
	DWORD		m_dwUsage;

protected:	//	Operations
	
	bool LoadNormalTexture(BYTE* pDataBuf, int iDataLen, const char* szRelativePath);
	bool LoadDetailTexture(BYTE* pDataBuf, int iDataLen, const char* szRelativePath);
	bool LoadLightMapTexture(BYTE* pDataBuf, int iDataLen, const char* szRelativePath);

	bool CreateDXTexture(int nWidth, int nHeight, A3DFORMAT format, int nMipLevel, DWORD dwUsage = 0);
	virtual void CalcTextureDataSize();

	static inline int GetMaxMipLevel(int nWidth, int nHeight)
	{
		int	nMaxLevel = 1;
		int	nMipSize = 1;
		while (nMipSize < nWidth && nMipSize < nHeight)
		{
			nMaxLevel ++;
			nMipSize *= 2;
		}
		return nMaxLevel;
	}
    bool UpdateFromSurfaceDynamic(A3DSurface * pA3DSurface, A3DRECT& rect);

};

class A3DTextureProxy
{
public:

	A3DTextureProxy();
	A3DTextureProxy(A3DEngine* pA3DEngine, IDirect3DTexture9* pDXTexture);

	void SetA3DDevice(A3DDevice* pA3DDevice) { m_pA3DDevice = pA3DDevice; }

	IDirect3DTexture9* SetDXTexture(IDirect3DTexture9* pDXTexture);
	IDirect3DTexture9* GetD3DTexture() { return m_pDXTexture; }

	//	Apply texture to device
	bool Appear(int iLayer);

protected:

	A3DDevice*			m_pA3DDevice;
	IDirect3DTexture9*	m_pDXTexture;
};


#endif

