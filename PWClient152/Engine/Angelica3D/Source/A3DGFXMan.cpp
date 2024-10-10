/*
 * FILE: A3DGFXMan.cpp
 *
 * DESCRIPTION: Graphics FX Managing Class
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DTypes.h"
#include "A3DGFXMan.h"
#include "A3DConfig.h"
#include "A3DGraphicsFX.h"
#include "A3DGFXCollector.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DModel.h"
#include "A3DViewport.h"
#include "AFileImage.h"
#include "AFI.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DGFXMan::A3DGFXMan()
{
	m_szFolderName[0]	= '\0';
	m_pA3DDevice		= NULL;
	m_nGFXCache			= 10;
	m_pGFXCollector		= NULL;
}

A3DGFXMan::~A3DGFXMan()
{
}

bool A3DGFXMan::Init(A3DDevice * pA3DDevice)
{
	strcpy_s(m_szFolderName, "Gfx");
	strcpy_s(m_szTextureFolder, "Gfx\\Textures");

	m_pA3DDevice = pA3DDevice;
	m_GFXList.Init();
	m_PreloadedGFXFilesList.Init();

	m_pGFXCollector = new A3DGFXCollector(100);
	if( NULL == m_pGFXCollector )
	{
		g_A3DErrLog.Log("A3DGFXMan::Init() Not enough memory!");
		return false;
	}

	// Now preload the gfx files written in gfxlist.txt
	AFileImage gfxListFile;
	if( !gfxListFile.Open(m_szFolderName, "gfxlist.txt", AFILE_OPENEXIST) )
	{
		gfxListFile.Close();
		g_A3DErrLog.Log("A3DGFXMan::Init() Can not open %s", "gfxlist.txt");
		return true;
	}

	char	szLineBuffer[AFILE_LINEMAXLEN];
	DWORD	dwRead;

	while( gfxListFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwRead) )
	{
		if( strlen(szLineBuffer) == 0 )
			continue;

		AFileImage * pFileImage = new AFileImage();
		if( NULL == pFileImage )
		{
			g_A3DErrLog.Log("A3DGFXMan::Init() Not enough memory!");
			return false;
		}

		if( !pFileImage->Open(m_szFolderName, szLineBuffer, AFILE_OPENEXIST) )
		{
			pFileImage->Close();
			delete pFileImage;
			g_A3DErrLog.Log("A3DGFXMan::Init() Can not open file [%s] to be a fileimage", szLineBuffer);
			return false;
		}

		m_PreloadedGFXFilesList.Append((LPVOID) pFileImage);
	}
	gfxListFile.Close();
	return true;
}

bool A3DGFXMan::Release()
{
	ALISTELEMENT * pThisElement = m_GFXList.GetFirst();
	while( pThisElement != m_GFXList.GetTail() )
	{
		A3DGraphicsFX * pGFX = (A3DGraphicsFX *) pThisElement->pData;
		pGFX->Release();
		delete pGFX;
		pGFX = NULL;

		ALISTELEMENT * pElementToDelete = pThisElement;
		pThisElement = pThisElement->pNext;
		m_GFXList.Delete(pElementToDelete);
	}
	
	if( m_pGFXCollector )
	{
		A3DGFXCollector * pCollector = m_pGFXCollector;
		m_pGFXCollector = NULL; // This is used to avoid push back gfx when releasing gfx man

		pCollector->Release();
		delete pCollector;
	}

	pThisElement = m_PreloadedGFXFilesList.GetFirst();
	while( pThisElement != m_PreloadedGFXFilesList.GetTail() )
	{
		AFileImage * pFileImage = (AFileImage *) pThisElement->pData;
		pFileImage->Close();
		delete pFileImage;
		pThisElement = pThisElement->pNext;
	}

	m_PreloadedGFXFilesList.Release();
	m_GFXList.Release();
	return true;
}

// Create an A3DGraphicsFX class object from a file; but this graphicsFX is invisible at beginning, 
// call A3DGraphicsFX::Start if you want the effect showing;
/*
	This function load a graphics fx from a file
	IN: 
		szGFXFile			filename of the graphics fx;
		pModel				the model pointer to which this effect will be applied;
		pParentFrame		the parent frame's pointer of this effect;
		bLinked				whether this effect will move together with its parent frame;
	OUT:
		ppGFX				the returned graphics fx's pointer will be placed in this pointer;
*/

bool A3DGFXMan::LoadGFXFromFile(const char* szGFXFile, A3DModel* pModel, const char * szParentFrameName, bool bLinked, A3DGraphicsFX ** ppGFX,
								A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, bool bForceFile)
{
	*ppGFX = NULL;
	A3DGraphicsFX * pGFX = NULL;

	// First we try to find an unused gfx object from the collector;
	pGFX = m_pGFXCollector->PopUp(szGFXFile);
	if( !pGFX )
	{
		// If not found, we must create a new gfx object, but first we create the file image;
		char szFullPath[MAX_PATH];
		AFileImage * pFileImage = NULL;

		if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		{
			// We create one empty graphics object;
			pGFX = new A3DGraphicsFX();
			if( NULL == pGFX )
			{
				g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile Not enough memory!");
				return false;
			}

			// Load the A3DGraphics object from a NULL file image;
			pGFX->Load(m_pA3DDevice, pFileImage);
			pGFX->SetName(szGFXFile);
		}
		else
		{
			af_GetFullPath(szFullPath, m_szFolderName, szGFXFile);
			
			if( !bForceFile )
			{
				pFileImage = FindFileImage(szFullPath);
				if( NULL == pFileImage )
				{
					g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile() Can not find preloaded file image [%s], now try to load from file...", szFullPath);
					
					// Now we should load the file image and add it to the list;
					pFileImage = new AFileImage();
					if( NULL == pFileImage )
					{
						g_A3DErrLog.Log("A3DGFXMan::Init() Not enough memory!");
						return false;
					}

					if( !pFileImage->Open(szFullPath, AFILE_OPENEXIST) )
					{
						g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile() Try to load a fileimage fail [%s]", szFullPath);
						pFileImage->Close();
						delete pFileImage;
						return false;
					}

					// Add it into the file image list;
					m_PreloadedGFXFilesList.Append((LPVOID) pFileImage);
					g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile(), Load a file image [%s] from file done!", szFullPath);
				}

				// Now reset the pointer for later usage;
				pFileImage->ResetPointer();
			}
			else
			{
				// Create a tempory used file image;
				pFileImage = new AFileImage();
				if( NULL == pFileImage )
				{
					g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile(), Not enough memory!");
					return false;
				}
				if( !pFileImage->Open(szFullPath, AFILE_OPENEXIST) )
				{
					g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile(), Can not open file %s to be a fileimage!", szGFXFile);
					pFileImage->Close();
					delete pFileImage;
					return false;
				}
			}
			// Create a new graphis object;
			pGFX = new A3DGraphicsFX();
			if( NULL == pGFX )
			{
				g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile Not enough memory!");
				return false;
			}

			// Load the A3DGraphics object from the file image;
			if( !pGFX->Load(m_pA3DDevice, pFileImage) )
			{
				g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile Call GraphicsFX::Load Fail!");
				if( bForceFile )
				{
					pFileImage->Close();
					delete pFileImage;
					pFileImage = NULL;
				}
				pGFX->Release();
				delete pGFX;
				return false;
			}

			pGFX->SetName(szGFXFile);
			
			// If the pFile is an tempory used file image, we need to close it;
			if( bForceFile )
			{
				pFileImage->Close();
				delete pFileImage;
				pFileImage = NULL;
			}
		}
	}

	A3DFrame * pParentFrame = NULL;

	if( pModel && szParentFrameName )
	{
		pParentFrame = pModel->FindChildFrameByName(szParentFrameName);
		if( NULL == pParentFrame )
		{
			pGFX->Release();
			delete pGFX;
			g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile() Can not find parent frame [%s]", szParentFrameName);
			return false;
		}
	}

	if( !pGFX->CreateAllFX(pModel, pParentFrame, bLinked, vecPos, vecDir, vecUp) )
	{
		pGFX->Release();
		delete pGFX;
		g_A3DErrLog.Log("A3DGFXMan::LoadGFXFromFile Call GraphicsFX::CreateAllFX Fail!");
		return false;
	}

	*ppGFX = pGFX;
	return true;
}

// Remove the reference from m_GFXList, and Release an A3DGraphicsFX class object;
bool A3DGFXMan::ReleaseGFX(A3DGraphicsFX*& pGFX)
{
	// First remove the reference;
	DeleteGFX(pGFX);

	if( pGFX->GetPArrayList()->GetSize() > 0 )
	{
		// Then release the A3DGraphicsFX object;
		// We will never really release these resource, we collect it and reuse them when need;
		pGFX->Release();
		delete pGFX;
		pGFX = NULL;
	}
	else
	{
		if( m_pGFXCollector )
			m_pGFXCollector->PushBack(pGFX->GetName(), pGFX);
		else
		{
			pGFX->Release();
			delete pGFX;
		}
		pGFX = NULL;
	}

	return true;
}

// Remove the reference from m_GFXList, but not release it;
bool A3DGFXMan::DeleteGFX(A3DGraphicsFX*& pGFX)
{
	if( NULL == pGFX->GetStoredElement() )
		return true;

	m_GFXList.Delete(pGFX->GetStoredElement());
	pGFX->SetStoredElement(NULL);
	return true;
}

bool A3DGFXMan::IsGFXAlive(A3DGraphicsFX * pGFX)
{
	if( m_GFXList.FindElementByData(pGFX) )
		return true;

	return false;
}

bool A3DGFXMan::Render(A3DViewport * pCurrentViewport, int nCategoryMask)
{
	ALISTELEMENT * pThisElement = m_GFXList.GetFirst();
	while (pThisElement != m_GFXList.GetTail())
	{
		A3DGraphicsFX * pGFX = (A3DGraphicsFX *) pThisElement->pData;

		if( !pGFX->IsExpired() && (nCategoryMask & pGFX->GetCategory()) && pGFX->IsDrawByMan() )
		{
			if( !pGFX->Render(pCurrentViewport) )
				return false;
		}

		pThisElement = pThisElement->pNext;
	}

	return true;
}

bool A3DGFXMan::TickAnimation()
{
	ALISTELEMENT * pThisElement = m_GFXList.GetFirst();
	while( pThisElement != m_GFXList.GetTail() )
	{
		A3DGraphicsFX * pGFX = (A3DGraphicsFX *) pThisElement->pData;
		
		if( !pGFX->IsExpired() && !pGFX->TickAnimation() )
			return false;

		pThisElement = pThisElement->pNext;

		if( pGFX->IsExpired() )
		{
			if( pGFX->IsDead() )
				ReleaseGFX(pGFX);
			else
				DeleteGFX(pGFX);
		}
	}
	return true;
}

bool A3DGFXMan::Reset()
{
	// Release all current activated A3DGraphicsFX class objects;
	ALISTELEMENT * pThisElement = m_GFXList.GetFirst();
	while( pThisElement != m_GFXList.GetTail() )
	{
		A3DGraphicsFX * pGFX = (A3DGraphicsFX *) pThisElement->pData;
		pGFX->Release();
		delete pGFX;
		pGFX = NULL;

		pThisElement = pThisElement->pNext;
	}
	m_GFXList.Reset();

	if( m_pGFXCollector )
	{
		A3DGFXCollector * pCollector = m_pGFXCollector;

		m_pGFXCollector = NULL; // This is used to avoid push back gfx when releasing gfx man
		pCollector->Release();

		m_pGFXCollector = pCollector;
	}

	// Should release all gfx file images and repreload again!!!!!!!!!!!
	return true;
}

// Called directly from A3DGraphicsFX::Start, add a reference into m_GFXList
bool A3DGFXMan::AddGFX(A3DGraphicsFX * pGFX)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	ALISTELEMENT * pStoredElement = NULL;

	if( pGFX->GetStoredElement() )
		return true;

	m_GFXList.Append((LPVOID) pGFX, &pStoredElement);
	pGFX->SetStoredElement(pStoredElement);
	return true;
}

AFileImage * A3DGFXMan::FindFileImage(const char * szFileName)
{
	ALISTELEMENT * pThisElement = m_PreloadedGFXFilesList.GetFirst();
	while( pThisElement != m_PreloadedGFXFilesList.GetTail() )
	{
		AFileImage * pFileImage = (AFileImage *) pThisElement->pData;

		if( 0 == _stricmp(pFileImage->GetFileName(), szFileName) )
		{
			return pFileImage;
		}
		pThisElement = pThisElement->pNext;
	}

	return NULL;
}