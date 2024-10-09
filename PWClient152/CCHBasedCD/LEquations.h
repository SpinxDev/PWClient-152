//////////////////////////////////////////////////////////////////////
// LEquations.h
//
// ������Է�������� CLEquations �������ӿ�
//
// from source code of <��ѧ�빤����ֵ�㷨 by Zhou changfa>
//////////////////////////////////////////////////////////////////////

#ifndef	_LEQUATIONS_H_
#define _LEQUATIONS_H_

// ��Ҫ���þ��������
#include "Matrix.h"

namespace CHBasedCD
{

// ������
class CLEquations  
{
	//
	// ���нӿں���
	//
public:

	//
	// ����������
	//

	CLEquations();				// Ĭ�Ϲ��캯��
	// ָ��ϵ���ͳ������캯��
	CLEquations(const CMatrix& mtxCoef, const CMatrix& mtxConst);
	virtual ~CLEquations();		// ��������
	// ��ʼ��
	bool Init(const CMatrix& mtxCoef, const CMatrix& mtxConst);

	//
	// ����
	//

	CMatrix GetCoefMatrix() const;	// ��ȡϵ������
	CMatrix GetConstMatrix() const;	// ��ȡ��������
	int	GetNumEquations() const;	// ��ȡ���̸���
	int	GetNumUnknowns() const;		// ��ȡδ֪������

	//
	// ���Է���������㷨
	//

	// ȫѡ��Ԫ��˹��ȥ��
	bool GetRootsetGauss(CMatrix& mtxResult);

	//
	// ���������ݳ�Ա
	//
protected:
	CMatrix	m_mtxCoef;		// ϵ������
	CMatrix m_mtxConst;		// ��������

};

}	// end namespace

#endif // _LEQUATIONS_H_
