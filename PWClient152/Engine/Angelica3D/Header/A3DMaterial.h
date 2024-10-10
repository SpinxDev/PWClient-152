/*
 * FILE: A3DMaterial.h
 *
 * DESCRIPTION: Class that representing one material;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMATERIAL_H_
#define _A3DMATERIAL_H_

#include "A3DTypes.h"
#include "A3DObject.h"

class A3DDevice;

class A3DMaterial : public A3DObject
{
private:
	A3DMATERIALPARAM	m_MaterialParam;
	bool				m_b2Sided;
	bool				m_bAlphaMaterial;
	bool				m_bHasBloomEffect;
	
	A3DDevice *			m_pA3DDevice;
public:
	A3DMaterial();
	A3DMaterial(const A3DMaterial& src);
	~A3DMaterial();

	bool Init(A3DDevice* pDevice);
	bool Init(A3DDevice* pDevice, const A3DCOLORVALUE& clAmbient, const A3DCOLORVALUE& clDiffuse,
		const A3DCOLORVALUE& clEmissive, const A3DCOLORVALUE& clSpecular, float fShiness);
	bool Init(A3DDevice* pDevice, const A3DMATERIALPARAM& Params);
	bool Release();
	bool Appear() const;

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pDevice, AFile * pFileToLoad);
	
	//	== operator
	friend bool operator == (const A3DMaterial& m1, const A3DMaterial& m2);

	//	= operator
	const A3DMaterial& operator = (const A3DMaterial& mtl);

	//	Set material parameters
	void SetMaterialParam(const A3DMATERIALPARAM& Param) { m_MaterialParam = Param; }
	void SetValues(const A3DCOLORVALUE& clAmbient, const A3DCOLORVALUE& clDiffuse, 
		const A3DCOLORVALUE& clEmissive, const A3DCOLORVALUE& clSpecular, float fShininess, bool b2Sided=false);
	void SetDiffuse(float r, float g, float b, float a) { m_MaterialParam.Diffuse.Set(r, g, b, a); }
	void SetDiffuse(const A3DCOLORVALUE& col) { m_MaterialParam.Diffuse = col; }
	void SetAmbient(float r, float g, float b, float a) { m_MaterialParam.Ambient.Set(r, g, b, a); }
	void SetAmbient(const A3DCOLORVALUE& col) { m_MaterialParam.Ambient = col; }
	void SetSpecular(float r, float g, float b, float a) { m_MaterialParam.Specular.Set(r, g, b, a); }
	void SetSpecular(const A3DCOLORVALUE& col) { m_MaterialParam.Specular = col; }
	void SetEmissive(float r, float g, float b, float a) { m_MaterialParam.Emissive.Set(r, g, b, a); }
	void SetEmissive(const A3DCOLORVALUE& col) { m_MaterialParam.Emissive = col; }
	void SetPower(float fPower) { m_MaterialParam.Power = fPower; }
	void SetBloomEffect(bool bHasBloomEffect) { m_bHasBloomEffect = bHasBloomEffect; }

	//	Sacle values
	void ScaleValues(const A3DCOLORVALUE& s, bool bClamp);

	//	Get material parameters
	const A3DMATERIALPARAM& GetMaterialParam() const { return m_MaterialParam; }
	const A3DCOLORVALUE& GetDiffuse() const { return m_MaterialParam.Diffuse; }
	const A3DCOLORVALUE& GetSpecular() const { return m_MaterialParam.Specular; }
	const A3DCOLORVALUE& GetAmbient() const { return m_MaterialParam.Ambient; }
	const A3DCOLORVALUE& GetEmissive() const { return m_MaterialParam.Emissive; }
	float GetPower() const { return m_MaterialParam.Power; }

	void Set2Sided(bool b2Sides) { m_b2Sided = b2Sides; }
	void SetAlphaMaterial(bool bAlpha) { m_bAlphaMaterial = bAlpha; }
	bool Is2Sided() const { return m_b2Sided; }
	bool IsAlphaMaterial() const { return m_bAlphaMaterial; }
	bool HasBloomEffect() const { return m_bHasBloomEffect; }

	bool Match(A3DMaterial* pMaterial) const
	{
		if (!pMaterial)
			return false;

		return (*this == *pMaterial); 
	}
};


#endif