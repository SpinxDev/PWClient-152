  /*
 * FILE: ADirImage.h
 *
 * DESCRIPTION: The class which operating the directory images in memory for Angelica Engine
 *
 * CREATED BY: Cuiming, 2003/10/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ADIRIMAGE_H_
#define _ADIRIMAGE_H_

#include "AFPlatform.h"
#include "AFilePackage.h"
#include "ADir.h"

class ADirImage  : public ADir
{
private:
	AFilePackage * m_pck;
	AFilePackage::directory * m_dir;
	int m_dirIdx;
	int m_flag;
public:

	ADirImage();
	virtual ~ADirImage();
	virtual bool Open(const char* szFullPath);
	virtual bool Close();
	virtual bool Read(AFileStat & fileData);
};


#endif