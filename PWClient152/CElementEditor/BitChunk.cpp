// BitChunk.cpp: implementation of the CBitChunk class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "BitChunk.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitChunk::CBitChunk()
{
	m_pData = 0;
}

CBitChunk::~CBitChunk()
{
	if(m_pData) delete []m_pData;
}

//�������2ά����ĳߴ磬ע�⣺���óߴ�ʱҪ�ƻ�ԭ������
//���ݶ�����ʼ��Ϊ0
void CBitChunk::SetSize(int width,int height)
{
	if(width<=0 || height<=0)
	{
		AfxMessageBox("CBitChunk::SetSize(),�ڴ����û�гɹ����ڴ��Ŀ�͸߱������0");
		m_nWidth = 0;
		m_nHeight = 0;
		return;
	}
	m_nWidth = width;
	m_nHeight = height;
	
	//����÷�����ֽ���
	int temp = width*height;
	int size = temp/8;
	if(temp%8!=0)
		size = size + 1;

	if(m_pData) delete []m_pData;
	m_pData = NULL;

	m_pData = new BYTE[size];
	if(m_pData==NULL)
	{
		m_nByteNum = 0;
		AfxMessageBox("CBitChunk::SetSize(),�ڴ����û�гɹ����������ڴ��Ѿ��ľ�����ر�һЩ����Ӧ�ó���");
		return;
	}
	//ȫ����ʼ��Ϊ0
	memset(m_pData,0,size);
	m_nByteNum = size;
}
	
//���ö�ά�����ֵ
void CBitChunk::SetValue( int w, int h, int value)
{
	//�ж�ȡֵ��Χ
	if(w>=m_nWidth || w<0 || h>=m_nHeight || h<0)
	{
		AfxMessageBox("CBitChunk::SetVaule(),����ȡֵ��Χ");
		ASSERT(false);
	}
	//�����ҳ���ŵ��ֽ�
	int pos = (m_nWidth*h + w)/8;
	int shift = (m_nWidth*h + w)%8;
	BYTE data = m_pData[pos];
	
	switch(shift)
	{
	case 0:
		if(value==0)
			data = data & 0x7F;
		else data = data | 0x80;
		break;
	case 1:
		if(value==0)
			data = data & 0xBF;
		else data = data | 0x40;
		break;
	case 2:
		if(value==0)
			data = data & 0xDF;
		else data = data | 0x20;
		break;
	case 3: 
		if(value==0)
			data = data & 0xEF;
		else data = data | 0x10;
		break;
	case 4: 
		if(value==0)
			data = data & 0xF7;
		else data = data | 0x8;
		break;
	case 5:
		if(value==0)
			data = data & 0xFB;
		else data = data | 0x4;
		break;
	case 6:
		if(value==0)
			data = data & 0xFD;
		else data = data | 0x2;
		break;
	case 7:
		if(value==0)
			data = data & 0xFE;
		else data = data | 0x1;
		break;
	}
	
	m_pData[pos] = data;
}
	
//ȡ�ض�ά�����ֵ
int CBitChunk::GetVaule( int w, int h)
{
	//�ж�ȡֵ��Χ
	if(w>=m_nWidth || w<0 || h>=m_nHeight || h<0)
	{
		AfxMessageBox("CBitChunk::GetVaule(),����ȡֵ��Χ");
		ASSERT(false);
	}
	//�����ҳ���ŵ��ֽ�
	int pos = (m_nWidth*h + w)/8;
	int shift = (m_nWidth*h + w)%8;
	BYTE data = m_pData[pos];
	data = data>>(7-shift);
	data = data & 0x1;
	return (int)data;
}

void CBitChunk::Release()
{
	if(m_pData) delete []m_pData;
	m_pData = NULL;
}
