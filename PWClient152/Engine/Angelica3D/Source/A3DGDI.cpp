#include "A3DGDI.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"
#include "A3DStream.h"
#include "A3DDevice.h"
#include "A3DTexture.h"

A3DGDI * g_pA3DGDI = NULL;

A3DGDI::A3DGDI()
{
	m_pA3DDevice	= NULL;
	m_pA3DStream	= NULL;
	m_pA3DTLStream	= NULL;
	m_pA3DTexture	= NULL;
}

A3DGDI::~A3DGDI()
{
}
				
bool A3DGDI::Init(A3DDevice * pA3DDevice, char * pszTextureMap)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;
	m_pA3DStream = new A3DStream();
	if (!m_pA3DStream)
	{
		g_A3DErrLog.Log("A3DGDI::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_LVERTEX, A3DGDI_MAX_FACES * 3, 
		A3DGDI_MAX_FACES * 3, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DGDI::Init() Init Stream Fail!");
		return false;
	}

	m_pA3DTLStream = new A3DStream();
	if (!m_pA3DTLStream)
	{
		g_A3DErrLog.Log("A3DGDI::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DTLStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, A3DGDI_MAX_FACES * 3, 
		A3DGDI_MAX_FACES * 3, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DGDI::Init() Init TL Stream Fail!");
		return false;
	}

	if (pszTextureMap)
	{
		if (!m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pszTextureMap, &m_pA3DTexture))
			return false;
	}

	return true;
}

bool A3DGDI::Release()
{
	if( m_pA3DStream )
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}
	if( m_pA3DTLStream )
	{
		m_pA3DTLStream->Release();
		delete m_pA3DTLStream;
		m_pA3DTLStream = NULL;
	}
	if( m_pA3DTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}
	return true;
}

bool A3DGDI::DrawTriangle(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, FLOAT vSize1, FLOAT vSize2, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	A3DVECTOR3	vecLeft;
	A3DMATRIX4  matTM;

	A3DVECTOR3  vecTrueUp;

	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	vecLeft = Normalize(CrossProduct(vecDir, vecUp));
	vecTrueUp = Normalize(CrossProduct(vecLeft, vecDir));

	matTM = a3d_TransformMatrix(vecDir, vecTrueUp, vecPos);

	A3DVECTOR3 vecVertex;

	//	  0
	//	  *
	//	 / \
	//	/	\
	//2*-----*1

	vecVertex = A3DVECTOR3(0.0f, 0.0f, vSize2/2.0f) * matTM;
	m_pVertexBuffer[0] = A3DLVERTEX(vecVertex, color, specular, 0.5f, 0.0f);

	vecVertex = A3DVECTOR3(vSize1/2.0f, 0.0f, -vSize2/2.0f) * matTM;
	m_pVertexBuffer[1] = A3DLVERTEX(vecVertex, color, specular, 1.0f, 1.0f);

	vecVertex = A3DVECTOR3(-vSize1/2.0f, 0.0f, -vSize2/2.0f) * matTM;
	m_pVertexBuffer[2] = A3DLVERTEX(vecVertex, color, specular, 0.0f, 1.0f);

	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1; m_pIndexBuffer[2] = 2;
	m_pIndexBuffer[3] = 0; m_pIndexBuffer[4] = 2; m_pIndexBuffer[5] = 1;

	m_pA3DStream->SetVertsDynamic((BYTE *)m_pVertexBuffer, 3);
	m_pA3DStream->SetIndicesDynamic(m_pIndexBuffer, 6);
	m_pA3DStream->Appear();
	
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 3, 0, 2) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::DrawRectangle(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, FLOAT vSize1, FLOAT vSize2, A3DCOLOR color, bool b2Sided)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	A3DVECTOR3	vecLeft;
	A3DMATRIX4  matTM;
	A3DVECTOR3  vecTrueUp;

	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	vecLeft = Normalize(CrossProduct(vecDir, vecUp));
	vecTrueUp = Normalize(CrossProduct(vecLeft, vecDir));

	matTM = a3d_TransformMatrix(vecDir, vecTrueUp, vecPos);

	A3DVECTOR3 vecVertex;

	//0-------1
	//|	  	/ |
	//|	  /	  |
	//|	/	  |
	//2-------3

	vecVertex = A3DVECTOR3(-vSize1/2.0f, 0.0f, vSize2/2.0f) * matTM;
	m_pVertexBuffer[0] = A3DLVERTEX(vecVertex, color, specular, 0.0f, 0.0f);

	vecVertex = A3DVECTOR3(vSize1/2.0f, 0.0f, vSize2/2.0f) * matTM;
	m_pVertexBuffer[1] = A3DLVERTEX(vecVertex, color, specular, 1.0f, 0.0f);

	vecVertex = A3DVECTOR3(-vSize1/2.0f, 0.0f, -vSize2/2.0f) * matTM;
	m_pVertexBuffer[2] = A3DLVERTEX(vecVertex, color, specular, 0.0f, 1.0f);

	vecVertex = A3DVECTOR3(vSize1/2.0f, 0.0f, -vSize2/2.0f) * matTM;
	m_pVertexBuffer[3] = A3DLVERTEX(vecVertex, color, specular, 1.0f, 1.0f);

	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1; m_pIndexBuffer[2] = 2;
	m_pIndexBuffer[3] = 2; m_pIndexBuffer[4] = 1; m_pIndexBuffer[5] = 3;
	m_pIndexBuffer[6] = 0; m_pIndexBuffer[7] = 2; m_pIndexBuffer[8] = 1;
	m_pIndexBuffer[9] = 2; m_pIndexBuffer[10] = 3; m_pIndexBuffer[11] = 1;

	m_pA3DStream->SetVertsDynamic((BYTE *)m_pVertexBuffer, 4);
	m_pA3DStream->SetIndicesDynamic(m_pIndexBuffer, 12);
	m_pA3DStream->Appear();
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	//m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if( b2Sided )
	{
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 4) )
		{
			g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
			return false;
		}
	}
	else
	{
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
		{
			g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
			return false;
		}
	}

	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	//m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::ChangeTexture(char * pszTextureMap)
{
	if( !m_pA3DDevice )	return true;

	if( m_pA3DTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}

	if( pszTextureMap )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pszTextureMap, &m_pA3DTexture) )
			return false;
	}	
	return true;
}

bool A3DGDI::Draw2DRectangle(const A3DRECT& rect, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	
	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR4 vecVertex;

	//0-------1
	//|	  	/ |
	//|	  /	  |
	//|	/	  |
	//2-------3

	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.top, 0.0f, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, color, specular, 0.0f, 0.0f);

	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.top, 0.0f, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, color, specular, 1.0f, 0.0f);

	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.bottom, 0.0f, 1.0f);
	m_pTLVertexBuffer[2] = A3DTLVERTEX(vecVertex, color, specular, 0.0f, 1.0f);

	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.bottom, 0.0f, 1.0f);
	m_pTLVertexBuffer[3] = A3DTLVERTEX(vecVertex, color, specular, 1.0f, 1.0f);

	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1; m_pIndexBuffer[2] = 2;
	m_pIndexBuffer[3] = 2; m_pIndexBuffer[4] = 1; m_pIndexBuffer[5] = 3;
	
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 4);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 6);
	m_pA3DTLStream->Appear();
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::Draw2DRectWithEdge(const A3DRECT& rect, A3DCOLOR colorEdge, A3DCOLOR colorRect)
{
	if( !m_pA3DDevice )	return true;

	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	A3DCOLOR	diffuse, specular;
	
	diffuse  = colorRect;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR4 vecVertex;

	//0-------1
	//|	  	/ |
	//|	  /	  |
	//|	/	  |
	//2-------3

	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.top, 0.0f, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.top, 0.0f, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.bottom, 0.0f, 1.0f);
	m_pTLVertexBuffer[2] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 1.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.bottom, 0.0f, 1.0f);
	m_pTLVertexBuffer[3] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 1.0f);
	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1; m_pIndexBuffer[2] = 2;
	m_pIndexBuffer[3] = 2; m_pIndexBuffer[4] = 1; m_pIndexBuffer[5] = 3;
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 4);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 6);
	m_pA3DTLStream->Appear();
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	
	diffuse  = colorEdge;
	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.top, 0.0f, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.top, 0.0f, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.bottom, 0.0f, 1.0f);
	m_pTLVertexBuffer[2] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 1.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.bottom, 0.0f, 1.0f);
	m_pTLVertexBuffer[3] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 1.0f);
	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1;
	m_pIndexBuffer[2] = 1; m_pIndexBuffer[3] = 2;
	m_pIndexBuffer[4] = 2; m_pIndexBuffer[5] = 3;
	m_pIndexBuffer[6] = 3; m_pIndexBuffer[7] = 0;
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 4);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 8);
	m_pA3DTLStream->Appear();
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, 4, 0, 4) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}

	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::Draw2DZRectWithEdge(const A3DRECT& rect, float z, A3DCOLOR colorEdge, A3DCOLOR colorRect)
{
	if( !m_pA3DDevice )	return true;

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	A3DCOLOR	diffuse, specular;
	
	diffuse  = colorRect;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR4 vecVertex;

	//0-------1
	//|	  	/ |
	//|	  /	  |
	//|	/	  |
	//2-------3

	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.top, z, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.top, z, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.bottom, z, 1.0f);
	m_pTLVertexBuffer[2] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 1.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.bottom, z, 1.0f);
	m_pTLVertexBuffer[3] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 1.0f);
	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1; m_pIndexBuffer[2] = 2;
	m_pIndexBuffer[3] = 2; m_pIndexBuffer[4] = 1; m_pIndexBuffer[5] = 3;
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 4);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 6);
	m_pA3DTLStream->Appear();
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	
	diffuse  = colorEdge;
	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.top, z, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.top, z, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 0.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.right, (FLOAT)rect.bottom, z, 1.0f);
	m_pTLVertexBuffer[2] = A3DTLVERTEX(vecVertex, diffuse, specular, 1.0f, 1.0f);
	vecVertex = A3DVECTOR4((FLOAT)rect.left, (FLOAT)rect.bottom, z, 1.0f);
	m_pTLVertexBuffer[3] = A3DTLVERTEX(vecVertex, diffuse, specular, 0.0f, 1.0f);
	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1;
	m_pIndexBuffer[2] = 1; m_pIndexBuffer[3] = 2;
	m_pIndexBuffer[4] = 2; m_pIndexBuffer[5] = 3;
	m_pIndexBuffer[6] = 3; m_pIndexBuffer[7] = 0;
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 4);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 8);
	m_pA3DTLStream->Appear();
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, 4, 0, 4) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}

	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	return true;
}

bool A3DGDI::Draw2DLine(const A3DPOINT2& ptStart, const A3DPOINT2& ptEnd, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	
	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR4 vecVertex;

	vecVertex = A3DVECTOR4((FLOAT)ptStart.x, (FLOAT)ptStart.y, 0.0f, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, color, specular, 0.0f, 0.0f);

	vecVertex = A3DVECTOR4((FLOAT)ptEnd.x, (FLOAT)ptEnd.y, 0.0f, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, color, specular, 1.0f, 0.0f);

	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1;
	
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 2);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 2);
	m_pA3DTLStream->Appear();
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, 2, 0, 1) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);

	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::Draw3DLine(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecEnd, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	
	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	m_pVertexBuffer[0] = A3DLVERTEX(vecStart, color, specular, 0.0f, 0.0f);
	m_pVertexBuffer[1] = A3DLVERTEX(vecEnd, color, specular, 0.0f, 1.0f);

	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1;
	
	m_pA3DStream->SetVertsDynamic((BYTE *)m_pVertexBuffer, 2);
	m_pA3DStream->SetIndicesDynamic(m_pIndexBuffer, 2);
	m_pA3DStream->Appear();
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetZWriteEnable(false);
	//m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, 2, 0, 1) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	//m_pA3DDevice->SetZTestEnable(true);
	return true;
}
 
bool A3DGDI::DrawBox(const A3DAABB& aabb, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DVECTOR3 vecDir, vecUp;
	
	// First horizontal planes;
	vecUp  = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	vecDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	if( !DrawRectangle(aabb.Center + vecUp * aabb.Extents.y, vecDir, vecUp, aabb.Extents.x * 2, aabb.Extents.z * 2, color, false) )
		return false;
	if( !DrawRectangle(aabb.Center - vecUp * aabb.Extents.y, vecDir, -vecUp, aabb.Extents.x * 2, aabb.Extents.z * 2, color, false) )
		return false;

	// Front and back planes;
	vecUp  = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	vecDir = A3DVECTOR3(0.0f, -1.0f, 0.0f);
	if( !DrawRectangle(aabb.Center + vecUp * aabb.Extents.z, vecDir, vecUp, aabb.Extents.x * 2, aabb.Extents.y * 2, color, false) )
		return false;
	if( !DrawRectangle(aabb.Center - vecUp * aabb.Extents.z, vecDir, -vecUp, aabb.Extents.x * 2, aabb.Extents.y * 2, color, false) )
		return false;

	// Left and right planes;
	vecUp  = A3DVECTOR3(1.0f, 0.0f, 0.0f);
	vecDir = A3DVECTOR3(0.0f, -1.0f, 0.0f);
	if( !DrawRectangle(aabb.Center + vecUp * aabb.Extents.x, vecDir, vecUp, aabb.Extents.z * 2, aabb.Extents.y * 2, color, false) )
		return false;
	if( !DrawRectangle(aabb.Center - vecUp * aabb.Extents.x, vecDir, -vecUp, aabb.Extents.z * 2, aabb.Extents.y * 2, color, false) )
		return false;

	return true;
}

bool A3DGDI::DrawBox(const A3DOBB& obb, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DVECTOR3 vecDir, vecUp;
	
	// First horizontal planes;
	vecUp  = obb.YAxis;
	vecDir = obb.ZAxis;
	if( !DrawRectangle(obb.Center + vecUp * obb.Extents.y, vecDir, vecUp, obb.Extents.x * 2, obb.Extents.z * 2, color, false) )
		return false;
	if( !DrawRectangle(obb.Center - vecUp * obb.Extents.y, vecDir, -vecUp, obb.Extents.x * 2, obb.Extents.z * 2, color, false) )
		return false;

	// Front and back planes;
	vecUp  = obb.ZAxis;
	vecDir = -obb.YAxis;
	if( !DrawRectangle(obb.Center + vecUp * obb.Extents.z, vecDir, vecUp, obb.Extents.x * 2, obb.Extents.y * 2, color, false) )
		return false;
	if( !DrawRectangle(obb.Center - vecUp * obb.Extents.z, vecDir, -vecUp, obb.Extents.x * 2, obb.Extents.y * 2, color, false) )
		return false;

	// Left and right planes;
	vecUp  = obb.XAxis;
	vecDir = -obb.YAxis;
	if( !DrawRectangle(obb.Center + vecUp * obb.Extents.x, vecDir, vecUp, obb.Extents.z * 2, obb.Extents.y * 2, color, false) )
		return false;
	if( !DrawRectangle(obb.Center - vecUp * obb.Extents.x, vecDir, -vecUp, obb.Extents.z * 2, obb.Extents.y * 2, color, false) )
		return false;
	return true;
}

bool A3DGDI::Draw2DPoint(const A3DPOINT2& ptPos, A3DCOLOR color, FLOAT vSize)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	
	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR4 vecVertex;

	vecVertex = A3DVECTOR4((FLOAT)ptPos.x, (FLOAT)ptPos.y, 0.0f, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, color, specular, 0.0f, 0.0f);

	m_pIndexBuffer[0] = 0;
	
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 1);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 1);
	m_pA3DTLStream->Appear();
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	m_pA3DDevice->SetPointSize(vSize);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if( !m_pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, 1) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}

	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::Draw3DPoint(const A3DVECTOR3& vecPos, A3DCOLOR color, FLOAT vSize)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	
	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	m_pVertexBuffer[0] = A3DLVERTEX(vecPos, color, specular, 0.0f, 0.0f);

	m_pIndexBuffer[0] = 0;
	
	m_pA3DStream->SetVertsDynamic((BYTE *)m_pVertexBuffer, 1);
	m_pA3DStream->SetIndicesDynamic(m_pIndexBuffer, 1);
	m_pA3DStream->Appear();
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	m_pA3DDevice->SetPointSize(vSize);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if( !m_pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, 1) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}

	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A3DGDI::Draw2DTriangle(const A3DPOINT2& v0, const A3DPOINT2& v1, const A3DPOINT2& v2, A3DCOLOR color)
{
	if( !m_pA3DDevice )	return true;

	A3DCOLOR	diffuse, specular;
	
	diffuse  = color;
	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR4 vecVertex;

	//0-------1
	//|	  	/
	//|	  /	
	//|	/
	//2

	vecVertex = A3DVECTOR4((FLOAT)v0.x, (FLOAT)v0.y, 0.0f, 1.0f);
	m_pTLVertexBuffer[0] = A3DTLVERTEX(vecVertex, color, specular, 0.0f, 0.0f);

	vecVertex = A3DVECTOR4((FLOAT)v1.x, (FLOAT)v1.y, 0.0f, 1.0f);
	m_pTLVertexBuffer[1] = A3DTLVERTEX(vecVertex, color, specular, 1.0f, 0.0f);

	vecVertex = A3DVECTOR4((FLOAT)v2.x, (FLOAT)v2.y, 0.0f, 1.0f);
	m_pTLVertexBuffer[2] = A3DTLVERTEX(vecVertex, color, specular, 0.0f, 1.0f);

	m_pIndexBuffer[0] = 0; m_pIndexBuffer[1] = 1; m_pIndexBuffer[2] = 2;
	m_pIndexBuffer[3] = 2; m_pIndexBuffer[4] = 1; m_pIndexBuffer[5] = 0;
	
	m_pA3DTLStream->SetVertsDynamic((BYTE *)m_pTLVertexBuffer, 3);
	m_pA3DTLStream->SetIndicesDynamic(m_pIndexBuffer, 6);
	m_pA3DTLStream->Appear();
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	if( m_pA3DTexture )
		m_pA3DTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 3, 0, 2) )
	{
		g_A3DErrLog.Log("A3DGDI::DrawTriangle() DrawIndexedPrimitive Fail!");
		return false;
	}
	if( m_pA3DTexture )
		m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}
