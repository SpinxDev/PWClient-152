// Cloud.h: interface for the CCloud class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "EL_CloudSprite.h"

class CELCloud  
{
public:
	void Render(A3DViewport* pViewport);
	bool Tick(int nDeltaTime);
	static int m_nCloudLevel;
	static A3DVECTOR3 m_vSunPos;		//sun pos in world space
	static float m_fRenderValve;			//square of render range

	int GetRenderSpritesNum();			//sprite number for render
	bool LoadCloud(BYTE* pBuffer);		//load cloud from file buffer
	void ReFillStream(A3DViewport* pViewport,int idxStreamPos,int idxSprite,int nSprites);	// fill stream in CCloudmanager
	void ChangeCloudLevel();			
	void CalculateRenderSeq(A3DViewport* pViewport);			// calcualte which group to render

	void Init(A3DDevice* pA3DDevice,A3DTexture* pTexture);
	void Release();
			
	CELCloud();
	~CELCloud();

private:
	void ReCalculateSpriteSize();
	float CalculateDistance(const A3DVECTOR3& pos1,const A3DVECTOR3& pos2);
	
	typedef struct _CLOUD_GROUP
	{
		A3DVECTOR3 vCenterPos;			//origin center pos of Group,use for SunLight Shading and render sequence
		abase::vector<int> vSprites;	//index of sprites in this group and render sequence
		bool bRender;					//be rendered?
		A3DVECTOR3 vGroupToSun;			//group to sun vector
		float fSpriteSize;				//size of sprite in the group
		float fRenderSize;				//sprite size when render by defferent size
	}CloudGroup;
	
	abase::vector<CloudGroup> m_vGroups;
	abase::vector<CELCloudSprite*> m_vSprites;	
	abase::vector<int> m_vRenderSeq;			//render sequence of sprites, from back to front
		
	A3DDevice* m_pA3DDevice;  
	A3DTexture* m_pTexture;			//texture
	
	//bool m_bChangeCloudLevel;		//change cloud level?
	float m_fOldTime;				//last time to calculate color
};

