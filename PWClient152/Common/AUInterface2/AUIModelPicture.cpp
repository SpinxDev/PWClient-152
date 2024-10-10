// Filename	: AUIModelPicture.cpp
// Creator	: Tom Zhou
// Date		: July 27, 2004
// Desc		: AUIModelPicture is the class of model display control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A3DLightMan.h"
#include "A3DLight.h"
#include "A3DSkinModelAct.h"
#include "A3DCamera.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "AUIManager.h"
#include "AUIDialog.h"
#include "AUIModelPicture.h"
#include "AUICommon.h"

AUIModelPicture::AUIModelPicture() : AUIPicture()
{
	m_nType = AUIOBJECT_TYPE_MODELPICTURE;
	m_pDirLight = NULL;
	m_pA3DModel = NULL;
	m_pA3DCamera = NULL;
	m_pA3DViewport = NULL;
	m_dwTime = a_GetTime();
	m_bAcceptMouseMsg = false;
}

AUIModelPicture::~AUIModelPicture()
{
}

bool AUIModelPicture::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Model Picture property line format:
	 *
	 *		MODELPICTURE		Keyword
	 *		Character			Name
	 *		""					String ID of command.
	 *		0 0 150 150			x y Width Height
	 *		"Woman.smd"			ModelFileName
	 *		64 64 64			Background color
	 */
	

	if (!AUIPicture::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Init(), failed to call base AUIPicture::Init()");

	if( !m_pDirLight )
	{
		if (!m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(
				1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
				A3DVECTOR3(0.0f, -0.7f, 1.0f),
				A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f),
				A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f),
				A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f)))
			return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Init(), failed to call m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight()");
	}
	if( !m_pA3DCamera )
	{
		m_pA3DCamera = new A3DCamera;
		if( !m_pA3DCamera ) return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Init(), no enough memory");

		if (!m_pA3DCamera->Init(m_pA3DDevice, DEG2RAD(56.0f), 0.1f, 2000.0f))
			return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Init(), failed to call m_pA3DCamera->Init()");

		float fRatio = float(m_nWidth) / float(m_nHeight);
		m_pA3DCamera->SetProjectionParam(DEG2RAD(56.0f), 0.1f, 2000.0f, fRatio);

		m_pA3DCamera->SetPos(A3DVECTOR3(0.0f, 0.0f, 0.0f));
		m_pA3DCamera->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	}

	if( pASF )
	{
		BEGIN_FAKE_WHILE2;

		CHECK_BREAK2(pASF->GetNextToken(true));

		// Maybe no model.
		if( strlen(pASF->m_szToken) > 0 )
		{
			if (!InitIndirect(pASF->m_szToken, 0))
				return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Init(), failed to call InitIndirect()");
		}

		// Background color.
		CHECK_BREAK2(pASF->GetNextToken(true));
		int r = atoi(pASF->m_szToken);

		CHECK_BREAK2(pASF->GetNextToken(true));
		int g = atoi(pASF->m_szToken);

		CHECK_BREAK2(pASF->GetNextToken(true));
		int b = atoi(pASF->m_szToken);

		m_color = A3DCOLORRGB(r, g, b);

		END_FAKE_WHILE2;

		BEGIN_ON_FAIL_FAKE_WHILE2;

		AUI_ReportError(DEFAULT_2_PARAM, "AUIModelPicture::Init(), failed to read from file");

		END_ON_FAIL_FAKE_WHILE2;
	}

	if( !m_pA3DViewport )
	{
		A3DRECT rc = GetRect();

		if (!m_pA3DDevice->CreateViewport(&m_pA3DViewport, rc.left,
			rc.top, rc.right, rc.bottom, 0.0f, 1.0f, true, true, m_color))
			return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Init(), failed to call m_pA3DDevice->CreateViewport()");

		m_pA3DViewport->SetCamera(m_pA3DCamera);
	}

	return true;
}

bool AUIModelPicture::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	if( strlen(pszFileName) <= 0 ) return true;

	A3DRELEASE(m_pA3DModel);

	m_pA3DModel = new A3DAlterableSkinModel;
	if( !m_pA3DModel ) return false;

	bval = m_pA3DModel->Init(m_pA3DEngine);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIModelPicture::InitIndirect(), failed to call m_pA3DModel->Init()");

	bval = m_pA3DModel->Load(pszFileName);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIModelPicture::InitIndirect(), failed to call m_pA3DModel->Load()");

	const A3DLIGHTPARAM& LightParams = m_pDirLight->GetLightparam();
	A3DSkinModel::LIGHTINFO LightInfo;
	LightInfo.colAmbient	= m_pA3DDevice->GetAmbientValue();
	LightInfo.vLightDir		= LightParams.Direction;	//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
	LightInfo.bPtLight		= false;					//	false, disable dynamic point light,
	m_pA3DModel->SetLightInfo(LightInfo);

	m_pA3DModel->SetPos(A3DVECTOR3(0.0f, 0.0f, 0.0f));
	m_pA3DModel->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, -1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pA3DModel->Update(0);

	A3DVECTOR3 vecPosCam;
	const A3DAABB& aabb = m_pA3DModel->GetModelAABB();
	vecPosCam.x = aabb.Center.x;
	vecPosCam.y = aabb.Center.y;
	vecPosCam.z = -aabb.Extents.Magnitude() * 2.2f;
	m_pA3DCamera->SetPos(vecPosCam);

	A3DSkinModelActionCore* pActCore = m_pA3DModel->GetFirstAction();
	if (pActCore)
		m_pA3DModel->PlayActionByName(pActCore->GetName(), 1.0f);

	return true;
}

bool AUIModelPicture::Release(void)
{
	if (m_pA3DEngine && m_pDirLight)
	{
		m_pA3DEngine->GetA3DLightMan()->ReleaseLight(m_pDirLight);
		m_pDirLight = NULL;
	}
	A3DRELEASE(m_pA3DModel);
	A3DRELEASE(m_pA3DCamera);
	A3DRELEASE(m_pA3DViewport);

	return AUIPicture::Release();
}

bool AUIModelPicture::Save(FILE *file)
{
	fprintf(file, "MODELPICTURE");

	if( !AUIPicture::Save(file) ) return false;

	fprintf(file, " \"%s\" %d %d %d\n", m_pA3DModel ? m_pA3DModel->GetName() : "",
		A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color), A3DCOLOR_GETBLUE(m_color));

	return true;
}

bool AUIModelPicture::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return AUIPicture::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIModelPicture::Tick(void)
{
	return AUIPicture::Tick();
}

bool AUIModelPicture::Render(void)
{
	if( !m_pA3DModel ) return AUIPicture::Render();

	bool bval = true;
	A3DViewport * pViewportNow = m_pA3DEngine->GetActiveViewport();

	POINT ptPos = GetPos();
	A3DVIEWPORTPARAM Param;
	Param.X = ptPos.x;
	Param.Y = ptPos.y;
	Param.Width = m_nWidth;
	Param.Height = m_nHeight;
	Param.MinZ = 0.0f;
	Param.MaxZ = 1.0f;
	m_pA3DViewport->SetParam(&Param);

	m_pA3DViewport->Active();
	m_pA3DViewport->ClearDevice();

	DWORD dwTime = a_GetTime();
	DWORD dwDiff = min(dwTime - m_dwTime, 100);
	m_pA3DModel->Update(dwDiff);
	m_dwTime = dwTime;

	bval = m_pA3DModel->RenderAtOnce(m_pA3DViewport, 0, false);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIModelPicture::Render(), failed to call m_pA3DModel->RenderAtOnce()");

	pViewportNow->Active();

	return true;
}

bool AUIModelPicture::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Model File") )
		strncpy(Property->fn, m_pA3DModel ? m_pA3DModel->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Background Color") )
		Property->dw = m_color;
	else
		return AUIPicture::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIModelPicture::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Model File") )
		return InitIndirect(Property->fn, 0);
	else if( 0 == strcmpi(pszPropertyName, "Background Color") )
	{
		m_color = Property->dw;
		if( m_pA3DViewport ) m_pA3DViewport->SetClearColor(m_color);
	}
	else
		return AUIPicture::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIModelPicture::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Model File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Background Color", AUIOBJECT_PROPERTY_VALUE_INT);
}

A3DCamera * AUIModelPicture::GetA3DCamera(void)
{
	return m_pA3DCamera;
}

A3DAlterableSkinModel * AUIModelPicture::GetA3DAlterableSkinModel(void)
{
	return m_pA3DModel;
}
