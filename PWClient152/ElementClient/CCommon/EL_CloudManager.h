// CloudManager.h: interface for the CCloudManager class.
// control loading of cloud files 
// yy created on 2005.07.05
//////////////////////////////////////////////////////////////////////

#pragma once
#include "EL_Cloud.h"
 
class CELCloudManager  
{
public:
	//change sprite color, final vertex color = V.Diffuse * clrAmb + Dot * clrSun;
	void SetSpriteColor(const A3DCOLORVALUE& clrAmb,const A3DCOLORVALUE& clrSun);
	void SetRenderRange(float fRenderRange);	//max distance between camera and rendered cloud
	void SetCloudLevel(int nLevel);		//nLevel [1,10]  
	void Release();
	//strCloudFilename: all cloud scenes in one big file(*.cld)
	bool Init(A3DDevice* pA3DDevice,const char* strCloudFilename);
	void Render(A3DViewport* pViewport);
	//clrTime: color changed with time
	void Tick(int nDeltaTime,const A3DVECTOR3& vSunPos);
	CELCloudManager();
	~CELCloudManager();

private:
	void ReFillStream(A3DViewport* pViewport);
	bool LoadCloudFiles(const char* strFilename);
	void CalculateSceneRendered(A3DViewport* pViewport);				//find which scene will be render
	void CalculateSceneIndex(const A3DVECTOR3& cameraPos,int& idx_x,int& idx_z);

	int m_nScenes;						//scenes number,like 11*8
	int m_nSceneRow;					//scene number in one row 
	int m_nSceneCol;					//scene number in one col

	A3DVECTOR3* m_vSceneCenterPos;		//center pos of every scenes in big cloud file(*.cld)
	A3DVECTOR3 m_vOldCameraPos;			//use for calculate scene index for rendering
	abase::vector<int> m_vSceneRender;	//scenes in render range 
	float m_fRenderRange;					

	A3DStream* m_pStream;
	int m_nStreamVertices;				//vertices number in stream
	int m_nStreamSprites;				//spirtes number in stream

	A3DDevice* m_pA3DDevice; 
	A3DTexture* m_pTexture;
 
	CELCloud** m_pClouds;					//clouds file 
};

