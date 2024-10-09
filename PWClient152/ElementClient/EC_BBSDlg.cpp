/*
 * FILE: EC_BBSDlg.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_BBSDlg.h"
#include "A3DDevice.h"
#include "EC_Game.h"
#include "EC_BBSFileDownloader.h"


const bool bUseBBS=false;

CECBBSDlg::CECBBSDlg()
{
	if(!bUseBBS)
		return;
	m_pAUIManager=NULL;
	m_pGoBackButton=NULL;
	m_pGoAheadButton=NULL;
	m_pRefreshButton=NULL;
	m_pFontTipLabel=NULL;
	m_pFontSizeCombo=NULL;
	m_bUseNewFontSize=false;
	m_pLastObj=NULL;
	m_pHoverObj=NULL;
	BasePage=NULL;
	m_bInit=false;
	m_bNeedClose=false;
	for(int i=0;i<1000;i++)
		HistoryPages[i].parm=NULL;
}

CECBBSDlg::~CECBBSDlg()
{
}

bool CECBBSDlg::Release()
{
	if(!bUseBBS)
		return true;
	if(BasePage)
	{
		BBSDlg->DeleteControl(m_pGoBackButton);
		BBSDlg->DeleteControl(m_pGoAheadButton);
		BBSDlg->DeleteControl(m_pRefreshButton);
		BBSDlg->DeleteControl(m_pHomePageButton);
		BBSDlg->DeleteControl(m_pCloseButton);
		BBSDlg->DeleteControl(m_pFontTipLabel);
		BBSDlg->DeleteControl(m_pFontSizeCombo);
		delete BasePage;
		BasePage=NULL;
		for(int i=0;i<1000;i++)
			if(HistoryPages[i].parm!=NULL)
			{
				delete []HistoryPages[i].parm;
				HistoryPages[i].parm=NULL;
			}
		ImgList.Release();
	}

	return true;
}

void CECBBSDlg::Show(bool bShow)
{
	if(!bUseBBS)
		return;
	if(m_bInit)
	{
		if(BBSDlg->IsShow())
			BBSDlg->Show(false);
		else
		{
			if(m_bFirstOpen)
			{
				m_bFirstOpen=false;
				desHisPage=-9999;
				totHisPages=0;
				nowHisPage=0;
				if(m_aInitUrl=="")
					BasePage->GetByURL("http://gamebbs/");
				else
					BasePage->GetByURL(m_aInitUrl);
				m_pAUIManager->BringWindowToTop(BBSDlg);
				BBSDlg->SetCanOutOfWindow(true);
			}
			BBSDlg->Show(true);
		}
	}
}

bool CECBBSDlg::SetSize(int nWidth,int nHeight)
{
	if(!bUseBBS)
		return true;
	if(m_bInit)
	{
		m_nWidth=nWidth-80;
		if(m_nWidth>900)
			m_nWidth=900;
		m_nHeight=nHeight-80;
		if(m_nHeight>650)
			m_nHeight=650;
		BBSDlg->SetSize(m_nWidth,m_nHeight);
		BBSDlg->SetPos(-1,-1);
		m_pHomePageButton->SetPos(m_nWidth-120,10);
		m_pCloseButton->SetPos(m_nWidth-60,10);
		A3DRECT aPos;
		aPos.SetRect(5,45,m_nWidth-5,m_nHeight-5);
		BasePage->SetPos(aPos);
		BasePage->UseNewFontSize();
	}
	return true;
}

bool CECBBSDlg::InitBBSDlg(PAUIMANAGER pAUIManager,AString initurl,int nWidth,int nHeight)
{
	if(!bUseBBS)
		return true;
	
	m_bUseNewFontSize=false;
	DefaultFontSize=1;
	m_pLastObj=NULL;
	m_pHoverObj=NULL;
	EndThreadId=0;
	m_aInitUrl=initurl;
	m_nWidth=nWidth-80;
	if(m_nWidth>900)
		m_nWidth=900;
	m_nHeight=nHeight-80;
	if(m_nHeight>650)
		m_nHeight=650;

	PageNum=0;
	m_pAUIManager=pAUIManager;
	int ret=m_pAUIManager->CreateEmptyDlg(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice());
	BBSDlg=m_pAUIManager->GetDialog(ret);

	BBSDlg->Show(false);
	BBSDlg->SetName("Win_BBSDlg");
	BBSDlg->SetSize(m_nWidth,m_nHeight);
	BBSDlg->SetPos(-1,-1);
	AUIOBJECT_SETPROPERTY pp;
	strcpy(pp.fn,"InBBS/对话框_标准边框.bmp");
	BBSDlg->SetProperty("Frame Image",&pp);
	BasePage=new CECBBSPage(NULL);
	BasePage->CalcFontHeight();
	A3DRECT aPos;
	aPos.SetRect(5,45,m_nWidth-5,m_nHeight-5);
	BasePage->SetPos(aPos);

	AUIOBJECT_SETPROPERTY p;
	
	m_pGoBackButton=(PAUISTILLIMAGEBUTTON)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pGoBackButton->SetPos(20,10);
	m_pGoBackButton->SetSize(40,24);
	m_pGoBackButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pGoBackButton->SetText(TEXT("后退"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pGoBackButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pGoBackButton->SetProperty("Down Frame File",&p);
	m_pGoBackButton->SetName("HTMLPageGoBack");

	m_pGoAheadButton=(PAUISTILLIMAGEBUTTON)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pGoAheadButton->SetPos(65,10);
	m_pGoAheadButton->SetSize(40,24);
	m_pGoAheadButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pGoAheadButton->SetText(TEXT("前进"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pGoAheadButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pGoAheadButton->SetProperty("Down Frame File",&p);
	m_pGoAheadButton->SetName("HTMLPageGoAhead");

	m_pRefreshButton=(PAUISTILLIMAGEBUTTON)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pRefreshButton->SetPos(110,10);
	m_pRefreshButton->SetSize(40,24);
	m_pRefreshButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pRefreshButton->SetText(TEXT("刷新"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pRefreshButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pRefreshButton->SetProperty("Down Frame File",&p);
	m_pRefreshButton->SetName("HTMLPageRefresh");

	m_pHomePageButton=(PAUISTILLIMAGEBUTTON)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pHomePageButton->SetPos(m_nWidth-120,10);
	m_pHomePageButton->SetSize(54,24);
	m_pHomePageButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pHomePageButton->SetText(TEXT("首页"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pHomePageButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pHomePageButton->SetProperty("Down Frame File",&p);
	m_pHomePageButton->SetName("HTMLPageHomePage");

	m_pCloseButton=(PAUISTILLIMAGEBUTTON)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_STILLIMAGEBUTTON);
	m_pCloseButton->SetPos(m_nWidth-60,10);
	m_pCloseButton->SetSize(40,24);
	m_pCloseButton->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pCloseButton->SetText(TEXT("关闭"));
	strcpy(p.fn,"InBBS/按钮_文本1.bmp");
	m_pCloseButton->SetProperty("Up Frame File",&p);
	strcpy(p.fn,"InBBS/按钮_文本2.bmp");
	m_pCloseButton->SetProperty("Down Frame File",&p);
	m_pCloseButton->SetName("HTMLPageClose");

	m_pFontTipLabel=(PAUILABEL)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_LABEL);
	m_pFontTipLabel->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pFontTipLabel->SetText(TEXT("文字大小"));
	m_pFontTipLabel->SetPos(170,16);
	m_pFontTipLabel->SetSize(60,20);

	m_pFontSizeCombo=(PAUICOMBOBOX)BBSDlg->CreateControl(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),AUIOBJECT_TYPE_COMBOBOX);
	strcpy(p.fn,"InBBS/ComboFrame.bmp");
	m_pFontSizeCombo->SetProperty("Frame Image",&p);
	strcpy(p.fn,"InBBS/ComboHilight.bmp");
	m_pFontSizeCombo->SetProperty("Hilight Image",&p);
	m_pFontSizeCombo->SetPos(220,10);
	m_pFontSizeCombo->SetSize(36,24);
	m_pFontSizeCombo->SetFontAndColor(DefaultFontName,FontSize[DefaultFontSize],DefaultFontColor);
	m_pFontSizeCombo->AddString(TEXT("小"));
	m_pFontSizeCombo->AddString(TEXT("中"));
	m_pFontSizeCombo->AddString(TEXT("大"));
	m_pFontSizeCombo->SetCurSel(1);
	m_pFontSizeCombo->SetName("FontSize");

	m_bInit=true;
	m_bFirstOpen=true;
	return true;
}

bool CECBBSDlg::Render()
{
	return true;
}

bool CECBBSDlg::Tick()
{
	if(!bUseBBS)
		return true;
	if(m_bInit&&BBSDlg->IsShow())
	{
		if(m_bNeedClose)
		{
			m_bNeedClose=false;
			BBSDlg->Show(false);
			return true;
		}

		if(m_bUseNewFontSize)
		{
			BasePage->UseNewFontSize();
			m_bUseNewFontSize=false;
		}
		BasePage->UseNewPage();
		int k=0;
		while(!HistoryPages[k].bNewPage&&k<totHisPages)
			k++;
		if(nowHisPage<=k)
			m_pGoBackButton->Enable(false);
		else
			m_pGoBackButton->Enable(true);
		if(nowHisPage>=totHisPages)
			m_pGoAheadButton->Enable(false);
		else
			m_pGoAheadButton->Enable(true);
		BasePage->MovePageObject(0);
		if(bLoadNewImage)
		{
			bLoadNewImage=false;
			BasePage->LoadNewImage();
		}
	}
	return true;
}

bool CECBBSDlg::OnCommand(const char *pszCommand, PAUIDIALOG pDlg)
{
	if(!bUseBBS)
		return true;
	if(m_bInit&&BBSDlg->IsShow())
	{
		if(strcmp(pszCommand,"SelectChange")==0&&m_pLastObj!=NULL&&m_pLastObj->GetType()==AUIOBJECT_TYPE_COMBOBOX)
		{
			ObjectNode *ObjNode=BasePage->FindObject(m_pLastObj);
			if(ObjNode!=NULL)
			{
				int sel=((PAUICOMBOBOX)m_pLastObj)->GetCurSel();
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
								desHisPage=-9999;
								HistoryPages[nowHisPage].bNewPage=true;
								totHisPages=nowHisPage;
								ObjNode->page->GetByURL(a);
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
	}
	return true;
}

bool CECBBSDlg::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj)
{
	if(!bUseBBS)
		return true;
	if(!(m_bInit&&BBSDlg->IsShow()))
		return true;
	m_pLastObj=pObj;
	if(uMsg==WM_MOUSEWHEEL&&pDlg==BBSDlg)
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int zDelta = (short)HIWORD(wParam);
		POINT ptClient = { 0, 0 };
		ClientToScreen(g_pGame->GetA3DDevice()->GetDeviceWnd(), &ptClient);
		x -= ptClient.x;
		y -= ptClient.y;
		x -= BBSDlg->GetPos().x;
		y -= BBSDlg->GetPos().y;
		CECBBSPage *page=BasePage->GetPageFromCursor(x,y);
		BBSDlg->ChangeFocus(NULL);
		if(page!=NULL&&page->m_pScroll!=NULL)
			page->m_pScroll->SetBarLevel(page->m_pScroll->GetBarLevel()-zDelta/2);
		if(m_pHoverObj!=NULL)
		{
			ObjectNode *ObjNode=BasePage->FindObject(m_pHoverObj);
			m_pHoverObj=NULL;
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
	if(uMsg==WM_MOUSEMOVE)
	{
		if(m_pHoverObj!=pObj)
		{
			if(m_pHoverObj!=NULL)
			{
				ObjectNode *ObjNode=BasePage->FindObject(m_pHoverObj);
				m_pHoverObj=NULL;
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
				ObjectNode *ObjNode=BasePage->FindObject(pObj);
				if(ObjNode!=NULL)
				{
					HTMLLabel *hLabel=ObjNode->label;
					HTMLLabel *pLabel=hLabel->ParentLabel;
					while(pLabel!=NULL&&pLabel->LabelType!="a")
						pLabel=pLabel->ParentLabel;
					if(	//hLabel->ParentLabel->FirstChildLabel==hLabel&&
						hLabel->LabelType=="text"&&pLabel!=NULL)
					{
						ObjNode->page->m_pObjectList->SetLabelColor(pLabel,hrefHoverColor);
						m_pHoverObj=pObj;
					}
				}
			}
		}
	}
	if(uMsg==WM_LBUTTONUP)
	{
		if(pObj!=NULL&&strcmp(pObj->GetName(),"FontSize")==0)
		{
			int p=m_pFontSizeCombo->GetCurSel();
			if(p!=DefaultFontSize)
			{
				DefaultFontSize=p;
				m_bUseNewFontSize=true;
			}
		}
		ObjectNode *ObjNode=BasePage->FindObject(pObj);
		if(ObjNode!=NULL)
		{
			HTMLLabel *hLabel=ObjNode->label;
			if(hLabel->Attrib->GetAttribute("inserttext")!=NULL)
				BasePage->InsertText(hLabel);
		}
	}
	if(uMsg==WM_LBUTTONUP)
	{
		if(pObj!=NULL&&strcmp(pObj->GetName(),"HTMLPageHomePage")==0)
		{
			desHisPage=-9999;
			HistoryPages[nowHisPage].bNewPage=true;
			totHisPages=nowHisPage;
			BasePage->GetByURL(m_aInitUrl);
		}
		if(pObj!=NULL&&strcmp(pObj->GetName(),"HTMLPageClose")==0)
			m_bNeedClose=true;
		if(pObj!=NULL&&strcmp(pObj->GetName(),"HTMLPageGoBack")==0)
		{
			CECBBSPage *page;
			int k;
			k=nowHisPage;
			int q=nowHisPage;
			int p=k-1;
			while(p>=0)
			{
				if(HistoryPages[p].target==HistoryPages[k].target)
				{
					page=BasePage->FindPage(HistoryPages[k].target);
					desHisPage=k-1;
					if(!page->GetByURL(HistoryPages[p].localfn))
						desHisPage=q;
					break;
				}
				if(	HistoryPages[k].parentsTarget!=""&&
					HistoryPages[p].target==HistoryPages[k].parentsTarget)
					k=p;
				p--;
			}
		}
		if(pObj!=NULL&&strcmp(pObj->GetName(),"HTMLPageGoAhead")==0)
		{
			CECBBSPage *page;
			desHisPage=nowHisPage+1;
			while(!HistoryPages[desHisPage].bNewPage&&desHisPage<totHisPages)
				desHisPage++;
			page=BasePage->FindPage(HistoryPages[nowHisPage+1].target);
			if(page!=NULL)
				page->GetByURL(HistoryPages[nowHisPage+1].localfn);
		}
		if(pObj!=NULL&&strcmp(pObj->GetName(),"HTMLPageRefresh")==0)
		{
			CECBBSPage *page;
			int p=nowHisPage;
			while(p>1&&HistoryPages[p-1].target==HistoryPages[p].parentsTarget)
				p--;
			desHisPage=-nowHisPage;
			page=BasePage->FindPage(HistoryPages[p].target);
			if(page!=NULL)
				page->GetByURL(HistoryPages[p].url);
		}
		ObjectNode *ObjNode=BasePage->FindObject(pObj);
		if(ObjNode!=NULL)
		{
			HTMLLabel *hLabel=ObjNode->label;
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
							desHisPage=-9999;
							HistoryPages[nowHisPage].bNewPage=true;
							totHisPages=nowHisPage;
							page->GetByURL(*a);
							m_pHoverObj=NULL;
						}
					}
				}
			}
		}
	}
	return true;
}
