/*
 * FILE: EC_BBSPage.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_BBSPageFile.h"
#include "AUI.h"
#include "A3DViewport.h"

#ifndef EC_BBSPAGE_H
#define EC_BBSPAGE_H

const int MaxTableRows=80;
const int MaxTableCols=30;

class CECBBSPage;

//==================================DownloadedPageManager=============================
struct DownloadedPagesInfo
{
	AString		url;
	AString		localfn;
	bool		downloaded;
	HANDLE		downloadthread;
	int			totalparm;
	AWString	*parm;

// add a ref counter for this shared data
// (shared_ptr should be a better choice)

	DownloadedPagesInfo(CRITICAL_SECTION* cs);
	
	void Release();
	void ReleaseInThread();
	void AddRef();
	void AddRefInThread();

private:
	DownloadedPagesInfo(const DownloadedPagesInfo&);
	DownloadedPagesInfo& operator=(const DownloadedPagesInfo&);

	~DownloadedPagesInfo() {}; // do nothing, just seal the destructor

	CRITICAL_SECTION* m_pCS;
	int m_Counter;
};

struct DownloadThreadData
{
	DownloadedPagesInfo *pageinfo;
	int					parmtype;
	bool				bGet;
	AString				parm;
};

class DownloadedPagesManager
{
public:
	DownloadedPagesManager();
	~DownloadedPagesManager();
	void Init();
	void Release();
	AString GetPagesLocalFn(int index);
	AString GetPagesURL(int index);
	DownloadedPagesInfo* GetPagesInfo(int index);
	int AddPage(AString url,CECBBSPage *page);
	void SaveParm(int index,CECBBSPage *page);
	void LoadParm(int index,CECBBSPage *page);

public:
	DownloadedPagesInfo**	m_DownloadedPages;
	int						m_nTotal;
	int						m_nMax;

private:
	CRITICAL_SECTION	m_CS;
	typedef abase::vector<HANDLE> CSUsers;
	CSUsers m_CSUsers;
};

//==================================HistoryPagesManager=============================
struct HistoryPageNode
{
	HistoryPageNode	*next;
	AString			target;	
	int				pageindex;
	bool			bLoaded;
};

class HistoryPagesInfo
{
private:
	HistoryPageNode* head;
public:
	HistoryPagesInfo();
	~HistoryPagesInfo();
	void Release();
	void AddPageNode(AString target,int pageindex);
	HistoryPageNode* GetPageNode(AString target);
	void SetRefreshAll();
};

class HistoryPagesManager
{
public:
	HistoryPagesManager();
	~HistoryPagesManager();
	void Init();
	void Release();
	void SetNowPage(int nPage,bool bTotalPage=false);
	int GetNowPage();
	void SaveNowPages(CECBBSPage *basepage);
	int GetTotalPages();
	HistoryPagesInfo* GetHistoryPageInfo(int index);

private:
	void SavePage(CECBBSPage *page);

public:
	HistoryPagesInfo	**m_Pages;
	int					m_nTotal;
	int					m_nMax;
	int					m_nNow;
};

//=====================================ObjectList=====================================
struct ObjectNode
{
	ObjectNode	*next;
	HTMLLabel	*label;
	AUIObject	*obj;
	CECBBSPage	*page;
};

class ObjectList
{
public:
	ObjectNode	*head;
	ObjectNode	*tail;
public:
	ObjectList();
	~ObjectList();
	ObjectNode *FindObject(PAUIOBJECT pObj);
	ObjectNode *FindObject(HTMLLabel *label);
	void Release();
	bool LoadNewImage();
	void AddObject(HTMLLabel *label,PAUIOBJECT pObj,CECBBSPage *hPage);
	void SetLabelColor(HTMLLabel *hLabel,A3DCOLOR aTextColor);
	void ScrollPage(int iInc);
};

struct CECBBSPageNode
{
	CECBBSPageNode	*next;
	CECBBSPage		*page;
};

//=====================================CECBBSPageList==================================
class CECBBSPageList
{
public:
	CECBBSPageNode	*head;
public:
	CECBBSPageList();
	~CECBBSPageList();
	CECBBSPage* FindPage(AString aName);
	ObjectNode *FindObject(PAUIOBJECT pObj);
	CECBBSPage* CreatePage(CECBBSPage *parentPage);
	CECBBSPage* GetPageFromCursor(int x,int y);
	bool LoadNewImage();
	bool PrepareNewPage();
	void ScrollPage(int iInc);
};

struct HTMLTable
{
	int iAlign;
	int iTotalCols;
	int iTotalRows;
	int iRows;
	int iCols;
	short iTableWidth;
	short iWidth[MaxTableCols];
	short iMinWidth[MaxTableCols];
	short iHeight[MaxTableRows];
	short iTdWidth[MaxTableRows][MaxTableCols];
	short iTdMinWidth[MaxTableRows][MaxTableCols];
	short iTdHeight[MaxTableRows][MaxTableCols];
	short iIncCol[MaxTableRows][MaxTableCols];
	byte iSpanRows[MaxTableRows][MaxTableCols];
	byte iSpanCols[MaxTableRows][MaxTableCols];
	bool bXLine[MaxTableRows][MaxTableCols];
	bool bYLine[MaxTableRows][MaxTableCols];
	short iDesWidth[MaxTableCols];
	byte	iInit;
	A3DRECT tableRect;
};

//======================================TableList=====================================
struct TableListNode
{
	TableListNode	*next;
	HTMLTable		*table;
	HTMLLabel		*label;
};

class TableList
{
public:
	TableListNode *head;
public:
	TableList();
	~TableList();
	void AddTable(HTMLLabel *hLabel,HTMLTable *hTable);
	HTMLTable* GetTable(HTMLLabel *hLabel);
	void ReleaseTables();
};

//======================================CECBBSPage=====================================
struct GlobalInfo
{
	int			iTextSize;
	bool		bPreText;
	bool		bDrawObject;
	A3DCOLOR	iTextColor;
	ACHAR		aFont[20];
	int			iAlign;
	int			iVAlign;
};

struct ThreadData
{
	CECBBSPage *page;
	AString	pagename;
	AString localpagename;
	bool bStop;
	UINT threadid;
	HANDLE thread;
	bool bAutoLoad;
};

class CECBBSPage
{
public:
	bool			m_bGet;
	int				m_iContentType;
	int				m_iRadioGroupID;
	int				m_iRadioButtonID[100];
	AString			m_aGroupName[100];
	HTMLLabel		*m_hHrefLabel;
	AString			m_aParm;
	AString			m_aName;
	CECBBSPageFile	m_hFile;
	int				m_iCodePage;
	A3DRECT			m_aPostion;
	CECBBSPage		*m_hParentPage;
	int				m_iFirstLineWidth;
	int				m_iFirstLineHeight;
	bool			m_bBreak;
	HTMLTable		*m_hTable;
	GlobalInfo		*m_pGlobalInfo;
	ObjectNode		*m_pLineStart;
	HTMLLabel		*m_pRootLabel;
	HTMLLabel		*m_pNewRootLabel;
	ObjectList		*m_pObjectList;
	ObjectList		*m_pNewObjectList;
	CECBBSPageList	*m_pChPageList;
	CECBBSPageList	*m_pNewChPageList;
	AString			m_aURL;
	AString			m_aNewURL;
	AString			m_aLocalFn;
	AString			m_aNewLocalFn;
	UINT			m_pDownloadThreadId;
	TableList		m_pTableList;
	int				m_iDefaultFontSize;
	int				m_iNewDefaultFontSize;
	int				m_iHisPageNum;
	int				m_iLoadTimes;
	int				m_iScrollHeight;
	int				m_iScrollPos;
	PAUISCROLL		m_pScroll;
	bool			m_bHisPage;
	bool			m_bLastParagraph;
	PA3DViewport	m_pViewport;
	PA3DViewport	m_pNewViewport;
	HANDLE			m_hParseEvent;
	HANDLE			m_hParseThread;
	int				m_iNewPageNumber;
	int				m_iNextPageNumber;
	int				m_iPageNumber;
	AString			m_aNextURL;
	bool			m_bSaveHistory;
	bool			m_bNextSaveHistory;
	bool			m_bNewPage;
	bool			m_bNextNewPage;
	bool			m_bStop;

public:
	CECBBSPage(CECBBSPage *hParentPage);
	~CECBBSPage();

	AString GetFullURL(AString url,bool bNew=false);
	void CalcFontHeight();
	void AddParam(ACHAR *name,ACHAR *value);
	void AddParamFile(ObjectNode *node);
	CECBBSPage* FindPage(AString aName);
	bool PrepareNewPage();
	bool LoadFile(AString file,AString url);
	bool LoadPage();
	bool UseNewPage(bool bStop,bool &bNewHistoryPage);
	bool LoadNewImage();
	ObjectNode *FindObject(PAUIOBJECT pObj);
	void Submit(ObjectNode *pNode);
	void Reset(ObjectNode *pNode);
	void InsertText(HTMLLabel *hLabel);
	void ScrollPage(int iInc);
	CECBBSPage* GetPageFromCursor(int x,int y);
	void SetPos(A3DRECT lRect)
	{
		m_aPostion=lRect;
	}

	bool CreatePageScroll();
	bool CreateHTMLLabel(HTMLLabel *hLabel,A3DRECT &lRect);
	bool GetFont(GlobalInfo *gInfo,HTMLLabel *hLabel);

	bool CreateBreak(A3DRECT &lRect);
	bool CreateCenter(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateDIV(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateFont(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateFrame(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateHR(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateImage(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateImageLine(HTMLLabel *hLabel,int x1,int y1,int x2,int y2);
	bool CreateInput(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateLink(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateParagraph(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreatePre(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateTable(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateText(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateTextArea(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateTD(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateTR(HTMLLabel *hLabel,A3DRECT &lRect);
	bool CreateSelect(HTMLLabel *hLabel,A3DRECT &lRect);

	bool GetTableWidth(HTMLLabel *hLabel);
	
	// load control style from template
	// set pObj == NULL will refresh all objects in current page
	bool LoadTemplateFrame(PAUIOBJECT pObj);
};

extern CECBBSPage *BasePage;
extern PAUIDIALOG BBSDlg;
extern const A3DCOLOR hrefColor;
extern const A3DCOLOR hrefHoverColor;
extern const A3DCOLOR hrefVisitedColor;
extern const A3DCOLOR DefaultFontColor;
extern const A3DCOLOR DisabledFontColor;
extern int DefaultFontSize;
extern const ACHAR DefaultFontName[];
extern const int FontSize[];
extern PAUIOBJECT HoverObject;
extern DownloadedPagesManager DownloadedPages;
extern HistoryPagesManager	HistoryPagesMan;
extern int ControlNumber;


#endif