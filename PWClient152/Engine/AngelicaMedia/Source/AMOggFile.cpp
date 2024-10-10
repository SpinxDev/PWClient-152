/*
 * FILE: AMOggFile.cpp
 *
 * DESCRIPTION: class for operate mp3 file
 *
 * CREATED BY: Hedi, 2002/1/29
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMOggFile.h"
#include "AMPI.h"
#include "vorbis/vorbisfile.h"
#include "AFile.h"
#include "AFileImage.h"
#include "AFileUnicode.h"
#include "AMemFile.h"
#include <io.h>

AMOggFile::AMOggFile()
{
	m_szFilename[0] = '\0';
	m_pOggFile = NULL;
	m_pFile = NULL;
	m_pDataBuf = NULL;
}

AMOggFile::~AMOggFile()
{
}

size_t oggfile_fread(void *buffer, size_t size, size_t num, void *stream)
{
	AFile * pFile = (AFile *) stream;
	if( !pFile )
		return 0;

	DWORD dwReadLen = 0;
	pFile->Read(buffer, size * num, &dwReadLen);

	return dwReadLen;
}

int oggfile_fseek(void * stream, ogg_int64_t offset, int origin)
{
	AFile * pFile = (AFile *) stream;
	if( !pFile )
		return -1;

	AFILE_SEEK afile_origin = AFILE_SEEK_SET;
	switch(origin)
	{
	case SEEK_SET:
		afile_origin = AFILE_SEEK_SET;
		break;
	case SEEK_CUR:
		afile_origin = AFILE_SEEK_CUR;
		break;
	case SEEK_END:
		afile_origin = AFILE_SEEK_END;
		break;
	default:
		return -1;
	}

	if( pFile->Seek((long)offset, afile_origin) )
		return 0;
	else
		return -1;
}

int oggfile_fclose(void * stream)
{
	AFile * pFile = (AFile *) stream;
	if( !pFile )
		return -1;

	pFile->Close();
	return 0;
}

long oggfile_ftell(void * stream)
{
	AFile * pFile = (AFile *) stream;
	if( !pFile )
		return -1;

	return (long)pFile->GetPos();
}
 
bool AMOggFile::Open(const char* szFilename)
{
	strncpy(m_szFilename, szFilename, MAX_PATH);

	if( 0 == _access(szFilename, 0) )
		m_pFile = new AFile();
	else if( AFileUnicode::IsFileUnicodeExist(szFilename) )
		m_pFile = new AFileUnicode();
	else
		m_pFile = new AFileImage();

	if( !m_pFile->Open(szFilename, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		delete m_pFile;
		m_pFile = NULL;
		AMERRLOG(("AMOggFile::Open(), can not locate file [%s] !", szFilename));
		return false;
	}

	if (!Open_Internal())
	{
		Close();
		return false;
	}

	return true;
}

bool AMOggFile::Open(AFile* pFile, DWORD dwDataLen)
{
	if (!pFile || !dwDataLen)
		return false;

	strncpy(m_szFilename, pFile->GetFileName(), MAX_PATH);

	//	Create data buffer
	if (!(m_pDataBuf = (BYTE*)a_malloc(dwDataLen)))
	{
		AMERRLOG(("AMOggFile::Open_Internal(), not enough memory !"));
		return false;
	}

	//	Load data from file
	DWORD dwRead;
	if (!pFile->Read(m_pDataBuf, dwDataLen, &dwRead))
	{
		AMERRLOG(("AMOggFile::Open_Internal(), failed to load data from file [%s] !", m_szFilename));
		return false;
	}

	AMemFile* pMemFile = new AMemFile;

	if (!pMemFile->Attach(m_pDataBuf, dwDataLen, 0))
		return false;
	
	m_pFile = pMemFile;

	if (!Open_Internal())
	{
		Close();
		return false;
	}

	return true;
}

bool AMOggFile::Open_Internal()
{
	ov_callbacks callbacks = 
	{
		oggfile_fread,
		oggfile_fseek,
		oggfile_fclose,
		oggfile_ftell
	};

	m_pOggFile = new OggVorbis_File();
	if( ::ov_open_callbacks((void *)m_pFile, m_pOggFile, NULL, 0, callbacks) != 0 )
	{
		delete m_pOggFile;
		m_pOggFile = NULL;
		AMERRLOG(("AMOggFile::Open_Internal(), open mp3 file [%s] fail!", m_szFilename));
		return false;
	}

	if( m_pOggFile->links > 1 )
		return false;

	if( !AnalyseWaveformat() )
		return false;

	m_nTotalSample = (int)(::ov_pcm_total(m_pOggFile, 0));
	m_dwDataSize = m_nTotalSample * m_wfex.nBlockAlign;
	
	return true;
}

bool AMOggFile::Close()
{
	if( m_pOggFile )
	{
		if( ::ov_clear(m_pOggFile) != 0 )
		{
			AMERRLOG(("AMOggFile::Close(), fail!"));
			return false;
		}

		delete m_pOggFile;
		m_pOggFile = NULL;
	}

	if (m_pDataBuf && m_pFile)
	{
		((AMemFile*)m_pFile)->Detach();
		a_free(m_pDataBuf);
		m_pDataBuf = NULL;
	}

	if( m_pFile )
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}

	return true;
}

bool AMOggFile::Seek(int nSample)
{
	if( !m_pOggFile )
		return false;

	if( ::ov_pcm_seek(m_pOggFile, nSample) != 0 )
	{
		AMERRLOG(("AMOggFile::Seek(), fail!"));
		return false;
	}
	return true;
}

bool AMOggFile::ReadData(LPBYTE pBuffer, DWORD dwSize, DWORD *pdwReadSize, bool * pbEnd)
{
	*pbEnd = false;

	int nReadSize = 0;

	bool bEnd = false;
	int nRet = 0;

READAGAIN:
	nRet = ::ov_read(m_pOggFile, (char *)pBuffer + nReadSize, dwSize - nReadSize, 0, 2, 1, NULL);
	if( nRet < 0 )
	{
		AMERRLOG(("AMOggFile::ReadData(), Fail!"));
		return false;
	}

	bEnd = nRet ? false : true;
	nReadSize += nRet;
	if( !bEnd && nReadSize < (int)dwSize )
		goto READAGAIN;

	*pbEnd = bEnd;
	*pdwReadSize = nReadSize;
	return true;
}

bool AMOggFile::AnalyseWaveformat()
{
	if( !m_pOggFile )
		return false;

	ZeroMemory(&m_wfex, sizeof(WAVEFORMATEX));
	m_wfex.wFormatTag = WAVE_FORMAT_PCM;
	m_wfex.nSamplesPerSec = ov_info(m_pOggFile, 0)->rate;
	m_wfex.nChannels = ov_info(m_pOggFile, 0)->channels;
	m_wfex.wBitsPerSample = 16;

	m_wfex.nBlockAlign = m_wfex.nChannels * m_wfex.wBitsPerSample / 8;
	m_wfex.nAvgBytesPerSec = m_wfex.nSamplesPerSec * m_wfex.nBlockAlign;
	return true;
}