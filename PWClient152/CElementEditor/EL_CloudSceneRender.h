// ELCloudSceneRender.h: interface for the CELCloudSceneRender class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "vector.h"
#include "EL_CloudCreator.h"
#include "EL_CloudSprite.h"

class CELCloudSceneRender   
{
public:
	void SetGroupSelected(int idxGroup);
	void SetRenderLine(bool bRender);
	void SetSpriteColor(const A3DCOLORVALUE& clrAmb,const A3DCOLORVALUE& clrSun);
	void SetRenderRange(float fRenderRange);	//max distance between camera and rendered cloud
	void SetCloudLevel(int nLevel);		//nLevel [1,10]  

	void Release();
	void SetCloudInfo(CloudCreatorParam* pParam,abase::vector<CloudSprite>& vSprites,abase::vector<CloudGroup>& vGroups);
	void Render(A3DViewport* pViewport);
	void Tick(int nDeltaTime,const A3DVECTOR3& vSunPos);
	bool Init(A3DDevice* pA3DDevice,const char* strTextureFile);
	CELCloudSceneRender(); 
	virtual ~CELCloudSceneRender();

private:
	void RenderStream_Line();
	void ReFillStream_Line();
	void ReCalculateSpriteSize();  
	float CalculateDistance(const A3DVECTOR3& v1,const A3DVECTOR3& v2);
	void ReFillStreamFromSprite(A3DViewport* pViewport,int idxStreamPos,int idxSprite,int nSprites);
	void ReFillStream(A3DViewport* pViewport);
	void CalculateRenderSeq(A3DViewport* pViewport);
	void CalculateCloudColor(A3DViewport* pViewport);
	int m_nStreamVertices;
	int m_nStreamSprites;

	A3DDevice* m_pA3DDevice;
	A3DTexture* m_pTexture;
	A3DStream* m_pStream;
	A3DStream* m_pStreamLine;
	bool m_bRenderLine;

	CloudCreatorParam* m_pmCloudCreator;
	abase::vector<CELCloudSprite*> m_vSprites;
	abase::vector<CloudGroup>* m_pGroups;
	abase::vector<int> m_vRenderSeq;

	A3DVECTOR3 m_vSunPos;
	float m_fRenderValve;
	int m_nCloudLevel;
};

