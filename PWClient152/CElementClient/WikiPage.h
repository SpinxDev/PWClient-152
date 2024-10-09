// Filename	: WikiPage.h
// Creator	: Xu Wenbin
// Date		: 2010/04/08

#pragma once

#include <vector.h>

//
//	WikiPage:	��ҳ����������
//				��Ҫ�ṩ��ǰ��ҳ
//				�ṩÿҳ�ı�Ǽ�¼
//
template <typename P>
class WikiPage
{
public:
	typedef P PAGE_POSITION;

	WikiPage()
		: m_bLastPage(false)
		, m_bEmpty(true)
	{}

	void FirstPage(const PAGE_POSITION & p0){
		// ���¿�ʼ��¼ҳ��p0Ϊ��ҳ�Ŀ�ʼ���
		m_pagesBegin.clear();
		m_pagesBegin.push_back(p0);		//	��һ�δ�����ҳΪ��һ��ҳ��
		m_bLastPage = false;
		m_bEmpty = true;
	}
	void AddPageItem(){
		// ��ӱ�ҳ��һ����Ŀ
		m_bEmpty = false;
	}
	void AddPage(const PAGE_POSITION &p){
		// �����ҳ���
		m_pagesBegin.push_back(p);
	}
	void FinishPage(){
		// ���ҳ��λ�ü�¼��ָʾ��һҳΪ���һҳ		
		m_bLastPage = true;
	}

	int  GetPagesCount()const{
		return (int)m_pagesBegin.size();
	}
	const PAGE_POSITION & GetPageBack()const{
		return m_pagesBegin.back();
	}
	bool IsPageInvalid()const{
		return m_pagesBegin.empty();
	}
	bool IsPageEmpty()const{
		return m_bEmpty;
	}
	bool HaveNextPage()const{

		// �ж��Ƿ�����һҳ�������������

		return !m_bLastPage && m_pagesBegin.size()>1;
	}
	bool HavePrevPage()const{
		
		// �ж��Ƿ���Է�����ǰҳ��ǰһҳ

		if (m_pagesBegin.size() == 0)
		{
			return false;
		}		
		if (m_pagesBegin.size() == 1)
		{
			// �� m_pagesBegin size Ϊ 1����ʾ��������������ֻ��һҳ��������������δչ��
			return false;
		}		
		if (m_pagesBegin.size() == 2)
		{
			if (!m_bLastPage)
			{
				// �� m_pagesBegin size Ϊ 2����ʾ��ǰֻ�����˵�һҳ����һҳ�д�����
				return false;
			}
		}
		return true;
	}

	void PrevPage(){

		// ������ǰҳ��ǰһҳ

		if (!HavePrevPage())
			return;

		// ������������ʼλ��
		if (!m_bLastPage){
			m_pagesBegin.pop_back();//	����һҳ��������ǰҳ��һҳ�Ŀ�ʼ����λ��
		}
		m_pagesBegin.pop_back();	//	������ǰҳ�Ŀ�ʼ����λ��
		m_bLastPage = false;		//	�Ѿ��������һҳ
	}
	
	void Clear()
	{
		m_pagesBegin.clear();
		m_bLastPage = false;
		m_bEmpty = true;
	}

private:

	abase::vector<PAGE_POSITION> m_pagesBegin;	//	��¼ÿҳ�Ŀ�ʼλ��
	bool m_bLastPage;	//	��ǰҳ�Ƿ������һҳ
	bool m_bEmpty;		//	�ϴ������Ƿ�û���������κ�ҳ
};
