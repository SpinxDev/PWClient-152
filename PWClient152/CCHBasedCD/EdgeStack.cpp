// EdgeStack.cpp: implementation of the CEdgeStack class.
//
//////////////////////////////////////////////////////////////////////

#include "EdgeStack.h"

namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEdgeStack::CEdgeStack()
{

}

CEdgeStack::~CEdgeStack()
{

}

//成功压栈，返回true
//否则，返回false
bool CEdgeStack::CheckPush(Edge e)
{
	//先查找，注意m_iStackPointer是最大元素的索引，而不是元素的个数，因此循环控制条件也有点不同
	for(int i=0;i<m_iStackPointer;i++)
		if(m_aElements[i].start==e.start && m_aElements[i].end==e.end 
			|| m_aElements[i].start==e.end && m_aElements[i].end==e.start )
		{
			//该边已经在栈内了，删除该边！
			m_aElements.RemoveAt(i);
			m_iStackPointer--;

			return false;
		}

	//说明没有找到该边
	Push(e);
	return true;
}

bool CEdgeStack::IsEmpty()
{
	return (m_iStackPointer==0);
}

//清空堆栈！
void CEdgeStack::Clear()
{
	/*
	Edge e;
	while(!IsEmpty())
		Pop(&e);
	*/

	//用下面的方法似乎更快捷
	m_aElements.RemoveAll(); 
	m_iStackPointer = 0;
}


}	// end namespace