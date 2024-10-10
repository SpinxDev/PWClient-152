/////////////////////////////////////////////////
//	Created by He wenfeng
//  2004-8-18
//  A class to use as a edge stack for Gift Wrip
/////////////////////////////////////////////////

#ifndef	_EDGE_STACK_H_
#define _EDGE_STACK_H_

#include <AStack.h>
#include "ConvexHullAlgorithm.h"

namespace CHBasedCD
{


class CEdgeStack:public AStack<Edge, Edge>
{
public:
	CEdgeStack();
	virtual ~CEdgeStack();

// some new operations
public:
	void Clear();
	bool IsEmpty();

	bool CheckPush(Edge e);			//ѡ����ѹջ�����e�Ѿ���ջ�У��򲻵���ѹ������������

//some new attributes
protected:

};

}	// end namespace

#endif // _EDGE_STACK_H_
