#include "StdAfx.h"
#include "A3DGFXGridAnimation.h"
#include "A3DGridDecalEx.h"
#include "base64.h"

static const char * _format_key_num		= "keyNumber: %d";
static const char* _format_gridanimation_lines		= "GridAnimationLines: %d";

int A3DGfxGridAnimation::nVertsLimit = 10 * 10;

A3DGfxGridAnimation::A3DGfxGridAnimation()
{

}

A3DGfxGridAnimation::~A3DGfxGridAnimation()
{
	Clear();
}

void A3DGfxGridAnimation::Clear()
{
	for (int i=0; i<(int)m_Keys.size(); ++i)
	{
		delete[] m_Keys[i].gridVertsData;
	}
	m_Keys.clear();
}

bool A3DGfxGridAnimation::AddKey( int time, A3DGridDecalEx::GRID_VERTEX_DATA*		gridVertsData, int size )
{
	if (size > nVertsLimit)
		return false;

	GRID_ANIM_KEY keypoint;
	keypoint.gridVertsData  = new A3DGridDecalEx::GRID_VERTEX_DATA[nVertsLimit];
	memcpy(keypoint.gridVertsData, gridVertsData, sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size);
	keypoint.time = time;

	m_Keys.push_back(keypoint);

	return true;
}

int A3DGfxGridAnimation::GetValue( int time, A3DGridDecalEx::GRID_VERTEX_DATA*		gridVertsData, int size)
{
	if (m_Keys.empty() || size > nVertsLimit)
		return -1;

	int numKeys = (int)m_Keys.size();

	int pos = -1;			//找到的key位置
	if (numKeys > 1)
	{
		if (time < m_Keys[0].time)			//小于最小帧
		{
			memcpy(gridVertsData, m_Keys[0].gridVertsData, sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size);
			return 0;
		}

		if (time >= m_Keys[numKeys-1].time)				//大于最大帧
		{
			memcpy(gridVertsData, m_Keys[numKeys-1].gridVertsData, sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size);

			return numKeys-1;
		}

		for (int i=1; i<numKeys; ++i)
		{
			if (time < m_Keys[i].time)
			{
				pos = i-1;					//找到位置
				break;
			}
		}

		if (pos != -1)
		{
			int t1 = m_Keys[pos].time;
			int t2 = m_Keys[pos+1].time;

			float r = (time-t1) /(float)(t2-t1);

			for (int i=0; i<size; ++i)
			{
				gridVertsData[i].m_vPos = m_Keys[pos].gridVertsData[i].m_vPos * (1.0f - r) + m_Keys[pos+1].gridVertsData[i].m_vPos * r;
				gridVertsData[i].m_dwColor = Interp_Color(m_Keys[pos].gridVertsData[i].m_dwColor, m_Keys[pos+1].gridVertsData[i].m_dwColor, r);
			}
		}
	}
	else if (numKeys == 1)
	{
		memcpy(gridVertsData, m_Keys[0].gridVertsData, sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size);
		pos = 0;
	}

	return pos;
}

bool A3DGfxGridAnimation::GetKeyData( int idx, A3DGridDecalEx::GRID_VERTEX_DATA* gridVertsData, int size )
{
	if (idx >= (int)m_Keys.size() || size > nVertsLimit)
		return false;

	memcpy(gridVertsData, m_Keys[idx].gridVertsData, sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size);
	return true;
}

const GRID_ANIM_KEY* A3DGfxGridAnimation::GetKey( int idx ) const
{
	if (idx < 0 || idx >= (int)m_Keys.size())
		return NULL;
	return &m_Keys[idx];
}

bool A3DGfxGridAnimation::Load( AFile* pFile, DWORD dwVersion, int size )
{
	DWORD	dwReadLen;
	char	szLine[AFILE_LINEMAXLEN];

	int numKeys = 0;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_key_num, &numKeys);

	if (numKeys > 0)
	{
		ASSERT(numKeys <= 5);
		ASSERT(size <= nVertsLimit);

		numKeys = min(numKeys, 5);
		size = min(size, nVertsLimit);

		Clear();

		for (int i=0; i<numKeys; ++i)
		{
			m_Keys.push_back(GRID_ANIM_KEY());

			if(m_Keys[i].gridVertsData == NULL)
				m_Keys[i].gridVertsData = new A3DGridDecalEx::GRID_VERTEX_DATA[nVertsLimit];

			m_Keys[i].Load(pFile, size);
		}
	}
	return true;
}

bool A3DGfxGridAnimation::Save( AFile* pFile, int size )
{
	int numKeys = (int)m_Keys.size();

	char	szLine[AFILE_LINEMAXLEN];
	sprintf(szLine, _format_key_num, numKeys);
	pFile->WriteLine(szLine);

	if (numKeys > 0)
	{
		ASSERT(numKeys <= 5);	
		ASSERT(size <= nVertsLimit);

		numKeys = min(numKeys, 5);
		size = min(size, nVertsLimit);

		for (int i=0; i<numKeys; ++i)
		{
			m_Keys[i].Save(pFile, size);
		}

	}
	return true;
}

A3DGfxGridAnimation& A3DGfxGridAnimation::operator = (const A3DGfxGridAnimation& src)
{
	if(this != &src)
	{
		int numKeys = src.GetNumKeys();

		Clear();

		for (int i=0; i<numKeys; ++i)
		{
			m_Keys.push_back(GRID_ANIM_KEY());

			if(m_Keys[i].gridVertsData == NULL)
				m_Keys[i].gridVertsData = new A3DGridDecalEx::GRID_VERTEX_DATA[nVertsLimit];

			m_Keys[i].time = src.GetKey(i)->time;
			memcpy(m_Keys[i].gridVertsData, src.GetKey(i)->gridVertsData, sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*nVertsLimit);
		}
	}
	return *this;
}


void GRID_ANIM_KEY::Load( AFile* pFile, int size )
{
	DWORD	dwReadLen;
	char	szLine[AFILE_LINEMAXLEN];

	ASSERT(gridVertsData);

	int len = 0;
	int lines = 0;
	AString str;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_gridanimation_lines, &lines);

	if (lines == 0)
		return;

	while(lines)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		str += szLine;
		--lines; 
	}

	int buf_len = str.GetLength() + 32;
	char* pBuf = new char[buf_len];
	len = base64_decode((char*)(const char*)str, str.GetLength(), (unsigned char*)pBuf);
	pBuf[len] = 0;

	//读取数据
	int bufferSize = sizeof(int) + sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size;
	char* t = pBuf;

	int copySize = sizeof(int);
	memcpy(&time, t, copySize);
	t += copySize;

	ASSERT(gridVertsData);

	for (int i=0; i<size; ++i)
	{
		copySize = sizeof(A3DGridDecalEx::GRID_VERTEX_DATA);
		memcpy(&gridVertsData[i], t, copySize);
		t += copySize;
	}

	delete[] pBuf;

}

void GRID_ANIM_KEY::Save( AFile* pFile, int size )
{
	char	szLine[AFILE_LINEMAXLEN];

	//内存形式
	int bufferSize = sizeof(int) + sizeof(A3DGridDecalEx::GRID_VERTEX_DATA)*size;
	char* buffer = new char[bufferSize];			//数据
	char* t = buffer;
	int leftSize = bufferSize;

	int copySize = sizeof(int);
	memcpy(t, &time, copySize);
	t += copySize;
	leftSize -= copySize;

	ASSERT(gridVertsData);

	for (int i=0; i<size; ++i)
	{
		copySize = sizeof(A3DGridDecalEx::GRID_VERTEX_DATA);
		memcpy(t, &gridVertsData[i], copySize);
		t += copySize;
		leftSize -= copySize;
	}

	//base64
	int nBufLen = (bufferSize+3) / 3 * 4 + 32;
	char* pBuf = new char[nBufLen];
	int len = base64_encode((unsigned char*)(const char*)buffer, bufferSize, pBuf);
	int nLines = (len + 1499) / 1500;

	sprintf(szLine, _format_gridanimation_lines, nLines);
	pFile->WriteLine(szLine);

	const char* pWrite = pBuf;
	while (len)
	{
		int nWrite = len > 1500 ? 1500 : len;
		len -= nWrite;

		AString s(pWrite, nWrite);
		pFile->WriteLine(s);
		pWrite += nWrite;
	}

	delete[] pBuf;
	delete[] buffer;	
}