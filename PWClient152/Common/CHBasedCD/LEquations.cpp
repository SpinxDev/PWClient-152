//////////////////////////////////////////////////////////////////////
// LEquations.cpp
//
// ������Է�������� CLEquations ��ʵ�ִ���
//
// from source code of <��ѧ�빤����ֵ�㷨 by Zhou changfa>
//////////////////////////////////////////////////////////////////////

#include "LEquations.h"
#include <assert.h>


namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// �������캯��
//////////////////////////////////////////////////////////////////////
CLEquations::CLEquations()
{
}

//////////////////////////////////////////////////////////////////////
// ָ��ϵ���ͳ������캯��
//
// ������
// 1. const CMatrix& mtxCoef - ָ����ϵ������
// 2. const CMatrix& mtxConst - ָ���ĳ�������
//////////////////////////////////////////////////////////////////////
CLEquations::CLEquations(const CMatrix& mtxCoef, const CMatrix& mtxConst)
{
	bool res=Init(mtxCoef, mtxConst);
	assert(res);
}

//////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////
CLEquations::~CLEquations()
{
}

//////////////////////////////////////////////////////////////////////
// ��ʼ������
//
// ������
// 1. const CMatrix& mtxCoef - ָ����ϵ������
// 2. const CMatrix& mtxConst - ָ���ĳ�������
//
// ����ֵ��bool �ͣ���ʼ���Ƿ�ɹ�
//////////////////////////////////////////////////////////////////////
bool CLEquations::Init(const CMatrix& mtxCoef, const CMatrix& mtxConst)
{
	if (mtxCoef.GetNumRows() != mtxConst.GetNumRows())
		return false;

	m_mtxCoef = mtxCoef;
	m_mtxConst = mtxConst;

	return true;
}

//////////////////////////////////////////////////////////////////////
// ��ȡϵ������
//
// ��������
//
// ����ֵ��CMatrix �ͣ�����ϵ������
//////////////////////////////////////////////////////////////////////
inline CMatrix CLEquations::GetCoefMatrix() const
{
	return m_mtxCoef;
}

//////////////////////////////////////////////////////////////////////
// ��ȡ��������
//
// ��������
//
// ����ֵ��CMatrix �ͣ����س�������
//////////////////////////////////////////////////////////////////////
inline CMatrix CLEquations::GetConstMatrix() const
{
	return m_mtxConst;
}

//////////////////////////////////////////////////////////////////////
// ��ȡ���̸���
//
// ��������
//
// ����ֵ��int �ͣ����ط����鷽�̵ĸ���
//////////////////////////////////////////////////////////////////////
inline int	CLEquations::GetNumEquations() const
{
	return GetCoefMatrix().GetNumRows();
}

//////////////////////////////////////////////////////////////////////
// ��ȡδ֪������
//
// ��������
//
// ����ֵ��int �ͣ����ط�����δ֪���ĸ���
//////////////////////////////////////////////////////////////////////
inline int	CLEquations::GetNumUnknowns() const
{
	return GetCoefMatrix().GetNumColumns();
}

//////////////////////////////////////////////////////////////////////
// ȫѡ��Ԫ��˹��ȥ��
//
// ������
// 1. CMatrix& mtxResult - CMatrix���ö��󣬷��ط�����Ľ�
//
// ����ֵ��bool �ͣ�����������Ƿ�ɹ�
//////////////////////////////////////////////////////////////////////
bool CLEquations::GetRootsetGauss(CMatrix& mtxResult)
{ 
	int *pnJs,l,k,i,j,nIs,p,q;
    double d,t;

	// ����������ԣ����������󸳸������
	mtxResult = m_mtxConst;
	double *pDataCoef = m_mtxCoef.GetData();
	double *pDataConst = mtxResult.GetData();
	int n = GetNumUnknowns();

	// ��ʱ���������������
    pnJs = new int[n];

	// ��Ԫ
    l=1;
    for (k=0;k<=n-2;k++)
    { 
		d=0.0;
        for (i=k;i<=n-1;i++)
		{
			for (j=k;j<=n-1;j++)
            { 
				t=fabs(pDataCoef[i*n+j]);
				if (t>d) 
				{ 
					d=t; 
					pnJs[k]=j; 
					nIs=i;
				}
            }
		}

        if (d == 0.0) 
			l=0;
        else
        { 
			if (pnJs[k]!=k)
            {
				for (i=0;i<=n-1;i++)
                { 
					p=i*n+k; 
					q=i*n+pnJs[k];
					t=pDataCoef[p]; 
					pDataCoef[p]=pDataCoef[q]; 
					pDataCoef[q]=t;
                }
			}

            if (nIs!=k)
            { 
				for (j=k;j<=n-1;j++)
                { 
					p=k*n+j; 
					q=nIs*n+j;
                    t=pDataCoef[p]; 
					pDataCoef[p]=pDataCoef[q]; 
					pDataCoef[q]=t;
                }
                
				t=pDataConst[k]; 
				pDataConst[k]=pDataConst[nIs]; 
				pDataConst[nIs]=t;
            }
        }
        
		// ���ʧ��
		if (l==0)
        { 
			delete[] pnJs;
            return false;
        }
        
		d=pDataCoef[k*n+k];
        for (j=k+1;j<=n-1;j++)
        { 
			p=k*n+j; 
			pDataCoef[p]=pDataCoef[p]/d;
		}
        
		pDataConst[k]=pDataConst[k]/d;
        for (i=k+1;i<=n-1;i++)
        { 
			for (j=k+1;j<=n-1;j++)
            { 
				p=i*n+j;
                pDataCoef[p]=pDataCoef[p]-pDataCoef[i*n+k]*pDataCoef[k*n+j];
            }
            
			pDataConst[i]=pDataConst[i]-pDataCoef[i*n+k]*pDataConst[k];
        }
    }
    
	// ���ʧ��
	d=pDataCoef[(n-1)*n+n-1];
    if (d == 0.0)
	{ 
		delete[] pnJs;
		return false;
	}

	// ���
    pDataConst[n-1]=pDataConst[n-1]/d;
    for (i=n-2;i>=0;i--)
    { 
		t=0.0;
        for (j=i+1;j<=n-1;j++)
			t=t+pDataCoef[i*n+j]*pDataConst[j];
        pDataConst[i]=pDataConst[i]-t;
    }
    
	// �������λ��
	pnJs[n-1]=n-1;
    for (k=n-1;k>=0;k--)
    {
		if (pnJs[k]!=k)
        { 
			t=pDataConst[k]; 
			pDataConst[k]=pDataConst[pnJs[k]]; 
			pDataConst[pnJs[k]]=t;
		}
	}

	// �����ڴ�
    delete[] pnJs;

    return true;
}

}	// end namespace