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
		//无解的情况
		m_bSolvable=false;
		m_pData=NULL;
		return;
	}

	m_pData=new int[n];			//分配空间

	for(int i=0;i<n;i++)		//初始化，并得到了第一个组合 0,1,2,...,n-1
		m_pData[i]=i;			

	m_bOver=false;
}

//////////////////////////////////////////////////////////////////////
// 本类的核心方法
// 将当前算出的组合传给pData，同时计算下一个组合
// 要求pData在调用者那里已经被初始化为长度为n的一个数组
//////////////////////////////////////////////////////////////////////
void CCombGenerator::GetNextComb(int *pData)
{
	//传出当前的组合
	memcpy(pData,m_pData,n*sizeof(int));

	if(IsOver()) return;

	int CarryPos=n-1;		//进位的位置！
	//计算下一组合
	int i(0);
	for(i=n-1;i>=0;i--)
	{
		if(m_pData[i]==m-n+i)			//当前位置已经达到最大值，如果增1，需要进位！因此记录进位位置为上一位置
			CarryPos=i-1;
		else
			break;
	}

	if(CarryPos==-1)
	{
		m_bOver=true;		//所有的组合都已产生完毕！
		return;				//如果进位的位置是-1，则说明已经计算出所有的组合，并且不能再进位了，因此直接返回！
	}

	m_pData[CarryPos]++;					//进位处增1

	int nValue=m_pData[CarryPos]+1;
	for(i=CarryPos+1;i<n;i++)				//进位处后面的位置重新置新值，如进位处为k,则后面依次为k+1,k+2,...
	{
		m_pData[i]=nValue;
		nValue++;
	}

}

}	// end namespace

#endif	//NULL
