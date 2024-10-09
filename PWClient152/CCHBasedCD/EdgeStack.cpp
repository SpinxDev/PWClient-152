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

//�ɹ�ѹջ������true
//���򣬷���false
bool CEdgeStack::CheckPush(Edge e)
{
	//�Ȳ��ң�ע��m_iStackPointer�����Ԫ�ص�������������Ԫ�صĸ��������ѭ����������Ҳ�е㲻ͬ
	for(int i=0;i<m_iStackPointer;i++)
		if(m_aElements[i].start==e.start && m_aElements[i].end==e.end 
			|| m_aElements[i].start==e.end && m_aElements[i].end==e.start )
		{
			//�ñ��Ѿ���ջ���ˣ�ɾ���ñߣ�
			m_aElements.RemoveAt(i);
			m_iStackPointer--;

			return false;
		}

	//˵��û���ҵ��ñ�
	Push(e);
	return true;
}

bool CEdgeStack::IsEmpty()
{
	return (m_iStackPointer==0);
}

//��ն�ջ��
void CEdgeStack::Clear()
{
	/*
	Edge e;
	while(!IsEmpty())
		Pop(&e);
	*/

	//������ķ����ƺ������
	m_aElements.RemoveAll(); 
	m_iStackPointer = 0;
}


}	// end namespace