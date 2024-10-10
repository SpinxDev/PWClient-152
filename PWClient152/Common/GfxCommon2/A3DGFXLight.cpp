#include "StdAfx.h"
#include "A3DGFXLight.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"

#if defined(_ANGELICA21) && defined(UNICODE)
	extern DWORD g_GfxGetLightUniqueId();
#endif

static const char _format_type[]		= "LightType: %d";
static const char _format_diffuse[]		= "Diffuse: %d";
static const char _format_specular[]	= "Specular: %d";
static const char _format_ambient[]		= "Ambient: %d";
static const char _format_position[]	= "Position: %f, %f, %f";
static const char _format_direction[]	= "Direction: %f, %f, %f";
static const char _format_range[]		= "Range: %f";
static const char _format_falloff[]		= "FallOff: %f";
static const char _format_attenuation0[]= "Attenuation0: %f";
static const char _format_attenuation1[]= "Attenuation0: %f";
static const char _format_attenuation2[]= "Attenuation0: %f";
static const char _format_theta[]		= "Theta: %f";
static const char _format_phi[]			= "Phi: %f";
static const char _format_inner_use[]	= "InnerUse: %d";


A3DGFXLight::A3DGFXLight(A3DGFXEx* pGfx) 
: A3DGFXElement(pGfx)
{
	m_nEleType					= ID_ELE_TYPE_LIGHT;
	m_LightParam.Type			= A3DLIGHT_POINT;
	m_LightParam.Diffuse		= A3DCOLORRGBA(255, 255, 255, 255);
	m_LightParam.Specular		= A3DCOLORRGBA(0, 0, 0, 255);
	m_LightParam.Ambient		= A3DCOLORRGBA(0, 0, 0, 255);
	m_LightParam.Range			= 10.0f;
	m_LightParam.Position		= A3DVECTOR3(0, 0, 0);
	m_LightParam.Direction		= A3DVECTOR3(0, 0, 1.0f);
	m_LightParam.Attenuation0	= 0.0f;
	m_LightParam.Attenuation1	= 0.1f;
	m_LightParam.Attenuation2	= 0.0f;
	m_LightParam.Falloff		= 0.0f;
	m_LightParam.Theta			= 0.0f;
	m_LightParam.Phi			= 0.0f;
	m_dwUniqueId				= 0;
	m_bAddedToLightQueue		= false;
	m_bInnerUse					= false;
}

A3DGFXLight::~A3DGFXLight()
{
	AfxGetGFXExMan()->RemoveLightParam(this);
}

A3DGFXElement* A3DGFXLight::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXLight* p = new A3DGFXLight(pGfx);
	return &(*p = *this);
}

A3DGFXLight& A3DGFXLight::operator = (const A3DGFXLight& src)
{
	if (&src == this)
		return *this;

	_CloneBase(&src);
	Init(src.m_pDevice);
	m_LightParam = src.m_LightParam;
	m_bInnerUse = src.m_bInnerUse;
	return *this;
}

// interfaces of A3DGFXElement
bool A3DGFXLight::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_LightParam, sizeof(m_LightParam), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		A3DCOLOR color;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_type, &m_LightParam.Type);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_diffuse, &color);
		m_LightParam.Diffuse = color;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_specular, &color);
		m_LightParam.Specular = color;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_ambient, &color);
		m_LightParam.Ambient = color;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_position, VECTORADDR_XYZ(m_LightParam.Position));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_direction, VECTORADDR_XYZ(m_LightParam.Direction));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_range, &m_LightParam.Range);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_falloff, &m_LightParam.Falloff);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_attenuation0, &m_LightParam.Attenuation0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_attenuation1, &m_LightParam.Attenuation1);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_attenuation2, &m_LightParam.Attenuation2);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_theta, &m_LightParam.Theta);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_phi, &m_LightParam.Phi);

		if (dwVersion >= 119)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_inner_use, &nBool);
			m_bInnerUse = (nBool != 0);
		}
	}

	return true;
}

bool A3DGFXLight::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_LightParam, sizeof(m_LightParam), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_type, m_LightParam.Type);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_diffuse, m_LightParam.Diffuse.ToRGBAColor());
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_specular, m_LightParam.Specular.ToRGBAColor());
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_ambient, m_LightParam.Ambient.ToRGBAColor());
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_position, VECTOR_XYZ(m_LightParam.Position));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_direction, VECTOR_XYZ(m_LightParam.Direction));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_range, m_LightParam.Range);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_falloff, m_LightParam.Falloff);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_attenuation0, m_LightParam.Attenuation0);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_attenuation1, m_LightParam.Attenuation1);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_attenuation2, m_LightParam.Attenuation2);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_theta, m_LightParam.Theta);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_phi, m_LightParam.Phi);
		pFile->WriteLine(szLine);
		
		sprintf(szLine, _format_inner_use, m_bInnerUse);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXLight::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime))
		return false;

	if (!m_bVisible || m_nPriority > AfxGetGFXExMan()->GetPriority())
		return true;

	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return false;

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	m_LightParam.Position = GetParentTM() * kp.m_vPos;

	if (m_LightParam.Type != A3DLIGHT_POINT)
		m_LightParam.Direction = RotateVec(GetParentDir() * kp.m_vDir, _unit_z);

	m_LightParam.Diffuse = kp.m_color;

#ifdef _ANGELICA21
	if (!m_pGfx->Is2DRender() && !m_bInnerUse)
		AfxGetGFXExMan()->AddLightParam(this);
#else
	AfxGetGFXExMan()->AddLightParam(this);
#endif
	return true;
}


#ifdef GFX_EDITOR

static void 
drawDirectional(A3DWireCollector* pWireCollector, const A3DMATRIX4& matABS)
{
	if(pWireCollector==NULL) 
		return;

	const static A3DCOLOR dwCol = A3DCOLORRGB(0, 0, 255);
	float fDelta = 2.0f;
	float fTemp = 0.05f;
	float fArrow = 0.75f;
	float fPlane = 0.15f;

	A3DVECTOR3 Vertices[7];
	WORD indices[14];

	indices[0] = 0; indices[1] = 1;
	indices[2] = 1; indices[3] = 2;
	indices[4] = 2; indices[5] = 3;
	indices[6] = 3; indices[7] = 4;
	indices[8] = 4; indices[9] = 5;
	indices[10] = 5; indices[11] = 6;
	indices[12] = 6; indices[13] = 0;

	//z 轴,因为默认情况我们取Z为正方向
	Vertices[0] =  A3DVECTOR3(0,-fPlane,0);
	Vertices[1] =  A3DVECTOR3(0,fPlane,0);
	Vertices[2] =  A3DVECTOR3(0,fPlane,fArrow);
	Vertices[3] =  A3DVECTOR3(0,fPlane+fPlane,fArrow);
	Vertices[4] =  A3DVECTOR3(0,0,fDelta);
	Vertices[5] =  A3DVECTOR3(0,-fPlane-fPlane,fArrow);
	Vertices[6] =  A3DVECTOR3(0,-fPlane,fArrow);

	Vertices[0] = matABS*Vertices[0];
	Vertices[1] = matABS*Vertices[1];
	Vertices[2] = matABS*Vertices[2];
	Vertices[3] = matABS*Vertices[3];
	Vertices[4] = matABS*Vertices[4];
	Vertices[5] = matABS*Vertices[5];
	Vertices[6] = matABS*Vertices[6];
	pWireCollector->AddRenderData_3D(Vertices, 7, indices, 14, dwCol);

	//z 轴,因为默认情况我们取Z为正方向
	Vertices[0] =  A3DVECTOR3(-fPlane,0,0);
	Vertices[1] =  A3DVECTOR3(fPlane,0,0);
	Vertices[2] =  A3DVECTOR3(fPlane,0,fArrow);
	Vertices[3] =  A3DVECTOR3(fPlane+fPlane,0,fArrow);
	Vertices[4] =  A3DVECTOR3(0,0,fDelta);
	Vertices[5] =  A3DVECTOR3(-fPlane-fPlane,0,fArrow);
	Vertices[6] =  A3DVECTOR3(-fPlane,0,fArrow);

	Vertices[0] = matABS*Vertices[0];
	Vertices[1] = matABS*Vertices[1];
	Vertices[2] = matABS*Vertices[2];
	Vertices[3] = matABS*Vertices[3];
	Vertices[4] = matABS*Vertices[4];
	Vertices[5] = matABS*Vertices[5];
	Vertices[6] = matABS*Vertices[6];
	pWireCollector->AddRenderData_3D(Vertices, 7, indices, 14, dwCol);

	Vertices[0] = A3DVECTOR3(0,0,0);
	Vertices[0] = matABS*Vertices[0];
	Vertices[1] = Vertices[4];
	pWireCollector->AddRenderData_3D(Vertices, 2, indices, 2, dwCol);
}

static const int MAX_NUM_SECT = 24;

static void 
drawLine(A3DWireCollector *pWireCollector, A3DVECTOR3 *pVertices, DWORD dwNum, DWORD clr)
{
	if (!pWireCollector || !pVertices)
		return;

	WORD pIndices[MAX_NUM_SECT * 2];
	int n = 0;
	for(int i = 0; i < MAX_NUM_SECT; i++)
	{
		pIndices[n] = i;
		pIndices[n + 1] = (i + 1) % (MAX_NUM_SECT);
		n += 2;
	}

	pWireCollector->AddRenderData_3D(pVertices, dwNum, pIndices, MAX_NUM_SECT * 2, clr);
}

static void 
drawSpot(A3DWireCollector* pWireCollector, bool bPhi, const A3DMATRIX4& matABS, const A3DLIGHTPARAM& lp)
{
	A3DVECTOR3 vertices[MAX_NUM_SECT+1];
	const float pi_2 = A3D_2PI;
	const float pi_delta = pi_2/MAX_NUM_SECT;

	float fRadius;
	DWORD clr;
	if(bPhi)
	{
		fRadius = lp.Range * tanf(DEG2RAD(lp.Phi)/2.0f);
		clr = A3DCOLORRGB(255,255,0);
	}else 
	{
		fRadius = lp.Range * tanf(DEG2RAD(lp.Theta)/2.0f);
		clr = A3DCOLORRGB(255,100,0);
	}
	A3DVECTOR3 vPos = A3DVECTOR3(0,0,0);

	//Calculate vertices array
	float angle = 0;
	//0 - pi2
	int i;
	for(i=0; i<MAX_NUM_SECT; i++)
	{
		vertices[i].x = fRadius * cosf(angle);
		vertices[i].y = fRadius * sinf(angle);
		vertices[i].z = lp.Range;

		angle += pi_delta;
	}

	vertices[MAX_NUM_SECT] = A3DVECTOR3(0,0,0);
	//在这儿做位移以及旋转变化
	A3DMATRIX4 matTrans;
	matTrans = matABS;
	for(i=0; i<MAX_NUM_SECT+1; i++)
	{
		vertices[i] = vertices[i]*matTrans;
	}
	//。。。。。。。。。。。
	//
	drawLine(pWireCollector, vertices,MAX_NUM_SECT,clr);

	WORD pIndices[MAX_NUM_SECT*2];
	int n = 0;
	for(i=0; i<MAX_NUM_SECT; i++)
	{
		pIndices[n] = MAX_NUM_SECT;
		pIndices[n+1] = i;
		n += 2;
	}

	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(vertices, MAX_NUM_SECT+1, pIndices, MAX_NUM_SECT*2, clr);
	}
}

#endif

bool A3DGFXLight::Render(A3DViewport*)
{
#ifdef GFX_EDITOR

	if (AfxGetGFXExMan()->GetShowLightDemoFlag())
	{
		A3DVECTOR3 vRight = CrossProduct(m_LightParam.Direction, _unit_y);
		A3DVECTOR3 vUp = CrossProduct(vRight, m_LightParam.Direction);
		A3DMATRIX4 matABS = a3d_TransformMatrix(m_LightParam.Direction, vUp, m_LightParam.Position);
		A3DWireCollector* pColl = m_pDevice->GetA3DEngine()->GetA3DWireCollector();
		switch (m_LightParam.Type)
		{
		case A3DLIGHT_POINT:
			pColl->AddSphere(m_LightParam.Position, .5f, A3DCOLORRGBA(0, 0, 255, 255));
			break;
		case A3DLIGHT_DIRECTIONAL:
			drawDirectional(pColl, matABS);
			break;
		case A3DLIGHT_SPOT:
			drawSpot(pColl, true, matABS, m_LightParam);
			drawSpot(pColl, false, matABS, m_LightParam);
			break;
		default:
			// Do nothing
			break;
		}
	}

#endif

	return true;
}

bool A3DGFXLight::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_LIGHT_TYPE:
		{
			A3DLIGHTTYPE lt = (A3DLIGHTTYPE)(int)prop;
			if (lt != A3DLIGHT_POINT && lt != A3DLIGHT_SPOT && lt != A3DLIGHT_DIRECTIONAL)
				break;

			m_LightParam.Type = lt;
		}
		break;
	case ID_GFXOP_LIGHT_DIFFUSE:
		m_LightParam.Diffuse = prop;
		break;
	case ID_GFXOP_LIGHT_SPECULAR:
		m_LightParam.Specular = prop;
		break;
	case ID_GFXOP_LIGHT_AMBIENT:
		m_LightParam.Ambient = prop;
		break;
	case ID_GFXOP_LIGHT_POSITION:
		m_LightParam.Position = prop;
		break;
	case ID_GFXOP_LIGHT_DIRECTION:
		m_LightParam.Direction = prop;
		break;
	case ID_GFXOP_LIGHT_RANGE:
		m_LightParam.Range = prop;
		break;
	case ID_GFXOP_LIGHT_FALLOFF:
		m_LightParam.Falloff = prop;
		break;
	case ID_GFXOP_LIGHT_ATTENUATION0:
		m_LightParam.Attenuation0 = prop;
		break;
	case ID_GFXOP_LIGHT_ATTENUATION1:
		m_LightParam.Attenuation1 = prop;
		break;
	case ID_GFXOP_LIGHT_ATTENUATION2:
		m_LightParam.Attenuation2 = prop;
		break;
	case ID_GFXOP_LIGHT_THETA:
		if ((float)prop > m_LightParam.Phi)
			break;

		m_LightParam.Theta = prop;
		break;
	case ID_GFXOP_LIGHT_PHI:
		if ((float)prop < m_LightParam.Theta)
			break;
		
		m_LightParam.Phi = prop;
		break;
	case ID_GFXOP_INNER_USE:
		m_bInnerUse = prop;
		break;
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}
	return true;
}

GFX_PROPERTY A3DGFXLight::GetProperty(int nOp) const
{
	switch (nOp)
	{
	case ID_GFXOP_LIGHT_TYPE:
		return GFX_PROPERTY(m_LightParam.Type).SetType(GFX_VALUE_LIGHT_TYPE);
	case ID_GFXOP_LIGHT_DIFFUSE:
		return GFX_PROPERTY(((A3DCOLORVALUE&)m_LightParam.Diffuse).ToRGBAColor());
	case ID_GFXOP_LIGHT_SPECULAR:
		return GFX_PROPERTY(((A3DCOLORVALUE&)m_LightParam.Specular).ToRGBAColor());
	case ID_GFXOP_LIGHT_AMBIENT:
		return GFX_PROPERTY(((A3DCOLORVALUE&)m_LightParam.Ambient).ToRGBAColor());
	case ID_GFXOP_LIGHT_POSITION:
		return GFX_PROPERTY(m_LightParam.Position);
	case ID_GFXOP_LIGHT_DIRECTION:
		return GFX_PROPERTY(m_LightParam.Direction);
	case ID_GFXOP_LIGHT_RANGE:
		return GFX_PROPERTY(m_LightParam.Range);
	case ID_GFXOP_LIGHT_FALLOFF:
		return GFX_PROPERTY(m_LightParam.Falloff);
	case ID_GFXOP_LIGHT_ATTENUATION0:
		return GFX_PROPERTY(m_LightParam.Attenuation0);
	case ID_GFXOP_LIGHT_ATTENUATION1:
		return GFX_PROPERTY(m_LightParam.Attenuation1);
	case ID_GFXOP_LIGHT_ATTENUATION2:
		return GFX_PROPERTY(m_LightParam.Attenuation2);
	case ID_GFXOP_LIGHT_THETA:
		return GFX_PROPERTY(m_LightParam.Theta);
	case ID_GFXOP_LIGHT_PHI:
		return GFX_PROPERTY(m_LightParam.Phi);
	case ID_GFXOP_INNER_USE:
		return GFX_PROPERTY(m_bInnerUse);
	}
	return A3DGFXElement::GetProperty(nOp);
}

bool A3DGFXLight::Init(A3DDevice* pDevice)
{
	if (!A3DGFXElement::Init(pDevice))
		return false;

#if defined(_ANGELICA21) && defined(UNICODE)
	m_dwUniqueId = g_GfxGetLightUniqueId();
#endif

	return true;
}
