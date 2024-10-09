/*
 * FILE: EC_BBSFileDownloader.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_BBSFileDownloader.h"


const AString MultiPartBoundary="---------asdhqiuwe";
const AString szGetHeader = "User-Agent: Mozilla/4.0\r\n";
const AString szPostHeaders0 = "Referer: ";
const AString szPostHeaders1 = "Content-Type: application/x-www-form-urlencoded";
const AString szPostHeaders2 = "Content-Type: multipart/form-data; boundary="+MultiPartBoundary;
const char AcceptTypes[]= "*/*\0";

CECBBSFileDownloader::CECBBSFileDownloader()
{
	m_pPostData=NULL;
	m_hRequest=NULL;
	m_hHttpSession=NULL;
	m_hInternet=NULL;
	m_pPostData=NULL;
	m_iContantType = CT_APPLICATION;
}

CECBBSFileDownloader::~CECBBSFileDownloader()
{
	if(m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest=NULL;
	}
	if(m_hHttpSession)
	{
		InternetCloseHandle(m_hHttpSession);
		m_hHttpSession=NULL;
	}
	if(m_hInternet)
	{
		InternetCloseHandle(m_hInternet);
		m_hInternet=NULL;
	}
	if(m_pPostData)
	{
		delete []m_pPostData;
		m_pPostData=NULL;
	}
}

bool CECBBSFileDownloader::DownloadByUrl(LPCSTR url, LPCSTR localfn, LPCSTR postdata, int postlen, int contenttype)
{
	m_hInternet = InternetOpenA("GameBBS Application", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!m_hInternet)
		return false;
	m_aUrl = url;
	m_aLocalfn = localfn;
	if(postlen>0)
	{
		m_aMethod = "POST";
		m_pPostData = new char[postlen+1];
		m_nPostLength = postlen;
		m_iContantType = contenttype;
		memcpy(m_pPostData,postdata,postlen);
		m_pPostData[postlen]='\0';
	}
	else
		m_aMethod = "GET";
	return Run();
}

DWORD CECBBSFileDownloader::Connect()
{
	AString url=m_aUrl;
	if(url.Left(7)=="http://")
		url.CutLeft(7);
	int p=url.Find("/");
	if(p<=0)
		throw ERR_URL_PARSE;
	m_aServer=url.Left(p);
	m_aFileName=url.Right(url.GetLength()-p);

	m_hHttpSession = InternetConnectA(m_hInternet, m_aServer, 
		INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);	
	
	if(!m_hHttpSession)
		throw ERR_CONNECTION;

	m_hRequest = HttpOpenRequestA(m_hHttpSession, m_aMethod, m_aFileName,  "HTTP/1.1", 
									 NULL, NULL, SESSION_FLAG, 1);
	if(!m_hRequest)
		throw ERR_REQ_DENIED;
	if(m_aMethod=="GET")
		HttpSendRequestA(m_hRequest, szGetHeader, szGetHeader.GetLength(), NULL, 0);
	else
	{
		if(!HttpAddRequestHeadersA(m_hRequest, szPostHeaders0+m_aUrl, 
			szPostHeaders0.GetLength()+m_aUrl.GetLength(), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
			throw ERR_REQ_DENIED;
		if(m_iContantType==CT_APPLICATION)
		{
			if(!HttpSendRequestA(m_hRequest, szPostHeaders1, szPostHeaders1.GetLength(), (LPVOID)m_pPostData, m_nPostLength))
				throw ERR_REQ_DENIED;
		}
		else
		{
			if(!HttpSendRequestA(m_hRequest, szPostHeaders2, szPostHeaders2.GetLength(), (LPVOID)m_pPostData, m_nPostLength))
				throw ERR_REQ_DENIED;
		}
	}

	char buf[3000];
	DWORD len=3000;
	DWORD index=0;
	if(!HttpQueryInfoA(m_hRequest,HTTP_QUERY_STATUS_CODE ,(LPVOID)&buf,&len,&index))
		throw ERR_QUARYINFO;

	AString ret=buf;
	return ret.ToInt();
}

DWORD CECBBSFileDownloader::Redirect()
{
	char buf[300];
	DWORD len=300;
	DWORD index=0;
	HttpQueryInfoA(m_hRequest,HTTP_QUERY_RAW_HEADERS_CRLF,&buf,&len,&index);

	char *p;
	p=strstr(buf,"Location: ");
	if(!p)
		throw ERR_GET_ADDR;

	strcpy(buf,p+10);
	p=strchr(buf,'\n');
	if(p)
		buf[p-buf]='\0';

	p=strstr(buf,"http://");
	if(!p)
	{
		int i;
		if(buf[0]=='/')
		{
			i=7;
			while(m_aUrl[i+1]!='/')
				i++;
		}
		else
		{
			i=m_aUrl.GetLength()-1;
			while(i>0&&m_aUrl[i]!='/')
				i--;
		}
		m_aUrl=m_aUrl.Left(i+1)+buf;
	}
	else
		m_aUrl=p;

	InternetCloseHandle(m_hRequest);
	InternetCloseHandle(m_hHttpSession);
	m_hRequest=NULL;
	m_hHttpSession=NULL;
	return Connect();
}

void CECBBSFileDownloader::Read()
{
	FILE* fp = fopen(m_aLocalfn, "wb");
	if(!fp)
		throw ERR_CREATE_FILE;
	char sz[4096];
	DWORD nRead = 1;
	while (nRead > 0) 
	{
		InternetReadFile(m_hRequest,sz,4096,&nRead);
		fwrite(sz, 1, nRead, fp);
	}
	fclose(fp);
}

bool CECBBSFileDownloader::Run()
{
	bool bRet=true;;
	DWORD dwRet;
	try{
		dwRet=Connect();
		if(	dwRet == HTTP_STATUS_MOVED ||
			dwRet == HTTP_STATUS_REDIRECT ||
			dwRet == HTTP_STATUS_REDIRECT_METHOD)
		{
			dwRet = Redirect();
		}
		if (dwRet > HTTP_STATUS_PARTIAL_CONTENT && m_aLocalfn.Right(4)==".img")
			throw ERR_CONNECTION;
		Read();
	}
	catch (...)
	{
		DeleteFileA(m_aLocalfn);
		bRet=false;
	}
	if(m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest=NULL;
	}
	if(m_hHttpSession)
	{
		InternetCloseHandle(m_hHttpSession);
		m_hHttpSession=NULL;
	}
	if(m_hInternet)
	{
		InternetCloseHandle(m_hInternet);
		m_hInternet=NULL;
	}
	if(m_pPostData)
	{
		delete []m_pPostData;
		m_pPostData=NULL;
	}
	return bRet;
}

