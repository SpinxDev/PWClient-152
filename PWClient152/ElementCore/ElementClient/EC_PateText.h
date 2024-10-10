/*
 * FILE: EC_PateText.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/4/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "AAssist.h"
#include "AUICommon.h"
#include "../../CFWEditorLib/FWFlatCollector.h"

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

class CECViewport;
class CECFaceDecal;
class CECIvtrItem;
class CECBarDrawing;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPateText
//	
///////////////////////////////////////////////////////////////////////////

class CECPateText
{
public:		//	Types

	//	Text item type
	enum
	{
		TYPE_TEXT = 0,
		TYPE_EMOTION,
		TYPE_BOOTHNAME,
	};

	//	Text item
	struct ITEM
	{
		int	iType;		//	Text type
		int	iIndex;		//	Index of item
		int iExtX;		//	Extent
		int iExtY;
		int	iLine;
		A3DCOLOR clItem;
	};

	//	Register render info
	struct REGRENDER
	{
		int		x;
		int		y;
		DWORD	col;
		float	z;
	};

	struct BGRANGE
	{
		int x;
		int y;
		int width;
		int height;
	};

public:		//	Constructor and Destructor

	CECPateText();
	virtual ~CECPateText();

public:		//	Attributes

public:		//	Operations

	//	Set text string
	int SetText(const ACHAR* szText, bool bIncEmotion, bool bEllipsis=true, CECIvtrItem *pIvtrItem=NULL, ACString *pstrTextConverted=NULL);
	//	Clear whole text string
	void Clear();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport, int sx, int sy, DWORD dwCol, float sz);
	//	Register render
	bool RegisterRender(int sx, int sy, DWORD dwCol, float z);

	//	Get whole text extent
	void GetExtent(int* pix, int* piy) { *pix = m_iExtX; *piy = m_iExtY * m_iLines; }
	//	Get item number
	int GetItemNum() { return m_aItems.GetSize(); }
	//	Set / Get maximum character length
	void SetMaxLen(int iLen) { m_iMaxLineLen = iLen; }
	int GetMaxLen() { return m_iMaxLineLen; }
	//	Enable border
	void EnableBorder(bool bTrue) { m_bBorder = bTrue; }
	//	Get border flag
	bool GetBorderFlag() { return m_bBorder; }
	//	Get register render info
	const REGRENDER& GetRegRender() { return m_RegRender; }
	
	static float GetRenderScale();

	//  判定鼠标位置是否在名字上方
	bool IsMouseInRect(int x, int y);

	void SetIsHover(bool bHover) { m_bIsHover = bHover; }

	int SetBoothContent(const ACHAR* szText);

	void SetBarImage(const char* szFile);

	void CalculateBoardRange(int x, int y, int sizex, int sizey, CECBarDrawing* pImage);

	bool HasPateImage()const;

protected:	//	Attributes

	APtrArray<ACString*>		m_aTextStrs;	//	Text strings
	APtrArray<CECFaceDecal*>	m_aEmotions;	//	Emotion array
	AArray<ITEM, ITEM&>			m_aItems;		//	Items
	EditBoxItemsSet				m_ItemsSet;

	int		m_iExtX;	//	Whole pate text extent
	int		m_iExtY;
	int		m_iLines;
	int		m_iMaxLineLen;	//	Maximum character length
	int		m_iMaxLines;
	bool	m_bBorder;	//	Draw a 1-pixel width black border
	bool	m_bIsHover;

	REGRENDER	m_RegRender;	//	Register render information
	BGRANGE	m_BoardRange;
	FWFlatCollector* m_pFlatCollector;
	CECBarDrawing* m_pBoard;

protected:	//	Operations

	//	Create a text item
	bool CreateTextItem(const ACHAR* szText, int iNumChar, int iLine, A3DCOLOR cl = 0);
	//	Create a emotion item
	bool CreateEmotionItem(int nEmotionSet, int iFace, int iLine);
	//	Get face index
	int GetFaceIndex(const ACHAR* szText, int& iSkipChar);

	bool CreateBoothBoard(const ACHAR* szText, int iNumChar, int iLine, A3DCOLOR cl = 0);
	

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


