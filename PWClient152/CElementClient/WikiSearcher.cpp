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
	
	// ��ʼ����֤��������������
	FirstPage(GetContentProvider()->Begin());
	
	// ��ʼ����
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
		// �Ƿ�����
		// m_pagesBegin ������һ��Ԫ�أ�ָʾ��ǰҪ������λ��
		// m_pagesBegin �����һ��Ԫ�أ�ָʾ����������ʼ��λ��
		// ��һҳ����ʼ����λ��Ϊ 0
		return;
	}

	SearchCommand& command = *GetSearchCommand();
	ContentProvider& provider = *GetContentProvider();
	
	// ����ǰ������������ڴ�����������������
	OnBeginSearch();
	
	// ������������ʼλ��
	WikiEntityPtr pCandidate = GetPageBack();

	// ����
	int nAdded(0);

	while (pCandidate)
	{
		// �жϵ�ǰ idNext �Ƿ�Ϸ�
		if (command(pCandidate))
		{
			// �������ǺϷ����
			if (nAdded < GetPageSize())
			{
				// ��ӵ�������
				if(OnAddSearch(pCandidate))
				{
					nAdded ++;
					
					// ֪ͨҳ�����������Ʒ��ӣ�����ͳ��
					AddPageItem();
				}
			}
			else
			{
				// ������������Ϊ��һ����������ʼλ��
				// ������ѭ��
				break;
			}
		}
		
		// ������һ����ѡ
		pCandidate = provider.Next(pCandidate);
	}

	// ��������
	if (pCandidate)
	{
		// �Ѿ��ѳ���һҳ�ĵ�һ���Ϸ�ֵ
		AddPage(pCandidate);
	}
	else
	{
		// û�кϷ�����һҳ
		FinishPage();

		// m_pagesBegin δ���£�m_pagesBegin �����һ���Ӧ��ǰҳ��������һҳ����������ʼλ��
		// ������ SearchNextPage������Ҳֻ�����²��ҵ�ǰҳ��
	}

	// ����������֪ͨ�����б��Ѿ��������
	// ����ɸ����б��е����ݣ���ʼ��������������
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
	// ������ǰҳ����һҳ
	//
	if (!HaveNextPage())
		return;
	
	SearchNextPage();
}

void WikiSearcher::TurnPageUp()
{
	// ������ǰҳ��ǰһҳ
	//
	if (!HavePrevPage())
		return;
	
	// ��ҳ����Ƶ���ǰҳ��ǰһҳ
	PrevPage();
	
	// m_pagesBegin ���һ��Ԫ�أ�ָ��ǰһҳ������λ�ã����Կ�ʼ����
	SearchNextPage();
}

bool WikiSearcher::Refresh(const SearchCommand* pCommand)
{
	SetSearchCommand(pCommand);
	
	return !TestEmpty();
}