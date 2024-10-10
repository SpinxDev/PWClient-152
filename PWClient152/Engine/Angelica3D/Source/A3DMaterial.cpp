/*
 * FILE: A3DMaterial.cpp
 *
 * DESCRIPTION: Class that representing one material;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMaterial.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DDevice.h"
#include "AFile.h"
#include "AAssist.h"

//	== operator
bool operator == (const A3DMaterial& m1, const A3DMaterial& m2)
{
	const A3DMATERIALPARAM& p1 = m1.GetMaterialParam();
	const A3DMATERIALPARAM& p2 = m2.GetMaterialParam();
	
	if (p1.Ambient != p2.Ambient)
		return false;

	if (p1.Diffuse != p2.Diffuse)
		return false;

	if (p1.Specular != p2.Specular)
		return false;

	if (p1.Emissive != p2.Emissive)
		return false;

	if (p1.Power != p2.Power)
		return false;

	if (m1.m_b2Sided != m2.m_b2Sided)
		return false;

	return true;
}

A3DMaterial::A3DMaterial()
{
	ZeroMemory(&m_MaterialParam, sizeof(A3DMATERIALPARAM));
	m_b2Sided = false;
	m_bAlphaMaterial = false;
	m_bHasBloomEffect = false;
	m_pA3DDevice = NULL;
}

A3DMaterial::~A3DMaterial()
{
}

A3DMaterial::A3DMaterial(const A3DMaterial& src)
{
	m_pA3DDevice		= src.m_pA3DDevice;
	m_MaterialParam		= src.m_MaterialParam;
	m_b2Sided			= src.m_b2Sided;
	m_bAlphaMaterial	= src.m_bAlphaMaterial;
	m_bHasBloomEffect	= src.m_bHasBloomEffect;
}
							
bool A3DMaterial::Init(A3DDevice * pDevice)
{
	m_pA3DDevice = pDevice;

	//	Set to a default material;
	m_MaterialParam.Ambient.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_MaterialParam.Diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_MaterialParam.Emissive.Set(0.0f, 0.0f, 0.0f, 0.0f);
	m_MaterialParam.Specular.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_MaterialParam.Power = 0.0f;

	m_b2Sided = false;
	m_bAlphaMaterial = false;

	return true;
}

bool A3DMaterial::Init(A3DDevice * pDevice, const A3DCOLORVALUE& clAmbient, const A3DCOLORVALUE& clDiffuse, 
					   const A3DCOLORVALUE& clEmissive, const A3DCOLORVALUE& clSpecular, float fShininess)
{
	m_pA3DDevice = pDevice;
	SetValues(clAmbient, clDiffuse, clEmissive, clSpecular, fShininess);
	return true;
}

bool A3DMaterial::Init(A3DDevice * pDevice, const A3DMATERIALPARAM& Params)
{
	m_pA3DDevice	= pDevice;
	m_MaterialParam	= Params;

	m_b2Sided = false;
	m_bAlphaMaterial = false;

	a_Clamp(m_MaterialParam.Power, -128.0f, 128.0f);

	return true;
}

void A3DMaterial::SetValues(const A3DCOLORVALUE& clAmbient, const A3DCOLORVALUE& clDiffuse,
							const A3DCOLORVALUE& clEmissive, const A3DCOLORVALUE& clSpecular, 
							float fShininess, bool b2Sided)
{
	m_MaterialParam.Ambient		= clAmbient;
	m_MaterialParam.Diffuse		= clDiffuse;
	m_MaterialParam.Emissive	= clEmissive;
	m_MaterialParam.Specular	= clSpecular;
	m_MaterialParam.Power		= fShininess;

	a_Clamp(m_MaterialParam.Power, -128.0f, 128.0f);

	if( m_MaterialParam.Diffuse.a < 1.0f )
		m_bAlphaMaterial = true;
	else
		m_bAlphaMaterial = false;

	m_b2Sided = b2Sided;
}

bool A3DMaterial::Release()
{
	return true;
}

bool A3DMaterial::Appear() const
{
	if (!m_pA3DDevice)
		return true;

	m_pA3DDevice->SetMaterial(&m_MaterialParam, m_b2Sided);
	return true;
}

bool A3DMaterial::Save(AFile * pFileToSave)
{
	if( pFileToSave->IsBinary() )
	{
		DWORD		dwWriteLength;
		char		szLineBuffer[2048];

		sprintf(szLineBuffer, "MATERIAL: %s", GetName());
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);

		pFileToSave->Write(&m_MaterialParam.Ambient, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Diffuse, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Emissive, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Specular, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Power, sizeof(FLOAT), &dwWriteLength);
		pFileToSave->Write(&m_b2Sided, sizeof(bool), &dwWriteLength);
	}
	else
	{			 
		char szLineBuffer[2048];
		sprintf(szLineBuffer, "MATERIAL: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Ambient:(%f, %f, %f, %f)", m_MaterialParam.Ambient.r, m_MaterialParam.Ambient.g, m_MaterialParam.Ambient.b, m_MaterialParam.Ambient.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Diffuse:(%f, %f, %f, %f)", m_MaterialParam.Diffuse.r, m_MaterialParam.Diffuse.g, m_MaterialParam.Diffuse.b, m_MaterialParam.Diffuse.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Emissive:(%f, %f, %f, %f)", m_MaterialParam.Emissive.r, m_MaterialParam.Emissive.g, m_MaterialParam.Emissive.b, m_MaterialParam.Emissive.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Specular:(%f, %f, %f, %f)", m_MaterialParam.Specular.r, m_MaterialParam.Specular.g, m_MaterialParam.Specular.b, m_MaterialParam.Specular.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Power:(%f)", m_MaterialParam.Power);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "2Sided:(%d)", m_b2Sided);
		pFileToSave->WriteLine(szLineBuffer);

		//<== "MATERIAL: {"
		pFileToSave->WriteLine("}");
	}
	return true;
}

bool A3DMaterial::Load(A3DDevice * pDevice, AFile * pFileToLoad)
{
	if( !Init(pDevice) )
		return false;

	if( pFileToLoad->IsBinary() )
	{
		DWORD		dwReadLength;
		char		szLineBuffer[2048];
		char		szResult[2048];

		pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		a_GetStringAfter(szLineBuffer, "MATERIAL: ", szResult);
		SetName(szResult);

		pFileToLoad->Read(&m_MaterialParam.Ambient, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Diffuse, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Emissive, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Specular, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Power, sizeof(FLOAT), &dwReadLength);
		pFileToLoad->Read(&m_b2Sided, sizeof(bool), &dwReadLength);
	}
	else
	{
		char szLineBuffer[AFILE_LINEMAXLEN];
		char szResult[AFILE_LINEMAXLEN];
		int  nValue;
		DWORD dwReadLen;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "MATERIAL: ", szResult);
		SetName(szResult);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Ambient:(%f, %f, %f, %f)", &m_MaterialParam.Ambient.r, &m_MaterialParam.Ambient.g, &m_MaterialParam.Ambient.b, &m_MaterialParam.Ambient.a);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Diffuse:(%f, %f, %f, %f)", &m_MaterialParam.Diffuse.r, &m_MaterialParam.Diffuse.g, &m_MaterialParam.Diffuse.b, &m_MaterialParam.Diffuse.a);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Emissive:(%f, %f, %f, %f)", &m_MaterialParam.Emissive.r, &m_MaterialParam.Emissive.g, &m_MaterialParam.Emissive.b, &m_MaterialParam.Emissive.a);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Specular:(%f, %f, %f, %f)", &m_MaterialParam.Specular.r, &m_MaterialParam.Specular.g, &m_MaterialParam.Specular.b, &m_MaterialParam.Specular.a);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Power:(%f)", &m_MaterialParam.Power);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);

		sscanf(szLineBuffer, "2Sided:(%d)", &nValue);
		m_b2Sided = nValue ? true : false;
		
		//<== "MATERIAL: {"
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;
	}

	//	Test code...
//	m_MaterialParam.Specular.Set(1.0f, 1.0f, 1.0f, 1.0f);
/*	m_MaterialParam.Diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_MaterialParam.Ambient.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_MaterialParam.Emissive.Set(0.0f, 0.0f, 0.0f, 0.0f);
	m_MaterialParam.Specular.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_MaterialParam.Power = 10.0f;
*/	
	a_Clamp(m_MaterialParam.Power, -128.0f, 128.0f);

	return true;
}

//	= operator
const A3DMaterial& A3DMaterial::operator = (const A3DMaterial& mtl)
{
	m_pA3DDevice		= mtl.m_pA3DDevice;
	m_MaterialParam		= mtl.m_MaterialParam;
	m_b2Sided			= mtl.m_b2Sided;
	m_bAlphaMaterial	= mtl.m_bAlphaMaterial;
	m_bHasBloomEffect	= mtl.m_bHasBloomEffect;	
	return *this;
}

//	Sacle values
void A3DMaterial::ScaleValues(const A3DCOLORVALUE& s, bool bClamp)
{
	m_MaterialParam.Diffuse	*= s;
	m_MaterialParam.Specular *= s;
	m_MaterialParam.Ambient	*= s;
	m_MaterialParam.Emissive *= s;

	if (bClamp)
	{
		m_MaterialParam.Diffuse.Clamp();
		m_MaterialParam.Specular.Clamp();
		m_MaterialParam.Ambient.Clamp();
		m_MaterialParam.Emissive.Clamp();
	}
}


