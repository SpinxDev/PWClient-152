// Filename	: WikiSearcher.cpp
// Creator	: Feng Ning
// Date		: 2010/04/23

#include "WikiSearcher.h"

#define new A_DEBUG_NEW

WikiSearcher::WikiSearcher()
:m_pCommand(NULL)
,m_pProvider(NULL)
,m_IsSearched(false)
{
}

WikiSearcher::~WikiSearcher()
{
	WikiSearcher::Release();
}

int WikiSearcher::GetPageSize() const
{
	return 13;
}

void WikiSearcher::TurnPageHome()
{
	if(!GetSearchCommand() || !GetContentProvider() ||
		(IsSearched() && !HavePrevPage()))
	{
		return;
	}
	
	// 初始化保证逆向搜索的数组
	FirstPage(GetContentProvider()->Begin());
	
	// 开始搜索
	SearchNextPage();
}

void WikiSearcher::SearchNextPage()
{
	if(!GetSearchCommand() || !GetContentProvider())
	{
		// must assign a command instance first
		return;
	}

	if (IsPageInvalid())
	{
		// 非法调用
		// m_pagesBegin 起码有一个元素，指示当前要搜索的位置
		// m_pagesBegin 的最后一个元素，指示本次搜索开始的位置
		// 第一页的起始搜索位置为 0
		return;
	}

	SearchCommand& command = *GetSearchCommand();
	ContentProvider& provider = *GetContentProvider();
	
	// 搜索前、子类清除用于存放搜索结果的容器等
	OnBeginSearch();
	
	// 计算搜索的起始位置
	WikiEntityPtr pCandidate = GetPageBack();

	// 搜索
	int nAdded(0);

	while (pCandidate)
	{
		// 判断当前 idNext 是否合法
		if (command(pCandidate))
		{
			// 搜索项是合法结果
			if (nAdded < GetPageSize())
			{
				// 添加到容器中
				if(OnAddSearch(pCandidate))
				{
					nAdded ++;
					
					// 通知页面管理者有物品添加，便于统计
					AddPageItem();
				}
			}
			else
			{
				// 容器已满，作为下一次搜索的起始位置
				// 并跳出循环
				break;
			}
		}
		
		// 查找下一个候选
		pCandidate = provider.Next(pCandidate);
	}

	// 搜索结束
	if (pCandidate)
	{
		// 已经搜出下一页的第一个合法值
		AddPage(pCandidate);
	}
	else
	{
		// 没有合法的下一页
		FinishPage();

		// m_pagesBegin 未更新，m_pagesBegin 的最后一项对应当前页（而非下一页）的搜索开始位置
		// 若误用 SearchNextPage，顶多也只是重新查找当前页面
	}

	// 搜索结束，通知子类列表已经更新完毕
	// 子类可根据列表中的内容，初始化其它界面内容
	OnEndSearch();

	m_IsSearched = true;
}

void WikiSearcher::SetSearchCommand(const SearchCommand* pCommand)
{
	if(m_pCommand) delete m_pCommand;
	m_pCommand = pCommand ? pCommand->Clone():NULL;

	m_IsSearched = false;
}

void WikiSearcher::SetContentProvider(const ContentProvider* pProvider)
{
	if(m_pProvider) delete m_pProvider;
	m_pProvider = pProvider ? pProvider->Clone():NULL;

	m_IsSearched = false;
}

void WikiSearcher::ResetSearchCommand()
{
	SetSearchCommand(NULL);
}

bool WikiSearcher::TestEmpty()
{
	if (!IsSearched())
	{
		TurnPageHome();
	}
	
	return IsPageEmpty();
}

void WikiSearcher::Release()
{
	WikiPage<WikiEntityPtr>::Clear();
	
	// delete current command
	SetSearchCommand(NULL);
	
	// delete current provider
	SetContentProvider(NULL);
}

void WikiSearcher::TurnPageDown()
{
	// 搜索当前页的下一页
	//
	if (!HaveNextPage())
		return;
	
	SearchNextPage();
}

void WikiSearcher::TurnPageUp()
{
	// 搜索当前页的前一页
	//
	if (!HavePrevPage())
		return;
	
	// 将页标记移到当前页的前一页
	PrevPage();
	
	// m_pagesBegin 最后一个元素，指向前一页的搜索位置，可以开始搜索
	SearchNextPage();
}

bool WikiSearcher::Refresh(const SearchCommand* pCommand)
{
	SetSearchCommand(pCommand);
	
	return !TestEmpty();
}