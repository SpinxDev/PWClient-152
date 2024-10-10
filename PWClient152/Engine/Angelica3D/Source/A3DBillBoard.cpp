#include "A3DBillBoard.h"
#include "A3DPI.h"
#include "A3DStream.h"
#include "A3DViewport.h"
#include "A3DGFXMan.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"
#include "A3DCollision.h"
#include "A3DVertexCollector.h"
#include "A3DTextureMan.h"
#include "A3DSceneHull.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DCameraBase.h"
#include "A3DCDS.h"
#include "A3DIBLScene.h"
#include "AFile.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DBillBoard::A3DBillBoard()
{
	m_dwClassID		= A3D_CID_BILLBOARD;
	m_pA3DDevice	= NULL;
	m_pTexture		= NULL;
	m_pStream		= NULL;
	m_pMarkVerts	= NULL;
	m_pMarkIndices	= NULL;

	m_bExpired		= true;
	m_vSizeX = m_vSizeY = m_vSizeZ = m_vAngle = 0.0f;
	m_vScaleX = m_vScaleY = m_vScaleZ = 1.0f;
	m_vLength = 0.0f;

	m_nKeyNumber	= 0;
	m_nCurrentKey	= 0;
	m_nTexRow		= 1;
	m_nTexCol		= 1;
	m_nTexInterval	= 1;
	m_nTexCurRow	= 0;
	m_nTexCurCol	= 0;
	m_nRepeatCount  = 1;

	m_nMarkVertCount = 0;
	m_nMarkIndexCount = 0;

	m_nDLightID		= -1;
}

A3DBillBoard::~A3DBillBoard()
{
}

bool A3DBillBoard::Init(A3DDevice * pA3DDevice)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;

	m_nRepeatNumber = 1;
	m_bTextureRepeatFlag = false; // by default an sequence will stop at the last frame

	m_nKeyNumber = 1; 
	m_pKeyInfos[0].vSizeX = 0.5f;
	m_pKeyInfos[0].vSizeY = 0.5f;
	m_pKeyInfos[0].vSizeZ = 4.0f;
	m_pKeyInfos[0].vAngle = 0.0f;
	m_pKeyInfos[0].color = A3DCOLORRGBA(255, 255, 255, 255);
	m_pKeyInfos[0].nTick = 0;

	m_Type = A3DBILLBOARD_BILL;
	m_bExpired = true;
	m_nTicks = 0;
	m_Color = 0;
	m_nTexRow = m_nTexCol = 1;

	m_nDLightID	= -1;

	m_SrcBlend = A3DBLEND_SRCALPHA;
	m_DestBlend = A3DBLEND_INVSRCALPHA;

	m_vecPos = A3DVECTOR3(0.0f);
	m_vecDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUp  = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecLeft= Normalize(CrossProduct(m_vecDir, m_vecUp));
	m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);
	m_matParentTM.Identity();
	m_matAbsoluteTM = m_matRelativeTM * m_matParentTM;

	return true;
}

bool A3DBillBoard::Create()
{
	if( m_Type == A3DBILLBOARD_DYNAMICLIGHT )
	{
		// If do not need dynamic light, we can kill this effect
		if( !g_pA3DConfig->GetFlagDynamicLight() )
		{
			m_pA3DDevice = NULL;
			return true;
		}
	}
	if( !m_pA3DDevice )	return true;

	static WORD index[] = { 0, 1, 2, 2, 1, 3, 
							4, 5, 6, 6, 5, 7, 
							8, 9, 10, 10, 9, 11 };

	if (m_strTextureMap.GetLength())
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_strTextureMap, m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pTexture, A3DTF_MIPLEVEL, 1) )
		{
			g_A3DErrLog.Log("A3DBillBoard::Create(), Can not find texture: [%s]", m_strTextureMap);
			return false;
		}
	}
	
	m_pStream = new A3DStream();
	if( NULL == m_pStream )
	{
		g_A3DErrLog.Log("A3DBillBoard::Create() Not enough memory!");
		return false;
	}

	A3DCDS* pA3DCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
	if ((m_Type == A3DBILLBOARD_MARK || m_Type == A3DBILLBOARD_DYNAMICLIGHT || m_Type == A3DBILLBOARD_SHADOW) &&
		pA3DCDS && pA3DCDS->CanSplitMark())
	{
		m_pMarkVerts = (A3DLVERTEX*)a_malloc(sizeof(A3DLVERTEX) * A3DSceneHull::MAXNUM_MARKVERT);
		if( NULL == m_pMarkVerts )
		{
			g_A3DErrLog.Log("A3DBillBoard::Create() Not enough memory!");
			return false;
		}
		m_pMarkIndices = (WORD *)a_malloc(sizeof(WORD) * A3DSceneHull::MAXNUM_MARKINDEX);
		if( NULL == m_pMarkIndices )
		{
			g_A3DErrLog.Log("A3DBillBoard::Create() Not enough memory!");
			return false;
		}

		if (!m_pStream->Init(m_pA3DDevice, A3DVT_LVERTEX, A3DSceneHull::MAXNUM_MARKVERT, 
			A3DSceneHull::MAXNUM_MARKINDEX, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
		{
			g_A3DErrLog.Log("A3DBillBoard::Create() Init A3DStream Fail!");
			return false;
		}
	}
	else
	{
		if (!m_pStream->Init(m_pA3DDevice, A3DVT_LVERTEX, 12, 18, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
		{
			g_A3DErrLog.Log("A3DBillBoard::Create() Init A3DStream Fail!");
			return false;
		}

		m_pStream->SetIndices(index, 18);
	}

	m_vecLeft= Normalize(CrossProduct(m_vecDir, m_vecUp));
	UpdateRelativeTM();

	if( m_pA3DDevice->GetA3DEngine()->IsOptimizedEngine() && g_pA3DConfig->GetRunEnv() != A3DRUNENV_GFXEDITOR )
	{
		A3DSHADER shader;
		shader.SrcBlend = m_SrcBlend;
		shader.DestBlend = m_DestBlend;
		m_hVertexCollectorHandle = m_pA3DDevice->GetA3DEngine()->GetVertexCollector()->RegisterTexture(A3DVT_LVERTEX, m_pTexture, NULL, &shader);
		if( NULL == m_hVertexCollectorHandle )
		{
			g_A3DErrLog.Log("A3DBillBoard::Init() Register texture in VertexCollector fail!");
			return false;
		}
	}
	return true;
}

bool A3DBillBoard::Release()
{
	if( m_pMarkVerts )
	{
		a_free(m_pMarkVerts);
		m_pMarkVerts = NULL;
	}
	if( m_pMarkIndices )
	{
		a_free(m_pMarkIndices);
		m_pMarkIndices = NULL;
	}
	if( m_pTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}
	if( m_pStream )
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}
	return true;
}

bool A3DBillBoard::TickAnimation()
{
	if( !m_pA3DDevice )	return true;

	if( m_bExpired )
		return true;

	if( m_nKeyNumber < 1 )
	{
		Stop();
		return true;
	}

	if( m_nKeyNumber > 1 )
	{
		if( m_nTicks >= m_pKeyInfos[m_nCurrentKey + 1].nTick )
			m_nCurrentKey ++;
			
		if( m_nCurrentKey >= m_nKeyNumber - 1 ) // Reach last key;
		{
			m_nRepeatCount ++;
			if( m_nRepeatNumber > 0 && m_nRepeatCount >= m_nRepeatNumber )
			{
				Stop();
				return true;
			}
			else
			{
				m_nTicks		= 0;
				m_nCurrentKey	= 0;
				m_nTexCurRow	= 0;
				m_nTexCurCol	= 0;
			}
		}

		if( m_nTicks > 0 && 0 == m_nTicks % m_nTexInterval )
		{
			m_nTexCurCol ++;
			if( m_nTexCurCol >= m_nTexCol )
			{
				m_nTexCurRow ++;
				if( m_nTexCurRow >= m_nTexRow )
				{
					if( m_bTextureRepeatFlag )//m_nRepeatNumber > 1 )
					{
						m_nTexCurRow = 0;
						m_nTexCurCol = 0;
					}
					else
					{
						//We should wait here;
						m_nTexCurRow --;
						m_nTexCurCol --;
					}
				}
				else
				{
					m_nTexCurCol = 0;
				}
			}
		}

		// Update this bill board info;
		BILLBOARD_KEYINFO *	pKey1 = &m_pKeyInfos[m_nCurrentKey];
		BILLBOARD_KEYINFO *	pKey2 = &m_pKeyInfos[m_nCurrentKey + 1];

		// Interpolate between key 1 and key 2;
		if( pKey2->nTick == pKey1->nTick )
			goto NEXT;

		FLOAT f = 1.0f - 1.0f * (m_nTicks - pKey1->nTick) / (pKey2->nTick - pKey1->nTick);
		m_vSizeX = pKey1->vSizeX * f + pKey2->vSizeX * (1.0f - f);
		m_vSizeY = pKey1->vSizeY * f + pKey2->vSizeY * (1.0f - f);
		m_vSizeZ = pKey1->vSizeZ * f + pKey2->vSizeZ * (1.0f - f);
		m_vAngle = pKey1->vAngle * f + pKey2->vAngle * (1.0f - f);

		int r1, g1, b1, a1, r2, g2, b2, a2, r, g, b, a;
		r1 = A3DCOLOR_GETRED  (pKey1->color);
		g1 = A3DCOLOR_GETGREEN(pKey1->color);
		b1 = A3DCOLOR_GETBLUE (pKey1->color);
		a1 = A3DCOLOR_GETALPHA(pKey1->color);
		r2 = A3DCOLOR_GETRED  (pKey2->color);
		g2 = A3DCOLOR_GETGREEN(pKey2->color);
		b2 = A3DCOLOR_GETBLUE (pKey2->color);
		a2 = A3DCOLOR_GETALPHA(pKey2->color);
		r = (int)(r1 * f + r2 * (1.0f - f));
		g = (int)(g1 * f + g2 * (1.0f - f));
		b = (int)(b1 * f + b2 * (1.0f - f));
		a = (int)(a1 * f + a2 * (1.0f - f));
		m_Color = A3DCOLORRGBA(r, g, b, a);
	}
	else //m_nKeyNumber == 1
	{
		m_vSizeX = m_pKeyInfos[0].vSizeX;
		m_vSizeY = m_pKeyInfos[0].vSizeY;
		m_vSizeZ = m_pKeyInfos[0].vSizeZ;
		m_vAngle = m_pKeyInfos[0].vAngle;
		m_Color  = m_pKeyInfos[0].color;
	}

NEXT:
	// Update light if neccessary
	if( m_nDLightID != -1 )
	{
		// Now update dynamic light's all information
		A3DIBLLightGrid * pLightGrid = A3DIBLScene::GetGobalLightGrid();
		if( pLightGrid )
		{
			pLightGrid->UpdateDLightColorOnly(m_nDLightID, a3d_ColorRGBAToColorValue(m_Color));
		}
	}
	m_nTicks ++;
	return true;
}

bool A3DBillBoard::RenderMark(A3DViewport * pCurrentViewport)
{
	if( !m_pA3DDevice )	return true;

	for(int i=0; i<m_nMarkVertCount; i++)
	{
		m_pMarkVerts[i].diffuse = m_Color;
		m_pMarkVerts[i].specular = 0xff000000;
	}

	if( 0 )//!m_pA3DDevice->IsZBiasSupported() && g_pA3DConfig->GetRunEnv() != A3DRUNENV_GFXEDITOR )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetVertexCollector()->PushVertices(m_hVertexCollectorHandle,
			A3DVT_LVERTEX, m_pMarkVerts, m_nMarkVertCount, m_pMarkIndices, m_nMarkIndexCount) )
		{
			g_A3DErrLog.Log("A3DBillBoard::Render add to vertex collector fail!");
			return false;
		}
	}
	else
	{
		A3DCameraBase* pCamera = pCurrentViewport->GetCamera();
		m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
		m_pA3DDevice->SetDestAlpha(m_DestBlend);
		m_pA3DDevice->SetZWriteEnable(false);
		//m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
		A3DMATRIX4 matZPull = a3d_IdentityMatrix();
		matZPull._43 = -3.5f / 32767.0f;
		matZPull = pCamera->GetProjectionTM() * matZPull;
		m_pA3DDevice->SetProjectionMatrix(matZPull);
		m_pStream->SetVertsDynamic((LPBYTE) m_pMarkVerts, m_nMarkVertCount);
		m_pStream->Appear();
		if( m_pTexture )
			m_pTexture->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nMarkVertCount, 0, m_nMarkIndexCount / 3);
		
		if( m_pTexture )
			m_pTexture->Disappear(0);

		m_pA3DDevice->SetProjectionMatrix(pCamera->GetProjectionTM());
		m_pA3DDevice->SetLightingEnable(true);
		m_pA3DDevice->SetZWriteEnable(true);
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
	return true;
}

bool A3DBillBoard::Render(A3DViewport * pCurrentViewport)
{
	if (!m_pA3DDevice || m_bExpired)
		return true;

	if (max2(max2(m_vSizeX, m_vSizeY), m_vSizeZ) < 6.0f)
	{
		//	Check whether particle is visible
		A3DCDS* pCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
		if (pCDS)
		{
			A3DVECTOR3 vPos = m_vecPos * m_matParentTM;

			if (!pCDS->PosIsVisible(vPos, VISOBJ_PARTICAL, 0))
				return true;
		}
	}

	if (m_pMarkVerts && m_pMarkIndices)
		return RenderMark(pCurrentViewport);

	static A3DCOLOR specular = A3DCOLORRGBA(0, 0, 0, 255);
	static FLOAT u[] = {0.0f, 1.0f, 0.0f, 1.0f};
	static FLOAT v[] = {0.0f, 0.0f, 1.0f, 1.0f};
	static WORD index[] = { 0, 1, 2, 2, 1, 3, 
							4, 5, 6, 6, 5, 7, 
							8, 9, 10, 10, 9, 11 };
	
	int	   nRectCount = 0;

	A3DVECTOR3 vecCamDir = pCurrentViewport->GetCamera()->GetDir();

	A3DVECTOR3  pVertPos[12];

	A3DVECTOR3	vecX, vecY, vecZ, vecPos, vecPosEnd, vecStart, vecEnd;
	A3DMATRIX4	matToWorld = m_matParentTM;

	if( m_vAngle != 0.0f )
	{
		matToWorld = a3d_RotateZ(DEG2RAD(m_vAngle)) * matToWorld;
	}

	vecX = -1.0f * m_vecLeft * matToWorld - matToWorld.GetRow(3);
	vecY = m_vecUp * matToWorld - matToWorld.GetRow(3);
	vecZ = m_vecDir * matToWorld - matToWorld.GetRow(3);

	vecPos = m_vecPos * matToWorld;
	//if( m_Type == A3DBILLBOARD_FACING )
	//	vecPos = vecPos + vecZ * 0.1f;

	// Transform into world space;
	vecX = vecX * m_vSizeX * m_vScaleX;
	vecY = vecY * m_vSizeY * m_vScaleY;
	if( m_vLength != 0.0f )
		vecZ = vecZ * m_vLength;
	else
		vecZ = vecZ * m_vSizeZ * m_vScaleZ;

	switch( m_Type )
	{
	case A3DBILLBOARD_BILL:
		// 0, 1, 2, 3 horinontal plane;
		pVertPos[0] = vecPos - vecX + vecZ;
		pVertPos[1] = vecPos + vecX + vecZ;
		pVertPos[2] = vecPos - vecX;
		pVertPos[3] = vecPos + vecX;
		
		// 4, 5, 6, 7 vertical plane;
		pVertPos[4] = vecPos + vecY + vecZ;
		pVertPos[5] = vecPos - vecY + vecZ;
		pVertPos[6] = vecPos + vecY;
		pVertPos[7] = vecPos - vecY;

		nRectCount = 2;
		break;
	case A3DBILLBOARD_FACING:
	case A3DBILLBOARD_MARK:
	case A3DBILLBOARD_DYNAMICLIGHT:
	case A3DBILLBOARD_SHADOW:
	default:
		// 0, 1, 2, 3 vertical plane;
		pVertPos[0] = vecPos - vecX + vecY;
		pVertPos[1] = vecPos + vecX + vecY;
		pVertPos[2] = vecPos - vecX - vecY;
		pVertPos[3] = vecPos + vecX - vecY;
		nRectCount = 1;
		break;
	case A3DBILLBOARD_AXISDECAL:
		// 0, 1, 2, 3 vertical plane along z-axis;
		vecX = Normalize(CrossProduct(vecCamDir, vecY)) * m_vSizeX * m_vScaleX;
		pVertPos[0] = vecPos - vecX + vecY;
		pVertPos[1] = vecPos + vecX + vecY;
		pVertPos[2] = vecPos - vecX;
		pVertPos[3] = vecPos + vecX;
		nRectCount = 1;
		break;
	}

	u[0] = u[2] = 1.0f * m_nTexCurCol / m_nTexCol;
	u[1] = u[3] = 1.0f * (m_nTexCurCol + 1) / m_nTexCol;
	v[0] = v[1] = 1.0f * m_nTexCurRow / m_nTexRow;
	v[2] = v[3] = 1.0f * (m_nTexCurRow + 1) / m_nTexRow;

	for(int i=0; i<nRectCount * 4; i++)
		m_aRenderVerts[i] = A3DLVERTEX(pVertPos[i], m_Color, specular, u[i % 4], v[i % 4]);
	
	if( m_pA3DDevice->GetA3DEngine()->IsOptimizedEngine() && g_pA3DConfig->GetRunEnv() != A3DRUNENV_GFXEDITOR )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetVertexCollector()->PushVertices(m_hVertexCollectorHandle,
			A3DVT_LVERTEX, m_aRenderVerts, nRectCount * 4, index, nRectCount * 6) )
		{
			g_A3DErrLog.Log("A3DBillBoard::Render add to vertex collector fail!");
			return false;
		}
	}
	else
	{
		m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
		m_pA3DDevice->SetDestAlpha(m_DestBlend);
		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
		m_pStream->SetVertsDynamic((LPBYTE) m_aRenderVerts, nRectCount * 4);
		m_pStream->Appear();
		if( m_pTexture )
			m_pTexture->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nRectCount * 4, 0, nRectCount * 2);
		
		if( m_pTexture )
			m_pTexture->Disappear(0);

		m_pA3DDevice->SetLightingEnable(true);
		m_pA3DDevice->SetZWriteEnable(true);
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
	return true;
}

bool A3DBillBoard::Start()
{
	if( !m_pA3DDevice )	return true;

	m_bExpired = false;
	m_nTicks = 0;
	m_nCurrentKey = 0;
	m_nRepeatCount = 0;
	m_nTexCurRow = 0;
	m_nTexCurCol = 0;

	m_vSizeX = m_pKeyInfos[0].vSizeX;
	m_vSizeY = m_pKeyInfos[0].vSizeY;
	m_vSizeZ = m_pKeyInfos[0].vSizeZ;
	m_vAngle = m_pKeyInfos[0].vAngle;
	m_Color  = m_pKeyInfos[0].color;

	// Apply for a dynamic light before I will show up
	if( m_Type == A3DBILLBOARD_DYNAMICLIGHT )
	{
		A3DIBLLightGrid * pLightGrid = A3DIBLScene::GetGobalLightGrid();
		if( pLightGrid )
		{
			if( m_nDLightID != -1 )
				pLightGrid->SetFreeDLight(m_nDLightID);
			
			pLightGrid->GetNextDLight(&m_nDLightID);
			if( m_nDLightID != -1 )
			{
				// Now update dynamic light's all information
				A3DIBLLightGrid * pLightGrid = A3DIBLScene::GetGobalLightGrid();
				if( pLightGrid )
				{
					pLightGrid->UpdateDLight(m_nDLightID, m_matAbsoluteTM.GetRow(3), a3d_ColorRGBAToColorValue(m_Color), m_vSizeX * 2.0f);
				}
			}
		}
	}
	return true;
}

bool A3DBillBoard::Stop()
{
	m_bExpired = true;

	if( m_Type == A3DBILLBOARD_DYNAMICLIGHT && m_nDLightID != -1 )
	{
		A3DIBLLightGrid * pLightGrid = A3DIBLScene::GetGobalLightGrid();
		if( pLightGrid )
		{
			pLightGrid->SetFreeDLight(m_nDLightID);
			m_nDLightID = -1;
		}
	}
	return true;
}

bool A3DBillBoard::UpdateRelativeTM()
{
	m_vecLeft = Normalize(CrossProduct(m_vecDir, m_vecUp));

	m_matRelativeTM = a3d_TransformMatrix(m_vecDir, m_vecUp, m_vecPos);

	return UpdateAbsoluteTM();
}

bool A3DBillBoard::UpdateAbsoluteTM()
{
	m_matAbsoluteTM = m_matRelativeTM * m_matParentTM;
	return true;
}

bool A3DBillBoard::UpdateUp()
{
	if( fabs(m_vecDir.y) > 0.9f )
		m_vecUp = A3DVECTOR3(1.0f, 0.0f, 0.0f); // X-Axis;
	else
		m_vecUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecLeft = Normalize(CrossProduct(m_vecDir, m_vecUp));
	m_vecUp = Normalize(CrossProduct(m_vecLeft, m_vecDir));
	return true;
}

bool A3DBillBoard::Save(AFile * pFileToSave)
{
	if( !m_pA3DDevice )	return true;

	//Save billboard specification data first;
	if( pFileToSave->IsBinary() )
	{
		//Save Binary Data here;
	}
	else
	{
		//Save Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];

		sprintf(szLineBuffer, "Pos: (%f, %f, %f)", m_vecPos.x, m_vecPos.y, m_vecPos.z);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Dir: (%f, %f, %f)", m_vecDir.x, m_vecDir.y, m_vecDir.z);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Up: (%f, %f, %f)", m_vecUp.x, m_vecUp.y, m_vecUp.z);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "TextureMap: %s", m_strTextureMap);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SrcBlend: %d", m_SrcBlend);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "DestBlend: %d", m_DestBlend);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TextureRow: %d", m_nTexRow);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TextureCol: %d", m_nTexCol);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TextureInterval: %d", m_nTexInterval);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Type: %d", m_Type);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "KeyNumber: %d", m_nKeyNumber);
		pFileToSave->WriteLine(szLineBuffer);
		for(int i=0; i<m_nKeyNumber; i++)
		{
			sprintf(szLineBuffer, "Key: (%d, %d, %f, %f, %f, %f, %x)", i, m_pKeyInfos[i].nTick,
				m_pKeyInfos[i].vSizeX, m_pKeyInfos[i].vSizeY, m_pKeyInfos[i].vSizeZ, m_pKeyInfos[i].vAngle, m_pKeyInfos[i].color);
			pFileToSave->WriteLine(szLineBuffer);
		}
		sprintf(szLineBuffer, "RepeatNumber: %d", GetRepeatNumber());
		pFileToSave->WriteLine(szLineBuffer);
	}
	return true;
}

bool A3DBillBoard::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;
	
	if( !Init(pA3DDevice) )
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

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Pos: (%f, %f, %f)", &m_vecPos.x, &m_vecPos.y, &m_vecPos.z);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Dir: (%f, %f, %f)", &m_vecDir.x, &m_vecDir.y, &m_vecDir.z);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Up: (%f, %f, %f)", &m_vecUp.x, &m_vecUp.y, &m_vecUp.z);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TextureMap: ", szResult);
		m_strTextureMap = szResult;
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strstr(szLineBuffer, "SrcBlend: ") == szLineBuffer )
		{
			sscanf(szLineBuffer, "SrcBlend: %d", &m_SrcBlend);
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "DestBlend: %d", &m_DestBlend);
			// Prepare for following reading;
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		}

		sscanf(szLineBuffer, "TextureRow: %d", &m_nTexRow);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "TextureCol: %d", &m_nTexCol);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "TextureInterval: %d", &m_nTexInterval);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Type: %d", &m_Type);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "KeyNumber: %d", &m_nKeyNumber);
		
		for(int i=0; i<m_nKeyNumber; i++)
		{
			int nval;
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "Key: (%d, %d, %f, %f, %f, %f, %x)", &nval, &m_pKeyInfos[i].nTick,
				&m_pKeyInfos[i].vSizeX, &m_pKeyInfos[i].vSizeY, &m_pKeyInfos[i].vSizeZ, &m_pKeyInfos[i].vAngle, &m_pKeyInfos[i].color);

			if( m_Type != A3DBILLBOARD_BILL )
				m_pKeyInfos[i].vSizeZ = 0.0f;
		}
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "RepeatNumber: %d", &m_nRepeatNumber);
		
		m_bTextureRepeatFlag = m_nRepeatNumber < 0 ? true : false;
		m_nRepeatNumber = abs(m_nRepeatNumber);
	}

	return true;
}

bool A3DBillBoard::UpdateParentTM(const A3DMATRIX4& matParentTM)
{
	m_matParentTM = matParentTM;
	UpdateAbsoluteTM();
	return true;
}

bool A3DBillBoard::UpdateInfo()
{
	if( m_nKeyNumber == 1 )
	{
		m_vSizeX = m_pKeyInfos[0].vSizeX;
		m_vSizeY = m_pKeyInfos[0].vSizeY;
		m_vSizeZ = m_pKeyInfos[0].vSizeZ;
		m_vAngle = m_pKeyInfos[0].vAngle;
		m_Color  = m_pKeyInfos[0].color;
	}
	return true;
}

bool A3DBillBoard::SplitMark()
{
	if( !m_pA3DDevice )	return true;

	if( m_Type != A3DBILLBOARD_MARK && m_Type != A3DBILLBOARD_DYNAMICLIGHT && m_Type != A3DBILLBOARD_SHADOW )
		return true;

	m_vSizeX = m_pKeyInfos[0].vSizeX;
	m_vSizeY = m_pKeyInfos[0].vSizeY;

	if( m_pMarkVerts && m_pMarkIndices )
	{
		A3DCDS * pCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
		if (!pCDS || !pCDS->CanSplitMark())
			return true;

		FLOAT vRadiusX = m_vSizeX * m_vScaleX;
		FLOAT vRadiusZ = m_vSizeY * m_vScaleY;

		if( vRadiusX == 0.0f || vRadiusZ == 0.0f )
			return true;

		A3DAABB aabb;	

		aabb.Mins = m_matAbsoluteTM.GetRow(3) - A3DVECTOR3(vRadiusX, (vRadiusX + vRadiusZ) / 2.0f, vRadiusZ);
		aabb.Maxs = m_matAbsoluteTM.GetRow(3) + A3DVECTOR3(vRadiusX, (vRadiusX + vRadiusZ) / 2.0f, vRadiusZ);
		a3d_CompleteAABB(&aabb);

		float vMarkRangeFactor = 0.2f; // This is how far from center that faces will be marked
		A3DVECTOR3 vecNormal;
		bool  bJog = true;

		if( m_Type == A3DBILLBOARD_DYNAMICLIGHT )
		{
			vecNormal = A3DVECTOR3(0.0f);//
			vMarkRangeFactor = 1.0f;
			bJog = false;
		}
		else
		{
			if( m_Type == A3DBILLBOARD_SHADOW ) // We will consider face normal for shadows, because shadow shall not make effect on other side of the wall
				vecNormal = m_matAbsoluteTM.GetRow(2);
			else
				vecNormal = A3DVECTOR3(0.0f);
			vMarkRangeFactor = 0.5f;
			bJog = false;
		}

		if (!pCDS->SplitMark(aabb, vecNormal, m_pMarkVerts, m_pMarkIndices, bJog, &m_nMarkVertCount, &m_nMarkIndexCount, vMarkRangeFactor))
		{
			g_A3DErrLog.Log("A3DBillBoard::SplitMark(), Fail to split the mark on the wall");
			return false;
		}
	
		m_pStream->SetIndicesDynamic(m_pMarkIndices, m_nMarkIndexCount);	
	}

	// If this call is during a dynamic light flying
	if( m_nDLightID != -1 )
	{
		// Now update dynamic light's all information
		A3DIBLLightGrid * pLightGrid = A3DIBLScene::GetGobalLightGrid();
		if( pLightGrid )
		{
			pLightGrid->UpdateDLight(m_nDLightID, m_matAbsoluteTM.GetRow(3), a3d_ColorRGBAToColorValue(m_Color), m_vSizeX * 2.0f);
		}
	}
	return true;
}

bool A3DBillBoard::ChangeTextureMap(const char* pszTextureMap)
{
	if( m_pTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}
	if( pszTextureMap )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(pszTextureMap, m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pTexture, A3DTF_MIPLEVEL, 1) )
			return false;

		m_strTextureMap = pszTextureMap;
	}
	return true;
}

