//////////////////////////////////////////////////////////////////////
// Matrix.h
//
// ����������� CMatrix �������ӿ�
//
// from source code of <��ѧ�빤����ֵ�㷨 by Zhou changfa>
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
	// ˵�� by wenfeng,2004-9-21
	// ��������ݻ�����Ϊһάdouble*	m_pData
	// �洢˳��Ϊ�����ȣ������д洢��ͬһ�е�Ԫ����λ�����ڽ�


	//
	// ���нӿں���
	//
public:

	//
	// ����������
	//

	CMatrix();										// �������캯��
	CMatrix(int nRows, int nCols);					// ָ�����й��캯��
	CMatrix(int nRows, int nCols, double value[]);	// ָ�����ݹ��캯��
	CMatrix(int nSize);								// �����캯��
	CMatrix(int nSize, double value[]);				// ָ�����ݷ����캯��
	CMatrix(const CMatrix& other);					// �������캯��
	bool	Init(int nRows, int nCols);				// ��ʼ������	
	bool	MakeUnitMatrix(int nSize);				// �������ʼ��Ϊ��λ����
	virtual ~CMatrix();								// ��������

	//
	// Ԫ����ֵ����
	//

	bool	SetElement(int nRow, int nCol, double value);	// ����ָ��Ԫ�ص�ֵ
	double	GetElement(int nRow, int nCol) const;			// ��ȡָ��Ԫ�ص�ֵ
	void    SetData(double value[]);						// ���þ����ֵ
	int		GetNumColumns() const;							// ��ȡ���������
	int		GetNumRows() const;								// ��ȡ���������
	int     GetRowVector(int nRow, double* pVector) const;	// ��ȡ�����ָ���о���
	int     GetColVector(int nCol, double* pVector) const;	// ��ȡ�����ָ���о���
	double* GetData() const;								// ��ȡ�����ֵ

	//
	// ��ѧ����
	//

	CMatrix& operator=(const CMatrix& other);
	bool operator==(const CMatrix& other) const;
	bool operator!=(const CMatrix& other) const;
	CMatrix	operator+(const CMatrix& other) const;
	CMatrix	operator-(const CMatrix& other) const;
	CMatrix	operator*(double value) const;
	CMatrix	operator*(const CMatrix& other) const;
	// ������˷�
	bool CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, const CMatrix& BI, CMatrix& CR, CMatrix& CI) const;
	// �����ת��
	CMatrix Transpose() const;


	//
	// ���������ݳ�Ա
	//
protected:
	int	m_nNumColumns;			// ��������
	int	m_nNumRows;				// ��������
	double*	m_pData;			// �������ݻ�����

	//
	// �ڲ�����
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
