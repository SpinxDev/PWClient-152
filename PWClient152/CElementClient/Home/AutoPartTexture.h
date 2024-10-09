/*
 * FILE: AutoPartTexture.h
 *
 * DESCRIPTION: Class for generating part texture
 *
 * CREATED BY: Jiang Dalong, 2005/05/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOPARTTEXTURE_H_
#define _AUTOPARTTEXTURE_H_

#include "AutoPartTerrainBase.h"
#include "ClosedArea.h"

class CAutoHome;

class CAutoPartTexture : public CAutoPartTerrainBase  
{
public:
	enum PARTTEXTURETYPE
	{
		PTT_MAIN		= 0,
		PTT_PATCH		= 1,
		PTT_BLEND		= 2,
	};
public:
	CAutoPartTexture(CAutoHome* pAutoHome);
	virtual ~CAutoPartTexture();

public:
	void OnLButtonDown(UINT nFlags, APointI point);
	void OnRButtonUp(UINT nFlags, APointI point);
	void OnRButtonDblClk(UINT nFlags, APointI point);
	bool Render();
	void Release();

	// Create part texture
	bool CreatePartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, DWORD dwSeed, float fBorderScope, int type);
	// Is polygon valid
	bool IsPolygonValid();
	// Set area points
	bool SetPoints(int nNumPoints, POINT_FLOAT* pPoints);
	// Close area
	bool CloseArea();
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	inline CClosedArea* GetArea() { return &m_Area; }

protected:
	CClosedArea			m_Area;
	BYTE*				m_pInArea;
	int					m_nAreaWidth;
	int					m_nAreaHeight;
	POINT				m_ptAreaLeftTop;
	float				m_fTransformBorder;
	
protected:
	bool CalArea();
	void ReleaseArea();
	// Add polygon texture
	bool AddPolygonTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision);
	// Transform area
	bool TransformArea(DWORD dwSeed, float fBorderScope, bool bNoise);
	// Attach single texture on terrain
	bool SingleTexture(float fBorderScope, PARTTEXTURETYPE type);

};

#endif // #ifndef _AUTOPARTTEXTURE_H_
