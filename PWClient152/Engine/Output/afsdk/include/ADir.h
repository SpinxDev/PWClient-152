/*
 * FILE: ADir.h
 *
 * DESCRIPTION: The class which operating the directorys in Angelica Engine
 *
 * CREATED BY: cuiming, 2003/10/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ADIRECTORY_H_
#define _ADIRECTORY_H_

struct AFileStat
{
	char m_szName[MAX_PATH+16];
	int  m_nLength;
	bool m_bIsDir;
};

class ADir
{
private:
	HANDLE  m_hDirHandle;

protected:
	// An fullpath file name;
	char	m_szFileName[MAX_PATH];
	bool	m_bHasOpened;

public:
	 
public:
	ADir();
	virtual ~ADir();

	virtual bool Open(const char* szFullPath);
	virtual bool Close();
	virtual bool Read(AFileStat & fileData);
};
#endif