// File		: DlgLoginPage.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/23

#include "DlgLoginPage.h"
#include "EC_LoginUIMan.h"
#include "EC_BBSPage.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "A3DDevice.h"
#include <WindowsX.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgLoginPage, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Cancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgLoginPage, CDlgBase)
AUI_ON_EVENT(NULL,	WM_MOUSEWHEEL,	OnEvent_mousewheel)
AUI_ON_EVENT("*",	WM_MOUSEWHEEL,	OnEvent_mousewheel)
AUI_ON_EVENT(NULL,	WM_MOUSEMOVE,	OnEvent_mousemove)
AUI_ON_EVENT("*",	WM_MOUSEMOVE,	OnEvent_mousemove)
AUI_ON_EVENT("*",	WM_LBUTTONUP,	OnEvent_lbuttonup)
AUI_END_EVENT_MAP()

CDlgLoginPage::CDlgLoginPage()
{
	m_aInitUrl="info/index.html";
	BasePage=NULL;
	m_bFirstOpen=false;
}

CDlgLoginPage::~CDlgLoginPage()
{
}

void CDlgLoginPage::OnShowDialog()
{
	if(m_bFirstOpen)
	{
		m_bFirstOpen=false;
		BasePage->m_aNextURL=m_aInitUrl;
		BasePage->m_iNextPageNumber=-1;
		BasePage->m_bNextSaveHistory=true;
		BasePage->m_bNextNewPage=true;
		SetCanOutOfWindow(true);
	}
	CDlgBase::OnShowDialog();
}

void CDlgLoginPage::OnTick()
{
	if(IsShow())
	{
		bool bNewHistoryPage=false;
		BasePage->UseNewPage(false,bNewHistoryPage);
		if(bNewHistoryPage)
			HistoryPagesMan.SaveNowPages(BasePage);
		BasePage->ScrollPage(0);
		if(bLoadNewImage)
		{
			bLoadNewImage=false;
			BasePage->LoadNewImage();
		}
		if( !m_bLoaded && BasePage->m_aNextURL == "" )
		{
			AString strText = GetBaseUIMan()->GetURL("URL", "LoginPage");
			if( strText.GetLength() > 0 )
			{
				BasePage->m_aParm="";
				BasePage->m_aNextURL=strText;
				BasePage->m_iNextPageNumber=-1;
				BasePage->m_bNextSaveHistory=true;
				BasePage->m_bNextNewPage=true;
			}
			m_bLoaded=true;
		}
	}
}

bool CDlgLoginPage::Release()
{
	if(BasePage)
	{
		delete BasePage;
		BasePage=NULL;
		ImgList.Release();
		DownloadedPages.Release();
		HistoryPagesMan.Release();
		m_bUseNewFontSize=false;
		m_bFirstOpen=false;
		DeleteCriticalSection(&ImageSection);
	}

	return CDlgBase::Release();
}

bool CDlgLoginPage::OnInitDialog()
{
	InitializeCriticalSection(&ImageSection);
	DownloadedPages.Init();
	HistoryPagesMan.Init();
	ImgList.Init();
	m_bUseNewFontSize=false;
	DefaultFontSize=1;
	ControlNumber=0;
	EndThreadId=0;
	HoverObject=NULL;
	BBSDlg=this;

	// old : SetPos(-1,-1);
	SetPosEx(0, 0, CDlgBase::alignCenter, CDlgBase::alignCenter);


	BasePage=new CECBBSPage(NULL);
	BasePage->CalcFontHeight();
	SetBBSPagePos();

	m_bFirstOpen=true;
	m_bLoaded=false;

	const char * szLogPath = "BBSPages";
	CreateDirectoryA(szLogPath, NULL);
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA("BBSPages\\*.*", &fd);
	if( INVALID_HANDLE_VALUE == hFind )
		return true;
	
	do
	{
		char szFileName[MAX_PATH];
		
		sprintf(szFileName, "BBSPages\\%s", fd.cFileName);
		strlwr(szFileName);
		
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// ignore folders
			continue;
		}
		else
			DeleteFileA(szFileName);
	} while( FindNextFileA(hFind, &fd) );
	return true;
}

void CDlgLoginPage::OnCommand_Cancel(const char* szCommand)
{
	PAUIDIALOG pDlgSelect = GetAUIManager()->GetDialog("Win_Select");
	pDlgSelect->OnCommand("IDCANCEL");
}

void CDlgLoginPage::OnEvent_mousewheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int zDelta = (short)HIWORD(wParam);
	POINT ptClient = { 0, 0 };
	ClientToScreen(g_pGame->GetA3DDevice()->GetDeviceWnd(), &ptClient);
	x -= ptClient.x;
	y -= ptClient.y;
	x -= GetPos().x;
	y -= GetPos().y;
	CECBBSPage *page=BasePage->GetPageFromCursor(x,y);
	BBSDlg->ChangeFocus(NULL);
	if(page!=NULL&&page->m_pScroll!=NULL)
		page->m_pScroll->SetBarLevel(page->m_pScroll->GetBarLevel()-zDelta/2);
	if(HoverObject!=NULL)
	{
		ObjectNode *ObjNode=(ObjectNode*)HoverObject->GetDataPtr();
		HoverObject=NULL;
		if(ObjNode!=NULL)
		{
			HTMLLabel *hLabel=ObjNode->label;
			HTMLLabel *pLabel=hLabel->ParentLabel;
			while(pLabel!=NULL&&pLabel->LabelType!="a")
				pLabel=pLabel->ParentLabel;
			if(	pLabel!=NULL && ObjNode->page->m_pObjectList)
				ObjNode->page->m_pObjectList->SetLabelColor(pLabel,hrefColor);
		}
	}
}

void CDlgLoginPage::OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if(HoverObject!=pObj)
	{
		if(HoverObject!=NULL)
		{
			ObjectNode *ObjNode=(ObjectNode*)HoverObject->GetDataPtr();
			HoverObject=NULL;
			if(ObjNode!=NULL)
			{
				HTMLLabel *hLabel=ObjNode->label;
				HTMLLabel *pLabel=hLabel->ParentLabel;
				while(pLabel!=NULL&&pLabel->LabelType!="a")
					pLabel=pLabel->ParentLabel;
				if(	pLabel!=NULL && ObjNode->page->m_pObjectList)
					ObjNode->page->m_pObjectList->SetLabelColor(pLabel,hrefColor);
			}
		}
		if(pObj!=NULL)
		{
			ObjectNode *ObjNode=(ObjectNode*)pObj->GetDataPtr();
			if(ObjNode!=NULL)
			{
				HTMLLabel *hLabel=ObjNode->label;
				HTMLLabel *pLabel=hLabel->ParentLabel;
				while(pLabel!=NULL&&pLabel->LabelType!="a")
					pLabel=pLabel->ParentLabel;
				if(	hLabel->LabelType=="text"&&pLabel!=NULL)
				{
					ObjNode->page->m_pObjectList->SetLabelColor(pLabel,hrefHoverColor);
					HoverObject=pObj;
				}
			}
		}
	}
}

void CDlgLoginPage::OnEvent_lbuttonup(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ObjectNode *ObjNode=(ObjectNode*)pObj->GetDataPtr();
	if(ObjNode!=NULL)
	{
		HTMLLabel *hLabel=ObjNode->label;
		if(hLabel->Attrib->GetAttribute("inserttext")!=NULL)
			BasePage->InsertText(hLabel);
		if(hLabel->LabelType=="input"&&ObjNode->obj->GetType()==AUIOBJECT_TYPE_STILLIMAGEBUTTON)
		{
			AString *a=hLabel->Attrib->GetAttribute("type");
			if(a!=NULL)
				if(*a=="submit")
					ObjNode->page->Submit(ObjNode);
				else if(*a=="reset")
					ObjNode->page->Reset(ObjNode);
		}
		else
		{
			HTMLLabel *pLabel=hLabel->ParentLabel;
			while(pLabel!=NULL&&pLabel->LabelType!="a")
				pLabel=pLabel->ParentLabel;
			if(pLabel!=NULL&&(hLabel->LabelType=="text"||hLabel->LabelType=="img"))
			{
				AString *a=pLabel->Attrib->GetAttribute("target");
				CECBBSPage *page;
				if(a==NULL)
					page=ObjNode->page;
				else
					page=BasePage->FindPage(*a);
				if(page==NULL)
					page=ObjNode->page;
				if(page!=NULL)
				{
					a=pLabel->Attrib->GetAttribute("ahref");
					if(a!=NULL)
						GetBaseUIMan()->NavigateURL(page->GetFullURL(*a));
					else
					{
						a=pLabel->Attrib->GetAttribute("href");
						if(a!=NULL)
						{
							page->m_aParm="";
							page->m_aNextURL=page->GetFullURL(*a);
							page->m_iNextPageNumber=-1;
							page->m_bNextSaveHistory=true;
							page->m_bNextNewPage=true;
						}
					}
				}
			}
		}
	}
}

void CDlgLoginPage::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	this->AlignTo(NULL, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_CENTER);
	SetBBSPagePos();
}

void CDlgLoginPage::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		BasePage->LoadTemplateFrame(NULL);
		SetBBSPagePos();
	}
}

void CDlgLoginPage::SetBBSPagePos()
{
	PAUIOBJECT pObjShowPos = GetDlgItem("ShowPos");
	POINT pt = pObjShowPos->GetDefaultPos();
	SIZE size = pObjShowPos->GetDefaultSize();
	int left = pt.x;
	int top = pt.y;
	int right = left + size.cx;
	int bottom = top + size.cy;	
	float fWindowScale = GetAUIManager()->GetWindowScale();
	left *= fWindowScale;
	top *= fWindowScale;
	right *= fWindowScale;
	bottom *= fWindowScale;
	BasePage->SetPos(A3DRECT(left, top, right, bottom));
}