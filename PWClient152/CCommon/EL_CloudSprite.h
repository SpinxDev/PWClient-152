// CloudSprite.h: interface for the CCloudSprite class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "a3dstream.h"
#include "A3DViewport.h"
#include "A3DDevice.h"
#include "a3dTexture.h"
#include "A3DCamera.h"
#include "vector.h"
   
class CELCloudSprite  
{
public:
	static A3DTLVERTEX* m_pVertexBuffer;	 
	static A3DCOLORVALUE m_clrSun,m_clrAmb;	//amb color and sun color from CCloudManager
   
	bool Update(int nDeltaTime);
	void ReFillStream(A3DViewport* pViewport,int idxStreamPos,float fSpriteSize);	//fill stream in CCloudManager	
	int GetCloudLevel();		
 
	//idxTexture : [1,16] little textures in texture file
	//nLevel:	CloudLevel[1,10] 
	void SetSpriteInfo(const A3DVECTOR3& vCenterPos,int idxTexture,int nLevel,DWORD dwColor[4]);
	void SetAlphaFromGroup(float fAlpha);	//alpha value changed with distance between camera and group center 
	void SetGroupCenterPos(const A3DVECTOR3& vGroupCenter);
	void ReShading(A3DViewport *pViewport,float fSpriteSize);						//calculate vertex color
	//void SetTimeColor(const A3DCOLOR& vTimeColor);	
	void SetGroupToSunVector(const A3DVECTOR3& vGroupToSun);
	
	void Init(A3DDevice* pA3DDevice);
	
	void SetGroup(UINT nGroup);		//group index that sprite belong to
	UINT GetGroup();						

	A3DVECTOR3 GetCenterPos();
	void SetTexture(int idxTexture);

	CELCloudSprite();
	~CELCloudSprite();
	
private:
	A3DVECTOR3 CalculateSpritePos(float& fAlpha);
	void FillStream(A3DViewport* pViewport, int idxStreamPos,float fAlpha,A3DVECTOR3 v0,A3DVECTOR3 v3);

	A3DDevice* m_pA3DDevice;
	
	A3DVECTOR3 m_vCenterPos;	//center pos of this sprites
	A3DVECTOR3 m_vGroupCenterPos;	//group center pos 
	A3DVECTOR3 m_vGroupToSun;		//

	UINT m_nGroup;				//which group this sprites belong to
	A3DCOLOR m_vAmbColor[4];	//amb color info in mem
	A3DCOLORVALUE m_vResultColor[4];
	float m_fTextureU[4],m_fTextureV[4];
	float m_fAlpha;				//alpha channel from group  
	int m_nCloudLevel;

	//
	A3DVECTOR3 m_vMovedPos;		//moved pos of center
	float m_fMoveDist;			//move distance along dir
	DWORD m_dwLastTime;			
	DWORD m_dwHoldTime;
	DWORD m_dwStepTime;
	bool m_bFarAway;
}; 

 