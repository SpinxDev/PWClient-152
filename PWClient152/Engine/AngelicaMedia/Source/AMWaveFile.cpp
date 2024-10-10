/*
 * FILE: AMWaveFile.cpp
 *
 * DESCRIPTION: class for operate wave file
 *
 * CREATED BY: Hedi, 2002/1/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMWaveFile.h"
#include "AMPI.h"
#include "AFile.h"

AMWaveFile::AMWaveFile()
{
	m_szFilename[0] = '\0';
	m_pAFile = NULL;
	m_nTotalSample	= 0;
	m_dwDataSize	= 0;
}

AMWaveFile::~AMWaveFile()
{
}

bool AMWaveFile::Open(AFile* pAFile)
{
	strncpy(m_szFilename, pAFile->GetFileName(), MAX_PATH);

	//	Open a wavefile using file system functions.
	m_pAFile = pAFile;

	//	Read wave file header, now only strict use the rules files are surported.
	DWORD dwReadLength;

	m_pAFile->Read(&m_waveFileHeader, sizeof (WAVEFILEHEADER), &dwReadLength);

	if (dwReadLength != sizeof (WAVEFILEHEADER))
	{
		AMERRLOG(("AMWaveFile::Open, File [%s] has corrupt!", m_szFilename));
		return false;
	}

	//	Check wave form type!
	if (m_waveFileHeader.dwFileType != FOURCC_RIFF || m_waveFileHeader.dwFormType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		AMERRLOG(("AMWaveFile::Open, File [%s]'s wave header type error!", m_szFilename));
		return false;
	}
	
	if (m_waveFileHeader.dwFmt != mmioFOURCC('f', 'm', 't', ' '))
	{
		AMERRLOG(("AMWaveFile::Open, File [%s]'s wave format error!", m_szFilename));
		return false;
	}
	
	if (m_waveFileHeader.dwFmtSize < sizeof (PCMWAVEFORMAT))
	{
		AMERRLOG(("AMWaveFile::Open, File [%s]'s wave header size error!", m_szFilename));
		return false;
	}
	
	m_dwData = 0;
	m_dwDataBegin = m_pAFile->GetPos();

	m_pAFile->Read(&m_dwData, sizeof (DWORD), &dwReadLength);
	if (dwReadLength != sizeof (DWORD))
	{
		AMERRLOG(("AMWaveFile::Open, File [%s] has corrupt!", m_szFilename));
		return false;
	}

	while (m_dwData != mmioFOURCC('d', 'a', 't', 'a'))
	{
		m_dwDataBegin++;
		m_pAFile->Seek(m_dwDataBegin, AFILE_SEEK_SET);
		m_pAFile->Read(&m_dwData, sizeof (DWORD), &dwReadLength);

		if (dwReadLength != sizeof (DWORD))
		{
			AMERRLOG(("AMWaveFile::Open, File [%s] has corrupt!", m_szFilename));
			return false;
		}
	}

	m_pAFile->Read(&m_dwDataSize, sizeof (DWORD), &dwReadLength);
	if (dwReadLength != sizeof (DWORD))
	{
		AMERRLOG(("AMWaveFile::Open, File [%s] has corrupt!", m_szFilename));
		return false;
	}

    m_dwDataBegin = m_pAFile->GetPos();

	//OK, now the wave file has been opened and the file pointer has been set
	//to the data chuck's data portion.
	m_dwPos = 0;
	memcpy(&m_wfex, &(m_waveFileHeader.pcmWaveFormat), sizeof(PCMWAVEFORMAT));
	m_wfex.cbSize = 0;

	m_nTotalSample = m_dwDataSize / m_wfex.nBlockAlign;
	return true;
}

bool AMWaveFile::Close()
{
	m_pAFile = NULL; // For this file is opened outside this class, so just set it to NULL here;
	return true;
}

bool AMWaveFile::Seek(int nSample)
{
	m_dwPos = (int)((float)nSample / m_wfex.nSamplesPerSec * m_wfex.nAvgBytesPerSec);
	return m_pAFile->Seek(m_dwPos + m_dwDataBegin, AFILE_SEEK_SET);
}

bool AMWaveFile::ReadData(LPBYTE pBuffer, DWORD dwSize, DWORD *pdwReadSize, bool * pbEnd)
{
	*pbEnd = false;

	DWORD	dwReadSize;
	DWORD	dwReadLength;

	if( dwSize > m_dwDataSize - m_dwPos )
	{
		dwReadSize = m_dwDataSize - m_dwPos;
		*pbEnd = true;
	}
	else
		dwReadSize = dwSize;

	if( dwReadSize )
	{
		m_pAFile->Read(pBuffer, dwReadSize, &dwReadLength);
		if( dwReadSize != dwReadLength )
		{
			//Wave file is corrupt;
			*pbEnd = true;
			dwReadSize = 0;
		}
	}

	*pdwReadSize = dwReadSize;
	m_dwPos += dwReadSize;
	return true;
}