// ELCloudSceneRender.cpp: implementation of the CELCloudSceneRender class.
//
//////////////////////////////////////////////////////////////////////

#include "EL_CloudSceneRender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const int SCENEWIDTH = 1024;

CELCloudSceneRender::CELCloudSceneRender()
{
	m_nCloudLevel = 10;
	m_fRenderValve = 800*800;
	m_vSunPos.Set(0.0f,0.0f,0.0f);

	m_nStreamVertices = 1000;
	m_nStreamSprites = 250;

	m_bRenderLine = true;

	m_pStream = NULL;
	m_pStreamLine = NULL;
	m_pTexture = NULL;
	m_pA3DDevice = NULL;
	m_pGroups = NULL;
}

CELCloudSceneRender::~CELCloudSceneRender()
{

}

bool CELCloudSceneRender::Init(A3DDevice *pA3DDevice,const char* strTextureFile)
{
	m_pA3DDevice = pA3DDevice;

	//load texture
	m_pTexture = new A3DTexture();
	if( !m_pTexture->LoadFromFile(pA3DDevice,strTextureFile,1024,1024,A3DFMT_A8R8G8B8))
		return false;	

	//initial sprite stream
	m_pStream = new A3DStream;
	if( !m_pStream->Init(pA3DDevice,sizeof(A3DTLVERTEX),A3DFVF_A3DTLVERTEX,m_nStreamVertices,m_nStreamSprites*6,A3DSTRM_STATIC,A3DSTRM_STATIC) )
		return false;
	
	WORD* pIndex;
	m_pStream->LockIndexBuffer(0,6*m_nStreamSprites*sizeof(WORD),(BYTE**)&pIndex,0);
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
	
	return true;
}

void CELCloudSceneRender::Tick(int nDeltaTime, const A3DVECTOR3 &vSunPos)
{
	m_vSunPos = vSunPos;		
}

void CELCloudSceneRender::Render(A3DViewport *pViewport)
{
	if( pViewport == NULL || m_pGroups == NULL)
		return;

	CalculateRenderSeq(pViewport);
	CalculateCloudColor(pViewport);

	if( m_bRenderLine )
	{
		ReFillStream_Line();
		RenderStream_Line();
	}
	
	m_pTexture->Appear();
	m_pA3DDevice->SetTextureAlphaOP(0,A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaArgs(0,A3DTA_TEXTURE,A3DTA_DIFFUSE);
	m_pA3DDevice->SetTextureAddress(0,A3DTADDR_CLAMP,A3DTADDR_CLAMP);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetLightingEnable(false);	

	m_pStream->Appear();	
	ReFillStream(pViewport);	
	//--------------------------------------------------
	//restore
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetTextureAddress(0,A3DTADDR_WRAP,A3DTADDR_WRAP);
	m_pTexture->Disappear();
}

void CELCloudSceneRender::SetCloudInfo(CloudCreatorParam* pParam,abase::vector<CloudSprite>& vSprites,abase::vector<CloudGroup>& vGroups)
{
	if( vGroups.size() < 0 || vSprites.size() < 0 )
		return;

	m_pGroups = &vGroups;
	m_pmCloudCreator = pParam;

	//----------------------------------
	//clear group info and calculate sprite move dist
	float* vSpriteMove = new float[vSprites.size()];
	for(UINT m=0;m<vSprites.size();m++)
		vSpriteMove[m] = 0.0f;
 
	float* fMinDist = new float[m_pGroups->size()];
	float* fMaxDist = new float[m_pGroups->size()];
	for(m=0;m<m_pGroups->size();m++)
	{
		fMinDist[m] = 10000;
		fMaxDist[m] = -10000;
	}

	A3DVECTOR3 vTemp;
	int idxSprite = 0;
	for(m=0;m<m_pGroups->size();m++)
	{
		(*m_pGroups)[m].bRender = false;
		for(UINT j = 0;j< (*m_pGroups)[m].vSprites.size();j++)
		{
			idxSprite = (*m_pGroups)[m].vSprites[j];
			vTemp = vSprites[idxSprite].vCenterPos - (*m_pGroups)[m].vCenterPos;
			vSpriteMove[idxSprite] = vTemp.Magnitude();
			if( vSpriteMove[idxSprite] < fMinDist[m] )
				fMinDist[m] = vSpriteMove[idxSprite];
			if( vSpriteMove[idxSprite] > fMaxDist[m] )
				fMaxDist[m] = vSpriteMove[idxSprite];
		}
	}
	int idxGroup = 0;
	for(m =0; m < vSprites.size() ;m++)
	{
		float temp = vSpriteMove[m];
		idxGroup = vSprites[m].nGroup;
		vSpriteMove[m] = 1 - (vSpriteMove[m] - fMinDist[idxGroup]) / ( 1.3*fMaxDist[idxGroup] - fMinDist[idxGroup] );
			
		//test
		//char str[50];
		//sprintf(str,"vSpriteMove group = %d,dist = %f,ratio= %f \n",idxGroup,temp,vSpriteMove[m]);
		//OutputDebugString(str);
	}
		
	//------------------------------------------------------
	ReCalculateSpriteSize();
	//sprite info
	//clear
	for(UINT i=0;i<m_vSprites.size();i++)
	{
		if( m_vSprites[i] != NULL)
			delete m_vSprites[i];
	}
	m_vSprites.clear();
	m_vRenderSeq.clear();
	if(fMinDist) delete []fMinDist;
	if(fMaxDist) delete []fMaxDist;
	//add sprites
	DWORD vColor[4];
	UINT nSprites = vSprites.size();
	for(i=0;i<nSprites;i++)
	{
		CELCloudSprite* pSprite = new CELCloudSprite;
		pSprite->Init(m_pA3DDevice);
				
		for(int j=0;j<4;j++)
			vColor[j] = vSprites[i].vColor[j].ToRGBAColor();
		
		pSprite->SetSpriteInfo(vSprites[i].vCenterPos,vSprites[i].idxTexture,vSprites[i].nLevel,vColor);
		pSprite->SetGroup(vSprites[i].nGroup);
		pSprite->SetGroupCenterPos(vSprites[i].vGroupCenterPos);
		//pSprite->SetSpriteMotion(vSpriteMove[i]);

		m_vSprites.push_back(pSprite);
	}

	//initial line stream
	if( m_pStreamLine )
		A3DRELEASE(m_pStreamLine);
	if(vSpriteMove) delete []vSpriteMove;

	int nGroups = m_pGroups->size();
	int nVertices = nSprites + nGroups; 
	int nIndex = nSprites * 2;
	m_pStreamLine = new A3DStream;
	if( !m_pStreamLine->Init(m_pA3DDevice,sizeof(A3DLVERTEX),A3DFVF_A3DLVERTEX,nVertices,nIndex,A3DSTRM_REFERENCEPTR,A3DSTRM_STATIC))
		return ;

}

void CELCloudSceneRender::CalculateCloudColor(A3DViewport *pViewport)
{
	//group to sun vector
	for(UINT i=0;i<(*m_pGroups).size();i++)
	{
		if( (*m_pGroups)[i].bRender )
		{
			(*m_pGroups)[i].vGroupToSun = m_vSunPos - (*m_pGroups)[i].vCenterPos;
			(*m_pGroups)[i].vGroupToSun.Normalize();
		}
	}
 
	UINT idxGroup,idxSprite;
	for(i=0;i<m_vRenderSeq.size();i++)
	{
		idxSprite = m_vRenderSeq[i];
		if( idxSprite >=0 && idxSprite < m_vSprites.size())
		{
			idxGroup = m_vSprites[idxSprite]->GetGroup();
			m_vSprites[idxSprite]->SetGroupToSunVector((*m_pGroups)[idxGroup].vGroupToSun);
			m_vSprites[idxSprite]->ReShading(pViewport,(*m_pGroups)[idxGroup].fRenderSize);
		}
		
	}
}

void CELCloudSceneRender::CalculateRenderSeq(A3DViewport *pViewport)
{
	A3DVECTOR3 cameraPos = pViewport->GetCamera()->GetPos();
	//calculate render seq of group, base on m_fRenderValve
	float fDist,alpha;
	A3DVECTOR3 vDir;
	int idxSprite;
	for(UINT i=0;i<(*m_pGroups).size();i++)
	{
		fDist = CalculateDistance(cameraPos,(*m_pGroups)[i].vCenterPos);
		if ( fDist < m_fRenderValve )
		{
			for(UINT j=0;j<(*m_pGroups)[i].vSprites.size();j++)
			{
				idxSprite = (*m_pGroups)[i].vSprites[j];
				if( m_vSprites[idxSprite]->GetCloudLevel() <= m_nCloudLevel )
				{
					if( (*m_pGroups)[i].bRender == false)
						m_vRenderSeq.push_back(idxSprite);
					//alpha channel of group
					alpha = (m_fRenderValve - fDist) * 4 / m_fRenderValve;
					alpha = (alpha>1) ? 1:alpha;
					m_vSprites[idxSprite]->SetAlphaFromGroup(alpha);
				}
			}
			
			(*m_pGroups)[i].bRender = true;
		}
		else if( (*m_pGroups)[i].bRender == true)
		{
			for(int m = (int)m_vRenderSeq.size()-1;m>=0;m--)
			{
				if( m_vSprites[m_vRenderSeq[m]]->GetGroup() == i )
					m_vRenderSeq.erase(&m_vRenderSeq[m]);
			}
			(*m_pGroups)[i].bRender = false;
		}
	}		

	//sort
	int idx1,idx2;
	float fDist1,fDist2;	
	abase::vector<int>::iterator it,itPre;
	//test
	int nSort = 0;
	for(i=0;i<m_vRenderSeq.size();i++)
	{
		bool bFinish = true;

		itPre = m_vRenderSeq.begin();
		it = m_vRenderSeq.begin();++it;
		for( ;it != m_vRenderSeq.end();++it,++itPre)	
		{
			idx1 = (*itPre);
			idx2 = (*it);
			if( idx1 >= 0 && idx2 >= 0 && 
				idx1 < (int)m_vSprites.size() && idx2 < (int)m_vSprites.size() )
			{
				fDist1 = CalculateDistance(m_vSprites[idx1]->GetCenterPos(),cameraPos);
				fDist2 = CalculateDistance(m_vSprites[idx2]->GetCenterPos(),cameraPos);
				if( fDist1 > fDist2 )
				{
					(*itPre) = idx2;
					(*it) = idx1;
					bFinish = false;
				}
			}
			
		}
		
		nSort++;
		if( bFinish )
			break;		
	}

	//test
// 	char str[50];
// 	sprintf(str,"sort times = %d \n",nSort);
// 	OutputDebugString(str);
}
 
void CELCloudSceneRender::ReFillStream(A3DViewport *pViewport)
{ 
	A3DTLVERTEX* pVertex = NULL;
	if( !m_pStream->LockVertexBuffer(0,m_nStreamVertices*sizeof(A3DTLVERTEX),(BYTE**)&pVertex,0) )
		return;
	CELCloudSprite::m_pVertexBuffer = pVertex;

	//render
	int nVertices,idxStreamPos=0;
	int idxSprite,nSprites,nRenderSprites = 0;
	int nFirstparts,nRemainparts;
	//get sprites number need to be render
	idxSprite = 0;
	nVertices = 4 * m_vRenderSeq.size();

	nRemainparts = nVertices;
	while( nRemainparts > m_nStreamVertices - idxStreamPos )
	{
		//fill first parts
		nFirstparts = m_nStreamVertices - idxStreamPos;
		nSprites = nFirstparts >> 2;
		
		ReFillStreamFromSprite(pViewport,idxStreamPos,idxSprite,nSprites);
		
		//render stream
		m_pStream->UnlockVertexBuffer();
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST,0,m_nStreamVertices,0,2*m_nStreamSprites);
		if( !m_pStream->LockVertexBuffer(0,m_nStreamVertices*sizeof(A3DTLVERTEX),(BYTE**)&pVertex,0) )
			return;
		CELCloudSprite::m_pVertexBuffer = pVertex;
		
		idxStreamPos = 0;
		nRemainparts -= nFirstparts;
		idxSprite += nSprites;
		nRenderSprites = 0;
	}
	
	nSprites = nRemainparts >> 2;
	nRenderSprites += nSprites;
	ReFillStreamFromSprite(pViewport,idxStreamPos,idxSprite,nSprites);
	idxStreamPos += nRemainparts;

	m_pStream->UnlockVertexBuffer();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST,0,m_nStreamVertices,0,2*nRenderSprites);				

}

void CELCloudSceneRender::ReFillStreamFromSprite(A3DViewport *pViewport, int idxStreamPos, int idxSprite, int nSprites)
{
	int idxSprites,idx = m_vRenderSeq.size() - idxSprite -1;
	int idxGroup;
	for(int i=0;i<nSprites;i++)
	{
		idxSprites = m_vRenderSeq[idx];
		idxGroup = m_vSprites[idxSprites]->GetGroup();
		m_vSprites[idxSprites]->ReFillStream(pViewport,idxStreamPos+i*4,(*m_pGroups)[idxGroup].fRenderSize);
		idx--;		
	}
}


void CELCloudSceneRender::Release()
{ 
	A3DRELEASE(m_pTexture);
	A3DRELEASE(m_pStream);
	A3DRELEASE(m_pStreamLine);

	for(UINT i = 0;i<m_vSprites.size();i++)
	{
		if( m_vSprites[i] )
			delete m_vSprites[i];
	}
	
	m_vSprites.clear();
}

float CELCloudSceneRender::CalculateDistance(const A3DVECTOR3 &v1, const A3DVECTOR3 &v2)
{
	A3DVECTOR3 temp = v1 - v2;
	return temp.SquaredMagnitude();
}

void CELCloudSceneRender::SetSpriteColor(const A3DCOLORVALUE& clrAmb,const A3DCOLORVALUE& clrSun)
{
	CELCloudSprite::m_clrAmb = clrAmb;
	CELCloudSprite::m_clrSun = clrSun;
}

void CELCloudSceneRender::SetRenderRange(float fRenderRange)
{
	m_fRenderValve = fRenderRange*fRenderRange;
}

void CELCloudSceneRender::SetCloudLevel(int nLevel) 
{
	if( nLevel > 10 )
		nLevel = 10;
	else if ( nLevel < 1 )
		nLevel = 1;

	m_nCloudLevel = nLevel;
	//change sprite size
	ReCalculateSpriteSize();

	m_vRenderSeq.clear();
	for(UINT i=0;i<(*m_pGroups).size();i++)
	{
		(*m_pGroups)[i].bRender = false;
	}	
}

void CELCloudSceneRender::ReCalculateSpriteSize()
{
	
	for(UINT i=0;i<m_pGroups->size();i++)
	{
		(*m_pGroups)[i].fRenderSize = 110 - (110 - (*m_pGroups)[i].fSpriteSize) *(m_nCloudLevel -1 ) / 9;
		if( (*m_pGroups)[i].fRenderSize <= (*m_pGroups)[i].fSpriteSize )
			(*m_pGroups)[i].fRenderSize = (*m_pGroups)[i].fSpriteSize;
		if( (*m_pGroups)[i].fRenderSize >= 110 )
			(*m_pGroups)[i].fRenderSize = 110;
		
	}
}

void CELCloudSceneRender::ReFillStream_Line()
{
	int nSprites = m_vSprites.size();
	int nGroups = m_pGroups->size();
	int nVertices = nSprites + nGroups; 
	int nIndex = nSprites * 2;

	A3DLVERTEX* pVertex;
	if( !m_pStreamLine->LockVertexBuffer(0,nVertices*sizeof(A3DLVERTEX),(BYTE**)&pVertex,0))
		return ;
	
	WORD* pIndex;
	if( !m_pStreamLine->LockIndexBuffer(0,nIndex*sizeof(WORD),(BYTE**)&pIndex,0))
		return ;

	int idxVStream=0,idxIStream=0,idxTemp;
	int idxSprite;
	for(UINT i=0;i<m_pGroups->size();i++)
	{
		//group center pos 
		pVertex[idxVStream].x = (*m_pGroups)[i].vCenterPos.x;
		pVertex[idxVStream].y = (*m_pGroups)[i].vCenterPos.y;
		pVertex[idxVStream].z = (*m_pGroups)[i].vCenterPos.z;
		pVertex[idxVStream].diffuse = (*m_pGroups)[i].clrLine.ToRGBAColor();
		pVertex[idxVStream].specular = 0xff000000;
		pVertex[idxVStream].tu = 0.0f;
		pVertex[idxVStream].tv = 0.0f;
		
		for(UINT j=0;j<(*m_pGroups)[i].vSprites.size();j++)
		{
			idxTemp = idxVStream + j + 1;
			idxSprite = (*m_pGroups)[i].vSprites[j];
			pVertex[idxTemp].x = m_vSprites[idxSprite]->GetCenterPos().x;
			pVertex[idxTemp].y = m_vSprites[idxSprite]->GetCenterPos().y;
			pVertex[idxTemp].z = m_vSprites[idxSprite]->GetCenterPos().z;
			
			pVertex[idxTemp].diffuse = (*m_pGroups)[i].clrLine.ToRGBAColor();
			pVertex[idxTemp].specular = 0xff000000;
			pVertex[idxTemp].tu = 0.0f;
			pVertex[idxTemp].tv = 0.0f;

			pIndex[idxIStream++] = (WORD)idxVStream;
			pIndex[idxIStream++] = (WORD)idxTemp;
		}
				
		idxVStream += (*m_pGroups)[i].vSprites.size() + 1;
	}

	m_pStreamLine->UnlockVertexBuffer();
	m_pStreamLine->UnlockIndexBuffer();
}

void CELCloudSceneRender::RenderStream_Line()
{
	int nVertices = m_pGroups->size() + m_vSprites.size();
	int nPrimitive = m_vSprites.size() ;
	m_pStreamLine->Appear();	
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST,0,nVertices,0,nPrimitive);
	m_pA3DDevice->SetLightingEnable(true);
}

void CELCloudSceneRender::SetRenderLine(bool bRender)
{
	m_bRenderLine = bRender;
}

void CELCloudSceneRender::SetGroupSelected(int idxGroup)
{
	for(int i=0;i<(int)m_pGroups->size();i++)
	{
		if( idxGroup == i )
			(*m_pGroups)[idxGroup].clrLine = 0xff0000ff;
		else
			(*m_pGroups)[i].clrLine = 0xffff0000;
	}
}
