#include "StdAfx.h"
#include "A3DParticleEmitter.h"
#include "A3DGFXKeyPoint.h"
#include "AAssist.h"
#include "A3DPointEmitter.h"
#include "A3DBoxEmitter.h"
#include "A3DMacros.h"
#include "A3DMultiPlaneEmitter.h"
#include "A3DEllipsoidEmitter.h"
#include "A3DCylinderEmitter.h"
#include "A3DCurveEmitter.h"
#include "A3DSkeletonEmitter.h"
#include "A3DGFXElement.h"

static const char* _format_rate		= "EmissionRate: %f";
static const char* _format_angle	= "Angle: %f";
static const char* _format_speed	= "Speed: %f";
static const char* _format_par_acc	= "ParAcc: %f";
static const char* _format_acc_dir	= "AccDir: %f, %f, %f";
static const char* _format_acc		= "Acc: %f";
static const char* _format_ttl		= "TTL: %f";
static const char* _format_cl_min	= "ColorMin: %d";
static const char* _format_cl_max	= "ColorMax: %d";
static const char* _format_scale	= "Scale: %f";
static const char* _format_sca_min	= "ScaleMin: %f";
static const char* _format_sca_max	= "ScaleMax: %f";
static const char* _format_rot_min	= "RotMin: %f";
static const char* _format_rot_max	= "RotMax: %f";
static const char* _format_surface	= "IsSurface: %d";
static const char* _format_bind		= "IsBind: %d";
static const char* _format_drag		= "IsDrag: %d";
static const char* _format_drag_pow	= "DragPow: %f";
static const char* _format_particle_init_dir = "ParIniDir: %f, %f, %f";
static const char* _format_isusehsvinterp = "IsUseHSVInterp: %d";

A3DParticleEmitter::A3DParticleEmitter() : 
	m_vAccDir(0, 0, -1.0f),
	m_vParentPos(0),
	m_qParentDir(_unit_dir),
	m_vParticalInitDir(0),
	m_bUseHSVInterpolation(false),
	m_pParticleSystem(NULL)
{
	m_nType			= EMITTER_TYPE_NONE;
	m_fEmissionRate	= 0.0f;
	m_fRemainder	= 0.0f;
	m_fAngle		= 0.0f;
	m_fSpeed		= 0.0f;
	m_fParAcc		= 0.0f;
	m_fAcc			= 0.0f;
	m_fTTL			= 0.0f;
	m_colorMin		= A3DCOLORRGBA(255, 255, 255, 255);
	m_colorMax		= A3DCOLORRGBA(255, 255, 255, 255);
	a3d_RGBToHSV(m_colorMin, &m_hsvMin);
	a3d_RGBToHSV(m_colorMax, &m_hsvMax);
	m_fScaleMin		= 1.0f;
	m_fScaleMax		= 1.0f;
	m_fRotMin		= 0;
	m_fRotMax		= 0;
	m_bSurface		= false;
	m_bBind			= true;
	m_bDrag			= false;
	m_fDragIntensity= 1.0f;
	m_fEmitterAlpha	= 1.0f;
	m_pKeyPoint		= 0;

	calcParticleDir();
}

A3DParticleEmitter::~A3DParticleEmitter()
{
}

void A3DParticleEmitter::GenDirection(A3DVECTOR3& vDir)
{
	vDir = (m_bBind ? _unit_z : RotateVec(m_qParentDir, _unit_z));

	if (m_fAngle != 0.0f)
	{
		float fAngle = _UnitRandom() * m_fAngle;
		A3DVECTOR3 vUp = (m_bBind ? _unit_x :  RotateVec(m_qParentDir, _unit_x));
		A3DQUATERNION q(vDir, _UnitRandom() * A3D_2PI);
		vUp = RotateVec(q, vUp);
		q.ConvertFromAxisAngle(vUp, fAngle);
		vDir = RotateVec(q, vDir);
	}
}

float A3DParticleEmitter::GenTTL()
{
	return m_fTTL;
}

A3DCOLOR A3DParticleEmitter::GenColor()
{
	if (m_colorMin == m_colorMax) return m_colorMin;
	
	// Use HSV Interpolation
	if (m_bUseHSVInterpolation)
	{
		float fRatio = _UnitRandom();
		A3DCOLOR rgbVal;
		A3DHSVCOLORVALUE hsvVal;
		if (m_hsvMin.h > m_hsvMax.h) {
			hsvVal.h = m_hsvMin.h * fRatio + (1 + m_hsvMax.h) * (1 - fRatio);
			if (hsvVal.h > 1.f)
				hsvVal.h -= 1.f;
		}
		else
			hsvVal.h = m_hsvMin.h * fRatio + (1 - fRatio) * m_hsvMax.h;

		hsvVal.s = m_hsvMin.s * fRatio + (1 - fRatio) * m_hsvMax.s;
		hsvVal.v = m_hsvMin.v * fRatio + (1 - fRatio) * m_hsvMax.v;
		hsvVal.a = m_hsvMin.a * fRatio + (1 - fRatio) * m_hsvMax.a;
		a3d_HSVToRGB(hsvVal, &rgbVal);
		return rgbVal;
	}
	else
	{
		float fR = _UnitRandom();
		float fG = _UnitRandom();
		float fB = _UnitRandom();
		float fA = _UnitRandom();
		int nR = int(A3DCOLOR_GETRED  (m_colorMin) * (1.0f - fR) + A3DCOLOR_GETRED(m_colorMax)	* fR);
		int nG = int(A3DCOLOR_GETGREEN(m_colorMin) * (1.0f - fG) + A3DCOLOR_GETGREEN(m_colorMax)* fG);
		int nB = int(A3DCOLOR_GETBLUE (m_colorMin) * (1.0f - fB) + A3DCOLOR_GETBLUE(m_colorMax)	* fB);
		int nA = int(A3DCOLOR_GETALPHA(m_colorMin) * (1.0f - fA) + A3DCOLOR_GETALPHA(m_colorMax)* fA);
		nA = (int)(nA * m_fEmitterAlpha);
		return A3DCOLORRGBA(nR, nG, nB, nA);
	}
}

float A3DParticleEmitter::GenScale()
{
	float f = _UnitRandom();
	return m_fScaleMin * (1.f - f) + m_fScaleMax * f;
}

float A3DParticleEmitter::GenRotAngle()
{
	float f = _UnitRandom();
	return m_fRotMin * (1.f - f) + m_fRotMax * f;
}

int A3DParticleEmitter::GenEmissionCount(float fTimeSpan)
{
	m_fRemainder += m_fEmissionRate * fTimeSpan;
	int nRequest = (int) m_fRemainder;
	m_fRemainder -= nRequest;
	return nRequest;
}

A3DParticleEmitter* A3DParticleEmitter::CreateEmitter(int nType)
{
	switch (nType)
	{
	case EMITTER_TYPE_POINT:
		return new A3DPointEmitter;
	case EMITTER_TYPE_BOX:
		return new A3DBoxEmitter;
	case EMITTER_TYPE_MULTIPLANE:
		return new A3DMultiPlaneEmitter;
	case EMITTER_TYPE_ELLIPSOID:
		return new A3DEllipsoidEmitter;
	case EMITTER_TYPE_CYLINDER:
		return new A3DCylinderEmitter;
	case EMITTER_TYPE_CURVE:
		return new A3DCurveEmitter;
	case EMITTER_TYPE_SKELETON:
		return new A3DSkeletonEmitter;
	default:
		return NULL;
	}
}

bool A3DParticleEmitter::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fEmissionRate, sizeof(m_fEmissionRate), &dwRead);
		pFile->Read(&m_fAngle, sizeof(m_fAngle), &dwRead);
		pFile->Read(&m_fSpeed, sizeof(m_fSpeed), &dwRead);
		pFile->Read(&m_fParAcc, sizeof(m_fParAcc), &dwRead);
		pFile->Read(m_vAccDir.m, sizeof(m_vAccDir.m), &dwRead);
		pFile->Read(&m_fAcc, sizeof(m_fAcc), &dwRead);
		pFile->Read(&m_fTTL, sizeof(m_fTTL), &dwRead);
		pFile->Read(&m_colorMin, sizeof(m_colorMin), &dwRead);
		pFile->Read(&m_colorMax, sizeof(m_colorMax), &dwRead);
		pFile->Read(&m_fScaleMin, sizeof(m_fScaleMin), &dwRead);
		pFile->Read(&m_fScaleMax, sizeof(m_fScaleMax), &dwRead);
		pFile->Read(&m_fRotMin, sizeof(m_fRotMin), &dwRead);
		pFile->Read(&m_fRotMax, sizeof(m_fRotMax), &dwRead);
		pFile->Read(&m_bSurface, sizeof(m_bSurface), &dwRead);
		pFile->Read(&m_bBind, sizeof(m_bBind), &dwRead);
		pFile->Read(&m_bDrag, sizeof(m_bDrag), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		int		nRead;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_rate, &m_fEmissionRate);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_angle, &m_fAngle);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_speed, &m_fSpeed);

		if (dwVersion >= 20)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_par_acc, &m_fParAcc);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_acc_dir, VECTORADDR_XYZ(m_vAccDir));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_acc, &m_fAcc);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_ttl, &m_fTTL);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_cl_min, &m_colorMin);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_cl_max, &m_colorMax);

		if (dwVersion <= 1)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_scale, &m_fScaleMin);
			m_fScaleMax = m_fScaleMin;
		}
		else
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sca_min, &m_fScaleMin);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sca_max, &m_fScaleMax);
		}

		if (dwVersion >= 10)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rot_min, &m_fRotMin);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rot_max, &m_fRotMax);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_surface, &nRead);
		m_bSurface = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_bind, &nRead);
		m_bBind = (nRead != 0);

		if (dwVersion >= 43)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_drag, &nRead);
			m_bDrag = (nRead != 0);
		}

		if (dwVersion >= 48)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_drag_pow, &m_fDragIntensity);
		}

		if (dwVersion >= 62)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_particle_init_dir, VECTORADDR_XYZ(m_vParticalInitDir));
			calcParticleDir();
		}

		if (dwVersion >= 70)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_isusehsvinterp, &nRead);
			m_bUseHSVInterpolation = nRead != 0;
		}
	}

	a3d_RGBToHSV(m_colorMin, &m_hsvMin);
	a3d_RGBToHSV(m_colorMax, &m_hsvMax);

	return true;
}

bool A3DParticleEmitter::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fEmissionRate, sizeof(m_fEmissionRate), &dwWrite);
		pFile->Write(&m_fAngle, sizeof(m_fAngle), &dwWrite);
		pFile->Write(&m_fSpeed, sizeof(m_fSpeed), &dwWrite);
		pFile->Write(&m_fParAcc, sizeof(m_fParAcc), &dwWrite);
		pFile->Write(m_vAccDir.m, sizeof(m_vAccDir.m), &dwWrite);
		pFile->Write(&m_fAcc, sizeof(m_fAcc), &dwWrite);
		pFile->Write(&m_fTTL, sizeof(m_fTTL), &dwWrite);
		pFile->Write(&m_colorMin, sizeof(m_colorMin), &dwWrite);
		pFile->Write(&m_colorMax, sizeof(m_colorMax), &dwWrite);
		pFile->Write(&m_fScaleMin, sizeof(m_fScaleMin), &dwWrite);
		pFile->Write(&m_fScaleMax, sizeof(m_fScaleMax), &dwWrite);
		pFile->Write(&m_fRotMin, sizeof(m_fRotMin), &dwWrite);
		pFile->Write(&m_fRotMax, sizeof(m_fRotMax), &dwWrite);
		pFile->Write(&m_bSurface, sizeof(m_bSurface), &dwWrite);
		pFile->Write(&m_bBind, sizeof(m_bBind), &dwWrite);
		pFile->Write(&m_bDrag, sizeof(m_bDrag), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_rate, m_fEmissionRate);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_angle, m_fAngle);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_speed, m_fSpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_par_acc, m_fParAcc);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_acc_dir, VECTOR_XYZ(m_vAccDir));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_acc, m_fAcc);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_ttl, m_fTTL);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_cl_min, m_colorMin);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_cl_max, m_colorMax);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sca_min, m_fScaleMin);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sca_max, m_fScaleMax);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_rot_min, m_fRotMin);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_rot_max, m_fRotMax);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_surface, m_bSurface);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_bind, m_bBind);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_drag, m_bDrag);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_drag_pow, m_fDragIntensity);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_particle_init_dir, VECTOR_XYZ(m_vParticalInitDir));
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_isusehsvinterp, m_bUseHSVInterpolation);
		pFile->WriteLine(szLine);
	}

	return true;
}

void A3DParticleEmitter::CloneFrom(const A3DParticleEmitter* p)
{
	m_fEmissionRate	= p->m_fEmissionRate;
	m_fAngle		= p->m_fAngle;
	m_fSpeed		= p->m_fSpeed;
	m_fParAcc		= p->m_fParAcc;
	m_vAccDir		= p->m_vAccDir;
	m_fAcc			= p->m_fAcc;
	m_fTTL			= p->m_fTTL;
	m_colorMin		= p->m_colorMin;
	m_colorMax		= p->m_colorMax;
	m_fScaleMin		= p->m_fScaleMin;
	m_fScaleMax		= p->m_fScaleMax;
	m_fRotMin		= p->m_fRotMin;
	m_fRotMax		= p->m_fRotMax;
	m_bSurface		= p->m_bSurface;
	m_bBind			= p->m_bBind;
	m_bDrag			= p->m_bDrag;
	m_fDragIntensity= p->m_fDragIntensity;
	m_fEmitterAlpha	= p->m_fEmitterAlpha;
	m_vParticalInitDir = p->m_vParticalInitDir;
	calcParticleDir();
	m_bUseHSVInterpolation = p->m_bUseHSVInterpolation;
	a3d_RGBToHSV(m_colorMin, &m_hsvMin);
	a3d_RGBToHSV(m_colorMax, &m_hsvMax);
}

void A3DParticleEmitter::OnStart(A3DParticleSystemEx* pParticleSystem)
{
	m_pParticleSystem = pParticleSystem;
}
