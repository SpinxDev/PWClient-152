/********************************************************************
	created:	2008/06/10
	author:		kuiwu
	
	purpose:	height map for path finding
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _HEIGHT_IMAGE_H_
#define _HEIGHT_IMAGE_H_

#include <AFileImage.h>
#include <assert.h>

namespace AutoMove
{

// version
#define HEIGHT_IMAGE_FILE_VER 0x00000001	

// magic 
#define  HEIGHT_IMAGE_FILE_MAGIC   (DWORD)(('p'<<24)| ('h'<<16)|('m'<<8)|('f'))

template<class T>
class CHeightImage
{
public:
	CHeightImage()
		:m_pData(NULL)
	{

	}
	~CHeightImage()
	{
		Release();
	}
	
	void Init(int iW, int iL)
	{
		Release();
		m_iWidth = iW;
		m_iLength = iL;
		m_pData = new T[iW * iL];
	}
	T GetHeight(int w, int l) const
	{
		if (w < 0 || w >= m_iWidth || l < 0 || l >= m_iLength)
		{
			return T(0);
		}

		return m_pData[l*m_iWidth + w];
	}

	void SetHeight(int w, int l, T h)
	{
		if (w < 0 || w >= m_iWidth || l < 0 || l >= m_iLength)
		{
			return;
		}

		m_pData[l * m_iWidth + w] = h;
	}

	void Release()
	{
		if (m_pData)
		{
			delete[] m_pData;
			m_pData = NULL;
		}
	}
	
	bool  Save(const char * szPath);
	bool  Load(const char * szPath);
private:
	T * m_pData;
	int     m_iWidth;   
	int     m_iLength;
};

template<class T>
bool  CHeightImage<T>::Save(const char * szPath)
{
	AFile  aFile;
	if (!aFile.Open(szPath, AFILE_BINARY| AFILE_CREATENEW| AFILE_NOHEAD))
	{
		return false;
	}
	
	DWORD dwLen;
	DWORD flag;
	flag = HEIGHT_IMAGE_FILE_MAGIC;
	aFile.Write(&flag, sizeof(DWORD), &dwLen);
	flag = HEIGHT_IMAGE_FILE_VER;
	aFile.Write(&flag, sizeof(DWORD), &dwLen);
	
	aFile.Write(&m_iWidth, sizeof(int), &dwLen);
	aFile.Write(&m_iLength, sizeof(int), &dwLen);

	aFile.Write(m_pData, sizeof(T)*m_iWidth*m_iLength, &dwLen);

	aFile.Close();

	return true;
}
template<class T>
bool CHeightImage<T>::Load(const char * szPath)
{
	Release();

	AFileImage   fileimg;
	if (!fileimg.Open(szPath, AFILE_BINARY|AFILE_OPENEXIST))
	{
		return false;
	}
	DWORD dwLen;
	DWORD flag;
	
	fileimg.Read(&flag, sizeof(DWORD), &dwLen);
	if (flag != HEIGHT_IMAGE_FILE_MAGIC)
	{
		return false;
	}

	fileimg.Read(&flag, sizeof(DWORD), &dwLen);
	if (flag != HEIGHT_IMAGE_FILE_VER)
	{
		return false;
	}

	fileimg.Read(&m_iWidth, sizeof(int), &dwLen);
	fileimg.Read(&m_iLength, sizeof(int), &dwLen);

	m_pData = new T[m_iWidth * m_iLength];
	assert(m_pData);

	fileimg.Read(m_pData, sizeof(T) * m_iWidth * m_iLength, &dwLen);

	fileimg.Close();

	return true;
}



};


#endif