/*
 * FILE: TerrainPage.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "RollUpWnd.h"
#include "TerrainModifyPanel.h"
#include "TerrainModifyStretchPanel.h"
#include "TerrainModifyNoisePanel.h"
#include "TerrainModifyLayMaskPanel.h"
#include "TerrainModifySmoothPanel.h"
#include "terrainModifyLayMaskPanel.h"
#include "terrainModifyLayMaskSmoothPanel.h"
#include "TerrainModifyPlantsPanel.h"


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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainPage
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainPage : public CRollUpWnd
{
public:		//	Types

	//	Index of panels
	enum
	{
		PANEL_MODIFY = 0,
		PANEL_MODIFYSTRETCH,
		PANEL_MODIFYNOISE,
		PANEL_MODIFYSMOOTH,
		PANEL_MODIFYLAYMASK,
		PANEL_MODIFYLAYSMOOTH,
		PANEL_MODIFYPLANTS,
		NUM_PANEL,
	};

public:		//	Constructor and Destructor

	CTerrainPage();
	virtual ~CTerrainPage();

public:		//	Attributes
	CTerrainModifyLayMaskPanel m_TerrainModifyLayMaskPanel;
	CTerrainModifyLayMaskSmoothPanel m_TerrainModifyLayMaskSmoothPanel;
public:		//	Operations

	//	Release resources
	void Release();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainPage)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	
protected:	//	Attributes

	//Added by QingFeng Xin
	CTerrainModifyPanel m_TerrainModifyPanel;
	CTerrainModifyStretchPanel m_TerrainModifyStretchPanel;
	CTerrainModifyNoisePanel m_TerrainModifyNoisePanel;
	CTerrainModifySmoothPanel m_TerrainModifySmoothPanle;
	CTerrainModifyPlantsPanel m_TerrainModifyPlants;
	
	
	
	//End 

protected:	//	Operations

	//{{AFX_MSG(CTerrainPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

