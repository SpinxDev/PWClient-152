#include "A3DLight.h"
#include "A3DFuncs.h"
#include "A3DDevice.h"

A3DLight::A3DLight()
{
	m_bOn = false;
}

A3DLight::~A3DLight()
{
}

bool A3DLight::Init(A3DDevice * pDevice, int nLightID)
{
	m_pA3DDevice = pDevice;
	m_bOn = false;
	m_nLightID = nLightID;

	TurnOff();
	return true;
}

bool A3DLight::Release()
{
	TurnOff();

	return true;
}

bool A3DLight::TurnOn()
{
	m_bOn = true;

	m_pA3DDevice->SetLightParam(m_nLightID, &m_LightParam);
	m_pA3DDevice->LightEnable(m_nLightID, true);
	return true;
}

bool A3DLight::TurnOff()
{
	m_bOn = false;
	m_pA3DDevice->LightEnable(m_nLightID, false);
	return true;
}

bool A3DLight::SetLightParam(const A3DLIGHTPARAM& LightParam)
{
	m_LightParam = LightParam;
	m_Type = m_LightParam.Type;

	m_pA3DDevice->SetLightParam(m_nLightID, &m_LightParam);
	return true;
}
