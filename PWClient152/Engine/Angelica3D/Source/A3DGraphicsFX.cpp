/*
 * FILE: A3DGraphicsFX.cpp
 *
 * DESCRIPTION: Graphics FX Class is representing the graphics effects used in game;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DGraphicsFX.h"
#include "A3DGFXMan.h"
#include "A3DConfig.h"
#include "A3DParticleSystem.h"
#include "A3DSuperSpray.h"
#include "A3DSuperDecal.h"
#include "A3DBillBoard.h"
#include "A3DPArray.h"
#include "A3DEngine.h"
#include "A3DFrame.h"
#include "A3DModel.h"
#include "A3DDevice.h"
#include "AFile.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DGraphicsFX::A3DGraphicsFX()
{
	m_dwClassID		= A3D_CID_GRAPHICSFX;
	m_pA3DDevice	= NULL;
	m_bHWIGFX		= false;
	m_bExpired		= true;
	m_bDieOnExpired = true;
	m_pParentFrame	= NULL;
	m_pHostModel	= NULL;

	m_bLinked		= false;
	m_vScaleX		= 1.0f;
	m_vScaleY		= 1.0f;
	m_vScaleZ		= 1.0f;
	m_vScale		= 1.0f;
	m_vMaxDecalSize	= 0.0f;
	m_Category		= A3DGFX_CATEGORY_NORMALGFX;
	m_bHasCreated	= false;
	m_bDrawByMan	= true;

	m_pStoredElement = NULL;
}

A3DGraphicsFX::~A3DGraphicsFX()
{
}

bool A3DGraphicsFX::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;	

	m_nEverTicks = 0;
	m_nDelayTicks = 0;

	m_SuperDecalList.Init();
	m_BillBoardList.Init();
	m_SuperSprayList.Init();
	m_PArrayList.Init();
	m_PlayEventList.Init();

	SetName("UNTITLED");

	m_vecPos = A3DVECTOR3(0.0f);
	m_vecDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUp  = A3DVECTOR3(0.0f, 1.0f, 0.0f);

	m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);
	m_matParentTM.Identity();
	m_matAbsoluteTM = m_matRelativeTM * m_matParentTM;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWIGFX = true;
		return true;
	}
	return true;
}

bool A3DGraphicsFX::Release()
{
	//g_A3DErrLog.Log("%s, A3DGraphicsFX::Release()", GetName());
	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_GFX);

	// Just release the resources, don't try to remove it from GFXMan, because 
	// GFXMan will manage all A3DGraphicsFX;
	ALISTELEMENT * pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		pSuperDecal->Release();
		delete pSuperDecal;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		pBillBoard->Release();
		delete pBillBoard;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		pSuperSpray->Release();
		delete pSuperSpray;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray * pPArray = (A3DPArray *) pThisElement->pData;
		pPArray->Release();
		delete pPArray;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PlayEventList.GetFirst();
	while( pThisElement != m_PlayEventList.GetTail() )
	{
		A3DGFXPlayEvent * pEvent = (A3DGFXPlayEvent *) pThisElement->pData;
		pEvent->Release();
		delete pEvent;
		pThisElement = pThisElement->pNext;
	}

	m_SuperDecalList.Release();
	m_BillBoardList.Release();
	m_SuperSprayList.Release();
	m_PArrayList.Release();
	m_PlayEventList.Release();
	return true;		   
}

bool A3DGraphicsFX::Render(A3DViewport * pCurrentViewport)
{
	if( m_bExpired || m_bHWIGFX )
		return true;

	ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		if( !pSuperSpray->RenderParticles(pCurrentViewport) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
		if( !pPArray->RenderParticles(pCurrentViewport) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		if( !pSuperDecal->Render(pCurrentViewport) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		if( !pBillBoard->Render(pCurrentViewport) )
			return false;
		pThisElement = pThisElement->pNext;
	}
	return true;
}

bool A3DGraphicsFX::TickAnimation()
{
	if( m_bExpired || m_bHWIGFX )
		return true;

	bool	bAllExpired = true;
	
	ALISTELEMENT * pThisElement;
	
	// First see if there is some scheduled play events;
	pThisElement = m_PlayEventList.GetFirst();
	while( pThisElement != m_PlayEventList.GetTail() )
	{
		A3DGFXPlayEvent * pEvent = (A3DGFXPlayEvent *) pThisElement->pData;
		if( pEvent->IsActive() )
		{
			bAllExpired = false;
			if( m_nEverTicks == pEvent->GetThisStart() )
			{
				FirePlayEvent(pEvent);
				pEvent->SetActive(false);
			}
		}

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		if( !pSuperSpray->TickEmitting() )
			return false;
		if( !pSuperSpray->IsExpired() )
			bAllExpired = false;

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
		if( !pPArray->TickEmitting() )
			return false;
		if( !pPArray->IsExpired() )
			bAllExpired = false;

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		if( !pSuperDecal->TickAnimation() )
			return false;
		if( !pSuperDecal->IsFinish() )
			bAllExpired = false;

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		if( !pBillBoard->TickAnimation() )
			return false;
		if( !pBillBoard->IsExpired() )
			bAllExpired = false;

		pThisElement = pThisElement->pNext;
	}

	if( bAllExpired )
		m_bExpired = true;

	m_nEverTicks ++;
	return true;
}

bool A3DGraphicsFX::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if( !Init(pA3DDevice) )
		return false;

	if( m_bHWIGFX )	return true;

	if( pFileToLoad->IsBinary() )
	{
		//Load Binary Data here;
	}
	else
	{
		//Load Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];
		char			szResult[AFILE_LINEMAXLEN];
		DWORD			dwReadLen;
		int				nSuperSprayCount;
		int				nPArrayCount;
		int				nSuperDecalCount;
		int				nBillBoardCount;
		int				nPlayEventCount;
		int				i;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "GRAPHICSFX: ", szResult);
		SetName(szResult);

		// Load super sprays here;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);		
		sscanf(szLineBuffer, "SUPERSPRAYCOUNT: %d", &nSuperSprayCount);

		for(i=0; i<nSuperSprayCount; i++)
		{
			A3DSuperSpray * pNewSuperSpray;

			pNewSuperSpray = new A3DSuperSpray();
			if( NULL == pNewSuperSpray )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Not enough memory!");
				return false;
			}

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			a_GetStringAfter(szLineBuffer, "SUPERSPRAY: ", szResult);
			pNewSuperSpray->SetName(szResult);

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}
			if( !pNewSuperSpray->Load(m_pA3DDevice, pFileToLoad) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Load Super Spray Fail");
				return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}
				
			// comment below to let super spray disappear;
			if( 1 )
				m_SuperSprayList.Append((LPVOID) pNewSuperSpray, NULL);
			else
			{
				pNewSuperSpray->Release();
				delete pNewSuperSpray;
			}
		}

		// Load PArrays here;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);		
		sscanf(szLineBuffer, "PARRAYCOUNT: %d", &nPArrayCount);

		for(i=0; i<nPArrayCount; i++)
		{
			A3DPArray * pNewPArray;

			pNewPArray = new A3DPArray();
			if( NULL == pNewPArray )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Not enough memory!");
				return false;
			}

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			a_GetStringAfter(szLineBuffer, "PARRAY: ", szResult);
			pNewPArray->SetName(szResult);

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}
			if( !pNewPArray->Load(m_pA3DDevice, pFileToLoad) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Load PArray Fail");
				return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}

			if( 1 )
				m_PArrayList.Append((LPVOID) pNewPArray, NULL);
			else
			{
				pNewPArray->Release();
				delete pNewPArray;
			}
		}

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);		
		sscanf(szLineBuffer, "SUPERDECALCOUNT: %d", &nSuperDecalCount);

		for(i=0; i<nSuperDecalCount; i++)
		{
			A3DSuperDecal * pNewSuperDecal;

			pNewSuperDecal = new A3DSuperDecal();
			if( NULL == pNewSuperDecal )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Not enough memory!");
				return false;
			}

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			a_GetStringAfter(szLineBuffer, "SUPERDECAL: ", szResult);
			pNewSuperDecal->SetName(szResult);

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}
			if( !pNewSuperDecal->Load(m_pA3DDevice, pFileToLoad) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Load SuperDecal Fail");
				return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}

			m_SuperDecalList.Append((LPVOID) pNewSuperDecal, NULL);
		}

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);		
		sscanf(szLineBuffer, "BILLBOARDCOUNT: %d", &nBillBoardCount);

		for(i=0; i<nBillBoardCount; i++)
		{
			A3DBillBoard * pNewBillBoard;

			pNewBillBoard = new A3DBillBoard();
			if( NULL == pNewBillBoard )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Not enough memory!");
				return false;
			}

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			a_GetStringAfter(szLineBuffer, "BILLBOARD: ", szResult);
			pNewBillBoard->SetName(szResult);

			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}
			if( !pNewBillBoard->Load(m_pA3DDevice, pFileToLoad) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Load BillBoard Fail");
				return false;
			}
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") != 0 )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
				return false;
			}

			m_BillBoardList.Append((LPVOID) pNewBillBoard, NULL);

			if( pNewBillBoard->GetBillBoardType() == A3DBILLBOARD_DYNAMICLIGHT )
				m_Category = A3DGFX_CATEGORY_DLIGHTGFX;
			else if( pNewBillBoard->GetBillBoardType() == A3DBILLBOARD_SHADOW )
				m_Category = A3DGFX_CATEGORY_SHADOWGFX;
			else if( pNewBillBoard->GetBillBoardType() == A3DBILLBOARD_MARK )
				m_Category = A3DGFX_CATEGORY_MARKGFX;
		}

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strstr(szLineBuffer, "PLAYEVENTCOUNT: ") )
		{
			sscanf(szLineBuffer, "PLAYEVENTCOUNT: %d", &nPlayEventCount);

			for(i=0; i<nPlayEventCount; i++)
			{
				A3DGFXPlayEvent * pNewEvent;

				pNewEvent = new A3DGFXPlayEvent();
				if( NULL == pNewEvent )
				{
					g_A3DErrLog.Log("A3DGraphicsFX::Load() Not enough memory!");
					return false;
				}

				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				a_GetStringAfter(szLineBuffer, "PLAYEVENT: ", szResult);
				pNewEvent->SetName(szResult);

				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				if( strcmp(szLineBuffer, "{") != 0 )
				{
					g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
					return false;
				}
				if( !pNewEvent->Load(this, pFileToLoad) )
				{
					pNewEvent->Release();
					delete pNewEvent;
					g_A3DErrLog.Log("A3DGraphicsFX::Load(), Load PlayEvent [%s] Fail", szResult);
					// make it continue run, just drop this play event
				}
				else
					m_PlayEventList.Append((LPVOID) pNewEvent, NULL);

				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				if( strcmp(szLineBuffer, "}") != 0 )
				{
					g_A3DErrLog.Log("A3DGraphicsFX::Load() Graphics FX File parsed error!");
					return false;
				}
			}
		}
	}

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_GFX);
	//g_A3DErrLog.Log("%s, A3DGraphicsFX::Load()", pFileToLoad->GetRelativeName());
	return true;
}

bool A3DGraphicsFX::Save(AFile * pFileToSave, bool bPackedUp)
{
	if( m_bHWIGFX )	return true;

	if( pFileToSave->IsBinary() )
	{
		//Save Binary Data here;
	}
	else
	{
		//Save Text Data here;
		char szLineBuffer[2048];

		sprintf(szLineBuffer, "GRAPHICSFX: %s", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "SUPERSPRAYCOUNT: %d", m_SuperSprayList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
		while( pThisElement != m_SuperSprayList.GetTail() )
		{
			A3DSuperSpray * pSuperSpray = (A3DSuperSpray * ) pThisElement->pData;
			sprintf(szLineBuffer, "SUPERSPRAY: %s", pSuperSpray->GetName());
			pFileToSave->WriteLine(szLineBuffer); 

			pFileToSave->WriteLine("{");
			if( !pSuperSpray->Save(pFileToSave) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Save Save Super Spray Fail");
				return false;
			}
			pFileToSave->WriteLine("}");
			pThisElement = pThisElement->pNext;
		}

		sprintf(szLineBuffer, "PARRAYCOUNT: %d", m_PArrayList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		pThisElement = m_PArrayList.GetFirst();
		while( pThisElement != m_PArrayList.GetTail() )
		{
			A3DPArray * pPArray = (A3DPArray * ) pThisElement->pData;
			sprintf(szLineBuffer, "PARRAY: %s", pPArray->GetName());
			pFileToSave->WriteLine(szLineBuffer); 

			pFileToSave->WriteLine("{");
			if( !pPArray->Save(pFileToSave) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Save Save PArray Fail");
				return false;
			}
			pFileToSave->WriteLine("}");
			pThisElement = pThisElement->pNext;
		}

		sprintf(szLineBuffer, "SUPERDECALCOUNT: %d", m_SuperDecalList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		pThisElement = m_SuperDecalList.GetFirst();
		while( pThisElement != m_SuperDecalList.GetTail() )
		{
			A3DSuperDecal * pSuperDecal = (A3DSuperDecal * ) pThisElement->pData;
			sprintf(szLineBuffer, "SUPERDECAL: %s", pSuperDecal->GetName());
			pFileToSave->WriteLine(szLineBuffer); 

			pFileToSave->WriteLine("{");
			if( !pSuperDecal->Save(pFileToSave) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Save Save Super Decal Fail");
				return false;
			}
			pFileToSave->WriteLine("}");
			pThisElement = pThisElement->pNext;
		}

		sprintf(szLineBuffer, "BILLBOARDCOUNT: %d", m_BillBoardList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		pThisElement = m_BillBoardList.GetFirst();
		while( pThisElement != m_BillBoardList.GetTail() )
		{
			A3DBillBoard * pBillBoard = (A3DBillBoard * ) pThisElement->pData;
			sprintf(szLineBuffer, "BILLBOARD: %s", pBillBoard->GetName());
			pFileToSave->WriteLine(szLineBuffer); 

			pFileToSave->WriteLine("{");
			if( !pBillBoard->Save(pFileToSave) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Save Save BillBoard Fail");
				return false;
			}
			pFileToSave->WriteLine("}");
			pThisElement = pThisElement->pNext;
		}

		sprintf(szLineBuffer, "PLAYEVENTCOUNT: %d", m_PlayEventList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);

		pThisElement = m_PlayEventList.GetFirst();
		while( pThisElement != m_PlayEventList.GetTail() )
		{
			A3DGFXPlayEvent * pEvent = (A3DGFXPlayEvent * ) pThisElement->pData;
			sprintf(szLineBuffer, "PLAYEVENT: %s", pEvent->GetName());
			pFileToSave->WriteLine(szLineBuffer); 

			pFileToSave->WriteLine("{");
			if( !pEvent->Save(pFileToSave) )
			{
				g_A3DErrLog.Log("A3DGraphicsFX::Save PlayEvent Fail");
				return false;
			}
			pFileToSave->WriteLine("}");
			pThisElement = pThisElement->pNext;
		}
	}
	return true;
}

bool A3DGraphicsFX::Start(bool bDieOnExpired)
{
	if( m_bHWIGFX )
	{
		if( bDieOnExpired )
		{
			// It must be finished here, because the effect will never really reach the end when it is a dummy
			Release();
			delete this;
		}
		return true;
	}

	if( m_pStoredElement )
	{
		//We don't need to add it into GFXMan, for it has the reference already;
	}
	else
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->AddGFX(this);
	}

	if( m_PlayEventList.GetSize() == 0 )
	{
		ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
		while( pThisElement != m_SuperSprayList.GetTail() )
		{
			A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
			if( !pSuperSpray->UpdateParentTM(m_matAbsoluteTM) )
				return false;
			if( !pSuperSpray->Start() )
				return false;
			pThisElement = pThisElement->pNext;
		}

		pThisElement = m_PArrayList.GetFirst();
		while( pThisElement != m_PArrayList.GetTail() )
		{
			A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
			if( !pPArray->UpdateParentTM(m_matAbsoluteTM) )
				return false;
			if( !pPArray->Start() )
				return false;
			pThisElement = pThisElement->pNext;
		}

		pThisElement = m_SuperDecalList.GetFirst();
		while( pThisElement != m_SuperDecalList.GetTail() )
		{
			A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
			pSuperDecal->Play();
			pThisElement = pThisElement->pNext;
		}

		pThisElement = m_BillBoardList.GetFirst();
		while( pThisElement != m_BillBoardList.GetTail() )
		{
			A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
			if( !pBillBoard->Start() )
				return false;
			pThisElement = pThisElement->pNext;
		}
	}
	else
	{
		if( !ActiveAllPlayEvents(true) )
			return false;
	}

	m_bExpired = false;
	m_bDieOnExpired = bDieOnExpired;
	m_nEverTicks = 0;
	return true;
}

bool A3DGraphicsFX::Stop(bool bDieOnExpired)
{
	if( m_bHWIGFX )
	{
		if( bDieOnExpired )
		{
			// For we only create a dummy object here, and it is not in gfx man, 
			// So we must delete it here;
			Release();
			delete this;
		}
		return true;
	}

	bool bForceStop = true;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_GAME ||
		g_pA3DConfig->GetRunEnv() == A3DRUNENV_MODEDITOR )
		bForceStop = false;

	// First deactive all play events;
	if( !ActiveAllPlayEvents(false) )
		return false;

	ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		if( !pSuperSpray->Stop(bForceStop) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
		if( !pPArray->Stop(bForceStop) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		pSuperDecal->Stop();

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		if( !pBillBoard->Stop() )
			return false;
		pThisElement = pThisElement->pNext;
	}

	// Do not force expire this one when in game;
	m_bExpired = bForceStop;
	m_bDieOnExpired = bDieOnExpired;

	if( !m_pStoredElement )
	{
		// Have not started yet, so we have to add it into the gfx man list, let gfx man auto release it;
		m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->AddGFX(this);
		return true;
	}

	return true;
}

A3DSuperSpray * A3DGraphicsFX::GetSuperSpray(int index)
{
	ALISTELEMENT * pElement = m_SuperSprayList.GetElementByOrder(index);
	if( NULL == pElement )
		return NULL;
	return (A3DSuperSpray *) pElement->pData; 
}

A3DPArray * A3DGraphicsFX::GetPArray(int index)
{
	ALISTELEMENT * pElement = m_PArrayList.GetElementByOrder(index);
	if( NULL == pElement )
		return NULL;
	return (A3DPArray *) pElement->pData; 
}

A3DSuperDecal * A3DGraphicsFX::GetSuperDecal(int index)
{
	ALISTELEMENT * pElement = m_SuperDecalList.GetElementByOrder(index);
	if( NULL == pElement )
		return NULL;
	return (A3DSuperDecal *) pElement->pData; 
}

A3DBillBoard * A3DGraphicsFX::GetBillBoard(int index)
{
	ALISTELEMENT * pElement = m_BillBoardList.GetElementByOrder(index);
	if( NULL == pElement )
		return NULL;
	return (A3DBillBoard *) pElement->pData; 
}

A3DGFXPlayEvent * A3DGraphicsFX::GetPlayEvent(int index)
{
	ALISTELEMENT * pElement = m_PlayEventList.GetElementByOrder(index);
	if( NULL == pElement )
		return NULL;
	return (A3DGFXPlayEvent *) pElement->pData;
}

bool A3DGraphicsFX::RecalculateAllFX()
{
	if( m_bHWIGFX )		return true;

	// Just update the informations;
	ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		
		if( !pSuperSpray->UpdateParentTM(m_matAbsoluteTM) )
			return false;

		pThisElement = pThisElement->pNext;
	}

	// PArray can not recreate the object fragments;
	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
	
		if( !pPArray->UpdateParentTM(m_matAbsoluteTM) )
			return false;

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;

		pSuperDecal->UpdateParentTM(m_matAbsoluteTM);

		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		
		if( !pBillBoard->UpdateParentTM(m_matAbsoluteTM) )
			return false;

		if(	!pBillBoard->SplitMark() )
			return false;

		pThisElement = pThisElement->pNext;
	}
	return true;
}

bool A3DGraphicsFX::CreateAllFX(A3DModel * pHostModel, A3DFrame * pParentFrame, bool bLinked, 
								A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	if( m_bHWIGFX )		return true;

	m_pHostModel = pHostModel;
	m_pParentFrame = pParentFrame;
	m_bLinked = bLinked;
	
	m_vecPos = vecPos;
	m_vecDir = vecDir;
	m_vecUp  = vecUp;

	m_matAbsoluteTM = m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);
	
	if( m_pParentFrame )
	{
		m_matParentTM = m_pParentFrame->GetAbsoluteTM();
		m_matAbsoluteTM = m_matRelativeTM * m_matParentTM;
	}

	if( !m_bHasCreated )
	{
		ALISTELEMENT * pThisElement;

		pThisElement = m_SuperSprayList.GetFirst();
		while( pThisElement != m_SuperSprayList.GetTail() )
		{
			A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
			switch( pSuperSpray->GetParticleType() )
			{
			case A3DPARTICLE_STANDARD_PARTICLES:
				if( !pSuperSpray->CreateStandard() )
					return false;
				break;
			case A3DPARTICLE_META_PARTICLES:
				if( !pSuperSpray->CreateMeta() )
					return false;
				break;
			case A3DPARTICLE_OBJECT_FRAGMENTS:
				g_A3DErrLog.Log("A3DGraphicsFX::CreateAllFX(), SuperSpray can not contain object fragments!");
				return false;
			case A3DPARTICLE_INSTANCED_GEOMETRY:
				if( !pSuperSpray->CreateInstancedGeometry() )
					return false;
				break;
			}
			
			if( !pSuperSpray->UpdateParentTM(m_matAbsoluteTM) )
				return false;

			pThisElement = pThisElement->pNext;
		}

		pThisElement = m_PArrayList.GetFirst();
		while( pThisElement != m_PArrayList.GetTail() )
		{
			A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
			switch( pPArray->GetParticleType() )
			{
			case A3DPARTICLE_OBJECT_FRAGMENTS:
				if( !pPArray->CreateObjectFragment(pHostModel) )
					return false;
				break;
			case A3DPARTICLE_STANDARD_PARTICLES:
			case A3DPARTICLE_META_PARTICLES:
			case A3DPARTICLE_INSTANCED_GEOMETRY:
				g_A3DErrLog.Log("A3DGraphicsFX::CreateAllFX() PArray now only support object fragment particles");
				return false;
			}

			if( !pPArray->UpdateParentTM(m_matAbsoluteTM) )
				return false;

			pThisElement = pThisElement->pNext;
		}

		pThisElement = m_SuperDecalList.GetFirst();
		while( pThisElement != m_SuperDecalList.GetTail() )
		{
			A3DSuperDecal * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;

			pSuperDecal->UpdateParentTM(m_matAbsoluteTM);

			if( m_vMaxDecalSize < pSuperDecal->GetSize() )
				m_vMaxDecalSize = pSuperDecal->GetSize();

			pThisElement = pThisElement->pNext;
		}

		pThisElement = m_BillBoardList.GetFirst();
		while( pThisElement != m_BillBoardList.GetTail() )
		{
			A3DBillBoard * pBillBoard = (A3DBillBoard *) pThisElement->pData;
			if( !pBillBoard->Create() )
				return false;

			if( !pBillBoard->UpdateParentTM(m_matAbsoluteTM) )
				return false;

			if( !pBillBoard->SplitMark() )
				return false;

			pThisElement = pThisElement->pNext;
		}
	}
	else
	{	
		if( m_PArrayList.GetSize() > 0 )
		{
			g_A3DErrLog.Log("A3DGraphicsFX::CreateAll, Recreated, but has parray!");
			return false;
		}

		if( !RecalculateAllFX() )
			return false;
	}

	m_bHasCreated = true;
	return true;
}

bool A3DGraphicsFX::CleanRefInEventList(A3DObject * pElement)
{
	ALISTELEMENT * pThisElement = m_PlayEventList.GetFirst();
	while( pThisElement != m_PlayEventList.GetTail() )
	{
		A3DGFXPlayEvent * pEvent = (A3DGFXPlayEvent *) pThisElement->pData;

		if( pEvent->HasElement(pElement) )
		{
			if( !pEvent->GetRidOfElement(pElement) )
			{
				// We have to delete  this event;
				pThisElement = pThisElement->pNext;
				DeletePlayEvent(pEvent);
				continue;
			}
		}

		pThisElement = pThisElement->pNext;
	}
	return true;
}

bool A3DGraphicsFX::AddSuperSpray(A3DSuperSpray * pSuperSpray)
{
	if( m_bHWIGFX )		return true;

	m_SuperSprayList.Append((LPVOID) pSuperSpray, NULL);
	return true;
}

bool A3DGraphicsFX::DeleteSuperSpray(A3DSuperSpray * pSuperSpray)
{
	if( m_bHWIGFX )		return true;

	// First we must clean the reference in playevent;
	if( !CleanRefInEventList(pSuperSpray) )
		return false;
	
	ALISTELEMENT * pElement = m_SuperSprayList.FindElementByData(pSuperSpray);
	if( NULL == pElement )
		return false;
	return m_SuperSprayList.Delete(pElement);
}

bool A3DGraphicsFX::AddPArray(A3DPArray * pPArray)
{
	if( m_bHWIGFX )		return true;

	m_PArrayList.Append((LPVOID) pPArray);
	return true;
}

bool A3DGraphicsFX::DeletePArray(A3DPArray * pPArray)
{
	if( m_bHWIGFX )		return true;
	
	if( !CleanRefInEventList(pPArray) )
		return false;

	ALISTELEMENT * pElement = m_PArrayList.FindElementByData(pPArray);
	if( NULL == pElement )
		return false;

	return m_PArrayList.Delete(pElement);
}

bool A3DGraphicsFX::AddSuperDecal(A3DSuperDecal * pSuperDecal)
{
	if( m_bHWIGFX )		return true;
	
	m_SuperDecalList.Append((LPVOID) pSuperDecal);
	return true;
}

bool A3DGraphicsFX::DeleteSuperDecal(A3DSuperDecal * pSuperDecal)
{
	if( m_bHWIGFX )		return true;

	if( !CleanRefInEventList(pSuperDecal) )
		return false;

	ALISTELEMENT * pElement = m_SuperDecalList.FindElementByData(pSuperDecal);
	if( NULL == pElement )
		return false;

	return m_SuperDecalList.Delete(pElement);
}

bool A3DGraphicsFX::AddBillBoard(A3DBillBoard * pBillBoard)
{
	if( m_bHWIGFX )		return true;

	m_BillBoardList.Append((LPVOID) pBillBoard);
	return true;
}

bool A3DGraphicsFX::DeleteBillBoard(A3DBillBoard * pBillBoard)
{
	if( m_bHWIGFX )		return true;

	if( !CleanRefInEventList(pBillBoard) )
		return false;

	ALISTELEMENT * pElement = m_BillBoardList.FindElementByData(pBillBoard);
	if( NULL == pElement )
		return false;

	return m_BillBoardList.Delete(pElement);
}

bool A3DGraphicsFX::AddPlayEvent(A3DGFXPlayEvent * pEvent)
{
	if( m_bHWIGFX )		return true;

	m_PlayEventList.Append((LPVOID) pEvent);
	return true;
}

bool A3DGraphicsFX::DeletePlayEvent(A3DGFXPlayEvent * pEvent)
{
	if( m_bHWIGFX)		return true;

	ALISTELEMENT * pElement = m_PlayEventList.FindElementByData(pEvent);
	if( NULL == pElement )
		return false;

	return m_PlayEventList.Delete(pElement);
}

A3DSuperSpray * A3DGraphicsFX::GetSuperSprayByName(char * szName)
{
	ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		if( 0 == _stricmp(pSuperSpray->GetName(), szName) )
			return pSuperSpray;
		pThisElement = pThisElement->pNext;
	}
	return NULL;
}

A3DPArray * A3DGraphicsFX::GetPArrayByName(char * szName)
{
	ALISTELEMENT * pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
		if( 0 == _stricmp(pPArray->GetName(), szName) )
			return pPArray;
		pThisElement = pThisElement->pNext;
	}
	return NULL;
}

A3DSuperDecal * A3DGraphicsFX::GetSuperDecalByName(char * szName)
{
	ALISTELEMENT * pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		if( 0 == _stricmp(pSuperDecal->GetName(), szName) )
			return pSuperDecal;
		pThisElement = pThisElement->pNext;
	}
	return NULL;
}

A3DBillBoard * A3DGraphicsFX::GetBillBoardByName(char * szName)
{
	ALISTELEMENT * pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		if( 0 == _stricmp(pBillBoard->GetName(), szName) )
			return pBillBoard;
		pThisElement = pThisElement->pNext;
	}
	return NULL;
}

A3DGFXPlayEvent * A3DGraphicsFX::GetPlayEventByName(char * szName)
{
	ALISTELEMENT * pThisElement = m_PlayEventList.GetFirst();
	while( pThisElement != m_PlayEventList.GetTail() )
	{
		A3DGFXPlayEvent * pEvent = (A3DGFXPlayEvent *) pThisElement->pData;
		if( 0 == _stricmp(pEvent->GetName(), szName) )
			return pEvent;
		pThisElement = pThisElement->pNext;
	}
	return NULL;
}

bool A3DGraphicsFX::SetPos(A3DVECTOR3 vecPos)
{
	m_vecPos = vecPos;
	m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);
	return UpdateAbsoluteTM();
}

bool A3DGraphicsFX::SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	m_vecDir = vecDir;
	m_vecUp  = vecUp;
	m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);
	return UpdateAbsoluteTM();
}

bool A3DGraphicsFX::SetDir(A3DVECTOR3 vecDir)
{
	m_vecDir = vecDir;

	// Calculate a correct up vector;
	A3DVECTOR3 vecLeft;
	if( fabs(m_vecDir.y) > 0.9f )
		m_vecUp = A3DVECTOR3(1.0f, 0.0f, 0.0f); // X-Axis;
	else
		m_vecUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	vecLeft = Normalize(CrossProduct(m_vecDir, m_vecUp));
	m_vecUp = Normalize(CrossProduct(vecLeft, m_vecDir));

	m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);
	return UpdateAbsoluteTM();
}

bool A3DGraphicsFX::SetParentTM(A3DMATRIX4 parentTM)
{
	m_matParentTM = parentTM;
	return UpdateAbsoluteTM();
}

bool A3DGraphicsFX::UpdateAbsoluteTM()
{
	m_matAbsoluteTM = m_matRelativeTM * m_matParentTM;
	
	// Update my child effects parent tm;
	ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		if( !pSuperSpray->UpdateParentTM(m_matAbsoluteTM) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
		if( !pPArray->UpdateParentTM(m_matAbsoluteTM) )
			return false;
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		pSuperDecal->UpdateParentTM(m_matAbsoluteTM);
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		if( !pBillBoard->UpdateParentTM(m_matAbsoluteTM) )
			return false;
		pThisElement = pThisElement->pNext;
	}
	return true;
}

bool A3DGraphicsFX::UpdateParentInfo()
{
	if( m_bLinked )
	{
		if( m_pParentFrame )
		{
			SetParentTM(m_pParentFrame->GetAbsoluteTM());
		}
	}
	return true;
}

bool A3DGraphicsFX::SetScale(FLOAT vScale)
{
	return SetScaleAll(vScale, vScale, vScale);
}

bool A3DGraphicsFX::SetLength(FLOAT vLength)
{
	m_vLength = vLength;

	ALISTELEMENT * pThisElement; 

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		pBillBoard->SetLength(m_vLength);
		pThisElement = pThisElement->pNext;
	}
	return true;
}

bool A3DGraphicsFX::SetScaleAll(FLOAT vScaleX, FLOAT vScaleY, FLOAT vScaleZ)
{
	m_vScaleX = vScaleX;
	m_vScaleY = vScaleY;
	m_vScaleZ = vScaleZ;

	m_vScale = (m_vScaleX + m_vScaleY + m_vScaleZ) / 3.0f;

	ALISTELEMENT * pThisElement = m_SuperSprayList.GetFirst();
	while( pThisElement != m_SuperSprayList.GetTail() )
	{
		A3DSuperSpray  * pSuperSpray = (A3DSuperSpray *) pThisElement->pData;
		pSuperSpray->SetScale(m_vScale);
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_PArrayList.GetFirst();
	while( pThisElement != m_PArrayList.GetTail() )
	{
		A3DPArray  * pPArray = (A3DPArray *) pThisElement->pData;
		pPArray->SetScale(m_vScale);
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_SuperDecalList.GetFirst();
	while( pThisElement != m_SuperDecalList.GetTail() )
	{
		A3DSuperDecal  * pSuperDecal = (A3DSuperDecal *) pThisElement->pData;
		pSuperDecal->SetScale(m_vScale);
		pThisElement = pThisElement->pNext;
	}

	pThisElement = m_BillBoardList.GetFirst();
	while( pThisElement != m_BillBoardList.GetTail() )
	{
		A3DBillBoard  * pBillBoard = (A3DBillBoard *) pThisElement->pData;
		pBillBoard->SetScaleX(m_vScaleX);
		pBillBoard->SetScaleY(m_vScaleY);
		pBillBoard->SetScaleZ(m_vScaleZ);
		pThisElement = pThisElement->pNext;
	}
	return true;
}

bool A3DGraphicsFX::ActiveAllPlayEvents(bool bActive)
{
	ALISTELEMENT * pThisElement = m_PlayEventList.GetFirst();

	while( pThisElement != m_PlayEventList.GetTail() )
	{
		A3DGFXPlayEvent * pEvent = (A3DGFXPlayEvent *) pThisElement->pData;

		pEvent->SetActive(bActive);

		pThisElement = pThisElement->pNext;
	}

	return true;
}

bool A3DGraphicsFX::FirePlayEvent(A3DGFXPlayEvent * pEvent)
{
	switch(pEvent->GetType())
	{
	case A3DGFXPlayEvent::TYPE_ITEM:
	{
		A3DGFXPlayEvent::EVENTITEM* pItem;
		pItem = (A3DGFXPlayEvent::EVENTITEM*) pEvent->GetItemPtr();
		switch(pItem->dwClassID)
		{
		case A3D_CID_SUPERSPRAY:
			A3DSuperSpray * pSuperSpray;
			pSuperSpray = (A3DSuperSpray *) pItem->pElement;
			if( !pSuperSpray->UpdateParentTM(m_matAbsoluteTM) )
				return false;
			if( !pSuperSpray->Start() )
				return false;
			break;

		case A3D_CID_PARRAY:
			A3DPArray * pPArray;
			pPArray = (A3DPArray *) pItem->pElement;
			if( !pPArray->UpdateParentTM(m_matAbsoluteTM) )
				return false;
			if( !pPArray->Start() )
				return false;
			break;

		case A3D_CID_SUPERDECAL:
			A3DSuperDecal * pSuperDecal;
			pSuperDecal = (A3DSuperDecal *) pItem->pElement;
			pSuperDecal->Play();
			break;

		case A3D_CID_BILLBOARD:
			A3DBillBoard * pBillBoard;
			pBillBoard = (A3DBillBoard *) pItem->pElement;
			if( !pBillBoard->Start() )
				return false;
			break;

		default:
			g_A3DErrLog.Log("A3DGraphicsFX::FirePlayEvent(), Unknown element class id");
			return false;
		}
		break;
	}
	case A3DGFXPlayEvent::TYPE_RANDITEM:

		g_A3DErrLog.Log("A3DGraphicsFX::FirePlayEvent(), unsupported event type used!");
		break;
	}

	return true;
}

// section for calss A3DGFXPlayEvent;
A3DGFXPlayEvent::A3DGFXPlayEvent()
{
	m_dwClassID		= A3D_CID_GFXPLAYEVENT;
	m_pHostGFX		= NULL;

	m_Type			= TYPE_NULL;
	m_pItem			= NULL;
	m_nStartBegin	= 0;
	m_nStartEnd		= 0;
	m_nThisStart	= 0;
}

A3DGFXPlayEvent::~A3DGFXPlayEvent()
{
}

bool A3DGFXPlayEvent::Init(A3DGraphicsFX * pHostGFX)
{
	m_pHostGFX		= pHostGFX;
	m_Type			= TYPE_NULL;
	m_pItem			= NULL;
	m_nStartBegin	= 0;
	m_nStartEnd		= 0;

	m_nThisStart	= 0;
	m_bIsActive		= false;
	return true;
}

bool A3DGFXPlayEvent::Release()
{
	if( m_pItem )
	{
		delete m_pItem;
		m_pItem = NULL;
	}

	m_Type = TYPE_NULL;
	return true;
}

bool A3DGFXPlayEvent::Load(A3DGraphicsFX * pHostGFX, AFile * pFileToLoad)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;
	
	if( !Init(pHostGFX) )
		return false;

	//Load super spray specification data first;
	if( pFileToLoad->IsBinary() )
	{
		//Load Binary Data here;
	}
	else
	{
		//Load Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];
		char			szResult[AFILE_LINEMAXLEN];
		DWORD			dwReadLen;

		int				nType;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Type: %d", &nType);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "StartBegin: %d", &m_nStartBegin);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "StartEnd: %d", &m_nStartEnd);

		switch(nType)
		{
		case TYPE_ITEM:
			EVENTITEM * pItem;
			
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			a_GetStringAfter(szLineBuffer, "ITEMELEMENT: ", szResult);

			// Find the item's element effect;
			A3DObject * pItemElement;
			pItemElement = NULL;
			if( (pItemElement = m_pHostGFX->GetBillBoardByName(szResult)) )
			{
			}
			else if( (pItemElement = m_pHostGFX->GetSuperDecalByName(szResult)) )
			{
			}
			else if( (pItemElement = m_pHostGFX->GetSuperSprayByName(szResult)) )
			{
			}
			else if( (pItemElement = m_pHostGFX->GetPArrayByName(szResult)) )
			{
			}

			if( !pItemElement )
			{
				g_A3DErrLog.Log("A3DGFXPlayEvent::Load(), Item effect [%s] missing!", szResult);
				return false;
			}

			if (!(pItem = new EVENTITEM))
			{
				g_A3DErrLog.Log("A3DGFXPlayEvent::Load(), Not enough memory!");
				return false;
			}

			pItem->pElement = pItemElement;
			pItem->dwClassID = pItemElement->GetClassID();
			
			m_pItem = pItem;
			m_Type = TYPE_ITEM;
			break;

		case TYPE_RANDITEM:
			g_A3DErrLog.Log("A3DGFXPlayEvent::Load(), RandItem not supported now!");
			return false;

		default:
			g_A3DErrLog.Log("A3DGFXPlayEvent::Load(), Unknown event type not supported now!");
			return false;
		}
	}

	return true;
}

bool A3DGFXPlayEvent::Save(AFile * pFileToSave)
{
	//Save billboard specification data first;
	if( pFileToSave->IsBinary() )
	{
		//Save Binary Data here;
	}
	else
	{
		//Save Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];

		sprintf(szLineBuffer, "Type: %d", m_Type);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "StartBegin: %d", m_nStartBegin);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "StartEnd: %d", m_nStartEnd);
		pFileToSave->WriteLine(szLineBuffer);
		
		switch(m_Type)
		{
		case TYPE_ITEM:
			EVENTITEM* pItem;
			pItem = (EVENTITEM*)m_pItem;
			if (pItem)
				sprintf(szLineBuffer, "ITEMELEMENT: %s", pItem->pElement->GetName());
			else
				sprintf(szLineBuffer, "ITEMELEMENT: ");
			pFileToSave->WriteLine(szLineBuffer);
			break;

		case TYPE_RANDITEM:
			g_A3DErrLog.Log("A3DGFXPlayEvent::Save(), RandItem not supported now!");
			return false;

		default:
			g_A3DErrLog.Log("A3DGFXPlayEvent::Save(), Unknown event type not supported now!");
			return false;
		}
	}

	return true;
}

bool A3DGFXPlayEvent::SetActive(bool bActive)
{
	m_bIsActive = bActive;

	if( m_bIsActive )
	{
		// Determine this time start tick;
		if( m_nStartBegin == m_nStartEnd )
			m_nThisStart = m_nStartBegin;
		else
			m_nThisStart = m_nStartBegin + a_Random() % (m_nStartEnd - m_nStartBegin + 1);
	}

	return true;
}

// method to query if a specified element is referenced by this event;
bool A3DGFXPlayEvent::HasElement(A3DObject * pElement)
{
	switch(m_Type)
	{
	case TYPE_ITEM:
		EVENTITEM* pItem;
		pItem = (EVENTITEM*) m_pItem;
		if( pItem->pElement == pElement )
			return true;
		break;
	case TYPE_RANDITEM:
		break;
	default:
		break;
	}

	return false;
}

// methods let the event to clean the reference to some element, return false will let gfx to delete this event
bool A3DGFXPlayEvent::GetRidOfElement(A3DObject * pElement)
{
	return false;
}