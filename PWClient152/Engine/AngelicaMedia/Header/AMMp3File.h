/*
 * FILE: AMMp3File.h
 *
 * DESCRIPTION: class for operate mp3 file
 *
 * CREATED BY: Hedi, 2002/1/29
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMMP3FILE_H_
#define _AMMP3FILE_H_

#include "AMPlatform.h"
#include "AMTypes.h"


class CMpg123;

class AMMp3File
{
private:

	WAVEFORMATEX	m_wfex;
	int				m_nTotalSample;
	DWORD			m_dwDataSize;
	char			m_szFilename[MAX_PATH];
	CMpg123*		m_pMpg123;

	bool AnalyseWaveformat();

protected:

public:
	AMMp3File();
	virtual ~AMMp3File();

	bool Seek(int nSample);
	bool Open(const char* szFilename);
	bool Close();
	bool ReadData(LPBYTE pBuffer, DWORD dwSize, DWORD * pdwReadSize, bool * pbEnd);

	inline WAVEFORMATEX GetWaveformatEx() { return m_wfex; }
	inline DWORD GetDataSize() { return m_dwDataSize; }
	inline int GetTotalSample() { return m_nTotalSample; }
};
#endif//_AMMP3FILE_H_