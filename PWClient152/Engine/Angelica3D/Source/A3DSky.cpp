#include "A3DSky.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DObject.h"
#include "A3DStream.h"
#include "A3DFrame.h"
#include "A3DCameraBase.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DSky::A3DSky() :
m_WorldMatrix(A3DMATRIX4::IDENTITY)
{
	m_pA3DDevice		= NULL;
	m_pCamera			= NULL;
	m_pTextureTrans		= NULL;

	m_pVBSurround		= NULL;
	m_pIndicesSurround	= NULL;
	m_pVBFlat			= NULL;
	m_pIndicesFlat		= NULL;
	m_pVBCap			= NULL;
	m_pIndicesCap		= NULL;

	m_pStreamSurround	= NULL;
	m_pStreamFlat		= NULL;
	m_pStreamCap		= NULL;

	m_pTextureSurroundF = NULL;
	m_pTextureSurroundB = NULL;
	m_pTextureFlat		= NULL;
	m_pTextureCap		= NULL;

	m_colorSkyBottom	= 0xffffffff;
}

A3DSky::~A3DSky()
{
}

bool A3DSky::Init(A3DDevice* pDevice, A3DCameraBase* pCamera, const char* szCap, 
				  const char* szSurroundF, const char* szSurroundB, const char* szFlat)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;
	
	bool bval;

	//Creation Param;
	m_nLevelNum		= 4;
	m_nConeNum		= 5;
	m_fRadius		= 800.0f;

	m_pA3DDevice = pDevice;
	m_pCamera = pCamera;

	strncpy(m_szCap, szCap, MAX_PATH);
	strncpy(m_szSurroundF, szSurroundF, MAX_PATH);
	strncpy(m_szSurroundB, szSurroundB, MAX_PATH);
	strncpy(m_szFlat, szFlat, MAX_PATH);

	//First Create Surround Cloud;
	m_nVertCountS = m_nLevelNum * m_nConeNum * 2;
	m_nIndexCountS = (m_nLevelNum - 1) * 6 * (m_nConeNum - 1) * 2;

	m_pVBSurround = (A3DLVERTEX *) malloc(m_nVertCountS * sizeof(A3DLVERTEX));
	if( NULL == m_pVBSurround )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}
	
	m_pIndicesSurround = (WORD *) malloc(m_nIndexCountS * sizeof(WORD));
	if( NULL == m_pIndicesSurround )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}

	WORD i, j;
	FLOAT vDelta;
	FLOAT vDegDelta;

	vDelta = 3.14159f * 2.0f / 4.0f / (m_nConeNum - 1);
	vDegDelta = 60.0f / 180.0f * 3.14159f / (m_nLevelNum - 1);
	for( j=0; j<m_nLevelNum; j++ )
	{
		FLOAT vRadius = m_fRadius * (FLOAT)(cos((m_nLevelNum - 1 - j) * vDegDelta));
		FLOAT vHeight = m_fRadius * (FLOAT)(sin((m_nLevelNum - 1 - j) * vDegDelta));

		//Left Quadro Sphere;
		for( i=0; i<m_nConeNum; i++ )
		{
			int nVertNum = j * m_nConeNum + i;
			int nCellNum = j * (m_nConeNum - 1) + i;
			
			m_pVBSurround[nVertNum].x = (FLOAT)(vRadius * sin(vDelta * i));
			m_pVBSurround[nVertNum].y = vHeight;
			m_pVBSurround[nVertNum].z = (FLOAT)(vRadius * cos(vDelta * i));
			m_pVBSurround[nVertNum].diffuse = A3DCOLORRGBA(255, 255, 255, j == 0 ? 0 : 255);
			m_pVBSurround[nVertNum].specular = A3DCOLORRGBA(0, 0, 0, 255);
			m_pVBSurround[nVertNum].tu = 1.0f * i / (m_nConeNum - 1); 
			m_pVBSurround[nVertNum].tv = 0.5f * j / (m_nLevelNum - 1);
			
			if( i != m_nConeNum - 1 && j != m_nLevelNum - 1)
			{
				m_pIndicesSurround[nCellNum * 6] = nVertNum;
				m_pIndicesSurround[nCellNum * 6 + 1] = nVertNum + 1;
				m_pIndicesSurround[nCellNum * 6 + 2] = nVertNum + m_nConeNum;
				m_pIndicesSurround[nCellNum * 6 + 3] = nVertNum + 1;
				m_pIndicesSurround[nCellNum * 6 + 4] = nVertNum + m_nConeNum + 1;
				m_pIndicesSurround[nCellNum * 6 + 5] = nVertNum + m_nConeNum;
			}
		}
		//Right Quadro Sphere;
		for( i=0; i<m_nConeNum; i++ )
		{
			int nVertNum = m_nLevelNum * m_nConeNum + j * m_nConeNum + i;
			int nCellNum = (m_nLevelNum - 1) * (m_nConeNum - 1) + j * (m_nConeNum - 1) + i;
			
			m_pVBSurround[nVertNum].x = (FLOAT)(vRadius * sin(vDelta * (i + m_nConeNum - 1)));
			m_pVBSurround[nVertNum].y = vHeight;
			m_pVBSurround[nVertNum].z = (FLOAT)(vRadius * cos(vDelta * (i + m_nConeNum - 1)));
			m_pVBSurround[nVertNum].diffuse = A3DCOLORRGBA(255, 255, 255, j == 0 ? 0 : 255);
			m_pVBSurround[nVertNum].specular = A3DCOLORRGBA(0, 0, 0, 255);
			//Texture is upside down;
			m_pVBSurround[nVertNum].tu = 1.0f * i / (m_nConeNum - 1); 
			m_pVBSurround[nVertNum].tv = 1.0f - 0.5f * j / (m_nLevelNum - 1);
			
			if( i == m_nConeNum - 1 )
			{
				m_pVBSurround[nVertNum].x = m_pVBSurround[j * m_nConeNum].x;
				m_pVBSurround[nVertNum].z = -m_pVBSurround[j * m_nConeNum].z;
			}
			if( i != m_nConeNum - 1 && j != m_nLevelNum - 1)
			{
				m_pIndicesSurround[nCellNum * 6] = nVertNum;
				m_pIndicesSurround[nCellNum * 6 + 1] = nVertNum + 1;
				m_pIndicesSurround[nCellNum * 6 + 2] = nVertNum + m_nConeNum;
				m_pIndicesSurround[nCellNum * 6 + 3] = nVertNum + 1;
				m_pIndicesSurround[nCellNum * 6 + 4] = nVertNum + m_nConeNum + 1;
				m_pIndicesSurround[nCellNum * 6 + 5] = nVertNum + m_nConeNum;
			}
		}
	}

	m_pStreamSurround = new A3DStream();
	if( NULL == m_pStreamSurround )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}

	bval = m_pStreamSurround->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCountS, 
		m_nIndexCountS, A3DSTRM_STATIC, A3DSTRM_STATIC);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Surround Init Fail!");
		return false;
	}
	bval = m_pStreamSurround->SetVerts((LPBYTE)m_pVBSurround, m_nVertCountS);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Surround Set Verts Fail!");
		return false;
	}
	bval = m_pStreamSurround->SetIndices(m_pIndicesSurround, m_nIndexCountS);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Surround Set Indices Fail!");
		return false;
	}

	//Build Flat Cloud Here;
	m_nVertCountF = 20;
	m_nIndexCountF = 90;

	m_pVBFlat = (A3DLVERTEX *) malloc(m_nVertCountF * sizeof(A3DLVERTEX));
	if( NULL == m_pVBFlat )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}

	m_pIndicesFlat = (WORD *) malloc(m_nIndexCountF * sizeof(WORD));
	if( NULL == m_pIndicesFlat )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}


	FLOAT	vGridSize = m_fRadius * 0.3f;
	FLOAT	vCloudHeight = m_fRadius * 0.3f;
	static WORD EdgeIndex[] = {
		0, 16, 1, 1, 16, 2, 2, 16, 3,
		3, 17, 7, 7, 17, 11, 11, 17, 15,
		14, 15, 18, 13, 14, 18, 12, 13, 18,
		0, 4, 19, 4, 8, 19, 8, 12, 19};

	for(i=0; i<16; i++)
	{
		m_pVBFlat[i].x = ((i % 4) - 1.5f) * vGridSize;
		m_pVBFlat[i].y = vCloudHeight;
		m_pVBFlat[i].z = ((i / 4) - 1.5f) * vGridSize;
		m_pVBFlat[i].diffuse = A3DCOLORRGBA(255, 255, 255, 255);
		m_pVBFlat[i].specular = A3DCOLORRGBA(0, 0, 0, 255);
		m_pVBFlat[i].tu = (i % 4) * 0.8f;
		m_pVBFlat[i].tv = (i / 4) * 0.8f;
	}

	m_pVBFlat[0].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[3].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[12].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[15].diffuse = A3DCOLORRGBA(255, 255, 255, 0);

	m_pVBFlat[16].x = 0.0f;
	m_pVBFlat[16].y = vCloudHeight;
	m_pVBFlat[16].z = -2.0f * vGridSize;
	m_pVBFlat[16].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[16].specular = A3DCOLORRGBA(0, 0, 0, 255);
	m_pVBFlat[16].tu = 1.5f * 0.8f;
	m_pVBFlat[16].tv = -0.5f * 0.8f;

	m_pVBFlat[17].x = 2.0f * vGridSize;
	m_pVBFlat[17].y = vCloudHeight;
	m_pVBFlat[17].z = 0.0f;
	m_pVBFlat[17].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[17].specular = A3DCOLORRGBA(0, 0, 0, 255);
	m_pVBFlat[17].tu = 3.5f * 0.8f;
	m_pVBFlat[17].tv = 1.5f * 0.8f;

	m_pVBFlat[18].x = 0.0f;
	m_pVBFlat[18].y = vCloudHeight;
	m_pVBFlat[18].z = 2.0f * vGridSize;
	m_pVBFlat[18].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[18].specular = A3DCOLORRGBA(0, 0, 0, 255);
	m_pVBFlat[18].tu = 1.5f * 0.8f;
	m_pVBFlat[18].tv = 3.5f * 0.8f;

	m_pVBFlat[19].x = -2.0f * vGridSize;
	m_pVBFlat[19].y = vCloudHeight;
	m_pVBFlat[19].z = 0.0f;
	m_pVBFlat[19].diffuse = A3DCOLORRGBA(255, 255, 255, 0);
	m_pVBFlat[19].specular = A3DCOLORRGBA(0, 0, 0, 255);
	m_pVBFlat[19].tu = -0.5f * 0.8f;
	m_pVBFlat[19].tv = 1.5f * 0.8f;

#define PULL_DOWN 0.7f
	m_pVBFlat[0].y -= vCloudHeight * 0.76f * PULL_DOWN;
	m_pVBFlat[1].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[2].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[3].y -= vCloudHeight * 0.76f * PULL_DOWN;
	m_pVBFlat[4].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[7].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[8].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[11].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[12].y -= vCloudHeight * 0.76f * PULL_DOWN;
	m_pVBFlat[13].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[14].y -= vCloudHeight * 0.6f * PULL_DOWN;
	m_pVBFlat[15].y -= vCloudHeight * 0.76f * PULL_DOWN;
	m_pVBFlat[16].y -= vCloudHeight * 0.75f * PULL_DOWN;
	m_pVBFlat[17].y -= vCloudHeight * 0.75f * PULL_DOWN;
	m_pVBFlat[18].y -= vCloudHeight * 0.75f * PULL_DOWN;
	m_pVBFlat[19].y -= vCloudHeight * 0.75f * PULL_DOWN;
  
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			m_pIndicesFlat[(i * 3 + j) * 6] = (i * 4 + j);
			m_pIndicesFlat[(i * 3 + j) * 6 + 1] = (i * 4 + j + 1);
			m_pIndicesFlat[(i * 3 + j) * 6 + 2] = (i * 4 + j + 4);

			m_pIndicesFlat[(i * 3 + j) * 6 + 3] = (i * 4 + j + 1);
			m_pIndicesFlat[(i * 3 + j) * 6 + 4] = (i * 4 + j + 5);
			m_pIndicesFlat[(i * 3 + j) * 6 + 5] = (i * 4 + j + 4);
		}
	}
	for(i=54; i<90; i++)
	{
		m_pIndicesFlat[i] = EdgeIndex[i - 54];
	}

	//Now create the streams here;
	m_pStreamFlat = new A3DStream();
	if( NULL == m_pStreamFlat )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}

	bval = m_pStreamFlat->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCountF, m_nIndexCountF,
		A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Flat Init Fail!");
		return false;
	}
	bval = m_pStreamFlat->SetVerts((LPBYTE)m_pVBFlat, m_nVertCountF);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Flat Set Verts Fail!");
		return false;
	}
	bval = m_pStreamFlat->SetIndices(m_pIndicesFlat, m_nIndexCountF);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Flat Set Indices Fail!");
		return false;
	}

	m_pStreamFlat->SetVertexRef((LPBYTE)m_pVBFlat);
	m_pStreamFlat->SetIndexRef(m_pIndicesFlat);

	//Build Cap Vertex Buffer Here;
	m_nVertCountC = 4;
	m_nIndexCountC = 6;
	m_pVBCap = (A3DLVERTEX *) malloc(m_nVertCountC * sizeof(A3DLVERTEX));
	if( NULL == m_pVBCap )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}
	m_pIndicesCap = (WORD *) malloc(m_nIndexCountC * sizeof(WORD));
	if( NULL == m_pIndicesCap )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}
	for(i=0; i<4; i++)
	{

		m_pVBCap[i].x = ((i % 2) - 0.5f) * m_fRadius * 1.5f;
		m_pVBCap[i].y = m_fRadius * 0.55f;
		m_pVBCap[i].z = ((i / 2) - 0.5f) * m_fRadius * 1.5f;
		m_pVBCap[i].diffuse = A3DCOLORRGBA(255, 255, 255, 255);
		m_pVBCap[i].specular = A3DCOLORRGBA(0, 0, 0, 255);
		m_pVBCap[i].tu = 1.0f * (i % 2);
		m_pVBCap[i].tv = 1.0f * (i / 2);
	}
	m_pIndicesCap[0] = 0; m_pIndicesCap[1] = 1; m_pIndicesCap[2] = 2;
	m_pIndicesCap[3] = 2; m_pIndicesCap[4] = 1; m_pIndicesCap[5] = 3;
	m_pStreamCap = new A3DStream();
	if( NULL == m_pStreamCap )
	{
		g_A3DErrLog.Log("A3DSky::Init Not enough memory!");
		return false;
	}
	bval = m_pStreamCap->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCountC, m_nIndexCountC, 
		A3DSTRM_STATIC, A3DSTRM_STATIC);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Cap Init Fail!");
		return false;
	}
	bval = m_pStreamCap->SetVerts((LPBYTE)m_pVBCap, m_nVertCountC);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Cap Set Verts Fail!");
		return false;
	}
	bval = m_pStreamCap->SetIndices(m_pIndicesCap, m_nIndexCountC);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSky::Init Stream Flat Set Indices Fail!");
		return false;
	}

	//Load the textures;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szCap, "Textures\\Sky", &m_pTextureCap, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szSurroundF, "Textures\\Sky", &m_pTextureSurroundF, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szSurroundB, "Textures\\Sky", &m_pTextureSurroundB, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szFlat, "Textures\\Sky", &m_pTextureFlat, A3DTF_MIPLEVEL, 1) )
		return false;

	m_WorldMatrix.Identity();
	return true;
}

bool A3DSky::Release()
{
	if( m_pTextureCap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
		m_pTextureCap = NULL;
	}
	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
		m_pTextureSurroundF = NULL;
	}
	if( m_pTextureSurroundB )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
		m_pTextureSurroundB = NULL;
	}
	if( m_pTextureFlat )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureFlat);
		m_pTextureFlat = NULL;
	}
	if( m_pStreamSurround )
	{
		m_pStreamSurround->Release();
		delete m_pStreamSurround;
		m_pStreamSurround = NULL;
	}
	if( m_pStreamFlat )
	{
		m_pStreamFlat->Release();
		delete m_pStreamFlat;
		m_pStreamFlat = NULL;
	}
	if( m_pStreamCap )
	{
		m_pStreamCap->Release();
		delete m_pStreamCap;
		m_pStreamCap = NULL;
	}
	if( m_pVBCap )
	{
		free(m_pVBCap);
		m_pVBCap = NULL;
	}
	if( m_pIndicesCap )
	{
		free(m_pIndicesCap);
		m_pIndicesCap = NULL;
	}
	if( m_pVBSurround )
	{
		free(m_pVBSurround);
		m_pVBSurround = NULL;
	}
	if( m_pIndicesSurround )
	{
		free(m_pIndicesSurround);
		m_pIndicesSurround = NULL;
	}
	if( m_pVBFlat )
	{
		free(m_pVBFlat);
		m_pVBFlat = NULL;
	}
	if( m_pIndicesFlat )
	{
		free(m_pIndicesFlat);
		m_pIndicesFlat = NULL;
	}
	return true;
}

bool A3DSky::Render()
{	
	if( !m_pA3DDevice ) return true;

	//Update World Matrix First;
	if( m_pCamera )
	{
		A3DVECTOR3 vecPos;
		if( m_pCamera->IsMirrored() )
		{
			vecPos = m_pCamera->GetMirrorCamera()->GetPos();
			A3DMATRIX4 matMirror = m_pCamera->GetMirrorMatrix();
			vecPos = vecPos * InverseTM(matMirror);
		}
		else
		{
			vecPos = m_pCamera->GetPos();
		}
		m_WorldMatrix = a3d_Translate(vecPos.x, vecPos.y, vecPos.z);
	}
	
	m_pA3DDevice->SetLightingEnable(false);
	bool bFogEnable = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	//First of all show top cap of the surround cloud;
	m_pA3DDevice->SetWorldMatrix(m_WorldMatrix);
	m_pTextureCap->Appear(0);
	m_pStreamCap->Appear();
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountC, 0, m_nIndexCountC / 3) )
	{
		g_A3DErrLog.Log("A3DSky::Render DrawIndexPrimitive Fail!");
		return false;
	}
	m_pTextureCap->Disappear(0);

	//First show the surround cloud front part;
	m_pA3DDevice->SetWorldMatrix(m_WorldMatrix);
	m_pTextureSurroundF->Appear(0);
	m_pStreamSurround->Appear();
	
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountS, 0, m_nIndexCountS / 3) )
	{
		g_A3DErrLog.Log("A3DSky::Render DrawIndexPrimitive Fail!");
		return false;
	}
	m_pTextureSurroundF->Disappear(0);

	//Second show the surround cloud back part;
	A3DMATRIX4 matRotate180 = a3d_IdentityMatrix();
	matRotate180._11 = -1.0f;
	matRotate180._33 = -1.0f;
	m_pA3DDevice->SetWorldMatrix(matRotate180 * m_WorldMatrix);
	m_pTextureSurroundB->Appear(0);
	m_pStreamSurround->Appear();
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountS, 0, m_nIndexCountS / 3) )
	{
		g_A3DErrLog.Log("A3DSky::Render DrawIndexPrimitive Fail!");
		return false;
	}
	m_pTextureSurroundB->Disappear(0);

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	if( g_pA3DConfig->GetFlagHQSky() )
	{
		//Then show the flat cloud
		m_pA3DDevice->SetWorldMatrix(m_WorldMatrix);
		m_pTextureFlat->Appear(0);
		m_pStreamFlat->Appear();
		
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountF, 0, m_nIndexCountF / 3) )
		{
			g_A3DErrLog.Log("A3DSky::Render DrawIndexPrimitive Fail!");
			return false;
		}
		m_pTextureSurroundF->Disappear(0);
	}
	
	m_pA3DDevice->SetZWriteEnable(TRUE);
	m_pA3DDevice->SetZTestEnable(TRUE);
	m_pA3DDevice->SetLightingEnable(TRUE);
	m_pA3DDevice->SetFogEnable(bFogEnable);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A3DSky::TickAnimation()
{
	if( !m_pA3DDevice ) return true;

	//Update the flat cloud's texture uv coordinates;
	for(int i=0; i<m_nVertCountF; i++)
	{
		m_pVBFlat[i].tu += 0.0002f;
		m_pVBFlat[i].tv += 0.0002f;
	}
	if( !m_pStreamFlat->SetVerts((LPBYTE)m_pVBFlat, m_nVertCountF) )
	{
		g_A3DErrLog.Log("A3DSky::Render Stream Flat Set Verts Fail!");
		return false;
	}

	return true;
}

bool A3DSky::SetCamera(A3DCameraBase * pCamera)
{
	if( !m_pA3DDevice ) return true;

	//If the new set camera is Null, then the old camera is retained;
	if( NULL == pCamera )
		return true;

	m_pCamera = pCamera;
	return true;
}

bool A3DSky::SetTextureSurroundF(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureSurroundF, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSky::SetTextureSurroundF(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	strncpy(m_szSurroundF, szTextureFile, MAX_PATH);
	return true;
}

bool A3DSky::SetTextureSurroundB(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSurroundB )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureSurroundB, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSky::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	strncpy(m_szSurroundB, szTextureFile, MAX_PATH);
	return true;
}

bool A3DSky::SetTextureFlat(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureFlat )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureFlat);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureFlat, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSky::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	strncpy(m_szFlat, szTextureFile, MAX_PATH);
	return true;
}

bool A3DSky::SetTextureCap(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureCap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureCap, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSky::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	strncpy(m_szCap, szTextureFile, MAX_PATH);
	return true;
}