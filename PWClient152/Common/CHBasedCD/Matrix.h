//////////////////////////////////////////////////////////////////////
// Matrix.h
//
// 操作矩阵的类 CMatrix 的声明接口
//
// from source code of <科学与工程数值算法 by Zhou changfa>
//
// Simplified by He wenfeng  test
//////////////////////////////////////////////////////////////////////

#ifndef	_MATRIX_H_
#define _MATRIX_H_

#include <math.h>

#if !defined(_BITSET_)
#	include <bitset>
#endif // !defined(_BITSET_)
//////////////////////////////////////////////////////////////////////////////////////


namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////////////////////
//
//(-- class CMatrix
//
class CMatrix  
{
	// 说明 by wenfeng,2004-9-21
	// 其矩阵数据缓冲区为一维double*	m_pData
	// 存储顺序为行优先，即逐行存储，同一行的元素在位置上邻近


	//
	// 公有接口函数
	//
public:

	//
	// 构造与析构
	//

	CMatrix();										// 基础构造函数
	CMatrix(int nRows, int nCols);					// 指定行列构造函数
	CMatrix(int nRows, int nCols, double value[]);	// 指定数据构造函数
	CMatrix(int nSize);								// 方阵构造函数
	CMatrix(int nSize, double value[]);				// 指定数据方阵构造函数
	CMatrix(const CMatrix& other);					// 拷贝构造函数
	bool	Init(int nRows, int nCols);				// 初始化矩阵	
	bool	MakeUnitMatrix(int nSize);				// 将方阵初始化为单位矩阵
	virtual ~CMatrix();								// 析构函数

	//
	// 元素与值操作
	//

	bool	SetElement(int nRow, int nCol, double value);	// 设置指定元素的值
	double	GetElement(int nRow, int nCol) const;			// 获取指定元素的值
	void    SetData(double value[]);						// 设置矩阵的值
	int		GetNumColumns() const;							// 获取矩阵的列数
	int		GetNumRows() const;								// 获取矩阵的行数
	int     GetRowVector(int nRow, double* pVector) const;	// 获取矩阵的指定行矩阵
	int     GetColVector(int nCol, double* pVector) const;	// 获取矩阵的指定列矩阵
	double* GetData() const;								// 获取矩阵的值

	//
	// 数学操作
	//

	CMatrix& operator=(const CMatrix& other);
	bool operator==(const CMatrix& other) const;
	bool operator!=(const CMatrix& other) const;
	CMatrix	operator+(const CMatrix& other) const;
	CMatrix	operator-(const CMatrix& other) const;
	CMatrix	operator*(double value) const;
	CMatrix	operator*(const CMatrix& other) const;
	// 复矩阵乘法
	bool CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, const CMatrix& BI, CMatrix& CR, CMatrix& CI) const;
	// 矩阵的转置
	CMatrix Transpose() const;


	//
	// 保护性数据成员
	//
protected:
	int	m_nNumColumns;			// 矩阵列数
	int	m_nNumRows;				// 矩阵行数
	double*	m_pData;			// 矩阵数据缓冲区

	//
	// 内部函数
	//
private:
	void ppp(double a[], double e[], double s[], double v[], int m, int n);
	void sss(double fg[2], double cs[2]);

};
//
//--) // class CMatrix
//
//////////////////////////////////////////////////////////////////////////////////////

}	// end namespace

#endif // _MATRIX_H_
