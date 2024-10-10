/*
 * FILE: AMWaveFile.h
 *
 * DESCRIPTION: class for operate wave file
 *
 * CREATED BY: Hedi, 2002/1/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMWAVEFILE_H_
#define _AMWAVEFILE_H_

#include "AMPlatform.h"
#include "AMTypes.h"

class AFile;

class AMWaveFile
{
private:

	struct WAVEFILEHEADER
	{
		DWORD			dwFileType;
		DWORD			dwFileSize;
		DWORD			dwFormType;
		DWORD			dwFmt;
		DWORD			dwFmtSize;
		PCMWAVEFORMAT	pcmWaveFormat;
	};

	int				m_nTotalSample;
	DWORD			m_dwDataBegin;
	DWORD			m_dwData;
	DWORD			m_dwDataSize;
	DWORD			m_dwPos;
	WAVEFORMATEX	m_wfex;
	WAVEFILEHEADER  m_waveFileHeader;
	char			m_szFilename[MAX_PATH];

	AFile *			m_pAFile;
protected:
public:
	AMWaveFile();
	~AMWaveFile();

	bool Open(AFile * pAFile);
	bool Close();
	
	bool Seek(int nSample);
	bool ReadData(LPBYTE pBuffer, DWORD dwSize, DWORD * pdwReadSize, bool * pbEnd);

	inline WAVEFORMATEX GetWaveformatEx() { return m_wfex; }
	inline DWORD GetDataSize() { return m_dwDataSize; }
	inline int GetTotalSample() { return m_nTotalSample; }
};

typedef class AMWaveFile * PAMWaveFile;
#endif//_AMWAVEFILE_H_