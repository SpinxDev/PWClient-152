#include "stdafx.h"
#include "EC_ModelHook.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static const char * s_formatECMHookName = "ECMHook: %s";
static const char * s_formatECMHookID = "ID: %d";
static const char * s_formatECMHookScale = "Scale: %f";
static const char * s_formatECMHookVec = "%f %f %f";

CECModelHook::CECModelHook()
: m_matRelative(a3d_IdentityMatrix())
{
	m_nID = 0;
	m_fScale = 1.f;
}

CECModelHook::~CECModelHook()
{
	
}

//	Save / Load ECModelHook
bool CECModelHook::Save(AFile* pFileToSave)
{
	if (!pFileToSave)
		return false;

	char szBuf[AFILE_LINEMAXLEN];
	_snprintf(szBuf, AFILE_LINEMAXLEN, s_formatECMHookName, m_strName);
	pFileToSave->WriteLine(szBuf);

	_snprintf(szBuf, AFILE_LINEMAXLEN, s_formatECMHookID, m_nID);
	pFileToSave->WriteLine(szBuf);

	_snprintf(szBuf, AFILE_LINEMAXLEN, s_formatECMHookScale, m_fScale);
	pFileToSave->WriteLine(szBuf);

	for (int iRow = 0; iRow < 4; ++iRow)
	{
		A3DVECTOR3 vRow = m_matRelative.GetRow(iRow);
		_snprintf(szBuf, AFILE_LINEMAXLEN, s_formatECMHookVec, vRow.x, vRow.y, vRow.z);
		pFileToSave->WriteLine(szBuf);
	}
	
	return true;
}

bool CECModelHook::Load(AFile* pFileToLoad, DWORD dwVersion)
{
	if (!pFileToLoad)
		return false;

	char szBuf[AFILE_LINEMAXLEN];
	char szName[AFILE_LINEMAXLEN];
	DWORD dwReadLength;
	pFileToLoad->ReadLine(szBuf, AFILE_LINEMAXLEN, &dwReadLength);
	sscanf(szBuf, s_formatECMHookName, szName);
	m_strName = szName;

	pFileToLoad->ReadLine(szBuf, AFILE_LINEMAXLEN, &dwReadLength);
	sscanf(szBuf, s_formatECMHookID, &m_nID);

	pFileToLoad->ReadLine(szBuf, AFILE_LINEMAXLEN, &dwReadLength);
	sscanf(szBuf, s_formatECMHookScale, &m_fScale);

	for (int iRow = 0; iRow < 4; ++iRow)
	{
		A3DVECTOR3 vRow;
		pFileToLoad->ReadLine(szBuf, AFILE_LINEMAXLEN, &dwReadLength);
		sscanf(szBuf, s_formatECMHookVec, &vRow.x, &vRow.y, &vRow.z);
		m_matRelative.SetRow(iRow, vRow);
	}

	return true;
}
