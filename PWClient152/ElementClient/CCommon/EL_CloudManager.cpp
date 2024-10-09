// CloudManager.cpp: implementation of the CCloudManager class.
//
//////////////////////////////////////////////////////////////////////

#include "EL_CloudManager.h"
#include "AFileImage.h"
#include "AIniFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const int SCENEWIDTH = 1024;				//width of one scene,1024

CELCloudManager::CELCloudManager()        
: m_nScenes(88)
, m_fRenderRange(800.0f)
, m_nStreamSprites(250)
{
	m_nSceneRow = 11;
	m_nSceneCol = 8;

	m_nStreamVertices = m_nStreamSprites * 4;

	m_vOldCameraPos.Set(1000000.0f,1000000.0f,1000000.0f);

	m_pTexture = NULL;
	m_pStream = NULL;
	m_pClouds = NULL;
	m_pA3DDevice = NULL;
	m_vSceneCenterPos = NULL;
}

CELCloudManager::~CELCloudManager()
{
}

void CELCloudManager::Tick(int nDeltaTime,const A3DVECTOR3& vSunPos)
{
	CELCloud::m_vSunPos = vSunPos;	

	int idxScene;
	for(UINT i=0;i<m_vSceneRender.size();i++)
	{
		idxScene = m_vSceneRender[i];
		if( idxScene >= 0 && idxScene < m_nScenes && m_pClouds[idxScene])
		{
			m_pClouds[idxScene]->Tick(nDeltaTime);
		}
	}	
}

void CELCloudManager::Render(A3DViewport* pViewport)
{
	if( pViewport == NULL)
		return;

	CalculateSceneRendered(pViewport);

	int idxScene;
	for(UINT i=0;i<m_vSceneRender.size();i++)
	{
		idxScene = m_vSceneRender[i];
		if( idxScene >= 0 && idxScene < m_nScenes && m_pClouds[idxScene])
		{
			m_pClouds[idxScene]->Render(pViewport);
		}
	}	

	m_pTexture->Appear();
	m_pA3DDevice->SetTextureAddress(0,A3DTADDR_CLAMP,A3DTADDR_CLAMP);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetLightingEnable(false);	

	m_pStream->Appear();	
	ReFillStream(pViewport);	
	//--------------------------------------------------
	//restore
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetTextureAddress(0,A3DTADDR_WRAP,A3DTADDR_WRAP);
	m_pTexture->Disappear();
}

bool CELCloudManager::Init(A3DDevice* pA3DDevice,const char* strCloudFilename)
{ 
	m_pA3DDevice = pA3DDevice;

	int nHalfWidth = SCENEWIDTH >> 1;
	
	//load texture
	m_pTexture = new A3DTexture();
	if( !m_pTexture->LoadFromFile(pA3DDevice,"textures\\cumulus01.tga",1024,1024,A3DFMT_A8R8G8B8))
		return false;	

	//initial sprite stream
	m_pStream = new A3DStream;
	if( !m_pStream->Init(pA3DDevice,sizeof(A3DTLVERTEX),A3DFVF_A3DTLVERTEX,m_nStreamVertices,m_nStreamSprites*6,A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC) )
		return false;
	
	WORD* pIndex;
	if( !m_pStream->LockIndexBuffer(0,6*m_nStreamSprites*sizeof(WORD),(BYTE**)&pIndex,0) )
		return false;
	for(WORD m=0;m<m_nStreamSprites;m++)
	{
		pIndex[m*6] = WORD(m*4);
		pIndex[m*6+1] = WORD(m*4 + 1);
		pIndex[m*6+2] = WORD(m*4 + 2);
		pIndex[m*6+3] = WORD(m*4 + 1);
		pIndex[m*6+4] = WORD(m*4 + 3);
		pIndex[m*6+5] = WORD(m*4 + 2);
	}
	m_pStream->UnlockIndexBuffer();
	
	if( !LoadCloudFiles(strCloudFilename) )
		return false;

	int nTopLeft_x = -m_nSceneCol * nHalfWidth + nHalfWidth;
	int nTopLeft_z =  m_nSceneRow * nHalfWidth - nHalfWidth;
	m_vSceneCenterPos = new A3DVECTOR3[m_nScenes];
	for(int i=0;i<m_nScenes;i++)
	{
		m_vSceneCenterPos[i].x = float(nTopLeft_x + (i % m_nSceneCol)*SCENEWIDTH);
		m_vSceneCenterPos[i].z = float(nTopLeft_z - (i / m_nSceneCol)*SCENEWIDTH);
		m_vSceneCenterPos[i].y = 200.0f;
	}
	
	return true;
}

void CELCloudManager::CalculateSceneIndex(const A3DVECTOR3& cameraPos,int &idx_x, int &idx_z)
{
	idx_x = idx_z = -1;
	int nHalfWidth = SCENEWIDTH >> 1;
	for(int i=0;i<m_nSceneCol;i++)
	{
		if( cameraPos.x > (m_vSceneCenterPos[i].x - nHalfWidth))
			idx_x = i;
	}

	for(int j=0;j<m_nSceneRow;j++)
	{
		if( cameraPos.z < (m_vSceneCenterPos[j*m_nSceneCol].z + nHalfWidth))
			idx_z = j;
	}
}

void CELCloudManager::Release()
{
	A3DRELEASE(m_pTexture);
	A3DRELEASE(m_pStream);

	if( m_vSceneCenterPos )
		delete[] m_vSceneCenterPos;

	if( m_pClouds )
	{
		for(int i=0;i<m_nScenes;i++)
		{
			if( m_pClouds[i] )
			{
				m_pClouds[i]->Release();
				delete m_pClouds[i];
			}
		}
		delete[] m_pClouds;
	}

	
}
void CELCloudManager::CalculateSceneRendered(A3DViewport* pViewport)
{
	//which cloud to render
	A3DVECTOR3 cameraPos = pViewport->GetCamera()->GetPos();
	A3DVECTOR3 vDist = cameraPos - m_vOldCameraPos;
	float fDist = vDist.SquaredMagnitude();

	//if move dist < 400 , don't recalculate
	if( fDist < 400 )
		return;
	m_vOldCameraPos = cameraPos;
	//scene index
	int idxScene_x,idxScene_z;
	CalculateSceneIndex(cameraPos,idxScene_x,idxScene_z);
	//local camera pos
	if( idxScene_x < 0 || idxScene_z < 0 )
		return; 

	//find scenes in view field: topleft and buttomright
	A3DVECTOR3 vTopLeft = cameraPos + A3DVECTOR3(-m_fRenderRange,0,m_fRenderRange);
	A3DVECTOR3 vBottomRight = cameraPos + A3DVECTOR3(m_fRenderRange,0,-m_fRenderRange);
	int idxTopLeft_x,idxTopLeft_z;
	int idxBottomRight_x,idxBottomRight_z;

	CalculateSceneIndex(vTopLeft,idxTopLeft_x,idxTopLeft_z);
	CalculateSceneIndex(vBottomRight,idxBottomRight_x,idxBottomRight_z);

	if( idxTopLeft_x < 0 )
		idxTopLeft_x = 0;
	if( idxTopLeft_z < 0 )
		idxTopLeft_z = 0;
	if( idxBottomRight_x >= m_nSceneCol )
		idxBottomRight_x = m_nSceneCol -1;
	if( idxBottomRight_z >= m_nSceneRow )
		idxBottomRight_z = m_nSceneRow -1;

	//clear first
	m_vSceneRender.clear();
	int idxScene;
	int i(0);
	for(i=idxTopLeft_x;i<=idxBottomRight_x;i++)
	{
		for(int j=idxTopLeft_z;j<=idxBottomRight_z;j++)
		{
			idxScene = i + j*m_nSceneCol;
			m_vSceneRender.push_back(idxScene);
		}
	}
	
	//render seq
	int idxLocalScene = idxScene_x + idxScene_z * m_nSceneCol;
	int num = m_vSceneRender.size();
	for(i=0;i<num;i++)
	{
		if( m_vSceneRender[i] == idxLocalScene )
		{
			int temp = m_vSceneRender[i];
			m_vSceneRender[i] = m_vSceneRender[num - 1];
			m_vSceneRender[num-1] = temp;
		}
	}
}

void CELCloudManager::SetCloudLevel(int nLevel)
{
	if( nLevel > 10 )
		nLevel = 10;
	else if ( nLevel < 1 )
		nLevel = 1;

	CELCloud::m_nCloudLevel = nLevel;
	//change sprite size
 	//CELCloudSprite::m_nSpriteSize = 110 - 25*(CELCloud::m_nCloudLevel -1 ) / 9;
	
	for(int i=0;i<m_nScenes;i++)
	{
		if( m_pClouds[i] )
			m_pClouds[i]->ChangeCloudLevel();
	}
}

void CELCloudManager::SetRenderRange(float fRenderRange)
{
	m_fRenderRange = fRenderRange;
	CELCloud::m_fRenderValve = fRenderRange*fRenderRange;
}

bool CELCloudManager::LoadCloudFiles(const char *strFilename)
{	
	AFileImage file;
	if( !file.Open(strFilename,AFILE_BINARY|AFILE_OPENEXIST))
		return false;

	DWORD nRead = 0;
	file.Read(&m_nSceneRow,sizeof(DWORD),&nRead);
	file.Read(&m_nSceneCol,sizeof(DWORD),&nRead);
	m_nScenes = m_nSceneRow * m_nSceneCol;
	m_pClouds = new CELCloud*[m_nScenes];
	int i(0);
	for(i=0;i<m_nScenes;i++)
	{	
		m_pClouds[i] = NULL;
	}

	int nFiles = 0;
	file.Read(&nFiles,sizeof(int),&nRead);

	for( i=0;i<nFiles;i++)
	{
		int idxScene = 0;
		file.Read(&idxScene,sizeof(DWORD),&nRead);
		DWORD nLength = 0;
		file.Read(&nLength,sizeof(DWORD),&nRead);
		
		CELCloud* pCloud = NULL;
		if( nLength > 0 )
		{
			BYTE* pBuffer = new BYTE[nLength];
			file.Read(pBuffer,nLength,&nRead);

			pCloud = new CELCloud;
			pCloud->Init(m_pA3DDevice,m_pTexture);
			pCloud->LoadCloud(pBuffer);
			delete[] pBuffer;
		}
		
		if( idxScene < m_nScenes && idxScene >= 0)
		{
			m_pClouds[idxScene] = pCloud;			
		}
		else if (pCloud != NULL)
			delete pCloud;
	}
	
	file.Close();
	return true;
}

void CELCloudManager::ReFillStream(A3DViewport* pViewport)
{
	A3DTLVERTEX* pVertex = NULL;
	if( !m_pStream->LockVertexBuffer(0,m_nStreamVertices*sizeof(A3DTLVERTEX),(BYTE**)&pVertex,D3DLOCK_DISCARD) )
		return;
	CELCloudSprite::m_pVertexBuffer = pVertex;

	//render
	int idxScene,nVertices,idxStreamPos=0;
	int idxSprite,nSprites,nRenderSprites = 0;
	int nFirstparts,nRemainparts;
	for(UINT i=0;i<m_vSceneRender.size();i++)
	{
		idxScene = m_vSceneRender[i];
		if( idxScene >= 0 && idxScene < m_nScenes && m_pClouds[idxScene])
		{
			//get sprites number need to be render
			idxSprite = 0;
			nVertices = 4 * m_pClouds[idxScene]->GetRenderSpritesNum();

			nRemainparts = nVertices;
			while( nRemainparts > m_nStreamVertices - idxStreamPos )
			{
				//fill first parts
				nFirstparts = m_nStreamVertices - idxStreamPos;
				nSprites = nFirstparts >> 2;
				
				m_pClouds[idxScene]->ReFillStream(pViewport,idxStreamPos,idxSprite,nSprites);
				
				//render stream
				m_pStream->UnlockVertexBuffer();
				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST,0,m_nStreamVertices,0,2*m_nStreamSprites);
				if( !m_pStream->LockVertexBuffer(0,m_nStreamVertices*sizeof(A3DTLVERTEX),(BYTE**)&pVertex,D3DLOCK_DISCARD) )
					return;
				CELCloudSprite::m_pVertexBuffer = pVertex;
				
				idxStreamPos = 0;
				nRemainparts -= nFirstparts;
				idxSprite += nSprites;
				nRenderSprites = 0;
			}
			
			nSprites = nRemainparts >> 2;
			nRenderSprites += nSprites;
			m_pClouds[idxScene]->ReFillStream(pViewport,idxStreamPos,idxSprite,nSprites);
			idxStreamPos += nRemainparts;
			
		}
	}

	m_pStream->UnlockVertexBuffer();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST,0,m_nStreamVertices,0,2*nRenderSprites);				
}

void CELCloudManager::SetSpriteColor(const A3DCOLORVALUE &clrAmb, const A3DCOLORVALUE &clrSun)
{
	CELCloudSprite::m_clrAmb = clrAmb;
	CELCloudSprite::m_clrSun = clrSun;
}
