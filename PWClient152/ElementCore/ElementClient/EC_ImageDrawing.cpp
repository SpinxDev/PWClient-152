
#include "A3DDevice.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DEngine.h"
#include "A2DSpriteBuffer.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_ImageDrawing.h"


CECImageDrawing::CECImageDrawing()
{
	m_pImage = NULL;
	m_pImageBuf = NULL;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iItemNum = 0;
	m_iCurItem = -1;
}

CECImageDrawing::~CECImageDrawing()
{
	ReleaseImage();
}

bool CECImageDrawing::LoadImageToDraw( const char* szFile, int iWidth, int iHeight, int iNumElem/*=0 */ )
{
	A3DRECT Rect(0, 0, iWidth, iHeight);
	if (!LoadImageToDraw(szFile, iWidth, iHeight, 1, &Rect, iNumElem))
		return false;
	
	if (!iWidth || !iHeight){
		if (m_pImage){
			A3DRECT rect(0, 0, iWidth, iHeight);
			m_pImage->ResetItems(1, &rect);
		}
	}
	
	return true;
}

bool CECImageDrawing::LoadImageToDraw( const char* szFile, int iWidth, int iHeight, int iNumItem, A3DRECT* aRects, int iNumElem/*=0*/ )
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	
	ReleaseImage(); 
	
	A2DSprite* pImage = new A2DSprite;
	if (!pImage){
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to create A2DSprite object");
		return false;
	}
	
	if (!pImage->Init(pA3DDevice, szFile, iWidth, iHeight, 0, iNumItem, aRects)){
		delete pImage;
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to initialize A2DSprite object from file %s", szFile);
		return false;
	}
	
	m_pImage		= pImage;
	m_pImageBuf		= NULL;
	m_iItemNum		= iNumItem;
	m_iCurItem		= -1;
	
	if (iNumElem){
		if (!(m_pImageBuf = new A2DSpriteBuffer)){
			a_LogOutput(1, "CECImageRes::LoadImage, Failed to new A2DSpriteBuffer");
		}
		else if (!m_pImageBuf->Init(pA3DDevice, m_pImage->GetTextures(), iNumElem)){
			delete m_pImageBuf;
			m_pImageBuf = NULL;
			a_LogOutput(1, "CECImageRes::LoadImage, Failed to initialize A2DSpriteBuffer");
		}
	}
	
	return true;
}

bool CECImageDrawing::HasImage()const{
	return m_pImage != NULL;
}

void CECImageDrawing::DrawImage( int x, int y, A3DCOLOR col, int iItem/*=0*/, float z/*=0.0f*/, int iDstWid/*=0*/, int iDstHei/*=0*/ )
{	
	assert(iItem >= 0 && iItem < m_iItemNum);
	
	A2DSprite* pImage = m_pImage;
	
	if (iItem != m_iCurItem){
		pImage->SetCurrentItem(iItem);
		m_iCurItem = iItem;
	}
	
	A3DRECT Rect = pImage->GetItem(iItem)->GetRect();
	float fScaleX = iDstWid ? (float)iDstWid / Rect.Width() : 1.0f;
	float fScaleY = iDstHei ? (float)iDstHei / Rect.Height() : 1.0f;
	
	pImage->SetScaleX(fScaleX);
	pImage->SetScaleY(fScaleY);
	pImage->SetZPos(z);
	pImage->SetColor(col);
	
	if (m_pImageBuf)
		pImage->DrawToBuffer(m_pImageBuf, x, y);
	else
		pImage->DrawToBack(x, y);
	
}

void CECImageDrawing::Flush()
{
	if (m_pImageBuf)
		m_pImageBuf->FlushBuffer();
}

void CECImageDrawing::ReleaseImage()
{
	if (m_pImage){
		m_pImage->Release();
		delete m_pImage;
		m_pImage = NULL;
	}
	if (m_pImageBuf){
		m_pImageBuf->Release();
		delete m_pImageBuf;
		m_pImageBuf = NULL;
	}
}
CECBarDrawing::CECBarDrawing():
CECImageDrawing()
{
	
}

bool CECBarDrawing::InitialImage(const char* szFile)
{
	AString imgPath = szFile;
	if (imgPath.IsEmpty()){
		ReleaseImage();
		return false;
	}
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	ReleaseImage(); 	
	A2DSprite* pImage = new A2DSprite;
	if (!pImage){
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to create A2DSprite object");
		return false;
	}
	if (!pImage->Init(pA3DDevice, szFile, 0xffff00ff)){
		delete pImage;
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to initialize A2DSprite object from file %s", szFile);
		return false;
	}
	m_pImage = pImage;
	int i,iWidth, iHeight; 
	m_iWidth = m_pImage->GetWidth(), m_iHeight = m_pImage->GetHeight();
	A3DRECT aRects[9];
	iWidth = m_iWidth/3;
	iHeight = m_iHeight/3;
	for (i=0; i < 3; i++){
		for (int j=0; j < 3; j++)
			aRects[i*3+j] = A3DRECT(j*iWidth, i*iHeight, j*iWidth+iWidth, i*iHeight+iHeight);
	}	
	return ResetImage(m_iWidth, m_iHeight, 9, aRects, 128);	
}

void CECBarDrawing::DrawingBoard(int x, int y, int sizex, int sizey, float z)
{
	int iWidth = m_iWidth / 3;	
	int iHeight = m_iHeight / 3;
				
	int cx = sizex - 2*iWidth;  
	int	cy = sizey - 2*iHeight;	
	
	DWORD dwCol = 0xffffffff;

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);	//¿ªÆôalphaÈÚºÏ
	
	DrawImage( x, y, dwCol, 0, z, iWidth, iHeight);
	DrawImage( x+iWidth, y, dwCol, 1, z, cx, iHeight);
	DrawImage( x+iWidth+cx, y, dwCol, 2, z, iWidth, iHeight);
	
	DrawImage( x, y+iHeight, dwCol, 3, z, iWidth, cy);
	DrawImage( x+iWidth, y+iHeight, dwCol, 4, z, cx, cy);
	DrawImage( x+iWidth+cx, y+iHeight, dwCol, 5, z, iWidth, cy);
	
	DrawImage( x, y+iHeight+cy, dwCol, 6, z, iWidth, iHeight);
	DrawImage( x+iWidth, y+iHeight+cy, dwCol, 7, z, cx, iHeight);
	DrawImage( x+iWidth+cx, y+iHeight+cy, dwCol, 8, z, iWidth, iHeight);

	Flush();

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
}

bool CECBarDrawing::ResetImage( int iWidth, int iHeight, int iNumItem, A3DRECT* aRects, int iNumElem/*=0*/ )
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();


	if (!m_pImage){
		a_LogOutput(1, "CECImageRes::LoadImage, Failed to create A2DSprite object");
		return false;
	}

	m_pImageBuf		= NULL;
	m_iItemNum		= iNumItem;
	m_iCurItem		= -1;

	m_pImage->ResetItems(iNumItem, aRects);
	
	return true;
}