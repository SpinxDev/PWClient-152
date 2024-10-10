//////////////////////////////////////////////////////////////////////
// LEquations.h
//
// 求解线性方程组的类 CLEquations 的声明接口
//
// from source code of <科学与工程数值算法 by Zhou changfa>
//////////////////////////////////////////////////////////////////////

#ifndef	_LEQUATIONS_H_
#define _LEQUATIONS_H_

// 需要调用矩阵操作类
#include "Matrix.h"

namespace CHBasedCD
{

// 类声明
class CLEquations  
{
	//
	// 公有接口函数
	//
public:

	//
	// 构造与析构
	//

	CLEquations();				// 默认构造函数
	// 指定系数和常数构造函数
	CLEquations(const CMatrix& mtxCoef, const CMatrix& mtxConst);
	virtual ~CLEquations();		// 析构函数
	// 初始化
	bool Init(const CMatrix& mtxCoef, const CMatrix& mtxConst);

	//
	// 属性
	//

	CMatrix GetCoefMatrix() const;	// 获取系数矩阵
	CMatrix GetConstMatrix() const;	// 获取常数矩阵
	int	GetNumEquations() const;	// 获取方程个数
	int	GetNumUnknowns() const;		// 获取未知数个数

	//
	// 线性方程组求解算法
	//

	// 全选主元高斯消去法
	bool GetRootsetGauss(CMatrix& mtxResult);

	//
	// 保护性数据成员
	//
protected:
	CMatrix	m_mtxCoef;		// 系数矩阵
	CMatrix m_mtxConst;		// 常数矩阵

};

}	// end namespace

#endif // _LEQUATIONS_H_
