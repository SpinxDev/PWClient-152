//////////////////////////////////////////////////////////////////////
// Matrix.cpp
//
// 操作矩阵的类 CMatrix 的实现文件
//
// from source code of <科学与工程数值算法 by Zhou changfa>
//////////////////////////////////////////////////////////////////////

#include "Matrix.h"
#include <assert.h>

namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// 基本构造函数
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix()
{
	m_nNumColumns = 1;
	m_nNumRows = 1;
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);
}

//////////////////////////////////////////////////////////////////////
// 指定行列构造函数
//
// 参数：
// 1. int nRows - 指定的矩阵行数
// 2. int nCols - 指定的矩阵列数
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nRows, int nCols)
{
	m_nNumRows = nRows;
	m_nNumColumns = nCols;
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);
}

//////////////////////////////////////////////////////////////////////
// 指定值构造函数
//
// 参数：
// 1. int nRows - 指定的矩阵行数
// 2. int nCols - 指定的矩阵列数
// 3. double value[] - 一维数组，长度为nRows*nCols，存储矩阵各元素的值
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nRows, int nCols, double value[])
{
	m_nNumRows = nRows;
	m_nNumColumns = nCols;
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);

	SetData(value);
}

//////////////////////////////////////////////////////////////////////
// 方阵构造函数
//
// 参数：
// 1. int nSize - 方阵行列数
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nSize)
{
	m_nNumRows = nSize;
	m_nNumColumns = nSize;
	m_pData = NULL;
	bool bSuccess = Init(nSize, nSize);
	assert (bSuccess);
}

//////////////////////////////////////////////////////////////////////
// 方阵构造函数
//
// 参数：
// 1. int nSize - 方阵行列数
// 2. double value[] - 一维数组，长度为nRows*nRows，存储方阵各元素的值
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nSize, double value[])
{
	m_nNumRows = nSize;
	m_nNumColumns = nSize;
	m_pData = NULL;
	bool bSuccess = Init(nSize, nSize);
	assert (bSuccess);

	SetData(value);
}

//////////////////////////////////////////////////////////////////////
// 拷贝构造函数
//
// 参数：
// 1. const CMatrix& other - 源矩阵
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(const CMatrix& other)
{
	m_nNumColumns = other.GetNumColumns();
	m_nNumRows = other.GetNumRows();
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);

	// copy the pointer
	memcpy(m_pData, other.m_pData, sizeof(double)*m_nNumColumns*m_nNumRows);
}

//////////////////////////////////////////////////////////////////////
// 析构函数
//////////////////////////////////////////////////////////////////////
CMatrix::~CMatrix()
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// 初始化函数
//
// 参数：
// 1. int nRows - 指定的矩阵行数
// 2. int nCols - 指定的矩阵列数
//
// 返回值：bool 型，初始化是否成功
//////////////////////////////////////////////////////////////////////
bool CMatrix::Init(int nRows, int nCols)
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}

	m_nNumRows = nRows;
	m_nNumColumns = nCols;
	int nSize = nCols*nRows;
	if (nSize < 0)
		return false;

	// 分配内存
	m_pData = new double[nSize];
	
	if (m_pData == NULL)
		return false;					// 内存分配失败
	//if (IsBadReadPtr(m_pData, sizeof(double) * nSize))
	//	return false;

	// 将各元素值置0
	memset(m_pData, 0, sizeof(double) * nSize);

	return true;
}

//////////////////////////////////////////////////////////////////////
// 将方阵初始化为单位矩阵
//
// 参数：
// 1. int nSize - 方阵行列数
//
// 返回值：bool 型，初始化是否成功
//////////////////////////////////////////////////////////////////////
bool CMatrix::MakeUnitMatrix(int nSize)
{
	if (! Init(nSize, nSize))
		return false;

	for (int i=0; i<nSize; ++i)
		for (int j=0; j<nSize; ++j)
			if (i == j)
				SetElement(i, j, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////
// 设置矩阵各元素的值
//
// 参数：
// 1. double value[] - 一维数组，长度为m_nNumColumns*m_nNumRows，存储
//                     矩阵各元素的值
//
// 返回值：无
//////////////////////////////////////////////////////////////////////
void CMatrix::SetData(double value[])
{
	// empty the memory
	memset(m_pData, 0, sizeof(double) * m_nNumColumns*m_nNumRows);
	// copy data
	memcpy(m_pData, value, sizeof(double)*m_nNumColumns*m_nNumRows);
}

//////////////////////////////////////////////////////////////////////
// 设置指定元素的值
//
// 参数：
// 1. int nRows - 指定的矩阵行数
// 2. int nCols - 指定的矩阵列数
// 3. double value - 指定元素的值
//
// 返回值：bool 型，说明设置是否成功
//////////////////////////////////////////////////////////////////////
bool CMatrix::SetElement(int nRow, int nCol, double value)
{
	if (nCol < 0 || nCol >= m_nNumColumns || nRow < 0 || nRow >= m_nNumRows)
		return false;						// array bounds error
	if (m_pData == NULL)
		return false;							// bad pointer error
	
	m_pData[nCol + nRow * m_nNumColumns] = value;

	return true;
}

//////////////////////////////////////////////////////////////////////
// 设置指定元素的值
//
// 参数：
// 1. int nRows - 指定的矩阵行数
// 2. int nCols - 指定的矩阵列数
//
// 返回值：double 型，指定元素的值
//////////////////////////////////////////////////////////////////////
double CMatrix::GetElement(int nRow, int nCol) const
{
	assert(nCol >= 0 && nCol < m_nNumColumns && nRow >= 0 && nRow < m_nNumRows); // array bounds error
	assert(m_pData);							// bad pointer error
	return m_pData[nCol + nRow * m_nNumColumns] ;		//可见是行优先存储，即同行的元素相邻
}

//////////////////////////////////////////////////////////////////////
// 获取矩阵的列数
//
// 参数：无
//
// 返回值：int 型，矩阵的列数
//////////////////////////////////////////////////////////////////////
int	CMatrix::GetNumColumns() const
{
	return m_nNumColumns;
}

//////////////////////////////////////////////////////////////////////
// 获取矩阵的行数
//
// 参数：无
//
// 返回值：int 型，矩阵的行数
//////////////////////////////////////////////////////////////////////
int	CMatrix::GetNumRows() const
{
	return m_nNumRows;
}

//////////////////////////////////////////////////////////////////////
// 获取矩阵的数据
//
// 参数：无
//
// 返回值：double型指针，指向矩阵各元素的数据缓冲区
//////////////////////////////////////////////////////////////////////
double* CMatrix::GetData() const
{
	return m_pData;
}

//////////////////////////////////////////////////////////////////////
// 获取指定行的向量
//
// 参数：
// 1. int nRows - 指定的矩阵行数
// 2.  double* pVector - 指向向量中各元素的缓冲区
//
// 返回值：int 型，向量中元素的个数，即矩阵的列数
//////////////////////////////////////////////////////////////////////
int CMatrix::GetRowVector(int nRow, double* pVector) const
{
	if (pVector == NULL)
		delete pVector;

	pVector = new double[m_nNumColumns];
	assert(pVector != NULL);

	for (int j=0; j<m_nNumColumns; ++j)
		pVector[j] = GetElement(nRow, j);

	return m_nNumColumns;
}

//////////////////////////////////////////////////////////////////////
// 获取指定列的向量
//
// 参数：
// 1. int nCols - 指定的矩阵列数
// 2.  double* pVector - 指向向量中各元素的缓冲区
//
// 返回值：int 型，向量中元素的个数，即矩阵的行数
//////////////////////////////////////////////////////////////////////
int CMatrix::GetColVector(int nCol, double* pVector) const
{
	if (pVector == NULL)
		delete pVector;

	pVector = new double[m_nNumRows];
	assert(pVector != NULL);

	for (int i=0; i<m_nNumRows; ++i)
		pVector[i] = GetElement(i, nCol);

	return m_nNumRows;
}

//////////////////////////////////////////////////////////////////////
// 重载运算符=，给矩阵赋值
//
// 参数：
// 1. const CMatrix& other - 用于给矩阵赋值的源矩阵
//
// 返回值：CMatrix型的引用，所引用的矩阵与other相等
//////////////////////////////////////////////////////////////////////
CMatrix& CMatrix::operator=(const CMatrix& other)
{
	if (&other != this)
	{
		bool bSuccess = Init(other.GetNumRows(), other.GetNumColumns());
		assert(bSuccess);

		// copy the pointer
		memcpy(m_pData, other.m_pData, sizeof(double)*m_nNumColumns*m_nNumRows);
	}

	// finally return a reference to ourselves
	return *this ;
}

//////////////////////////////////////////////////////////////////////
// 重载运算符==，判断矩阵是否相等
//
// 参数：
// 1. const CMatrix& other - 用于比较的矩阵
//
// 返回值：bool 型，两个矩阵相等则为true，否则为false
//////////////////////////////////////////////////////////////////////
bool CMatrix::operator==(const CMatrix& other) const
{
	// 首先检查行列数是否相等
	if (m_nNumColumns != other.GetNumColumns() || m_nNumRows != other.GetNumRows())
		return false;

	for (int i=0; i<m_nNumRows; ++i)
	{
		for (int j=0; j<m_nNumColumns; ++j)
		{
			if (GetElement(i, j) != other.GetElement(i, j))
				return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// 重载运算符!=，判断矩阵是否不相等
//
// 参数：
// 1. const CMatrix& other - 用于比较的矩阵
//
// 返回值：bool 型，两个不矩阵相等则为true，否则为false
//////////////////////////////////////////////////////////////////////
bool CMatrix::operator!=(const CMatrix& other) const
{
	return !(*this == other);
}

//////////////////////////////////////////////////////////////////////
// 重载运算符+，实现矩阵的加法
//
// 参数：
// 1. const CMatrix& other - 与指定矩阵相加的矩阵
//
// 返回值：CMatrix型，指定矩阵与other相加之和
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator+(const CMatrix& other) const
{
	// 首先检查行列数是否相等
	assert (m_nNumColumns == other.GetNumColumns() && m_nNumRows == other.GetNumRows());

	// 构造结果矩阵
	CMatrix	result(*this) ;		// 拷贝构造
	// 矩阵加法
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j <  m_nNumColumns; ++j)
			result.SetElement(i, j, result.GetElement(i, j) + other.GetElement(i, j)) ;
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// 重载运算符-，实现矩阵的减法
//
// 参数：
// 1. const CMatrix& other - 与指定矩阵相减的矩阵
//
// 返回值：CMatrix型，指定矩阵与other相减之差
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator-(const CMatrix& other) const
{
	// 首先检查行列数是否相等
	assert (m_nNumColumns == other.GetNumColumns() && m_nNumRows == other.GetNumRows());

	// 构造目标矩阵
	CMatrix	result(*this) ;		// copy ourselves
	// 进行减法操作
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j <  m_nNumColumns; ++j)
			result.SetElement(i, j, result.GetElement(i, j) - other.GetElement(i, j)) ;
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// 重载运算符*，实现矩阵的数乘
//
// 参数：
// 1. double value - 与指定矩阵相乘的实数
//
// 返回值：CMatrix型，指定矩阵与value相乘之积
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator*(double value) const
{
	// 构造目标矩阵
	CMatrix	result(*this) ;		// copy ourselves
	// 进行数乘
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j <  m_nNumColumns; ++j)
			result.SetElement(i, j, result.GetElement(i, j) * value) ;
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// 重载运算符*，实现矩阵的乘法
//
// 参数：
// 1. const CMatrix& other - 与指定矩阵相乘的矩阵
//
// 返回值：CMatrix型，指定矩阵与other相乘之积
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator*(const CMatrix& other) const
{
	// 首先检查行列数是否符合要求
	assert (m_nNumColumns == other.GetNumRows());

	// construct the object we are going to return
	CMatrix	result(m_nNumRows, other.GetNumColumns()) ;

	// 矩阵乘法，即
	//
	// [A][B][C]   [G][H]     [A*G + B*I + C*K][A*H + B*J + C*L]
	// [D][E][F] * [I][J] =   [D*G + E*I + F*K][D*H + E*J + F*L]
	//             [K][L]
	//
	double	value ;
	for (int i = 0 ; i < result.GetNumRows() ; ++i)
	{
		for (int j = 0 ; j < other.GetNumColumns() ; ++j)
		{
			value = 0.0 ;
			for (int k = 0 ; k < m_nNumColumns ; ++k)
			{
				value += GetElement(i, k) * other.GetElement(k, j) ;
			}

			result.SetElement(i, j, value) ;
		}
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// 复矩阵的乘法
//
// 参数：
// 1. const CMatrix& AR - 左边复矩阵的实部矩阵
// 2. const CMatrix& AI - 左边复矩阵的虚部矩阵
// 3. const CMatrix& BR - 右边复矩阵的实部矩阵
// 4. const CMatrix& BI - 右边复矩阵的虚部矩阵
// 5. CMatrix& CR - 乘积复矩阵的实部矩阵
// 6. CMatrix& CI - 乘积复矩阵的虚部矩阵
//
// 返回值：bool型，复矩阵乘法是否成功
//////////////////////////////////////////////////////////////////////
bool CMatrix::CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, const CMatrix& BI, CMatrix& CR, CMatrix& CI) const
{
	// 首先检查行列数是否符合要求
	if (AR.GetNumColumns() != AI.GetNumColumns() ||
		AR.GetNumRows() != AI.GetNumRows() ||
		BR.GetNumColumns() != BI.GetNumColumns() ||
		BR.GetNumRows() != BI.GetNumRows() ||
		AR.GetNumColumns() != BR.GetNumRows())
		return false;

	// 构造乘积矩阵实部矩阵和虚部矩阵
	CMatrix mtxCR(AR.GetNumRows(), BR.GetNumColumns()), mtxCI(AR.GetNumRows(), BR.GetNumColumns());
	// 复矩阵相乘
    for (int i=0; i<AR.GetNumRows(); ++i)
	{
	    for (int j=0; j<BR.GetNumColumns(); ++j)
		{
			double vr = 0;
			double vi = 0;
            for (int k =0; k<AR.GetNumColumns(); ++k)
			{
                double p = AR.GetElement(i, k) * BR.GetElement(k, j);
                double q = AI.GetElement(i, k) * BI.GetElement(k, j);
                double s = (AR.GetElement(i, k) + AI.GetElement(i, k)) * (BR.GetElement(k, j) + BI.GetElement(k, j));
                vr += p - q;
                vi += s - p - q;
			}
            mtxCR.SetElement(i, j, vr);
            mtxCI.SetElement(i, j, vi);
        }
	}

	CR = mtxCR;
	CI = mtxCI;

	return true;
}

//////////////////////////////////////////////////////////////////////
// 矩阵的转置
//
// 参数：无
//
// 返回值：CMatrix型，指定矩阵转置矩阵
//////////////////////////////////////////////////////////////////////
CMatrix CMatrix::Transpose() const
{
	// 构造目标矩阵
	CMatrix	Trans(m_nNumColumns, m_nNumRows);

	// 转置各元素
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j < m_nNumColumns ; ++j)
			Trans.SetElement(j, i, GetElement(i, j)) ;
	}

	return Trans;
}

}	// end namespace