// CloudCreator.h: interface for the CCloudCreator class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "AFileImage.h"
#include "A3DTypes.h"
#include "vector.h"
 
typedef struct _CLOUDCREATORPARAM
{
	int nSpriteNum;			//700-1000
	float fSpriteSize;		//60.0 
	float fCullDist;		//6.5
	int nCloudType;			//0:Low,1:stratus,2:solid,3:wispy
	bool bUseBase;			//use base texture
	bool bOneBoxOneSprite;	//at least one sprite in one box

	A3DCOLORVALUE color[5];
	int height[5];

	_CLOUDCREATORPARAM()
	{
		nSpriteNum = 700;
		fSpriteSize = 70.0f;
		fCullDist = 9.25f;
		nCloudType = 3;
		bUseBase = true;
		bOneBoxOneSprite = true;

		color[0].r=104/255.0f;color[0].g=105/255.0f;color[0].b=110/255.0f;
		color[1].r=155/255.0f;color[1].g=156/255.0f;color[1].b=163/255.0f;
		color[2].r=193/255.0f;color[2].g=196/255.0f;color[2].b=203/255.0f;
		color[3].r=224/255.0f;color[3].g=230/255.0f;color[3].b=235/255.0f;
		color[4].r=250/255.0f;color[4].g=250/255.0f;color[4].b=250/255.0f;

		height[0] = 0;
		height[1] = 18;
		height[2] = 22;
		height[3] = 70;
		height[4] = 100;
	}
}CloudCreatorParam;

//----------------------------
//data load from box file 
typedef struct _CLOUD_BOX
{
	int nGroup;				//belong to which group
	int nType;				//0:base,1:stratus,2:solid,3:wispy,4: no change
	int nSeed;				//seed of random 
	A3DVECTOR3 vMaxPos;
	A3DVECTOR3 vMinPos;
}CloudBox;

//--------------------------
//sprite info
typedef struct _SPRITE_INFO
{
	int nGroup;
	A3DVECTOR3 vCenterPos;
	A3DVECTOR3 vGroupCenterPos;
	int idxTexture;
	A3DCOLORVALUE vColor[4];
	int nLevel;		//[1,10]
}CloudSprite;

//------------------------
typedef struct _CLOUD_GROUP
{
	A3DVECTOR3 vCenterPos;			//origin center pos of Group,use for SunLight Shading and render sequence
	abase::vector<int> vSprites;		//index of sprites in this group and render sequence
	
	A3DVECTOR3 vGroupToSun;			//group to sun vector
	float fSpriteSize;				//size of sprite in this group
	
	//for rendering 
	bool bRender;
	float fRenderSize;				//sprite size when render with different cloud level
	A3DCOLORVALUE clrLine;			//line color of selected group
	
	_CLOUD_GROUP()
	{
		clrLine.Set(1.0f,0.0f,0.0f,1.0f);
		fSpriteSize = 50.0f;
	}

}CloudGroup;

typedef struct _CLOUD_FILE_INFO
{
	char strFilename[260];
	int idxScene;
	float off_x,off_y,off_z;
}CloudFileInfo;

class CELCloudCreator  
{
public:   
	static bool SaveMergeFile(const char* strMergeFile,int nRow,int nCol,abase::vector<CloudFileInfo>& vCloudFileInfo,int nMajorVersion,int nMinorVersion);
	void GetBoxFileInfo(abase::vector<CloudBox>** pBoxes);
	bool LoadRenderParamFile(const char* strFilename);
	bool SaveRenderParamFile(const char* strFilename);
	void GetCloudFileVersion(WORD& wMajor,WORD& wMinor);
	void SetBoxSeed(int idxBox,unsigned int nSeed);
	void ChangeSpriteSize(int idxGroup,float fSpriteSize);
	void ClearBoxInfo();
	void SetCloudHeight(float fHeight);
	bool SaveBoxFile(const char* strFilename);
	void AddBoxInfo(const CloudBox& boxInfo);
	//recreate cloud sprites in the group,nSprites: new Sprites number
	void ReCreateCloud_Group(int idxGroup,int nSprites);	
	void Release();
	//load cloud scene file(*.sce)
	bool LoadCloud(const char* strFilename);	
	//save cloud scene file(*.sce)
	// idx: idx in all scene files, [1..n], base on row  
	bool SaveCloud(const char* strFilename);
	// get cloud group and cloud sprites info
	void GetCloudInfo(abase::vector<CloudSprite>** pSprites,abase::vector<CloudGroup>** pGroups);
	//load box info from file (*.txt)
	bool LoadBoxFile(const char* strFilename);

	bool CreateCloud();
	//params for create cloud
	void SetCloudCreatorParam(CloudCreatorParam* pParam);
	CloudCreatorParam* GetCloudCreatorParam();

	CELCloudCreator();
	~CELCloudCreator();

private: 
	void ClearSpritesNoGroup(int idxGroup);
	static void OffsetCloudInfo(BYTE* pBuffer,float off_x,float off_y,float off_z);
	int CalculateCloudFileLength();
	void LoadCloud_Version_1_0(AFileImage* pFile);
	void LoadBoxFile_Version_1_0(AFileImage* pFile);
	void ReAssignLevelToSprites_Group(int idxGroup);
	void ReShadingCloudSprite_Group(int idxGroup);
	void ReCreateSprites_Group(int idxGroup,int nSprites);
	
	void AssignLevelToSprites();
	void ShadingCloudSprite();
	float CalculateDistance(A3DVECTOR3 v1,A3DVECTOR3 v2);
	float Random(float max,float min);
	void CreateGroup();
	void CreateCloudSprite();
	CloudCreatorParam m_pmCloudCreator;

	float m_fRandInv;				
	//box info
	abase::vector<CloudBox> m_vBoxes;		//boxinfo
	abase::vector<CloudSprite> m_vSprites;	//sprite info
	abase::vector<CloudGroup> m_vGroups;	//group inf

	int m_nLevel;						//level,now is 10

	WORD m_wBoxMajorVersion,m_wBoxMinorVersion;			//VERSION info
	WORD m_wCloudMajorVersion,m_wCloudMinorVersion;

	//temp struct, for calculation of group center
 	typedef struct _TEMP_POS
	{
		A3DVECTOR3 max;
		A3DVECTOR3 min;

		_TEMP_POS()
		{
			max.Set(-10000,-10000,-10000);
			min.Set(10000,10000,10000);
		};
	}TempPos;
};

