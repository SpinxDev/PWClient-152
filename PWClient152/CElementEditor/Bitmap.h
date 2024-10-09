/*
 * FILE: Bitmap.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AString.h"

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

class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

/*
//	Create a 32-bit bitmap
HBITMAP AUX_Create32BitBitmap(int iWidth, int iHeight, void** pData);
//	Create a 8-bit bitmap
HBITMAP AUX_Create8BitBitmap(int iWidth, int iHeight, void** pData, const PALETTEENTRY* aEntries);
//	Create a 32-bit windows bitmap form a texture
HBITMAP AUX_Create32BitBmpFromTexture(A3DTexture* pA3DTexture, void** pBmpData=NULL);
//	Create a 8-bit gray bitmap from a texture
HBITMAP AUX_CreateGrayBmpFromTexture(A3DTexture* pA3DTexture, const PALETTEENTRY* aEntries, void** pBmpData=NULL);
//	Save a bitmap to file
bool AUX_SaveBitmapToFile(const char* szFile, HBITMAP hBitmap, const BYTE* pBmpData, const PALETTEENTRY* aEntries);
*/

///////////////////////////////////////////////////////////////////////////
//
//	Class CELBitmap
//
///////////////////////////////////////////////////////////////////////////

class CELBitmap
{
public:		//	Types

	//	Lock information
	struct LOCKINFO
	{
		int		iWidth;		//	Map size in pixels
		int		iHeight;
		int		iPitch;		//	Map pitch
		BYTE*	pData;		//	Point to the first pixel
	};

public:		//	Constructor and Destructor

	CELBitmap();
	virtual ~CELBitmap();

public:		//	Attributes

public:		//	Operations

	//	Create empty bitmap
	bool CreateBitmap(int iWidth, int iHeight, int iBPP, const PALETTEENTRY* aEntries);
	//	Release object
	void Release();

	//	Load a picture from file and store it as 32-bit bitmap
	bool LoadAs32BitBitmap(const char* szFile, int iWidth, int iHeight);
	//	Load a gray map from file and store it as 8-bit bitmap
	bool LoadAsGrayBitmap(const char* szFile, int iWidth, int iHeight);
	//	Save bitmap data to file
	bool SaveToFile(const char* szFile);
	//	Calculate size of memory buffer need to store whole bitmap file
	int CalcBitmapFileSize();
	//	Save bitmap data to memory buffer
	bool SaveToMemory(BYTE* pBuffer);
	//	Load a gray map from memory buffer
	bool LoadFromMemory(BYTE* pBuffer);

	//	Lock rectangle
	bool LockRect(const ARectI& rc, LOCKINFO* pLockInfo);
	//	Clear bitmap with black color
	void Clear();

	//	Get attributes
	HBITMAP GetBitmap() { return m_hBitmap; }
	BYTE* GetBitmapData() { return m_pBmpData; }
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	int GetBPP() { return m_iBPP; }
	int GetPitch() { return m_iPitch; }
	PALETTEENTRY* GetPalette() { return m_PalEntries; }
	const char* GetFileName() { return m_strFile; }

protected:	//	Attributes

	HBITMAP			m_hBitmap;			//	Bitmap handle
	BYTE*			m_pBmpData;			//	Bitmap data
	int				m_iWidth;			//	Size of bitmap
	int				m_iHeight;
	int				m_iBPP;				//	Bits Per Pixel
	int				m_iPitch;			//	Bytes per line
	PALETTEENTRY	m_PalEntries[256];	//	Palette entries
	AString			m_strFile;			//	Bitmap file name. relative to g_szWorkDir

protected:	//	Operations

	//	Create a non 8-bit bitmap
	bool CreateNon8BitBitmap(int iWidth, int iHeight, int iBPP);
	//	Create a 8-bit bitmap
	bool Create8BitBitmap(int iWidth, int iHeight, const PALETTEENTRY* aEntries);
	//	Create a 32-bit windows bitmap form a texture
	bool Create32BitBmpFromTexture(A3DTexture* pA3DTexture);
	//	Create a 8-bit gray bitmap from a texture
	bool CreateGrayBmpFromTexture(A3DTexture* pA3DTexture, const PALETTEENTRY* aEntries);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

