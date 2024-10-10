#include "StdAfx.h"
#include "A3DLtnBolt.h"
#include "A3DGFXEditorInterface.h"
#include "A3DMacros.h"

static const char _format_deviation[]	= "Deviation: %f";
static const char _format_step_min[]	= "StepMin: %f";
static const char _format_step_max[]	= "StepMax: %f";
static const char _format_width_start[]	= "WidthStart: %f";
static const char _format_width_end[]	= "WidthEnd: %f";
static const char _format_alpha_start[]	= "AlphaStart: %f";
static const char _format_alpha_end[]	= "AlphaEnd: %f";
static const char _format_stroke_amp[]	= "StrokeAmp: %f";
static const char _format_max_steps[]	= "MaxSteps: %d";
static const char _format_max_branches[]= "MaxBranches: %d";
static const char _format_interval[]	= "Interval: %d";
static const char _format_per_bolts[]	= "PerBolts: %d";
static const char _format_circles[]		= "Circles: %d";

A3DGFXElement* A3DGFXLtnBolt::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXLtnBolt* p = new A3DGFXLtnBolt(pGfx);
	return &(*p = *this);
}

A3DGFXLtnBolt& A3DGFXLtnBolt::operator = (const A3DGFXLtnBolt& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	m_pDevice		= src.m_pDevice;
	m_fDeviation	= src.m_fDeviation;
	m_fStepLenMin	= src.m_fStepLenMin;
	m_fStepLenMax	= src.m_fStepLenMax;
	m_fWidthStart	= src.m_fWidthStart;
	m_fWidthEnd		= src.m_fWidthEnd;
	m_fStartAlpha	= src.m_fStartAlpha;
	m_fEndAlpha		= src.m_fEndAlpha;
	m_fStrokeAmp	= src.m_fStrokeAmp;
	m_nMaxSteps		= src.m_nMaxSteps;
	m_nMaxBraches	= src.m_nMaxBraches;
	m_dwInterval	= src.m_dwInterval;
	m_nPerBolts		= src.m_nPerBolts;
	m_nCircles		= src.m_nCircles;
	m_nTotalBolts	= src.m_nTotalBolts;

	return *this;
}

bool A3DGFXLtnBolt::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_fDeviation, sizeof(m_fDeviation), &dwReadLen);
		pFile->Read(&m_fStepLenMin, sizeof(m_fStepLenMin), &dwReadLen);
		pFile->Read(&m_fStepLenMax, sizeof(m_fStepLenMax), &dwReadLen);
		pFile->Read(&m_fWidthStart, sizeof(m_fWidthStart), &dwReadLen);
		pFile->Read(&m_fWidthEnd, sizeof(m_fWidthEnd), &dwReadLen);
		pFile->Read(&m_fStartAlpha, sizeof(m_fStartAlpha), &dwReadLen);
		pFile->Read(&m_fEndAlpha, sizeof(m_fEndAlpha), &dwReadLen);
		pFile->Read(&m_fStrokeAmp, sizeof(m_fStrokeAmp), &dwReadLen);
		pFile->Read(&m_nMaxSteps, sizeof(m_nMaxSteps), &dwReadLen);
		pFile->Read(&m_nMaxBraches, sizeof(m_nMaxBraches), &dwReadLen);
		pFile->Read(&m_dwInterval, sizeof(m_dwInterval), &dwReadLen);
		pFile->Read(&m_nPerBolts, sizeof(m_nPerBolts), &dwReadLen);
		pFile->Read(&m_nCircles, sizeof(m_nCircles), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_deviation, &m_fDeviation);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_step_min, &m_fStepLenMin);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_step_max, &m_fStepLenMax);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_width_start, &m_fWidthStart);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_width_end, &m_fWidthEnd);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_alpha_start, &m_fStartAlpha);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_alpha_end, &m_fEndAlpha);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_stroke_amp, &m_fStrokeAmp);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_max_steps, &m_nMaxSteps);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_max_branches, &m_nMaxBraches);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_interval, &m_dwInterval);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_per_bolts, &m_nPerBolts);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_circles, &m_nCircles);
	}

	Init(pDevice);
	
	return true;
}

bool A3DGFXLtnBolt::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fDeviation, sizeof(m_fDeviation), &dwWrite);
		pFile->Write(&m_fStepLenMin, sizeof(m_fStepLenMin), &dwWrite);
		pFile->Write(&m_fStepLenMax, sizeof(m_fStepLenMax), &dwWrite);
		pFile->Write(&m_fWidthStart, sizeof(m_fWidthStart), &dwWrite);
		pFile->Write(&m_fWidthEnd, sizeof(m_fWidthEnd), &dwWrite);
		pFile->Write(&m_fStartAlpha, sizeof(m_fStartAlpha), &dwWrite);
		pFile->Write(&m_fEndAlpha, sizeof(m_fEndAlpha), &dwWrite);
		pFile->Write(&m_fStrokeAmp, sizeof(m_fStrokeAmp), &dwWrite);
		pFile->Write(&m_nMaxSteps, sizeof(m_nMaxSteps), &dwWrite);
		pFile->Write(&m_nMaxBraches, sizeof(m_nMaxBraches), &dwWrite);
		pFile->Write(&m_dwInterval, sizeof(m_dwInterval), &dwWrite);
		pFile->Write(&m_nPerBolts, sizeof(m_nPerBolts), &dwWrite);
		pFile->Write(&m_nCircles, sizeof(m_nCircles), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_deviation, m_fDeviation);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_step_min, m_fStepLenMin);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_step_max, m_fStepLenMax);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_width_start, m_fWidthStart);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_width_end, m_fWidthEnd);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_alpha_start, m_fStartAlpha);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_alpha_end, m_fEndAlpha);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_stroke_amp, m_fStrokeAmp);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_max_steps, m_nMaxSteps);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_max_branches, m_nMaxBraches);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_interval, m_dwInterval);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_per_bolts, m_nPerBolts);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_circles, m_nCircles);
		pFile->WriteLine(szLine);
	}

	return true;
}

void A3DGFXLtnBolt::BuildStrokeDir2D()
{
	int nBolts = m_nPerBolts << 1;
	float fDelta = A3D_2PI / nBolts;

	for (int i = 0; i < nBolts; i++)
	{
		float fAngle = i * fDelta;
		m_pRandDir[i].x = (float)sin(fAngle);
		m_pRandDir[i].y = (float)cos(fAngle);
		m_pRandDir[i].z = 0;
		m_pRandUp[i] = _unit_z;
	}
}

void A3DGFXLtnBolt::BuildStrokeDir3D()
{
	BuildStrokeDir2D();
	if (m_nCircles <= 1) return;

	float fDelta = A3D_PI / m_nCircles;
	int nBolts = m_nPerBolts << 1;
	int nCount = nBolts;

	for (int i = 1; i < m_nCircles; i++)
	{
		A3DQUATERNION q(_unit_y, fDelta * i);
		for (int j = 1; j < nBolts; j++)
		{
			if (j == m_nPerBolts) continue;
			m_pRandDir[nCount] = RotateVec(q, m_pRandDir[j]);
			m_pRandUp[nCount] = RotateVec(q, m_pRandUp[j]);
			nCount++;
		}
	}
}

void A3DGFXLtnBolt::BuildStrokeDir()
{
	if (m_nPerBolts < 1 || m_nCircles < 1) return;
	if (m_pRandDir) delete[] m_pRandDir;
	if (m_pRandUp) delete[] m_pRandUp;

	m_nTotalBolts = m_nCircles * (m_nPerBolts - 1) * 2 + 2;
	m_pRandDir = new A3DVECTOR3[m_nTotalBolts];
	m_pRandUp = new A3DVECTOR3[m_nTotalBolts];

	BuildStrokeDir3D();
}

void A3DGFXLtnBolt::BuildPath(float fScale)
{
	Resume();

	for (int n = 0; n < m_nTotalBolts; n++)
	{
		m_vDir = m_pRandDir[n];
		m_vUp = m_pRandUp[n];
		AddPath(1, 1, _unit_zero, fScale, rand());
	}

	for (size_t i = 0; i < m_Strokes.size(); i++)
		m_Strokes[i]->Play();
}

void A3DGFXLtnBolt::AddPath(int nStep, int nBranch, const A3DVECTOR3& vStart, float fScale, int nNoisePos)
{
	if (nStep > m_nMaxSteps) return;

	float fRatio = _UnitRandom();
	float fStepLen = m_fStepLenMin * (1.f - fRatio) + m_fStepLenMax * fRatio;
	fStepLen *= fScale;

	A3DVECTOR3 vEnd = vStart + fStepLen * m_vDir;
	A3DGFXLightning* pStroke = new A3DGFXLightning(m_pGfx);
	A3DGFXKeyPoint* pKeyPoint = new A3DGFXKeyPoint;
	KEY_POINT kp;
	pKeyPoint->SetData(kp.Init(), -1);
	pStroke->AddKeyPoint(pKeyPoint);
	pStroke->Init(m_pDevice);
	pStroke->SetTotalBind(true);
	pStroke->SetBindEle(this);
	float fMaxSteps = (float)m_nMaxSteps;
	float fRatioStart = (nStep - 1) / fMaxSteps;
	float fRatioEnd = nStep / fMaxSteps;
	float fWidthStart = m_fWidthStart * (1.f - fRatioStart) + m_fWidthEnd * fRatioStart;
	float fWidthEnd	  = m_fWidthStart * (1.f - fRatioEnd)   + m_fWidthEnd * fRatioEnd;
	float fStartAlpha = m_fStartAlpha * (1.f - fRatioStart) + m_fEndAlpha * fRatioStart;
	float fEndAlpha   = m_fStartAlpha * (1.f - fRatioEnd)   + m_fEndAlpha * fRatioEnd;
	pStroke->SetDeviation(m_fDeviation);
	pStroke->SetNoiseData(256, 2.f, 1, .8f, 5);
	pStroke->SetData(vStart, vEnd, 8, 1, fStepLen / 3.f, m_dwInterval, fWidthStart, fWidthEnd, fStartAlpha, fEndAlpha, m_fStrokeAmp);
	m_Strokes.push_back(pStroke);

	nStep++;
	if (nBranch < m_nMaxBraches)
	{
		nBranch++;
		float fProb = m_nMaxBraches / (float)m_nMaxSteps;
		if (_UnitRandom() < fProb)
			AddPath(nStep, nBranch, vEnd, fScale, nNoisePos++);
	}

	AddPath(nStep, nBranch, vEnd, fScale, nNoisePos++);
}

bool A3DGFXLtnBolt::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime)) return false;

	for (size_t i = 0; i < m_Strokes.size(); i++)
		if (m_Strokes[i]->IsInit())
			m_Strokes[i]->TickAnimation(dwTickTime);

	return true;
}

bool A3DGFXLtnBolt::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	for (size_t i = 0; i < m_Strokes.size(); i++)
		if (m_Strokes[i]->IsInit())
			m_Strokes[i]->Render(pView);

	return true;
}
