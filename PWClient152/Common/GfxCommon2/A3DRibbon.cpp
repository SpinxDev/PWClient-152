
#include "stdafx.h"
#include "A3DRibbon.h"
#include "TrailList.h"

static const float TRAIL_LERP_ANGLE = DEG2RAD(6.f);
static const DWORD _inner_tick = 40;

static const char _format_orgpos1[]	= "OrgPos1: %f, %f, %f";
static const char _format_orgpos2[]	= "OrgPos2: %f, %f, %f";
static const char _format_en_mat[]	= "EnableMat: %d";
static const char _format_en_pos1[]	= "EnableOrgPos1: %d";
static const char _format_en_pos2[]	= "EnableOrgPos2: %d";
static const char _format_seglife[]	= "SegLife: %d";
static const char _format_bind[]	= "Bind: %d";
static const char _format_splinemode[] = "Spline: %d";
static const char _format_time_to_gravity[] = "TimeToGravity: %d";
static const char _format_velocity_to_gravity[] = "VelocityToGravity: %f";
static const char _format_gravity[] = "Gravity: %f";
static const char _format_vertical_noise[] = "VerticalNoise: %f";
static const char _format_vertical_speed[] = "VerticalSpeed: %f";
static const char _format_horz_amplitude[] = "HorzAmplitude: %f";
static const char _format_horz_speed[] = "HorzSpeed: %f";
static const char _format_x_noise[] = "XNoise: %d";
static const char _format_z_noise[] = "ZNoise: %d";
static const char _format_gradient_mode[] = "GradientMode: %d";
static const char _format_start_portion[] = "StartPortion: %f";
static const char _format_add_per_frame[] = "AddPerFrame: %f";

A3DRibbon::A3DRibbon(A3DGFXEx* pGfx) 
: A3DTrail(pGfx)
{
	m_nEleType = ID_ELE_TYPE_RIBBON;

	m_nTimeToGravity	= 200;
	m_fVelocityToGravity	= 0.01f;
	m_dwStaticCount		= 0;
	m_vLast.Clear();

	m_GravityData.m_fGravity = 0.03f;
	m_GravityData.m_fAmplitude = 0.05f;
	m_GravityData.m_fHSpeed = 5.f;
	m_GravityData.m_fVerticalNoise = 0.3f;
	m_GravityData.m_fVSpeed = 1.5f;
	m_GravityData.m_bXNoise = false;
	m_GravityData.m_bZNoise = true;

	if (m_pTrailImpl)
		delete m_pTrailImpl;

#ifdef GFX_EDITOR
	// 只有编辑器里才需要在构造函数里create（添加一个新元素的时候需要这样）
	m_pTrailImpl		= new LineTrailGravityList;
	((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
#else
	// 正常情况下，对象会在创建之后Load，于是会在Load中创建TrailImpl对象
	m_pTrailImpl		= NULL;
#endif

}

A3DRibbon::~A3DRibbon()
{
}

bool A3DRibbon::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_RIBBON_ORGPOS1:
		m_vOrgPos[0] = prop;
		break;
	case ID_GFXOP_RIBBON_ORGPOS2:
		m_vOrgPos[1] = prop;
		break;
	case ID_GFXOP_RIBBON_SEGLIFE:
		m_nSegLife = prop;
		break;
	case ID_GFXOP_RIBBON_BIND:
		m_bBind = prop;
		break;
	case ID_GFXOP_RIBBON_TIME_TO_GRAVITY:
		m_nTimeToGravity = prop;
		if (m_nTimeToGravity < 0)
			m_nTimeToGravity = 0;
		break;
	case ID_GFXOP_RIBBON_VELOCITY_TO_GRAVITY:
		m_fVelocityToGravity = prop;
		break;
	case ID_GFXOP_RIBBON_GRAVITY:
		m_GravityData.m_fGravity = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	case ID_GFXOP_RIBBON_VERTICAL_NOISE:
		m_GravityData.m_fVerticalNoise = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	case ID_GFXOP_RIBBON_VERTICAL_SPEED:
		m_GravityData.m_fVSpeed = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	case ID_GFXOP_RIBBON_HORZ_AMPLITUDE:
		m_GravityData.m_fAmplitude = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	case ID_GFXOP_RIBBON_HORZ_SPEED:
		m_GravityData.m_fHSpeed = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	case ID_GFXOP_RIBBON_X_NOISE:
		m_GravityData.m_bXNoise = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	case ID_GFXOP_RIBBON_Z_NOISE:
		m_GravityData.m_bZNoise = prop;
		((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);
		break;
	
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}
	return true;
}

GFX_PROPERTY A3DRibbon::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_RIBBON_ORGPOS1:
		return GFX_PROPERTY(m_vOrgPos[0]);
	case ID_GFXOP_RIBBON_ORGPOS2:
		return GFX_PROPERTY(m_vOrgPos[1]);
	case ID_GFXOP_RIBBON_SEGLIFE:
		return GFX_PROPERTY(m_nSegLife);
	case ID_GFXOP_RIBBON_BIND:
		return GFX_PROPERTY(m_bBind);
	case ID_GFXOP_RIBBON_TIME_TO_GRAVITY:
		return GFX_PROPERTY(m_nTimeToGravity);
	case ID_GFXOP_RIBBON_VELOCITY_TO_GRAVITY:
		return GFX_PROPERTY(m_fVelocityToGravity);
	case ID_GFXOP_RIBBON_GRAVITY:
		return GFX_PROPERTY(m_GravityData.m_fGravity);
	case ID_GFXOP_RIBBON_VERTICAL_NOISE:
		return GFX_PROPERTY(m_GravityData.m_fVerticalNoise);
	case ID_GFXOP_RIBBON_VERTICAL_SPEED:
		return GFX_PROPERTY(m_GravityData.m_fVSpeed);
	case ID_GFXOP_RIBBON_HORZ_AMPLITUDE:
		return GFX_PROPERTY(m_GravityData.m_fAmplitude);
	case ID_GFXOP_RIBBON_HORZ_SPEED:
		return GFX_PROPERTY(m_GravityData.m_fHSpeed);
	case ID_GFXOP_RIBBON_X_NOISE:
		return GFX_PROPERTY(m_GravityData.m_bXNoise);
	case ID_GFXOP_RIBBON_Z_NOISE:
		return GFX_PROPERTY(m_GravityData.m_bZNoise);
	}
	return A3DGFXElement::GetProperty(nOp);
}

bool A3DRibbon::IsParamEnable(int nParamId) const
{ 
	switch (nParamId)
	{
	case ID_PARAM_RIBBON_MATRIX:
		return m_bMatrixEnable;
	case ID_PARAM_RIBBON_POS1:
		return m_bOrgPos1Enable;
	case ID_PARAM_RIBBON_POS2:
		return m_bOrgPos2Enable;
	}
	return false; 
}

void A3DRibbon::EnableParam(int nParamId, bool bEnable) 
{
	switch (nParamId)
	{
	case ID_PARAM_RIBBON_MATRIX:
		m_bMatrixEnable = bEnable;
		break;
	case ID_PARAM_RIBBON_POS1:
		m_bOrgPos1Enable = bEnable;
		break;
	case ID_PARAM_RIBBON_POS2:
		m_bOrgPos2Enable = bEnable;
		break;
	}
}

bool A3DRibbon::NeedUpdateParam(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_RIBBON_MATRIX:
		return true;
	case ID_PARAM_RIBBON_POS1:
		return !m_bOrgPos1Set;
	case ID_PARAM_RIBBON_POS2:
		return !m_bOrgPos2Set;
	}
	return true; 
}

void A3DRibbon::UpdateParam(int nParamId, const GFX_PROPERTY& prop)
{ 
	switch (nParamId)
	{

	case ID_PARAM_RIBBON_MATRIX:
		{

			KEY_POINT kp;
			GetCurKeyPoint(&kp);
			const A3DMATRIX4& mat = prop;
			if (m_pTrailImpl)
				m_pTrailImpl->AddKeyData(TRAIL_DATA(mat * m_vOrgPos[0], mat * m_vOrgPos[1], m_nSegLife - (int)m_dwDeltaTm, kp.m_color));

			break;
		}
	case ID_PARAM_RIBBON_POS1:
		m_vOrgPos[0] = prop;
		m_bOrgPos1Set = m_bMatrixEnable;
		break;
	case ID_PARAM_RIBBON_POS2:
		m_vOrgPos[1] = prop;
		m_bOrgPos2Set = m_bMatrixEnable;
		break;
	}
}

GFX_PROPERTY A3DRibbon::GetParam(int nParamId) const 
{ 
	switch (nParamId)
	{
	case ID_PARAM_RIBBON_POS1:
		return GFX_PROPERTY(m_vOrgPos[0]);
	case ID_PARAM_RIBBON_POS2:
		return GFX_PROPERTY(m_vOrgPos[1]);
	}
	return GFX_PROPERTY(); 
}

GfxValueType A3DRibbon::GetParamType(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_RIBBON_MATRIX:
		return GFX_VALUE_MATRIX4;
	case ID_PARAM_RIBBON_POS1:
	case ID_PARAM_RIBBON_POS2:
		return GFX_VALUE_VECTOR3;
	}
	return GFX_VALUE_UNKNOWN;
}

A3DGFXElement* A3DRibbon::Clone(A3DGFXEx* pGfx) const
{
	A3DRibbon* p = new A3DRibbon(pGfx);
	return &(*p = *this);
}

A3DRibbon& A3DRibbon::operator = (const A3DRibbon& src)
{
	if (&src == this)
		return *this;

	_CloneBase(&src);
	Init(src.m_pDevice);
	m_vOrgPos[0]	= src.m_vOrgPos[0];
	m_vOrgPos[1]	= src.m_vOrgPos[1];
	m_nSegLife		= src.m_nSegLife;
	m_bBind			= src.m_bBind;
	m_bOrgPos1Enable= src.m_bOrgPos1Enable;
	m_bOrgPos2Enable= src.m_bOrgPos2Enable;
	m_bMatrixEnable	= src.m_bMatrixEnable;

	m_nTimeToGravity = src.m_nTimeToGravity;	
	m_fVelocityToGravity = src.m_fVelocityToGravity;
	m_GravityData = src.m_GravityData;

	if (m_pTrailImpl == NULL)
	{
		m_pTrailImpl = new LineTrailGravityList;
	}	
	assert(m_pTrailImpl);
	((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);


	return *this;
}

bool A3DRibbon::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	TRAILMODE	iSplineMode = LINE_MODE;

	if (pFile->IsBinary())
	{
		pFile->Read(m_vOrgPos[0].m, sizeof(m_vOrgPos[0].m), &dwReadLen);
		pFile->Read(m_vOrgPos[1].m, sizeof(m_vOrgPos[1].m), &dwReadLen);
		pFile->Read(&m_bMatrixEnable, sizeof(m_bMatrixEnable), &dwReadLen);
		pFile->Read(&m_bOrgPos1Enable, sizeof(m_bOrgPos1Enable), &dwReadLen);
		pFile->Read(&m_bOrgPos2Enable, sizeof(m_bOrgPos2Enable), &dwReadLen);
		pFile->Read(&m_nSegLife, sizeof(m_nSegLife), &dwReadLen);
		pFile->Read(&m_bBind, sizeof(m_bBind), &dwReadLen);
	}
	else
	{
		int nRead;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_orgpos1, VECTORADDR_XYZ(m_vOrgPos[0]));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_orgpos2, VECTORADDR_XYZ(m_vOrgPos[1]));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_mat, &nRead);
		m_bMatrixEnable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_pos1, &nRead);
		m_bOrgPos1Enable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_pos2, &nRead);
		m_bOrgPos2Enable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_seglife, &m_nSegLife);

		if (dwVersion >= 18)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_bind, &nRead);
			m_bBind = (nRead != 0);
		}

		if (dwVersion < 80)
		{
			// 空间扭曲从版本80开始起作用
			m_bWarp = false;
		}

		if (dwVersion >= 87)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_splinemode, &iSplineMode);
		}
		else
		{
			// 版本87以前的都是不支持SplineMode的，将其值置为LINE_MODE
			iSplineMode = LINE_MODE;
		}
	}

	// 如果m_pTrailImpl为空，则创建一个新的，针对客户端
	if (m_pTrailImpl == NULL)
	{
		m_pTrailImpl = new LineTrailGravityList;
	}

	assert(m_pTrailImpl);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_time_to_gravity, &m_nTimeToGravity);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_velocity_to_gravity, &m_fVelocityToGravity);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_gravity, &m_GravityData.m_fGravity);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_vertical_noise, &m_GravityData.m_fVerticalNoise);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_vertical_speed, &m_GravityData.m_fVSpeed);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_horz_amplitude, &m_GravityData.m_fAmplitude);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_horz_speed, &m_GravityData.m_fHSpeed);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_x_noise, &m_GravityData.m_bXNoise);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_z_noise, &m_GravityData.m_bZNoise);

	((LineTrailGravityList*)m_pTrailImpl)->SetGravityData(m_GravityData);

	return true;
}

bool A3DRibbon::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vOrgPos[0].m, sizeof(m_vOrgPos[0].m), &dwWrite);
		pFile->Write(m_vOrgPos[1].m, sizeof(m_vOrgPos[1].m), &dwWrite);
		pFile->Write(&m_bMatrixEnable, sizeof(m_bMatrixEnable), &dwWrite);
		pFile->Write(&m_bOrgPos1Enable, sizeof(m_bOrgPos1Enable), &dwWrite);
		pFile->Write(&m_bOrgPos2Enable, sizeof(m_bOrgPos2Enable), &dwWrite);
		pFile->Write(&m_nSegLife, sizeof(m_nSegLife), &dwWrite);
		pFile->Write(&m_bBind, sizeof(m_bBind), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_orgpos1, VECTOR_XYZ(m_vOrgPos[0]));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_orgpos2, VECTOR_XYZ(m_vOrgPos[1]));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_mat, (int)m_bMatrixEnable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_pos1, (int)m_bOrgPos1Enable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_pos2, (int)m_bOrgPos2Enable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_seglife, m_nSegLife);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_bind, (int)m_bBind);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_splinemode, (int)1);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_time_to_gravity, (int)m_nTimeToGravity);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_velocity_to_gravity, m_fVelocityToGravity);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gravity, m_GravityData.m_fGravity);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_vertical_noise, m_GravityData.m_fVerticalNoise);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_vertical_speed, m_GravityData.m_fVSpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_horz_amplitude, m_GravityData.m_fAmplitude);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_horz_speed, m_GravityData.m_fHSpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_x_noise, m_GravityData.m_bXNoise);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_z_noise, m_GravityData.m_bZNoise);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DRibbon::TickAnimation(DWORD dwTickTime)
{
	if (dwTickTime) 
	{
		assert(m_pTrailImpl);

		float v = GetCurrentVelocity();
		bool bGravity = ShouldEnableGravity(dwTickTime, v);
		((LineTrailGravityList*)m_pTrailImpl)->UseGravity(bGravity);	

		m_pTrailImpl->Tick(dwTickTime);

		InnerTick(dwTickTime);
	}

	return true;
}

bool A3DRibbon::ShouldEnableGravity(DWORD dwTick, float velocity)
{
	if ( velocity < m_fVelocityToGravity*dwTick)
	{
		m_dwStaticCount += dwTick;
		if (m_dwStaticCount > (DWORD)m_nTimeToGravity)
			return true;
	}
	else
	{
		m_dwStaticCount = 0;
	}

	return false;
}

float A3DRibbon::GetCurrentVelocity()
{
	KEY_POINT kp;
	GetCurKeyPoint(&kp);

	A3DVECTOR3 vCurrent = kp.m_vPos * GetParentTM();

	float v = (vCurrent - m_vLast).Magnitude();
	m_vLast = vCurrent;

	return v;
}