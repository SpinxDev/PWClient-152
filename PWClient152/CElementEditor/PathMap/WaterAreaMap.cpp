 /*
 * FILE: WaterAreaMap.cpp
 *
 * DESCRIPTION:  implement of set of classes to describe all the water areas in one block of the Map!
 *
 * CREATED BY: He wenfeng, 2005/5/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "WaterAreaMap.h"

CWaterAreaMap::CWaterAreaMap()
{

}

CWaterAreaMap::~CWaterAreaMap()
{
	Release();
}

bool CWaterAreaMap::Load(const char* szFile)
{
	FILE *FileToLoad = fopen(szFile, "rb");
	if(FileToLoad)
	{
		bool bLoaded = Load(FileToLoad);
		fclose(FileToLoad);
		return bLoaded;
	}
	else
		return false;
}

bool CWaterAreaMap::Load(FILE* FileToLoad)
{
	if(!FileToLoad) return false;
	
	int iBufSize = sizeof(unsigned int)			// Version
					   +2*sizeof(float)					 // Width and Length
					   +sizeof(int)						  // number of water areas
					   +m_WaterAreas.size() * sizeof(CWaterArea);		// WaterArea Information
	
	unsigned char * buf = new unsigned char [iBufSize];
	
	int iReadLen = fread(buf, 1, iBufSize, FileToLoad);
	if( iReadLen!= iBufSize )
	{
		delete [] buf;
		return false;
	}

	// Identify the version
	int iCur=0;
	unsigned int uVer=* (unsigned int *) (buf+iCur);
	iCur+=sizeof(unsigned int);	
	if(uVer == WATER_AREA_MAP_VER)
	{
		Release();

		// width and length
		m_fWidth = * (float *) (buf+iCur);
		iCur+=sizeof(float);

		m_fLength = * (float *) (buf+iCur);
		iCur+=sizeof(float);

		// water area number
		int iNum =  * (int *) (buf+iCur);
		iCur+=sizeof(int);
		
		float fcx, fcz, fHalfWidth, fHalfLength, fHeight;
		for(int i=0; i<iNum; i++)
		{
			fcx = * (float *) (buf+iCur);
			iCur+=sizeof(float);	
			fcz = * (float *) (buf+iCur);
			iCur+=sizeof(float);	

			fHalfWidth = * (float *) (buf+iCur);
			iCur+=sizeof(float);	
			fHalfLength = * (float *) (buf+iCur);
			iCur+=sizeof(float);	

			fHeight = * (float *) (buf+iCur);
			iCur+=sizeof(float);
		
			AddWaterArea(fcx, fcz, fHalfWidth,fHalfLength, fHeight);
		}
		
		delete [] buf;
		return true;	
	}
	else
	{
		delete [] buf;
		return false;
	}
}

bool CWaterAreaMap::Save(const char* szFile)
{
	FILE *FileToSave = fopen(szFile, "wb");
	if(FileToSave)
	{
		bool bSaved = Save(FileToSave);
		fclose(FileToSave);
		return bSaved;
	}
	else
		return false;
}

bool CWaterAreaMap::Save(FILE* FileToSave)
{
	if(!FileToSave) return false;

	int iBufSize = sizeof(unsigned int)			// Version
					   +2*sizeof(float)					 // Width and Length
					   +sizeof(int)						  // number of water areas
					   +m_WaterAreas.size() * sizeof(CWaterArea);		// WaterArea Information
	
	unsigned char * buf = new unsigned char [iBufSize];
	int iCur=0;

	// 4-byte version
	* (unsigned int *) (buf+iCur) = WATER_AREA_MAP_VER;
	iCur+=sizeof(unsigned int);	

	// width and length
	* (float *) (buf+iCur) = m_fWidth;
	iCur+=sizeof(float);

	* (float *) (buf+iCur) = m_fLength;
	iCur+=sizeof(float);

	// each water area information
	* (int *) (buf+iCur) = m_WaterAreas.size();
	iCur+=sizeof(int);

	for(int i=0; i<m_WaterAreas.size(); i++)
	{
		CWaterArea* pWaterArea = m_WaterAreas[i];
		
		* (float *) (buf+iCur) = pWaterArea->CenterX;
		iCur+=sizeof(float);

		* (float *) (buf+iCur) = pWaterArea->CenterZ;
		iCur+=sizeof(float);
		
		* (float *) (buf+iCur) = pWaterArea->HalfWidth;
		iCur+=sizeof(float);

		* (float *) (buf+iCur) = pWaterArea->HalfLength;
		iCur+=sizeof(float);

		* (float *) (buf+iCur) = pWaterArea->Height;
		iCur+=sizeof(float);

		
	}

	// write to the file
	int iWriteLen = fwrite(buf, 1, iBufSize, FileToSave);
	if(iWriteLen != iBufSize)
	{
		delete [] buf;
		return false;
	}

	delete [] buf;
	return true;
}
