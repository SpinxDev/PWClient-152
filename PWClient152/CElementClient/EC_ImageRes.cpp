/*
 * FILE: EC_ImageRes.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_ImageRes.h"

#include "A3DTypes.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DEngine.h"
#include "A2DSpriteBuffer.h"

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
//	Implement CECImageRes
//	
///////////////////////////////////////////////////////////////////////////

CECImageRes::CECImageRes()
{
	memset(m_aImages, 0, sizeof (m_aImages));
}

CECImageRes::~CECImageRes()
{
}

//	Load all images
bool CECImageRes::LoadAllImages()
{
	int i;
	A3DRECT aRects[64];

	//	Popup number
	for (i=0; i < 10; i++)
		aRects[i] = A3DRECT(i*15, 0, i*15+15, 20);

	LoadImage(IMG_POPUPNUM, "InGame\\头顶数字.tga", 150, 20, 10, aRects, 64);

	//	Temporary faction icons
	for (i=0; i < 5; i++)
		aRects[i] = A3DRECT(i*16, 0, i*16+16, 16);

	LoadImage(IMG_FACTION, "InGame\\Faction.bmp", 100, 16, 5, aRects, 30);
	
	//	Pate quest icons
	for (i=0; i < 14; i++)
		aRects[i] = A3DRECT(i*30, 0, i*30+30, 30);

	LoadImage(IMG_PATEQUEST, "InGame\\PateQuest.tga", 420, 30, 14, aRects, 16);

	//	Booth name's background
/*	for (i=0; i < 3; i++)
	{
		for (int j=0; j < 3; j++)
			aRects[i*3+j] = A3DRECT(j*5, i*5, j*5+5, i*5+5);
	}

	LoadImage(IMG_BOOTHBAR, "InGame\\boothbar.tga", 15, 15, 9, aRects, 128);
*/
	//	Other images ...
	LoadImage(IMG_HITMISSED, "InGame\\未命中.tga", 0, 0, 10);
	LoadImage(IMG_LEVELUP, "InGame\\升级了.tga", 0, 0);
	LoadImage(IMG_GOTEXP, "InGame\\经验.tga", 0, 0);
	LoadImage(IMG_GOTMONEY, "InGame\\金钱.tga", 0, 0);
	LoadImage(IMG_DEADLYSTRIKE, "InGame\\爆击.tga", 0, 0);
	LoadImage(IMG_GOTSP, "InGame\\元神.tga", 0, 0);
	LoadImage(IMG_INVALIDHIT, "InGame\\无效.tga", 0, 0);
	LoadImage(IMG_TEAMLEADER, "Window\\LeaderMark.tga", 0, 0);
	LoadImage(IMG_HPWARN, "InGame\\hp_warn.tga", 0, 0);
	LoadImage(IMG_MPWARN, "InGame\\mp_warn.tga", 0, 0);
	LoadImage(IMG_RETORT, "InGame\\反震.tga", 0, 0);
	LoadImage(IMG_IMMUNE, "InGame\\免疫.tga", 0, 0);
	LoadImage(IMG_TEAMMATE, "Window\\Teammate.tga", 0, 0);
	LoadImage(IMG_PKSTATE, "InGame\\PK状态标记.tga", 0, 0);
	LoadImage(IMG_GMFLAG, "InGame\\GM标志.dds", 0, 0);
	LoadImage(IMG_ATTACKLOSE, "InGame\\失败.tga", 0, 0);
	LoadImage(IMG_SUCCESS, "InGame\\成功.tga", 0, 0);

	LoadImage(IMG_REBOUND, "InGame\\复仇惩戒.tga", 0, 0);
	LoadImage(IMG_BEAT_BACK, "InGame\\复仇镜像.tga", 0, 0);
	LoadImage(IMG_ADD, "InGame\\吸血.tga", 0, 0);
	LoadImage(IMG_DODGE_DEBUFF, "InGame\\状态闪避.tga", 0, 0);

	LoadImage(IMG_KING, "King\\皇冠图标.tga", 0, 0);

	return true;
}

/*	Load image which has only one item

	Return true for success, otherwise return false.

	dwSlot: slot will be used by the image
	szFile: image file name
	iWidth, iHeight: iamge size
	iNumElem: element number used to initialize image buffer
*/
bool CECImageRes::LoadImage(DWORD dwSlot, char* szFile, int iWidth, int iHeight, int iNumElem/* 0 */)
{
	A3DRECT Rect(0, 0, iWidth, iHeight);
	if (!LoadImage(dwSlot, szFile, iWidth, iHeight, 1, &Rect, iNumElem))
		return false;

	if (!iWidth || !iHeight)
	{
		IMAGE* pSlot = &m_aImages[dwSlot];
		if (pSlot->pImage)
		{
			A3DRECT rect(0, 0, pSlot->iWidth, pSlot->iHeight);
			pSlot->pImage->ResetItems(1, &rect);
		}
	}

	return true;
}

/*	Load image from file

	Return true for success, otherwise return false.

	dwSlot: slot will be used by the image
	szFile: image file name
	iWidth, iHeight: iamge size
	iNumItem: number of image item in this image
	aRects: rectanges for image item
*/
bool CECImageRes::LoadImage(DWORD dwSlot, char* szFile, int iWidth, int iHeight,
							int iNumItem, A3DRECT* aRects, int iNumElem/* 0 */)
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();

	IMAGE* pSlot = &m_aImages[dwSlot];
	if (pSlot->pImage)
	{
		a_LogOutput(1, "CECImageRes::LoadImage, Slot %d has been used by other image", dwSlot);
		return false;
	}

	A2DSprite* pImage = new A2DSprite;
	if (!pImage)
	{
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to create A2DSprite object");
		return false;
	}

	if (!pImage->Init(pA3DDevice, szFile, iWidth, iHeight, 0, iNumItem, aRects))
	{
		delete pImage;
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to initialize A2DSprite object from file %s", szFile);
		return false;
	}

	pSlot->pImage		= pImage;
	pSlot->iNumItem		= iNumItem;
	pSlot->iWidth		= pImage->GetWidth();
	pSlot->iHeight		= pImage->GetHeight();
	pSlot->iCurItem		= -1;
	pSlot->pImageBuf	= NULL;

	if (iNumElem)
	{
		if (!(pSlot->pImageBuf = new A2DSpriteBuffer))
		{
			a_LogOutput(1, "CECImageRes::LoadImage, Failed to new A2DSpriteBuffer");
		}
		else if (!pSlot->pImageBuf->Init(pA3DDevice, pSlot->pImage->GetTextures(), iNumElem))
		{
			delete pSlot->pImageBuf;
			pSlot->pImageBuf = NULL;
			a_LogOutput(1, "CECImageRes::LoadImage, Failed to initialize A2DSpriteBuffer");
		}
	}

	return true;
}

/*	Release speicfied image

	dwSlot: image's slot.
*/
void CECImageRes::ReleaseImage(DWORD dwSlot)
{
	assert(dwSlot < NUM_IMAGE);

	IMAGE* pImage = &m_aImages[dwSlot];

	if (pImage->pImage)
	{
		pImage->pImage->Release();
		delete pImage->pImage;
		pImage->pImage = NULL;
	}

	if (pImage->pImageBuf)
	{
		pImage->pImageBuf->Release();
		delete pImage->pImageBuf;
		pImage->pImageBuf = NULL;
	}

	pImage->iNumItem = 0;
	pImage->iCurItem = -1;
}

//	Release all images
void CECImageRes::Release()
{
	int i;
	for (i=0; i < NUM_IMAGE; i++)
	{
		IMAGE* pImage = &m_aImages[i];
		
		if (pImage->pImage)
		{
			pImage->pImage->Release();
			delete pImage->pImage;
		}

		if (pImage->pImageBuf)
		{
			pImage->pImageBuf->Release();
			delete pImage->pImageBuf;
		}
	}

	memset(m_aImages, 0, sizeof (m_aImages));
	
	int iNumDraw = m_aRegDraws.GetSize();
	for (i=0; i < iNumDraw; i++)
		delete m_aRegDraws[i];

	m_aRegDraws.RemoveAll();
}

/*	Draw image item to backbuffer considing scale factor

	dwSlot: image slot.
	x, y: image's local center position on screen. x, y will also are be effected
		  by scale factors
	col: image color
	iItem: iamge item index
	z: z value
	iDstWid, iDstHei: image destination size on screen
*/
void CECImageRes::DrawImage(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem/* 0 */, 
						float z/* 0.0f */, int iDstWid/* 1.0f */, int iDstHei/* 1.0f */)
{
	assert(dwSlot < NUM_IMAGE);

	IMAGE* pSlot = &m_aImages[dwSlot];
	assert(iItem >= 0 && iItem < pSlot->iNumItem);

	A2DSprite* pImage = pSlot->pImage;

	if (iItem != pSlot->iCurItem)
	{
		pImage->SetCurrentItem(iItem);
		pSlot->iCurItem = iItem;
	}

	A3DRECT Rect = pImage->GetItem(iItem)->GetRect();
	float fScaleX = iDstWid ? (float)iDstWid / Rect.Width() : 1.0f;
	float fScaleY = iDstHei ? (float)iDstHei / Rect.Height() : 1.0f;

	pImage->SetScaleX(fScaleX);
	pImage->SetScaleY(fScaleY);
	pImage->SetZPos(z);
	pImage->SetColor(col);

	if(dwSlot == CECImageRes::IMG_PATEQUEST)  // 解决NPC头顶状态标记被某些透明片遮挡问题
	{
		pImage->DrawToInternalBuffer(x, y);
	}
	else
	{
		if (pSlot->pImageBuf)
			pImage->DrawToBuffer(pSlot->pImageBuf, x, y);
		else
			pImage->DrawToBack(x, y);
	}
}

/*	Draw image item to backbuffer without scale.

	dwSlot: image slot.
	x, y: image's local center position on screen. x, y will also are be effected
		  by scale factors
	col: image color
	iItem: iamge item index
	z: z value
*/
void CECImageRes::DrawImageNoScale(DWORD dwSlot, int x, int y, A3DCOLOR col, 
								int iItem/* 0 */, float z/* 0.0f */)
{
	assert(dwSlot < NUM_IMAGE);

	IMAGE* pSlot = &m_aImages[dwSlot];
	assert(iItem >= 0 && iItem < pSlot->iNumItem);

	A2DSprite* pImage = pSlot->pImage;

	if (iItem != pSlot->iCurItem)
	{
		pImage->SetCurrentItem(iItem);
		pSlot->iCurItem = iItem;
	}

	pImage->SetScaleX(1.0f);
	pImage->SetScaleY(1.0f);
	pImage->SetZPos(z);
	pImage->SetColor(col);

	if (pSlot->pImageBuf)
		pImage->DrawToBuffer(pSlot->pImageBuf, x, y);
	else
		pImage->DrawToBack(x, y);
}

/*	Get image item size

	dwSlot: image slot.
	iItem: item index.
	piWid (out): used to receive image item's width
	piHei (out): used to receive image item's height.
*/
void CECImageRes::GetImageItemSize(DWORD dwSlot, int iItem, int* piWid, int* piHei)
{
	if (dwSlot >= NUM_IMAGE)
	{
		assert(0);
		return;
	}

	IMAGE* pImage = &m_aImages[dwSlot];

	assert(iItem >= 0 && iItem < pImage->iNumItem);

	A2DSpriteItem* pItem = pImage->pImage->GetItem(iItem);
	A3DRECT Rect = pItem->GetRect();

	if (piWid)
		*piWid = Rect.right - Rect.left;

	if (piHei)
		*piHei = Rect.bottom - Rect.top;
}

//	Flush all rendered images
void CECImageRes::Flush()
{
	//	Flush image buffer
	for (int i=0; i < NUM_IMAGE; i++)
	{
		IMAGE* pSlot = &m_aImages[i];
		if (pSlot->pImageBuf)
			pSlot->pImageBuf->FlushBuffer();
	}
}

//	Register draw
void CECImageRes::RegisterDraw(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem/* 0 */, 
						float z/* 0.0f */, int iDstWid/* 0 */, int iDstHei/* 0 */)
{
	REGDRAW* pd = new REGDRAW;
	if (!pd)
		return;

	pd->dwSlot	= dwSlot;
	pd->x		= x;
	pd->y		= y;
	pd->z		= z;
	pd->dwCol	= col;
	pd->iItem	= iItem;
	pd->iDstWid	= iDstWid;
	pd->iDstHei	= iDstHei;

	m_aRegDraws.Add(pd);
}

//	Present all registered draws
void CECImageRes::PresentDraws()
{
	int iNumDraw = m_aRegDraws.GetSize();
	if (!iNumDraw)
		return;

	for (int i=0; i < iNumDraw; i++)
	{
		REGDRAW* pd = m_aRegDraws[i];
		DrawImage(pd->dwSlot, pd->x, pd->y, pd->dwCol, pd->iItem, pd->z, pd->iDstWid, pd->iDstHei);
		delete pd;
	}
	
	m_aRegDraws.RemoveAll(false);

	Flush();
}