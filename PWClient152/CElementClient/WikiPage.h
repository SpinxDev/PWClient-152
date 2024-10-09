// Filename	: WikiPage.h
// Creator	: Xu Wenbin
// Date		: 2010/04/08

#pragma once

#include <vector.h>

//
//	WikiPage:	分页搜索管理类
//				主要提供向前翻页
//				提供每页的标记记录
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
		// 重新开始记录页，p0为首页的开始标记
		m_pagesBegin.clear();
		m_pagesBegin.push_back(p0);		//	第一次待搜索页为第一个页面
		m_bLastPage = false;
		m_bEmpty = true;
	}
	void AddPageItem(){
		// 添加本页的一个条目
		m_bEmpty = false;
	}
	void AddPage(const PAGE_POSITION &p){
		// 添加新页标记
		m_pagesBegin.push_back(p);
	}
	void FinishPage(){
		// 完成页面位置记录，指示上一页为最后一页		
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

		// 判断是否有下一页，供向后搜索用

		return !m_bLastPage && m_pagesBegin.size()>1;
	}
	bool HavePrevPage()const{
		
		// 判断是否可以翻到当前页的前一页

		if (m_pagesBegin.size() == 0)
		{
			return false;
		}		
		if (m_pagesBegin.size() == 1)
		{
			// 若 m_pagesBegin size 为 1，表示所有搜索结果最多只有一页，或者搜索尚且未展开
			return false;
		}		
		if (m_pagesBegin.size() == 2)
		{
			if (!m_bLastPage)
			{
				// 若 m_pagesBegin size 为 2，表示当前只搜索了第一页，下一页尚待搜索
				return false;
			}
		}
		return true;
	}

	void PrevPage(){

		// 翻到当前页的前一页

		if (!HavePrevPage())
			return;

		// 计算搜索的起始位置
		if (!m_bLastPage){
			m_pagesBegin.pop_back();//	有下一页，弹出当前页下一页的开始搜索位置
		}
		m_pagesBegin.pop_back();	//	弹出当前页的开始搜索位置
		m_bLastPage = false;		//	已经不是最后一页
	}
	
	void Clear()
	{
		m_pagesBegin.clear();
		m_bLastPage = false;
		m_bEmpty = true;
	}

private:

	abase::vector<PAGE_POSITION> m_pagesBegin;	//	记录每页的开始位置
	bool m_bLastPage;	//	当前页是否是最后一页
	bool m_bEmpty;		//	上次搜索是否没有搜索到任何页
};
