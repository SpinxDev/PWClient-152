//////////////////////////////////////////////////////////////////////
// LEquations.cpp
//
// 求解线性方程组的类 CLEquations 的实现代码
//
// from source code of <科学与工程数值算法 by Zhou changfa>
//////////////////////////////////////////////////////////////////////

#include "LEquations.h"
#include <assert.h>


namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// 基本构造函数
//////////////////////////////////////////////////////////////////////
CLEquations::CLEquations()
{
}

//////////////////////////////////////////////////////////////////////
// 指定系数和常数构造函数
//
// 参数：
// 1. const CMatrix& mtxCoef - 指定的系数矩阵
// 2. const CMatrix& mtxConst - 指定的常数矩阵
//////////////////////////////////////////////////////////////////////
CLEquations::CLEquations(const CMatrix& mtxCoef, const CMatrix& mtxConst)
{
	bool res=Init(mtxCoef, mtxConst);
	assert(res);
}

//////////////////////////////////////////////////////////////////////
// 析构函数
//////////////////////////////////////////////////////////////////////
CLEquations::~CLEquations()
{
}

//////////////////////////////////////////////////////////////////////
// 初始化函数
//
// 参数：
// 1. const CMatrix& mtxCoef - 指定的系数矩阵
// 2. const CMatrix& mtxConst - 指定的常数矩阵
//
// 返回值：bool 型，初始化是否成功
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
// 获取系数矩阵
//
// 参数：无
//
// 返回值：CMatrix 型，返回系数矩阵
//////////////////////////////////////////////////////////////////////
inline CMatrix CLEquations::GetCoefMatrix() const
{
	return m_mtxCoef;
}

//////////////////////////////////////////////////////////////////////
// 获取常数矩阵
//
// 参数：无
//
// 返回值：CMatrix 型，返回常数矩阵
//////////////////////////////////////////////////////////////////////
inline CMatrix CLEquations::GetConstMatrix() const
{
	return m_mtxConst;
}

//////////////////////////////////////////////////////////////////////
// 获取方程个数
//
// 参数：无
//
// 返回值：int 型，返回方程组方程的个数
//////////////////////////////////////////////////////////////////////
inline int	CLEquations::GetNumEquations() const
{
	return GetCoefMatrix().GetNumRows();
}

//////////////////////////////////////////////////////////////////////
// 获取未知数个数
//
// 参数：无
//
// 返回值：int 型，返回方程组未知数的个数
//////////////////////////////////////////////////////////////////////
inline int	CLEquations::GetNumUnknowns() const
{
	return GetCoefMatrix().GetNumColumns();
}

//////////////////////////////////////////////////////////////////////
// 全选主元高斯消去法
//
// 参数：
// 1. CMatrix& mtxResult - CMatrix引用对象，返回方程组的解
//
// 返回值：bool 型，方程组求解是否成功
//////////////////////////////////////////////////////////////////////
bool CLEquations::GetRootsetGauss(CMatrix& mtxResult)
{ 
	int *pnJs,l,k,i,j,nIs,p,q;
    double d,t;

	// 方程组的属性，将常数矩阵赋给解矩阵
	mtxResult = m_mtxConst;
	double *pDataCoef = m_mtxCoef.GetData();
	double *pDataConst = mtxResult.GetData();
	int n = GetNumUnknowns();

	// 临时缓冲区，存放列数
    pnJs = new int[n];

	// 消元
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
        
		// 求解失败
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
    
	// 求解失败
	d=pDataCoef[(n-1)*n+n-1];
    if (d == 0.0)
	{ 
		delete[] pnJs;
		return false;
	}

	// 求解
    pDataConst[n-1]=pDataConst[n-1]/d;
    for (i=n-2;i>=0;i--)
    { 
		t=0.0;
        for (j=i+1;j<=n-1;j++)
			t=t+pDataCoef[i*n+j]*pDataConst[j];
        pDataConst[i]=pDataConst[i]-t;
    }
    
	// 调整解的位置
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

	// 清理内存
    delete[] pnJs;

    return true;
}

}	// end namespace