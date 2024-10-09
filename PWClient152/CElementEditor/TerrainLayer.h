/*
 * FILE: TerrainLayer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AString.h"
#include "AArray.h"

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

class CELBitmap;
class CELArchive;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainLayer
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainLayer
{
public:		//	Types

	//	Mask map type
	enum
	{
		MASK_NONE = -1,
		MASK_FROMFILE = 0,
		MASK_AUTO,
		MASK_EDIT,
	};

	friend class CTerrain;

public:		//	Constructor and Destructor

	CTerrainLayer(CTerrain* pTerrain);
	virtual ~CTerrainLayer();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	bool Init(const char* szName, bool bDefault);
	//	Release object
	void Release();

	//	Load data
	bool Load(CELArchive& ar, int iLayerIdx);
	//	Save data
	bool Save(CELArchive& ar, int iLayerIdx);

	//	Change texture
	bool ChangeTexture(const char* szFile);
	//	Change specular map
	bool ChangeSpecularMap(const char* szFile);
	//	Change mask map type
	void ChangeMaskType(int iType);
	//	Update auto mask map content
	bool UpdateAutoMaskMap(int iNewSize);
	//	Load mask texture from file
	bool LoadMaskMap(const char* szFile);
	//	Edit mask content directly
	bool EditMaskMap(const ARectI& rcArea, int* pModifyData);

	//	Set / Get layer name
	void SetName(const char* szName) { m_strName = szName; }
	const char* GetName() { return m_strName; }
	//	Set / Get scale on U axis
	void SetScaleU(float s) { m_fScaleU = s; }
	float GetScaleU() { return m_fScaleU; }
	//	Set / Get scale on V axis
	void SetScaleV(float s) { m_fScaleV = s; }
	float GetScaleV() { return m_fScaleV; }
	//	Set / Get altitude start
	void SetAltiStart(float fStart) { m_fAltiStart = fStart; }
	float GetAltiStart() { return m_fAltiStart; }
	//	Set / Get altitude end
	void SetAltiEnd(float fEnd) { m_fAltiEnd = fEnd; }
	float GetAltiEnd() { return m_fAltiEnd; }
	//	Set / Get slope start
	void SetSlopeStart(float fStart) { m_fSlopeStart = fStart; }
	float GetSlopeStart() { return m_fSlopeStart; }
	//	Set / Get slope end
	void SetSlopeEnd(float fEnd) { m_fSlopeEnd = fEnd; }
	float GetSlopeEnd() { return m_fSlopeEnd; }
	//	Set / Get projection axis
	void SetProjAxis(int iAxis) { m_iProjAxis = iAxis; }
	int GetProjAxis() { return m_iProjAxis; }
	//	Set / Get enable flag
	void Enable(bool bEnable) { m_bEnable = bEnable; }
	bool IsEnable() { return m_bEnable; }
	//	Get mask map type
	int GetMaskType() { return m_iMaskMapType; }
	//	Is default layer ?
	bool IsDefault() { return m_bDefault; }
	//	Get auto mask map size
	int GetAutoMaskMapSize() { return m_iAutoMaskSize; }
	//	Get mask bitmap
	CELBitmap* GetMaskBitmap() { return m_pMaskBmp; }
	//	Get texture file name
	const char* GetTextureFile() { return m_strTexFile; }
	//	Get specular map file name
	const char* GetSpecularMapFile() { return m_strSpecuMap; }
	//	This layer has texture ?
	bool HasTexture() { return m_strTexFile.GetLength() ? true : false; }
	//	This layer has specular map
	bool HasSpecularMap() { return m_strSpecuMap.GetLength() ? true : false; }
	//	Set auto mask smooth flag
	void SetAutoMaskSmoothFlag(bool bSmooth) { m_bAutoMaskSmth = bSmooth; }
	//	Get auto mask smooth flag
	bool GetAutoMaskSmoothFlag() { return m_bAutoMaskSmth; }
	//	Get axis flag used to generate auto mask map
	DWORD GetAutoMaskAxisFlag() { return m_dwAutoMaskAxis; }
	//	Set axis flag used to generate auto mask map
	void SetAutoMaskAxisFlag(DWORD dwFlag) { m_dwAutoMaskAxis = dwFlag; }
	//	Set layer's weight
	void SetLayerWeight(int iWeight) { m_iWeight = iWeight; }
	//	Get layer's weight
	int GetLayerWeight() { return m_iWeight; }

	//	Set auto mask map size, this function is only used by CDlgSurfaces::OnSufFileLoad !
	void SetAutoMaskMapSize(int iSize) { m_iAutoMaskSize = iSize; }
	//	Create mask map object
	bool CreateMaskMap(int iSize);

protected:	//	Attributes

	CTerrain*	m_pTerrain;			//	Terrain object
	
	AString		m_strName;			//	Layer name
	AString		m_strTexFile;		//	Texture file name. relative to g_szWorkDir
	AString		m_strSpecuMap;		//	Specular map file name. relative to g_szWorkDir
	float		m_fScaleU;			//	Scale on U axis
	float		m_fScaleV;			//	Scale on V axis
	int			m_iProjAxis;		//	Projection axis
	bool		m_bEnable;			//	Enable flag
	bool		m_bDefault;			//	true, this is default layer
	int			m_iWeight;			//	Layer's weight

	CELBitmap*	m_pMaskBmp;			//	Auto mask bitmap
	int			m_iMaskMapType;		//	Mask map type
	bool		m_bAutoMask;		//	true, automatically generate layer mask
	bool		m_bAutoMaskSmth;	//	Auto mask smooth flag
	int			m_iAutoMaskSize;	//	Auto mask size
	float		m_fAltiStart;		//	Altitude start
	float		m_fAltiEnd;			//	Altitude end
	float		m_fSlopeStart;		//	Slope start
	float		m_fSlopeEnd;		//	Slope end
	DWORD		m_dwAutoMaskAxis;	//	Axis flag used to generate auto mask

protected:	//	Operations


	//	Release mask map
	void ReleaseMaskMap();
	//	Save mask map data
	bool SaveMaskMap(CELArchive& ar, int iLayerIdx);
	bool CheckLayerValid();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

