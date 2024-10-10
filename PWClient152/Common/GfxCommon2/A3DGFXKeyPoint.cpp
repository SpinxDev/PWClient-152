#include "StdAfx.h"
#include "A3DGFXKeyPoint.h"
#include "A3DGFXElement.h"

static const char _kp_interp_mode[]		= "InterpolateMode: %d";
static const char _kp_timespan[]		= "TimeSpan: %d";
static const char _kp_ftimespan[]		= "TimeSpan: %f";
static const char _kp_orgpos[]			= "Position: %f, %f, %f";
static const char _kp_color[]			= "Color: %d";
static const char _kp_scale[]			= "Scale: %f";
static const char _kp_dir[]				= "Direction: %f, %f, %f, %f";
static const char _kp_rad_2d[]			= "Rad_2D: %f";
static const char _kp_method_count[]	= "CtrlMethodCount: %d";
static const char _kp_alpha_only[]		= "AlphaOnly: %d";

static const char _kpset_load_format[]	= "KEYPOINTCOUNT: %d";
static const char _kpset_starttime[]	= "StartTime: %d";

static const char _ctrl_type_info[]		= "CtrlType: %d";
static const char _ctrl_start_tm[]		= "StartTime: %f";
static const char _ctrl_end_tm[]		= "EndTime: %f";

static const char _ctrl_dir[]			= "Dir: %f, %f, %f";
static const char _ctrl_vel[]			= "Vel: %f";
static const char _ctrl_acc[]			= "Acc: %f";
static const char _ctrl_pos[]			= "Pos: %f, %f, %f";
static const char _ctrl_axis[]			= "Axis: %f, %f, %f";
static const char _ctrl_cl_delta[]		= "ColorDelta: %d, %d, %d, %d";
static const char _ctrl_scale[]			= "ScaleChage: %f, %f, %f";
static const char _ctrl_noise_buflen[]	= "BufLen: %d";
static const char _ctrl_noise_amp[]		= "Amplitude: %f";
static const char _ctrl_noise_wavlen[]	= "WaveLen: %d";
static const char _ctrl_noise_persist[]	= "Persistence: %f";
static const char _ctrl_noise_octnum[]	= "OctaveNum: %d";
static const char _ctrl_noise_basecl[]	= "BaseColor: %d";
static const char _ctrl_count[]			= "Count: %d";
static const char _ctrl_calc_dir[]		= "CalcDir: %d";

const A3DMATRIX4 _identity_mat4 = a3d_IdentityMatrix();
const A3DVECTOR3 _unit_x(1.0f, 0, 0);
const A3DVECTOR3 _unit_y(0, 1.0f, 0);
const A3DVECTOR3 _unit_z(0, 0, 1.0f);
const A3DVECTOR3 _unit_zero(0);
const A3DQUATERNION _unit_dir(_unit_z, 0);
const A3DCOLOR _white_diffuse = A3DCOLORRGBA(255, 255, 255, 255);
const A3DCOLOR _white_spec = A3DCOLORRGBA(0, 0, 0, 255);

inline float CalcDist(float fVel, float fAcc, float fStartTime, float fTimeSpan)
{
	float f = fVel + fAcc * fStartTime;
	fVel = f + fTimeSpan * fAcc;
	return .5f * (f + fVel) * fTimeSpan;
}

void A3DGFXKeyPoint::Process(PROC_DATA& ProcData, int nTimeSpan)
{
	float fTime = nTimeSpan / 1000.f;
	for (size_t i = 0; i < m_CtrlLst.size(); i++)
	{
		m_CtrlLst[i]->Update(this);
		m_CtrlLst[i]->Process(ProcData, fTime);
	}
}

bool A3DGFXKeyPoint::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_enumMode, sizeof(m_enumMode), &dwWrite);
		pFile->Write(&m_dwTimeSpan, sizeof(m_dwTimeSpan), &dwWrite);
		pFile->Write(m_KPOrg.m_vPos.m, sizeof(m_KPOrg.m_vPos.m), &dwWrite);
		pFile->Write(&m_KPOrg.m_color, sizeof(m_KPOrg.m_color), &dwWrite);
		pFile->Write(&m_KPOrg.m_fScale, sizeof(m_KPOrg.m_fScale), &dwWrite);
		pFile->Write(m_KPOrg.m_vDir.m, sizeof(m_KPOrg.m_vDir.m), &dwWrite);
		pFile->Write(&m_KPOrg.m_fRad2D, sizeof(m_KPOrg.m_fRad2D), &dwWrite);
		size_t sz = m_CtrlLst.size();
		pFile->Write(&sz, sizeof(sz), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _kp_interp_mode, m_enumMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _kp_timespan, m_dwTimeSpan);
		pFile->WriteLine(szLine);

		sprintf(szLine, _kp_orgpos, VECTOR_XYZ(m_KPOrg.m_vPos));
		pFile->WriteLine(szLine);

		sprintf(szLine, _kp_color, m_KPOrg.m_color);
		pFile->WriteLine(szLine);

		sprintf(szLine, _kp_scale, m_KPOrg.m_fScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _kp_dir, QUAT_XYZW(m_KPOrg.m_vDir));
		pFile->WriteLine(szLine);

		sprintf(szLine, _kp_rad_2d, m_KPOrg.m_fRad2D);
		pFile->WriteLine(szLine);
		
		sprintf(szLine, _kp_method_count, m_CtrlLst.size());
		pFile->WriteLine(szLine);
	}

	for (size_t i = 0; i < m_CtrlLst.size(); i++)
		m_CtrlLst[i]->SaveToFile(pFile);

	return true;
}

bool A3DGFXKeyPoint::Load(AFile* pFile, DWORD dwVersion)
{
	int		nCtrlCount;
	
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_enumMode, sizeof(m_enumMode), &dwRead);
		pFile->Read(&m_dwTimeSpan, sizeof(m_dwTimeSpan), &dwRead);
		pFile->Read(m_KPOrg.m_vPos.m, sizeof(m_KPOrg.m_vPos.m), &dwRead);
		pFile->Read(&m_KPOrg.m_color, sizeof(m_KPOrg.m_color), &dwRead);
		pFile->Read(&m_KPOrg.m_fScale, sizeof(m_KPOrg.m_fScale), &dwRead);
		pFile->Read(m_KPOrg.m_vDir.m, sizeof(m_KPOrg.m_vDir.m), &dwRead);
		pFile->Read(&m_KPOrg.m_fRad2D, sizeof(m_KPOrg.m_fRad2D), &dwRead);
		pFile->Read(&nCtrlCount, sizeof(nCtrlCount), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_interp_mode, &m_enumMode);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_timespan, &m_dwTimeSpan);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_orgpos, VECTORADDR_XYZ(m_KPOrg.m_vPos));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_color, &m_KPOrg.m_color);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_scale, &m_KPOrg.m_fScale);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_dir, QUATADDR_XYZW(m_KPOrg.m_vDir));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_rad_2d, &m_KPOrg.m_fRad2D);
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kp_method_count, &nCtrlCount);
	}

	m_KPOrg.m_vDir.Normalize();

	for (int i = 0; i < nCtrlCount; i++)
	{
		A3DGFXKeyPointCtrlBase* pCtrl =	A3DGFXKeyPointCtrlBase::LoadFromFile(pFile, dwVersion);
		if (pCtrl == NULL) return false;
		m_CtrlLst.push_back(pCtrl);
	}

	return true;
}

bool A3DGFXKeyPointCtrlBase::SaveToFile(AFile* pFile)
{
	DWORD dwWrite;

	if (pFile->IsBinary())
	{
		pFile->Write(&m_nType, sizeof(m_nType), &dwWrite);
		pFile->Write(&m_fStartTime, sizeof(m_fStartTime), &dwWrite);
		pFile->Write(&m_fEndTime, sizeof(m_fEndTime), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		
		sprintf(szLine, _ctrl_type_info, m_nType);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_start_tm, m_fStartTime);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_end_tm, m_fEndTime);
		pFile->WriteLine(szLine);
	}

	return Save(pFile);
}

A3DGFXKeyPointCtrlBase* A3DGFXKeyPointCtrlBase::CreateKPCtrl(int nType)
{
	switch (nType)
	{
	case ID_KPCTRL_MOVE:
		return new A3DGFXCtrlMove;
	case ID_KPCTRL_ROT:
		return new A3DGFXCtrlRot;
	case ID_KPCTRL_ROTAXIS:
		return new A3DGFXCtrlRotAxis;
	case ID_KPCTRL_REVOL:
		return new A3DGFXCtrlRevol;
	case ID_KPCTRL_CENTRI:
		return new A3DGFXCtrlCentriMove;
	case ID_KPCTRL_COLOR:
		return new A3DGFXCtrlColorChange;
	case ID_KPCTRL_CL_TRANS:
		return new A3DGFXCtrlClTrans;
	case ID_KPCTRL_SCALE:
		return new A3DGFXCtrlScaleChange;
	case ID_KPCTRL_SCALE_TRANS:
		return new A3DGFXCtrlScaleTrans;
	case ID_KPCTRL_CL_NOISE:
		return new A3DGFXCtrlColorNoise;
	case ID_KPCTRL_SCA_NOISE:
		return new A3DGFXCtrlScaleNoise;
	case ID_KPCTRL_CURVE_MOVE:
		return new A3DGFXCtrlCurveMove;
	case ID_KPCTRL_NOISE_BASE:
		return new A3DGFXCtrlNoiseBase;
	}
	return NULL;
}

A3DGFXKeyPointCtrlBase* A3DGFXKeyPointCtrlBase::LoadFromFile(AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	int		nType;
	DWORD	dwReadLen;
	A3DGFXKeyPointCtrlBase * pCtrl;
	float fStartTime, fEndTime;

	if (pFile->IsBinary())
	{
		pFile->Read(&nType, sizeof(nType), &dwReadLen);
		pFile->Read(&fStartTime, sizeof(fStartTime), &dwReadLen);
		pFile->Read(&fEndTime, sizeof(fEndTime), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_type_info, &nType);

		if (dwVersion >= 21)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _ctrl_start_tm, &fStartTime);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _ctrl_end_tm, &fEndTime);
		}
		else
		{
			fStartTime = 0;
			fEndTime = -1.0f;
		}
	}

	pCtrl = CreateKPCtrl(nType);
	if (pCtrl == NULL) return NULL;

	if (!pCtrl->Load(pFile, dwVersion))
	{
		delete pCtrl;
		return NULL;
	}

	pCtrl->m_fStartTime = fStartTime;
	pCtrl->m_fEndTime = fEndTime;
	return pCtrl;
}

void A3DGFXCtrlMove::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	float fDist = CalcDist(m_fVelOrg, m_fAcc, ProcData.m_fTimeSpan, fTimeSpan);
	ProcData.m_KPProcess.m_vPos += m_vMoveDir * fDist;
	ProcData.m_vAxisOff += m_vMoveDir * fDist;
}

bool A3DGFXCtrlMove::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vMoveDir.m, sizeof(m_vMoveDir.m), &dwWrite);
		pFile->Write(&m_fVelOrg, sizeof(m_fVelOrg), &dwWrite);
		pFile->Write(&m_fAcc, sizeof(m_fAcc), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_dir, VECTOR_XYZ(m_vMoveDir));
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_vel, m_fVelOrg);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_acc, m_fAcc);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlMove::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(m_vMoveDir.m, sizeof(m_vMoveDir.m), &dwRead);
		pFile->Read(&m_fVelOrg, sizeof(m_fVelOrg), &dwRead);
		pFile->Read(&m_fAcc, sizeof(m_fAcc), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_dir, VECTORADDR_XYZ(m_vMoveDir));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_vel, &m_fVelOrg);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_acc, &m_fAcc);
	}

	return true;
}

void A3DGFXCtrlRot::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	float fDist = CalcDist(m_fAngVelRot, m_fAngAccRot, ProcData.m_fTimeSpan, fTimeSpan);
	ProcData.m_KPProcess.m_fRad2D += fDist;
}

bool A3DGFXCtrlRot::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fAngVelRot, sizeof(m_fAngVelRot), &dwWrite);
		pFile->Write(&m_fAngAccRot, sizeof(m_fAngAccRot), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_vel, m_fAngVelRot);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_acc, m_fAngAccRot);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlRot::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fAngVelRot, sizeof(m_fAngVelRot), &dwRead);
		pFile->Read(&m_fAngAccRot, sizeof(m_fAngAccRot), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_vel, &m_fAngVelRot);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_acc, &m_fAngAccRot);
	}

	return true;
}

void A3DGFXCtrlRotAxis::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	float fDist = CalcDist(m_fVel, m_fAcc, ProcData.m_fTimeSpan, fTimeSpan);
	ProcData.m_KPProcess.m_vDir = A3DQUATERNION(m_vAxis, fDist) * ProcData.m_KPProcess.m_vDir;
	ProcData.m_KPProcess.m_vDir.Normalize();
	ProcData.m_KPProcess.m_vPos = a3d_RotatePosAroundLine(
		ProcData.m_KPProcess.m_vPos,
		m_vPos + ProcData.m_vAxisOff,
		m_vAxis,
		fDist);
}

bool A3DGFXCtrlRotAxis::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vPos.m, sizeof(m_vPos.m), &dwWrite);
		pFile->Write(m_vAxis.m, sizeof(m_vAxis.m), &dwWrite);
		pFile->Write(&m_fVel, sizeof(m_fVel), &dwWrite);
		pFile->Write(&m_fAcc, sizeof(m_fAcc), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_pos, VECTOR_XYZ(m_vPos));
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_axis, VECTOR_XYZ(m_vAxis));
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_vel, m_fVel);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_acc, m_fAcc);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlRotAxis::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(m_vPos.m, sizeof(m_vPos.m), &dwRead);
		pFile->Read(m_vAxis.m, sizeof(m_vAxis.m), &dwRead);
		pFile->Read(&m_fVel, sizeof(m_fVel), &dwRead);
		pFile->Read(&m_fAcc, sizeof(m_fAcc), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_pos, VECTORADDR_XYZ(m_vPos));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_axis, VECTORADDR_XYZ(m_vAxis));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_vel, &m_fVel);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_acc, &m_fAcc);
	}

	return true;
}

void A3DGFXCtrlRevol::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	float fDist = CalcDist(m_fAngVelRevol, m_fAngAccRevol, ProcData.m_fTimeSpan, fTimeSpan);
	ProcData.m_KPProcess.m_vPos = a3d_RotatePosAroundLine(
		ProcData.m_KPProcess.m_vPos,
		m_vPosRevol + ProcData.m_vAxisOff,
		m_vAxiRevol,
		fDist);
}

bool A3DGFXCtrlRevol::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vPosRevol.m, sizeof(m_vPosRevol.m), &dwWrite);
		pFile->Write(m_vAxiRevol.m, sizeof(m_vAxiRevol.m), &dwWrite);
		pFile->Write(&m_fAngVelRevol, sizeof(m_fAngVelRevol), &dwWrite);
		pFile->Write(&m_fAngAccRevol, sizeof(m_fAngAccRevol), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_pos, VECTOR_XYZ(m_vPosRevol));
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_axis, VECTOR_XYZ(m_vAxiRevol));
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_vel, m_fAngVelRevol);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_acc, m_fAngAccRevol);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlRevol::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(m_vPosRevol.m, sizeof(m_vPosRevol.m), &dwRead);
		pFile->Read(m_vAxiRevol.m, sizeof(m_vAxiRevol.m), &dwRead);
		pFile->Read(&m_fAngVelRevol, sizeof(m_fAngVelRevol), &dwRead);
		pFile->Read(&m_fAngAccRevol, sizeof(m_fAngAccRevol), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_pos, VECTORADDR_XYZ(m_vPosRevol));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_axis, VECTORADDR_XYZ(m_vAxiRevol));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_vel, &m_fAngVelRevol);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_acc, &m_fAngAccRevol);
	}

	return true;
}

void A3DGFXCtrlCentriMove::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	if (m_fVel == 0.0f) return;
	float fDist = CalcDist(m_fVel, m_fAcc, ProcData.m_fTimeSpan, fTimeSpan);
	A3DVECTOR3 vCent = m_vCentPos + ProcData.m_vAxisOff;
	A3DVECTOR3 vDir = ProcData.m_KPProcess.m_vPos - vCent;
	float fMag = vDir.Normalize();
	if (m_fVel < 0 && fMag == 0) return;
	if (fDist < 0 && fMag < -fDist)
		ProcData.m_KPProcess.m_vPos = vCent;
	else
		ProcData.m_KPProcess.m_vPos += vDir * fDist;
}

bool A3DGFXCtrlCentriMove::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vCentPos.m, sizeof(m_vCentPos.m), &dwWrite);
		pFile->Write(&m_fVel, sizeof(m_fVel), &dwWrite);
		pFile->Write(&m_fAcc, sizeof(m_fAcc), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_pos, VECTOR_XYZ(m_vCentPos));
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_vel, m_fVel);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_acc, m_fAcc);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlCentriMove::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(m_vCentPos.m, sizeof(m_vCentPos.m), &dwRead);
		pFile->Read(&m_fVel, sizeof(m_fVel), &dwRead);
		pFile->Read(&m_fAcc, sizeof(m_fAcc), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_pos, VECTORADDR_XYZ(m_vCentPos));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_vel, &m_fVel);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_acc, &m_fAcc);
	}

	return true;
}

void A3DGFXCtrlColorChange::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	A3DCOLOR& cl = ProcData.m_KPProcess.m_color;
	BYTE c[4] = {
		A3DCOLOR_GETRED(cl),
		A3DCOLOR_GETGREEN(cl),
		A3DCOLOR_GETBLUE(cl),
		A3DCOLOR_GETALPHA(cl)
	};

	for (int i = 0; i < 4; i++)
	{
		float fDelta = m_colorDelta[i] * fTimeSpan + m_clRemain[i];
		int dc = (int)(fDelta);
		m_clRemain[i] = fDelta - dc;
		if (dc > 0 && (c[i] + dc) > 255)
		{
			c[i] = 255;
			m_clRemain[i] = 0;
		}
		else if (dc < 0 && c[i] < -dc)
		{
			c[i] = 0;
			m_clRemain[i] = 0;
		}
		else
			c[i] += dc;
	}

	ProcData.m_KPProcess.m_color = A3DCOLORRGBA(c[0], c[1], c[2], c[3]);
}

bool A3DGFXCtrlColorChange::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_colorDelta, sizeof(m_colorDelta), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(
			szLine, 
			_ctrl_cl_delta, 
			m_colorDelta[0], 
			m_colorDelta[1], 
			m_colorDelta[2], 
			m_colorDelta[3]);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlColorChange::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(m_colorDelta, sizeof(m_colorDelta), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(
			szLine, 
			_ctrl_cl_delta, 
			&m_colorDelta[0], 
			&m_colorDelta[1], 
			&m_colorDelta[2], 
			&m_colorDelta[3]);
	}

	return true;
}

void A3DGFXCtrlClTrans::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan))
		return;

	if (m_nDestNum <= 0)
		return;

	int nCurSeg = 0;
	float fSpan = ProcData.m_fTimeSpan * 1000.0f;

	while (nCurSeg < m_nDestNum)
	{
		if (fSpan < m_fTransTime[nCurSeg])
			break;

		fSpan -= m_fTransTime[nCurSeg];
		nCurSeg++;
	}

	float fPortion;

	if (nCurSeg >= m_nDestNum)
	{
		nCurSeg = m_nDestNum - 1;
		fPortion = 1.0f;
	}
	else
		fPortion = fSpan / m_fTransTime[nCurSeg];

	if(fPortion > 1.0f)
		fPortion = 1.0f;

	A3DCOLOR finalColor;
	if (nCurSeg == 0)
		finalColor = Interp_Color(m_clOrg, m_clDest[0], fPortion);
	else
		finalColor = Interp_Color(m_clDest[nCurSeg-1], m_clDest[nCurSeg], fPortion);

	if (m_bAlphaOnly)
		ProcData.m_KPProcess.m_color = ((ProcData.m_KPProcess.m_color & GFX_ALPHA_MASK) | (finalColor & A3DCOLORRGBA(0, 0, 0, 255)));
	else
		ProcData.m_KPProcess.m_color = finalColor;
}

bool A3DGFXCtrlClTrans::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_clOrg, sizeof(m_clOrg), &dwWrite);
		pFile->Write(&m_nDestNum, sizeof(m_nDestNum), &dwWrite);

		for (int i = 0; i < m_nDestNum; i++)
		{
			pFile->Write(&m_clDest[i], sizeof(m_clDest[i]), &dwWrite);
			pFile->Write(&m_fTransTime[i], sizeof(m_fTransTime[i]), &dwWrite);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _kp_color, m_clOrg);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_count, m_nDestNum);
		pFile->WriteLine(szLine);

		for (int i = 0; i < m_nDestNum; i++)
		{
			sprintf(szLine, _kp_color, m_clDest[i]);
			pFile->WriteLine(szLine);

			sprintf(szLine, _kp_ftimespan, m_fTransTime[i]);
			pFile->WriteLine(szLine);
		}

		_snprintf(szLine, sizeof(szLine), _kp_alpha_only, m_bAlphaOnly);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlClTrans::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwRead;
	if (pFile->IsBinary())
	{
		pFile->Read(&m_clOrg, sizeof(m_clOrg), &dwRead);
		pFile->Read(&m_nDestNum, sizeof(m_nDestNum), &dwRead);

		for (int i = 0; i < m_nDestNum; i++)
		{
			pFile->Read(&m_clDest[i], sizeof(m_clDest[i]), &dwRead);
			pFile->Read(&m_fTransTime[i], sizeof(m_fTransTime[i]), &dwRead);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _kp_color, &m_clOrg);

		if (dwVersion < 11)
		{
			m_nDestNum = 1;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _kp_color, &m_clDest[0]);

			int nTran;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _kp_timespan, &nTran);
			m_fTransTime[0] = nTran * 1000.0f;
		}
		else
		{
			m_nDestNum = 0;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _ctrl_count, &m_nDestNum);

			for (int i = 0; i < m_nDestNum; i++)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
				if (i < _cl_tbl_max_num) sscanf(szLine, _kp_color, &m_clDest[i]);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
				if (i < _cl_tbl_max_num)
				{
					int nTran;

					if (dwVersion < 12)
					{
						sscanf(szLine, _kp_timespan, &nTran);
						m_fTransTime[i] = (float)nTran;
					}
					else
						sscanf(szLine, _kp_ftimespan, &m_fTransTime[i]);
				}
			}
		}

		if (dwVersion >= 81)
		{
			int iRead = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _kp_alpha_only, &iRead);
			m_bAlphaOnly = (iRead ? true : false);
		}
	}
	
	return true;
}

A3DGFXCtrlScaleTrans::A3DGFXCtrlScaleTrans()
{
	m_nType			= ID_KPCTRL_SCALE_TRANS;
	m_nDestNum		= 0;
	
	memset(m_DestScales, 0, sizeof(m_DestScales));
	memset(m_fTransTime, 0, sizeof(m_fTransTime));
}

void A3DGFXCtrlScaleTrans::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan))
		return;

	if (m_nDestNum <= 0)
		return;

	int nCurSeg = 0;
	float fSpan = ProcData.m_fTimeSpan * 1000.0f, fPortion, fR;

	while (nCurSeg < m_nDestNum)
	{
		if (fSpan < m_fTransTime[nCurSeg])
			break;

		fSpan -= m_fTransTime[nCurSeg];
		nCurSeg++;
	}

	if (nCurSeg >= m_nDestNum)
	{
		ProcData.m_KPProcess.m_fScale = m_DestScales[nCurSeg];
		return;
	}

	fPortion = fSpan / m_fTransTime[nCurSeg];
	fR = 1.0f - fPortion;
	ProcData.m_KPProcess.m_fScale = m_DestScales[nCurSeg] * fR + m_DestScales[nCurSeg+1] * fPortion;
}

bool A3DGFXCtrlScaleTrans::Save(AFile* pFile)
{
	DWORD dwWrite;

	if (pFile->IsBinary())
	{
		pFile->Write(&m_DestScales[0], sizeof(float), &dwWrite);
		pFile->Write(&m_nDestNum, sizeof(m_nDestNum), &dwWrite);

		for (int i = 0; i < m_nDestNum; i++)
		{
			pFile->Write(&m_DestScales[i+1], sizeof(float), &dwWrite);
			pFile->Write(&m_fTransTime[i], sizeof(float), &dwWrite);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _kp_scale, m_DestScales[0]);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_count, m_nDestNum);
		pFile->WriteLine(szLine);

		for (int i = 0; i < m_nDestNum; i++)
		{
			sprintf(szLine, _kp_scale, m_DestScales[i+1]);
			pFile->WriteLine(szLine);

			sprintf(szLine, _kp_ftimespan, m_fTransTime[i]);
			pFile->WriteLine(szLine);
		}
	}

	return true;
}

bool A3DGFXCtrlScaleTrans::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwRead;
	m_nDestNum = 0;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_DestScales[0], sizeof(float), &dwRead);
		pFile->Read(&m_nDestNum, sizeof(m_nDestNum), &dwRead);

		for (int i = 0; i < m_nDestNum; i++)
		{
			pFile->Read(&m_DestScales[i+1], sizeof(float), &dwRead);
			pFile->Read(&m_fTransTime[i], sizeof(float), &dwRead);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _kp_scale, &m_DestScales[0]);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _ctrl_count, &m_nDestNum);

		for (int i = 0; i < m_nDestNum; i++)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _kp_scale, &m_DestScales[i+1]);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _kp_ftimespan, &m_fTransTime[i]);
		}
	}

	return true;
}

void A3DGFXCtrlScaleChange::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	ProcData.m_KPProcess.m_fScale += m_fScaleDelta * fTimeSpan;
	if (ProcData.m_KPProcess.m_fScale < m_fMinScale) ProcData.m_KPProcess.m_fScale = m_fMinScale;
	else if (ProcData.m_KPProcess.m_fScale > m_fMaxScale) ProcData.m_KPProcess.m_fScale = m_fMaxScale;
}

bool A3DGFXCtrlScaleChange::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fScaleDelta, sizeof(m_fScaleDelta), &dwWrite);
		pFile->Write(&m_fMinScale, sizeof(m_fMinScale), &dwWrite);
		pFile->Write(&m_fMaxScale, sizeof(m_fMaxScale), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_scale, m_fScaleDelta, m_fMinScale, m_fMaxScale);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlScaleChange::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fScaleDelta, sizeof(m_fScaleDelta), &dwRead);
		pFile->Read(&m_fMinScale, sizeof(m_fMinScale), &dwRead);
		pFile->Read(&m_fMaxScale, sizeof(m_fMaxScale), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_scale, &m_fScaleDelta, &m_fMinScale, &m_fMaxScale);
	}

	return true;
}

bool A3DGFXCtrlNoiseBase::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_nBufLen, sizeof(m_nBufLen), &dwWrite);
		pFile->Write(&m_fAmplitude, sizeof(m_fAmplitude), &dwWrite);
		pFile->Write(&m_nWaveLen, sizeof(m_nWaveLen), &dwWrite);
		pFile->Write(&m_fPersistence, sizeof(m_fPersistence), &dwWrite);
		pFile->Write(&m_nOctaveNum, sizeof(m_nOctaveNum), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_noise_buflen, m_nBufLen);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_noise_amp, m_fAmplitude);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_noise_wavlen, m_nWaveLen);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_noise_persist, m_fPersistence);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_noise_octnum, m_nOctaveNum);
		pFile->WriteLine(szLine);
	}
	
	return true;
}

bool A3DGFXCtrlNoiseBase::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_nBufLen, sizeof(m_nBufLen), &dwRead);
		pFile->Read(&m_fAmplitude, sizeof(m_fAmplitude), &dwRead);
		pFile->Read(&m_nWaveLen, sizeof(m_nWaveLen), &dwRead);
		pFile->Read(&m_fPersistence, sizeof(m_fPersistence), &dwRead);
		pFile->Read(&m_nOctaveNum, sizeof(m_nOctaveNum), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_noise_buflen, &m_nBufLen);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_noise_amp, &m_fAmplitude);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_noise_wavlen, &m_nWaveLen);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_noise_persist, &m_fPersistence);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_noise_octnum, &m_nOctaveNum);
	}

	return true;
}

void A3DGFXCtrlColorNoise::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	static const A3DCOLOR cMask = A3DCOLORRGBA(255, 255, 255, 0);
	float fNoise = 0;
	m_PerlinNoise.GetValue(ProcData.m_fTimeSpan * 1000.f, &fNoise, 1);

	BYTE c = A3DCOLOR_GETALPHA(m_clBase);
	int dc = (int)fNoise;
	if (dc > 0 && (c + dc) > 255)
		c = 255;
	else if (dc < 0 && c < -dc)
		c = 0;
	else
		c += dc;

	ProcData.m_KPProcess.m_color = (m_clBase & cMask) + A3DCOLORRGBA(0, 0, 0, c);
}

bool A3DGFXCtrlColorNoise::Save(AFile* pFile)
{
	if (!A3DGFXCtrlNoiseBase::Save(pFile)) return false;

	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_clBase, sizeof(m_clBase), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_noise_basecl, m_clBase);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXCtrlColorNoise::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DGFXCtrlNoiseBase::Load(pFile, dwVersion)) return false;

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_clBase, sizeof(m_clBase), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_noise_basecl, &m_clBase);
	}

	return true;
}

void A3DGFXCtrlScaleNoise::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	float fNoise = 0;
	m_PerlinNoise.GetValue(ProcData.m_fTimeSpan * 1000.f, &fNoise, 1);
	ProcData.m_fScaleNoise = fNoise;
}

bool A3DGFXCtrlScaleNoise::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DGFXCtrlNoiseBase::Load(pFile, dwVersion)) return false;

	if (dwVersion < 13)
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	}

	return true;
}

void A3DGFXCtrlCurveMove::Process(PROC_DATA& ProcData, float fTimeSpan)
{
	if (!Tick(ProcData.m_fTimeSpan)) return;

	if (!m_ulCount) return;
	float fRatio = ProcData.m_fTimeSpan / m_fTimeSpan;
	if (fRatio > 1.f) fRatio = 1.f;

	A3DVECTOR3 vNow;
	A3DQUATERNION vDir;

	if (!GetInterpPoint(fRatio, vNow, vDir, m_ulCurPoint))
		return;

	ProcData.m_vAxisOff += vNow - ProcData.m_KPProcess.m_vPos;
	ProcData.m_KPProcess.m_vPos = vNow;

	if (m_pDir)
		ProcData.m_KPProcess.m_vDir = vDir;
}

bool A3DGFXCtrlCurveMove::Save(AFile* pFile)
{
	size_t uCount = m_VertArray.size();

	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&uCount, sizeof(uCount), &dwWrite);
		for (size_t i = 0; i < uCount; i++)
			pFile->Write(m_VertArray[i].m, sizeof(m_VertArray[i].m), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _ctrl_calc_dir, m_bCalcDir);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ctrl_count, uCount);
		pFile->WriteLine(szLine);

		for (size_t i = 0; i < uCount; i++)
		{
			sprintf(szLine, _ctrl_pos, VECTOR_XYZ(m_VertArray[i]));
			pFile->WriteLine(szLine);
		}
	}

	return true;
}

bool A3DGFXCtrlCurveMove::Load(AFile* pFile, DWORD dwVersion)
{
	size_t uCount = 0;
	DWORD dwRead;

	if (pFile->IsBinary())
	{
		pFile->Read(&uCount, sizeof(uCount), &dwRead);

		A3DVECTOR3 vPos;
		for (size_t i = 0; i < uCount; i++)
		{
			pFile->Read(vPos.m, sizeof(vPos.m), &dwRead);
			m_VertArray.push_back(vPos);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		if (dwVersion >= 44)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _ctrl_calc_dir, &dwRead);
			m_bCalcDir = (dwRead != 0);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _ctrl_count, &uCount);

		A3DVECTOR3 vPos;
		for (size_t i = 0; i < uCount; i++)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _ctrl_pos, VECTORADDR_XYZ(vPos));
			m_VertArray.push_back(vPos);
		}
	}

	GenPath();
	return true;
}

static const size_t _div_num = 5;

void A3DGFXCtrlCurveMove::GenPath()
{
	if (m_VertArray.size() < 3) return;

	if (m_pBezier)
	{
		delete[] m_pBezier;
		m_pBezier = NULL;
	}

	if (m_pDir)
	{
		delete[] m_pDir;
		m_pDir = NULL;
	}
	
	size_t uBufSz = 3 * m_VertArray.size() - 2;
	size_t uVertSz = m_VertArray.size();

	A3DVECTOR3* pCorner = new A3DVECTOR3[uBufSz];
	pCorner[0] = m_VertArray[0];
	pCorner[uBufSz-1] = m_VertArray[uVertSz-1];

	size_t i;

	for (i = 1; i < m_VertArray.size()-1; i++)
	{
		A3DVECTOR3 vDiff = (m_VertArray[i+1] - m_VertArray[i-1]) / 6.f;
		size_t uBase = 2 + (i - 1) * 3;
		pCorner[uBase] = m_VertArray[i] - vDiff;
		pCorner[uBase+1] = m_VertArray[i];
		pCorner[uBase+2] = m_VertArray[i] + vDiff;
	}

	pCorner[1] = pCorner[2] + (m_VertArray[0] - m_VertArray[1]) / 3.f;
	pCorner[uBufSz-2] = pCorner[uBufSz-3] + (m_VertArray[uVertSz-1] - m_VertArray[uVertSz-2]) / 3.f;

	m_ulCount = (uVertSz-1)*_div_num+uVertSz;
	m_pBezier = new BezierPt[m_ulCount];

	if (m_bCalcDir)
		m_pDir = new A3DQUATERNION[m_ulCount];

	for (i = 0; i < uVertSz-1; i++)
	{
		size_t uBase = i * 3;
		size_t uInsert = i * (_div_num + 1);

		m_pBezier[uInsert].m_vec = m_VertArray[i];
		for (size_t j = 1; j <= _div_num; j++)
		{
			m_pBezier[uInsert+j].m_vec = Bezier4(
				pCorner[uBase],
				pCorner[uBase+1],
				pCorner[uBase+2],
				pCorner[uBase+3],
				j / (float)(_div_num + 1));
		}
	}

	m_pBezier[m_ulCount-1].m_vec = m_VertArray[uVertSz-1];
	m_pBezier[0].m_fRatio = 0;

	for (i = 1; i < m_ulCount; i++)
	{
		A3DVECTOR3 vDir = m_pBezier[i].m_vec - m_pBezier[i-1].m_vec;
		float fMag = vDir.Normalize();
		m_pBezier[i].m_fRatio = m_pBezier[i-1].m_fRatio + fMag;

		if (m_pDir)
			m_pDir[i-1] = A3DQUATERNION(_build_matrix(vDir, _unit_zero));
	}

	m_fTotalLen = m_pBezier[m_ulCount-1].m_fRatio;

	if (m_pDir)
		m_pDir[m_ulCount-1] = m_pDir[m_ulCount-2];

	for (i = 1; i < m_ulCount; i++)
		m_pBezier[i].m_fRatio /= m_pBezier[m_ulCount-1].m_fRatio;

	delete[] pCorner;
}

A3DGFXKeyPointCtrlBase* A3DGFXKeyPoint::AddCtrlMethodByType(int nType)
{
	A3DGFXKeyPointCtrlBase* pMethod = A3DGFXKeyPointCtrlBase::CreateKPCtrl(nType);
	if (pMethod == NULL) return NULL;
	m_CtrlLst.push_back(pMethod);
	return pMethod;
}

void A3DGFXKeyPointSet::TickAnimation(DWORD dwTickTime)
{
	if (IsFinished())
		return;

	if (m_bInDelay)
	{
		m_dwCurDelay += dwTickTime;

		if (m_dwCurDelay >= m_dwRepeatDelay)
		{
			m_dwCurDelay = 0;
			m_bInDelay = false;
		}
		else
			return;
	}

	m_dwTimeSpan += dwTickTime;

	if (m_dwTimeSpan < m_dwTimeStart)
		return;

	DWORD dwSpan = m_dwTimeSpan - m_dwTimeStart;
	DWORD dwTotal = 0;
	size_t i;

	for (i = 0; i < m_KeyPointArray.size(); i++)
	{
		if (m_KeyPointArray[i]->IsInfinite())
			break;

		DWORD dwCur = m_KeyPointArray[i]->m_dwTimeSpan;

		if (dwSpan < dwCur)
			break;

		dwSpan -= dwCur;
		dwTotal += dwCur;
	}

	if (i == m_KeyPointArray.size())
	{
		Resume(true);
		m_pEle->ResumeLoop();

		if (dwTotal)
			TickAnimation(dwSpan % dwTotal);

		return;
	}

	if (m_KeyPointArray[i]->m_enumMode != A3DGFXKeyPoint::enumInterpNone)
	{
		m_nActKeyPoint = i;
		m_ProcData.m_fTimeSpan = dwSpan / 1000.f;

		if (i == m_KeyPointArray.size()-1)
			m_ProcData.m_KPProcess = m_KeyPointArray[i]->m_KPOrg;
		else
			Interpolate(((float)dwSpan) / m_KeyPointArray[i]->m_dwTimeSpan);
	}
	else
	{
		if (m_nActKeyPoint != (int)i)
		{
			if (m_nActKeyPoint >= 0)
			{
				DWORD t = DWORD(m_ProcData.m_fTimeSpan * 1000.0f);
				A3DGFXKeyPoint* pCur = m_KeyPointArray[m_nActKeyPoint];
				m_ProcData.m_fTimeSpan = pCur->m_dwTimeSpan / 1000.0f;
				assert(pCur->m_dwTimeSpan >= t);
				pCur->Process(m_ProcData, pCur->m_dwTimeSpan - t);
			}

			for (size_t j = m_nActKeyPoint + 1; j < i; j++)
			{
				A3DGFXKeyPoint* pPrev = m_KeyPointArray[j];
				m_ProcData.m_fTimeSpan = pPrev->m_dwTimeSpan / 1000.0f;
				pPrev->Process(m_ProcData, pPrev->m_dwTimeSpan);
			}

			m_nActKeyPoint = i;
			dwTickTime = dwSpan;
		}

		m_ProcData.m_fTimeSpan = dwSpan / 1000.f;
		m_KeyPointArray[i]->Process(m_ProcData, dwTickTime);
	}
}

void A3DGFXKeyPointSet::Interpolate(float fPortion)
{
	const KEY_POINT& kp1 = m_KeyPointArray[m_nActKeyPoint]->m_KPOrg;
	const KEY_POINT& kp2 = m_KeyPointArray[m_nActKeyPoint+1]->m_KPOrg;

	float fRev = 1.0f - fPortion;
	m_ProcData.m_KPProcess.m_color = A3DCOLORRGBA(
		(int) (A3DCOLOR_GETRED(kp2.m_color) * fPortion + A3DCOLOR_GETRED(kp1.m_color) * fRev),
		(int) (A3DCOLOR_GETGREEN(kp2.m_color) * fPortion + A3DCOLOR_GETGREEN(kp1.m_color) * fRev),
		(int) (A3DCOLOR_GETBLUE(kp2.m_color) * fPortion + A3DCOLOR_GETBLUE(kp1.m_color) * fRev),
		(int) (A3DCOLOR_GETALPHA(kp2.m_color) * fPortion + A3DCOLOR_GETALPHA(kp1.m_color) * fRev));
	m_ProcData.m_KPProcess.m_fRad2D = kp2.m_fRad2D * fPortion + kp1.m_fRad2D * fRev;
	m_ProcData.m_KPProcess.m_fScale = kp2.m_fScale * fPortion + kp1.m_fScale * fRev;
	m_ProcData.m_KPProcess.m_vPos = kp2.m_vPos * fPortion + kp1.m_vPos * fRev;
	if (m_KeyPointArray[m_nActKeyPoint]->m_enumMode == A3DGFXKeyPoint::enumInterpAll)
		m_ProcData.m_KPProcess.m_vDir = SLERPQuad(kp1.m_vDir, kp2.m_vDir, fPortion);
}

bool A3DGFXKeyPointSet::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_dwTimeStart, sizeof(m_dwTimeStart), &dwWrite);
		size_t sz = m_KeyPointArray.size();
		pFile->Write(&sz, sizeof(sz), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _kpset_starttime, m_dwTimeStart);
		pFile->WriteLine(szLine);

		sprintf(szLine, _kpset_load_format, m_KeyPointArray.size());
		pFile->WriteLine(szLine);
	}

	for (size_t i = 0; i < m_KeyPointArray.size(); i++)
		m_KeyPointArray[i]->Save(pFile);

	return true;
}

bool A3DGFXKeyPointSet::Load(AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	int		nKeyPointCount;
	A3DGFXKeyPoint* pKeyPoint;

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_dwTimeStart, sizeof(m_dwTimeStart), &dwRead);
		pFile->Read(&nKeyPointCount, sizeof(nKeyPointCount), &dwRead);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kpset_starttime, &m_dwTimeStart);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _kpset_load_format, &nKeyPointCount);
	}

	for (int i = 0; i < nKeyPointCount; i++)
	{
		pKeyPoint = new A3DGFXKeyPoint;
		if (!pKeyPoint->Load(pFile, dwVersion))
		{
			delete pKeyPoint;
			return false;
		}
		m_KeyPointArray.push_back(pKeyPoint);
	}

	return true;
}
