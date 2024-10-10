/*
 * FILE: A3DLightMan.h
 *
 * DESCRIPTION: The managing class of lights;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLIGHTMAN_H_
#define _A3DLIGHTMAN_H_

#include "A3DTypes.h"
#include "AList.h"

class A3DDevice;
class A3DLight;

class A3DLightMan
{
private:
	AList		m_LightList;
	A3DDevice * m_pA3DDevice;

protected:

public:
	A3DLightMan();
	virtual ~A3DLightMan();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool Reset();
	bool RestoreLightResource(); // Restore all light resource when transition from lost state to operational state;

	bool CreateLight(int nLightID, A3DLight ** ppA3DLight);
	bool CreateDirecionalLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient);
	bool CreateSpotLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient, FLOAT range, FLOAT falloff, FLOAT theta, FLOAT phi, FLOAT attenuation);
	bool CreatePointLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient, FLOAT range, FLOAT attenuation);
	bool SetAmbient(A3DCOLOR colorAmbient);

	bool ReleaseLight(A3DLight*& pA3DLight);

	inline AList * GetLightList() { return &m_LightList; }
};

typedef A3DLightMan * PA3DLightMan;
#endif