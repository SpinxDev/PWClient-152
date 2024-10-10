// Cloud.cpp: implementation of the CCloud class.
//
//////////////////////////////////////////////////////////////////////

#include "EL_Cloud.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CELCloud::m_nCloudLevel = 10;
A3DVECTOR3 CELCloud::m_vSunPos;
float CELCloud::m_fRenderValve = 800*800;

CELCloud::CELCloud()
{
	//initial old time
	m_fOldTime = 0;

	m_pA3DDevice = NULL;
	m_pTexture = NULL;
}

CELCloud::~CELCloud()
{
}

void CELCloud::Init(A3DDevice* pA3DDevice,A3DTexture* pTexture)
{
	m_pA3DDevice = pA3DDevice;
	m_pTexture = pTexture;
}


float CELCloud::CalculateDistance(const A3DVECTOR3& pos1, const A3DVECTOR3& pos2)
{
	A3DVECTOR3 temp = pos1 - pos2;
	return temp.SquaredMagnitude();
}
 
void CELCloud::CalculateRenderSeq(A3DViewport* pViewport)
{
	A3DVECTOR3 cameraPos = pViewport->GetCamera()->GetPos();
	//calculate render seq of group, base on m_fRenderValve
	float fDist,alpha;
	A3DVECTOR3 vDir;
	int idxSprite;
	UINT i(0);
	for(i=0;i<m_vGroups.size();i++)
	{
		fDist = CalculateDistance(cameraPos,m_vGroups[i].vCenterPos);
		if ( fDist < m_fRenderValve )
		{
			//alpha channel of group
			alpha = (m_fRenderValve - fDist) * 4 / m_fRenderValve;
			alpha = (alpha>1) ? 1:alpha;

			for(UINT j=0;j<m_vGroups[i].vSprites.size();j++)
			{
				idxSprite = m_vGroups[i].vSprites[j];
				if( m_vSprites[idxSprite]->GetCloudLevel() <= m_nCloudLevel )
				{
					if( m_vGroups[i].bRender == false)
						m_vRenderSeq.push_back(idxSprite);
					
					m_vSprites[idxSprite]->SetAlphaFromGroup(alpha);
				}
			}
			
			m_vGroups[i].bRender = true;
		}
		else if( m_vGroups[i].bRender == true)
		{
			for(int m = (int)m_vRenderSeq.size()-1;m>=0;m--)
			{
				if( m_vSprites[m_vRenderSeq[m]]->GetGroup() == i )
					m_vRenderSeq.erase(&m_vRenderSeq[m]);
			}
			m_vGroups[i].bRender = false;
		}
	}		


	//sort
	int idx1,idx2;
	float fDist1,fDist2;	
	abase::vector<int>::iterator it,itPre;
	for(i=0;i<m_vRenderSeq.size();i++)
	{
		bool bFinish = true;

		itPre = m_vRenderSeq.begin();
		it = m_vRenderSeq.begin();++it;
		for( ;it != m_vRenderSeq.end();++it,++itPre)	
		{
			idx1 = (*itPre);
			idx2 = (*it);
			fDist1 = CalculateDistance(m_vSprites[idx1]->GetCenterPos(),cameraPos);
			fDist2 = CalculateDistance(m_vSprites[idx2]->GetCenterPos(),cameraPos);
			if( fDist1 > fDist2 )
			{
				(*itPre) = idx2;
				(*it) = idx1;
				bFinish = false;
			}
		}
	
		if( bFinish )
			break;		
	}
}

void CELCloud::Release()
{
	m_vRenderSeq.clear();

	UINT i(0);
	for(i=0;i<m_vSprites.size();i++)
	{
		if( m_vSprites[i] != NULL )
		{
			delete m_vSprites[i];
			m_vSprites[i] = NULL;
		}
	}

	m_vSprites.clear();
	for(i=0;i<m_vGroups.size();i++)
	{
		m_vGroups[i].vSprites.clear();
	}
	m_vGroups.clear();
}

void CELCloud::ChangeCloudLevel()
{
	m_vRenderSeq.clear();
	for(UINT i=0;i<m_vGroups.size();i++)
	{
		m_vGroups[i].bRender = false;
	}		

	ReCalculateSpriteSize();
}

int CELCloud::GetRenderSpritesNum()
{
	return m_vRenderSeq.size();
}

void CELCloud::ReFillStream(A3DViewport* pViewport,int idxStreamPos, int idxSprite, int nSprites)
{
	int idxGroup;
	int idxSprites,idx = m_vRenderSeq.size() - idxSprite -1;
	for(int i=0;i<nSprites;i++)
	{
		idxSprites = m_vRenderSeq[idx];
		idxGroup = m_vSprites[idxSprites]->GetGroup();
		//if( i == 25 )
		//{
			m_vSprites[idxSprites]->ReFillStream(pViewport,idxStreamPos+i*4,m_vGroups[idxGroup].fRenderSize);
		//}
		//else
		//	m_vSprites[idxSprites]->ReFillStream(pViewport,idxStreamPos+i*4,m_vGroups[idxGroup].fRenderSize);
		idx--;		
	}
}

bool CELCloud::LoadCloud(BYTE *pBuffer)
{
	//read sprite info
	DWORD nReadTotal = 0;
	DWORD nTotalSprites = (*(DWORD*)pBuffer);	
	
	nReadTotal += 4;

	A3DVECTOR3 vCenterPos;
	DWORD dwTexture,dwLevel,dwColor[4];
	DWORD i(0);
	for(i=0;i<nTotalSprites;i++)
	{
		vCenterPos.x = (*(float*)(pBuffer+nReadTotal));
		nReadTotal += 4;
		vCenterPos.y = (*(float*)(pBuffer+nReadTotal));
		nReadTotal += 4;
		vCenterPos.z = (*(float*)(pBuffer+nReadTotal));
		nReadTotal += 4;
			
		dwTexture = (*(DWORD*)(pBuffer+nReadTotal));
		nReadTotal += 4;
		dwLevel = (*(DWORD*)(pBuffer+nReadTotal));
		nReadTotal += 4;

		for(int m=0;m<4;m++)
		{
			dwColor[m] = (*(DWORD*)(pBuffer+nReadTotal));
			nReadTotal += 4;
		}
		
		CELCloudSprite* pSprite = new CELCloudSprite();
		pSprite->Init(m_pA3DDevice);
		pSprite->SetSpriteInfo(vCenterPos,dwTexture,dwLevel,dwColor);
		m_vSprites.push_back(pSprite);	
	}

	A3DVECTOR3 vTemp;
	DWORD nGroup,nSprites,nTemp;
	nGroup = (*(DWORD*)(pBuffer+nReadTotal));
	nReadTotal += 4;

	float fSpriteSize;
	for(i=0;i<nGroup;i++)
	{
		CloudGroup oGroup;
		vTemp.x = (*(float*)(pBuffer+nReadTotal));
		nReadTotal += 4;
		vTemp.y = (*(float*)(pBuffer+nReadTotal));
		nReadTotal += 4;
		vTemp.z = (*(float*)(pBuffer+nReadTotal));
		nReadTotal += 4;		
		oGroup.vCenterPos = vTemp;

		//sprite size
		fSpriteSize = (*(float*)(pBuffer + nReadTotal));
		nReadTotal += 4;
		oGroup.fSpriteSize = fSpriteSize;

		nSprites = (*(DWORD*)(pBuffer+nReadTotal));
		nReadTotal += 4;

		for(DWORD j=0;j<nSprites;j++)
		{
			nTemp = (*(int*)(pBuffer+nReadTotal));
			nReadTotal += 4;

			oGroup.vSprites.push_back(nTemp);
			m_vSprites[nTemp]->SetGroup(i);
			m_vSprites[nTemp]->SetGroupCenterPos(oGroup.vCenterPos);
		}
	
		oGroup.bRender = false;
		m_vGroups.push_back(oGroup);
	}

	//
	ReCalculateSpriteSize();	
	return true;
}

void CELCloud::Render(A3DViewport *pViewport)
{
	//create render seq	
	CalculateRenderSeq(pViewport);	

	//group to sun vector
	UINT i(0);
	for(i=0;i<m_vGroups.size();i++)
	{
		if( m_vGroups[i].bRender )
		{
			m_vGroups[i].vGroupToSun = m_vSunPos - m_vGroups[i].vCenterPos;
			m_vGroups[i].vGroupToSun.Normalize();
		}
	}
 
	int idxGroup,idxSprite;
	for(i=0;i<m_vRenderSeq.size();i++)
	{
		idxSprite = m_vRenderSeq[i];
		idxGroup = m_vSprites[idxSprite]->GetGroup();
		m_vSprites[idxSprite]->SetGroupToSunVector(m_vGroups[idxGroup].vGroupToSun);
		m_vSprites[idxSprite]->ReShading(pViewport,m_vGroups[idxGroup].fRenderSize);
	}
}

void CELCloud::ReCalculateSpriteSize()
{
	for(UINT i=0;i<m_vGroups.size();i++)
	{
		m_vGroups[i].fRenderSize = 110 - (110 - m_vGroups[i].fSpriteSize) *(m_nCloudLevel -1 ) / 9;
		if( m_vGroups[i].fRenderSize <= m_vGroups[i].fSpriteSize )
			m_vGroups[i].fRenderSize = m_vGroups[i].fSpriteSize;
		if( m_vGroups[i].fRenderSize >= 110 )
			m_vGroups[i].fRenderSize = 110;

	}
}

bool CELCloud::Tick(int nDeltaTime)
{
	for(size_t i=0;i<m_vSprites.size();i++)
		m_vSprites[i]->Update(nDeltaTime);

	return true;
}


