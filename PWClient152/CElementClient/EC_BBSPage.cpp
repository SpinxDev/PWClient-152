/*
 * FILE: EC_BBSPage.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_BBSPage.h"
#include "EC_BBSFileDownloader.h"
#include "Shlwapi.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include <process.h>
#include "FTInterface.h"
#include "EC_Utility.h"
#include "DlgTheme.h"

#include "AIniFile.h"

#define new A_DEBUG_NEW

//#define _USESTYLE

CECBBSPage *BasePage;
PAUIDIALOG BBSDlg=NULL;
const A3DCOLOR hrefColor=0xFF00FFFF;
const A3DCOLOR hrefHoverColor=0xFF0000FF;
const A3DCOLOR hrefVisitedColor=0xFFFF00FF;
const A3DCOLOR DefaultFontColor=0xFFFFFFFF;
const A3DCOLOR DisabledFontColor=0xFF606050;
const ACHAR DefaultFontName[]=TEXT("方正细黑一简体");
const int MaxFontNum=7;
const int FontSize[MaxFontNum]={9,10,11,12,14,16,20};
A3DFTFont *PointFont[MaxFontNum];
int RealFontHeight[MaxFontNum];
int DefaultFontSize=1;
AString	errorhtml="bbsfiles/error.htm";
AString	errorimg="InBBS/error.bmp";
PAUIOBJECT HoverObject;
DownloadedPagesManager DownloadedPages;
HistoryPagesManager	HistoryPagesMan;
int ControlNumber;

#define DELETEPOINTER(p)\
	if(p!=NULL)\
	{\
		delete p;\
		p=NULL;\
	}

void writelog(AString log)
{
	FILE *f;
	do
	{
		f=fopen("Logs\\bbs.log","a");
	}while(!f);
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	char s[400];
	sprintf(s,"%d/%d/%d %02d:%02d:%02d ",
			sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
	fputs(s,f);
	fputs(log,f);
	fputs("\r\n",f);
	fclose(f);
}

A3DFTFont *GetPointFont(const ACHAR *FontName,int FontSize)
{
	A3DFTFont *p=BBSDlg->GetAUIManager()->GetA3DFTFontMan()->CreatePointFont(
		FontSize, FontName, STYLE_NORMAL, 0);
	if(p==NULL)
	{
		p = BBSDlg->GetAUIManager()->GetA3DFTFontMan()->CreatePointFont(
			FontSize, DefaultFontName, STYLE_NORMAL, 0);
		if( !p )
			p = BBSDlg->GetAUIManager()->GetDefaultFont();
	}
	return p;
}

bool CreateFormatText(ACHAR *uCode,bool bPre)
{
	int i=0;
	int j=0;
	bool b=true;
	bool c=false;
	while(uCode[i]!='\0')
	{
		ACHAR a;
		a=uCode[i];
		if(a=='&')
		{
			i++;
			int k=i;
			if(uCode[i]=='#')
			{
				while(uCode[i]!='\0'&&uCode[i]!=';')
					i++;
				if(uCode[i]!='\0')
				{
					ACHAR st[10];
					a_strncpy(st,uCode+k+1,i-k-1);
					st[i-k-1]='\0';
					a=(ACHAR)StrToInt(st);
				}
				else
					i=k-1;
			}
			else
			{
				while(uCode[i]!='\0'&&uCode[i]!=';')
					i++;
				if(uCode[i]!='\0')
				{
					ACHAR st[10];
					a_strncpy(st,uCode+k,i-k);
					st[i-k]='\0';
					if(a_strcmp(st,TEXT("nbsp"))==0)
					{
						a=' ';
						c=true;
					}
					else if(a_strcmp(st,TEXT("lt"))==0)
						a='<';
					else if(a_strcmp(st,TEXT("gt"))==0)
						a='>';
					else if(a_strcmp(st,TEXT("amp"))==0)
						a='&';
					else if(a_strcmp(st,TEXT("quot"))==0)
						a='"';
					else
						a=' ';
				}
				else
					i=k-1;
			}
			if(i==k-1)
				a=uCode[i];
			b=false;
			uCode[j++]=a;
		}
		else
		{
			if(!bPre&&(a=='\r'||a=='\n'||a=='\t'))
				a=' ';
			if(bPre||!b||a!=' ')
				uCode[j++]=a;
			if(a==' ')
			{
				b=true;
				c=false;
			}
			else
				b=false;
		}
		i++;
	}
	if(uCode[j-1]==' '&&!c)
		uCode[j-1]='\0';
	else
		uCode[j]='\0';
	return true;
}

bool AString2Unicode(AString txt,int CodePage,ACHAR *uCode,int codelen)
{
	MultiByteToWideChar(CodePage,0,txt,-1,uCode,codelen);
	return true;
}

int Char2Int(char a)
{
	if(a>='0'&&a<='9')
		return a-48;
	else if(a>='a'&&a<='z')
		return a-87;
	else if(a>='A'&&a<='Z')
		return a-55;
	return 0;
}

A3DCOLOR Str2Color(AString *aStr)
{
	if(aStr==NULL)
		return 0;
	AString a=*aStr;
	if(a=="black")
		return 0;
	if(a=="white")
		return 0xFFFFFFFF;
	if(a=="black")
		return 0;
	if(a=="red")
		return 0xFFFF0000;
	if(a=="lime")
		return 0xFF00FF00;
	if(a=="blue")
		return 0xFF0000FF;
	if(a=="maroon")
		return 0xFF800000;
	if(a=="green")
		return 0xFF008000;
	if(a=="navy")
		return 0xFF000080;
	if(a=="olive")
		return 0xFF808000;
	if(a=="teal")
		return 0xFF008080;
	if(a=="purple")
		return 0xFF800080;
	if(a=="gray")
		return 0xFF808080;
	if(a=="fuchsia")
		return 0xFFFF00FF;
	if(a=="aqua")
		return 0xFF00FFFF;
	if(a=="yellow")
		return 0xFFFFFF00;
	if(a=="silver")
		return 0xFFC0C0C0;
	A3DCOLOR res=0;
	int i;
	if(a[0]!='#')
	{
		if(a.GetLength()<6)
			return DefaultFontColor;
		for(i=0;i<6;i++)
			if(!(a[i]>='a'&&a[i]<='f'||a[i]>='A'&&a[i]<='F'||a[i]>='0'&&a[i]<='9'))
				return DefaultFontColor;
		for(i=0;i<6;i++)
			res=(res<<4)|Char2Int(a[i]);
	}
	else
	{
		if(a.GetLength()<6)
			return DefaultFontColor;
		for(i=1;i<7;i++)
			if(!(a[i]>='a'&&a[i]<='f'||a[i]>='A'&&a[i]<='F'||a[i]>='0'&&a[i]<='9'))
				return DefaultFontColor;
		for(i=1;i<7;i++)
			res=(res<<4)|Char2Int(a[i]);
	}
	return 0xFF000000|res;
}

bool Str2FontName(AString *aStr,int CodePage,ACHAR *uCode,int codelen)
{
	int i=aStr->Find(',');
	if(i==-1)
		AString2Unicode(*aStr,CodePage,uCode,codelen);
	else
		AString2Unicode(aStr->Left(i),CodePage,uCode,codelen);
	return true;
}

AString FormatParm(ACHAR *src, int codepage)
{
	char c[300];
	WideCharToMultiByte(codepage,0,src,-1,c,300,NULL,NULL);
	AString des="";
	int len=strlen(c);
	for(int i=0;i<len;i++)
	{
		if(c[i]==' ')
			des+='+';
		else if(c[i]>='0'&&c[i]<='9'||
				c[i]>='A'&&c[i]<='Z'||
				c[i]>='a'&&c[i]<='z')
			des+=c[i];
		else
		{
			des+='%';
			char p;
			p=(c[i]>>4)&15;
			if(p<10)
				p+='0';
			else
				p+='A'-10;
			des+=p;
			p=c[i]&15;
			if(p<10)
				p+='0';
			else
				p+='A'-10;
			des+=p;
		}
	}
	return des;
}

int AUI_GetOneLineTextRect(A3DFTFont *pFont, const ACHAR *pszText,
	int nMaxWidth,int& nWidth, int& nHeight)
{
	nWidth = 0;
	nHeight = 0;
	if( !pFont )
		return -1;

	A3DPOINT2 ptSize;
	ACHAR szThis[2] = { 0, 0 };
	int i = 0;
	const ACHAR *pCh;
	for( pCh = pszText; *pCh!='\0'; pCh++)
	{
		if( *pCh == '\n' )
			break;
		else if( *pCh == '\r' )
		{
			i++;
			continue;
		}

		szThis[0] = pszText[i];
		ptSize = pFont->GetTextExtent(szThis);
		
		nWidth += ptSize.x;
		if(nWidth>nMaxWidth)
		{
			nWidth -= ptSize.x;
			break;
		}
		a_ClampFloor(nHeight, ptSize.y);
		i++;
	}

	return i;
}

UINT _DownloadPageImpl(LPVOID pParam)
{
	DownloadThreadData* threaddata=(DownloadThreadData*)pParam;
	CECBBSFileDownloader downloader;
	AString a;
	if(threaddata->bGet)
		a.Format("DownloadingPage: %s to %s",threaddata->pageinfo->url+threaddata->parm,threaddata->pageinfo->localfn);
	else
		a.Format("DownloadingPage: %s to %s",threaddata->pageinfo->url,threaddata->pageinfo->localfn);
	writelog(a);
	if(threaddata->parm=="")
		downloader.DownloadByUrl(threaddata->pageinfo->url,
								threaddata->pageinfo->localfn);
	else if(threaddata->bGet)
		downloader.DownloadByUrl(threaddata->pageinfo->url+threaddata->parm,
								threaddata->pageinfo->localfn);
	else
		downloader.DownloadByUrl(threaddata->pageinfo->url,threaddata->pageinfo->localfn,
				threaddata->parm,threaddata->parm.GetLength(),threaddata->parmtype);
	threaddata->pageinfo->downloaded=true;
	threaddata->pageinfo->ReleaseInThread();
	delete threaddata;
	return 1;
}

UINT WINAPI DownloadPage(LPVOID pParam)
{
	UINT ret;

	__try
	{
		ret = _DownloadPageImpl(pParam);
	}
	__except (glb_HandleException(GetExceptionInformation()))
	{
		a_LogOutput(1, "Exception occurred in DownloadPage thread... mini dumped!");
	}

	return ret;
}

UINT _ParsePageImpl(CECBBSPage* page, DownloadedPagesInfo* pageinfo)
{
	if(!pageinfo->downloaded)
	{
		HANDLE waitarray[2];
		waitarray[0]=page->m_hParseEvent;
		waitarray[1]=pageinfo->downloadthread;
		DWORD ret=WaitForMultipleObjects(2,waitarray,FALSE,INFINITE);
		if(ret==WAIT_OBJECT_0)
			return 1;
	}
	AString a;
	a.Format("LoadingLocalFile: %s",pageinfo->localfn);
	writelog(a);
	if( !page->LoadFile(pageinfo->localfn,pageinfo->url) )
	{
		a.Format("LoadingLocalFile failed.");
		writelog(a);
		return 1;
	}
	if(page->m_bStop)
	{
		a.Format("LoadingLocalFile: %s stopped",pageinfo->localfn);
		writelog(a);
		return 1;
	}
	a.Format("LoadingLocalFile: %s finished",pageinfo->localfn);
	writelog(a);
	while(!page->m_hFile.ImgNodeList.IsFinishLoadingImg())
	{
		if(page->m_bStop)
		{
			a.Format("LoadingPage: %s stopped",pageinfo->url);
			writelog(a);
			return 1;
		}
		Sleep(20);
	}
	a.Format("LoadingPage: %s",pageinfo->url);
	writelog(a);
	page->m_aNewURL=pageinfo->url;
	page->LoadPage();
	if(page->m_bStop)
	{
		a.Format("LoadingPage: %s stopped",pageinfo->url);
		writelog(a);
		return 1;
	}
	a.Format("LoadingPage: %s finished",pageinfo->url);
	writelog(a);
	return 0;
}

UINT WINAPI ParsePage(LPVOID pParam)
{
	UINT ret;

	CECBBSPage* page=(CECBBSPage*)pParam;

	// NOTICE:
	// here may cause problem, because Get and AddRef should be a atom operation
	// currently, the manager must wait all page parsing thread until them exited.
	DownloadedPagesInfo* pageinfo=DownloadedPages.GetPagesInfo(page->m_iNewPageNumber);
	pageinfo->AddRefInThread();
		
	__try
	{
		ret = _ParsePageImpl(page, pageinfo);
	}
	__except (glb_HandleException(GetExceptionInformation()))
	{
		a_LogOutput(1, "Exception occurred in ParsePage thread... mini dumped!");
	}
	
	pageinfo->ReleaseInThread();
	return ret;
}

//======================================CECBBSPage====================================
CECBBSPage::CECBBSPage(CECBBSPage *hParentPage)
{
	m_bGet=true;
	m_iContentType=CT_APPLICATION;
	m_pChPageList=NULL;
	m_pNewChPageList=NULL;
	m_pObjectList=NULL;
	m_pNewObjectList=NULL;
	m_pRootLabel=NULL;
	m_pNewRootLabel=NULL;
	m_pDownloadThreadId=0;
	int n = -1;
	AIniFile iniFile;
	if(iniFile.Open("configs\\loginpage.ini"))
	{
		n = iniFile.GetValueAsInt("URL", "CodePage", -1);
		iniFile.Close();
	}
	if( n != -1 )
		m_iCodePage=n;
	else
		m_iCodePage=936;
	m_hParentPage=hParentPage;
	m_aName="_blank";
	m_iHisPageNum=-1;
	m_iScrollPos=0;
	m_aParm="";
	m_iScrollHeight=0;
	m_pScroll=NULL;
	m_pViewport=NULL;
	m_pNewViewport=NULL;
	m_iDefaultFontSize=DefaultFontSize;
	m_iNewDefaultFontSize=-1;
	m_iNewPageNumber=-1;
	m_iNextPageNumber=-1;
	m_iPageNumber=-1;
	m_hParseEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hParseThread=NULL;
	m_aNextURL="";
	m_bSaveHistory=false;
}

CECBBSPage::~CECBBSPage()
{
	if(m_hParseThread!=NULL)
	{
		DWORD ret;
		if(GetExitCodeThread(m_hParseThread,&ret))
		{
			if(ret==STILL_ACTIVE)
			{
				m_hFile.bStop=true;
				m_bStop=true;
				SetEvent(m_hParseEvent);
				WaitForSingleObject(m_hParseThread,INFINITE);
			}
		}
		CloseHandle(m_hParseThread);
	}
	CloseHandle(m_hParseEvent);
	if(m_pViewport)
	{
		BBSDlg->DelFrame(m_pViewport);
		A3DRELEASE(m_pViewport);
	}
	A3DRELEASE(m_pNewViewport);
	DELETEPOINTER(m_pChPageList);
	DELETEPOINTER(m_pNewChPageList);
	A3DRELEASE(m_pObjectList);
	DELETEPOINTER(m_pNewObjectList);
	ReleaseLabel(m_pRootLabel);
	ReleaseLabel(m_pNewRootLabel);
}

void CECBBSPage::AddParamFile(ObjectNode *node)
{
	char cfname[1000];
	HTMLLabel *hLabel=node->label;
	WideCharToMultiByte(m_iCodePage,0,node->obj->GetText(),-1,cfname,1000,NULL,false);
	HANDLE hFile;
	hFile = CreateFileA(cfname,GENERIC_READ,FILE_SHARE_READ,NULL, 
                     OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return;
	DWORD iSize=GetFileSize(hFile,NULL);
	CloseHandle(hFile);
	
	if(iSize<500000)
	{
		FILE *f=fopen(cfname,"rb");
		if(f!=NULL)
		{
			char *cvalue=(char*)a_malloctemp(sizeof(char)*(iSize+3));
			fread(cvalue,sizeof(char),iSize,f);
			AString p;
			p="--";
			p+=MultiPartBoundary;
			p+="\r\nContent-Disposition: form-data; name=\"";
			m_aParm+=p;
			m_aParm+=*hLabel->Attrib->GetAttribute("name");
			m_aParm+="\"; filename=\"";
			m_aParm+=cfname;
			m_aParm+="\"\r\nContent-Type: image/pjepg\r\n\r\n";
			int len=m_aParm.GetLength();
			AString *a=new AString(cvalue,iSize);
			m_aParm+=*a;
			delete a;
			m_aParm+="\r\n";
			a_freetemp(cvalue);
			fclose(f);
		}
	}
}

void CECBBSPage::AddParam(ACHAR *name,ACHAR *value)
{
	if(m_iContentType==CT_APPLICATION)
	{
		AString p;
		if(m_aParm=="")
		{
			if(m_bGet)
				m_aParm+="?";
		}
		else
			m_aParm+="&";
		p=FormatParm(name,m_iCodePage);
		m_aParm+=p;
		m_aParm+="=";
		if(value[0]!='\0')
		{
			p=FormatParm(value,m_iCodePage);
			m_aParm+=p;
		}
	}
	else
	{
		char cname[1000],cvalue[3000];
		WideCharToMultiByte(m_iCodePage,0,name,-1,cname,1000,NULL,false);
		WideCharToMultiByte(m_iCodePage,0,value,-1,cvalue,3000,NULL,false);
		AString p;
		p="--";
		p+=MultiPartBoundary;
		p+="\r\nContent-Disposition: form-data; name=\"";
		m_aParm+=p;
		m_aParm+=cname;
		m_aParm+="\"\r\n\r\n";
		if(cvalue!="")
			m_aParm+=cvalue;
		m_aParm+="\r\n";
	}
}

void CECBBSPage::CalcFontHeight()
{
	for(int i=0;i<MaxFontNum;i++)
	{
		PointFont[i]=GetPointFont(DefaultFontName,FontSize[i]);
//		AUI_GetTextRect(PointFont[i],L"i",nWidth,nHeight,nLines);
		RealFontHeight[i]=PointFont[i]->GetFontHeight();
	}
}

void CECBBSPage::Submit(ObjectNode *pNode)
{
	m_aParm="";
	HTMLLabel *hLabel=pNode->label;
	AString *aStr;
	ACHAR name[100];
	ACHAR value[5000];
	ACHAR temp[5000];
	while(hLabel!=NULL&&hLabel->LabelType!="form")
		hLabel=hLabel->ParentLabel;
	int len=0;
	HTMLLabel *hFormLabel=hLabel;
	if(hFormLabel!=NULL)
	{
		aStr=hFormLabel->Attrib->GetAttribute("method");
		if(aStr==NULL)
			m_bGet=true;
		else
		{
			aStr->MakeLower();
			if((*aStr)=="get")
				m_bGet=true;
			else
				m_bGet=false;
		}
		aStr=hFormLabel->Attrib->GetAttribute("enctype");
		if(aStr!=NULL)
		{
			aStr->MakeLower();
			if((*aStr)=="multipart/form-data")
				m_iContentType=CT_MULTIPART;
		}
		ObjectNode *node=m_pObjectList->head;
		while( (node=node->next) != NULL)
		{
			hLabel=node->label;
			while(hLabel!=NULL&&hLabel!=hFormLabel)
				hLabel=hLabel->ParentLabel;
			if(hLabel==hFormLabel)
			{
				hLabel=node->label;
				if(hLabel->LabelType!="input"||hLabel->LabelType!="textarea")
				{
					aStr=hLabel->Attrib->GetAttribute("name");
					int type=node->obj->GetType();
					if(aStr!=NULL)
					{
						AString2Unicode(*aStr,m_iCodePage,name,1000);
						value[0]='\0';
						if(type==AUIOBJECT_TYPE_EDITBOX)
						{
							aStr=hLabel->Attrib->GetAttribute("type");
							if(aStr!=NULL&&(*aStr)=="file")
								AddParamFile(node);
							else
							{
								a_strcpy(temp,node->obj->GetText());
								int j=0;
								int len=a_strlen(temp);
								for(int i=0;i<=len;i++)
									if(temp[i]!='\r')
										value[j++]=temp[i];
									else
									{
										value[j++]='\r';
										value[j++]='\n';
									}
								AddParam(name,value);
							}
						}
						else if(type==AUIOBJECT_TYPE_CHECKBOX)
						{
							if(((PAUICHECKBOX)node->obj)->IsChecked())
							{
								aStr=hLabel->Attrib->GetAttribute("value");
								if(aStr!=NULL)
									AString2Unicode(*aStr,m_iCodePage,value,5000);
								AddParam(name,value);
							}
						}
						else if(type==AUIOBJECT_TYPE_RADIOBUTTON)
						{
							if(((PAUIRADIOBUTTON)node->obj)->IsChecked())
							{
								aStr=hLabel->Attrib->GetAttribute("value");
								if(aStr!=NULL)
									AString2Unicode(*aStr,m_iCodePage,value,5000);
								AddParam(name,value);
							}
						}
						else if(type==AUIOBJECT_TYPE_STILLIMAGEBUTTON)
						{
							AString *a=hLabel->Attrib->GetAttribute("type");
							if(node==pNode||a!=NULL&&*a=="hidden")
							{
								aStr=hLabel->Attrib->GetAttribute("value");
								if(aStr!=NULL)
									AString2Unicode(*aStr,m_iCodePage,value,5000);
								AddParam(name,value);
							}
						}
						else if(type==AUIOBJECT_TYPE_LISTBOX||type==AUIOBJECT_TYPE_COMBOBOX)
						{
							int sel;
							if(type==AUIOBJECT_TYPE_LISTBOX)
								sel=((PAUILISTBOX)node->obj)->GetCurSel();
							else
								sel=((PAUICOMBOBOX)node->obj)->GetCurSel();
							HTMLLabel *chLabel=hLabel->FirstChildLabel;
							while(chLabel!=NULL&&sel>=0)
							{
								if(chLabel->LabelType=="option"&&chLabel->NextLabel!=NULL&&
									chLabel->NextLabel->LabelType=="text")
								{
									if(sel==0)
									{
										AString *txt=chLabel->Attrib->GetAttribute("value");
										AString2Unicode(*txt,m_iCodePage,value,5000);
										AddParam(name,value);
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
			}
		}
		if(m_iContentType==CT_MULTIPART)
		{
			m_aParm+="--";
			m_aParm+=MultiPartBoundary;
			m_aParm+="--";
		}
		m_aNextURL=GetFullURL(*hFormLabel->Attrib->GetAttribute("action"));
		m_iNextPageNumber=-1;
		m_bNextSaveHistory=true;
		m_bNextNewPage=true;
	}
}

void CECBBSPage::InsertText(HTMLLabel *hLabel)
{
	AString *a=hLabel->Attrib->GetAttribute("inserttext");
	while(hLabel!=NULL&&hLabel->LabelType!="form")
		hLabel=hLabel->ParentLabel;
	HTMLLabel *hFormLabel=hLabel;
	if(hFormLabel!=NULL)
	{
		HTMLLabel *hFormLabel=hLabel;
		ObjectNode *node=m_pObjectList->head;
		while( (node=node->next) != NULL)
		{
			hLabel=node->label;
			while(hLabel!=NULL&&hLabel!=hFormLabel)
				hLabel=hLabel->ParentLabel;
			if(hLabel==hFormLabel)
			{
				hLabel=node->label;
				if(hLabel->LabelType=="textarea")
				{
					ObjectNode *ObjNode=m_pObjectList->FindObject(hLabel);
					if(ObjNode!=NULL)
					{
						ACHAR txt[200];
						int len=MultiByteToWideChar(m_iCodePage,0,*a,-1,txt,200);
						for(int i=0;i<len-1;i++)
							ObjNode->obj->OnDlgItemMessage(WM_CHAR,txt[i],0);
					}
					BBSDlg->ChangeFocus(ObjNode->obj);

					return;
				}
			}
		}
	}
}

void CECBBSPage::Reset(ObjectNode *pNode)
{
	HTMLLabel *hLabel=pNode->label;
	AString *aStr;
	while(hLabel!=NULL&&hLabel->LabelType!="form")
		hLabel=hLabel->ParentLabel;
	int len=0;
	HTMLLabel *hFormLabel=hLabel;
	if(hFormLabel!=NULL)
	{
		HTMLLabel *hFormLabel=hLabel;
		ObjectNode *node=m_pObjectList->head;
		while( (node=node->next) != NULL)
		{
			hLabel=node->label;
			while(hLabel!=NULL&&hLabel!=hFormLabel)
				hLabel=hLabel->ParentLabel;
			if(hLabel==hFormLabel)
			{
				hLabel=node->label;
				if(hLabel->LabelType!="input"||hLabel->LabelType!="textarea")
				{
					int type=node->obj->GetType();
					if(type==AUIOBJECT_TYPE_EDITBOX)
					{
						aStr=hLabel->Attrib->GetAttribute("value");
						if(aStr!=NULL)
						{
							int l=MultiByteToWideChar(m_iCodePage,0,*aStr,-1,NULL,NULL);
							ACHAR *uCode=(ACHAR*)a_malloctemp(sizeof(ACHAR) * (l+1));
							MultiByteToWideChar(m_iCodePage,0,*aStr,-1,uCode,l);
							uCode[l]='\0';
							node->obj->SetText(uCode);
							a_freetemp(uCode);
						}
						else
							node->obj->SetText(TEXT(""));
					}
					else if(type==AUIOBJECT_TYPE_CHECKBOX)
					{
						aStr=hLabel->Attrib->GetAttribute("checked");
						if(aStr!=NULL)
							((PAUICHECKBOX)node->obj)->Check(true);
						else
							((PAUICHECKBOX)node->obj)->Check(false);
					}
					else if(type==AUIOBJECT_TYPE_RADIOBUTTON)
					{
						aStr=hLabel->Attrib->GetAttribute("checked");
						if(aStr!=NULL)
							((PAUIRADIOBUTTON)node->obj)->Check(true);
						else
							((PAUIRADIOBUTTON)node->obj)->Check(false);
					}
					else if(type==AUIOBJECT_TYPE_LISTBOX)
					{
						aStr=hLabel->Attrib->GetAttribute("defaultsel");
						if(aStr!=NULL)
							((PAUILISTBOX)node->obj)->SetCurSel(aStr->ToInt());
					}
					else if(type==AUIOBJECT_TYPE_COMBOBOX)
					{
						aStr=hLabel->Attrib->GetAttribute("defaultsel");
						if(aStr!=NULL)
							((PAUICOMBOBOX)node->obj)->SetCurSel(aStr->ToInt());
					}
				}
			}
		}
	}
}

CECBBSPage* CECBBSPage::GetPageFromCursor(int x,int y)
{
	if(m_aPostion.PtInRect(x,y)&&m_pChPageList!=NULL)
	{
		CECBBSPage *page=m_pChPageList->GetPageFromCursor(x,y);
		if(page==NULL)
			return this;
		else
			return page;
	}
	else
		return NULL;
}

AString CECBBSPage::GetFullURL(AString url,bool bNew)
{
	AString srcurl;
	if(bNew)
		srcurl=m_aNewURL;
	else
		srcurl=m_aURL;
	if(url.GetLength()<7||url.Left(7)!="http://")
	{
		int i=srcurl.GetLength()-1;
		if(url[0]=='/')
		{
			i=7;
			while(i<srcurl.GetLength()&&srcurl[i+1]!='/')
				i++;
		}
		else if(url[0]=='?')
		{
			while(i>6&&srcurl[i]!='?')
				i--;
			if(i<=6)
				i=srcurl.GetLength();
			i--;
		}
		else
		{
			while(i>6&&srcurl[i]!='/')
				i--;
		}
		if(url[0]=='#'&&url[1]=='\0')
			url=srcurl;
		else
			url=srcurl.Left(i+1)+url;
	}
	return url;
}

bool CECBBSPage::PrepareNewPage()
{
	ResetEvent(m_hParseEvent);
	m_bStop=false;
	m_hFile.bStop=false;
	UINT t;
	if(m_iNextPageNumber==-1)
		m_iNewPageNumber=DownloadedPages.AddPage(m_aNextURL,this);
	else
		m_iNewPageNumber=m_iNextPageNumber;
	if(m_hParentPage==NULL && m_aName=="")
		m_aName.Format("BBSPage %d",m_iNewPageNumber);
	m_bSaveHistory=m_bNextSaveHistory;
	m_bNewPage=m_bNextNewPage;
	m_iNewDefaultFontSize=DefaultFontSize;
	m_bNextSaveHistory=false;
	m_bNextNewPage=false;
	m_iNextPageNumber=-1;
	m_aNextURL="";
	
	m_hParseThread=(HANDLE)_beginthreadex(NULL,0,ParsePage,this,0,&t);

	return true;
}

bool CECBBSPage::LoadFile(AString file,AString url)
{
	if(m_pNewRootLabel!=NULL)
		return false;
	m_pNewRootLabel=m_hFile.LoadFile(file,url);
	if(m_pNewRootLabel==NULL)
		m_pNewRootLabel=m_hFile.LoadFile(errorhtml,url);
	return (m_pNewRootLabel!=NULL);
}

bool CECBBSPage::LoadPage()
{
	if(m_pNewRootLabel==NULL)
		return true;
	if(m_pNewRootLabel->FirstChildLabel==NULL)
		return true;

	m_hHrefLabel=NULL;
	m_pGlobalInfo=new GlobalInfo;
	m_pNewChPageList=new CECBBSPageList;
	m_pNewObjectList=new ObjectList;
	m_bBreak=false;

	m_pNewViewport=new A3DViewport;
	A3DVIEWPORTPARAM parm = { 0,0,0,0,0.0f,1.0f };
	bool bret=m_pNewViewport->Init(g_pGame->GetA3DDevice(), &parm, false, false, 0);
	if(!bret)
		return false;

	A3DRECT aRect=m_aPostion;
	float fWindowScale = BBSDlg->GetAUIManager()->GetWindowScale();
	aRect.left = int(aRect.left + 5 * fWindowScale);
	aRect.right = int(aRect.right - 22 * fWindowScale);
	aRect.top = int(aRect.top + 5 * fWindowScale);
	aRect.bottom = int(aRect.bottom - 5 * fWindowScale);

	m_iFirstLineWidth=0;
	m_iFirstLineHeight=0;
	m_pGlobalInfo->iTextColor=DefaultFontColor;
	m_pGlobalInfo->iTextSize=m_iNewDefaultFontSize;
	m_pGlobalInfo->bPreText=false;
	m_pGlobalInfo->iAlign=0;
	m_pGlobalInfo->iVAlign=0;
	m_pGlobalInfo->bDrawObject=true;
	m_bLastParagraph=false;
	a_strcpy(m_pGlobalInfo->aFont,DefaultFontName);
	m_iRadioGroupID=0;
	m_hTable=NULL;
	CreateHTMLLabel(m_pNewRootLabel->FirstChildLabel,aRect);
	if(m_iFirstLineWidth!=0)
		CreateBreak(aRect);
	if(aRect.top>aRect.bottom)
	{
		m_iScrollHeight=aRect.top-aRect.bottom;
		CreatePageScroll();
	}
	else
	{
		m_iScrollHeight=0;
		m_pScroll=NULL;
	}
	m_iScrollPos=0;
	m_pTableList.ReleaseTables();
	delete m_pGlobalInfo;
	if(m_bStop)
	{
		A3DRELEASE(m_pNewViewport);
		DELETEPOINTER(m_pNewChPageList);
		DELETEPOINTER(m_pNewObjectList);
		ReleaseLabel(m_pNewRootLabel);
		m_pNewRootLabel=NULL;
	}
	return true;
}

bool CECBBSPage::LoadNewImage()
{
	if(m_pObjectList!=NULL)
		m_pObjectList->LoadNewImage();
	if(m_pChPageList!=NULL)
		m_pChPageList->LoadNewImage();
	return true;
}

bool CECBBSPage::UseNewPage(bool bStop,bool &bNewHistoryPage)
{
	HistoryPagesInfo* pageinfo;
	HistoryPageNode* pagenode;
	if(m_hParseThread!=NULL)
	{
		DWORD ret;
		if(GetExitCodeThread(m_hParseThread,&ret))
		{
			if(ret==0)
			{
				CloseHandle(m_hParseThread);
				m_hParseThread=NULL;
				DownloadedPages.SaveParm(m_iPageNumber,this);
				DownloadedPages.LoadParm(m_iNewPageNumber,this);
				AString a;
				a.Format("Page %d: UpdatePage %s",this,m_aNewURL);
				writelog(a);
				m_iDefaultFontSize=m_iNewDefaultFontSize;
				m_iNewDefaultFontSize=-1;
				m_iPageNumber=m_iNewPageNumber;
				m_iNewPageNumber=-1;
				if(m_bSaveHistory)
				{
					bNewHistoryPage=true;
					HistoryPagesMan.SetNowPage(HistoryPagesMan.GetNowPage()+1,true);
				}
				if(m_bNewPage)
				{
					pageinfo=HistoryPagesMan.GetHistoryPageInfo(HistoryPagesMan.GetNowPage());
					pagenode=pageinfo->GetPageNode(m_aName);
					if(pagenode!=NULL)
						pagenode->pageindex=m_iPageNumber;
					else
						pageinfo->AddPageNode(m_aName,m_iPageNumber);
					if(m_hParentPage!=NULL)
					{
						for(int i=0;i<HistoryPagesMan.GetTotalPages();i++)
						{
							pageinfo=HistoryPagesMan.GetHistoryPageInfo(i);
							pagenode=pageinfo->GetPageNode(m_hParentPage->m_aName);
							if(pagenode!=NULL)
							{
								pagenode=pageinfo->GetPageNode(m_aName);
								if(pagenode==NULL)
									pageinfo->AddPageNode(m_aName,m_iPageNumber);
							}
						}
					}
				}
				m_aParm="";
				m_bGet=true;
				m_iContentType=CT_APPLICATION;
				m_aURL=m_aNewURL;
				m_aNewURL="";
				m_aLocalFn=m_aNewLocalFn;
				m_aNewLocalFn="";
				ReleaseLabel(m_pRootLabel);
				A3DRELEASE(m_pObjectList);
				DELETEPOINTER(m_pChPageList);
				
				if(m_pViewport)
				{
					BBSDlg->DelFrame(m_pViewport);
					A3DRELEASE(m_pViewport);
				}
				m_pViewport=m_pNewViewport;
				m_pNewViewport=NULL;
				A3DRECT aRect=m_aPostion;
				float fWindowScale = BBSDlg->GetAUIManager()->GetWindowScale();
				aRect.left = int(aRect.left / fWindowScale + 5);
				aRect.right = int(aRect.right / fWindowScale - 22);
				aRect.top = int(aRect.top / fWindowScale + 5);
				aRect.bottom = int(aRect.bottom / fWindowScale - 5);
				BBSDlg->AddFrame(m_pViewport,aRect);

				m_pObjectList=m_pNewObjectList;
				ObjectNode *node=m_pObjectList->head->next;
				while(node!=NULL)
				{
					char p[100];
					sprintf(p,"BBSControl %d",ControlNumber++);
					BBSDlg->AddObjectToList(node->obj);
					node->obj->SetName(p);
					node=node->next;
				}
				m_pRootLabel=m_pNewRootLabel;
				m_iScrollPos=0;
				m_pChPageList=m_pNewChPageList;

				m_pNewRootLabel=NULL;
				m_pNewObjectList=NULL;
				m_pNewChPageList=NULL;
			}
			else if(ret==STILL_ACTIVE)
			{
				if(bStop || m_aNextURL!="")
				{
					m_hFile.bStop=true;
					m_bStop=true;
					SetEvent(m_hParseEvent);
				}
			}
			else
			{
				CloseHandle(m_hParseThread);
				m_hParseThread=NULL;
			}
		}
	}
	int nowPage=HistoryPagesMan.GetNowPage();
	if(nowPage>=0)
	{
		pageinfo=HistoryPagesMan.GetHistoryPageInfo(nowPage);
		pagenode=pageinfo->GetPageNode(m_aName);
		if(pagenode!=NULL && !pagenode->bLoaded)
		{
			int num=pagenode->pageindex;
			if( !(DefaultFontSize==m_iDefaultFontSize && num==m_iPageNumber && m_iNewPageNumber==-1) && 
				!(DefaultFontSize==m_iNewDefaultFontSize && num!=m_iNewPageNumber))
			{
				m_iNextPageNumber=num;
				m_aNextURL=DownloadedPages.GetPagesInfo(num)->url;
				m_bNextSaveHistory=false;
				m_bNextNewPage=false;
			}
			pagenode->bLoaded=true;
		}
	}
	if(m_hParseThread==NULL && m_aNextURL!="")
		PrepareNewPage();
	if(m_pChPageList!=NULL)
	{
		CECBBSPageNode *node=m_pChPageList->head->next;
		while(node!=NULL)
		{
			node->page->UseNewPage(bStop,bNewHistoryPage);
			node=node->next;
		}
	}
	return true;
}

ObjectNode* CECBBSPage::FindObject(PAUIOBJECT pObj)
{
	ObjectNode *res=NULL;
	if(m_pObjectList!=NULL)
		res=m_pObjectList->FindObject(pObj);
	if(res==NULL&&m_pChPageList!=NULL)
		res=m_pChPageList->FindObject(pObj);
	return res;
}

CECBBSPage* CECBBSPage::FindPage(AString aName)
{
	if(m_aName==aName)
		return this;
	if(m_pChPageList!=NULL)
		return m_pChPageList->FindPage(aName);
	return NULL;
}

void CECBBSPage::ScrollPage(int iInc)
{
	if(m_pScroll!=NULL)
	{
		int iBarLevel=m_pScroll->GetBarLevel();
		iInc+=m_iScrollPos-iBarLevel;
		m_iScrollPos=iBarLevel;
	}
	if(iInc!=0&&m_pObjectList!=NULL)
		m_pObjectList->ScrollPage(iInc);
	if(m_pChPageList!=NULL)
		m_pChPageList->ScrollPage(iInc);
}

bool CECBBSPage::CreatePageScroll()
{
	HTMLLabel *hRootLabel=m_pNewRootLabel->FirstChildLabel;
	HTMLLabel *hLabel;
	HTMLLabel *chLabel=hRootLabel->FirstChildLabel;
	while(chLabel->NextLabel!=NULL)
		chLabel=chLabel->NextLabel;

	float fWindowScale = BBSDlg->GetAUIManager()->GetWindowScale();
	
	PAUISCROLL scroll=new AUIScroll;
	scroll->SetParent(BBSDlg);
	scroll->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
	scroll->SetPos(int(m_aPostion.right-16*fWindowScale),m_aPostion.top);
	
	//
	// NOTICE: height will be set here but width will be loaded from template
	//
	scroll->SetSize(0, m_aPostion.Height());

	if(!LoadTemplateFrame(scroll))
	{
		A3DRELEASE(scroll);
		return false;
	}

	scroll->SetScrollRange(0,m_iScrollHeight);
	scroll->SetBarLength(m_aPostion.Height());
	scroll->SetScrollStep(60);
	hLabel=new HTMLLabel;
	hLabel->Attrib=new HTMLLabelAttribute;
	hLabel->LabelType="PageScroll";
	hLabel->ParentLabel=hRootLabel;
	hLabel->FirstChildLabel=NULL;
	hLabel->NextLabel=NULL;
	chLabel->NextLabel=hLabel;
	chLabel=hLabel;
	m_pNewObjectList->AddObject(hLabel,scroll,this);
	m_pScroll=scroll;
	
	return true;
}

bool CECBBSPage::CreateHTMLLabel(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(	hLabel==NULL || m_bStop)
		return true;
	if(hLabel->ParentLabel!=NULL&&
		(hLabel->ParentLabel->LabelType=="style"||
		hLabel->ParentLabel->LabelType=="script"||
		hLabel->ParentLabel->LabelType=="title"||
		hLabel->ParentLabel->LabelType=="noframes"))
		return true;
	while(hLabel!=NULL)
	{
#ifdef _USESTYLE
		HTMLLabel *styleLabel=m_hFile.StyleLabel;
		AString *a=hLabel->Attrib->GetAttribute("class");
		bool bNewStyle=false;
		GlobalInfo *t;
		while(styleLabel!=NULL)
		{
			if(	styleLabel->LabelType==hLabel->LabelType||	a!=NULL&&
				styleLabel->LabelType[0]=='.'&&styleLabel->LabelType=='.'+*a)
			{
				bNewStyle=true;
				t=m_pGlobalInfo;
				m_pGlobalInfo=new GlobalInfo;
				*m_pGlobalInfo=*t;
//				m_pGlobalInfo->iTextSize=m_iDefaultFontSize;
//				a_strcpy(m_pGlobalInfo->aFont,DefaultFontName);
//				m_pGlobalInfo->iTextColor=DefaultFontColor;
				GetFont(m_pGlobalInfo,styleLabel);
				break;
			}
			styleLabel=styleLabel->NextLabel;
		}
#endif
		if(hLabel->LabelType=="text")
			CreateText(hLabel,lRect);
		else if(hLabel->LabelType=="br")
			CreateBreak(lRect);
		else if(hLabel->LabelType=="td"||hLabel->LabelType=="th")
			CreateTD(hLabel,lRect);
		else if(hLabel->LabelType=="tr")
			CreateTR(hLabel,lRect);
		else if(hLabel->LabelType=="a")
			CreateLink(hLabel,lRect);
		else if(hLabel->LabelType=="font")
			CreateFont(hLabel,lRect);
		else if(hLabel->LabelType=="pre")
			CreatePre(hLabel,lRect);
		else if(hLabel->LabelType=="table")
			CreateTable(hLabel,lRect);
		else if(hLabel->LabelType=="p")
			CreateParagraph(hLabel,lRect);
		else if(hLabel->LabelType=="div")
			CreateDIV(hLabel,lRect);
		else if(hLabel->LabelType=="img")
			CreateImage(hLabel,lRect);
		else if(hLabel->LabelType=="center")
			CreateCenter(hLabel,lRect);
		else if(hLabel->LabelType=="input")
			CreateInput(hLabel,lRect);
		else if(hLabel->LabelType=="hr")
			CreateHR(hLabel,lRect);
		else if(hLabel->LabelType=="textarea")
			CreateTextArea(hLabel,lRect);
		else if(hLabel->LabelType=="form")
			CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
		else if(hLabel->LabelType=="select")
			CreateSelect(hLabel,lRect);
		else if(hLabel->LabelType=="frameset")
			CreateFrame(hLabel,m_aPostion);
		else
			CreateHTMLLabel(hLabel->FirstChildLabel,lRect);

#ifdef _USESTYLE
		if(bNewStyle)
		{
			delete m_pGlobalInfo;
			m_pGlobalInfo=t;
		}
#endif
		hLabel=hLabel->NextLabel;
	}
	return true;
}

bool CECBBSPage::CreateLink(HTMLLabel *hLabel,A3DRECT &lRect)
{
	HTMLLabel *oldHrefLabel=m_hHrefLabel;
	m_hHrefLabel=hLabel;
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	m_hHrefLabel=oldHrefLabel;
	return true;
}

bool CECBBSPage::CreateBreak(A3DRECT &lRect)
{
	if(m_iFirstLineWidth==0)
		m_iFirstLineHeight=RealFontHeight[m_pGlobalInfo->iTextSize];
	if(m_hTable!=NULL&&m_hTable->iInit==1)
		if(m_iFirstLineWidth>m_hTable->iTdWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdWidth[m_hTable->iRows][m_hTable->iCols]=m_iFirstLineWidth;
	lRect.top+=m_iFirstLineHeight+1;
	if(m_pGlobalInfo->bDrawObject&&m_pGlobalInfo->iAlign!=0&&m_iFirstLineWidth!=0)
	{
		ObjectNode *node=m_pLineStart;
		int iWidth=lRect.Width()-m_iFirstLineWidth;
		while(node!=NULL)
		{
			POINT p=node->obj->GetPos(true);
			if(m_pGlobalInfo->iAlign==1)
				node->obj->SetPos(p.x+iWidth/2,p.y);
			else if(m_pGlobalInfo->iAlign==2)
				node->obj->SetPos(p.x+iWidth,p.y);
			node=node->next;
		}
		m_bBreak=true;
		m_pLineStart=NULL;
	}
	m_iFirstLineWidth=0;
	m_iFirstLineHeight=0;
	return true;
}

bool CECBBSPage::CreateCenter(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	int iOldAlign=m_pGlobalInfo->iAlign;
	m_pGlobalInfo->iAlign=1;
	if(m_pGlobalInfo->iAlign!=0)
	{
		m_bBreak=true;
		m_pLineStart=NULL;
	}
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	m_pLineStart=NULL;
	m_pGlobalInfo->iAlign=iOldAlign;
	return true;
}

bool CECBBSPage::CreateDIV(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	AString *aStr;
	int iOldAlign=m_pGlobalInfo->iAlign;
	int iOldVAlign=m_pGlobalInfo->iVAlign;
	aStr=hLabel->Attrib->GetAttribute("align");
	if(aStr!=NULL)
	{
		AString a=*aStr;
		if(a=="left")
			m_pGlobalInfo->iAlign=0;
		else if(a=="center")
			m_pGlobalInfo->iAlign=1;
		else if(a=="right")
			m_pGlobalInfo->iAlign=2;
	}
	aStr=hLabel->Attrib->GetAttribute("valign");
	if(aStr!=NULL)
	{
		AString a=*aStr;
		if(a=="top")
			m_pGlobalInfo->iVAlign=1;
		else if(a=="middle")
			m_pGlobalInfo->iVAlign=0;
		else if(a=="bottom")
			m_pGlobalInfo->iVAlign=2;
	}
	if(m_pGlobalInfo->iAlign!=0)
	{
		m_bBreak=true;
		m_pLineStart=NULL;
	}
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	m_pLineStart=NULL;
	m_pGlobalInfo->iAlign=iOldAlign;
	m_pGlobalInfo->iVAlign=iOldVAlign;
	return true;
}

bool CECBBSPage::GetFont(GlobalInfo *gInfo,HTMLLabel *hLabel)
{
	AString *aStr;
	aStr=hLabel->Attrib->GetAttribute("size");
	if(aStr!=NULL)
	{
//		if(aStr->GetLength()>0&&(*aStr)[aStr->GetLength()-1]=='p')
//		{
//			gInfo->iTextSize=aStr->Left(aStr->GetLength()-1).ToInt();
//			for(int j=0;j<6;j++)
//				if(gInfo->iTextSize<=FontSize[j])
//				{
//					gInfo->iTextSize=FontSize[j];
//					break;
//				}
//			a_Clamp(gInfo->iTextSize,FontSize[0],FontSize[6]);
//		}
//		else
		{
			int i=aStr->ToInt();
			if((*aStr)[0]=='+'||(*aStr)[0]=='-')
				i+=gInfo->iTextSize;
			if(i<=0||i>7)
				gInfo->iTextSize=m_iNewDefaultFontSize;
			else
				gInfo->iTextSize=i;
		}
	}
	aStr=hLabel->Attrib->GetAttribute("color");
	if(aStr!=NULL)
		gInfo->iTextColor=Str2Color(aStr);
	aStr=hLabel->Attrib->GetAttribute("face");
	if(aStr!=NULL)
		Str2FontName(aStr,m_iCodePage,gInfo->aFont,20);
	return true;
}

bool CECBBSPage::CreateFont(HTMLLabel *hLabel,A3DRECT &lRect)
{
	GlobalInfo *t=m_pGlobalInfo;
	m_pGlobalInfo=new GlobalInfo;
	*m_pGlobalInfo=*t;
	m_pGlobalInfo->iTextSize=m_iNewDefaultFontSize;
	a_strcpy(m_pGlobalInfo->aFont,DefaultFontName);
	m_pGlobalInfo->iTextColor=DefaultFontColor;
	GetFont(m_pGlobalInfo,hLabel);
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	delete m_pGlobalInfo;
	m_pGlobalInfo=t;
	return true;
}

bool CECBBSPage::CreateFrame(HTMLLabel *hLabel,A3DRECT &lRect)
{
	AString aStr;
	int size[10];
	int iTotal;
	int iTotalFrame=0;
	int i;
	bool bRows;
	A3DRECT tempPos;
	AString *lpStr=hLabel->Attrib->GetAttribute("rows");
	if(lpStr!=NULL&&lpStr->Find(',')!=-1)
	{
		aStr=*lpStr;
		iTotal=lRect.Height();
		bRows=true;
	}
	else
	{
		lpStr=hLabel->Attrib->GetAttribute("cols");
		if(lpStr==NULL)
			return false;
		aStr=*lpStr;
		iTotal=lRect.Width();
		bRows=false;
	}
	size[0]=0;
	aStr+=',';
	i=aStr.Find(',');
	while(i!=-1)
	{
		if(aStr[i-1]=='%')
			size[iTotalFrame+1]=size[iTotalFrame]+iTotal*aStr.Left(i-1).ToInt()/100;
		else if(aStr[i-1]=='*')
			size[iTotalFrame+1]=size[0]+iTotal;
		else
			size[iTotalFrame+1]=size[iTotalFrame]+aStr.Left(i).ToInt();
		iTotalFrame++;
		aStr.CutLeft(i+1);
		i=aStr.Find(',');
	}
	HTMLLabel *chLabel=hLabel->FirstChildLabel;
	i=0;
	while(chLabel!=NULL)
	{
		if(chLabel->LabelType=="frame")
		{
			CECBBSPage *hPage;
			hPage=m_pNewChPageList->CreatePage(this);
			hPage->m_aURL=m_aURL;
			AString *a=chLabel->Attrib->GetAttribute("name");
			if(a==NULL)
				hPage->m_aName="";
			else
				hPage->m_aName=*a;
			if(bRows)
				tempPos.SetRect(lRect.left,lRect.top+size[i],
							  lRect.right,lRect.top+size[i+1]);
			else
				tempPos.SetRect(lRect.left+size[i],lRect.top,
							  lRect.left+size[i+1],lRect.bottom);
			if(i!=0)
			{
				if(bRows)
					CreateImageLine(hLabel,	lRect.left,lRect.top+size[i],
											lRect.right,lRect.top+size[i]);
				else
					CreateImageLine(hLabel,	lRect.left+size[i],lRect.top,
											lRect.left+size[i],lRect.bottom);
			}

			i++;
			hPage->SetPos(tempPos);
			hPage->m_aParm="";
			hPage->m_iPageNumber=-1;
			hPage->m_aNextURL=GetFullURL(*chLabel->Attrib->GetAttribute("src"),true);
			hPage->m_iNextPageNumber=-1;
			hPage->m_bNextSaveHistory=false;
			hPage->m_bNextNewPage=m_bNewPage;
		}
		else if(chLabel->LabelType=="frameset")
		{
			if(bRows)
				tempPos.SetRect(lRect.left,lRect.top+size[i],
							  lRect.right,lRect.top+size[i+1]);
			else
				tempPos.SetRect(lRect.left+size[i],lRect.top,
							  lRect.right+size[i+1],lRect.bottom);
			i++;
			CreateFrame(chLabel,tempPos);
		}
		chLabel=chLabel->NextLabel;
	}
	return true;
}

bool CECBBSPage::CreateHR(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	int nWidth=lRect.Width();
	int nStart=0;
	AString *aStr=hLabel->Attrib->GetAttribute("width");
	if(aStr!=NULL)
	{
		int p=aStr->Find('%');
		if(p>=0)
			nWidth=aStr->Left(p).ToInt()*lRect.Width()/100;
		else
			nWidth=aStr->ToInt();
	}
	if(m_pGlobalInfo->bDrawObject)
	{
		aStr=hLabel->Attrib->GetAttribute("align");
		nStart=(lRect.Width()-nWidth)/2;
		if(aStr!=NULL)
		{
			AString a=*aStr;
			if(a=="left")
				nStart=0;
			else if(a=="right")
				nStart=lRect.Width()-nWidth;
		}
		nStart+=lRect.left;
		CreateImageLine(hLabel,nStart,lRect.top+6,nStart+nWidth,lRect.top+6);
	}
	CreateBreak(lRect);
	return true;
}

bool CECBBSPage::CreateImage(HTMLLabel *hLabel,A3DRECT &lRect)
{
	AString *a=hLabel->Attrib->GetAttribute("imgindex");
	if(a==NULL)
		return false;
	ImageInfo* node=ImgList.GetImageInfo(a->ToInt());
	if(node==NULL)
		return true;
	bool bCantDisplay=false;
	D3DXIMAGE_INFO ImageFileInfo;
	if(node->state==IMG_DOWNLOADED)
	{
		if(D3DXGetImageInfoFromFileA(node->fn,&ImageFileInfo)!=D3D_OK)
		{
			ImageFileInfo.Width=3;
			ImageFileInfo.Height=3;
			bCantDisplay=true;
		}
	}
	else
	{
		ImageFileInfo.Width=3;
		ImageFileInfo.Height=3;
	}
	int nWidth;
	a=hLabel->Attrib->GetAttribute("width");
	if(a==NULL||(nWidth=a->ToInt())==0)
		nWidth=ImageFileInfo.Width;
	int nHeight;
	a=hLabel->Attrib->GetAttribute("height");
	if(a==NULL||(nHeight=a->ToInt())==0)
		nHeight=ImageFileInfo.Height;
	if(m_iFirstLineWidth+nWidth>lRect.Width())
		CreateBreak(lRect);
	if(m_pGlobalInfo->bDrawObject)
	{
		PAUIIMAGEPICTURE image=new AUIImagePicture;
		image->SetParent(BBSDlg);
		image->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
		AUIOBJECT_SETPROPERTY p;
		AString fn=node->fn;
		FILE *f=fopen(fn,"rb");
		if(f==NULL)
			fn=errorimg;
		else
		{
			fclose(f);
			fn="../"+fn;
		}
		{
			ScopedAUIControlSpriteModify _dummy(image);

			if(node->state==IMG_DOWNLOADED)
			{
				strcpy(p.fn,fn);
				if(bCantDisplay || !image->SetProperty("Image File",&p))
				{
					strcpy(p.fn,errorimg);
					if(!image->SetProperty("Image File",&p))
					{
						A3DRELEASE(image);
						return false;
					}
				}
				p.i=1;
				image->SetProperty("Frames Number",&p);
			}
			else
			{
				strcpy(p.fn,errorimg);
				if(!image->SetProperty("Image File",&p))
				{
					A3DRELEASE(image);
					return false;
				}
				p.i=1;
				image->SetProperty("Frames Number",&p);
			}
		}
		image->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
		image->SetSize(nWidth,nHeight);
		image->SetClipViewport(m_pNewViewport);
		m_pNewObjectList->AddObject(hLabel,image,this);
	}
	if(nHeight>m_iFirstLineHeight)
		m_iFirstLineHeight=nHeight;
	m_iFirstLineWidth+=nWidth;
	return true;
}

bool CECBBSPage::CreateImageLine(HTMLLabel *hLabel,int x1,int y1,int x2,int y2)
{
	PAUIIMAGEPICTURE image=new AUIImagePicture;
	image->SetParent(BBSDlg);
	image->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
	AUIOBJECT_SETPROPERTY p;
	strcpy(p.fn,"InBBS/列表框_反显.bmp");
	{
		ScopedAUIControlSpriteModify _dummy(image);
		if(!image->SetProperty("Image File",&p))
		{
			A3DRELEASE(image);
			return false;
		}
	}
	p.i=1;
	image->SetProperty("Frames Number",&p);
	if(x1==x2)
	{
		image->SetPos(x1,y1);
		image->SetSize(1,y2-y1);
	}
	else
	{
		image->SetPos(x1,y1);
		image->SetSize(x2-x1,1);
	}
	image->SetClipViewport(m_pNewViewport);
	m_pNewObjectList->AddObject(hLabel,image,this);
	return true;
}

bool CECBBSPage::CreateInput(HTMLLabel *hLabel,A3DRECT &lRect)
{
	AString *aStr=hLabel->Attrib->GetAttribute("type");
	AString a;
	if(aStr==NULL)
		a="text";
	else
		a=*aStr;
	int nHeight=0;
	int nWidth=0;
	if(a=="text"||a=="password"||a=="file")
	{
		aStr=hLabel->Attrib->GetAttribute("size");
		if(aStr==NULL||aStr->ToInt()<1)
		{
			nHeight=RealFontHeight[m_pGlobalInfo->iTextSize]+4;
			nWidth=100;
		}
		else
		{
			nHeight=RealFontHeight[m_pGlobalInfo->iTextSize]+4;
			nWidth=aStr->ToInt()*RealFontHeight[m_pGlobalInfo->iTextSize]/2+4;
		}
		if(nWidth<20)
			nWidth=20;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
		if(nWidth>lRect.Width())
			nWidth=lRect.Width();
		if(nWidth+m_iFirstLineWidth>lRect.Width())
			CreateBreak(lRect);
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;

		if(m_pGlobalInfo->bDrawObject)
		{
			PAUIEDITBOX editBox=new AUIEditBox;
			editBox->SetParent(BBSDlg);
			editBox->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			
			if(!LoadTemplateFrame(editBox))
			{
				A3DRELEASE(editBox);
				return false;
			}

			editBox->SetSize(nWidth,nHeight);
			editBox->RebuildTextRect();
			editBox->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
			editBox->SetMaxLength(250);
			editBox->SetFontAndColor(m_pGlobalInfo->aFont,FontSize[m_pGlobalInfo->iTextSize],m_pGlobalInfo->iTextColor);
			if(a=="password")
				editBox->SetIsPassword(true);
			aStr=hLabel->Attrib->GetAttribute("value");
			if(aStr!=NULL)
			{
				AString txt=*aStr;
				int l=MultiByteToWideChar(m_iCodePage,0,txt,-1,NULL,0);
				ACHAR *uCode=(ACHAR*)a_malloctemp(sizeof(ACHAR) * (l+1));
				MultiByteToWideChar(m_iCodePage,0,txt,-1,uCode,l);
				uCode[l]='\0';
				editBox->SetText(uCode);
				a_freetemp(uCode);
			}
			editBox->SetClipViewport(m_pNewViewport);
			m_pNewObjectList->AddObject(hLabel,editBox,this);
		}
		m_iFirstLineWidth+=nWidth+2;
	}
	else if(a=="submit"||a=="reset")
	{
		aStr=hLabel->Attrib->GetAttribute("value");
		ACHAR *uCode;
		if(aStr!=NULL)
		{
			AString txt=*aStr;
			int l=MultiByteToWideChar(m_iCodePage,0,txt,-1,NULL,0);
			uCode=(ACHAR*)a_malloctemp(sizeof(ACHAR) * (l+1));
			MultiByteToWideChar(m_iCodePage,0,txt,-1,uCode,l);
			uCode[l]='\0';
		}
		else
		{
			uCode=(ACHAR*)a_malloctemp(sizeof(ACHAR)*5);
			if(a=="submit")
				a_strcpy(uCode,TEXT("提交"));
			else
				a_strcpy(uCode,TEXT("重设"));
		}
		AUI_GetOneLineTextRect(PointFont[m_iNewDefaultFontSize],
						uCode,9999,nWidth,nHeight);
		if(nHeight==0)
			nHeight=m_iNewDefaultFontSize;
		nWidth+=16;
		nHeight+=12;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
		if(nWidth+m_iFirstLineWidth>lRect.Width())
			CreateBreak(lRect);
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;
		if(m_pGlobalInfo->bDrawObject)
		{
			PAUISTILLIMAGEBUTTON button=new AUIStillImageButton;
			button->SetParent(BBSDlg);
			button->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			button->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
			button->SetSize(nWidth,nHeight);
			button->SetFontAndColor(DefaultFontName,FontSize[m_iNewDefaultFontSize],DefaultFontColor);
			button->SetText(uCode);

			if(!LoadTemplateFrame(button))
			{
				a_freetemp(uCode);
				A3DRELEASE(button);
				return false;
			}

			button->SetClipViewport(m_pNewViewport);
			m_pNewObjectList->AddObject(hLabel,button,this);
		}
		a_freetemp(uCode);
		m_iFirstLineWidth+=nWidth+2;
	}
	else if(a=="checkbox")
	{
		nWidth+=20;
		nHeight+=20;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
		if(nWidth+m_iFirstLineWidth>lRect.Width())
			CreateBreak(lRect);
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;
		if(m_pGlobalInfo->bDrawObject)
		{
			PAUICHECKBOX checkbox=new AUICheckBox;
			checkbox->SetParent(BBSDlg);
			checkbox->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			aStr=hLabel->Attrib->GetAttribute("checked");
			if(aStr!=NULL)
				checkbox->Check(true);
			checkbox->SetSize(nWidth,nHeight);
			checkbox->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
			checkbox->SetFontAndColor(DefaultFontName,FontSize[m_iNewDefaultFontSize],DefaultFontColor);

			if(!LoadTemplateFrame(checkbox))
			{
				A3DRELEASE(checkbox);
				return false;
			}

			checkbox->SetClipViewport(m_pNewViewport);
			m_pNewObjectList->AddObject(hLabel,checkbox,this);
		}
		m_iFirstLineWidth+=nWidth+2;
	}
	else if(a=="radio")
	{
		nWidth+=20;
		nHeight+=20;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
		if(nWidth+m_iFirstLineWidth>lRect.Width())
			CreateBreak(lRect);
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;
		if(m_pGlobalInfo->bDrawObject)
		{
			PAUIRADIOBUTTON radio=new AUIRadioButton;
			radio->SetParent(BBSDlg);
			radio->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			radio->SetSize(nWidth,nHeight);
			radio->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
			aStr=hLabel->Attrib->GetAttribute("name");
			int i;
			if(aStr!=NULL)
			{
				i=m_iRadioGroupID-1;
				while(i>=0&&m_aGroupName[i]!=*aStr)
					i--;
				if(i==-1)
				{
					i=m_iRadioGroupID;
					m_aGroupName[i]=*aStr;
					m_iRadioButtonID[i]=0;
					m_iRadioGroupID++;
				}
				radio->SetGroupID(i);
				radio->SetButtonID(m_iRadioButtonID[i]++);
			}
			radio->SetFontAndColor(DefaultFontName,FontSize[m_iNewDefaultFontSize],DefaultFontColor);

			if(!LoadTemplateFrame(radio))
			{
				A3DRELEASE(radio);
				return false;
			}

			aStr=hLabel->Attrib->GetAttribute("checked");
			if(aStr!=NULL)
			{
				int idGroup = radio->GetGroupID();
				ObjectNode *node=m_pNewObjectList->head->next;
				while(node!=NULL)
				{
					if(	node->obj->GetType()==AUIOBJECT_TYPE_RADIOBUTTON &&
						((PAUIRADIOBUTTON)node->obj)->GetGroupID()==idGroup)
						((PAUIRADIOBUTTON)node->obj)->Check(false);
					node=node->next;
				}
				radio->Check(true);
				BBSDlg->CheckRadioButton(i,m_iRadioButtonID[i]-1);
			}
			radio->SetClipViewport(m_pNewViewport);
			m_pNewObjectList->AddObject(hLabel,radio,this);
		}
		m_iFirstLineWidth+=nWidth+2;
	}
	else if(a=="hidden")
	{
		if(m_pGlobalInfo->bDrawObject)
		{
			PAUISTILLIMAGEBUTTON button=new AUIStillImageButton;
			button->SetParent(BBSDlg);
			button->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			button->Show(false);
			button->SetClipViewport(m_pNewViewport);
			m_pNewObjectList->AddObject(hLabel,button,this);
		}
	}

	return true;
}

bool CECBBSPage::CreateParagraph(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(m_iFirstLineWidth>0)
		CreateBreak(lRect);
	if(m_bLastParagraph)
	{
		CreateBreak(lRect);
		m_bLastParagraph=false;
	}
	CreateDIV(hLabel,lRect);
	m_bLastParagraph=false;
	return true;
}

bool CECBBSPage::CreatePre(HTMLLabel *hLabel,A3DRECT &lRect)
{
	bool OldPre=m_pGlobalInfo->bPreText;
	m_pGlobalInfo->bPreText=true;
	CreateBreak(lRect);
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	m_pGlobalInfo->bPreText=OldPre;
	CreateBreak(lRect);
	return true;
}

bool CECBBSPage::CreateSelect(HTMLLabel *hLabel,A3DRECT &lRect)
{
	AString *aStr=hLabel->Attrib->GetAttribute("size");
	int nHeight;
	int nWidth;
	if(aStr==NULL||aStr->ToInt()==1)
	{
		PAUICOMBOBOX combo;
		if(m_pGlobalInfo->bDrawObject)
		{
			combo=new AUIComboBox;
			combo->SetParent(BBSDlg);
			combo->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			combo->SetFontAndColor(m_pGlobalInfo->aFont,FontSize[m_pGlobalInfo->iTextSize],m_pGlobalInfo->iTextColor);
		}
		HTMLLabel *chLabel=hLabel->FirstChildLabel;
		int maxWidth=0;
		int maxHeight=0;
		int i=0;
		while(chLabel!=NULL)
		{
			if(chLabel->LabelType=="option"&&chLabel->NextLabel!=NULL&&
				chLabel->NextLabel->LabelType=="text")
			{
				bool b=(chLabel->Attrib->GetAttribute("selected")!=NULL);
				chLabel=chLabel->NextLabel;
				ACHAR uCode[100];
				AString txt=*chLabel->Attrib->GetAttribute("text");
				AString2Unicode(txt,m_iCodePage,uCode,1000);
				int len=AUI_GetOneLineTextRect(
						PointFont[m_pGlobalInfo->iTextSize],
						uCode,9999,nWidth,nHeight);
				if( len == -1 )
					return false;
				if(nWidth>maxWidth)
					maxWidth=nWidth;
				if(nHeight>maxHeight)
					maxHeight=nHeight;
				if(m_pGlobalInfo->bDrawObject)
				{
					uCode[len]='\0';
					combo->AddString(uCode);
					combo->SetItemTextColor(i,m_pGlobalInfo->iTextColor);
					if(b)
					{
						combo->SetCurSel(i);
						char a[10];
						sprintf(a,"%d",i);
						hLabel->Attrib->SetAttribute("defaultsel",a);
					}
				}
				i++;
			}
			chLabel=chLabel->NextLabel;
		}
		nWidth=maxWidth+20;
		nHeight=maxHeight+11;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
		if(nWidth+m_iFirstLineWidth>lRect.Width())
			CreateBreak(lRect);
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;
		if(m_pGlobalInfo->bDrawObject)
		{
			combo->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
			combo->SetSize(nWidth,nHeight);

			if(!LoadTemplateFrame(combo))
			{
				A3DRELEASE(combo);
				return false;
			}

			combo->SetCommand("SelectChange");
			combo->SetClipViewport(m_pNewViewport);
			combo->SetCommand("BBSSelectChange");
			m_pNewObjectList->AddObject(hLabel,combo,this);
		}
		m_iFirstLineWidth+=nWidth+2;
	}
	else
	{
		int nSize=aStr->ToInt();
		PAUILISTBOX list;
		if(m_pGlobalInfo->bDrawObject)
		{
			list=new AUIListBox;
			list->SetParent(BBSDlg);
			list->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
			list->SetFontAndColor(m_pGlobalInfo->aFont,FontSize[m_pGlobalInfo->iTextSize],m_pGlobalInfo->iTextColor);
		}
		HTMLLabel *chLabel=hLabel->FirstChildLabel;
		int maxWidth=0;
		int maxHeight=0;
		int i=0;
		while(chLabel!=NULL)
		{
			if(chLabel->LabelType=="option"&&chLabel->NextLabel!=NULL&&
				chLabel->NextLabel->LabelType=="text")
			{
				bool b=(chLabel->Attrib->GetAttribute("selected")!=NULL);
				chLabel=chLabel->NextLabel;
				ACHAR uCode[100];
				AString txt=*chLabel->Attrib->GetAttribute("text");
				AString2Unicode(txt,m_iCodePage,uCode,1000);
				AUI_GetOneLineTextRect(
						PointFont[m_pGlobalInfo->iTextSize],
						uCode,9999,nWidth,nHeight);
				if(nWidth>maxWidth)
					maxWidth=nWidth;
				if(nHeight>maxHeight)
					maxHeight=nHeight;
				if(m_pGlobalInfo->bDrawObject)
				{
					list->AddString(uCode);
					list->SetItemTextColor(i,m_pGlobalInfo->iTextColor);
					if(b)
					{
						list->SetCurSel(i);
						char a[10];
						sprintf(a,"%d",i);
						hLabel->Attrib->SetAttribute("defaultsel",a);
					}
				}
				i++;
			}
			chLabel=chLabel->NextLabel;
		}
		nWidth=maxWidth+30;
		nHeight=maxHeight*nSize+20;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
		if(nWidth+m_iFirstLineWidth>lRect.Width())
			CreateBreak(lRect);
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;
		if(m_pGlobalInfo->bDrawObject)
		{
			list->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
			list->SetSize(nWidth,nHeight);

			if(!LoadTemplateFrame(list))
			{
				A3DRELEASE(list);
				return false;
			}

			list->SetClipViewport(m_pNewViewport);
			m_pNewObjectList->AddObject(hLabel,list,this);
		}
		m_iFirstLineWidth+=nWidth+2;
	}
	return true;
}

bool CECBBSPage::CreateTable(HTMLLabel *hLabel,A3DRECT &lRect)
{
	A3DRECT tempPos;
	HTMLTable *iOldTable=m_hTable;
	bool bOldDrawObject=m_pGlobalInfo->bDrawObject;
	int iOldAlign=m_pGlobalInfo->iAlign;
	int iOldVAlign=m_pGlobalInfo->iVAlign;
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	lRect.right--;
	if(iOldTable!=NULL)
	{
		lRect.top-=2;
		lRect.left-=2;
		lRect.right+=2;
		lRect.bottom++;
	}
	A3DRECT tempRect=lRect;
	AString *aStr;
	int i,j,k;
	m_pGlobalInfo->bDrawObject=false;
	m_hTable=m_pTableList.GetTable(hLabel);
	aStr=hLabel->Attrib->GetAttribute("border");
	bool bBorder=(aStr!=NULL&&*aStr!="0");
	if(m_hTable==NULL||lRect.Width()<m_hTable->iTableWidth)
	{
		if(m_hTable==NULL)
		{
			m_hTable=new HTMLTable;
			m_pTableList.AddTable(hLabel,m_hTable);
		}
		else
		{
			int i=0;
		}
		{
			memset(m_hTable,0,sizeof(HTMLTable));
			m_hTable->iInit=1;
			aStr=hLabel->Attrib->GetAttribute("align");
			if(aStr!=NULL)
			{
				AString a=*aStr;
				if(a=="center")
					m_hTable->iAlign=1;
				else if(a=="right")
					m_hTable->iAlign=2;
			}
			else
				m_hTable->iAlign=m_pGlobalInfo->iAlign;
			aStr=hLabel->Attrib->GetAttribute("width");
			if(aStr!=NULL)
			{
				int p=aStr->Find('%');
				if(p>=0)
					m_hTable->iTableWidth=aStr->Left(p).ToInt()*lRect.Width()/100;
				else
					m_hTable->iTableWidth=aStr->ToInt();
				if(m_hTable->iTableWidth>lRect.Width())
					m_hTable->iTableWidth=lRect.Width();
			}
			else
				m_hTable->iTableWidth=9999;
			m_hTable->tableRect=tempRect;
			lRect.left=0;
			lRect.top=0;
			if(m_hTable->iTableWidth!=9999)
				lRect.right=m_hTable->iTableWidth;
			else
				lRect.right=tempRect.Width();
			lRect.bottom=9999;
			m_iFirstLineWidth=0;
			m_iFirstLineHeight=0;
			//计算每个单元格需要的宽度
			CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
		}
//		else
//		{
//			aStr=hLabel->Attrib->GetAttribute("width");
//			if(aStr!=NULL)
//			{
//				int p=aStr->Find('%');
//				if(p>=0)
//					m_hTable->iTableWidth=aStr->Left(p).ToInt()*lRect.Width()/100;
//				else
//					m_hTable->iTableWidth=aStr->ToInt();
//				if(m_hTable->iTableWidth>lRect.Width())
//					m_hTable->iTableWidth=lRect.Width();
//			}
//			else
//				m_hTable->iTableWidth=9999;
//			memset(m_hTable->iMinWidth,0,sizeof(m_hTable->iMinWidth));
//			memset(m_hTable->iWidth,0,sizeof(m_hTable->iWidth));
//			memset(m_hTable->iHeight,0,sizeof(m_hTable->iHeight));
//			m_hTable->tableRect=tempRect;
//			lRect.left=0;
//			lRect.top=0;
//			lRect.right=m_hTable->iTableWidth;
//			lRect.bottom=9999;
//			m_iFirstLineWidth=0;
//			m_iFirstLineHeight=0;
//		}

	//	if(m_hTable->iTableWidth!=9999)
	//		m_hTable->iTableWidth-=m_hTable->iTotalCols*4;
		
		//计算每列宽度
		for(i=1;i<m_hTable->iTotalRows+1;i++)
			for(j=1;j<m_hTable->iTotalCols+1;j++)
			{
	//			if(m_hTable->iSpanRows[i][j]==0&&m_hTable->iHeight[i]<m_hTable->iTdHeight[i][j])
	//				m_hTable->iHeight[i]=m_hTable->iTdHeight[i][j];
				if(m_hTable->iSpanCols[i][j]==1&&m_hTable->iWidth[j]<m_hTable->iTdWidth[i][j])
					m_hTable->iWidth[j]=m_hTable->iTdWidth[i][j];
			}
//		for(i=1;i<m_hTable->iTotalCols+1;i++)
//			m_hTable->iMinWidth[i]=16;

		//计算每列最小宽度
		for(i=1;i<m_hTable->iTotalRows+1;i++)
			for(j=1;j<m_hTable->iTotalCols+1;j++)
				if(m_hTable->iSpanCols[i][j]==1&&m_hTable->iTdMinWidth[i][j]>m_hTable->iMinWidth[j])
					m_hTable->iMinWidth[j]=m_hTable->iTdMinWidth[i][j];

		//对宽度小于最小宽度的列，宽度设为最小宽度
		for(i=1;i<m_hTable->iTotalCols+1;i++)
			if(m_hTable->iWidth[i]<m_hTable->iMinWidth[i])
				m_hTable->iWidth[i]=m_hTable->iMinWidth[i];

		for(j=1;j<m_hTable->iTotalCols+1;j++)
		{
			m_hTable->iMinWidth[j]+=3;
			m_hTable->iWidth[j]+=3;
		}

		//对跨多列的单元格宽度不够的情况进行处理
		for(i=1;i<m_hTable->iTotalRows+1;i++)
			for(j=1;j<m_hTable->iTotalCols+1;j++)
				if(m_hTable->iSpanCols[i][j]>1)
				{
					int tw=0;
					for(k=j;k<j+m_hTable->iSpanCols[i][j];k++)
						tw+=m_hTable->iWidth[k];
					tw=m_hTable->iTdWidth[i][j]-tw;
					if(tw>0)
					{
						int incW=(tw-1)/m_hTable->iSpanCols[i][j]+1;
						for(k=j;k<j+m_hTable->iSpanCols[i][j];k++)
							m_hTable->iWidth[k]+=incW;
					}
				}
		int iTotWidth=0;
		for(i=1;i<m_hTable->iTotalCols+1;i++)
			iTotWidth+=m_hTable->iWidth[i];
		
		//如果给定了表格宽度，将宽度设定为给定宽度
		if(m_hTable->iTableWidth!=9999&&m_hTable->iTableWidth>iTotWidth)
			iTotWidth=m_hTable->iTableWidth;

		//对给定宽度的列进行设定
		for(i=1;i<m_hTable->iTotalCols+1;i++)
		{
			if(m_hTable->iDesWidth[i]>0&&m_hTable->iMinWidth[i]<m_hTable->iDesWidth[i])
				m_hTable->iWidth[i]=m_hTable->iDesWidth[i];
		}
		iTotWidth=0;
		for(i=1;i<m_hTable->iTotalCols+1;i++)
			iTotWidth+=m_hTable->iWidth[i];
		int iDesWidth=iTotWidth;
		//未给定表格总宽度时可扩展表格宽度
		if(m_hTable->iTableWidth!=9999)
		{
			for(i=1;i<m_hTable->iTotalCols+1;i++)
			{
				if(m_hTable->iDesWidth[i]<0&&
					m_hTable->iMinWidth[i]<-m_hTable->iDesWidth[i]*iTotWidth/100)
					m_hTable->iWidth[i]=-m_hTable->iDesWidth[i]*iTotWidth/100;
			}
			iTotWidth=0;
			for(i=1;i<m_hTable->iTotalCols+1;i++)
				iTotWidth+=m_hTable->iWidth[i];
			if(m_hTable->iTableWidth>iTotWidth)
				iDesWidth=m_hTable->iTableWidth;
		}
		else
		{
			for(i=1;i<m_hTable->iTotalCols+1;i++)
				if(m_hTable->iDesWidth[i]<0&&
					(m_hTable->iWidth[i]*100>-iDesWidth*m_hTable->iDesWidth[i]))
					iDesWidth=-m_hTable->iWidth[i]*100/m_hTable->iDesWidth[i];
		}
	//	m_hTable->iTableWidth=iTotWidth;
		if(iDesWidth>iTotWidth)
		{
			int restWidth=iDesWidth;
			int totNoDes=0;
			for(i=1;i<m_hTable->iTotalCols+1;i++)
				if(m_hTable->iDesWidth[i]==0)
					totNoDes+=m_hTable->iWidth[i];
				else if(m_hTable->iDesWidth[i]>0)
				{
					m_hTable->iWidth[i]=m_hTable->iDesWidth[i];
					restWidth-=m_hTable->iWidth[i];
				}
				else
				{
					m_hTable->iWidth[i]=-m_hTable->iDesWidth[i]*iDesWidth/100;
					restWidth-=m_hTable->iWidth[i];
				}
			for(i=1;i<m_hTable->iTotalCols+1;i++)
				if(m_hTable->iDesWidth[i]==0)
				{
					int iTemp=m_hTable->iWidth[i];
					m_hTable->iWidth[i]=restWidth*iTemp/totNoDes;
					totNoDes-=iTemp;
					restWidth-=m_hTable->iWidth[i];
				}
			if(restWidth>0)
			{
				iTotWidth=0;
				for(i=1;i<m_hTable->iTotalCols+1;i++)
					iTotWidth+=m_hTable->iWidth[i];
				for(i=1;i<m_hTable->iTotalCols+1;i++)
				{
					int iTemp=m_hTable->iWidth[i];
					int iTemp1=restWidth*iTemp/iTotWidth;
					m_hTable->iWidth[i]+=iTemp1;
					iTotWidth-=iTemp;
					restWidth-=iTemp1;
				}
			}
			iTotWidth=iDesWidth;
	//		m_hTable->iTableWidth=iDesWidth;
		}
		if(m_hTable->iTableWidth!=9999&&iTotWidth>m_hTable->iTableWidth)
		{
			int totNoDes=0;
			for(i=1;i<m_hTable->iTotalCols+1;i++)
				if(m_hTable->iDesWidth[i]==0)
					totNoDes+=m_hTable->iWidth[i]-m_hTable->iMinWidth[i];
			int restWidth=iTotWidth-m_hTable->iTableWidth;
			if(totNoDes!=0)
			{
				for(i=1;i<m_hTable->iTotalCols+1;i++)
					if(m_hTable->iDesWidth[i]==0)
					{
						int iTemp=restWidth*(m_hTable->iWidth[i]-m_hTable->iMinWidth[i])/totNoDes;
						if(iTemp>m_hTable->iWidth[i]-m_hTable->iMinWidth[i])
							iTemp=m_hTable->iWidth[i]-m_hTable->iMinWidth[i];
						totNoDes-=m_hTable->iWidth[i]-m_hTable->iMinWidth[i];
						m_hTable->iWidth[i]-=iTemp;
						restWidth-=iTemp;
						if(totNoDes==0)
							break;
					}
			}
			while(restWidth>0)
			{
				totNoDes=0;
				for(i=1;i<m_hTable->iTotalCols+1;i++)
					totNoDes+=m_hTable->iWidth[i]-m_hTable->iMinWidth[i];
				if(totNoDes==0)
					break;
				for(i=1;i<m_hTable->iTotalCols+1;i++)
				{
					int iTemp=restWidth*(m_hTable->iWidth[i]-m_hTable->iMinWidth[i])/totNoDes;
					if(iTemp>m_hTable->iWidth[i]-m_hTable->iMinWidth[i])
						iTemp=m_hTable->iWidth[i]-m_hTable->iMinWidth[i];
					totNoDes-=m_hTable->iWidth[i]-m_hTable->iMinWidth[i];
					m_hTable->iWidth[i]-=iTemp;
					restWidth-=iTemp;
					if(totNoDes==0)
						break;
				}
			}
			iTotWidth=m_hTable->iTableWidth+restWidth;
		}
		m_hTable->iTableWidth=iTotWidth;

		for(i=1;i<m_hTable->iTotalCols+1;i++)
			m_hTable->iWidth[i]+=m_hTable->iWidth[i-1];
		m_hTable->iInit=2;
		m_hTable->iRows=0;
		m_hTable->iCols=0;
		m_iFirstLineWidth=0;
		m_iFirstLineHeight=0;

		//计算出表格每列宽度后再计算每行高度
		CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
		for(i=1;i<m_hTable->iTotalRows+1;i++)
			if(m_hTable->iHeight[i]<m_hTable->iHeight[i-1])
				m_hTable->iHeight[i]=m_hTable->iHeight[i-1];
		m_hTable->tableRect=tempRect;
		m_hTable->tableRect.bottom=m_hTable->tableRect.top+m_hTable->iHeight[m_hTable->iTotalRows];
		m_hTable->tableRect.right=m_hTable->tableRect.left+m_hTable->iWidth[m_hTable->iTotalCols];
	}
	else
	{
		int iIncX=lRect.left-m_hTable->tableRect.left;
		int iIncY=lRect.top-m_hTable->tableRect.top;
		m_hTable->tableRect.left+=iIncX;
		m_hTable->tableRect.right+=iIncX;
		m_hTable->tableRect.top+=iIncY;
		m_hTable->tableRect.bottom+=iIncY;
	}

	m_pGlobalInfo->bDrawObject=bOldDrawObject;

	//当前表格是否需要显示，只计算表格宽度和高度时bOldDrawObject为false
	if(bOldDrawObject)
	{
		int iWidth=tempRect.Width()-m_hTable->iTableWidth;
		if(m_hTable->iAlign==1)
		{
			m_hTable->tableRect.left+=iWidth/2;
			m_hTable->tableRect.right+=iWidth/2;
		}
		else if(m_pGlobalInfo->iAlign==2)
		{
			m_hTable->tableRect.left+=iWidth;
			m_hTable->tableRect.right+=iWidth;
		}
		m_hTable->iInit=0;
		m_hTable->iRows=0;
		m_hTable->iCols=0;
		m_iFirstLineWidth=0;
		m_iFirstLineHeight=0;
//		for(i=0;i<m_hTable->iTotalRows+1;i++)
//			m_hTable->iHeight[i]+=tempRect.top;
//		for(i=0;i<m_hTable->iTotalCols+1;i++)
//			m_hTable->iWidth[i]+=tempRect.left+m_iFirstLineWidth;
		if(bBorder)
		{
			int x1,x2,y1,y2;
			for(i=2;i<m_hTable->iTotalRows+1;i++)
			{
				y1=m_hTable->iHeight[i-1]+m_hTable->tableRect.top;
				j=1;
				while(j<=m_hTable->iTotalCols)
				{
					while(j<=m_hTable->iTotalCols&&!m_hTable->bYLine[i][j])
						j++;
					if(j>m_hTable->iTotalCols)
						break;
					x1=m_hTable->iWidth[j-1]+m_hTable->tableRect.left;
					while(m_hTable->bYLine[i][j])
						j++;
					x2=m_hTable->iWidth[j-1]+m_hTable->tableRect.left;
					CreateImageLine(hLabel,x1,y1,x2,y1);
				}
			}
			for(i=2;i<m_hTable->iTotalCols+1;i++)
			{
				x1=m_hTable->iWidth[i-1]+m_hTable->tableRect.left;
				j=1;
				while(j<=m_hTable->iTotalRows)
				{
					while(j<=m_hTable->iTotalRows&&!m_hTable->bXLine[j][i])
						j++;
					if(j>m_hTable->iTotalRows)
						break;
					y1=m_hTable->iHeight[j-1]+m_hTable->tableRect.top;
					while(m_hTable->bXLine[j][i])
						j++;
					y2=m_hTable->iHeight[j-1]+m_hTable->tableRect.top;
					CreateImageLine(hLabel,x1,y1,x1,y2);
				}
			}
			CreateImageLine(hLabel,	m_hTable->tableRect.left,m_hTable->tableRect.top,
									m_hTable->tableRect.left,m_hTable->tableRect.bottom);
			CreateImageLine(hLabel,	m_hTable->tableRect.right,m_hTable->tableRect.top,
									m_hTable->tableRect.right,m_hTable->tableRect.bottom+1);
			CreateImageLine(hLabel,	m_hTable->tableRect.left,m_hTable->tableRect.top,
									m_hTable->tableRect.right,m_hTable->tableRect.top);
			CreateImageLine(hLabel,	m_hTable->tableRect.left,m_hTable->tableRect.bottom,
									m_hTable->tableRect.right,m_hTable->tableRect.bottom);
		}
		m_pLineStart=NULL;
		CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	}

	m_iFirstLineWidth=m_hTable->iWidth[m_hTable->iTotalCols]-2;
	m_iFirstLineHeight=m_hTable->iHeight[m_hTable->iTotalRows]-2;
	m_pGlobalInfo->iAlign=iOldAlign;
	m_pGlobalInfo->iVAlign=iOldVAlign;
	int iWidth=m_hTable->iWidth[m_hTable->iTotalCols];
	int iHeight=m_hTable->iHeight[m_hTable->iTotalRows];
	m_hTable=iOldTable;
	lRect=tempRect;
	lRect.right++;
	if(iOldTable!=NULL)
	{
		lRect.top+=2;
		lRect.left+=2;
		lRect.right-=2;
		lRect.bottom--;
	}
	m_pLineStart=NULL;
	CreateBreak(lRect);
//	if(m_hTable!=NULL)
//		if(m_hTable->iTdWidth[m_hTable->iRows][m_hTable->iCols]<iWidth)
//			m_hTable->iTdWidth[m_hTable->iRows][m_hTable->iCols]=iWidth;
	
	return true;
}

bool CECBBSPage::CreateText(HTMLLabel *hLabel,A3DRECT &lRect)
{
	ACHAR *uCode;
//	if(m_hTable!=NULL&&!m_hTable->iInit)
//	{
//		int nWidth,nHeight,nLines;
//		AString txt=*hLabel->Attrib->GetAttribute("text");
//		int l=MultiByteToWideChar(m_iCodePage,0,txt,-1,uCode,0);
//		MultiByteToWideChar(m_iCodePage,0,txt,-1,uCode,l);
//		uCode[l]='\0';
//		AUI_GetOneLineTextRect(g_pGame->GetA3DEngine(),
//						BBSDlg->GetAUIManager()->GetA3DFont(TEXT("方正细黑一简体"),m_pGlobalInfo->iTextSize)->GetA3DFont(),
//						uCode,nWidth,nHeight,nLines);
//		if(nHeight>m_iFirstLineHeight)
//			m_iFirstLineHeight=nHeight;
//		m_iFirstLineWidth+=nWidth;
//		return true;
//	}
	if(m_hTable!=NULL&&m_hTable->iInit==1&&RealFontHeight[m_pGlobalInfo->iTextSize]>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
		m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=(int)(RealFontHeight[m_pGlobalInfo->iTextSize]);

	if(lRect.Width()<RealFontHeight[m_pGlobalInfo->iTextSize])
		return true;
	AString *txt=hLabel->Attrib->GetAttribute("text");
	int l=MultiByteToWideChar(m_iCodePage,0,*txt,-1,NULL,0);
	uCode=(ACHAR*)a_malloctemp(sizeof(ACHAR) * (l+1));
	ACHAR *uCodeHead=uCode;
	MultiByteToWideChar(m_iCodePage,0,*txt,-1,uCode,l);
	uCode[l]='\0';
	CreateFormatText(uCode,m_pGlobalInfo->bPreText);
	if(uCode[0]=='\0')
	{
		a_freetemp(uCodeHead);
		return true;
	}
	if(m_bLastParagraph)
	{
		CreateBreak(lRect);
		m_bLastParagraph=false;
	}
	A3DCOLOR aTextColor;
	if(	m_hHrefLabel!=NULL)
		aTextColor=hrefColor;
	else
		aTextColor=m_pGlobalInfo->iTextColor;
	while(uCode[0]!='\0')
	{
		int nWidth,nHeight;
		int MaxLen=AUI_GetOneLineTextRect(
						PointFont[m_pGlobalInfo->iTextSize],
						uCode,lRect.Width()-m_iFirstLineWidth,nWidth,nHeight);
		if( MaxLen == -1 )
		{
			a_freetemp(uCodeHead);
			return false;
		}
		if(nHeight>m_iFirstLineHeight)
			m_iFirstLineHeight=nHeight;
		if(uCode[MaxLen]!='\0')
		{
			if(MaxLen==0)
				break;

//			int MaxLen=AUI_GetTextMaxLength(g_pGame->GetA3DEngine(),
//								BBSDlg->GetAUIManager()->GetA3DFont(m_pGlobalInfo->aFont,m_pGlobalInfo->iTextSize)->GetA3DFont(),
//								uCode,lRect.Width()-m_iFirstLineWidth,nWidth);
			ACHAR ch=uCode[MaxLen];
			uCode[MaxLen]='\0';
			if(m_pGlobalInfo->bDrawObject)
			{
				PAUILABEL label=new AUILabel;
				label->SetParent(BBSDlg);
				label->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
				label->SetText(uCode);
				label->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
				label->SetFontAndColor(m_pGlobalInfo->aFont,FontSize[m_pGlobalInfo->iTextSize],aTextColor);
				label->SetSize(0,0);
				label->SetClipViewport(m_pNewViewport);
				m_pNewObjectList->AddObject(hLabel,label,this);
			}
			uCode+=MaxLen;
			uCode[0]=ch;
			if(nHeight>m_iFirstLineHeight)
				m_iFirstLineHeight=nHeight;
			m_iFirstLineWidth+=nWidth;
			CreateBreak(lRect);
			if(uCode[0]=='\n')
				uCode++;
		}
		else
		{
			if(m_pGlobalInfo->bDrawObject)
			{
				PAUILABEL label=new AUILabel;
				label->SetParent(BBSDlg);
				label->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
				label->SetText(uCode);
				label->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
				label->SetFontAndColor(m_pGlobalInfo->aFont,FontSize[m_pGlobalInfo->iTextSize],aTextColor);
				label->SetSize(0,0);
				label->SetClipViewport(m_pNewViewport);
				m_pNewObjectList->AddObject(hLabel,label,this);
			}
			if(nHeight>m_iFirstLineHeight)
				m_iFirstLineHeight=nHeight;
			m_iFirstLineWidth+=nWidth;
			break;
		}
	}
	a_freetemp(uCodeHead);
	return true;
}

bool CECBBSPage::CreateTextArea(HTMLLabel *hLabel,A3DRECT &lRect)
{
	int nHeight=0;
	int nWidth=0;
	PAUIEDITBOX editBox;
	if(m_pGlobalInfo->bDrawObject)
	{
		editBox=new AUIEditBox;
		editBox->SetParent(BBSDlg);
		editBox->Init(g_pGame->GetA3DEngine(),g_pGame->GetA3DDevice(),NULL);
	}
	AString *aStr;
	aStr=hLabel->Attrib->GetAttribute("cols");
	if(aStr==NULL||aStr->ToInt()<1)
		nWidth=500;
	else
		nWidth=(aStr->ToInt()+1)*(RealFontHeight[m_pGlobalInfo->iTextSize]/2+2);
	if(nWidth>lRect.Width())
		nWidth=lRect.Width();
	aStr=hLabel->Attrib->GetAttribute("rows");
	if(aStr==NULL||aStr->ToInt()<2)
		nHeight=(RealFontHeight[m_pGlobalInfo->iTextSize]+8)*2+2;
	else
		nHeight=aStr->ToInt()*(RealFontHeight[m_pGlobalInfo->iTextSize]+8)+2;
	if(m_hTable!=NULL&&m_hTable->iInit==1&&nWidth>m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols])
		m_hTable->iTdMinWidth[m_hTable->iRows][m_hTable->iCols]=nWidth;
	if(nWidth+m_iFirstLineWidth>lRect.Width())
		CreateBreak(lRect);
	if(nHeight>m_iFirstLineHeight)
		m_iFirstLineHeight=nHeight;
	if(m_pGlobalInfo->bDrawObject)
	{
		if(!LoadTemplateFrame(editBox))
		{
			A3DRELEASE(editBox);
			return false;
		}

		editBox->SetSize(nWidth,nHeight);
		editBox->RebuildTextRect();
		editBox->SetPos(lRect.left+m_iFirstLineWidth,lRect.top);
		editBox->SetFontAndColor(m_pGlobalInfo->aFont,FontSize[m_pGlobalInfo->iTextSize],m_pGlobalInfo->iTextColor);
		editBox->SetIsMultipleLine(true);
		editBox->SetIsAutoReturn(true);
		editBox->SetMaxLength(250);
		if(hLabel->FirstChildLabel!=NULL&&hLabel->FirstChildLabel->LabelType=="text")
		{
			aStr=hLabel->FirstChildLabel->Attrib->GetAttribute("text");
			if(aStr!=NULL)
			{
				AString txt=*aStr;
				int l=MultiByteToWideChar(m_iCodePage,0,txt,-1,NULL,0);
				ACHAR *uCode=(ACHAR*)a_malloctemp(sizeof(ACHAR) * (l+1));
				MultiByteToWideChar(m_iCodePage,0,txt,-1,uCode,l);
				uCode[l]='\0';
				editBox->SetText(uCode);
				a_freetemp(uCode);
			}
		}
		editBox->SetClipViewport(m_pNewViewport);
		m_pNewObjectList->AddObject(hLabel,editBox,this);
	}
	m_iFirstLineWidth+=nWidth+2;
	return true;
}

bool CECBBSPage::CreateTD(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(m_hTable==NULL)
		return true;
	if(m_hTable->iRows==0)
	{
		m_hTable->iRows++;
		m_hTable->iCols=0;
		if(m_hTable!=NULL&&m_hTable->iInit==1&&m_hTable->iIncCol[m_hTable->iRows][0]==0)
			m_hTable->iIncCol[m_hTable->iRows][0]=1;
		if(m_hTable->iRows>m_hTable->iTotalRows)
			m_hTable->iTotalRows=m_hTable->iRows;
		if(m_hTable!=NULL&&m_hTable->iInit==2)
			if(m_hTable->iHeight[m_hTable->iRows-1]>m_hTable->iHeight[m_hTable->iRows])
				m_hTable->iHeight[m_hTable->iRows]=m_hTable->iHeight[m_hTable->iRows-1];
		CreateTD(hLabel,lRect);
		return true;
	}
	m_bLastParagraph=false;
	A3DRECT tempRect=lRect;
	int iOldAlign=m_pGlobalInfo->iAlign;
	int iOldVAlign=m_pGlobalInfo->iVAlign;
	AString *aStr;
	if(m_hTable!=NULL&&m_hTable->iInit==1)
	{
		m_hTable->iCols+=m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols];
		if(m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols]==0)
			m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols]=1;
		AString *lpStr;
		lpStr=hLabel->Attrib->GetAttribute("colspan");
		int cols=1;
		if(m_hTable!=NULL&&lpStr!=NULL)
		{
			m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]=lpStr->ToInt();
			m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols]=lpStr->ToInt();
			cols=lpStr->ToInt();
		}
		if(m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]==0)
			m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]=1;
		aStr=hLabel->Attrib->GetAttribute("width");
		if(m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]==1)
		{
			if(aStr!=NULL)
			{
				int p=aStr->Find('%');
				if(p>=0)
					m_hTable->iDesWidth[m_hTable->iCols]=-(aStr->Left(p).ToInt());
				else
					m_hTable->iDesWidth[m_hTable->iCols]=aStr->ToInt();
			}
			if(m_hTable->iDesWidth[m_hTable->iCols]<0)
				lRect.right=-lRect.Width()*m_hTable->iDesWidth[m_hTable->iCols]/100;
			else if(m_hTable->iDesWidth[m_hTable->iCols]>0)
				lRect.right=m_hTable->iDesWidth[m_hTable->iCols];
		}
		lpStr=hLabel->Attrib->GetAttribute("rowspan");
		if(m_hTable!=NULL&&lpStr!=NULL)
		{
			m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]=lpStr->ToInt();
			for(int i=m_hTable->iRows+1;i<m_hTable->iRows+lpStr->ToInt();i++)
			{
				int k=m_hTable->iCols-2;
				while((k>=0)&&(m_hTable->iIncCol[i][k]!=m_hTable->iCols-k))
					k--;
				if(k==-1)
					m_hTable->iIncCol[i][m_hTable->iCols-1]=cols+1;
				else
					m_hTable->iIncCol[i][k]=m_hTable->iCols-k+cols;
			}
		}
		if(m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]==0)
			m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]=1;
		lRect.top+=2;
		lRect.left+=2;
		lRect.bottom--;
		lRect.right--;
		m_iFirstLineWidth=0;
		m_iFirstLineHeight=0;
		if(m_hTable->iCols+m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]-1>m_hTable->iTotalCols)
			m_hTable->iTotalCols=m_hTable->iCols+m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]-1;
		if(m_hTable->iRows+m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]-1>m_hTable->iTotalRows)
			m_hTable->iTotalRows=m_hTable->iRows+m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]-1;
	}
	else
	{
		m_hTable->iCols+=m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols];
		lRect.top=m_hTable->tableRect.top+m_hTable->iHeight[m_hTable->iRows-1];
		int i=m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols];
		lRect.bottom=m_hTable->tableRect.top+m_hTable->iHeight[m_hTable->iRows+i-1];
		lRect.left=m_hTable->tableRect.left+m_hTable->iWidth[m_hTable->iCols-1];
		int col=m_hTable->iCols+m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols];
		lRect.right=m_hTable->tableRect.left+m_hTable->iWidth[col-1];
		lRect.top+=2;
		lRect.left+=2;
		lRect.bottom--;
		lRect.right--;
		m_iFirstLineWidth=0;
		m_iFirstLineHeight=0;
		aStr=hLabel->Attrib->GetAttribute("align");
		if(aStr!=NULL)
		{
			AString a=*aStr;
			if(a=="left")
				m_pGlobalInfo->iAlign=0;
			else if(a=="center")
				m_pGlobalInfo->iAlign=1;
			else if(a=="right")
				m_pGlobalInfo->iAlign=2;
		}
//		else
//			m_pGlobalInfo->iAlign=0;
		aStr=hLabel->Attrib->GetAttribute("valign");
		if(aStr!=NULL)
		{
			AString a=*aStr;
			if(a=="top")
				m_pGlobalInfo->iVAlign=1;
			else if(a=="middle")
				m_pGlobalInfo->iVAlign=0;
			else if(a=="bottom")
				m_pGlobalInfo->iVAlign=2;
		}
//		else
//			m_pGlobalInfo->iVAlign=0;
		if(m_pGlobalInfo->iAlign!=0)
		{
			m_bBreak=true;
			m_pLineStart=NULL;
		}
		if(m_hTable!=NULL&&m_hTable->iInit==0)
		{
			int iHeight=m_hTable->iTdHeight[m_hTable->iRows][m_hTable->iCols];
			if(m_pGlobalInfo->iVAlign==0)
				lRect.top+=(lRect.Height()-iHeight)/2+2;
			else if(m_pGlobalInfo->iVAlign==2)
				lRect.top+=lRect.Height()-iHeight+2;
		}
	}
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	if(m_iFirstLineWidth!=0)
		CreateBreak(lRect);
	m_pGlobalInfo->iAlign=iOldAlign;
	m_pGlobalInfo->iVAlign=iOldVAlign;
	if(m_hTable->iInit==1)
	{
		if(m_iFirstLineWidth>m_hTable->iTdWidth[m_hTable->iRows][m_hTable->iCols])
			m_hTable->iTdWidth[m_hTable->iRows][m_hTable->iCols]=m_iFirstLineWidth;
//		if(lRect.top+m_iFirstLineHeight>m_hTable->iTdHeight[m_hTable->iRows][m_hTable->iCols])
//			m_hTable->iTdHeight[m_hTable->iRows][m_hTable->iCols]=lRect.top+m_iFirstLineHeight;
	}
	else
	{
		if(m_hTable->iInit==2)
		{
			int i;
			int iHeight=lRect.top-m_hTable->tableRect.top+3;
			int iSpanRows=m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols];
			int iSpanCols=m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols];
			m_hTable->iTdHeight[m_hTable->iRows][m_hTable->iCols]=
				lRect.top-m_hTable->tableRect.top-m_hTable->iHeight[m_hTable->iRows-1];

			for(i=m_hTable->iRows;i<m_hTable->iRows+iSpanRows;i++)
			{
				m_hTable->bXLine[i][m_hTable->iCols]=true;
				m_hTable->bXLine[i][m_hTable->iCols+iSpanCols]=true;
			}
			if(iHeight>m_hTable->iHeight[m_hTable->iRows+iSpanRows-1])
				m_hTable->iHeight[m_hTable->iRows+iSpanRows-1]=iHeight;

			for(i=m_hTable->iCols;i<m_hTable->iCols+iSpanCols;i++)
				m_hTable->bYLine[m_hTable->iRows+iSpanRows][i]=true;
		}
	}
	lRect=tempRect;
	return true;
}

bool CECBBSPage::CreateTR(HTMLLabel *hLabel,A3DRECT &lRect)
{
	if(m_hTable==NULL)
		return true;
	m_hTable->iRows++;
	m_hTable->iCols=0;
	if(m_hTable!=NULL&&m_hTable->iInit==1&&m_hTable->iIncCol[m_hTable->iRows][0]==0)
		m_hTable->iIncCol[m_hTable->iRows][0]=1;
	if(m_hTable->iRows>m_hTable->iTotalRows)
		m_hTable->iTotalRows=m_hTable->iRows;
	if(m_hTable!=NULL&&m_hTable->iInit==2)
		if(m_hTable->iHeight[m_hTable->iRows-1]>m_hTable->iHeight[m_hTable->iRows])
			m_hTable->iHeight[m_hTable->iRows]=m_hTable->iHeight[m_hTable->iRows-1];
	AString *aStr;
	int iOldAlign=m_pGlobalInfo->iAlign;
	int iOldVAlign=m_pGlobalInfo->iVAlign;
	aStr=hLabel->Attrib->GetAttribute("align");
	if(aStr!=NULL)
	{
		AString a=*aStr;
		if(a=="left")
			m_pGlobalInfo->iAlign=0;
		else if(a=="center")
			m_pGlobalInfo->iAlign=1;
		else if(a=="right")
			m_pGlobalInfo->iAlign=2;
	}
	else
		m_pGlobalInfo->iAlign=0;
	aStr=hLabel->Attrib->GetAttribute("valign");
	if(aStr!=NULL)
	{
		AString a=*aStr;
		if(a=="top")
			m_pGlobalInfo->iVAlign=1;
		else if(a=="middle")
			m_pGlobalInfo->iVAlign=0;
		else if(a=="bottom")
			m_pGlobalInfo->iVAlign=2;
	}
	else
		m_pGlobalInfo->iVAlign=0;
	CreateHTMLLabel(hLabel->FirstChildLabel,lRect);
	m_pGlobalInfo->iAlign=iOldAlign;
	m_pGlobalInfo->iVAlign=iOldVAlign;
	return true;
}

bool CECBBSPage::GetTableWidth(HTMLLabel *hLabel)
{
	while(hLabel!=NULL)
	{
		if(hLabel->LabelType=="tr")
		{
			m_hTable->iRows++;
			m_hTable->iCols=0;
			if(m_hTable->iIncCol[m_hTable->iRows][0]==0)
				m_hTable->iIncCol[m_hTable->iRows][0]=1;
			if(m_hTable->iRows>m_hTable->iTotalRows)
				m_hTable->iTotalRows=m_hTable->iRows;
			GetTableWidth(hLabel->FirstChildLabel);
		}
		else if(hLabel->LabelType=="td")
		{
			if(m_hTable->iRows==0)
			{
				m_hTable->iRows++;
				m_hTable->iCols=0;
				if(m_hTable->iIncCol[m_hTable->iRows][0]==0)
					m_hTable->iIncCol[m_hTable->iRows][0]=1;
				if(m_hTable->iRows>m_hTable->iTotalRows)
					m_hTable->iTotalRows=m_hTable->iRows;
				GetTableWidth(hLabel);
				return true;
			}
			AString *aStr;
			m_hTable->iCols+=m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols];
			if(m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols]==0)
				m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols]=1;
			AString *lpStr;
			lpStr=hLabel->Attrib->GetAttribute("colspan");
			int cols=1;
			if(lpStr!=NULL)
			{
				cols=lpStr->ToInt();
				m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]=cols;
				m_hTable->iIncCol[m_hTable->iRows][m_hTable->iCols]=cols;
			}
			aStr=hLabel->Attrib->GetAttribute("width");
			if(aStr!=NULL&&m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]==1
				//&&m_hTable->iDesWidth[m_hTable->iCols]==0
				)
			{
				int p=aStr->Find('%');
				if(p>=0)
					m_hTable->iDesWidth[m_hTable->iCols]=-(aStr->Left(p).ToInt());
				else
					m_hTable->iDesWidth[m_hTable->iCols]=aStr->ToInt();
			}
			lpStr=hLabel->Attrib->GetAttribute("rowspan");
			if(m_hTable!=NULL&&lpStr!=NULL)
			{
				m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]=lpStr->ToInt();
				for(int i=m_hTable->iRows+1;i<m_hTable->iRows+lpStr->ToInt();i++)
				{
					int k=m_hTable->iCols-2;
					while((k>=0)&&(m_hTable->iIncCol[i][k]!=m_hTable->iCols-k))
						k--;
					if(k==-1)
						m_hTable->iIncCol[i][m_hTable->iCols-1]=cols+1;
					else
						m_hTable->iIncCol[i][k]=m_hTable->iCols-k+cols;
				}
			}
			if(m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]==0)
				m_hTable->iSpanRows[m_hTable->iRows][m_hTable->iCols]=1;
			if(m_hTable->iCols+m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]-1>m_hTable->iTotalCols)
				m_hTable->iTotalCols=m_hTable->iCols+m_hTable->iSpanCols[m_hTable->iRows][m_hTable->iCols]-1;
		}
		else
			GetTableWidth(hLabel->FirstChildLabel);
		hLabel=hLabel->NextLabel;
	}
	return true;
}

//=========================================CECBBSPageList====================================
CECBBSPageList::CECBBSPageList()
{
	head=new CECBBSPageNode;
	head->next=NULL;
	head->page=NULL;
}

CECBBSPageList::~CECBBSPageList()
{
	CECBBSPageNode *node;
	while(head!=NULL)
	{
		node=head;
		head=head->next;
		if(node->page!=NULL)
			delete node->page;
		delete node;
	}
}

CECBBSPage* CECBBSPageList::CreatePage(CECBBSPage *parentPage)
{
	CECBBSPageNode *node=new CECBBSPageNode;
	CECBBSPageNode *p=head;
	node->next=NULL;
	node->page=new CECBBSPage(parentPage);
	while(p->next!=NULL)
		p=p->next;
	p->next=node;
	return node->page;
}

CECBBSPage* CECBBSPageList::FindPage(AString aName)
{
	CECBBSPageNode *node=head->next;
	while(node!=NULL)
	{
		if(aName==node->page->m_aName)
			return node->page;
		node=node->next;
	}
	node=head->next;
	CECBBSPage *res;
	while(node!=NULL)
	{
		if((res=node->page->m_pChPageList->FindPage(aName))!=NULL)
			return res;
		node=node->next;
	}
	return NULL;
}

CECBBSPage* CECBBSPageList::GetPageFromCursor(int x,int y)
{
	CECBBSPageNode *node=head->next;
	while(node!=NULL)
	{
		CECBBSPage *page=node->page->GetPageFromCursor(x,y);
		if(page!=NULL)
			return page;
		node=node->next;
	}
	return NULL;
}

bool CECBBSPageList::PrepareNewPage()
{
	bool bNoLoading=true;
	CECBBSPageNode *node=head->next;
	while(node!=NULL)
	{
		bNoLoading=bNoLoading&&node->page->PrepareNewPage();
		node=node->next;
	}
	return bNoLoading;
}

bool CECBBSPageList::LoadNewImage()
{
	CECBBSPageNode *node=head->next;
	while(node!=NULL)
	{
		node->page->LoadNewImage();
		node=node->next;
	}
	return true;
}

ObjectNode* CECBBSPageList::FindObject(PAUIOBJECT pObj)
{
	CECBBSPageNode *node=head->next;
	ObjectNode *res;
	while(node!=NULL)
	{
		res=node->page->FindObject(pObj);
		if(res!=NULL)
			return res;
		node=node->next;
	}
	return NULL;
}

void CECBBSPageList::ScrollPage(int iInc)
{
	CECBBSPageNode *node=head->next;
	while(node!=NULL)
	{
		node->page->ScrollPage(iInc);
		node=node->next;
	}
}

//=========================================ObjectList====================================
ObjectList::ObjectList()
{
	head=new ObjectNode;
	head->next=NULL;
	head->label=NULL;
	head->obj=NULL;
	head->page=NULL;
	tail=head;
}

ObjectList::~ObjectList()
{
	ObjectNode *node;
	while(head!=NULL)
	{
		node=head;
		head=head->next;
		A3DRELEASE(node->obj);
		delete node;
	}
}

void ObjectList::Release()
{
	ObjectNode *node;
	PAUIOBJECT captureObj=BBSDlg->GetCaptureObject();
	PAUIOBJECT focusObj=BBSDlg->GetFocus();
	while(head->next!=NULL)
	{
		node=head->next;
		head->next=node->next;
		if(node->obj==captureObj || node->obj==focusObj)
			BBSDlg->ChangeFocus(NULL);
		if(node->obj==HoverObject)
			HoverObject=NULL;
		if(node->obj!=NULL)
			BBSDlg->DeleteControl(node->obj);
		delete node;
	}
	tail=head;
}

void ObjectList::AddObject(HTMLLabel *label,PAUIOBJECT pObj,CECBBSPage *hPage)
{
	ObjectNode *node=new ObjectNode;
	ObjectNode *p=head;
	pObj->SetDataPtr(node);
	node->next=NULL;
	node->obj=pObj;
	node->label=label;
	node->page=hPage;
	tail->next=node;
	tail=node;
	if(hPage->m_bBreak)
	{
		hPage->m_bBreak=false;
		hPage->m_pLineStart=node;
	}
}

ObjectNode* ObjectList::FindObject(PAUIOBJECT pObj)
{
	ObjectNode *node=head->next;
	while(node!=NULL)
	{
		if(node->obj==pObj)
			return node;
		node=node->next;
	}
	return NULL;
}

ObjectNode* ObjectList::FindObject(HTMLLabel *label)
{
	ObjectNode *node=head->next;
	while(node!=NULL)
	{
		if(node->label==label)
			return node;
		node=node->next;
	}
	return NULL;
}

bool ObjectList::LoadNewImage()
{
	ObjectNode *node=head->next;
	ObjectNode *lastNode=head;
	while(node!=NULL)
	{
		if(node->obj->GetType()==AUIOBJECT_TYPE_IMAGEPICTURE)
		{
			AUIOBJECT_SETPROPERTY p;
			PAUIIMAGEPICTURE image=(PAUIIMAGEPICTURE)node->obj;
			image->GetProperty("Image File",&p);
			if(errorimg==p.fn||p.fn[0]=='\0')
			{
				AString *a=node->label->Attrib->GetAttribute("imgindex");
				if(a!=NULL)
				{
					ImageInfo *inode=ImgList.GetImageInfo(a->ToInt());
					if(inode!=NULL&&inode->state==IMG_DOWNLOADED)
					{
						AString fn=inode->fn;
						FILE *f=fopen(fn,"rb");
						if(f==NULL)
							fn=errorimg;
						else
						{
							fclose(f);
							fn="../"+fn;
						}
						strcpy(p.fn,fn);
						ScopedAUIControlSpriteModify _dummy(image);
						if(!image->SetProperty("Image File",&p))
						{
							strcpy(p.fn,errorimg);
							if(!image->SetProperty("Image File",&p))
							{
								lastNode->next=node->next;
								delete node;
								node=lastNode;
								BBSDlg->DeleteControl(image);
							}
						}
					}
				}
			}
		}
		lastNode=node;
		node=node->next;
	}
	return true;
}

void ObjectList::SetLabelColor(HTMLLabel *hLabel,A3DCOLOR aTextColor)
{
	ObjectNode *node=head->next;
	while(node!=NULL)
	{
		if(node->label->LabelType=="text")
		{
			HTMLLabel *pLabel=node->label;
			while(pLabel!=NULL&&pLabel->LabelType!="a")
				pLabel=pLabel->ParentLabel;
			if(pLabel==hLabel)
				node->obj->SetColor(aTextColor);
		}
		node=node->next;
	}
}

void ObjectList::ScrollPage(int iInc)
{
	ObjectNode *node=head->next;
	while(node!=NULL)
	{
		if(	node->label->LabelType!="PageUpButton"&&
			node->label->LabelType!="PageDownButton"&&
			node->label->LabelType!="PageScroll")
		{
			POINT pt=node->obj->GetPos(true);
			pt.y+=iInc;
			node->obj->SetPos(pt.x,pt.y);
		}
		node=node->next;
	}
}

//=========================================TableList====================================
TableList::TableList()
{
	head=new TableListNode;
	head->next=NULL;
	head->table=NULL;
	head->label=NULL;
}

TableList::~TableList()
{
	TableListNode *node;
	while(head!=NULL)
	{
		node=head;
		head=head->next;
		delete node->table;
		delete node;
	}
}

void TableList::AddTable(HTMLLabel *hLabel,HTMLTable *hTable)
{
	TableListNode *node=head->next;
	while(node!=NULL)
	{
		if(node->label==hLabel)
			if(hTable==node->table)
				return;
			else
			{
				delete node->table;
				node->table=hTable;
				return;
			}
		node=node->next;
	}
	node=new TableListNode;
	node->label=hLabel;
	node->table=hTable;
	node->next=head->next;
	head->next=node;
}

HTMLTable* TableList::GetTable(HTMLLabel *hLabel)
{
	TableListNode *node=head->next;
	while(node!=NULL)
	{
		if(node->label==hLabel)
			return node->table;
		node=node->next;
	}
	return NULL;
}

void TableList::ReleaseTables()
{
	TableListNode *node;
	while(head->next!=NULL)
	{
		node=head->next;
		head->next=node->next;
		delete node->table;
		delete node;
	}
	head->next=NULL;
}

DownloadedPagesInfo::DownloadedPagesInfo(CRITICAL_SECTION* cs)
:parm(NULL)
,m_pCS(cs)
,m_Counter(1) // must use Release to delete the object
{

}

// not thread safe
void DownloadedPagesInfo::AddRef()
{
	m_Counter++;
	ASSERT(m_Counter > 0);
}

void DownloadedPagesInfo::Release()
{
	m_Counter--;
	ASSERT(m_Counter >= 0);

	if(m_Counter == 0)
	{
		delete this;
	}
}

// thread safe, but cs must be available until info is deleted
void DownloadedPagesInfo::AddRefInThread()
{
	ASSERT(m_pCS != NULL);
	
	::EnterCriticalSection(m_pCS);
	AddRef();
	::LeaveCriticalSection(m_pCS);
}

void DownloadedPagesInfo::ReleaseInThread()
{
	ASSERT(m_pCS != NULL);

	// make a copy because 'this' pointer might be deleted
	CRITICAL_SECTION* cs = m_pCS;
	::EnterCriticalSection(cs);
	Release();
	::LeaveCriticalSection(cs);
}

//=================================DownloadedPagesManager=============================
DownloadedPagesManager::DownloadedPagesManager()
{
	m_nTotal=-1;
	::InitializeCriticalSection(&m_CS);
}

DownloadedPagesManager::~DownloadedPagesManager()
{
	bool bError = false;
	while (!m_CSUsers.empty())
	{
		int nCount = min(m_CSUsers.size(), MAXIMUM_WAIT_OBJECTS);
		if (WAIT_OBJECT_0 != WaitForMultipleObjects(nCount, m_CSUsers.begin(), TRUE, INFINITE))
		{
			//	有错误发生
			bError = true;
		}

		for (CSUsers::iterator it = m_CSUsers.begin(); it != m_CSUsers.begin()+nCount; ++ it)
		{
			//	关闭线程句柄
			::CloseHandle(*it);
		}

		//	删除当前等待对象
		m_CSUsers.erase(m_CSUsers.begin(), m_CSUsers.begin() + nCount);
	}
	
	if (!bError)
	{
		//	所有使用者都退出后
		::DeleteCriticalSection(&m_CS);
	}
}

void DownloadedPagesManager::Init()
{
	m_nTotal=0;
	m_nMax=100;
	m_DownloadedPages=new DownloadedPagesInfo*[m_nMax];
}

void DownloadedPagesManager::Release()
{
	for(int i=0;i<m_nTotal;i++)
	{
		if(m_DownloadedPages[i]->downloaded)
		{
			if(m_DownloadedPages[i]->parm!=NULL)
				delete []m_DownloadedPages[i]->parm;
		}

		m_DownloadedPages[i]->ReleaseInThread();
	}
	delete []m_DownloadedPages;
}

AString DownloadedPagesManager::GetPagesLocalFn(int index)
{
	ASSERT(index>=0 && index<m_nTotal);
	return m_DownloadedPages[index]->localfn;
}

AString DownloadedPagesManager::GetPagesURL(int index)
{
	ASSERT(index>=0 && index<m_nTotal);
	return m_DownloadedPages[index]->url;
}

DownloadedPagesInfo* DownloadedPagesManager::GetPagesInfo(int index)
{
	ASSERT(index>=0 && index<m_nTotal);
	return m_DownloadedPages[index];
}

int DownloadedPagesManager::AddPage(AString url,CECBBSPage *page)
{
	if(m_nTotal==-1)
		return -1;
	if(m_nTotal>=m_nMax)
	{
		DownloadedPagesInfo** oldPages=m_DownloadedPages;
		m_nMax+=100;
		m_DownloadedPages=new DownloadedPagesInfo*[m_nMax];
		for(int i=0;i<m_nTotal;i++)
			m_DownloadedPages[i]=oldPages[i];
		delete []oldPages;
	}
	m_DownloadedPages[m_nTotal]=new DownloadedPagesInfo(&m_CS);
	char s[100];
	sprintf(s,"BBSPages/temp%d.htm",m_nTotal);
	UINT t;
	m_DownloadedPages[m_nTotal]->localfn=s;
	m_DownloadedPages[m_nTotal]->url=url;
	m_DownloadedPages[m_nTotal]->downloaded=false;
	m_DownloadedPages[m_nTotal]->totalparm=0;
	m_DownloadedPages[m_nTotal]->parm=NULL;
	if( url.Left(7) == "http://" )
	{
		DownloadThreadData *data=new DownloadThreadData;
		data->bGet=page->m_bGet;
		data->parm=page->m_aParm;
		data->parmtype=page->m_iContentType;
		data->pageinfo=m_DownloadedPages[m_nTotal];
		data->pageinfo->AddRef(); // used in download thread, must add ref counter
		m_DownloadedPages[m_nTotal]->downloadthread=(HANDLE)_beginthreadex(NULL,0,DownloadPage,data,0,&t);

		if (m_DownloadedPages[m_nTotal]->downloadthread)
		{
			//	保存线程句柄用于退出等待
			m_CSUsers.push_back(m_DownloadedPages[m_nTotal]->downloadthread);
		}
	}
	else
	{
		CopyFileA(url, s, FALSE);
		m_DownloadedPages[m_nTotal]->downloaded=true;
	}
	int ret=m_nTotal++;
	return ret;
}

void DownloadedPagesManager::SaveParm(int index,CECBBSPage *page)
{
	if(index<0 || index>=m_nTotal || page->m_pObjectList==NULL)
		return;
	DownloadedPagesInfo *pageinfo=m_DownloadedPages[index];
	pageinfo->totalparm=0;
	ObjectNode *ObjNode=page->m_pObjectList->head->next;
	while(ObjNode!=NULL)
	{
		int type=ObjNode->obj->GetType();
		if( type==AUIOBJECT_TYPE_COMBOBOX ||
			type==AUIOBJECT_TYPE_LISTBOX ||
			type==AUIOBJECT_TYPE_EDITBOX ||
			type==AUIOBJECT_TYPE_CHECKBOX ||
			type==AUIOBJECT_TYPE_RADIOBUTTON
			)
			pageinfo->totalparm++;
		ObjNode=ObjNode->next;
	}
	if(pageinfo->parm!=NULL)
		delete []pageinfo->parm;
	pageinfo->parm=new AWString[pageinfo->totalparm];
	int i;
	int sum=0;
	ObjNode=page->m_pObjectList->head->next;
	while(ObjNode!=NULL)
	{
		int type=ObjNode->obj->GetType();
		switch(type)
		{
		case AUIOBJECT_TYPE_COMBOBOX:
			i=((PAUICOMBOBOX)ObjNode->obj)->GetCurSel();
			pageinfo->parm[sum].Format(L"%d",i);
			sum++;
			break;
		case AUIOBJECT_TYPE_LISTBOX:
			i=((PAUILISTBOX)ObjNode->obj)->GetCurSel();
			pageinfo->parm[sum].Format(L"%d",i);
			sum++;
			break;
		case AUIOBJECT_TYPE_EDITBOX:
			pageinfo->parm[sum]=ObjNode->obj->GetText();
			sum++;
			break;
		case AUIOBJECT_TYPE_CHECKBOX:
			if(((PAUICHECKBOX)ObjNode->obj)->IsChecked())
				pageinfo->parm[sum]=L"1";
			else
				pageinfo->parm[sum]=L"0";
			sum++;
			break;
		case AUIOBJECT_TYPE_RADIOBUTTON:
			if(((PAUIRADIOBUTTON)ObjNode->obj)->IsChecked())
				pageinfo->parm[sum]=L"1";
			else
				pageinfo->parm[sum]=L"0";
			sum++;
			break;
		}
		ObjNode=ObjNode->next;
	}
}

void DownloadedPagesManager::LoadParm(int index,CECBBSPage *page)
{
	if(index<0 || index>=m_nTotal || page->m_pObjectList==NULL)
		return;
	DownloadedPagesInfo *pageinfo=m_DownloadedPages[index];
	int sum=0;
	ObjectNode *ObjNode=page->m_pNewObjectList->head->next;
	while(ObjNode!=NULL && sum<pageinfo->totalparm)
	{
		int type=ObjNode->obj->GetType();
		switch(type)
		{
		case AUIOBJECT_TYPE_COMBOBOX:
			((PAUICOMBOBOX)ObjNode->obj)->SetCurSel(pageinfo->parm[sum].ToInt());
			sum++;
			break;
		case AUIOBJECT_TYPE_LISTBOX:
			((PAUILISTBOX)ObjNode->obj)->SetCurSel(pageinfo->parm[sum].ToInt());
			sum++;
			break;
		case AUIOBJECT_TYPE_EDITBOX:
			ObjNode->obj->SetText(pageinfo->parm[sum]);
			sum++;
			break;
		case AUIOBJECT_TYPE_CHECKBOX:
			((PAUICHECKBOX)ObjNode->obj)->Check(pageinfo->parm[sum].ToInt()==1);
			sum++;
			break;
		case AUIOBJECT_TYPE_RADIOBUTTON:
			((PAUIRADIOBUTTON)ObjNode->obj)->Check(pageinfo->parm[sum].ToInt()==1);
			sum++;
			break;
		}

		ObjNode=ObjNode->next;
	}
}
//======================================HistoryPagesInfo===============================
HistoryPagesInfo::HistoryPagesInfo()
{
	head=new HistoryPageNode;
	head->next=NULL;
}

HistoryPagesInfo::~HistoryPagesInfo()
{
	HistoryPageNode* node;
	while(head!=NULL)
	{
		node=head;
		head=head->next;
		delete node;
	}
}

void HistoryPagesInfo::Release()
{
	HistoryPageNode* node;
	while(head->next!=NULL)
	{
		node=head->next;
		head->next=node->next;
		delete node;
	}
}

void HistoryPagesInfo::AddPageNode(AString target,int pageindex)
{
	HistoryPageNode* node=new HistoryPageNode;
	node->target=target;
	node->pageindex=pageindex;
	node->bLoaded=false;
	node->next=head->next;
	head->next=node;
}

void HistoryPagesInfo::SetRefreshAll()
{
	HistoryPageNode* node=head->next;
	while(node!=NULL)
	{
		node->bLoaded=false;
		node=node->next;
	}
}

HistoryPageNode* HistoryPagesInfo::GetPageNode(AString target)
{
	HistoryPageNode* node=head->next;
	while(node!=NULL)
	{
		if(node->target==target)
			return node;
		node=node->next;
	}
	return NULL;
}

//=====================================HistoryPagesManager====================================
HistoryPagesManager::HistoryPagesManager()
{
}

HistoryPagesManager::~HistoryPagesManager()
{
}

void HistoryPagesManager::Init()
{
	m_nNow=-1;
	m_nTotal=-1;
	m_nMax=100;
	m_Pages=new HistoryPagesInfo*[m_nMax];
}

void HistoryPagesManager::Release()
{
	for(int i=0;i<m_nTotal;i++)
		DELETEPOINTER(m_Pages[i]);
	delete []m_Pages;
}

void HistoryPagesManager::SaveNowPages(CECBBSPage *basepage)
{
	delete m_Pages[m_nNow];
	m_Pages[m_nNow]=new HistoryPagesInfo;
	SavePage(basepage);
}

void HistoryPagesManager::SavePage(CECBBSPage *page)
{
	if(page->m_iPageNumber!=-1)
		m_Pages[m_nNow]->AddPageNode(page->m_aName,page->m_iPageNumber);
	if(page->m_pChPageList!=NULL)
	{
		CECBBSPageNode *node=page->m_pChPageList->head->next;
		while(node!=NULL)
		{
			SavePage(node->page);
			node=node->next;
		}
	}
}

HistoryPagesInfo* HistoryPagesManager::GetHistoryPageInfo(int index)
{
	ASSERT(index>=0&&index<m_nTotal);
	return m_Pages[index];
}

void HistoryPagesManager::SetNowPage(int nPage,bool bTotalPage)
{
	if(nPage<0)
		return;
	if(nPage>=m_nTotal)
	{
		if(nPage>=m_nMax)
		{
			HistoryPagesInfo** oldPages=m_Pages;
			m_nMax=nPage+100;
			m_Pages=new HistoryPagesInfo*[m_nMax];
			int i;
			for(i=0;i<m_nTotal;i++)
				m_Pages[i]=oldPages[i];
			for(i=m_nTotal;i<m_nMax;i++)
				m_Pages[i]=NULL;
			delete []oldPages;
		}
		m_Pages[nPage]=new HistoryPagesInfo;
		m_nTotal=nPage;
	}
	else if(bTotalPage)
		m_Pages[nPage]->Release();
	m_nNow=nPage;
	m_Pages[nPage]->SetRefreshAll();
	if(bTotalPage)
		m_nTotal=nPage+1;
}

int HistoryPagesManager::GetNowPage()
{
	return m_nNow;
}

int HistoryPagesManager::GetTotalPages()
{
	return m_nTotal;
}


bool CECBBSPage::LoadTemplateFrame(PAUIOBJECT pObj)
{
	PAUIDIALOG pDlg = BBSDlg;
	PAUIDIALOG pTmplDlg = pDlg->GetAUIManager()->GetDialog("Tmpl_BBSPage");
	
	if(!pDlg || !pTmplDlg)
	{
		return AUI_ReportError( __LINE__, 1, 
								"CECBBSPage::LoadTemplateFrame(), failed to get template");
	}
	
	// iterate all object and load their style from template
	if(!pObj)
	{
		PAUIOBJECTLISTELEMENT pElement = pDlg->GetFirstControl();
		while(pElement)
		{
			if(pElement->pThis)
			{
				if(!LoadTemplateFrame(pElement->pThis))
				{
					return AUI_ReportError( __LINE__, 1, 
						"CECBBSPage::LoadTemplateFrame(), failed to load template %s", 
						pElement->pThis->GetName() );
				}
			}
			pElement = pElement->pNext;
		}
		
		return true;
	}
	
	// load template by object type
	float fWindowScale = pDlg->GetAUIManager()->GetWindowScale();
	
	PAUIOBJECT pTmpl = NULL;
	AUIOBJECT_SETPROPERTY p;
	#define AUI_LOAD_TMPL_PROPERTY(res)																\
	if(!pTmpl->GetProperty(res,&p)) {																\
		return AUI_ReportError( __LINE__, 1,														\
								"CECBBSPage::LoadTemplateFrame(), failed to get property %s on %s",	\
								res, pObj->GetName()); }											\
																									\
	if(!pObj->SetProperty(res,&p)) {																\
		return AUI_ReportError( __LINE__, 1,														\
								"CECBBSPage::LoadTemplateFrame(), failed to set property %s on %s",	\
								res, pObj->GetName()); }											\
	//
	ScopedAUIControlSpriteModify _dummy(pObj);
	switch(pObj->GetType())
	{
	case AUIOBJECT_TYPE_SCROLL:
		{
			pTmpl = pTmplDlg->GetDlgItem("Scl_Tmpl");
			if(!pTmpl) return false;
			
			pObj->SetSize(int(pTmpl->GetSize().cx * fWindowScale), pObj->GetSize().cy);
			AUI_LOAD_TMPL_PROPERTY("Scroll Area Image");
			AUI_LOAD_TMPL_PROPERTY("Scroll Bar Image");
			AUI_LOAD_TMPL_PROPERTY("Up Button Image");
			AUI_LOAD_TMPL_PROPERTY("Down Button Image");
			AUI_LOAD_TMPL_PROPERTY("Frame Mode");
		}
		break;

	case AUIOBJECT_TYPE_EDITBOX:
		{
			pTmpl = pTmplDlg->GetDlgItem("Edit_Tmpl");
			if(!pTmpl) return false;

			AUI_LOAD_TMPL_PROPERTY("Frame Image");
		}
		break;

	case AUIOBJECT_TYPE_STILLIMAGEBUTTON:
		{
			pTmpl = pTmplDlg->GetDlgItem("Btn_Tmpl");
			if(!pTmpl) return false;

			AUI_LOAD_TMPL_PROPERTY("Up Frame File");
			AUI_LOAD_TMPL_PROPERTY("Down Frame File");
			AUI_LOAD_TMPL_PROPERTY("OnHover Frame File");
		}
		break;

	case AUIOBJECT_TYPE_CHECKBOX:
		{
			pTmpl = pTmplDlg->GetDlgItem("Chk_Tmpl");
			if(!pTmpl) return false;

			AUI_LOAD_TMPL_PROPERTY("Normal Image File");
			AUI_LOAD_TMPL_PROPERTY("Checked Image File");
			AUI_LOAD_TMPL_PROPERTY("OnHover Image File");
		}
		break;
	case AUIOBJECT_TYPE_RADIOBUTTON:
		{
			pTmpl = pTmplDlg->GetDlgItem("Rdo_Tmpl");
			if(!pTmpl) return false;

			AUI_LOAD_TMPL_PROPERTY("Normal Image File");
			AUI_LOAD_TMPL_PROPERTY("Checked Image File");
			AUI_LOAD_TMPL_PROPERTY("OnHover Image File");
		}
		break;
	case AUIOBJECT_TYPE_COMBOBOX:
		{
			pTmpl = pTmplDlg->GetDlgItem("Combo_Tmpl");
			if(!pTmpl) return false;

			AUI_LOAD_TMPL_PROPERTY("Frame Image");
			AUI_LOAD_TMPL_PROPERTY("Hilight Image");
		}
		break;
	case AUIOBJECT_TYPE_LISTBOX:
		{
			pTmpl = pTmplDlg->GetDlgItem("Lst_Tmpl");
			if(!pTmpl) return false;

			AUI_LOAD_TMPL_PROPERTY("Frame Image");
			AUI_LOAD_TMPL_PROPERTY("Hilight Image");
			AUI_LOAD_TMPL_PROPERTY("Up Button Image");
			AUI_LOAD_TMPL_PROPERTY("Down Button Image");
			AUI_LOAD_TMPL_PROPERTY("Scroll Area Image");
			AUI_LOAD_TMPL_PROPERTY("Scroll Bar Image");
		}
		break;
	}

	
	return true;
}
