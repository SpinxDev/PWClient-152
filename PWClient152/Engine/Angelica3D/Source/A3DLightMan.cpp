#include "A3DLightMan.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DLight.h"
#include "A3DDevice.h"

A3DLightMan::A3DLightMan()
{
	m_pA3DDevice = NULL;
}

A3DLightMan::~A3DLightMan()
{
}

bool A3DLightMan::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;

	if( !m_LightList.Init() )
	{
		g_A3DErrLog.Log("A3DLightMan::Init() Init Light List Fail");
		return false;
	}

	//Set the default ambient;
	SetAmbient(A3DCOLORRGBA(32, 32, 32, 255));
	return true;
}

bool A3DLightMan::Release()
{
	m_LightList.Release();
	return true;
}

bool A3DLightMan::Reset()
{
	m_LightList.Reset();
	return true;
}

bool A3DLightMan::CreateLight(int nLightID, A3DLight ** ppA3DLight)
{
	A3DLight * pLight = new A3DLight();
	if( NULL == pLight )
	{
		g_A3DErrLog.Log("A3DLightMan::CreateLight() Not enough memory!");
		return false;
	}

	if( !pLight->Init(m_pA3DDevice, nLightID) )
	{
		g_A3DErrLog.Log("A3DLightMan::CreateLight() Init Light Fail!");
		return false;
	}

	m_LightList.Append((LPVOID) pLight);

	*ppA3DLight = pLight;
	return true;
}

bool A3DLightMan::SetAmbient(A3DCOLOR colorAmbient)
{
	m_pA3DDevice->SetAmbient(colorAmbient);
	return true;
}

bool A3DLightMan::CreateDirecionalLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient)
{
	A3DLight * pNewLight;
	A3DLIGHTPARAM param;

	ZeroMemory(&param, sizeof(param));
	param.Type = A3DLIGHT_DIRECTIONAL;
	param.Position = vecPos;
	param.Direction = Normalize(vecDir);
	param.Diffuse = diffuse;
	param.Specular = specular;
	param.Ambient = ambient;

	if( !CreateLight(nLightID, &pNewLight) )
	{
		g_A3DErrLog.Log("A3DLightMan::CreateDirectoinalLight() Create Light Fail!");
		return false;
	}

	pNewLight->SetLightParam(param);

	pNewLight->TurnOn();
	*ppA3DLight = pNewLight;
	return true;
}

bool A3DLightMan::CreateSpotLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, 
		A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient, 
		FLOAT range, FLOAT falloff, FLOAT theta, FLOAT phi, FLOAT attenuation)
{
	A3DLight * pNewLight;
	A3DLIGHTPARAM param;

	ZeroMemory(&param, sizeof(param));
	param.Type = A3DLIGHT_SPOT;
	param.Position = vecPos;
	param.Direction = vecDir;
	param.Diffuse = diffuse;
	param.Specular = specular;
	param.Ambient = ambient;
	param.Range = range;
	param.Falloff = falloff;
	param.Theta = theta;
	param.Phi = phi;
	param.Attenuation0 = attenuation;
	//param.Attenuation1 = attenuation1;
	//param.Attenuation2 = attenuation2;

	if( !CreateLight(nLightID, &pNewLight) )
	{
		g_A3DErrLog.Log("A3DLightMan::CreateDirectoinalLight() Create Light Fail!");
		return false;
	}

	pNewLight->SetLightParam(param);
	pNewLight->TurnOn();

	*ppA3DLight = pNewLight;
	return true;
}

bool A3DLightMan::CreatePointLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient, FLOAT range, FLOAT attenuation)
{
	A3DLight * pNewLight;
	A3DLIGHTPARAM param;

	ZeroMemory(&param, sizeof(param));
	param.Type = A3DLIGHT_POINT;
	param.Diffuse = diffuse;
	param.Specular = specular;
	param.Ambient = ambient;
	param.Position = vecPos;
	param.Range = range;
	param.Attenuation0 = 1.0f;
	param.Attenuation1 = attenuation;
	param.Attenuation2 = 0.0f;	

	if( !CreateLight(nLightID, &pNewLight) )
	{
		g_A3DErrLog.Log("A3DLightMan::CreatePointLight() Create Light Fail!");
		return false;
	}

	pNewLight->SetLightParam(param);

	pNewLight->TurnOn();
	*ppA3DLight = pNewLight;
	return true;
}

bool A3DLightMan::ReleaseLight(A3DLight*& pA3DLight)
{
	ALISTELEMENT * pLightElement = m_LightList.FindElementByData((LPVOID) pA3DLight);
	if( NULL == pLightElement ) return false;
	m_LightList.Delete(pLightElement);

	pA3DLight->Release();
	delete pA3DLight;
	pA3DLight = NULL;
	return true;
}

bool A3DLightMan::RestoreLightResource()
{
	ALISTELEMENT * pLightElement = m_LightList.GetFirst();

	while( pLightElement != m_LightList.GetTail() )
	{
		A3DLight * pLight = (A3DLight *)pLightElement->pData;

		pLight->TurnOn();

		pLightElement = pLightElement->pNext;
	}

	return true;
}

