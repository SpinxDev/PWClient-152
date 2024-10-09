// CloudCreator.cpp: implementation of the CCloudCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "time.h"
#include "EL_CloudCreator.h"
#include "AIniFile.h"
#include "AString.h"


/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CELCloudCreator::CELCloudCreator()
: m_fRandInv(1.0f/RAND_MAX)
, m_nLevel(10)
{
	m_wBoxMajorVersion = 1;
	m_wBoxMinorVersion = 0;
	m_wCloudMajorVersion = 1;
	m_wCloudMinorVersion = 0;
}

CELCloudCreator::~CELCloudCreator()
{
	
}

CloudCreatorParam* CELCloudCreator::GetCloudCreatorParam()
{
	return &m_pmCloudCreator;
}

void CELCloudCreator::SetCloudCreatorParam(CloudCreatorParam *pParam)
{
	m_pmCloudCreator.nCloudType = pParam->nCloudType;
	m_pmCloudCreator.nSpriteNum = pParam->nSpriteNum;	
	m_pmCloudCreator.fSpriteSize = pParam->fSpriteSize;
	m_pmCloudCreator.fCullDist = pParam->fCullDist;
	m_pmCloudCreator.bOneBoxOneSprite = pParam->bOneBoxOneSprite;
	m_pmCloudCreator.bUseBase = pParam->bUseBase;
	
	for(int i=0;i<5;i++)
	{
		m_pmCloudCreator.color[i] = pParam->color[i];
		m_pmCloudCreator.height[i] = pParam->height[i];
	}
}

bool CELCloudCreator::CreateCloud()
{
	if( m_vBoxes.size() == 0 )
		return false;
	//random create sprites base on boxes's pos and area
	CreateCloudSprite();
	//group info
	CreateGroup();
	//skylight shading 
	ShadingCloudSprite();
	//assign level to every sprite;
	AssignLevelToSprites();
	return true;
}

bool CELCloudCreator::LoadBoxFile(const char* strFilename)
{
	AFileImage file;
	if( !file.Open(strFilename,AFILE_TEXT|AFILE_OPENEXIST))
		return false;

	//clear first
	m_vBoxes.clear();

	DWORD nRead = 0;
	char buf[1024];
	file.ReadLine(buf,1000,&nRead);
	
	if( strcmp(buf," BoxVersion1.0") == 0 )
		LoadBoxFile_Version_1_0(&file);
	
	file.Close();
	return true;
}	

void CELCloudCreator::CreateCloudSprite()
{
	UINT i;
	//calculate box total area
	float fTotalArea = 0.0;
	for(i=0;i<m_vBoxes.size();i++)
	{
		fTotalArea += (m_vBoxes[i].vMaxPos.x - m_vBoxes[i].vMinPos.x )
					* (m_vBoxes[i].vMaxPos.y - m_vBoxes[i].vMinPos.y )
					* (m_vBoxes[i].vMaxPos.z - m_vBoxes[i].vMinPos.z );
	}
	fTotalArea = 1 / fTotalArea;
	//random create sprites in each box base on box's area
	float fArea;
	int nSprites=0;
	m_vSprites.clear();
	
	A3DVECTOR3 vCenterPos;
	for(i=0;i<m_vBoxes.size();i++)
	{
		//area of this box
		fArea = (m_vBoxes[i].vMaxPos.x - m_vBoxes[i].vMinPos.x )
				* (m_vBoxes[i].vMaxPos.y - m_vBoxes[i].vMinPos.y )
				* (m_vBoxes[i].vMaxPos.z - m_vBoxes[i].vMinPos.z );
		nSprites = (int)(fArea * fTotalArea * m_pmCloudCreator.nSpriteNum);
		
		if( m_pmCloudCreator.bOneBoxOneSprite && nSprites == 0)
			nSprites = 1;

		//random create sprites
		srand((unsigned int)m_vBoxes[i].nSeed);
		int UVW1 = 0,UVW2=0;
		int m;
		for(m=0;m<nSprites;m++)
		{ 
			//random pos
			vCenterPos.x = Random(m_vBoxes[i].vMaxPos.x,m_vBoxes[i].vMinPos.x);
			vCenterPos.y = Random(m_vBoxes[i].vMaxPos.y,m_vBoxes[i].vMinPos.y);
			vCenterPos.z = Random(m_vBoxes[i].vMaxPos.z,m_vBoxes[i].vMinPos.z);

			//random texture
			//first base on m_nCloudType
			/*if( m_pmCloudCreator.nCloudType == 0)
			{
				UVW1 = 5; UVW2=10;
			}
			else if ( m_pmCloudCreator.nCloudType == 1)
			{
				UVW1 = 1; UVW2=3;
			}
			else if ( m_pmCloudCreator.nCloudType == 2)
			{
				UVW1 = 10; UVW2=16;
			}
			else if ( m_pmCloudCreator.nCloudType == 3)
			{
				UVW1 = 5; UVW2=16;
			}
			*/
			//0:base,1:stratus,2:solid,3:wispy,4: no change
			switch( m_vBoxes[i].nType )
			{
			case 0:
				if( m_pmCloudCreator.bUseBase)
				{
					UVW1 = 4;UVW2= 4;
				}
				else
				{
					UVW1 = 1; UVW2=3;
				}
				
				break;
			case 1:
				UVW1 = 1; UVW2=3;
				break;
			case 2:
				UVW1 = 11; UVW2=16;
				break;
			case 3:
				UVW1 = 5; UVW2=10;
				break;
			default:
				UVW1 = 5; UVW2 =16;
				break;
			}
			
			CloudSprite oSprite;
			oSprite.vCenterPos = vCenterPos;
			oSprite.idxTexture = (int)Random((float)UVW1,(float)UVW2);
			oSprite.nGroup = m_vBoxes[i].nGroup;
			oSprite.nLevel = 0;	//initial

			m_vSprites.push_back(oSprite);
		}
	}

	//find out sprites, whose dist is too small
	float vValve = m_pmCloudCreator.fCullDist;
	vValve *= vValve; 

	abase::vector<int> vDelSprites;
	float dist;
	for(i=0;i<m_vSprites.size();i++)
	{
		for(UINT j=i+1;j<m_vSprites.size();j++)
		{
			if( i == j)
				continue;

			dist = CalculateDistance(m_vSprites[i].vCenterPos,m_vSprites[j].vCenterPos);
			if( dist < vValve )
			{
				bool bHave = false;
				for(UINT m=0;m<vDelSprites.size();m++)
				{
					if( i == (UINT)vDelSprites[m] )
					{
						bHave = true;
						break;
					}
				}

				if( !bHave ) 
					vDelSprites.push_back(i);
			}
		}
	}

	//delete sprites
	for(int m = (int)vDelSprites.size()-1; m >= 0; m--)
	{
		m_vSprites.erase(&m_vSprites[vDelSprites[m]]);
	}
}

void CELCloudCreator::CreateGroup()
{
	//calculate group number,group index base on 0
	int num = 0;
	for(UINT i=0;i<m_vBoxes.size();i++)
	{
		if( num < m_vBoxes[i].nGroup )
			num = m_vBoxes[i].nGroup;
	}
	num++;

	//max and min pos of boxes in one group
	TempPos* vTempPos = new TempPos[num];
	int idxGroup;
	for(i=0;i<m_vSprites.size();i++)
	{
		idxGroup = m_vSprites[i].nGroup;
		if( m_vSprites[i].vCenterPos.x > vTempPos[idxGroup].max.x )
			vTempPos[idxGroup].max.x = m_vSprites[i].vCenterPos.x;
		if( m_vSprites[i].vCenterPos.y > vTempPos[idxGroup].max.y )
			vTempPos[idxGroup].max.y = m_vSprites[i].vCenterPos.y;
		if( m_vSprites[i].vCenterPos.z > vTempPos[idxGroup].max.z )
			vTempPos[idxGroup].max.z = m_vSprites[i].vCenterPos.z;

		if( m_vSprites[i].vCenterPos.x < vTempPos[idxGroup].min.x )
			vTempPos[idxGroup].min.x = m_vSprites[i].vCenterPos.x;
		if( m_vSprites[i].vCenterPos.y < vTempPos[idxGroup].min.y )
			vTempPos[idxGroup].min.y = m_vSprites[i].vCenterPos.y;
		if( m_vSprites[i].vCenterPos.z < vTempPos[idxGroup].min.z )
			vTempPos[idxGroup].min.z = m_vSprites[i].vCenterPos.z;
	}

	m_vGroups.clear();
	//calcualte centerpos
	for(int m=0;m<num;m++)
	{
		CloudGroup oGroup;
		oGroup.vCenterPos.x = ( vTempPos[m].max.x + vTempPos[m].min.x ) * 0.5f;
		oGroup.vCenterPos.y = ( vTempPos[m].max.y + vTempPos[m].min.y ) * 0.5f;
		oGroup.vCenterPos.z = ( vTempPos[m].max.z + vTempPos[m].min.z ) * 0.5f;
		oGroup.bRender = false;
		oGroup.fSpriteSize = m_pmCloudCreator.fSpriteSize;
		m_vGroups.push_back(oGroup);
	}

	//set sprites index into every group
	for(i=0;i<m_vSprites.size();i++)
	{
		idxGroup = m_vSprites[i].nGroup;
		m_vGroups[idxGroup].vSprites.push_back(i);
		m_vSprites[i].vGroupCenterPos = m_vGroups[idxGroup].vCenterPos;
	}

	delete[] vTempPos;
}

float CELCloudCreator::Random(float max, float min)
{
	int ran = rand();
	return ran*(max-min)*m_fRandInv + min;
}

float CELCloudCreator::CalculateDistance(A3DVECTOR3 v1, A3DVECTOR3 v2)
{
	float x = v1.x-v2.x;
	float y = v1.y-v2.y;
	float z = v1.z-v2.z;
	return ( x*x + y*y + z*z );
}

void CELCloudCreator::GetCloudInfo(abase::vector<CloudSprite>** pSprites,abase::vector<CloudGroup>** pGroups)
{
	(*pSprites) = &m_vSprites;
	(*pGroups) = &m_vGroups;
}

void CELCloudCreator::ShadingCloudSprite()
{
	//
	float halfSpriteSize = m_pmCloudCreator.fSpriteSize*0.5f;
	for(UINT i=0;i<m_vGroups.size();i++)
	{
		float max_h = -10000,min_h=10000;
	
		abase::vector<int>::iterator it;
		for(it=m_vGroups[i].vSprites.begin();it!=m_vGroups[i].vSprites.end();++it)
		{
			int idxSprite = (*it);
			if( m_vSprites[idxSprite].vCenterPos.y < min_h)
				min_h = m_vSprites[idxSprite].vCenterPos.y;
			if( m_vSprites[idxSprite].vCenterPos.y > max_h)
				max_h = m_vSprites[idxSprite].vCenterPos.y;
		}
		max_h += halfSpriteSize;
		min_h -= halfSpriteSize;

		//calculate height level
		float height = max_h - min_h;
		float level[5];
		for(int m=0;m<5;m++)
			level[m] = min_h + height * ( m_pmCloudCreator.height[m] * 0.01f);


		float vHeight;		//height of vertex
		A3DCOLORVALUE vColor;
		//int idxLevel;
		for(it = m_vGroups[i].vSprites.begin();it != m_vGroups[i].vSprites.end();++it)
		{
			int idx = (*it);
			for(int m=0;m<2;m++)
			{
				if( m == 0 )
					vHeight = m_vSprites[idx].vCenterPos.y + halfSpriteSize;
				else 
					vHeight = m_vSprites[idx].vCenterPos.y - halfSpriteSize;
				//test
				if( vHeight > level[3] )
				{
					vColor = m_pmCloudCreator.color[4];
				}
				if ( vHeight <= level[3] )
				{
					vColor = m_pmCloudCreator.color[3];
				}
				if ( vHeight <= level[2] )
				{
					vColor = m_pmCloudCreator.color[2];
				}
				if ( vHeight <= level[1] )
				{
					vColor = m_pmCloudCreator.color[1];
				}
				if ( vHeight <= level[0] )
				{
					vColor = m_pmCloudCreator.color[0];
				}

				m_vSprites[idx].vColor[m*2] = vColor;
				m_vSprites[idx].vColor[m*2+1] = vColor;
			}
		}
	}
}

bool CELCloudCreator::SaveCloud(const char* strFilename)
{
	AFile file;
	if( !file.Open(strFilename,AFILE_BINARY|AFILE_CREATENEW) )
		return false;

	DWORD nWritten;
	//version info
	file.Write(&m_wCloudMajorVersion,sizeof(WORD),&nWritten);
	file.Write(&m_wCloudMinorVersion,sizeof(WORD),&nWritten);

	int nLength =  CalculateCloudFileLength();
	//write cloud info length
	file.Write(&nLength,sizeof(int),&nWritten);
	//write sprites info
	int nTotalSprites = m_vSprites.size();
	file.Write(&nTotalSprites,sizeof(int),&nWritten);
	
	A3DVECTOR3 vTemp;
	DWORD dwTemp;
	for(int i=0;i<nTotalSprites;i++)
	{
		//write center pos of sprite
		vTemp = m_vSprites[i].vCenterPos;

		file.Write(&vTemp.x,sizeof(float),&nWritten);
		file.Write(&vTemp.y,sizeof(float),&nWritten);
		file.Write(&vTemp.z,sizeof(float),&nWritten);

		dwTemp = m_vSprites[i].idxTexture;
		file.Write(&dwTemp,sizeof(DWORD),&nWritten);
		dwTemp = m_vSprites[i].nLevel;
		file.Write(&dwTemp,sizeof(DWORD),&nWritten);
			
		dwTemp = m_vSprites[i].vColor[0].ToRGBAColor();
		file.Write(&dwTemp,sizeof(DWORD),&nWritten);
		dwTemp = m_vSprites[i].vColor[1].ToRGBAColor();
		file.Write(&dwTemp,sizeof(DWORD),&nWritten);
		dwTemp = m_vSprites[i].vColor[2].ToRGBAColor();
		file.Write(&dwTemp,sizeof(DWORD),&nWritten);
		dwTemp = m_vSprites[i].vColor[3].ToRGBAColor();
		file.Write(&dwTemp,sizeof(DWORD),&nWritten);
	}

	//write group info
	int nGroup = m_vGroups.size();
	file.Write(&nGroup,sizeof(int),&nWritten);
	
	int nSprites,nTemp;
	float fSpriteSize;
	for(i=0;i<(int)m_vGroups.size();i++)
	{
		vTemp = m_vGroups[i].vCenterPos;
		file.Write(&vTemp.x,sizeof(float),&nWritten);
		file.Write(&vTemp.y,sizeof(float),&nWritten);
		file.Write(&vTemp.z,sizeof(float),&nWritten);

		fSpriteSize = m_vGroups[i].fSpriteSize;
		file.Write(&fSpriteSize,sizeof(float),&nWritten);

		nSprites = m_vGroups[i].vSprites.size();
		file.Write(&nSprites,sizeof(int),&nWritten);
		
		abase::vector<int>::iterator it;
		for(it = m_vGroups[i].vSprites.begin();
		    it != m_vGroups[i].vSprites.end();++it)
		{
			nTemp = (*it);
			file.Write(&nTemp,sizeof(int),&nWritten);
		}
	}

	//write box info
	int nBox = m_vBoxes.size();
	file.Write(&nBox,sizeof(int),&nWritten);

	float fTemp;
	for(i=0;i<nBox;i++)
	{
		nTemp = m_vBoxes[i].nGroup;
		file.Write(&nTemp,sizeof(int),&nWritten);
		nTemp = m_vBoxes[i].nType;
		file.Write(&nTemp,sizeof(int),&nWritten);
		nTemp = m_vBoxes[i].nSeed;
		file.Write(&nTemp,sizeof(int),&nWritten);

		fTemp = m_vBoxes[i].vMaxPos.x;
		file.Write(&fTemp,sizeof(float),&nWritten);
		fTemp = m_vBoxes[i].vMaxPos.y;
		file.Write(&fTemp,sizeof(float),&nWritten);
		fTemp = m_vBoxes[i].vMaxPos.z;
		file.Write(&fTemp,sizeof(float),&nWritten);

		fTemp = m_vBoxes[i].vMinPos.x;
		file.Write(&fTemp,sizeof(float),&nWritten);
		fTemp = m_vBoxes[i].vMinPos.y;
		file.Write(&fTemp,sizeof(float),&nWritten);
		fTemp = m_vBoxes[i].vMinPos.z;
		file.Write(&fTemp,sizeof(float),&nWritten);
	}

	file.Close();
	return true;
}

bool CELCloudCreator::LoadCloud(const char* strFilename)
{
	AFileImage file;
	if( !file.Open(strFilename,AFILE_BINARY|AFILE_OPENEXIST) )
		return false;

	//clear
	m_vSprites.clear();
	m_vGroups.clear();
	m_vBoxes.clear();

	DWORD nRead;
	//version info
	WORD major,minor;
	file.Read(&major,sizeof(WORD),&nRead);
	file.Read(&minor,sizeof(WORD),&nRead);

	if( major == 1 && minor == 0 )
		LoadCloud_Version_1_0(&file);
	
	file.Close();
	return true;
}

void CELCloudCreator::Release()
{

}

void CELCloudCreator::AssignLevelToSprites()
{
	const int MIN_SPRITESNUM = 50;
	//lowest level: 50 sprites
	int nAllSprites = m_vSprites.size();
	if( nAllSprites < MIN_SPRITESNUM )
	{
		for(UINT i=0;i<m_vSprites.size();i++)
			m_vSprites[i].nLevel = 1;
		return;
	}

	int nNum;	//sprites number in one level
	int nSpritesGroup;		// sprite num in one group
	int nSpritesNumGroup;	// leveled sprites in group
	abase::vector<int> vSprites;	
	int nIndex;
	for(int i=1;i<=m_nLevel;i++)
	{
		//sprites number in one level
		if( i == 1 )
			nNum = MIN_SPRITESNUM;
		else
			nNum =  (int) ((nAllSprites - MIN_SPRITESNUM) / 9.0f);
		
		for(UINT j=0;j<m_vGroups.size();j++)
		{
			nSpritesGroup = m_vGroups[j].vSprites.size();
			nSpritesNumGroup = (int)(nSpritesGroup * nNum * 1.0f / nAllSprites + 0.5f);
			
			for(int m=0;m<nSpritesNumGroup;m++)
			{
				nIndex = (int)Random(0.0f,float(nSpritesGroup-1));
				int idxSprite = m_vGroups[j].vSprites[nIndex];
				if( m_vSprites[idxSprite].nLevel == 0 )
					m_vSprites[idxSprite].nLevel = i;
				else 
				{
					//idxSprite = m_vGroups[j].vSprites[nIndex+1];
					for(int n=0;n<nSpritesGroup;n++)
					{
						nIndex++;
						if( nIndex >= nSpritesGroup)
							nIndex = 0;

						idxSprite = m_vGroups[j].vSprites[nIndex];
						if( m_vSprites[idxSprite].nLevel == 0 )
						{
							m_vSprites[idxSprite].nLevel = i;
							break;
						}
					}
				}
			}
			
		}
	}

	//add on
	for(i=0;i<nAllSprites;i++)
	{
		if( m_vSprites[i].nLevel == 0 )
		{
			int nLevel = (int)Random(1.0f,10.0f);
			m_vSprites[i].nLevel = nLevel;
		}
	}

}

void CELCloudCreator::ReCreateCloud_Group(int idxGroup, int nSprites)
{
	if( idxGroup >= m_vGroups.size() || idxGroup < 0 )
		return;
	//clear first
	ClearSpritesNoGroup(idxGroup);
	
	ReCreateSprites_Group(idxGroup,nSprites);
	CreateGroup();
	ReShadingCloudSprite_Group(idxGroup);
	ReAssignLevelToSprites_Group(idxGroup);
}

void CELCloudCreator::GetBoxFileInfo(abase::vector<CloudBox>** pBoxes)
{
	(*pBoxes) = &m_vBoxes;
}

void CELCloudCreator::AddBoxInfo(const CloudBox &boxInfo)
{
	m_vBoxes.push_back(boxInfo);

	if( m_vGroups.size() <= boxInfo.nGroup )
	{
		CloudGroup oGroup;
		m_vGroups.push_back(oGroup);
	}
}

bool CELCloudCreator::SaveBoxFile(const char *strFilename)
{
	AFile file;
	if( !file.Open(strFilename,AFILE_TEXT|AFILE_CREATENEW))
		return false;

	//version control
	char version[50];
	sprintf(version," BoxVersion%d.%d",m_wBoxMajorVersion,m_wBoxMinorVersion);
	file.WriteLine(version);

	AString str;
	for(UINT i=0;i<m_vBoxes.size();i++)
	{
		str.Format("%d,%d,%d,%f,%f,%f,%f,%f,%f",
			m_vBoxes[i].nGroup,m_vBoxes[i].nType,m_vBoxes[i].nSeed,
			m_vBoxes[i].vMaxPos.x,m_vBoxes[i].vMaxPos.y,m_vBoxes[i].vMaxPos.z,
			m_vBoxes[i].vMinPos.x,m_vBoxes[i].vMinPos.y,m_vBoxes[i].vMinPos.z);

		if( !file.WriteLine(str) )
			return false;
	}

	file.Close();
	return true;
}

void CELCloudCreator::SetCloudHeight(float fHeight)
{
	//change sprites and groups height
	for(UINT i=0;i<m_vGroups.size();i++)
	{
		m_vGroups[i].vCenterPos.y  += fHeight;
	}

	for(i=0;i<m_vSprites.size();i++)
	{
		m_vSprites[i].vCenterPos.y += fHeight;
		m_vSprites[i].vGroupCenterPos.y += fHeight;
	}

	for(i=0;i<m_vBoxes.size();i++)
	{
		m_vBoxes[i].vMaxPos.y += fHeight;
		m_vBoxes[i].vMinPos.y += fHeight;
	}
}

void CELCloudCreator::ClearBoxInfo()
{
	m_vBoxes.clear();
}

void CELCloudCreator::ReCreateSprites_Group(int idxGroup,int nSprites)
{
	if( idxGroup < 0 || idxGroup >= (int)m_vGroups.size() )
		return;
		
	//get box info in the group
	abase::vector<CloudBox> vBoxes;
	for(UINT i=0;i<m_vBoxes.size();i++)
	{
		if( m_vBoxes[i].nGroup == idxGroup )
		{
			vBoxes.push_back(m_vBoxes[i]);
		}
	}

	//total area in these boxes
	float fTotalArea = 0.0f;
	A3DVECTOR3 vTemp;
	for(i=0;i<vBoxes.size();i++)
	{
		vTemp = vBoxes[i].vMaxPos - vBoxes[i].vMinPos;
		fTotalArea += vTemp.x * vTemp.y * vTemp.z;
	}
	fTotalArea = 1 / fTotalArea; 
	//every box
	float fArea;
	int nSprite;
	A3DVECTOR3 vCenterPos;
	for(i = 0;i<vBoxes.size();i++)
	{
		vTemp = vBoxes[i].vMaxPos - vBoxes[i].vMinPos;
		fArea = vTemp.x * vTemp.y * vTemp.z;
		
		nSprite = (int)(fArea * fTotalArea * nSprites + 0.5f);
		if( nSprite == 0 )
			nSprite = 1;

		srand(vBoxes[i].nSeed);
		int UVW1=0,UVW2=0;
		int m;
		for(m=0;m<nSprite;m++)
		{ 
			//random pos
			vCenterPos.x = Random(vBoxes[i].vMaxPos.x,vBoxes[i].vMinPos.x);
			vCenterPos.y = Random(vBoxes[i].vMaxPos.y,vBoxes[i].vMinPos.y);
			vCenterPos.z = Random(vBoxes[i].vMaxPos.z,vBoxes[i].vMinPos.z);

			//random texture
			//first base on m_nCloudType
			if( m_pmCloudCreator.nCloudType == 0)
			{
				UVW1 = 4; UVW2= 4;
			}
			else if ( m_pmCloudCreator.nCloudType == 1)
			{
				UVW1 = 1; UVW2=3;
			}
			else if ( m_pmCloudCreator.nCloudType == 2 )
			{
				UVW1 = 10; UVW2=16;
			}
			else if ( m_pmCloudCreator.nCloudType == 3)
			{
				UVW1 = 5; UVW2=16;
			}
			
			//0:base,1:stratus,2:solid,3:wispy,4: no change
			switch( vBoxes[i].nType )
			{
			case 0:
				UVW1 = 4;UVW2= 4;
				break;
			case 1:
				UVW1 = 1; UVW2=3;
				break;
			case 2:
				UVW1 = 11; UVW2=16;
				break;
			case 3:
				UVW1 = 5; UVW2=10;
				break;
			default:
				break;
			}
			
			CloudSprite oSprite;
			oSprite.vCenterPos = vCenterPos;
			oSprite.idxTexture = (int)Random((float)UVW1,(float)UVW2);
			oSprite.nGroup = idxGroup;
			oSprite.nLevel = 0;	//initial

			m_vSprites.push_back(oSprite);
		}
	}
}

void CELCloudCreator::ReShadingCloudSprite_Group(int idxGroup)
{
	float halfSpriteSize = m_vGroups[idxGroup].fSpriteSize*0.5f;
	
	float max_h = -10000,min_h=10000;	
	abase::vector<int>::iterator it;
	for(it=m_vGroups[idxGroup].vSprites.begin();it!=m_vGroups[idxGroup].vSprites.end();++it)
	{
		int idxSprite = (*it);
		if( m_vSprites[idxSprite].vCenterPos.y < min_h)
			min_h = m_vSprites[idxSprite].vCenterPos.y;
		if( m_vSprites[idxSprite].vCenterPos.y > max_h)
			max_h = m_vSprites[idxSprite].vCenterPos.y;
	}
	max_h += halfSpriteSize;
	min_h -= halfSpriteSize;

	//calculate height level
	float height = max_h - min_h;
	float level[5];
	for(int m=0;m<5;m++)
		level[m] = min_h + height * ( m_pmCloudCreator.height[m] * 0.01f);


	float vHeight;		//height of vertex
	A3DCOLORVALUE vColor;
	for(it = m_vGroups[idxGroup].vSprites.begin();it != m_vGroups[idxGroup].vSprites.end();++it)
	{
		int idx = (*it);
		for(int m=0;m<2;m++)
		{
			if( m == 0 )
				vHeight = m_vSprites[idx].vCenterPos.y + halfSpriteSize;
			else 
				vHeight = m_vSprites[idx].vCenterPos.y - halfSpriteSize;
			//test
			if( vHeight > level[3] )
			{
				vColor = m_pmCloudCreator.color[4];
			}
			if ( vHeight <= level[3] )
			{
				vColor = m_pmCloudCreator.color[3];
			}
			if ( vHeight <= level[2] )
			{
				vColor = m_pmCloudCreator.color[2];
			}
			if ( vHeight <= level[1] )
			{
				vColor = m_pmCloudCreator.color[1];
			}
			if ( vHeight <= level[0] )
			{
				vColor = m_pmCloudCreator.color[0];
			}

			m_vSprites[idx].vColor[m*2] = vColor;
			m_vSprites[idx].vColor[m*2+1] = vColor;
		}
	}
}

void CELCloudCreator::ReAssignLevelToSprites_Group(int idxGroup)
{
	//lowest level: 50 sprites
	int MIN_SPRITESNUM = 5;
	int nAllSprites = m_vGroups[idxGroup].vSprites.size();
	if( nAllSprites < MIN_SPRITESNUM )
	{
		int idxSprite;
		for(UINT i=0;i<m_vGroups[idxGroup].vSprites.size();i++)
		{
			idxSprite = m_vGroups[idxGroup].vSprites[i];
			m_vSprites[idxSprite].nLevel = 1;
		}
		return;
	}

	int nNum;	//sprites number in one level
	int nSpritesGroup;		// sprite num in one group
	int nSpritesNumGroup;	// leveled sprites in group
	abase::vector<int> vSprites;	
	int nIndex;
	for(int i=1;i<=m_nLevel;i++)
	{
		//sprites number in one level
		if( i == 1 )
			nNum = MIN_SPRITESNUM;
		else
			nNum = (int)((nAllSprites - MIN_SPRITESNUM) / 9.0f);
		
		nSpritesGroup = m_vGroups[idxGroup].vSprites.size();
		nSpritesNumGroup = (int)(nSpritesGroup * nNum * 1.0f / nAllSprites + 0.5f);
			
		for(int m=0;m<nSpritesNumGroup;m++)
		{
			nIndex = (int)Random(0.0f,float(nSpritesGroup-1));
			int idxSprite = m_vGroups[idxGroup].vSprites[nIndex];
			if( m_vSprites[idxSprite].nLevel == 0 )
				m_vSprites[idxSprite].nLevel = i;
			else 
			{
				//idxSprite = m_vGroups[j].vSprites[nIndex+1];
				for(int n=0;n<nSpritesGroup;n++)
				{
					nIndex++;
					if( nIndex >= nSpritesGroup)
						nIndex = 0;

					idxSprite = m_vGroups[idxGroup].vSprites[nIndex];
					if( m_vSprites[idxSprite].nLevel == 0 )
					{
						m_vSprites[idxSprite].nLevel = i;
						break;
					}
				}
			}
		}
	}

	//add on
	int idxSprite = 0;
	for(i=0;i<nAllSprites;i++)
	{
		idxSprite = m_vGroups[idxGroup].vSprites[i];
		if( m_vSprites[idxSprite].nLevel == 0 )
		{
			int nLevel = (int)Random(1.0f,10.0f);
			m_vSprites[idxSprite].nLevel = nLevel;
		}
	}
}

void CELCloudCreator::ChangeSpriteSize(int idxGroup, float fSpriteSize)
{
	if( idxGroup >= 0 && idxGroup < (int)m_vGroups.size() )
	{
		m_vGroups[idxGroup].fSpriteSize = fSpriteSize;
	}
}

void CELCloudCreator::SetBoxSeed(int idxBox, unsigned int nSeed)
{
	if( idxBox >=0 && idxBox < (int)m_vBoxes.size())
		m_vBoxes[idxBox].nSeed = nSeed;
}

void CELCloudCreator::LoadBoxFile_Version_1_0(AFileImage *pFile)
{
	char buf[1024];
	DWORD nRead = 0;
	while( pFile->ReadLine(buf,1024,&nRead))
	{
		CloudBox oBox;
		int idx = 0;

		char* token = strtok(buf,",\t\n");
		while( token != NULL )
		{
			switch( idx )
			{
			case 0:
				oBox.nGroup = atoi(token);
				break;
			case 1:
				oBox.nType = atoi(token);
				break;
			case 2:
				oBox.nSeed = 1000;//atoi(token);	//seed of random
				break;
			case 3:
				oBox.vMaxPos.x = (float)atof(token);
				break;
			case 4:
				oBox.vMaxPos.y = (float)atof(token);
				break;
			case 5:
				oBox.vMaxPos.z = (float)atof(token);
				break;
			case 6:
				oBox.vMinPos.x = (float)atof(token);
				break;
			case 7:
				oBox.vMinPos.y = (float)atof(token);
				break;
			case 8:
				oBox.vMinPos.z = (float)atof(token);
				break;
			}
			
			token = strtok(NULL,",\t\n");
			idx++;
		}

		m_vBoxes.push_back(oBox);
	}	

}

void CELCloudCreator::LoadCloud_Version_1_0(AFileImage* pFile)
{
	DWORD nRead = 0;
	//read cloud info length
	int nLength;
	pFile->Read(&nLength,sizeof(int),&nRead);
	//read sprite info
	int nTotalSprites;
	pFile->Read(&nTotalSprites,sizeof(int),&nRead);
		
	A3DVECTOR3 vCenterPos;
	DWORD dwTexture,dwLevel,dwColor[4];
	for(int i=0;i<nTotalSprites;i++)
	{
		pFile->Read(&vCenterPos.x,sizeof(float),&nRead);
		pFile->Read(&vCenterPos.y,sizeof(float),&nRead);
		pFile->Read(&vCenterPos.z,sizeof(float),&nRead);

		pFile->Read(&dwTexture,sizeof(DWORD),&nRead);
		pFile->Read(&dwLevel,sizeof(DWORD),&nRead);

		for(int m=0;m<4;m++)
			pFile->Read(&dwColor[m],sizeof(DWORD),&nRead);

		CloudSprite oSprite;
		oSprite.vCenterPos = vCenterPos;
		oSprite.idxTexture = dwTexture;	
		oSprite.nLevel = dwLevel;
		for(m=0;m<4;m++)
		{
			oSprite.vColor[m] = dwColor[m];
		}			
		m_vSprites.push_back(oSprite);
	}

	A3DVECTOR3 vTemp;
	int nGroup,nSprites,nTemp;
	pFile->Read(&nGroup,sizeof(int),&nRead);
		
	float fSpriteSize;
	for( i=0;i<nGroup;i++)
	{
		CloudGroup oGroup;
		pFile->Read(&vTemp.x,sizeof(float),&nRead);
		pFile->Read(&vTemp.y,sizeof(float),&nRead);
		pFile->Read(&vTemp.z,sizeof(float),&nRead);
		oGroup.vCenterPos = vTemp;

		pFile->Read(&fSpriteSize,sizeof(float),&nRead);
		oGroup.fSpriteSize = fSpriteSize;
		
		pFile->Read(&nSprites,sizeof(int),&nRead);
		for(int j=0;j<nSprites;j++)
		{
			pFile->Read(&nTemp,sizeof(int),&nRead);

			oGroup.vSprites.push_back(nTemp);
			m_vSprites[nTemp].nGroup = i;
			m_vSprites[nTemp].vGroupCenterPos = vTemp;
		}

		m_vGroups.push_back(oGroup);
	}

	int nBox;
	pFile->Read(&nBox,sizeof(int),&nRead);

	for(i=0;i<nBox;i++)
	{
		CloudBox oBox;
		pFile->Read(&oBox.nGroup,sizeof(int),&nRead);
		pFile->Read(&oBox.nType,sizeof(int),&nRead);
		pFile->Read(&oBox.nSeed,sizeof(int),&nRead);

		pFile->Read(&oBox.vMaxPos.x,sizeof(float),&nRead);
		pFile->Read(&oBox.vMaxPos.y,sizeof(float),&nRead);
		pFile->Read(&oBox.vMaxPos.z,sizeof(float),&nRead);

		pFile->Read(&oBox.vMinPos.x,sizeof(float),&nRead);
		pFile->Read(&oBox.vMinPos.y,sizeof(float),&nRead);
		pFile->Read(&oBox.vMinPos.z,sizeof(float),&nRead);

		m_vBoxes.push_back(oBox);
	}
}

void CELCloudCreator::GetCloudFileVersion(WORD &wMajor, WORD &wMinor)
{
	wMajor = m_wCloudMajorVersion;
	wMinor = m_wCloudMinorVersion;
}

bool CELCloudCreator::SaveRenderParamFile(const char *strFilename)
{
	// TODO: Add your control notification handler code here
	AIniFile file;
	if( !file.Open(strFilename) )
		return false;

	bool re;
	re = file.WriteIntValue("General Info","SpritesNumber",m_pmCloudCreator.nSpriteNum);
	re = file.WriteFloatValue("General Info","SpriteSize",m_pmCloudCreator.fSpriteSize);
	file.WriteFloatValue("General Info","CullDistance",m_pmCloudCreator.fCullDist);
	file.WriteIntValue("General Info","CloudType",m_pmCloudCreator.nCloudType);

	if( m_pmCloudCreator.bUseBase )
		file.WriteIntValue("General Info","UseBase",1);
	else
		file.WriteIntValue("General Info","UserBase",0);

	if( m_pmCloudCreator.bOneBoxOneSprite )
		file.WriteIntValue("General Info","OneBoxOneSprite",1);
	else
		file.WriteIntValue("General Info","OneBoxOneSprite",0);

	char strKey[10];
	char str[100];
	for(int i=0;i<5;i++)
	{
		sprintf(strKey,"Color%d",i+1);
		sprintf(str,"%f,%f,%f",m_pmCloudCreator.color[i].r,m_pmCloudCreator.color[i].g,m_pmCloudCreator.color[i].b);
		file.WriteStringValue("Color Info",strKey,str);

		sprintf(strKey,"Height%d",i+1);
		file.WriteIntValue("Height Info",strKey,m_pmCloudCreator.height[i]);	
	}

	file.Save(strFilename);
	file.Close();
	return true;
}

bool CELCloudCreator::LoadRenderParamFile(const char *strFilename)
{
	// TODO: Add your control notification handler code here
	AIniFile file;
	if( !file.Open(strFilename) )
		return false;

	m_pmCloudCreator.nSpriteNum = file.GetValueAsInt("General Info","SpritesNumber",0);
	m_pmCloudCreator.fSpriteSize = file.GetValueAsFloat("General Info","SpriteSize",0);
	m_pmCloudCreator.fCullDist = file.GetValueAsFloat("General Info","CullDistance",0);
	m_pmCloudCreator.nCloudType = file.GetValueAsInt("General Info","CloudType",0);
	if( file.GetValueAsInt("General Info","UseBase",0) )
		m_pmCloudCreator.bUseBase = true;
	else
		m_pmCloudCreator.bUseBase = false;
	if( file.GetValueAsInt("General Info","OneBoxOneSprite",0) )
		m_pmCloudCreator.bOneBoxOneSprite = true;
	else 
		m_pmCloudCreator.bOneBoxOneSprite = false;

	char strKey[10];
	AString strLine;
	float r,g,b;
	for(int i=0;i<5;i++)
	{
		sprintf(strKey,"Color%d",i+1);
		strLine = file.GetValueAsString("Color Info",strKey,"Error");
		sscanf(strLine.GetBuffer(0),"%f,%f,%f",&r,&g,&b);
		m_pmCloudCreator.color[i].Set(r,g,b,1.0f);

		sprintf(strKey,"Height%d",i+1);
		m_pmCloudCreator.height[i] = file.GetValueAsInt("Height Info",strKey,0);
	}

	file.Close();
	return true;
}

int CELCloudCreator::CalculateCloudFileLength()
{
	int nSpriteLength = (m_vSprites.size() * 9 + 1) * sizeof(DWORD);
	
	int nGroupSprites = 0;
	for(int i=0;i<m_vGroups.size();i++)
	{
		nGroupSprites += m_vGroups[i].vSprites.size();
	}

	int nGroupLength = m_vGroups.size() * 5 * sizeof(DWORD) + nGroupSprites * sizeof(int) + sizeof(int) ;

	return nSpriteLength + nGroupLength;
}

bool CELCloudCreator::SaveMergeFile(const char* strMergeFile,int nRow,int nCol,abase::vector<CloudFileInfo>& vCloudFileInfo,int nMajorVersion,int nMinorVersion)
{
	AFile big;
	if( !big.Open(strMergeFile,AFILE_BINARY|AFILE_CREATENEW))
		return false;

	DWORD nWrite = 0;
	big.Write(&nRow,sizeof(int),&nWrite);
	big.Write(&nCol,sizeof(int),&nWrite);
	
	DWORD nFiles = vCloudFileInfo.size();
	big.Write(&nFiles,sizeof(DWORD),&nWrite);

	for(int i=0;i<vCloudFileInfo.size();i++)
	{
		AFile file;
		if( !file.Open(vCloudFileInfo[i].strFilename,AFILE_BINARY|AFILE_OPENEXIST) )
			return false;

		DWORD nRead = 0 ;
		//version
		WORD major,minor;
		file.Read(&major,sizeof(WORD),&nRead);
		file.Read(&minor,sizeof(WORD),&nRead);
		if( major != nMajorVersion || minor != nMinorVersion )
			return false;
			
		//read cloud info
		DWORD nLength;
		file.Read(&nLength,sizeof(DWORD),&nRead);

		//write scene index
		big.Write(&vCloudFileInfo[i].idxScene,sizeof(DWORD),&nWrite);
		//write buffer length
		big.Write(&nLength,sizeof(DWORD),&nWrite);
		
		if( nLength > 0 )
		{
			BYTE* pCloudBuffer = new BYTE[nLength];
			file.Read(pCloudBuffer,nLength,&nRead);

			OffsetCloudInfo(pCloudBuffer,vCloudFileInfo[i].off_x,
										 vCloudFileInfo[i].off_y,
										 vCloudFileInfo[i].off_z);

			//write cloud info
			big.Write(pCloudBuffer,nLength,&nWrite);
			delete[] pCloudBuffer;
		}
		
		file.Close();		
	}

	return true;
}

void CELCloudCreator::OffsetCloudInfo(BYTE* pBuffer,float off_x,float off_y,float off_z)
{
	int nReadAll = 0;
	int nSprites = (*(int*)pBuffer);
	nReadAll += 4;

	float* value;
	for(int i=0;i<nSprites;i++)
	{
		value = (float*)(pBuffer + nReadAll);
		*value = (*value + off_x);
		nReadAll += 4;

		value = (float*)(pBuffer + nReadAll);
		*value = (*value + off_y);
		nReadAll += 4;

		value = (float*)(pBuffer + nReadAll);
		*value = (*value + off_z);
		nReadAll += 4;

		nReadAll += 6 * sizeof(DWORD);
	}

	
	int nGroups = (*(int*)(pBuffer + nReadAll));
	nReadAll += 4;
	for(i=0;i<nGroups;i++)
	{
		value = (float*)(pBuffer + nReadAll);
		*value = (*value + off_x);
		nReadAll += 4;

		value = (float*)(pBuffer + nReadAll);
		*value = (*value + off_y);
		nReadAll += 4;

		value = (float*)(pBuffer + nReadAll);
		*value = (*value + off_z);
		nReadAll += 4;

		nReadAll += 1 * sizeof(DWORD);

		int nGroupSprites = (*(int*)(pBuffer + nReadAll));
		nReadAll += 4;
		nReadAll += nGroupSprites * sizeof(int);
	}
}

void CELCloudCreator::ClearSpritesNoGroup(int idxGroup)
{
	if( m_vGroups.size() <= 0 )
		return;

	int* vGroupID = new int[m_vGroups.size()];	//
	memset(vGroupID,0,4*m_vGroups.size());

	int idx;
	for(UINT i=0;i<m_vBoxes.size();i++)
	{
		idx = m_vBoxes[i].nGroup;
		if( idx >=0 && idx < m_vGroups.size() )
			vGroupID[idx]++;
	}

	int idxSprites;
	for(i=0;i<m_vGroups.size();i++)
	{
		if( vGroupID[i] == 0 || i == idxGroup )
		{
			for(UINT m=0;m<m_vGroups[i].vSprites.size();m++)
			{
				idxSprites = m_vGroups[i].vSprites[m];
				if ( idxSprites >= 0 && idxSprites < m_vSprites.size() )
					m_vSprites[idxSprites].nGroup = -1;
			}
			m_vGroups[i].vSprites.clear();
		}
	}

	for( int n = (int)m_vSprites.size()-1;n >= 0;n--)
	{
		if( m_vSprites[n].nGroup < 0 || m_vSprites[n].nGroup >= m_vGroups.size() )
			m_vSprites.erase(&m_vSprites[n]);
	}
	
	delete[] vGroupID;
}
