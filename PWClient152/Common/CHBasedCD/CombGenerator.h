/*
 * FILE: CombGenerator.h
 *
 * DESCRIPTION: a class to generate a combinatory number
 *				such as C(n,m)
 *
 * CREATED BY: He wenfeng, 2004/9/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_COMBGENERATOR_H_
#define _COMBGENERATOR_H_

namespace CHBasedCD
{


/////////////////////////////////////////////////////////////
// ��m�ֲ�ͬ��ѡ����ѡ��n����Ϊһ�����
// �����г���ÿһ�ֿ��ܵ�ѡ��
// ��һ��C(n,m)����
/////////////////////////////////////////////////////////////
class CCombGenerator  
{
public:
	void GetNextComb(int* pData);				//����ǰ�������ϴ���pData��ͬʱ������һ�����
	bool IsOver() {	return m_bOver;}			//�Ƿ��Ѿ�ȫ�����г����е���ϣ�
	void Set(int nn,int mm);
	bool IsSolvable() { return m_bSolvable;}
	CCombGenerator(int nn,int mm);				//��ʼ�� C(n,m)
	virtual ~CCombGenerator();

private:
	int* m_pData;							//�洢ѡ����n�����
	bool m_bSolvable;						//�Ƿ��н⣿��n>m�Լ�n,mΪ����ʱ����Ϊfalse
	bool m_bOver;							//����Ƿ��Ѿ�ȫ�����г����е���ϣ�
	int m;
	int n;
};

}	// end namespace

#endif // _COMBGENERATOR_H_
