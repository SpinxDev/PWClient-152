/*
 * FILE: EC_BBSPageFile.cpp
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
#include "EC_BBSFileDownloader.h"
#include "AIniFile.h"
#include <process.h>

//#define _WRITELOG

ImageList ImgList;
bool bLoadNewImage=false;
unsigned int EndThreadId;
CRITICAL_SECTION ImageSection;

bool Char2Wchar(AString txt,int CodePage,ACHAR *uCode)
{
	MultiByteToWideChar(CodePage,0,txt,-1,uCode,400);
	return true;
}

void __cdecl DownloadImage(LPVOID pParam)
{
	ImageInfo *node=(ImageInfo*)pParam;
	if(node!=NULL)
	{
		CECBBSFileDownloader downloader;
		downloader.DownloadByUrl(node->url,node->fn);
		EnterCriticalSection(&ImageSection);
		if(node->state==IMG_RELEASED)
		{
			delete node;
			LeaveCriticalSection(&ImageSection);
			return;
		}
		node->state=IMG_DOWNLOADED;
		LeaveCriticalSection(&ImageSection);
		bLoadNewImage=true;
	}
}

AString TransFormatText(AString src)
{
	int i=0;
	char pp[300];
	int p=0;
	AString res="";
	while(src[i]!='\0')
	{
		char a;
		a=src[i];
		if(a=='&')
		{
			i++;
			int k=i;
			if(src[i]=='#')
			{
				i++;
				AString st="";
				while(src[i]!='\0'&&src[i]!=';')
				{
					if(src[i]>='0'&&src[i]<='9')
						st+=src[i];
					i++;
				}
				if(src[i]!='\0')
					a=(char)st.ToInt();
				else
					i=k-1;
			}
			else
			{
				AString st="";
				while(src[i]!='\0'&&src[i]!=';')
				{
					st+=src[i];
					i++;
				}
				if(src[i]!='\0')
				{
					if(st=="nbsp")
						a=' ';
					else if(st=="lt")
						a='<';
					else if(st=="gt")
						a='>';
					else if(st=="amp")
						a='&';
					else if(st=="quot")
						a='"';
					else
						i=k-1;
				}
				else
					i=k-1;
			}
			if(i==k-1)
				a=src[i];
		}
		pp[p++]=a;
		if(p==299)
		{
			pp[p]='\0';
			res+=pp;
			p=0;
		}
		i++;
	}
	pp[p]='\0';
	res+=pp;
	return res;
}

HTMLLabelAttribute::HTMLLabelAttribute()
{
	Head=new AttributeNode;
	Head->next=NULL;
	Head->data=NULL;
}

HTMLLabelAttribute::~HTMLLabelAttribute()
{
	Release();
	if(Head!=NULL)
		delete Head;
}

void HTMLLabelAttribute::Release()
{
	AttributeNode *Node=Head;
	while(Head->next!=NULL)
	{
		Node=Head->next;
		Head->next=Node->next;
		delete Node->data;
		delete Node;
	}
}

void HTMLLabelAttribute::AddAttribute(Attribute *attrib)
{
	AttributeNode *AttribNode=new AttributeNode;
	for(int i=0;i<attrib->cName.GetLength();i++)
		if(attrib->cName[i]>='A'&&attrib->cName[i]<='Z')
			attrib->cName[i]+=32;
//	for(i=0;i<attrib->cValue.GetLength();i++)
//		if(attrib->cValue[i]>='A'&&attrib->cValue[i]<='Z')
//			attrib->cValue[i]+=32;
	if(attrib->cName!="href"&&attrib->cName!="ahref"&&attrib->cName!="text")
		attrib->cValue=TransFormatText(attrib->cValue);
	if(attrib->cName=="face")
		attrib->cValue="方正细黑一简体";
	AttribNode->data=attrib;
	AttribNode->next=Head->next;
	Head->next=AttribNode;
}

void HTMLLabelAttribute::AddAttribute(AString cName,AString cValue)
{
	Attribute *attrib=new Attribute;
	attrib->cName=cName;
	attrib->cValue=cValue;
	AddAttribute(attrib);
}

AString* HTMLLabelAttribute::GetAttribute(AString cName)
{
	AttributeNode *Node=Head;
	while((Node=Node->next)!=NULL)
		if(Node->data->cName==cName)
			return &(Node->data->cValue);
	return NULL;
}

bool HTMLLabelAttribute::SetAttribute(AString cName,AString cValue)
{
	AttributeNode *Node=Head;
	while((Node=Node->next)!=NULL)
		if(Node->data->cName==cName)
		{
			Node->data->cValue=cValue;
			return true;
		}
	AddAttribute(cName,cValue);
	return true;
}

void ReleaseLabel(HTMLLabel *hLabel)
{
	if(hLabel==NULL)
		return;
	HTMLLabel *h=hLabel;
	while(h!=NULL)
	{
		hLabel=h;
		h=h->NextLabel;
		ReleaseLabel(hLabel->FirstChildLabel);
		delete hLabel->Attrib;
		delete hLabel;
	}
}

CECBBSPageFile::CECBBSPageFile()
{
	StyleLabel=NULL;
}

CECBBSPageFile::~CECBBSPageFile()
{
	if(StyleLabel!=NULL)
		ReleaseLabel(StyleLabel);
}

HTMLLabel* CECBBSPageFile::LoadFile(AString lFileName,AString url)
{
	bStop=false;
	HANDLE hFile;
	hFile = CreateFileA(lFileName,GENERIC_READ,FILE_SHARE_READ,NULL, 
                     OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return false;
	DWORD iSize=GetFileSize(hFile,NULL);
	if(iSize<=0)
	{
		CloseHandle(hFile);
		return NULL;
	}
	fbuf=(char*)a_malloctemp(iSize+1);
	ReadFile(hFile,fbuf,iSize,&iSize,NULL);
	CloseHandle(hFile);
	if(iSize>=2&&fbuf[0]==(char)0xff&&fbuf[1]==(char)0xfe)
	{
		char *oldbuf=fbuf;
		int len=WideCharToMultiByte(936,0,((ACHAR*)oldbuf)+1,iSize/2,NULL,0,NULL,NULL);
		fbuf=(char*)a_malloctemp(len+1);
		WideCharToMultiByte(936,0,((ACHAR*)oldbuf)+1,iSize/2,(char*)fbuf,len+1,NULL,NULL);
		a_freetemp(oldbuf);
		iSize=len;
	}
	else if(iSize>=3&&fbuf[0]==(char)0xef&&fbuf[1]==(char)0xbb&&fbuf[2]==(char)0xbf)
	{
		AIniFile iniFile;
		int n;
		if(iniFile.Open("configs\\loginpage.ini"))
		{
			n = iniFile.GetValueAsInt("URL", "CodePage", -1);
			iniFile.Close();
		}
		int codepage=936;
		if( n != -1 )
			codepage=n;
		char *oldbuf=fbuf;
		ACHAR *abuf;
		int mlen=MultiByteToWideChar(CP_UTF8,0,oldbuf+3,iSize-3,NULL,0);
		abuf=(ACHAR*)a_malloctemp(sizeof(ACHAR)*(mlen+1));
		MultiByteToWideChar(CP_UTF8,0,oldbuf+3,iSize-3,abuf,mlen+1);
		a_freetemp(oldbuf);
		int len=WideCharToMultiByte(codepage,0,abuf,mlen,NULL,0,NULL,NULL);
		fbuf=(char*)a_malloctemp(len+1);
		WideCharToMultiByte(codepage,0,abuf,mlen,(char*)fbuf,len+1,NULL,NULL);
		a_freetemp(abuf);
		iSize=len;
	}
	ch=fbuf;
	fend=fbuf+iSize;
	ImgNodeList.DeleteAllNode();
	aUrl=url;
	HTMLLabel *RootLabel;
	RootLabel=new HTMLLabel;
	RootLabel->FirstChildLabel=NULL;
	RootLabel->NextLabel=NULL;
	RootLabel->Attrib=NULL;
	RootLabel->ParentLabel=NULL;
	RootLabel->LabelType="root";
	RootLabel->Attrib=new HTMLLabelAttribute;
	TempLabel=NULL;
	hEndLabel=NULL;
	if(StyleLabel!=NULL)
		ReleaseLabel(StyleLabel);
	StyleLabel=new HTMLLabel;
	StyleLabel->FirstChildLabel=NULL;
	StyleLabel->NextLabel=NULL;
	StyleLabel->Attrib=NULL;
	StyleLabel->ParentLabel=NULL;
	iLine=1;
	iLinePos=1;
#ifdef _WRITELOG
	if(lFileName!="bbsfiles\\error.htm")
		AppendLog("Start reading html file.",false);
	else
		AppendLog("Start reading html file.");
#endif
	bool b;
	do
	{
		b=LoadLabel(RootLabel);
		if(bStop)
		{
			a_freetemp(fbuf);
			ReleaseLabel(RootLabel);
			return NULL;
		}
		if(RootLabel->FirstChildLabel!=NULL&&RootLabel->FirstChildLabel->LabelType=="html")
			break;
		else
		{
			ReleaseLabel(RootLabel->FirstChildLabel);
			RootLabel->FirstChildLabel=NULL;
		}
	}while(b);
#ifdef _WRITELOG
	if(b)
		AppendLog("Finish reading html file.");
	else
		AppendLog("Html Reading terminated.");
#endif
	a_freetemp(fbuf);
	if(RootLabel->FirstChildLabel!=NULL)
		return RootLabel;
	else
	{
		ReleaseLabel(RootLabel);
		return NULL;
	}
}

void CalAttributesValue(char *str,HTMLLabel *label)
{
	AString name;
	AString value;
	char *p;
	char *attribute;
	do
	{
		while(*str==' '&&*str!='\0')
			str++;
		p=strchr(str,';');
		if(p!=NULL)
		{
			attribute=(char*)a_malloctemp(p-str+1);
			strncpy(attribute,str,p-str);
			attribute[p-str]='\0';
			str=p+1;
			p=strchr(attribute,':');
			if(p!=NULL)
			{
				AString name="";
				for(p=attribute;*p!=' '&&*p!=':';p++)
					name+=*p;
				if(name=="font"||name=="font-size")
				{
					name="size";
					value="";
					p=strstr(p,"px");
					if(p!=NULL)
					{
						p--;
						while(*p>='0'&&*p<='9')
						{
							value=*p+value;
							p--;
						}
					}
					value+="p";
					label->Attrib->AddAttribute(name,value);
				}
				else if(name=="color")
				{
					name="color";
					while((*p==' '||*p==':')&&*p!='\0')
						p++;
					while(*p!=' '&&*p!=';'&&*p!='\0')
					{
						value+=*p;
						p++;
					}
					label->Attrib->AddAttribute(name,value);
				}
			}
			a_freetemp(attribute);
		}
	}while(p!=NULL);
}

char* CECBBSPageFile::FindEndLabel(char *labelname)
{
	char *p,*p1,*p2;
	char desStr1[10];
	char desStr2[10];
	int nLen=strlen(labelname);
	p1=desStr1;
	p2=desStr2;
	for(p=labelname;*p!='\0';p++)
	{
		if(*p>='A'&&*p<='Z')
		{
			*p1++=*p;
			*p2++=*p+32;
		}
		else
		{
			*p2++=*p;
			*p1++=*p-32;
		}
	}
	*p1='\0';
	*p2='\0';
	p=ch;
	do
	{
		while(*p!='/'&&p!=fend)
			p++;
		if(p!=fend)
		{
			p1=desStr1;
			p2=desStr2;
			p++;
			int i(0);
			for(i=0;i<nLen;i++)
			{
				if(*p==*p1||*p==*p2)
				{
					p1++;
					p2++;
					p++;
				}
				else
					break;
			}
			if(i==nLen)
			{
				while(p!=fbuf&&*p!='<')
					p--;
				if(p>=fbuf)
					return p;
			}
		}
	}while(p!=fend);
	return NULL;
}

void CECBBSPageFile::GetStyle()
{
	char *styleEnd=FindEndLabel("style");
	if(styleEnd!=NULL)
	{
		char *style=(char*)a_malloctemp(styleEnd-ch+1);
		char *p=style;
		char *chStart;
		char *chEnd;

		for(chStart=ch;chStart!=styleEnd;chStart++)
		{
			if(*chStart!=' '&&*chStart!='\r'&&*chStart!='\n'&&*chStart!='\t')
			{
				*p=*chStart;
				if(*p<='Z'&&*p>='A')
					*p+=32;
				p++;
			}
			else
				*p++=' ';
		}
		*p='\0';
		chStart=style;
		ch=styleEnd;
		chEnd=strchr(style,'{');
		HTMLLabel *chLabel;
		while(chEnd!=NULL)
		{
			chLabel=new HTMLLabel;
			chLabel->Attrib=new HTMLLabelAttribute;
			chLabel->LabelType="";
			chLabel->ParentLabel=NULL;
			chLabel->FirstChildLabel=NULL;
			if(StyleLabel!=NULL)
			{
				chLabel->NextLabel=StyleLabel->NextLabel;
				StyleLabel->NextLabel=chLabel;
			}
			else
			{
				StyleLabel=chLabel;
				chLabel->NextLabel=NULL;
			}
			while(*chStart!='.'&&(*chStart<'a'||*chStart>'z'))
				chStart++;
			p=chEnd;
			while(*(p-1)==' ')
				p--;
			while(chStart!=p)
			{
				chLabel->LabelType+=*chStart;
				chStart++;
			}
			char *attributes;
			chStart=chEnd+1;
			chEnd=strchr(chEnd,'}');
			if(chEnd==NULL)
				break;
			else
			{
				attributes=(char*)a_malloctemp(chEnd-chStart+2);
				strncpy(attributes,chStart,chEnd-chStart);
				attributes[chEnd-chStart]=';';
				attributes[chEnd-chStart+1]='\0';
				CalAttributesValue(attributes,chLabel);
				a_freetemp(attributes);
				chStart=chEnd+1;
				chEnd=strchr(chStart,'{');
			}
		}
		
		a_freetemp(style);
	}
}

bool CECBBSPageFile::LoadLabel(HTMLLabel *ParentLabel)
{
	if(bStop)
		return true;
	AString aStr;
	HTMLLabel *BrotherLabel=NULL;
	do
	{
		HTMLLabel *Label;
		if(TempLabel!=NULL)
		{
			Label=TempLabel;
			TempLabel=NULL;
		}
		else
		{
			Label=new HTMLLabel;
			Label->Attrib=new HTMLLabelAttribute;
			Label->ParentLabel=NULL;
			if(!GetLabel(Label))
			{
				delete Label->Attrib;
				delete Label;
#ifdef _WRITELOG
				char st[100];
				sprintf(st,"Warning: Error file ending, lost label </%s>",ParentLabel->LabelType);
				AppendLog(st);
#endif
				return false;
			}
		}
//		if(Label->LabelType=="td"&&Label->ParentLabel!="tr")
//		{
//			HTMLLabel *LabelTR=new HTMLLabel;
//			LabelTR->Attrib=new HTMLLabelAttribute;
//			LabelTR->LabelType="tr";
//			ParentLabel
//			return true;
//		}
//		if(ParentLabel->LabelType=="table"&&Label->LabelType=="table")
//		{
//			Label->FirstChildLabel=NULL;
//			Label->NextLabel=NULL;
//			Label->ParentLabel=ParentLabel->ParentLabel;
//			ParentLabel->NextLabel=Label;
//			ParentLabel=Label;
//			if(!LoadLabel(Label))
//				return false;
//			break;
//		}
		if((ParentLabel->LabelType=="td"||ParentLabel->LabelType=="th")&&
			(Label->LabelType=="td"||Label->LabelType=="th"||Label->LabelType=="tr")
			||ParentLabel->LabelType=="table"&&Label->LabelType=="table"
			)
		{
			Label->FirstChildLabel=NULL;
			Label->NextLabel=NULL;
			Label->ParentLabel=ParentLabel;
//			if(ParentLabel->FirstChildLabel==NULL)
//				ParentLabel->FirstChildLabel=Label;
//			else
//				BrotherLabel->NextLabel=Label;
//			if(!LoadLabel(Label))
//				return false;
//			if(hEndLabel!=NULL)
//				if(hEndLabel!=ParentLabel)
//					break;
//				else
//				{
//					hEndLabel=NULL;
//					break;
//				}
//			else
			{
				HTMLLabel *hLabel=ParentLabel;
				while(hLabel!=NULL&&hLabel->LabelType!=Label->LabelType)
					hLabel=hLabel->ParentLabel;
				if(hLabel!=NULL)
				{
					hEndLabel=hLabel->ParentLabel;
					TempLabel=Label;
					return true;
				}
			}
			break;
		}
		if(Label->LabelType=="/input"||Label->LabelType=="/option"||
			Label->LabelType.Left(1)=="!")
		{
			delete Label->Attrib;
			delete Label;
			continue;
		}
		if(Label->LabelType[0]=='/')
			if(Label->LabelType=="/"+ParentLabel->LabelType)
			{
				delete Label->Attrib;
				delete Label;
				break;
			}
			else
			{
#ifdef _WRITELOG
				char st[100];
				sprintf(st,"Warning: Incompatible label <%s>",Label->LabelType);
				AppendLog(st);
#endif
				HTMLLabel *hLabel=ParentLabel;
				while(hLabel!=NULL&&"/"+hLabel->LabelType!=Label->LabelType)
					hLabel=hLabel->ParentLabel;
				if(hLabel!=NULL&&Label->LabelType!="/font")
				{
					hEndLabel=hLabel->ParentLabel;
					delete Label->Attrib;
					delete Label;
					return true;
				}
				else
				{
					delete Label->Attrib;
					delete Label;
					continue;
				}
			}
		Label->FirstChildLabel=NULL;
		Label->NextLabel=NULL;
		Label->ParentLabel=ParentLabel;
		if(ParentLabel->FirstChildLabel==NULL)
			ParentLabel->FirstChildLabel=Label;
		else
			BrotherLabel->NextLabel=Label;
		BrotherLabel=Label;
		if(Label->LabelType=="script")
		{
			char *p=FindEndLabel("script");
			if(p!=NULL)
				ch=p;
		}
		if(Label->LabelType=="style")
			GetStyle();
		else 
			if(	Label->LabelType!="text"&&
				Label->LabelType!="br"&&
				Label->LabelType!="img"&&
				Label->LabelType!="frame"&&
				Label->LabelType!="input"&&
				Label->LabelType!="option"&&
				Label->LabelType!="meta"&&
				Label->LabelType!="link"&&
				Label->LabelType!="hr")
		{
			if(!LoadLabel(Label))
				return false;
//			if(ParentLabel->LabelType=="table")
//				hEndLabel=NULL;
			if(hEndLabel!=NULL)
				if(hEndLabel!=ParentLabel)
					break;
				else
				{
					hEndLabel=NULL;
				}
		}
		if(ParentLabel->LabelType=="root")
			break;
	}while(true);
	return true;
}

bool CECBBSPageFile::GetLabel(HTMLLabel *hLabel)
{
	AString aStr,tempStr;
	hLabel->pos=ch-fbuf;
	int iRet=ReadString(aStr);
	Attribute *attrib;
	int i;
	switch(iRet)
	{
	case 0:
		return false;
	case 1:
		hLabel->LabelType="text";
		attrib=new Attribute;
		attrib->cName="text";
		attrib->cValue=aStr;

		hLabel->Attrib->AddAttribute(attrib);
		break;
	case 2:
		if(aStr=="!--")
			return GetLabel(hLabel);
		
		aStr+=" ";
		i=aStr.Find(' ');
		hLabel->LabelType=aStr.Left(i);
		while(i<aStr.GetLength()&&aStr[i]==' ')
			i++;
		aStr.CutLeft(i);
		while(aStr.GetLength()>0)
		{
			i=aStr.Find('=');
			int j=aStr.Find(' ');
			attrib=new Attribute;
			if(i==-1||j<i)
			{
				i=j;
				attrib->cName=aStr.Left(i);
				attrib->cValue="";
				aStr.CutLeft(i+1);
				i=0;
				while(i<aStr.GetLength()&&
					(aStr[i]==' '||aStr[i]=='\t'||aStr[i]=='\r'||aStr[i]=='\n'))
					i++;
				aStr.CutLeft(i);
			}
			else
			{
				attrib->cName=aStr.Left(i);
				aStr.CutLeft(i+1);
				int i=0;
				while(i<aStr.GetLength()&&
					(aStr[i]==' '||aStr[i]=='\t'||aStr[i]=='\r'||aStr[i]=='\n'))
					i++;
				aStr.CutLeft(i);
				if(aStr[0]=='"')
				{
					i=aStr.Find('"',1);
					attrib->cValue=aStr.Mid(1,i-1);
				}
				else if(aStr[0]=='\'')
				{
					i=aStr.Find('\'',1);
					attrib->cValue=aStr.Mid(1,i-1);
				}
				else
				{
					i=aStr.Find(' ');
					if(i!=-1)
						attrib->cValue=aStr.Left(i);
					else
						attrib->cValue="";
				}
				aStr.CutLeft(i+1);
			}
			hLabel->Attrib->AddAttribute(attrib);
			i=0;
			while(i<aStr.GetLength()&&
				(aStr[i]==' '||aStr[i]=='\t'||aStr[i]=='\r'||aStr[i]=='\n'))
				i++;
			aStr.CutLeft(i);
		}
		break;
	}
	if(hLabel!=NULL)
	{
		for(i=0;i<hLabel->LabelType.GetLength();i++)
			if(hLabel->LabelType[i]>='A'&&hLabel->LabelType[i]<='Z')
				hLabel->LabelType[i]+=32;
		if(hLabel->LabelType=="img"&&hLabel->Attrib->GetAttribute("src")!=NULL)
		{
			AString url=*hLabel->Attrib->GetAttribute("src");
			if(	url.GetLength()<=3||url.Right(4)==".gif"||
				aUrl.GetLength()<7||aUrl.Left(7)!="http://")
				hLabel->Attrib->SetAttribute("src"," ");
			else
			{
				int i=aUrl.GetLength()-1;
				if(url[0]=='/')
				{
					i=7;
					while(i<aUrl.GetLength()&&aUrl[i+1]!='/')
						i++;
				}
				else
					while(aUrl[i]!='/')
						i--;
				url=aUrl.Left(i+1)+url;
				hLabel->Attrib->SetAttribute("src",url);
				int index=ImgList.GetImage(url);
				if(index==0)
				{
					index=ImgList.AddImage(url);
					_beginthread(DownloadImage,0,ImgList.GetImageInfo(index));
				}
				char a[20];
				sprintf(a,"%d",index);
				hLabel->Attrib->SetAttribute("imgindex",a);
				if(hLabel->Attrib->GetAttribute("width")==NULL||hLabel->Attrib->GetAttribute("height")==NULL)
					ImgNodeList.AddImageNode(ImgList.GetImageInfo(index));
			}
		}
	}
	return true;
}

inline bool CECBBSPageFile::GetChar(char &resChar)
{
	if(ch==fend)
	{
#ifdef _WRITELOG
		char st[100];
		sprintf(st,"Finish reading char at (%d,%d)",iLine,iLinePos);
		AppendLog(st);
#endif
		return false;
	}
	resChar=*ch;
	ch++;
#ifdef _WRITELOG
	iLinePos++;
	if(resChar=='\n')
	{
		iLine++;
		iLinePos=1;
	}
#endif
	return true;
}

int CECBBSPageFile::ReadString(AString &aStr)
{
#ifdef _WRITELOG
	char st[100];
#endif
	char c=' ';
	aStr="";
	char pp[300];
	int p=0;
	bool b;
	while((b=GetChar(c))&&(c==' '||c=='\n'||c=='\r'||c=='\t'))
	{
		pp[p++]=c;
		if(p==299)
		{
			pp[p]='\0';
			aStr+=pp;
			p=0;
		}
	}
	pp[p]='\0';
	aStr+=pp;
	if(!b)
		return 0;
	char quot1=1;
	char quot2=1;
	if(c!='<')
	{
		ch--;
#ifdef _WRITELOG
		iLinePos--;
#endif
		while((b=GetChar(c))&&(c!='<'))
		{
//			if(c=='"'&&quot2!=-1)
//			{
//				quot1=-quot1;
//				quot2=1;
//			}
//			if(c=='\''&&quot1!=-1)
//			{
//				quot2=-quot2;
//				quot1=1;
//			}
			pp[p++]=c;
			if(p==299)
			{
				pp[p]='\0';
				aStr+=pp;
				p=0;
			}
		}
		pp[p]='\0';
		aStr+=pp;
		if(!b)
			return 0;
		ch--;
#ifdef _WRITELOG
		iLinePos--;
		sprintf(st,"Finish reading char at (%d,%d)",iLine,iLinePos);
		AppendLog(st);
#endif
		return 1;
	}
	else
	{
		aStr="";
//		while((b=GetChar(c,(quot1==1)&&(quot2==1)))&&(b==2||c!='>'))
		p=0;
		while((b=GetChar(c))&&(quot1!=1||quot2!=1||c!='>'))
		{
			if(c=='"'&&quot2!=-1)
			{
				quot1=-quot1;
				quot2=1;
			}
			if(c=='\''&&quot1!=-1)
			{
				quot2=-quot2;
				quot1=1;
			}
			pp[p++]=c;
			if(p==299)
			{
				pp[p]='\0';
				aStr+=pp;
				p=0;
			}
			if(p==3&&pp[0]=='!'&&pp[1]=='-'&&pp[2]=='-')
			{
				aStr="";
				int i=1;
				while(b=GetChar(c))
				{
					aStr+=c;
					if(aStr.GetLength()>4)
						aStr=aStr.Right(4);
					if(aStr=="<!--")
						i++;
					else if(aStr.Right(3)=="-->")
					{
						i--;
						if(i==0)
							break;
					}
				}
				if(!b)
					return 0;
				aStr="!--";
				break;
			}
		}
		pp[p]='\0';
		aStr+=pp;
		if(!b)
			return 0;
#ifdef _WRITELOG
		sprintf(st,"Finish reading char at (%d,%d)",iLine,iLinePos);
		AppendLog(st);
#endif
		return 2;
	}
}

void CECBBSPageFile::AppendLog(const char *log,bool b)
{
	FILE *f;
	if(b)
	{
		if((f=fopen(fnLog,"a"))!=NULL)
		{
			fprintf(f,log);
			fprintf(f,"\n");
			fclose(f);
		}
	}
	else
		if((f=fopen(fnLog,"w"))!=NULL)
		{
			fprintf(f,log);
			fprintf(f,"\n");
			fclose(f);
		}
}

//===================================ImageList=======================================
ImageList::ImageList()
{
}

ImageList::~ImageList()
{
}

void ImageList::Init()
{
	m_nMax=100;
	m_Images=new ImageInfo*[m_nMax];
	m_nTotal=1;
	m_Images[0]=new ImageInfo;
	m_Images[0]->url=" ";
	m_Images[0]->fn="InBBS/error.bmp";
	m_Images[0]->state=IMG_DOWNLOADED;
}

void ImageList::Release()
{
	EnterCriticalSection(&ImageSection);
	for(int i=0;i<m_nTotal;i++)
		if(m_Images[i]->state!=IMG_DOWNLOADING)
			delete m_Images[i];
		else
			m_Images[i]->state=IMG_RELEASED;
	delete []m_Images;
	m_Images=NULL;
	LeaveCriticalSection(&ImageSection);
}

int ImageList::AddImage(AString url)
{
	EnterCriticalSection(&ImageSection);
	if(m_nTotal>=m_nMax)
	{
		ImageInfo** oldImages=m_Images;
		m_nMax+=100;
		m_Images=new ImageInfo*[m_nMax];
		for(int i=0;i<m_nTotal;i++)
			m_Images[i]=oldImages[i];
		delete []oldImages;
	}
	m_Images[m_nTotal]=new ImageInfo;
	m_Images[m_nTotal]->url=url;
	m_Images[m_nTotal]->fn.Format("BBSPages/temp%d.img",m_nTotal);
	m_Images[m_nTotal]->state=IMG_DOWNLOADING;
	m_nTotal++;
	LeaveCriticalSection(&ImageSection);
	return m_nTotal-1;
}

int ImageList::GetImage(AString url)
{
	EnterCriticalSection(&ImageSection);
	for(int i=0;i<m_nTotal;i++)
		if(m_Images[i]->url==url)
		{
			LeaveCriticalSection(&ImageSection);
			return i;
		}
	LeaveCriticalSection(&ImageSection);
	return 0;
}

ImageInfo* ImageList::GetImageInfo(int index)
{
	ASSERT(index>=0&&index<m_nTotal);
	return m_Images[index];
}

//===================================FileImageList=======================================
FileImageList::FileImageList()
{
	head=new FileImageNode;
	head->next=NULL;
	head->node=NULL;
}

FileImageList::~FileImageList()
{
	Release();
	if(head!=NULL)
		delete head;
}

void FileImageList::Release()
{
	FileImageNode *node=head;
	while(head->next!=NULL)
	{
		node=head;
		head=head->next;
		delete node;
	}
}

void FileImageList::DeleteAllNode()
{
	FileImageNode *node;
	while(head->next!=NULL)
	{
		node=head->next;
		head->next=node->next;
		delete node;
	}
}

void FileImageList::AddImageNode(ImageInfo *node)
{
	FileImageNode *fnode=new FileImageNode;
	fnode->node=node;
	fnode->next=head->next;
	head->next=fnode;
}

bool FileImageList::IsFinishLoadingImg()
{
	FileImageNode *node=head;
	bool b=true;
	while(node->next!=NULL)
		if(node->next->node->state==IMG_DOWNLOADED)
		{
			FileImageNode *p=node->next;
			node->next=p->next;
			delete p;
		}
		else
		{
			node=node->next;
			b=false;
		}
	return b;
}