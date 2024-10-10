// File		: DlgBase.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/12

#include "DlgBBS.h"
#include "EC_GameUIMan.h"
#include "EC_BBSPage.h"
#include "EC_Game.h"
#include "A3DDevice.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBBS, CDlgBase)

AUI_ON_COMMAND("BBSGoBack",			OnCommand_goback)
AUI_ON_COMMAND("BBSGoAhead",		OnCommand_goahead)
AUI_ON_COMMAND("BBSRefresh",		OnCommand_refresh)
AUI_ON_COMMAND("BBSStop",			OnCommand_stop)
AUI_ON_COMMAND("BBSClose",			OnCommand_close)
AUI_ON_COMMAND("BBSHomePage",		OnCommand_homepage)
AUI_ON_COMMAND("BBSFontSize",		OnCommand_fontsize)
AUI_ON_COMMAND("BBSSelectChange",	OnCommand_selectchange)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBBS, CDlgBase)
AUI_ON_EVENT(NULL,	WM_MOUSEWHEEL,	OnEvent_mousewheel)
AUI_ON_EVENT("*",	WM_MOUSEWHEEL,	OnEvent_mousewheel)
AUI_ON_EVENT(NULL,	WM_MOUSEMOVE,	OnEvent_mousemove)
AUI_ON_EVENT("*",	WM_MOUSEMOVE,	OnEvent_mousemove)
AUI_ON_EVENT("*",	WM_LBUTTONUP,	OnEvent_lbuttonup)
AUI_END_EVENT_MAP()

CDlgBBS::CDlgBBS()
{
	m_bShowBBS=false;
//	m_aInitUrl="http://bbs.world2.com.cn/";
//	m_aInitUrl="http://www.vchome.net/";
	m_aInitUrl="http://gamebbs/";
	m_pGoBackButton=NULL;
	m_pGoAheadButton=NULL;
	m_pRefreshButton=NULL;
	m_pStopButton=NULL;
	m_pFontTipLabel=NULL;
	m_pFontSizeCombo=NULL;
	m_bUseNewFontSize=false;
	BasePage=NULL;
	m_bInit=false;
	m_bFirstOpen=false;
	m_bNeedClose=false;
	m_bNeedStop=false;
}

CDlgBBS::~CDlgBBS()
{
}

void CDlgBBS::ChangeSize(int nWidth,int nHeight)
{
	if(!m_bShowBBS)
		return;
	if(m_bInit)
	{
		nWidth-=80;
		if(nWidth>900)
			nWidth=900;
		nHeight-=80;
		if(nHeight>650)
			nHeight=650;
		if(nWidth==m_nWidth&&nHeight==m_nHeight)
			return;
		SetSize(nWidth,nHeight);
		
		// old : SetPos(-1,-1);
		SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

		m_pHomePageButton->SetPos(m_nWidth-105,10);
		m_pCloseButton->SetPos(m_nWidth-60,10);
		A3DRECT aPos;
		aPos.SetRect(5,45,m_nWidth-5,m_nHeight-5);
		BasePage->SetPos(aPos);
		m_bUseNewFontSize=true;
		if(BasePage->m_aNextURL=="")
		{
			BasePage->m_aParm="";
			BasePage->m_aNextURL=BasePage->m_aURL;
			BasePage->m_iNextPageNumber=BasePage->m_iPageNumber;
			BasePage->m_bNextSaveHistory=false;
			BasePage->m_bNextNewPage=false;
		}
	}
}

void CDlgBBS::OnShowDialog()
{
	if(!m_bShowBBS)
		return;
	if(m_bInit)
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
	}
}

bool CDlgBBS::Tick()
{
	CDlgBase::Tick();

	if(!m_bShowBBS)
		return true;
	if(m_bInit&&IsShow())
	{
		if(m_bNeedClose)
		{
			m_bNeedClose=false;
			Show(false);
			return true;
		}

		if(HistoryPagesMan.GetNowPage()>0)
			m_pGoBackButton->Enable(true);
		else
			m_pGoBackButton->Enable(false);
		if(HistoryPagesMan.GetNowPage()<HistoryPagesMan.GetTotalPages()-1)
			m_pGoAheadButton->Enable(true);
		else
			m_pGoAheadButton->Enable(false);
		bool bNewHistoryPage=false;
		BasePage->UseNewPage(m_bNeedStop,bNewHistoryPage);
		m_bNeedStop=false;
		if(bNewHistoryPage)
			HistoryPagesMan.SaveNowPages(BasePage);
		BasePage->ScrollPage(0);
		if(bLoadNewImage)
		{
			bLoadNewImage=false;
			BasePage->LoadNewImage();
		}
	}

	return true;
}

bool CDlgBBS::Release()
{
	if(!m_bShowBBS)
		DeleteControl(m_pFontTipLabel);
	else if(BasePage)
	{
		DeleteControl(m_pGoBackButton);
		DeleteControl(m_pGoAheadButton);
		DeleteControl(m_pRefreshButton);
		DeleteControl(m_pStopButton);
		DeleteControl(m_pHomePageButton);
		DeleteControl(m_pCloseButton);
		DeleteControl(m_pFontTipLabel);
		DeleteControl(m_pFontSizeCombo);
		delete BasePage;
		BasePage=NULL;
		ImgList.Release();
		DownloadedPages.Release();
		HistoryPagesMan.Release();
		m_bUseNewFontSize=false;
		m_bFirstOpen=false;
		m_bInit=false;
		DeleteCriticalSection(&ImageSection);
	}

	return AUIDialog::Release();
}

bool CDlgBBS::OnInitDialog()
{
	if(!m_bShowBBS)
	{
		m_pFontTipLabel=(PAUILABEL)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_LABEL);
		m_pFontTipLabel->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
		m_pFontTipLabel->SetText(TEXT("该功能未开放"));
		m_pFontTipLabel->SetPos(20,10);
		m_pFontTipLabel->SetSize(100,20);
		return true;
	}
	
	InitializeCriticalSection(&ImageSection);
	DownloadedPages.Init();
	HistoryPagesMan.Init();
	ImgList.Init();
	m_bUseNewFontSize=false;
	m_bNeedStop=false;
	DefaultFontSize=1;
	ControlNumber=0;
	EndThreadId=0;
	HoverObject=NULL;
	BBSDlg=this;

	// old : SetPos(-1,-1);
	SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);


	AUIOBJECT_SETPROPERTY pp;
	strcpy(pp.fn,"InBBS/对话框_标准边框.bmp");
	SetProperty("Frame Image",&pp);
	BasePage=new CECBBSPage(NULL);
	BasePage->CalcFontHeight();
	A3DRECT aPos;
	aPos.SetRect(5,45,m_nWidth-5,m_nHeight-5);
	BasePage->SetPos(aPos);

	AUIOBJECT_SETPROPERTY p;
	
	m_pGoBackButton=(PAUISTILLIMAGEBUTTON)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pGoBackButton->SetPos(20,10);
	m_pGoBackButton->SetSize(40,24);
	m_pGoBackButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pGoBackButton->SetText(TEXT("后退"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pGoBackButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pGoBackButton->SetProperty("Down Frame File",&p);
	m_pGoBackButton->SetName("HTMLPageGoBack");
	m_pGoBackButton->SetCommand("BBSGoBack");

	m_pGoAheadButton=(PAUISTILLIMAGEBUTTON)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pGoAheadButton->SetPos(65,10);
	m_pGoAheadButton->SetSize(40,24);
	m_pGoAheadButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pGoAheadButton->SetText(TEXT("前进"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pGoAheadButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pGoAheadButton->SetProperty("Down Frame File",&p);
	m_pGoAheadButton->SetName("HTMLPageGoAhead");
	m_pGoAheadButton->SetCommand("BBSGoAhead");

	m_pRefreshButton=(PAUISTILLIMAGEBUTTON)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pRefreshButton->SetPos(110,10);
	m_pRefreshButton->SetSize(40,24);
	m_pRefreshButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pRefreshButton->SetText(TEXT("刷新"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pRefreshButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pRefreshButton->SetProperty("Down Frame File",&p);
	m_pRefreshButton->SetName("HTMLPageRefresh");
	m_pRefreshButton->SetCommand("BBSRefresh");

	m_pStopButton=(PAUISTILLIMAGEBUTTON)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pStopButton->SetPos(155,10);
	m_pStopButton->SetSize(40,24);
	m_pStopButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pStopButton->SetText(TEXT("停止"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pStopButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pStopButton->SetProperty("Down Frame File",&p);
	m_pStopButton->SetName("HTMLPageStop");
	m_pStopButton->SetCommand("BBSStop");

	m_pHomePageButton=(PAUISTILLIMAGEBUTTON)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pHomePageButton->SetPos(m_nWidth-105,10);
	m_pHomePageButton->SetSize(40,24);
	m_pHomePageButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pHomePageButton->SetText(TEXT("首页"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pHomePageButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pHomePageButton->SetProperty("Down Frame File",&p);
	m_pHomePageButton->SetName("HTMLPageHomePage");
	m_pHomePageButton->SetCommand("BBSHomePage");

	m_pCloseButton=(PAUISTILLIMAGEBUTTON)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pCloseButton->SetPos(m_nWidth-60,10);
	m_pCloseButton->SetSize(40,24);
	m_pCloseButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pCloseButton->SetText(TEXT("关闭"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pCloseButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pCloseButton->SetProperty("Down Frame File",&p);
	m_pCloseButton->SetName("HTMLPageClose");
	m_pCloseButton->SetCommand("BBSClose");

	m_pFontTipLabel=(PAUILABEL)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_LABEL);
	m_pFontTipLabel->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pFontTipLabel->SetText(TEXT("文字大小"));
	m_pFontTipLabel->SetPos(215,16);
	m_pFontTipLabel->SetSize(60,20);

	m_pFontSizeCombo=(PAUICOMBOBOX)CreateControl(GetGame()->GetA3DEngine(),GetGame()->GetA3DDevice(),AUIOBJECT_TYPE_COMBOBOX);
	strcpy(p.fn,"InBBS/ComboFrame.bmp");
	m_pFontSizeCombo->SetProperty("Frame Image",&p);
	strcpy(p.fn,"InBBS/ComboHilight.bmp");
	m_pFontSizeCombo->SetProperty("Hilight Image",&p);
	m_pFontSizeCombo->SetPos(265,10);
	m_pFontSizeCombo->SetSize(36,24);
	m_pFontSizeCombo->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pFontSizeCombo->AddString(TEXT("小"));
	m_pFontSizeCombo->AddString(TEXT("中"));
	m_pFontSizeCombo->AddString(TEXT("大"));
	m_pFontSizeCombo->SetCurSel(1);
	m_pFontSizeCombo->SetName("FontSize");
	m_pFontSizeCombo->SetCommand("BBSFontSize");

	m_bInit=true;
	m_bFirstOpen=true;

	return true;
}

void CDlgBBS::OnCommand_goback(const char * szCommand)
{
	int nowpage=HistoryPagesMan.GetNowPage();
	if(nowpage>0)
		HistoryPagesMan.SetNowPage(nowpage-1);
}

void CDlgBBS::OnCommand_goahead(const char * szCommand)
{
	int nowpage=HistoryPagesMan.GetNowPage();
	int totalpage=HistoryPagesMan.GetTotalPages();
	if(nowpage<totalpage-1)
		HistoryPagesMan.SetNowPage(nowpage+1);
}

void CDlgBBS::OnCommand_refresh(const char * szCommand)
{
	BasePage->m_aParm="";
	BasePage->m_aNextURL=BasePage->m_aURL;
	BasePage->m_iNextPageNumber=-1;
	BasePage->m_bNextSaveHistory=false;
	BasePage->m_bNextNewPage=true;
}

void CDlgBBS::OnCommand_stop(const char * szCommand)
{
	m_bNeedStop=true;
}

void CDlgBBS::OnCommand_homepage(const char * szCommand)
{
	BasePage->m_aParm="";
	BasePage->m_aNextURL=m_aInitUrl;
	BasePage->m_iNextPageNumber=-1;
	BasePage->m_bNextSaveHistory=true;
	BasePage->m_bNextNewPage=true;
}

void CDlgBBS::OnCommand_close(const char * szCommand)
{
	m_bNeedClose=true;
}

void CDlgBBS::OnCommand_fontsize(const char * szCommand)
{
	int p=m_pFontSizeCombo->GetCurSel();
	if(p!=DefaultFontSize)
	{
		DefaultFontSize=p;
		m_bUseNewFontSize=true;
		BasePage->m_aParm="";
		BasePage->m_aNextURL=BasePage->m_aURL;
		BasePage->m_iNextPageNumber=BasePage->m_iPageNumber;
		BasePage->m_bNextSaveHistory=false;
		BasePage->m_bNextNewPage=false;
	}
}

void CDlgBBS::OnEvent_mousewheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int zDelta = (short)HIWORD(wParam);
	POINT ptClient = { 0, 0 };
	ClientToScreen(GetGame()->GetA3DDevice()->GetDeviceWnd(), &ptClient);
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

void CDlgBBS::OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
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

void CDlgBBS::OnEvent_lbuttonup(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
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

void CDlgBBS::OnCommand_selectchange(const char * szCommand)
{
	PAUIOBJECT pLastObj = GetLastCommandGenerator();
	if(pLastObj==NULL)
		return;
	ObjectNode *ObjNode=(ObjectNode*)pLastObj->GetDataPtr();
	if(ObjNode!=NULL)
	{
		int sel=((PAUICOMBOBOX)pLastObj)->GetCurSel();
		HTMLLabel *chLabel=ObjNode->label->FirstChildLabel;
		while(chLabel!=NULL&&sel>=0)
		{
			if(chLabel->LabelType=="option"&&chLabel->NextLabel!=NULL&&
				chLabel->NextLabel->LabelType=="text")
			{
				if(sel==0)
					if(ObjNode->page!=NULL&&chLabel->Attrib->GetAttribute("href")!=NULL)
					{
						AString a=*chLabel->Attrib->GetAttribute("href");
						ObjNode->page->m_aNextURL=ObjNode->page->GetFullURL(a);
						ObjNode->page->m_aParm="";
						ObjNode->page->m_iNextPageNumber=-1;
						ObjNode->page->m_bNextSaveHistory=true;
						ObjNode->page->m_bNextNewPage=true;
						break;
					}
					else if(chLabel->Attrib->GetAttribute("inserttext")!=NULL)
					{
						BasePage->InsertText(chLabel);
						break;
					}
				chLabel=chLabel->NextLabel;
				sel--;
			}
			chLabel=chLabel->NextLabel;
		}
	}
}
