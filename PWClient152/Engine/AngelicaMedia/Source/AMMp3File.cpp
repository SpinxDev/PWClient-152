/*
 * FILE: AMMp3File.cpp
 *
 * DESCRIPTION: class for operate mp3 file
 *
 * CREATED BY: Hedi, 2002/1/29
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMMp3File.h"
#include "AMPI.h"
#include "MP3Lib.h"

AMMp3File::AMMp3File()
{
	m_pMpg123		= new CMpg123;
	m_nTotalSample	= 0;
	m_dwDataSize	= 0;
	m_szFilename[0] = '\0';

	memset(&m_wfex, 0, sizeof (m_wfex));
}

AMMp3File::~AMMp3File()
{
	if (m_pMpg123)
	{
		delete m_pMpg123;
		m_pMpg123 = NULL;
	}
}

bool AMMp3File::Open(const char* szFilename)
{
	strncpy(m_szFilename, szFilename, MAX_PATH);

	ASSERT(m_pMpg123);

	if( !m_pMpg123->mpg123_open(m_szFilename) )
	{
		AMERRLOG(("AMMp3File::Open(), open mp3 file [%s] fail!", m_szFilename));
		return false;
	}

	if( !AnalyseWaveformat() )
		return false;

	m_nTotalSample = m_pMpg123->mpg123_get_total_sample();
	m_dwDataSize = m_nTotalSample * m_wfex.nBlockAlign;
	return true;
}

bool AMMp3File::Close()
{
	if( !m_pMpg123->mpg123_close() )
	{
		AMERRLOG(("AMMp3File::Close(), fail!"));
		return false;
	}
	return true;
}

bool AMMp3File::Seek(int nSample)
{
	if( !m_pMpg123->mpg123_seek(nSample) )
	{
		AMERRLOG(("AMMp3File::Seek(), fail!"));
		return false;
	}
	return true;
}

bool AMMp3File::ReadData(LPBYTE pBuffer, DWORD dwSize, DWORD *pdwReadSize, bool * pbEnd)
{
	*pbEnd = false;

	int nReachEnd = 0;
	if( !m_pMpg123->mpg123_read((char *)pBuffer, dwSize, (long *)pdwReadSize, &nReachEnd) )
	{
		AMERRLOG(("AMMp3File::ReadData(), Fail!"));
		return false;
	}

	*pbEnd = nReachEnd ? 1 : 0;
	return true;
}

bool AMMp3File::AnalyseWaveformat()
{
	long frequency;
	int stereo;

	if( !m_pMpg123->mpg123_getinfo(&frequency, &stereo) )
		return false;

	ZeroMemory(&m_wfex, sizeof(WAVEFORMATEX));
	m_wfex.wFormatTag = WAVE_FORMAT_PCM;
	m_wfex.nSamplesPerSec = frequency;
	m_wfex.nChannels = stereo ? 2 : 1;
	m_wfex.wBitsPerSample = 16;

	m_wfex.nBlockAlign = m_wfex.nChannels * m_wfex.wBitsPerSample / 8;
	m_wfex.nAvgBytesPerSec = m_wfex.nSamplesPerSec * m_wfex.nBlockAlign;
	return true;
}