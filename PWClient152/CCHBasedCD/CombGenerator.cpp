// CombGenerator.cpp: implementation of the CCombGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "CombGenerator.h"
#include <memory.h>

#ifndef NULL
#define NULL 0


namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCombGenerator::CCombGenerator(int nn,int mm)
{
	Set(nn,mm);
}

CCombGenerator::~CCombGenerator()
{
	if(m_pData) delete [] m_pData;
}

void CCombGenerator::Set(int nn, int mm)
{
	n=nn;
	m=mm;

	if(n<=0 || m<=0 || n>m )
	{
		//�޽�����
		m_bSolvable=false;
		m_pData=NULL;
		return;
	}

	m_pData=new int[n];			//����ռ�

	for(int i=0;i<n;i++)		//��ʼ�������õ��˵�һ����� 0,1,2,...,n-1
		m_pData[i]=i;			

	m_bOver=false;
}

//////////////////////////////////////////////////////////////////////
// ����ĺ��ķ���
// ����ǰ�������ϴ���pData��ͬʱ������һ�����
// Ҫ��pData�ڵ����������Ѿ�����ʼ��Ϊ����Ϊn��һ������
//////////////////////////////////////////////////////////////////////
void CCombGenerator::GetNextComb(int *pData)
{
	//������ǰ�����
	memcpy(pData,m_pData,n*sizeof(int));

	if(IsOver()) return;

	int CarryPos=n-1;		//��λ��λ�ã�
	//������һ���
	int i(0);
	for(i=n-1;i>=0;i--)
	{
		if(m_pData[i]==m-n+i)			//��ǰλ���Ѿ��ﵽ���ֵ�������1����Ҫ��λ����˼�¼��λλ��Ϊ��һλ��
			CarryPos=i-1;
		else
			break;
	}

	if(CarryPos==-1)
	{
		m_bOver=true;		//���е���϶��Ѳ�����ϣ�
		return;				//�����λ��λ����-1����˵���Ѿ���������е���ϣ����Ҳ����ٽ�λ�ˣ����ֱ�ӷ��أ�
	}

	m_pData[CarryPos]++;					//��λ����1

	int nValue=m_pData[CarryPos]+1;
	for(i=CarryPos+1;i<n;i++)				//��λ�������λ����������ֵ�����λ��Ϊk,���������Ϊk+1,k+2,...
	{
		m_pData[i]=nValue;
		nValue++;
	}

}

}	// end namespace

#endif	//NULL
