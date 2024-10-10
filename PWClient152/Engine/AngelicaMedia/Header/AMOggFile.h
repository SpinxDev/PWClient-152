/*
 * FILE: AMOggFile.h
 *
 * DESCRIPTION: class for operate ogg file
 *
 * CREATED BY: Hedi, 2008/1/9
 *
 * HISTORY:
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMOGGFILE_H_
#define _AMOGGFILE_H_

#include "AMPlatform.h"
#include "AMTypes.h"

struct OggVorbis_File;
class AFile;

class AMOggFile
{
private:
	WAVEFORMATEX	m_wfex;
	int				m_nTotalSample;
	DWORD			m_dwDataSize;
	char			m_szFilename[MAX_PATH];

	AFile *			m_pFile;
	OggVorbis_File* m_pOggFile;
	BYTE*			m_pDataBuf;

	bool AnalyseWaveformat();

protected:

	bool Open_Internal();

public:
	AMOggFile();
	virtual ~AMOggFile();
	
	bool Seek(int nSample);
	bool Open(const char* szFilename);
	bool Open(AFile* pFile, DWORD dwDataLen);
	bool Close();
	bool ReadData(LPBYTE pBuffer, DWORD dwSize, DWORD * pdwReadSize, bool * pbEnd);

	inline WAVEFORMATEX GetWaveformatEx() { return m_wfex; }
	inline DWORD GetDataSize() { return m_dwDataSize; }
	inline int GetTotalSample() { return m_nTotalSample; }
};

#endif//_AMOGGFILE_H_