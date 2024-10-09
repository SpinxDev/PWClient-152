/*
 * FILE: EC_PateText.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/4/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_PateText.h"
#include "EC_Decal.h"
#include "EC_Game.h"
#include "EC_Resource.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManDecal.h"
#include "EC_UIHelper.h"
#include "EC_ImageDrawing.h"

#include "A3DFTFont.h"
#include "A3DFTFontMan.h"

#include "A3DFont.h"
#include "A3DFontMan.h"

#include "A3DEngine.h"
#include "AUITextArea.h"
#include "AUICommon.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECPateText
//	
///////////////////////////////////////////////////////////////////////////

CECPateText::CECPateText()
{
	m_iExtX			= 0;
	m_iExtY			= 0;
	m_iLines		= 0;
	m_iMaxLineLen	= 30;
	m_iMaxLines		= 5;
	m_bBorder		= false;
	m_bIsHover		= false;
	m_pFlatCollector = new FWFlatCollector();	
	m_pFlatCollector->Init(g_pGame->GetA3DEngine());
	m_pBoard	= new CECBarDrawing();

	m_BoardRange.x = 0;
	m_BoardRange.y = 0;
	m_BoardRange.width = 0;
	m_BoardRange.height = 0;
}

CECPateText::~CECPateText()
{
	CECPateText::Clear();
	A3DRELEASE(m_pFlatCollector);
	delete m_pBoard;
}

/*	Set text string

	Return the number of characters in szText which was added into pate text.

	bIncEmotion: should parse emotion keyword
	bEllipsis: true, add ellipsis at the end of pate text if szText is too long
*/
int CECPateText::SetText(const ACHAR* szText, bool bIncEmotion, bool bEllipsis/* true */, CECIvtrItem *pIvtrItem/* NULL */, ACString *pstrTextConverted/* NULL */)
{
	//	Clear old content
	Clear();
	
	if (!szText || !szText[0])
		return true;
	
	
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ACString str = pGameUI->AUI_FilterEditboxItem(szText, CECGameUIMan::AUI_EditboxItemMaskFilter(1 << enumEICoord));

	ACString strName;
	A3DCOLOR clrName;
	pGameUI->TransformNameColor(pIvtrItem, strName, clrName);
	str = UnmarshalEditBoxText(str, m_ItemsSet, 0, strName, clrName);
	szText = str;
	if (pstrTextConverted){
		*pstrTextConverted = str;
	}

	int iAddedChar = 0;

	if (!bIncEmotion)
	{
		int iLen = a_strlen(szText);
		if (iLen > m_iMaxLineLen)
		{
			ACString str(szText, m_iMaxLineLen);

			//	Add '...' at string ending
			if (bEllipsis)
				str += _AL("...");

			CreateTextItem(str, -1, 0);
			iAddedChar = m_iMaxLineLen;
		}
		else
		{
			CreateTextItem(szText, -1, 0);
			iAddedChar = iLen;
		}
	}
	else	//	Should parse emotion keyword
	{
		int i=0, iStart=0, iEnd=0, iSkip=0, iLenCnt=0;
		bool bTooLong = false;
		int iLine = 0;

		while (szText[i])
		{
			ACHAR ch = szText[i];

			if (IsEditboxItemCode(ch))
			{
				if (iEnd > iStart)
					CreateTextItem(&szText[iStart], iEnd-iStart, iLine);

				EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(ch);

				if (pItem)
				{
					if (pItem->GetType() == enumEIEmotion)
					{
						int nSet = 0, nIndex = 0;
						UnmarshalEmotionInfo(pItem->GetInfo(), nSet, nIndex);
						CreateEmotionItem(nSet, nIndex, iLine);
						iLenCnt += 2;	//	a emotion take two-character place
					}
					else
					{
						const ACHAR* szName = pItem->GetName();
						CreateTextItem(szName, -1, iLine, pItem->GetColor());
						iLenCnt += wcslen(szName);
					}
				}

				i ++;
				iStart = i;
				iEnd = i;
				goto CheckLength;
			}

			iEnd++;
			i++;
			iLenCnt++;

		CheckLength:

			if (iLenCnt > m_iMaxLineLen)
			{
				if (iLine + 1 >= m_iMaxLines)
				{
					bTooLong = true;
					break;
				}

				if (iEnd > iStart)
					CreateTextItem(&szText[iStart], iEnd-iStart, iLine);

				iStart = i;
				iLine++;
				iLenCnt = 0;
			}
		}

		iAddedChar = i;

		if (iEnd > iStart)
		{
			if (bTooLong)
			{
				ACString strEnd(&szText[iStart], iEnd-iStart);

				//	Add '...' at string ending
				if (bEllipsis)
					strEnd += _AL("...");

				CreateTextItem(strEnd, -1, iLine);
			}
			else
				CreateTextItem(&szText[iStart], iEnd-iStart, iLine);
		}
		else if (bTooLong)
		{
			//	Add '...' at string ending
			if (bEllipsis)
				CreateTextItem(_AL("..."), -1, iLine);
		}
	}

	//	Calculate whole extent
	m_iExtX = 0;
	m_iExtY = 0;

	int iCurLine = 0;
	int iLineExtX = 0;
	int iLastLine = 0;

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		const ITEM& Item = m_aItems[i];

		if (Item.iLine != iLastLine)
		{
			iLastLine = Item.iLine;

			if (m_iExtX < iLineExtX)
				m_iExtX = iLineExtX;

			iLineExtX = Item.iExtX;
		}
		else
			iLineExtX += Item.iExtX;

		if (m_iExtY < Item.iExtY)
			m_iExtY = Item.iExtY;
	}

	m_iLines = iLastLine + 1;

	if (m_iExtX < iLineExtX)
		m_iExtX = iLineExtX;

	return iAddedChar;
}

//	Create a text item
bool CECPateText::CreateTextItem(const ACHAR* szText, int iNumChar, int iLine, A3DCOLOR cl)
{
	ASSERT(szText && szText[0]);

	ACString* pstr = NULL;
	if (iNumChar <= 0)
		pstr = new ACString(szText);
	else
		pstr = new ACString(szText, iNumChar);

	int iIndex = m_aTextStrs.Add(pstr);

	ITEM Item;
	Item.iType	= TYPE_TEXT;
	Item.iIndex	= iIndex;
	Item.iLine	= iLine;
	Item.clItem	= cl;
	
	AUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr();
	if (!pUIMan){
		assert (pUIMan);
		return false;
	}
	A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);
	A3DFontMan* pFontMan = g_pGame->GetA3DEngine()->GetA3DFontMan();
	A3DFTFont* pUIFont = pUIMan->GetPateFont();
	if (pFont && pFontMan)
	{
		//	Get string extent
		pFontMan->GetTextExtent(pFont->GetA3DFont(), *pstr, pstr->GetLength(), &Item.iExtX, &Item.iExtY);
	}
	else if( pUIFont )	
	{
		A3DPOINT2 sz;
		sz = pUIFont->GetTextExtent(*pstr);
		Item.iExtX  = sz.x;
		Item.iExtY	= sz.y;
	}
	else
	{
		assert(false);
		return false;
	}

	// 计算当scale＝1时大小
	float fScale = GetRenderScale();
	Item.iExtX = int (Item.iExtX/fScale);
	Item.iExtY = int (Item.iExtY/fScale);

	m_aItems.Add(Item);

	return true;
}

//	Get face index
int CECPateText::GetFaceIndex(const ACHAR* szText, int& iSkipChar)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (!pGameUI)
		return -1;

	A2DSprite* pFaceImg = NULL;
	abase::vector<AUITEXTAREA_EMOTION>* pAreaList = NULL;
	pGameUI->GetEmotionList(&pFaceImg, &pAreaList);
	if (!pFaceImg || !pAreaList)
		return -1;

	int iFace = -1;
	if (iFace < 0 || iFace >= (int)pAreaList->size())
		return -1;

	iSkipChar = 1;

	return iFace;
}

//	Create a emotion item
bool CECPateText::CreateEmotionItem(int nEmotionSet, int iFace, int iLine)
{
	CECFaceDecal* pDecal = new CECFaceDecal;
	if (!pDecal->Init(nEmotionSet, iFace))
	{
		delete pDecal;
		return false;
	}

	//	Add to emotion list
	int iIndex = m_aEmotions.Add(pDecal);

	ITEM Item;
	Item.iType	= TYPE_EMOTION;
	Item.iIndex	= iIndex;
	Item.iLine	= iLine;

	//	Get decal extent
	pDecal->GetExtent(&Item.iExtX, &Item.iExtY);

	m_aItems.Add(Item);

	return true;
}

//	Clear whole text string
void CECPateText::Clear()
{
	int i;

	for (i=0; i < m_aTextStrs.GetSize(); i++)
		delete m_aTextStrs[i];

	m_aTextStrs.RemoveAll(false);

	for (i=0; i < m_aEmotions.GetSize(); i++)
		delete m_aEmotions[i];

	m_aEmotions.RemoveAll(false);
	m_aItems.RemoveAll(false);
	m_ItemsSet.Release();
}

//	Tick routine
bool CECPateText::Tick(DWORD dwDeltaTime)
{
	//	Tick emotion animation
	for (int i=0; i < m_aEmotions.GetSize(); i++)
		m_aEmotions[i]->Tick(dwDeltaTime);

	return true;
}

float CECPateText::GetRenderScale()
{
	float fScale(1.0f);

	// 只有使用界面字体的才缩放
	//
	if (g_pGame)
	{
		A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);
		if (!pFont)
		{
			if (g_pGame->GetGameRun() &&
				g_pGame->GetGameRun()->GetUIManager())
			{
				AUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr();
				if (pUIMan)
				{
					fScale = pUIMan->GetWindowScale();
				}
			}
		}
	}

	return fScale;
}

//	Render routine
bool CECPateText::Render(CECViewport* pViewport, int sx, int sy, DWORD dwCol, float sz)
{
	AUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr();
	assert(pUIMan);
	A3DFTFont* pUIFont = pUIMan->GetPateFont();
	A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);

	float fScale = GetRenderScale();

	int i, x = sx, y = sy;
	float z = m_bIsHover ? 0 : sz;  //若此时被选中，在最上方渲染
	int iLine = 0;
	

	for (i=0; i < m_aItems.GetSize(); i++)
	{
		const ITEM& Item = m_aItems[i];

		if (Item.iLine != iLine)
		{
			x = sx;
			y += int(m_iExtY * fScale);
			iLine = Item.iLine;
		}

		if (Item.iType == TYPE_TEXT)
		{
			ACString* pstr = m_aTextStrs[Item.iIndex];
			
			if (m_bIsHover)	
			{
				m_pBoard->DrawingBoard(m_BoardRange.x, m_BoardRange.y, m_BoardRange.width, m_BoardRange.height, z);
			}
			if (pFont)
			{
				// draw normal text here
				pFont->TextOut(x, y, *pstr, Item.clItem ? Item.clItem : dwCol, 0, z, m_bBorder);
			}
			else if( pUIFont )
			{
				// draw normal text here
				pUIFont->TextOut(x, y, *pstr, Item.clItem ? Item.clItem : dwCol, true, A3DCOLORRGBA(0, 0, 0, 255), z);
			}		
		}
		else if (Item.iType == TYPE_EMOTION)	//	TYPE_EMOTION
		{
			CECFaceDecal* pDecal = m_aEmotions[Item.iIndex];
			pDecal->SetScreenPos(x, y);
			pDecal->SetZValue(z);
			pDecal->SetScaleX(fScale);
			pDecal->SetScaleY(fScale);
			pDecal->Render(pViewport);
		}
		else if (Item.iType == TYPE_BOOTHNAME)
		{
			ACString* pstr = m_aTextStrs[Item.iIndex];	
			if (HasPateImage())
			{
				float imgZ = m_bIsHover? 0 : m_RegRender.z;
				m_pBoard->DrawingBoard(m_BoardRange.x, m_BoardRange.y, m_BoardRange.width, m_BoardRange.height, imgZ);
			}
			if (pFont)
			{
				// draw normal text here
				pFont->TextOut(x, y, *pstr, Item.clItem ? Item.clItem : dwCol, 0, z, m_bBorder);
			}
		}
		x += int(Item.iExtX * fScale);
	}
		
	return true;
}

//	Register render
bool CECPateText::RegisterRender(int sx, int sy, DWORD dwCol, float z)
{
	m_RegRender.x	= sx;
	m_RegRender.y	= sy;
	m_RegRender.z	= z;
	m_RegRender.col	= dwCol;
	CalculateBoardRange(sx,sy,m_iExtX,m_iExtY*m_iLines, m_pBoard);
	g_pGame->GetGameRun()->GetWorld()->GetDecalMan()->RegisterPateText(this);
	return true;
}

bool CECPateText::IsMouseInRect( int x, int y )
{
	int offsetX;
	int offsetY;
	if (HasPateImage()){
		offsetX = x - m_BoardRange.x;
		offsetY = y - m_BoardRange.y;
		return ((offsetX >= 0 && offsetX <= m_BoardRange.width)&&(offsetY >= 0 && offsetY <= m_BoardRange.height));
	}else{
		offsetX = x - m_RegRender.x;
		offsetY = y - m_RegRender.y;
		return ((offsetX >= 0 && offsetX <= m_iExtX)&&(offsetY >= 0 && offsetY <= m_iExtY*m_iLines));		
	}	
}

int CECPateText::SetBoothContent(const ACHAR* szText)
{
	//	Clear old content
	Clear();
	
	if (!szText || !szText[0])
		return true;
	
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ACString str = pGameUI->AUI_FilterEditboxItem(szText, CECGameUIMan::AUI_EditboxItemMaskFilter(1 << enumEICoord));
	
	ACString strName;
	
	szText = str;
	int iAddedChar = 0;
	CreateBoothBoard(szText,-1,0);

	//	Calculate whole extent
	m_iExtX = 0;
	m_iExtY = 0;
	
	int iCurLine = 0;
	int iLineExtX = 0;
	int iLastLine = 0;
	
	for (int i=0; i < m_aItems.GetSize(); i++){
		const ITEM& Item = m_aItems[i];	
		if (Item.iLine != iLastLine){
			iLastLine = Item.iLine;
			
			if (m_iExtX < iLineExtX)
				m_iExtX = iLineExtX;
			
			iLineExtX = Item.iExtX;
		}
		else
			iLineExtX += Item.iExtX;
		
		if (m_iExtY < Item.iExtY)
			m_iExtY = Item.iExtY;
	}
	
	m_iLines = iLastLine + 1;
	
	if (m_iExtX < iLineExtX)
		m_iExtX = iLineExtX;
	return iAddedChar;
}

bool CECPateText::CreateBoothBoard( const ACHAR* szText, int iNumChar, int iLine, A3DCOLOR cl /*= 0*/ )
{
	ASSERT(szText && szText[0]);
	
	ACString* pstr = NULL;
	if (iNumChar <= 0)
		pstr = new ACString(szText);
	else
		pstr = new ACString(szText, iNumChar);
	
	int iIndex = m_aTextStrs.Add(pstr);
	
	ITEM Item;
	Item.iType	= TYPE_BOOTHNAME;
	Item.iIndex	= iIndex;
	Item.iLine	= iLine;
	Item.clItem	= cl;
	
	AUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr();
	if (!pUIMan){
		assert (pUIMan);
		return false;
	}
	A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);
	A3DFontMan* pFontMan = g_pGame->GetA3DEngine()->GetA3DFontMan();
	A3DFTFont* pUIFont = pUIMan->GetPateFont();
	if (pFont && pFontMan){
		//	Get string extent
		pFontMan->GetTextExtent(pFont->GetA3DFont(), *pstr, pstr->GetLength(), &Item.iExtX, &Item.iExtY);
	}else if( pUIFont )	{
		A3DPOINT2 sz;
		sz = pUIFont->GetTextExtent(*pstr);
		Item.iExtX  = sz.x;
		Item.iExtY	= sz.y;
	}else{
		assert(false);
		return false;
	}
	
	// 计算当scale＝1时大小
	float fScale = GetRenderScale();
	Item.iExtX = int (Item.iExtX/fScale);
	Item.iExtY = int (Item.iExtY/fScale);
	
	m_aItems.Add(Item);
	
	return true;
}

void CECPateText::SetBarImage( const char* szFile )
{
	m_pBoard->InitialImage(szFile);
}

bool CECPateText::HasPateImage()const{
	return m_pBoard->HasImage();
}

void CECPateText::CalculateBoardRange( int x, int y, int sizex, int sizey, CECBarDrawing* pImage )
{
	int iWidth = pImage->GetWidth() / 3;	//九宫格单位长度
	int iHeight = pImage->GetHeight() / 3;
	
	int bordersize = 20;
	int cx = sizex - 2*iWidth + bordersize;
	cx = cx > iWidth ? cx : iWidth;   // 不能比原始图片长度更短
	int	cy = sizey;
	x -= (cx + 2*iWidth - sizex) / 2;
	y -= iHeight;

	m_BoardRange.x = x;
	m_BoardRange.y = y;
	m_BoardRange.width = cx + 2*iWidth;
	m_BoardRange.height = cy + 2*iHeight;
}