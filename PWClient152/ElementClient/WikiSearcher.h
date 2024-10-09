// Filename	: WikiSearcher.h
// Creator	: Feng Ning
// Date		: 2010/04/23

#pragma once

#include "WikiPage.h"
#include "WikiEntity.h"

class WikiSearcher : protected WikiPage<WikiEntityPtr>
{
public:
	WikiSearcher();
	virtual ~WikiSearcher();
	
	// this abstract implement the search command on a entity
	class SearchCommand
	{
	public:
		virtual bool operator()(WikiEntityPtr candidate) const = 0;
		virtual SearchCommand* Clone() const = 0;
		virtual ~SearchCommand(){};

	protected:
		SearchCommand(){};

	private:
		SearchCommand(const SearchCommand&);
		SearchCommand& operator=(const SearchCommand&);
	};

	// this abstract class provide the entity that can be searched
	class ContentProvider
	{
	public:
		virtual WikiEntityPtr Next(WikiEntityPtr p) = 0;
		virtual WikiEntityPtr Begin() = 0;

		virtual ContentProvider* Clone() const = 0;
		virtual ~ContentProvider(){};

	protected:
		ContentProvider(){};

	private:
		ContentProvider(const ContentProvider&);
		ContentProvider& operator=(const ContentProvider&);
	};

	// release current searcher
	void Release();

	// try to turn to the next page
	void TurnPageDown();

	// try to turn to the previous page
	void TurnPageUp();

	// try to turn to the first page
	void TurnPageHome();

	// reset the command and refresh content
	bool Refresh(const SearchCommand* pCommand);
	
	// whether the result set is empty
	bool TestEmpty();

	// base method must be called in derived methods
	virtual void ResetSearchCommand();
	
	// stop search when reached page size limitation
	virtual int  GetPageSize() const;

	// set a search command to searcher
	void SetSearchCommand(const SearchCommand* pCommand);

	// set a content provider
	void SetContentProvider(const ContentProvider* pProvider);

	// get command from base class
	SearchCommand* GetSearchCommand() {return m_pCommand;}
	
	// get provider from base class
	ContentProvider* GetContentProvider() {return m_pProvider;}

	bool IsSearched() {return m_IsSearched;}

protected:
	// search execution template pattern
	virtual void OnBeginSearch() {};
	virtual bool OnAddSearch(WikiEntityPtr p) = 0;
	virtual void OnEndSearch() {};
	
private:
	// search execution template pattern
	void SearchNextPage();

	SearchCommand* m_pCommand;
	ContentProvider* m_pProvider;
	bool m_IsSearched;
};
