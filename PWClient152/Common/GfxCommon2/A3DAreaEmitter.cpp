#include "StdAfx.h"
#include "A3DAreaEmitter.h"
#include "A3DGFXKeyPoint.h"
#include "A3DGFXElement.h"

static const char _format_size[]	=	"AreaSize: %f, %f, %f";

void A3DAreaEmitter::UpdateEmitterScale(float fScale)
{
	if (m_bBind)
	{
		m_vXRange = _unit_x * (m_vSize.x * .5f);
		m_vYRange = _unit_y * (m_vSize.y * .5f);
		m_vZRange = _unit_z * (m_vSize.z * .5f);
	}
	else
	{
		A3DVECTOR3 vSize = m_vSize * fScale;
		m_vXRange = RotateVec(m_qParentDir, _unit_x) * (vSize.x * .5f);
		m_vYRange = RotateVec(m_qParentDir, _unit_y) * (vSize.y * .5f);
		m_vZRange = RotateVec(m_qParentDir, _unit_z) * (vSize.z * .5f);
	}
}

bool A3DAreaEmitter::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DParticleEmitter::Load(pFile, dwVersion))
		return false;

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(m_vSize.m, sizeof(m_vSize.m), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_size, VECTORADDR_XYZ(m_vSize));
	}

	return true;
}

bool A3DAreaEmitter::Save(AFile* pFile)
{
	if (!A3DParticleEmitter::Save(pFile))
		return false;

	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vSize.m, sizeof(m_vSize.m), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_size, VECTOR_XYZ(m_vSize));
		pFile->WriteLine(szLine);
	}

	return true;
}

void A3DAreaEmitter::CloneFrom(const A3DParticleEmitter* p)
{
	A3DParticleEmitter::CloneFrom(p);
	m_vSize = static_cast<const A3DAreaEmitter*>(p)->m_vSize;
}
