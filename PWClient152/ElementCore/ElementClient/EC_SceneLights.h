/*
 * FILE: EC_SceneLights.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/12/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_SCENELIGHTS_H_
#define _EC_SCENELIGHTS_H_

#include <A3DTypes.h>
#include <A3DFuncs.h>
#include <vector.h>

class A3DDevice;

using namespace abase;

#define SCENELIGHT_NIGHT_ONLY		0x1

class CECSceneLights
{
public:
	struct SCENELIGHT
	{
		A3DVECTOR3		vecPos;
		float			vRange;
		float			vAttenuation0;
		float			vAttenuation1;
		float			vAttenuation2;
		A3DCOLORVALUE	diffuse;
		A3DCOLORVALUE	ambient;
		A3DAABB			aabb;
		DWORD			flags;
	};

protected:
	struct SCENELIGHTBLOCK
	{
		vector<int>				lights;						// id of lights in this block
		vector<A3DLIGHTPARAM *>	dynamicLights;				// dynamic lights in this block
	};

private:
	A3DDevice *		m_pA3DDevice;

	SCENELIGHT *	m_pStaticLights;
	int				m_nNumStaticLights;

	A3DAABB				m_BlocksAABB;				// bounding box of all blocks.
	float				m_vBlockSize;				// size of each block
	int					m_nBlockDim;				// dimension of blocks.
	A3DVECTOR3			m_vecBlocksCenter;			// center position of all light blocks.
	SCENELIGHTBLOCK *	m_pLightBlocks;				// all light blocks currently in scene

	float				m_fDNFactor;				// day night factor, 0.0 means day, 1.0 mean night
public:
	CECSceneLights();
	~CECSceneLights();

	bool Init(A3DDevice * pA3DDevice, float vBlockSize=12.0f, int nBlockDim=10);

	bool Load(const char * szLightFile);
	bool Release();

	bool Update(DWORD dwDeltaTime, float fDNFactor, const A3DVECTOR3& vecCenter);
	bool GetSceneLight(const A3DVECTOR3& vecPos, SCENELIGHT& light);
};

#endif//_EC_SCENELIGHTS_H_

