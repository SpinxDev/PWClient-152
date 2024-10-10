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
// 从m种不同的选择中选出n种作为一个组合
// 本类列出了每一种可能的选法
// 即一个C(n,m)问题
/////////////////////////////////////////////////////////////
class CCombGenerator  
{
public:
	void GetNextComb(int* pData);				//将当前算出的组合传给pData，同时计算下一个组合
	bool IsOver() {	return m_bOver;}			//是否已经全部罗列出所有的组合？
	void Set(int nn,int mm);
	bool IsSolvable() { return m_bSolvable;}
	CCombGenerator(int nn,int mm);				//初始化 C(n,m)
	virtual ~CCombGenerator();

private:
	int* m_pData;							//存储选出的n种情况
	bool m_bSolvable;						//是否有解？当n>m以及n,m为非正时，置为false
	bool m_bOver;							//标记是否已经全部罗列出所有的组合？
	int m;
	int n;
};

}	// end namespace

#endif // _COMBGENERATOR_H_
